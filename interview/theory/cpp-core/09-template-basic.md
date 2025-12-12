# 09 - 模板基础 (Template Basics)

## 简介

模板是 C++ 泛型编程的核心机制，允许编写与类型无关的代码。模板在编译期实例化，实现零运行时开销的抽象。

**核心特点**：
- **编译期多态**：通过模板实例化生成特定类型的代码
- **类型安全**：编译期类型检查
- **代码复用**：一次编写，多种类型使用

---

## 1. 函数模板 (Function Templates)

### 1.1 如何定义和使用函数模板？

**问题描述**：函数模板的基本语法和使用方式。

**基础示例**：

```cpp
#include <iostream>

// 函数模板定义
template<typename T>
T max_value(T a, T b) {
    return a > b ? a : b;
}

// 或使用 class 关键字（等价）
template<class T>
T min_value(T a, T b) {
    return a < b ? a : b;
}

void function_template_demo() {
    // 显式指定类型参数
    int max_int = max_value<int>(10, 20);
    std::cout << "max_int: " << max_int << "\n";  // 20

    // 隐式类型推导
    double max_double = max_value(3.14, 2.71);
    std::cout << "max_double: " << max_double << "\n";  // 3.14

    // 字符串比较
    std::string max_str = max_value<std::string>("hello", "world");
    std::cout << "max_str: " << max_str << "\n";  // world
}
```

**多个模板参数**：

```cpp
// 两个不同类型的参数
template<typename T1, typename T2>
void print_pair(T1 first, T2 second) {
    std::cout << first << ", " << second << "\n";
}

void multi_param_demo() {
    print_pair(10, 3.14);        // int, double
    print_pair("name", 25);      // const char*, int
    print_pair<int, int>(1, 2);  // 显式指定类型
}
```

**返回类型推导**：

```cpp
// C++11: 尾置返回类型
template<typename T1, typename T2>
auto add(T1 a, T2 b) -> decltype(a + b) {
    return a + b;
}

// C++14: 自动推导返回类型
template<typename T1, typename T2>
auto multiply(T1 a, T2 b) {
    return a * b;
}

void return_type_demo() {
    auto result1 = add(5, 3.14);      // double
    auto result2 = multiply(2, 1.5);  // double
}
```

**关键要点**：
- `typename` 和 `class` 在定义模板参数时等价，推荐使用 `typename`
- 模板参数推导根据实参类型自动确定
- 模板必须在头文件中定义（或使用显式实例化）

---

### 1.2 模板参数推导的规则？

**问题描述**：编译器如何推导模板参数？

**推导规则**：

```cpp
#include <iostream>

template<typename T>
void func_by_value(T param) {
    // T 会忽略 const 和引用
}

template<typename T>
void func_by_ref(T& param) {
    // T 保留 const，但移除引用
}

template<typename T>
void func_by_const_ref(const T& param) {
    // T 移除 const 和引用
}

template<typename T>
void func_by_rvalue_ref(T&& param) {
    // 万能引用（转发引用）
}

void deduction_demo() {
    int x = 10;
    const int cx = 20;
    const int& rx = x;

    // 按值传递
    func_by_value(x);   // T = int
    func_by_value(cx);  // T = int (忽略 const)
    func_by_value(rx);  // T = int (忽略 const 和引用)

    // 按引用传递
    func_by_ref(x);   // T = int
    func_by_ref(cx);  // T = const int
    // func_by_ref(10);  // ❌ 错误：不能绑定临时对象

    // 按 const 引用传递
    func_by_const_ref(x);   // T = int
    func_by_const_ref(cx);  // T = int
    func_by_const_ref(10);  // T = int，可以绑定临时对象

    // 万能引用
    func_by_rvalue_ref(x);   // T = int&（左值引用）
    func_by_rvalue_ref(10);  // T = int（右值）
}
```

**数组和函数退化**：

```cpp
template<typename T>
void func_array_by_value(T param) {
    // 数组退化为指针
}

template<typename T>
void func_array_by_ref(T& param) {
    // 保留数组类型
}

void array_deduction() {
    int arr[5] = {1, 2, 3, 4, 5};

    func_array_by_value(arr);  // T = int*
    func_array_by_ref(arr);    // T = int[5]

    // 获取数组大小
    template<typename T, size_t N>
    void get_array_size(T (&arr)[N]) {
        std::cout << "数组大小: " << N << "\n";
    }

    get_array_size(arr);  // N = 5
}
```

**关键要点**：
- 按值传递：忽略 `const` 和引用
- 按引用传递：保留 `const`，移除引用
- 按万能引用传递：区分左值和右值
- 数组按值传递会退化为指针

**常见陷阱**：
```cpp
// ❌ 错误：类型推导不一致
template<typename T>
T max_value(T a, T b) {
    return a > b ? a : b;
}

auto result = max_value(10, 3.14);  // ❌ 错误：T 应该是 int 还是 double？

// ✅ 正确：显式指定类型
auto result2 = max_value<double>(10, 3.14);

// ✅ 或使用两个模板参数
template<typename T1, typename T2>
auto max_value2(T1 a, T2 b) {
    return a > b ? a : b;
}
```

---

## 2. 类模板 (Class Templates)

### 2.1 如何定义和使用类模板？

**问题描述**：类模板的基本语法。

**基础示例**：

```cpp
// 类模板定义
template<typename T>
class Stack {
private:
    std::vector<T> elements;

public:
    void push(const T& elem) {
        elements.push_back(elem);
    }

    void pop() {
        if (elements.empty()) {
            throw std::out_of_range("Stack<>::pop(): empty stack");
        }
        elements.pop_back();
    }

    T top() const {
        if (elements.empty()) {
            throw std::out_of_range("Stack<>::top(): empty stack");
        }
        return elements.back();
    }

    bool empty() const {
        return elements.empty();
    }
};

void class_template_demo() {
    // 必须显式指定模板参数
    Stack<int> int_stack;
    int_stack.push(10);
    int_stack.push(20);
    std::cout << int_stack.top() << "\n";  // 20

    Stack<std::string> str_stack;
    str_stack.push("hello");
    str_stack.push("world");
    std::cout << str_stack.top() << "\n";  // world
}
```

**类模板的成员函数**：

```cpp
// 在类外定义成员函数
template<typename T>
class MyClass {
public:
    void print(const T& value);
    T get_value();
private:
    T data;
};

// 成员函数定义（必须在头文件中）
template<typename T>
void MyClass<T>::print(const T& value) {
    std::cout << value << "\n";
}

template<typename T>
T MyClass<T>::get_value() {
    return data;
}
```

**C++17 类模板参数推导 (CTAD)**：

```cpp
// C++17 之前
std::pair<int, double> p1(10, 3.14);

// C++17 之后：自动推导
std::pair p2(10, 3.14);  // std::pair<int, double>

// 自定义类的 CTAD
template<typename T>
class Container {
public:
    Container(T value) : data(value) {}
private:
    T data;
};

void ctad_demo() {
    // C++17
    Container c1(42);        // Container<int>
    Container c2(3.14);      // Container<double>
    Container c3("hello");   // Container<const char*>
}
```

**关键要点**：
- 类模板必须显式指定模板参数（C++17 前）
- 成员函数在首次使用时才实例化
- 类模板的成员函数必须在头文件中定义

---

### 2.2 模板类的静态成员？

**问题描述**：模板类的静态成员如何工作？

```cpp
template<typename T>
class Counter {
public:
    static int count;  // 静态成员声明

    Counter() { ++count; }
    ~Counter() { --count; }
};

// 静态成员定义（每个类型实例化一份）
template<typename T>
int Counter<T>::count = 0;

void static_member_demo() {
    Counter<int> c1, c2, c3;
    std::cout << "Counter<int>::count = " << Counter<int>::count << "\n";  // 3

    Counter<double> d1, d2;
    std::cout << "Counter<double>::count = " << Counter<double>::count << "\n";  // 2

    // 不同类型的实例化有独立的静态成员
}
```

**关键要点**：
- 每个模板实例化都有自己的静态成员副本
- `Counter<int>` 和 `Counter<double>` 是完全不同的类型

---

## 3. 模板参数 (Template Parameters)

### 3.1 非类型模板参数？

**问题描述**：除了类型参数，模板还能接受什么参数？

**非类型参数示例**：

```cpp
#include <array>

// 非类型模板参数（整数）
template<typename T, size_t N>
class FixedArray {
private:
    T data[N];

public:
    size_t size() const { return N; }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }
};

void non_type_param_demo() {
    FixedArray<int, 5> arr1;
    FixedArray<int, 10> arr2;

    // arr1 和 arr2 是不同的类型
    std::cout << "arr1.size() = " << arr1.size() << "\n";  // 5
    std::cout << "arr2.size() = " << arr2.size() << "\n";  // 10
}
```

**允许的非类型参数类型**：

```cpp
// 整数类型
template<int N>
class IntParam { };

// 枚举类型
enum Color { RED, GREEN, BLUE };
template<Color C>
class ColorParam { };

// 指针类型
template<int* Ptr>
class PointerParam { };

// 引用类型 (C++11)
template<int& Ref>
class RefParam { };

// nullptr_t (C++11)
template<std::nullptr_t N>
class NullptrParam { };

// C++20: 浮点数、类类型
template<double D>  // C++20
class FloatParam { };
```

**编译期计算**：

```cpp
// 编译期阶乘
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

void compile_time_factorial() {
    std::cout << "5! = " << Factorial<5>::value << "\n";  // 120
    // 完全在编译期计算
}
```

**关键要点**：
- 非类型参数必须是编译期常量
- 不同的非类型参数值产生不同的类型
- 常用于数组大小、缓冲区大小等编译期常量

---

### 3.2 模板模板参数？

**问题描述**：模板参数本身是一个模板？

```cpp
#include <vector>
#include <list>
#include <deque>

// 模板模板参数
template<typename T, template<typename> class Container>
class MyContainer {
private:
    Container<T> data;

public:
    void add(const T& item) {
        data.push_back(item);
    }

    size_t size() const {
        return data.size();
    }
};

void template_template_param() {
    // 错误示例（std::vector 有两个模板参数）
    // MyContainer<int, std::vector> mc1;  // ❌ 错误

    // 正确：需要定义适配器
    template<typename T>
    using MyVector = std::vector<T>;

    MyContainer<int, MyVector> mc1;
    mc1.add(10);
    mc1.add(20);
}

// 更实用的示例
template<typename T, template<typename, typename> class Container>
class Stack {
private:
    Container<T, std::allocator<T>> data;

public:
    void push(const T& item) {
        data.push_back(item);
    }

    T pop() {
        T item = data.back();
        data.pop_back();
        return item;
    }
};

void stack_demo() {
    Stack<int, std::vector> vec_stack;
    Stack<int, std::deque> deque_stack;
    Stack<int, std::list> list_stack;
}
```

**关键要点**：
- 模板模板参数允许将容器类型参数化
- 需要注意模板参数的数量匹配
- 实际使用较少，但在某些泛型库中很有用

---

## 4. 模板特化 (Template Specialization)

### 4.1 全特化 (Full Specialization)？

**问题描述**：为特定类型提供特殊实现。

**函数模板全特化**：

```cpp
#include <iostream>
#include <cstring>

// 通用模板
template<typename T>
bool is_equal(T a, T b) {
    return a == b;
}

// 全特化：为 const char* 提供特殊实现
template<>
bool is_equal<const char*>(const char* a, const char* b) {
    return std::strcmp(a, b) == 0;
}

void full_specialization_demo() {
    std::cout << is_equal(10, 10) << "\n";  // true

    const char* s1 = "hello";
    const char* s2 = "hello";
    std::cout << is_equal(s1, s2) << "\n";  // true（使用特化版本）
}
```

**类模板全特化**：

```cpp
// 通用模板
template<typename T>
class Storage {
private:
    T data;

public:
    Storage(T value) : data(value) {}

    T get() const { return data; }

    void print() const {
        std::cout << "Generic: " << data << "\n";
    }
};

// 全特化：为 bool 提供特殊实现
template<>
class Storage<bool> {
private:
    bool data;

public:
    Storage(bool value) : data(value) {}

    bool get() const { return data; }

    void print() const {
        std::cout << "Bool: " << (data ? "true" : "false") << "\n";
    }
};

void class_specialization_demo() {
    Storage<int> int_storage(42);
    int_storage.print();  // Generic: 42

    Storage<bool> bool_storage(true);
    bool_storage.print();  // Bool: true
}
```

**关键要点**：
- 全特化为特定类型提供完全不同的实现
- 语法：`template<>` 后跟特化的类型
- 全特化必须在通用模板声明之后

---

### 4.2 偏特化 (Partial Specialization)？

**问题描述**：为一部分模板参数提供特殊实现（仅适用于类模板）。

**基于类型属性的偏特化**：

```cpp
// 通用模板
template<typename T>
class TypeTraits {
public:
    static constexpr bool is_pointer = false;
    static constexpr bool is_reference = false;
};

// 偏特化：指针类型
template<typename T>
class TypeTraits<T*> {
public:
    static constexpr bool is_pointer = true;
    static constexpr bool is_reference = false;
};

// 偏特化：引用类型
template<typename T>
class TypeTraits<T&> {
public:
    static constexpr bool is_pointer = false;
    static constexpr bool is_reference = true;
};

void partial_specialization_demo() {
    std::cout << TypeTraits<int>::is_pointer << "\n";     // false
    std::cout << TypeTraits<int*>::is_pointer << "\n";    // true
    std::cout << TypeTraits<int&>::is_reference << "\n";  // true
}
```

**基于多个模板参数的偏特化**：

```cpp
// 通用模板
template<typename T1, typename T2>
class Pair {
public:
    T1 first;
    T2 second;

    void print() const {
        std::cout << "Generic Pair\n";
    }
};

// 偏特化：两个类型相同
template<typename T>
class Pair<T, T> {
public:
    T first;
    T second;

    void print() const {
        std::cout << "Same Type Pair\n";
    }
};

// 偏特化：第二个类型是指针
template<typename T1, typename T2>
class Pair<T1, T2*> {
public:
    T1 first;
    T2* second;

    void print() const {
        std::cout << "Second is Pointer\n";
    }
};

void pair_specialization_demo() {
    Pair<int, double> p1;
    p1.print();  // Generic Pair

    Pair<int, int> p2;
    p2.print();  // Same Type Pair

    Pair<int, double*> p3;
    p3.print();  // Second is Pointer
}
```

**关键要点**：
- 偏特化仅适用于类模板（函数模板不支持）
- 偏特化允许为模板参数的子集提供特殊实现
- 编译器选择最特化的版本

**常见陷阱**：
```cpp
// ❌ 错误：函数模板不支持偏特化
template<typename T>
void func(T param) { }

template<typename T>
void func(T* param) { }  // ❌ 这是重载，不是偏特化

// ✅ 正确：使用函数重载
template<typename T>
void func_overload(T param) {
    std::cout << "Generic\n";
}

template<typename T>
void func_overload(T* param) {
    std::cout << "Pointer\n";
}
```

---

## 5. 模板实例化 (Template Instantiation)

### 5.1 隐式实例化 vs 显式实例化？

**问题描述**：模板何时被编译器实例化？

**隐式实例化**：

```cpp
// 模板定义
template<typename T>
T add(T a, T b) {
    return a + b;
}

void implicit_instantiation() {
    // 首次使用时自动实例化
    int result1 = add(1, 2);        // 实例化 add<int>
    double result2 = add(1.0, 2.0); // 实例化 add<double>
}
```

**显式实例化**：

```cpp
// 显式实例化声明（extern）
extern template int add<int>(int, int);

// 显式实例化定义
template int add<int>(int, int);

// 用途：减少编译时间，避免在每个翻译单元重复实例化
```

**模板的分离编译**：

```cpp
// my_template.h
template<typename T>
class MyClass {
public:
    void func();
};

// my_template.cpp
#include "my_template.h"

template<typename T>
void MyClass<T>::func() {
    // 实现
}

// 显式实例化常用类型
template class MyClass<int>;
template class MyClass<double>;
```

**关键要点**：
- **隐式实例化**：使用时自动生成代码
- **显式实例化**：手动指定需要实例化的类型
- **两阶段查找**：模板定义时检查语法，实例化时检查语义

---

### 5.2 模板的二次编译模型？

**问题描述**：模板的编译过程有何特殊之处？

```cpp
template<typename T>
void func(T param) {
    // 阶段 1：模板定义时检查（不依赖 T 的代码）
    int x = 10;  // ✅ 总是正确

    // 阶段 2：实例化时检查（依赖 T 的代码）
    param.foo();  // 只有当 T 有 foo() 方法时才正确
}

class MyClass {
public:
    void foo() { }
};

void two_phase_lookup() {
    MyClass obj;
    func(obj);  // ✅ MyClass 有 foo()

    int x = 10;
    // func(x);  // ❌ int 没有 foo()，实例化时错误
}
```

**依赖名称查找**：

```cpp
void helper() {
    std::cout << "Global helper\n";
}

template<typename T>
void func() {
    helper();  // 非依赖名称，在定义时查找
}

template<typename T>
void func2() {
    T::helper();  // 依赖名称，在实例化时查找
}
```

**关键要点**：
- **阶段 1**：检查模板本身的语法错误
- **阶段 2**：实例化时检查类型特定的语义错误
- 这就是为什么模板错误信息常常很长很复杂

---

## 6. 默认模板参数

### 6.1 如何使用默认模板参数？

**问题描述**：模板参数可以有默认值吗？

```cpp
// 类模板的默认参数
template<typename T, typename Container = std::vector<T>>
class Stack {
private:
    Container data;

public:
    void push(const T& item) {
        data.push_back(item);
    }

    T pop() {
        T item = data.back();
        data.pop_back();
        return item;
    }
};

void default_template_param() {
    // 使用默认容器（vector）
    Stack<int> s1;
    s1.push(10);

    // 指定自定义容器
    Stack<int, std::deque<int>> s2;
    s2.push(20);
}
```

**函数模板的默认参数 (C++11)**：

```cpp
// 函数模板的默认参数
template<typename T = int>
T get_default_value() {
    return T{};
}

void function_default_param() {
    auto v1 = get_default_value();     // int
    auto v2 = get_default_value<double>();  // double
}
```

**关键要点**：
- 默认模板参数从右往左指定
- 类似函数的默认参数规则

---

## 7. 常见陷阱与最佳实践

### 7.1 typename 关键字的使用

**问题描述**：何时必须使用 `typename` 关键字？

```cpp
template<typename T>
class MyClass {
public:
    // ❌ 错误：编译器不知道 value_type 是类型还是静态成员
    // T::value_type x;

    // ✅ 正确：使用 typename 声明这是类型
    typename T::value_type x;

    void func() {
        // 嵌套依赖类型名称需要 typename
        typename T::iterator it;
    }
};
```

**规则**：
- 当访问模板参数的嵌套类型时，必须使用 `typename`
- 例外：基类列表和初始化列表中不需要

```cpp
template<typename T>
class Derived : public T::BaseClass {  // 不需要 typename
    Derived() : T::BaseClass() { }     // 不需要 typename
};
```

---

### 7.2 模板的常见陷阱

**陷阱 1：模板必须在头文件中定义**

```cpp
// ❌ 错误：模板分离到 .cpp 文件
// my_template.h
template<typename T>
class MyClass {
public:
    void func();
};

// my_template.cpp
template<typename T>
void MyClass<T>::func() {
    // 实现
}

// main.cpp
#include "my_template.h"
MyClass<int> obj;
obj.func();  // ❌ 链接错误：找不到实例化代码

// ✅ 正确：在头文件中定义
// my_template.h
template<typename T>
class MyClass {
public:
    void func() {
        // 实现
    }
};
```

**陷阱 2：依赖名称的 `.template` 用法**

```cpp
template<typename T>
void func(T obj) {
    // ❌ 可能被解析为小于号
    // obj.get<int>();

    // ✅ 正确：使用 .template
    obj.template get<int>();
}
```

**陷阱 3：模板参数推导失败**

```cpp
template<typename T>
void func(T param) { }

void demo() {
    // ❌ 错误：不能从 {} 推导类型
    // func({1, 2, 3});

    // ✅ 正确：显式指定类型
    func<std::initializer_list<int>>({1, 2, 3});

    // ✅ 或使用 auto
    auto list = {1, 2, 3};
    func(list);
}
```

---

## 8. 实际应用场景

### 8.1 实现泛型容器

```cpp
template<typename T>
class DynamicArray {
private:
    T* data;
    size_t capacity;
    size_t length;

public:
    DynamicArray() : data(nullptr), capacity(0), length(0) {}

    ~DynamicArray() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (length >= capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[length++] = value;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    size_t size() const { return length; }

private:
    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < length; ++i) {
            new_data[i] = std::move(data[i]);
        }
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }
};
```

### 8.2 实现类型萃取

```cpp
// 判断是否为整数类型
template<typename T>
struct is_integer {
    static constexpr bool value = false;
};

// 特化各种整数类型
template<>
struct is_integer<int> {
    static constexpr bool value = true;
};

template<>
struct is_integer<long> {
    static constexpr bool value = true;
};

template<>
struct is_integer<short> {
    static constexpr bool value = true;
};

// 使用
template<typename T>
void process(T value) {
    if constexpr (is_integer<T>::value) {
        std::cout << "整数类型\n";
    } else {
        std::cout << "非整数类型\n";
    }
}
```

---

## 总结

### 关键概念速查

| 概念 | 说明 |
|------|------|
| **函数模板** | 泛型函数，支持类型推导 |
| **类模板** | 泛型类，必须显式指定类型（C++17 前） |
| **非类型参数** | 整数、指针等编译期常量 |
| **全特化** | 为特定类型提供完全不同的实现 |
| **偏特化** | 为部分模板参数提供特殊实现（仅类模板） |
| **隐式实例化** | 使用时自动生成代码 |
| **显式实例化** | 手动指定实例化的类型 |
| **CTAD** | C++17 类模板参数自动推导 |

### 关键记忆点

1. **模板在编译期实例化**，零运行时开销
2. **模板必须在头文件中定义**（或使用显式实例化）
3. **函数模板支持类型推导**，类模板需要显式指定（C++17 前）
4. **全特化**为特定类型提供特殊实现
5. **偏特化**仅适用于类模板，函数模板使用重载
6. **typename 关键字**用于访问模板参数的嵌套类型
7. **非类型参数**用于编译期常量（如数组大小）
8. **两阶段编译**：定义时检查语法，实例化时检查语义
