# 20 - 未定义行为 (Undefined Behavior)

## 简介

未定义行为（Undefined Behavior, UB）是 C++ 中最危险的陷阱之一。理解和避免 UB 对于编写可靠的代码至关重要。

**核心概念**：
- **未定义行为**：C++ 标准未规定的行为
- **未指定行为**：标准允许多种实现
- **实现定义行为**：由编译器文档定义
- **常见陷阱**：空指针解引用、数组越界、有符号溢出等

---

## 1. 未定义行为基础

### 1.1 三类行为？

| 行为类型 | 定义 | 示例 |
|---------|------|------|
| **未定义行为（UB）** | 标准完全未规定，任何事情都可能发生 | 空指针解引用、数组越界 |
| **未指定行为** | 标准允许多种合法结果 | 函数参数求值顺序 |
| **实现定义行为** | 由编译器文档规定 | `sizeof(int)` 的值 |

### 1.2 为什么存在 UB？

**编译器优化**：
```cpp
// UB 允许编译器假设 ptr 非空
void func(int* ptr) {
    int x = *ptr;  // 如果 ptr 为空 → UB
    if (ptr == nullptr) {  // ❌ 编译器可能优化掉这个检查
        return;
    }
}
```

**性能**：
```cpp
// 无符号整数溢出是定义的（模运算）
unsigned int x = UINT_MAX;
x = x + 1;  // ✅ 定义为 0

// 有符号整数溢出是未定义的
int y = INT_MAX;
y = y + 1;  // ❌ UB，编译器可以假设不会发生
```

---

## 2. 常见未定义行为

### 2.1 内存访问错误？

**1. 空指针解引用**：
```cpp
int* ptr = nullptr;
*ptr = 10;  // ❌ UB：空指针解引用
```

**2. 数组越界**：
```cpp
int arr[10];
arr[10] = 42;  // ❌ UB：越界访问
arr[-1] = 42;  // ❌ UB：负索引
```

**3. 访问已释放的内存**：
```cpp
int* ptr = new int(10);
delete ptr;
int x = *ptr;  // ❌ UB：use-after-free
```

**4. 访问未初始化的变量**：
```cpp
int x;
int y = x + 1;  // ❌ UB：x 未初始化
```

**5. 野指针**：
```cpp
int* get_ptr() {
    int local = 42;
    return &local;  // ❌ 返回局部变量地址
}

int* ptr = get_ptr();
*ptr = 10;  // ❌ UB：访问已销毁的对象
```

### 2.2 整数操作错误？

**1. 有符号整数溢出**：
```cpp
int x = INT_MAX;
x = x + 1;  // ❌ UB：有符号溢出

int y = INT_MIN;
y = y - 1;  // ❌ UB：有符号下溢

int z = INT_MIN;
z = -z;  // ❌ UB：-INT_MIN 溢出
```

**2. 除以零**：
```cpp
int x = 10 / 0;  // ❌ UB：除以零

int y = 10 % 0;  // ❌ UB：模运算除以零
```

**3. 非法位移**：
```cpp
int x = 1 << 32;   // ❌ UB：位移量 >= 类型位数

int y = 1 << -1;   // ❌ UB：负位移量

int z = -1 << 1;   // ❌ UB：左移负数
```

### 2.3 对象生命周期错误？

**1. 多次析构**：
```cpp
std::string str = "hello";
str.~string();  // 手动析构
// } - ❌ UB：再次自动析构
```

**2. 访问未构造的对象**：
```cpp
alignas(std::string) char buffer[sizeof(std::string)];
std::string* ptr = reinterpret_cast<std::string*>(buffer);
size_t len = ptr->length();  // ❌ UB：对象未构造
```

**3. 修改 const 对象**：
```cpp
const int x = 10;
int* ptr = const_cast<int*>(&x);
*ptr = 20;  // ❌ UB：修改 const 对象
```

### 2.4 类型相关错误？

**1. 严格别名规则（Strict Aliasing）**：
```cpp
int x = 42;
float* fptr = reinterpret_cast<float*>(&x);
float y = *fptr;  // ❌ UB：类型双关（type punning）
```

**2. 虚函数调用**：
```cpp
class Base {
public:
    virtual void foo() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void foo() override { std::cout << "Derived\n"; }
};

Base* ptr = static_cast<Base*>(malloc(sizeof(Derived)));
ptr->foo();  // ❌ UB：对象未构造就调用虚函数
```

**3. 不正确的 delete**：
```cpp
int* arr = new int[10];
delete arr;  // ❌ UB：应该用 delete[]

Base* ptr = new Derived();
delete ptr;  // ❌ UB（如果 Base 析构不是虚函数）
```

### 2.5 并发错误？

**1. 数据竞争**：
```cpp
int global = 0;

// 线程 1
global = 1;  // ❌ UB：无同步的并发写

// 线程 2
int x = global;  // ❌ UB：无同步的并发读写
```

**2. 修改 string literals**：
```cpp
char* str = "hello";
str[0] = 'H';  // ❌ UB：修改字符串字面量
```

---

## 3. 未指定行为与实现定义行为

### 3.1 未指定行为？

**1. 函数参数求值顺序**：
```cpp
int f1() { std::cout << "f1 "; return 1; }
int f2() { std::cout << "f2 "; return 2; }

func(f1(), f2());  // 未指定：可能输出 "f1 f2" 或 "f2 f1"
```

**2. 表达式求值顺序（C++17 前）**：
```cpp
int i = 0;
int arr[10];
arr[i] = i++;  // C++17 前未指定

// C++17 后：赋值左侧先于右侧求值
```

### 3.2 实现定义行为？

**1. 基本类型大小**：
```cpp
sizeof(int);     // 实现定义：通常 4
sizeof(long);    // 实现定义：32 位系统 4，64 位系统 8
sizeof(void*);   // 实现定义：取决于平台位数
```

**2. 有符号数表示**：
```cpp
// 实现定义：补码、反码、原码
int x = -1;
```

**3. 右移行为**：
```cpp
int x = -1;
int y = x >> 1;  // 实现定义：算术右移或逻辑右移
```

---

## 4. 检测未定义行为

### 4.1 UndefinedBehaviorSanitizer (UBSan)？

```bash
# 编译时启用 UBSan
g++ -fsanitize=undefined -g main.cpp -o program

# 运行
./program
```

**可检测的 UB**：
```cpp
// 1. 有符号整数溢出
int x = INT_MAX;
x = x + 1;  // UBSan 检测到

// 2. 数组越界
int arr[10];
arr[10] = 42;  // UBSan 检测到

// 3. 空指针解引用
int* ptr = nullptr;
*ptr = 10;  // UBSan 检测到

// 4. 除以零
int y = 10 / 0;  // UBSan 检测到

// 5. 非法位移
int z = 1 << 32;  // UBSan 检测到
```

### 4.2 静态分析工具？

**1. Clang Static Analyzer**：
```bash
scan-build make
```

**2. Cppcheck**：
```bash
cppcheck --enable=all main.cpp
```

**3. Clang-Tidy**：
```bash
clang-tidy main.cpp -- -std=c++17
```

### 4.3 编译器警告？

```bash
# GCC/Clang
g++ -Wall -Wextra -Wpedantic main.cpp

# 更严格的警告
g++ -Wall -Wextra -Wpedantic \
    -Wuninitialized \
    -Warray-bounds \
    -Wnull-dereference \
    main.cpp
```

---

## 5. 避免未定义行为

### 5.1 安全的整数运算？

```cpp
#include <limits>

// ❌ 不安全
int unsafe_add(int a, int b) {
    return a + b;  // 可能溢出
}

// ✅ 安全
std::optional<int> safe_add(int a, int b) {
    if ((b > 0 && a > INT_MAX - b) ||
        (b < 0 && a < INT_MIN - b)) {
        return std::nullopt;  // 溢出
    }
    return a + b;
}

// ✅ 使用无符号数（溢出有定义）
unsigned int safe_unsigned_add(unsigned int a, unsigned int b) {
    return a + b;  // 模运算，定义明确
}
```

### 5.2 安全的指针操作？

```cpp
// ❌ 不安全
void unsafe(int* ptr) {
    *ptr = 10;  // ptr 可能为空
}

// ✅ 使用引用
void safe_ref(int& ref) {
    ref = 10;  // 引用不能为空
}

// ✅ 检查指针
void safe_ptr(int* ptr) {
    if (ptr == nullptr) {
        throw std::invalid_argument("ptr is null");
    }
    *ptr = 10;
}

// ✅ 使用智能指针
void safe_smart(std::shared_ptr<int> ptr) {
    if (!ptr) {
        throw std::invalid_argument("ptr is null");
    }
    *ptr = 10;
}
```

### 5.3 安全的数组访问？

```cpp
// ❌ 不安全
int arr[10];
arr[i] = 42;  // i 可能越界

// ✅ 使用 std::vector 和 at()
std::vector<int> vec(10);
vec.at(i) = 42;  // 越界会抛异常

// ✅ 使用 std::array 和 at()
std::array<int, 10> arr;
arr.at(i) = 42;  // 越界会抛异常

// ✅ 使用 gsl::span（C++20: std::span）
#include <gsl/span>
void process(gsl::span<int> data) {
    data[i] = 42;  // 可以启用边界检查
}
```

### 5.4 避免严格别名问题？

```cpp
// ❌ 不安全
int x = 42;
float* fptr = reinterpret_cast<float*>(&x);
float y = *fptr;  // UB

// ✅ 使用 memcpy
int x = 42;
float y;
std::memcpy(&y, &x, sizeof(float));  // 安全

// ✅ 使用 std::bit_cast（C++20）
int x = 42;
float y = std::bit_cast<float>(x);  // 安全
```

---

## 6. 危险的"优化"

### 6.1 编译器利用 UB 优化？

**1. 空指针检查优化掉**：
```cpp
void func(int* ptr) {
    int x = *ptr;  // 假设 ptr 非空
    if (ptr == nullptr) {  // ❌ 编译器可能删除这个检查
        return;
    }
}
```

**2. 死代码消除**：
```cpp
int x = INT_MAX;
x = x + 1;  // UB
if (x < 0) {  // ❌ 编译器可能假设永远不执行
    handle_overflow();
}
```

**3. 循环优化**：
```cpp
for (int i = 0; i <= n; i++) {
    arr[i] = 0;  // 如果 i == n 时越界 → UB
}
// 编译器可能完全删除循环或产生意外结果
```

### 6.2 真实案例？

**Linux 内核 CVE-2009-1897**：
```cpp
// 旧代码
if (len + sizeof(struct header) < len) {
    return -EINVAL;  // 检查整数溢出
}

// 编译器优化：假设 len + sizeof(...) 不会溢出
// 检查被优化掉 → 安全漏洞
```

**修复**：
```cpp
if (len > SIZE_MAX - sizeof(struct header)) {
    return -EINVAL;  // 不依赖 UB
}
```

---

## 7. 特殊的未定义行为

### 7.1 Sequence Points（C++11 前）？

```cpp
int i = 0;
i = i++;  // ❌ UB：两次修改之间无序列点

int arr[10];
int i = 0;
arr[i++] = i;  // ❌ UB：读写同一变量
```

### 7.2 返回值优化 vs 移动？

```cpp
std::string func() {
    std::string str = "hello";
    return str;  // ✅ RVO 或移动，定义明确
}

std::string& func_bad() {
    std::string str = "hello";
    return str;  // ❌ UB：返回局部变量引用
}
```

---

## 总结

### 常见 UB 速查表

| UB 类型 | 示例 | 检测工具 |
|---------|------|---------|
| 空指针解引用 | `*nullptr` | UBSan, ASan |
| 数组越界 | `arr[10]` (size 10) | UBSan, ASan |
| 有符号溢出 | `INT_MAX + 1` | UBSan |
| 除以零 | `10 / 0` | UBSan |
| Use-after-free | `delete ptr; *ptr` | ASan |
| 数据竞争 | 无同步的并发访问 | TSan |
| 非法位移 | `1 << 32` | UBSan |
| 严格别名 | 类型双关 | `-fstrict-aliasing` 警告 |

### 关键记忆点

1. **UB = 任何事情都可能发生**：崩溃、错误结果、安全漏洞
2. **有符号溢出是 UB，无符号溢出不是**
3. **空指针解引用是最常见的 UB**
4. **编译器会利用 UB 进行优化**
5. **使用 Sanitizers 检测 UB**
6. **优先使用标准库容器和智能指针**
7. **启用编译器警告（-Wall -Wextra）**

---

## 参考资源

- [C++ Reference: Undefined Behavior](https://en.cppreference.com/w/cpp/language/ub)
- [What Every C Programmer Should Know About UB](http://blog.llvm.org/2011/05/what-every-c-programmer-should-know.html)
- [UBSan Documentation](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
