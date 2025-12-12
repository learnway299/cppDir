# C++ 类型系统

## 目录
1. [基本类型](#1-基本类型)
2. [类型转换](#2-类型转换)
3. [类型推导](#3-类型推导)
4. [类型特性](#4-类型特性)

---

## 1. 基本类型

### Q1: C++ 基本数据类型有哪些？各自的大小？

**答案：**

| 类型 | 典型大小 | 范围（有符号） |
|------|---------|---------------|
| `char` | 1 字节 | -128 ~ 127 |
| `short` | 2 字节 | -32768 ~ 32767 |
| `int` | 4 字节 | -2^31 ~ 2^31-1 |
| `long` | 4/8 字节 | 平台相关 |
| `long long` | 8 字节 | -2^63 ~ 2^63-1 |
| `float` | 4 字节 | ~7 位有效数字 |
| `double` | 8 字节 | ~15 位有效数字 |
| `bool` | 1 字节 | true/false |
| `void*` | 4/8 字节 | 平台相关 |

**注意**：C++ 标准只规定了最小大小，实际大小取决于平台。

```cpp
// 固定宽度整数类型（C++11）
#include <cstdint>
int8_t   i8;    // 精确 8 位
int16_t  i16;   // 精确 16 位
int32_t  i32;   // 精确 32 位
int64_t  i64;   // 精确 64 位
uint32_t u32;   // 无符号 32 位

// 查看类型大小
std::cout << sizeof(int) << std::endl;
```

---

### Q2: 有符号数和无符号数的区别？混用有什么风险？

**答案：**

- **有符号数**：最高位为符号位，可表示负数
- **无符号数**：所有位都用于数值，只能表示非负数

**混用风险：**

```cpp
int a = -1;
unsigned int b = 1;

// 危险：有符号数会被转换为无符号数
if (a < b) {
    // 不会执行！-1 转换为 unsigned 是一个很大的数
}

// 正确做法
if (a < static_cast<int>(b)) {
    // 会执行
}

// 另一个陷阱：循环
for (unsigned int i = 10; i >= 0; i--) {
    // 死循环！i 永远 >= 0
}
```

**规则：**
- 当有符号和无符号混合运算时，有符号会转换为无符号
- 避免在循环条件中使用无符号类型减法

---

### Q3: 浮点数比较为什么不能用 == ？

**答案：**
浮点数在计算机中以二进制表示，存在精度损失。

```cpp
double a = 0.1 + 0.2;
double b = 0.3;

// 错误：可能为 false
if (a == b) { }

// 正确：使用误差范围
const double EPSILON = 1e-9;
if (std::abs(a - b) < EPSILON) {
    // 相等
}

// C++11：使用 std::numeric_limits
#include <limits>
if (std::abs(a - b) < std::numeric_limits<double>::epsilon()) {
    // 相等
}
```

**浮点数的特殊值：**

```cpp
#include <cmath>
#include <limits>

double inf = std::numeric_limits<double>::infinity();
double nan = std::nan("");

std::isnan(nan);      // true
std::isinf(inf);      // true
std::isfinite(1.0);   // true
```

---

### Q4: 什么是类型别名？typedef 和 using 的区别？

**答案：**

```cpp
// typedef（传统方式）
typedef unsigned long ulong;
typedef int (*FuncPtr)(int, int);  // 函数指针类型别名

// using（C++11，推荐）
using ulong = unsigned long;
using FuncPtr = int (*)(int, int);

// using 支持模板别名，typedef 不支持
template<typename T>
using Vec = std::vector<T>;        // 模板别名

Vec<int> v;  // std::vector<int>

// typedef 无法直接实现模板别名
template<typename T>
struct VecTypedef {
    typedef std::vector<T> type;
};
VecTypedef<int>::type v;  // 繁琐
```

---

## 2. 类型转换

### Q5: C++ 有哪四种类型转换？各自的用途？

**答案：**

| 转换类型 | 用途 | 安全性 |
|---------|------|--------|
| `static_cast` | 基本类型转换、相关类型转换 | 编译期检查 |
| `dynamic_cast` | 多态类型向下转换 | 运行期检查 |
| `const_cast` | 添加/移除 const | 可能导致 UB |
| `reinterpret_cast` | 位模式重新解释 | 最不安全 |

```cpp
// 1. static_cast：最常用
double d = 3.14;
int i = static_cast<int>(d);           // 3

Base* b = new Derived();
Derived* d = static_cast<Derived*>(b); // 向下转换（不安全）

// 2. dynamic_cast：安全的向下转换（需要多态类型）
class Base { virtual void foo() {} };
class Derived : public Base {};

Base* b = new Derived();
Derived* d = dynamic_cast<Derived*>(b);  // 成功
if (d == nullptr) {
    // 转换失败
}

// 引用版本会抛出 std::bad_cast
try {
    Derived& dr = dynamic_cast<Derived&>(*b);
} catch (std::bad_cast& e) {
    // 转换失败
}

// 3. const_cast：移除 const
const int* cp = &i;
int* p = const_cast<int*>(cp);

// 4. reinterpret_cast：重新解释位模式
int* ip = &i;
char* cp = reinterpret_cast<char*>(ip);  // 按字节访问
```

---

### Q6: dynamic_cast 的工作原理？

**答案：**
`dynamic_cast` 利用**运行时类型信息（RTTI）**进行安全的类型转换。

**原理：**
1. 编译器为每个多态类生成类型信息
2. 虚函数表（vtable）中包含指向类型信息的指针
3. 运行时检查对象的实际类型是否与目标类型兼容

```cpp
class Base {
    virtual void foo() {}  // 必须有虚函数
};

class Derived : public Base {};
class Other : public Base {};

Base* b = new Derived();

// 成功：b 实际指向 Derived
Derived* d = dynamic_cast<Derived*>(b);  // 非空

// 失败：b 不是 Other
Other* o = dynamic_cast<Other*>(b);      // nullptr
```

**性能考虑：**
- `dynamic_cast` 有运行时开销
- 如果确定类型，使用 `static_cast`
- 某些编译器可以禁用 RTTI（`-fno-rtti`）

---

### Q7: 隐式类型转换在什么情况下发生？

**答案：**

1. **算术转换**：
```cpp
int i = 10;
double d = i;       // int -> double
long l = i + d;     // i 转换为 double，结果转换为 long
```

2. **指针转换**：
```cpp
Derived* d = new Derived();
Base* b = d;        // 派生类指针 -> 基类指针
void* v = b;        // 任意指针 -> void*
```

3. **转换构造函数**：
```cpp
class String {
public:
    String(const char* s) {}  // 转换构造函数
};

String s = "hello";  // const char* 隐式转换为 String
```

4. **转换运算符**：
```cpp
class Integer {
    int value;
public:
    operator int() const { return value; }  // 转换运算符
};

Integer i;
int x = i;  // 隐式调用 operator int()
```

---

### Q8: explicit 关键字的作用？

**答案：**
`explicit` 禁止隐式类型转换，只允许显式转换。

```cpp
class String {
public:
    explicit String(const char* s) {}
};

// String s1 = "hello";  // 错误：不能隐式转换
String s2("hello");       // 正确：显式构造
String s3 = String("hello"); // 正确：显式构造

// explicit 转换运算符（C++11）
class Integer {
    int value;
public:
    explicit operator bool() const { return value != 0; }
};

Integer i;
// bool b = i;    // 错误：不能隐式转换
bool b = static_cast<bool>(i);  // 正确
if (i) { }       // 正确：条件语句中允许
```

**建议**：单参数构造函数应该默认使用 `explicit`，除非确实需要隐式转换。

---

## 3. 类型推导

### Q9: auto 的推导规则是什么？

**答案：**

`auto` 的推导规则与模板参数推导相同：

```cpp
// 1. 基本推导
auto i = 10;          // int
auto d = 3.14;        // double
auto s = "hello";     // const char*

// 2. 引用和 const 会被忽略
int x = 10;
const int& rx = x;
auto a = rx;          // int（不是 const int&）

// 3. 保留引用和 const 需要显式指定
auto& ar = rx;        // const int&
const auto& car = x;  // const int&
auto&& rr = x;        // int&（转发引用）
auto&& rr2 = 10;      // int&&

// 4. 数组和函数退化为指针
int arr[10];
auto a = arr;         // int*

void func();
auto f = func;        // void(*)()

// 5. 初始化列表
auto il = {1, 2, 3};  // std::initializer_list<int>
// auto err = {1, 2.0}; // 错误：类型不一致
```

---

### Q10: decltype 的推导规则是什么？

**答案：**

`decltype` 推导表达式的类型，保留引用和 const。

```cpp
// 1. 变量名：推导声明类型
int x = 10;
const int& rx = x;
decltype(x) a;        // int
decltype(rx) b = x;   // const int&

// 2. 表达式：根据值类别推导
// - 左值表达式 -> T&
// - 右值表达式 -> T
decltype((x)) c = x;  // int&（(x) 是左值表达式）
decltype(x + 1) d;    // int（x + 1 是右值）

// 3. 函数调用：推导返回类型
int func();
int& funcRef();
decltype(func()) e;     // int
decltype(funcRef()) f = x;  // int&

// 4. decltype(auto)（C++14）
int& getRef();
auto a = getRef();           // int（引用丢失）
decltype(auto) b = getRef(); // int&（保留引用）
```

---

### Q11: auto 和 decltype 的区别？

**答案：**

| 特性 | auto | decltype |
|------|------|----------|
| 推导来源 | 初始化表达式 | 任意表达式 |
| 引用 | 默认忽略 | 保留 |
| const | 默认忽略 | 保留 |
| 使用时机 | 变量声明 | 类型查询 |

```cpp
int x = 10;
const int& rx = x;

auto a = rx;           // int
decltype(rx) b = x;    // const int&

// 典型用法：返回类型推导
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}

// C++14 简化
template<typename T, typename U>
decltype(auto) add(T t, U u) {
    return t + u;
}
```

---

### Q12: 返回类型推导（trailing return type）的用途？

**答案：**

```cpp
// 问题：模板函数返回类型难以在前面声明
template<typename T, typename U>
??? add(T t, U u) {  // 返回类型是什么？
    return t + u;
}

// 解决方案 1：尾置返回类型（C++11）
template<typename T, typename U>
auto add(T t, U u) -> decltype(t + u) {
    return t + u;
}

// 解决方案 2：返回类型推导（C++14）
template<typename T, typename U>
auto add(T t, U u) {
    return t + u;
}

// 注意：多个 return 必须推导出相同类型
auto func(bool flag) {
    if (flag) return 1;
    return 2;      // OK：都是 int
    // return 2.0; // 错误：与前面类型不一致
}
```

---

## 4. 类型特性

### Q13: 什么是 POD 类型？

**答案：**

POD（Plain Old Data）是可以用 memcpy 复制的简单类型。

C++11 将 POD 分解为：
- **Trivial（平凡）**：默认构造/拷贝/移动/析构都是平凡的
- **Standard Layout（标准布局）**：内存布局与 C 兼容

```cpp
#include <type_traits>

// Trivial 类型
struct Trivial {
    int x;
    double y;
};

// 非 Trivial（有自定义构造函数）
struct NonTrivial {
    NonTrivial() {}
    int x;
};

// Standard Layout
struct StandardLayout {
    int x;
    int y;
};

// 非 Standard Layout（访问控制不一致）
struct NonStandardLayout {
    int x;
private:
    int y;
};

// 检查类型特性
static_assert(std::is_trivial<Trivial>::value, "");
static_assert(std::is_standard_layout<StandardLayout>::value, "");
static_assert(std::is_pod<Trivial>::value, "");  // C++20 废弃
```

---

### Q14: 什么是聚合类型？

**答案：**

聚合类型可以使用大括号初始化。

**聚合类型条件（C++11）**：
- 没有用户声明的构造函数
- 没有私有/保护的非静态成员
- 没有虚函数
- 没有虚基类
- 没有默认成员初始化器（C++11，C++14 放宽）

```cpp
// 聚合类型
struct Aggregate {
    int x;
    double y;
    char z[10];
};

Aggregate a = {1, 2.0, "hello"};  // 聚合初始化

// 非聚合类型
struct NonAggregate {
    NonAggregate(int x) : x(x) {}  // 用户声明的构造函数
    int x;
};

// NonAggregate na = {1};  // 错误
```

---

### Q15: 如何判断两个类型是否相同？

**答案：**

```cpp
#include <type_traits>

// std::is_same
static_assert(std::is_same<int, int>::value, "");
static_assert(!std::is_same<int, double>::value, "");

// 注意：引用和 const 影响判断
static_assert(!std::is_same<int, int&>::value, "");
static_assert(!std::is_same<int, const int>::value, "");

// 移除修饰符后比较
using T1 = const int&;
using T2 = int;
static_assert(std::is_same<
    std::remove_cv_t<std::remove_reference_t<T1>>,
    T2
>::value, "");

// C++20 简化
static_assert(std::is_same_v<int, int>);  // _v 后缀
```

---

### Q16: 常用的 type_traits 有哪些？

**答案：**

```cpp
#include <type_traits>

// 类型判断
std::is_void<T>
std::is_integral<T>
std::is_floating_point<T>
std::is_pointer<T>
std::is_reference<T>
std::is_array<T>
std::is_class<T>
std::is_enum<T>
std::is_function<T>

// 类型关系
std::is_same<T, U>
std::is_base_of<Base, Derived>
std::is_convertible<From, To>

// 类型修改
std::remove_const<T>
std::remove_reference<T>
std::remove_pointer<T>
std::add_const<T>
std::add_pointer<T>
std::decay<T>               // 应用函数参数的类型退化

// 类型属性
std::is_const<T>
std::is_trivial<T>
std::is_pod<T>
std::is_standard_layout<T>

// 条件选择
std::conditional<condition, T, F>  // 条件 ? T : F
std::enable_if<condition, T>       // SFINAE
```

---

## 常见面试陷阱

### 陷阱 1：整数提升

```cpp
char a = 127;
char b = 1;
char c = a + b;  // a + b 的结果是 int（128），赋值给 char 溢出

// 正确：
int result = a + b;  // 128
```

### 陷阱 2：auto 与数组

```cpp
int arr[10];
auto a = arr;      // int*（不是 int[10]）
auto& r = arr;     // int(&)[10]（引用保留数组类型）

decltype(arr) b;   // int[10]
```

### 陷阱 3：decltype 与括号

```cpp
int x = 10;
decltype(x) a;     // int
decltype((x)) b = x;  // int&（加括号变成表达式）

auto f = [&]() -> decltype((x)) { return x; };
// 返回 int&，可能导致悬空引用
```

### 陷阱 4：size_t 与负数

```cpp
std::vector<int> v = {1, 2, 3};
for (int i = v.size() - 1; i >= 0; i--) {
    // 警告：v.size() 返回 size_t
    // i >= 0 永远为真（如果 i 是 unsigned）
}

// 正确：
for (size_t i = v.size(); i > 0; i--) {
    std::cout << v[i - 1];
}
// 或使用反向迭代器
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    std::cout << *it;
}
```

---

## 总结

| 主题 | 关键点 |
|------|--------|
| 基本类型 | 大小平台相关，使用固定宽度类型 |
| 类型转换 | static_cast 最常用，dynamic_cast 最安全 |
| auto | 推导初始化表达式类型，忽略引用和 const |
| decltype | 推导表达式类型，保留引用和 const |
| type_traits | 编译期类型判断和转换 |
