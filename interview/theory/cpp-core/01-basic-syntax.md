# C++ 基础语法

## 目录
1. [const 关键字](#1-const-关键字)
2. [static 关键字](#2-static-关键字)
3. [指针与引用](#3-指针与引用)
4. [作用域](#4-作用域)

---

## 1. const 关键字

### Q1: const 的作用是什么？

**答案：**
`const` 用于定义常量，表示值不可修改。主要作用：

1. **定义常量**：替代宏定义，有类型检查
2. **保护数据**：防止意外修改
3. **编译优化**：编译器可能将 const 变量放入只读段
4. **接口契约**：表明函数不会修改某些数据

```cpp
const int MAX_SIZE = 100;        // 常量
const int* p1;                   // 指向常量的指针
int* const p2 = &x;              // 常量指针
const int* const p3 = &x;        // 指向常量的常量指针
```

---

### Q2: const 与指针的结合有哪几种形式？如何区分？

**答案：**
记忆口诀：**const 在 * 左边修饰值，在 * 右边修饰指针**

| 形式 | 含义 | 可修改指针？ | 可修改值？ |
|------|------|-------------|-----------|
| `const int* p` | 指向常量的指针 | ✅ | ❌ |
| `int const* p` | 同上 | ✅ | ❌ |
| `int* const p` | 常量指针 | ❌ | ✅ |
| `const int* const p` | 指向常量的常量指针 | ❌ | ❌ |

```cpp
int a = 10, b = 20;

const int* p1 = &a;
// *p1 = 30;     // 错误：不能修改值
p1 = &b;         // 正确：可以修改指针

int* const p2 = &a;
*p2 = 30;        // 正确：可以修改值
// p2 = &b;      // 错误：不能修改指针

const int* const p3 = &a;
// *p3 = 30;     // 错误
// p3 = &b;      // 错误
```

---

### Q3: const 成员函数的含义和作用？

**答案：**
const 成员函数表示该函数**不会修改对象的任何成员变量**（mutable 成员除外）。

```cpp
class MyClass {
    int value;
    mutable int accessCount;  // mutable 允许在 const 函数中修改
public:
    int getValue() const {    // const 成员函数
        accessCount++;        // 可以修改 mutable 成员
        // value = 10;        // 错误：不能修改普通成员
        return value;
    }

    void setValue(int v) {    // 非 const 成员函数
        value = v;
    }
};
```

**重要规则：**
- const 对象只能调用 const 成员函数
- 非 const 对象可以调用任意成员函数
- const 成员函数中 `this` 指针类型是 `const MyClass*`

```cpp
const MyClass obj1;
MyClass obj2;

obj1.getValue();   // 正确
// obj1.setValue(10); // 错误：const 对象不能调用非 const 函数
obj2.getValue();   // 正确
obj2.setValue(10); // 正确
```

---

### Q4: const 与 #define 的区别？

**答案：**

| 特性 | const | #define |
|------|-------|---------|
| 类型检查 | ✅ 有 | ❌ 无 |
| 作用域 | 遵循 C++ 作用域规则 | 全局替换 |
| 调试 | 可调试，有符号 | 无符号，难调试 |
| 内存 | 可能分配内存 | 不分配内存 |
| 编译阶段 | 编译期 | 预处理期 |

```cpp
#define PI 3.14159          // 宏定义
const double PI = 3.14159;  // 常量（推荐）

// C++11 推荐使用 constexpr
constexpr double PI = 3.14159;
```

---

### Q5: constexpr 与 const 的区别？

**答案：**

- **const**：运行时常量，值在运行时确定后不可修改
- **constexpr**：编译时常量，值必须在编译期确定

```cpp
const int a = 10;           // 编译期常量
const int b = getValue();   // 运行期常量（getValue 非 constexpr）

constexpr int c = 10;       // 编译期常量
// constexpr int d = getValue(); // 错误：getValue 必须是 constexpr

constexpr int square(int x) {
    return x * x;
}
constexpr int result = square(5);  // 编译期计算，result = 25
```

---

## 2. static 关键字

### Q6: static 在不同场景下的作用？

**答案：**

| 场景 | 作用 | 存储位置 | 生命周期 |
|------|------|---------|---------|
| 全局变量/函数 | 限制为内部链接 | 静态区 | 程序运行期 |
| 局部变量 | 持久化存储 | 静态区 | 程序运行期 |
| 类成员变量 | 所有对象共享 | 静态区 | 程序运行期 |
| 类成员函数 | 无 this 指针 | 代码段 | - |

```cpp
// 1. 全局 static：限制链接性
static int globalVar = 10;      // 仅当前编译单元可见

// 2. 局部 static：持久化
void counter() {
    static int count = 0;       // 只初始化一次
    count++;
    std::cout << count << std::endl;
}

// 3. 类 static 成员
class MyClass {
public:
    static int count;           // 声明
    static void printCount() {  // 静态成员函数
        std::cout << count << std::endl;
        // std::cout << value; // 错误：不能访问非静态成员
    }
private:
    int value;
};
int MyClass::count = 0;         // 定义（类外）
```

---

### Q7: 静态局部变量的初始化时机和线程安全性？

**答案：**

- **初始化时机**：第一次执行到该语句时初始化（懒初始化）
- **C++11 保证**：静态局部变量的初始化是线程安全的

```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;  // C++11 保证线程安全
        return instance;
    }
private:
    Singleton() = default;
};
```

**底层机制**：编译器会生成类似双检锁的代码，使用原子操作保证只初始化一次。

---

### Q8: 静态成员变量和普通成员变量的区别？

**答案：**

| 特性 | 静态成员变量 | 普通成员变量 |
|------|-------------|-------------|
| 存储位置 | 静态区 | 对象内部 |
| 初始化位置 | 类外定义 | 构造函数 |
| 访问方式 | 类名::变量 或 对象.变量 | 对象.变量 |
| 共享性 | 所有对象共享 | 每个对象独立 |
| 生命周期 | 程序运行期 | 对象生命周期 |

```cpp
class Counter {
public:
    static int totalCount;   // 静态：所有对象共享
    int id;                  // 普通：每个对象独立

    Counter() : id(++totalCount) {}
};

int Counter::totalCount = 0;

Counter c1, c2, c3;
// c1.id = 1, c2.id = 2, c3.id = 3
// Counter::totalCount = 3
```

---

## 3. 指针与引用

### Q9: 指针和引用的区别？

**答案：**

| 特性 | 指针 | 引用 |
|------|------|------|
| 初始化 | 可以不初始化 | 必须初始化 |
| 空值 | 可以为 nullptr | 不能为空 |
| 重新绑定 | 可以指向其他对象 | 不能重新绑定 |
| 多级 | 可以有多级指针 | 没有多级引用 |
| sizeof | 指针本身大小 | 被引用对象大小 |
| 底层实现 | 存储地址 | 通常实现为指针 |

```cpp
int a = 10, b = 20;

// 指针
int* p = &a;
p = &b;          // 可以重新指向
p = nullptr;     // 可以为空

// 引用
int& r = a;      // 必须初始化
// int& r2;      // 错误：必须初始化
r = b;           // 这是赋值，不是重新绑定！a 变成 20
```

---

### Q10: 引用的底层实现原理？

**答案：**
引用在底层通常实现为**常量指针**（指针本身不可变）：

```cpp
int a = 10;
int& ref = a;

// 编译器可能将其实现为：
int* const ref_impl = &a;
```

但引用在语义上与指针不同：
- 引用是别名，使用时自动解引用
- 编译器可能优化掉引用，直接使用原变量

---

### Q11: 右值引用是什么？有什么作用？

**答案：**
右值引用（C++11）用于绑定临时对象，实现**移动语义**和**完美转发**。

```cpp
int&& rref = 10;           // 右值引用绑定右值
// int&& rref2 = a;        // 错误：不能绑定左值

// 移动语义
std::string str1 = "Hello";
std::string str2 = std::move(str1);  // str1 的资源转移给 str2

// 移动构造函数
class MyString {
    char* data;
public:
    // 移动构造函数
    MyString(MyString&& other) noexcept
        : data(other.data) {
        other.data = nullptr;  // 转移所有权
    }
};
```

**左值 vs 右值：**
- 左值：有名字、可取地址的表达式
- 右值：临时对象、字面量、表达式结果

---

### Q12: 什么是悬空指针和野指针？如何避免？

**答案：**

- **悬空指针（Dangling Pointer）**：指向已释放内存的指针
- **野指针（Wild Pointer）**：未初始化的指针

```cpp
// 悬空指针
int* p = new int(10);
delete p;
// p 现在是悬空指针
*p = 20;  // 未定义行为！

// 野指针
int* q;   // 未初始化
*q = 10;  // 未定义行为！
```

**避免方法：**
1. 初始化指针为 nullptr
2. delete 后将指针置为 nullptr
3. 使用智能指针
4. 避免返回局部变量的地址

```cpp
int* p = nullptr;          // 初始化
delete p;
p = nullptr;               // delete 后置空

// 使用智能指针
std::unique_ptr<int> up = std::make_unique<int>(10);
```

---

## 4. 作用域

### Q13: C++ 有哪些作用域？

**答案：**

1. **全局作用域**：在所有函数和类之外
2. **命名空间作用域**：在命名空间内
3. **类作用域**：在类定义内
4. **局部作用域**：在函数或代码块内
5. **语句作用域**：for/if/switch 的条件部分

```cpp
int global = 1;                    // 全局作用域

namespace MyNS {
    int ns_var = 2;                // 命名空间作用域
}

class MyClass {
    int member = 3;                // 类作用域
    void func() {
        int local = 4;             // 局部作用域
        {
            int block = 5;         // 块作用域
        }
        for (int i = 0; i < 10; i++) {  // 语句作用域
            // i 在此可见
        }
        // i 不可见
    }
};
```

---

### Q14: 什么是名字隐藏（Name Hiding）？

**答案：**
内层作用域的名字会隐藏外层作用域的同名实体。

```cpp
int x = 10;                // 全局

void func() {
    int x = 20;            // 隐藏全局 x
    {
        int x = 30;        // 隐藏外层 x
        std::cout << x;    // 输出 30
        std::cout << ::x;  // 输出 10（全局）
    }
    std::cout << x;        // 输出 20
}
```

**类中的名字隐藏：**
```cpp
class Base {
public:
    void func(int) { }
};

class Derived : public Base {
public:
    void func(double) { }  // 隐藏 Base::func(int)
};

Derived d;
// d.func(10);   // 调用 Derived::func(double)，不是 Base::func(int)
d.Base::func(10);  // 显式调用 Base::func(int)

// 使用 using 声明解决
class Derived2 : public Base {
public:
    using Base::func;       // 引入 Base::func
    void func(double) { }
};
```

---

### Q15: extern 关键字的作用？

**答案：**
`extern` 用于声明变量或函数具有**外部链接**。

```cpp
// file1.cpp
int globalVar = 10;              // 定义

// file2.cpp
extern int globalVar;            // 声明（不是定义）
void func() {
    std::cout << globalVar;      // 使用 file1.cpp 中的变量
}
```

**extern "C"**：用于 C++ 调用 C 代码，禁用名字修饰（name mangling）。

```cpp
extern "C" {
    void c_function();           // C 风格链接
}

// 或单个声明
extern "C" void c_function();
```

---

### Q16: 链接性（Linkage）有哪几种？

**答案：**

| 链接性 | 含义 | 示例 |
|--------|------|------|
| 外部链接 | 可跨编译单元访问 | 普通全局变量/函数 |
| 内部链接 | 仅当前编译单元可见 | static 全局变量/函数、匿名命名空间 |
| 无链接 | 仅在作用域内可见 | 局部变量 |

```cpp
int global = 1;              // 外部链接
static int internal = 2;     // 内部链接

namespace {
    int anonymous = 3;       // 内部链接（推荐替代 static）
}

void func() {
    int local = 4;           // 无链接
    static int persist = 5;  // 无链接，但持久存储
}
```

---

## 常见面试陷阱

### 陷阱 1：const 引用延长临时对象生命周期

```cpp
const std::string& ref = std::string("temp");
// 临时对象生命周期延长到 ref 作用域结束
std::cout << ref;  // 安全
```

### 陷阱 2：static 变量的初始化顺序

```cpp
// file1.cpp
int a = getB();    // 可能在 b 初始化前调用！

// file2.cpp
int b = 10;
int getB() { return b; }

// 解决方案：使用函数内 static
int& getB() {
    static int b = 10;
    return b;
}
```

### 陷阱 3：指针常量 vs 常量指针

```cpp
int a = 10;
const int* p1 = &a;   // p1 可变，*p1 不可变
int* const p2 = &a;   // p2 不可变，*p2 可变

// 面试题：下面哪个能通过编译？
// *p1 = 20;  // 错误
*p2 = 20;     // 正确
// p2 = &b;   // 错误
p1 = &a;      // 正确
```

---

## 总结

| 关键字 | 核心作用 | 注意事项 |
|--------|---------|---------|
| const | 定义常量、保护数据 | 区分指针 const 位置 |
| static | 持久化、共享、限制链接 | 注意初始化时机 |
| 指针 | 存储地址、间接访问 | 避免悬空/野指针 |
| 引用 | 别名、避免拷贝 | 必须初始化、不能重绑定 |
| extern | 外部链接声明 | 区分声明和定义 |
