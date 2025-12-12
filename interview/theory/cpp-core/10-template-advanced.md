# 10 - 模板高级 (Template Advanced)

## 简介

模板的高级特性是 C++ 元编程和泛型编程的核心，包括 SFINAE、变参模板、折叠表达式等强大工具，使得编译期计算和类型操作成为可能。

**核心特点**：
- **SFINAE**：编译期类型筛选
- **变参模板**：处理任意数量的模板参数
- **模板元编程**：编译期计算
- **Concepts (C++20)**：类型约束

---

## 1. SFINAE (Substitution Failure Is Not An Error)

### 1.1 什么是 SFINAE？

**问题描述**：SFINAE 的原理和使用场景。

**基本原理**：
- 模板参数替换失败不是错误，只是从候选函数集合中移除该模板
- 用于编译期类型筛选和条件编译

**基础示例**：

```cpp
#include <iostream>
#include <type_traits>

// 版本 1：适用于整数类型
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
process(T value) {
    std::cout << "整数类型: " << value << "\n";
    return value * 2;
}

// 版本 2：适用于浮点类型
template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
process(T value) {
    std::cout << "浮点类型: " << value << "\n";
    return value * 1.5;
}

void sfinae_demo() {
    process(10);      // 调用整数版本
    process(3.14);    // 调用浮点版本
    // process("str");  // ❌ 编译错误：没有匹配的函数
}
```

**SFINAE 工作原理**：

```cpp
// 示例：检测类型是否有 size() 方法

// 辅助函数：有 size() 方法的情况
template<typename T>
auto has_size_impl(int) -> decltype(std::declval<T>().size(), std::true_type{});

// 辅助函数：没有 size() 方法的情况（备选）
template<typename T>
std::false_type has_size_impl(...);

// 主模板
template<typename T>
struct has_size : decltype(has_size_impl<T>(0)) {};

void has_size_demo() {
    std::cout << has_size<std::vector<int>>::value << "\n";  // true
    std::cout << has_size<int>::value << "\n";               // false
}
```

**关键要点**：
- SFINAE 发生在模板参数替换阶段
- 常用于函数重载选择
- `enable_if` 是最常用的 SFINAE 工具

---

### 1.2 `std::enable_if` 的使用？

**问题描述**：如何使用 `enable_if` 进行条件编译？

**三种使用位置**：

```cpp
#include <type_traits>

// 1. 返回类型
template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
add1(T a, T b) {
    return a + b;
}

// 2. 函数参数（常用于构造函数）
template<typename T>
void add2(T a, T b, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr) {
    std::cout << a + b << "\n";
}

// 3. 模板参数（推荐，C++11）
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
void add3(T a, T b) {
    std::cout << a + b << "\n";
}

// C++14 简化写法
template<typename T>
std::enable_if_t<std::is_arithmetic<T>::value, T>
add4(T a, T b) {
    return a + b;
}
```

**实际应用：构造函数 SFINAE**：

```cpp
template<typename T>
class SmartPointer {
public:
    // 只接受指针类型
    template<typename U, typename = std::enable_if_t<std::is_pointer<U>::value>>
    SmartPointer(U ptr) : data(ptr) {}

private:
    T* data;
};

void constructor_sfinae() {
    int* p = new int(10);
    SmartPointer<int> sp1(p);  // ✅
    // SmartPointer<int> sp2(10);  // ❌ 编译错误：int 不是指针
}
```

**多个条件组合**：

```cpp
// 条件：整数且有符号
template<typename T>
std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, T>
abs_value(T value) {
    return value < 0 ? -value : value;
}

// 条件：整数且无符号
template<typename T>
std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, T>
abs_value(T value) {
    return value;  // 无符号数不需要取绝对值
}
```

**关键要点**：
- `enable_if<Condition, T>` 在条件为真时定义 `type` 为 `T`
- 条件为假时不定义 `type`，触发 SFINAE
- C++14 引入 `enable_if_t` 简化写法

---

### 1.3 `decltype` 与 `declval` 的使用？

**问题描述**：如何在不构造对象的情况下获取类型信息？

**`decltype` 用法**：

```cpp
int x = 10;
decltype(x) y = 20;  // y 的类型是 int

// 函数返回类型推导
template<typename T1, typename T2>
auto add(T1 a, T2 b) -> decltype(a + b) {
    return a + b;
}

// C++14 简化
template<typename T1, typename T2>
auto add14(T1 a, T2 b) {
    return a + b;
}
```

**`std::declval` 用法**：

```cpp
#include <utility>

// declval：获取类型的右值引用，不需要构造对象
template<typename T>
struct has_foo {
private:
    // 检测是否有 foo() 方法
    template<typename U>
    static auto test(int) -> decltype(std::declval<U>().foo(), std::true_type{});

    template<typename>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

class MyClass {
public:
    void foo() {}
};

void declval_demo() {
    std::cout << has_foo<MyClass>::value << "\n";  // true
    std::cout << has_foo<int>::value << "\n";      // false
}
```

**检测运算符支持**：

```cpp
// 检测是否支持 + 运算符
template<typename T>
struct has_plus {
private:
    template<typename U>
    static auto test(int) -> decltype(std::declval<U>() + std::declval<U>(), std::true_type{});

    template<typename>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

void has_plus_demo() {
    std::cout << has_plus<int>::value << "\n";          // true
    std::cout << has_plus<std::string>::value << "\n";  // true
    std::cout << has_plus<void*>::value << "\n";        // false
}
```

**关键要点**：
- `decltype(expr)`：获取表达式的类型
- `std::declval<T>()`：获取 T 的右值引用，无需构造
- 常用于 SFINAE 和类型萃取

---

## 2. 变参模板 (Variadic Templates)

### 2.1 如何定义变参模板？

**问题描述**：处理任意数量的模板参数。

**基础语法**：

```cpp
#include <iostream>

// 递归终止条件
void print() {
    std::cout << "\n";
}

// 变参模板
template<typename T, typename... Args>
void print(T first, Args... args) {
    std::cout << first << " ";
    print(args...);  // 递归调用
}

void variadic_demo() {
    print(1, 2, 3);                    // 1 2 3
    print("hello", 3.14, 'x', 100);    // hello 3.14 x 100
}
```

**参数包展开**：

```cpp
// sizeof...：获取参数包大小
template<typename... Args>
void show_count(Args... args) {
    std::cout << "参数数量: " << sizeof...(Args) << "\n";
    std::cout << "参数数量: " << sizeof...(args) << "\n";  // 等价
}

void count_demo() {
    show_count(1, 2, 3, 4, 5);  // 参数数量: 5
}
```

**参数包展开模式**：

```cpp
#include <vector>

// 将所有参数添加到 vector
template<typename T, typename... Args>
std::vector<T> make_vector(Args... args) {
    return std::vector<T>{static_cast<T>(args)...};  // 展开并转换
}

void expand_demo() {
    auto vec = make_vector<double>(1, 2, 3, 4, 5);
    // vec: {1.0, 2.0, 3.0, 4.0, 5.0}
}
```

**关键要点**：
- `typename... Args`：模板参数包
- `Args... args`：函数参数包
- `args...`：参数包展开
- `sizeof...(Args)`：获取参数包大小

---

### 2.2 变参模板的高级用法？

**问题描述**：如何实现复杂的参数包操作？

**转发所有参数**：

```cpp
#include <utility>
#include <memory>

// 完美转发
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

void forward_demo() {
    auto p = make_unique<Point>(10, 20);
}
```

**索引参数包**：

```cpp
#include <tuple>

// 获取第 N 个参数
template<size_t N, typename... Args>
auto get_nth(Args... args) {
    return std::get<N>(std::make_tuple(args...));
}

void nth_demo() {
    auto value = get_nth<2>(10, 3.14, "hello", 'x');
    std::cout << value << "\n";  // hello
}
```

**对每个参数应用函数**：

```cpp
// C++17 折叠表达式之前的方式
template<typename F, typename... Args>
void for_each(F func, Args... args) {
    (func(args), ...);  // C++17 折叠表达式
}

// 或使用初始化列表技巧（C++11）
template<typename F, typename... Args>
void for_each_old(F func, Args... args) {
    int dummy[] = {(func(args), 0)...};
    (void)dummy;  // 避免未使用警告
}

void for_each_demo() {
    for_each([](auto x) { std::cout << x << " "; }, 1, 2, 3, 4, 5);
    // 输出: 1 2 3 4 5
}
```

**类型检查**：

```cpp
// 检查所有参数是否为同一类型
template<typename T, typename... Args>
struct all_same : std::conjunction<std::is_same<T, Args>...> {};

void all_same_demo() {
    std::cout << all_same<int, int, int>::value << "\n";       // true
    std::cout << all_same<int, int, double>::value << "\n";    // false
}
```

---

### 2.3 C++17 折叠表达式 (Fold Expressions)？

**问题描述**：如何简化参数包的展开？

**四种折叠形式**：

```cpp
// 1. 一元右折叠：(args op ...)
template<typename... Args>
auto sum_right(Args... args) {
    return (args + ...);  // ((arg1 + arg2) + arg3) + ...
}

// 2. 一元左折叠：(... op args)
template<typename... Args>
auto sum_left(Args... args) {
    return (... + args);  // ... + (arg(n-1) + argn)
}

// 3. 二元右折叠：(args op ... op init)
template<typename... Args>
auto sum_with_init(Args... args) {
    return (args + ... + 0);  // (arg1 + (arg2 + ... (argn + 0)))
}

// 4. 二元左折叠：(init op ... op args)
template<typename... Args>
auto product_with_init(Args... args) {
    return (1 * ... * args);  // (((1 * arg1) * arg2) * ...)
}

void fold_demo() {
    std::cout << sum_right(1, 2, 3, 4, 5) << "\n";         // 15
    std::cout << sum_left(1, 2, 3, 4, 5) << "\n";          // 15
    std::cout << sum_with_init(1, 2, 3, 4, 5) << "\n";     // 15
    std::cout << product_with_init(2, 3, 4) << "\n";       // 24
}
```

**支持的运算符**：

```cpp
// 逻辑运算
template<typename... Args>
bool all_true(Args... args) {
    return (args && ...);  // 所有参数都为真
}

template<typename... Args>
bool any_true(Args... args) {
    return (args || ...);  // 至少一个参数为真
}

// 输出所有参数
template<typename... Args>
void print_all(Args... args) {
    (std::cout << ... << args) << "\n";  // 连续输出
}

// 带分隔符输出
template<typename... Args>
void print_with_space(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

void fold_operators_demo() {
    std::cout << all_true(true, true, true) << "\n";   // true
    std::cout << any_true(false, false, true) << "\n"; // true
    print_all(1, 2, 3, 4, 5);          // 12345
    print_with_space(1, 2, 3, 4, 5);   // 1 2 3 4 5
}
```

**函数调用折叠**：

```cpp
template<typename... Args>
void call_all(Args... args) {
    (args(), ...);  // 调用所有函数
}

void fold_call_demo() {
    call_all(
        []() { std::cout << "first\n"; },
        []() { std::cout << "second\n"; },
        []() { std::cout << "third\n"; }
    );
}
```

**关键要点**：
- C++17 折叠表达式大大简化了参数包展开
- 支持几乎所有二元运算符
- 空参数包对某些运算符有默认值（`&&` → true，`||` → false，`,` → void()）

---

## 3. 模板元编程 (Template Metaprogramming)

### 3.1 编译期计算？

**问题描述**：如何在编译期进行计算？

**编译期阶乘**：

```cpp
// 递归模板
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

// 特化：递归终止条件
template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

void compile_time_factorial() {
    constexpr int result = Factorial<5>::value;
    std::cout << "5! = " << result << "\n";  // 120

    // 编译期计算，可用于数组大小
    int arr[Factorial<4>::value];  // int arr[24];
}
```

**编译期斐波那契数**：

```cpp
template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

void compile_time_fibonacci() {
    constexpr int fib10 = Fibonacci<10>::value;
    std::cout << "Fibonacci(10) = " << fib10 << "\n";  // 55
}
```

**C++11/14/17 constexpr 函数**：

```cpp
// constexpr 函数：编译期和运行期都可用
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    return n <= 1 ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

void constexpr_demo() {
    // 编译期计算
    constexpr int fact5 = factorial(5);
    int arr[factorial(4)];  // 编译期数组大小

    // 运行期计算
    int n;
    std::cin >> n;
    int result = factorial(n);  // 运行期计算
}
```

**关键要点**：
- 模板元编程：通过模板递归实现编译期计算
- `constexpr`：更现代、更易读的编译期计算方式
- 编译期计算结果可用于常量表达式（数组大小、模板参数等）

---

### 3.2 类型列表 (Type List)？

**问题描述**：如何在编译期操作类型集合？

**类型列表实现**：

```cpp
// 类型列表定义
template<typename... Ts>
struct TypeList {};

// 获取第一个类型
template<typename List>
struct Front;

template<typename Head, typename... Tail>
struct Front<TypeList<Head, Tail...>> {
    using type = Head;
};

// 获取剩余类型
template<typename List>
struct PopFront;

template<typename Head, typename... Tail>
struct PopFront<TypeList<Head, Tail...>> {
    using type = TypeList<Tail...>;
};

// 获取列表大小
template<typename List>
struct Size;

template<typename... Ts>
struct Size<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

void type_list_demo() {
    using MyList = TypeList<int, double, char, std::string>;

    using First = typename Front<MyList>::type;  // int
    using Rest = typename PopFront<MyList>::type;  // TypeList<double, char, std::string>

    constexpr size_t size = Size<MyList>::value;  // 4
}
```

**类型列表查找**：

```cpp
// 查找类型索引
template<typename List, typename T>
struct IndexOf;

template<typename T>
struct IndexOf<TypeList<>, T> {
    static constexpr int value = -1;  // 未找到
};

template<typename T, typename... Tail>
struct IndexOf<TypeList<T, Tail...>, T> {
    static constexpr int value = 0;  // 找到
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T> {
private:
    static constexpr int temp = IndexOf<TypeList<Tail...>, T>::value;
public:
    static constexpr int value = temp == -1 ? -1 : temp + 1;
};

void index_of_demo() {
    using MyList = TypeList<int, double, char, std::string>;

    constexpr int idx1 = IndexOf<MyList, double>::value;  // 1
    constexpr int idx2 = IndexOf<MyList, float>::value;   // -1
}
```

**关键要点**：
- 类型列表是编译期类型集合
- 使用模板偏特化实现类型操作
- 常用于泛型库和元编程框架

---

## 4. 类型萃取 (Type Traits)

### 4.1 标准库类型萃取？

**问题描述**：如何在编译期检查和操作类型？

**基本类型判断**：

```cpp
#include <type_traits>

void type_traits_demo() {
    // 基本类型判断
    std::cout << std::is_integral<int>::value << "\n";           // true
    std::cout << std::is_floating_point<double>::value << "\n";  // true
    std::cout << std::is_pointer<int*>::value << "\n";           // true
    std::cout << std::is_reference<int&>::value << "\n";         // true
    std::cout << std::is_array<int[5]>::value << "\n";           // true

    // 类型属性
    std::cout << std::is_const<const int>::value << "\n";        // true
    std::cout << std::is_volatile<volatile int>::value << "\n";  // true
    std::cout << std::is_signed<int>::value << "\n";             // true
    std::cout << std::is_unsigned<unsigned int>::value << "\n";  // true

    // 类型关系
    std::cout << std::is_same<int, int>::value << "\n";          // true
    std::cout << std::is_same<int, float>::value << "\n";        // false
    std::cout << std::is_base_of<std::ios, std::iostream>::value << "\n";  // true
}
```

**类型转换**：

```cpp
void type_transformation() {
    // 移除修饰符
    using T1 = std::remove_const<const int>::type;      // int
    using T2 = std::remove_reference<int&>::type;       // int
    using T3 = std::remove_pointer<int*>::type;         // int
    using T4 = std::remove_cv<const volatile int>::type;  // int

    // 添加修饰符
    using T5 = std::add_const<int>::type;         // const int
    using T6 = std::add_pointer<int>::type;       // int*
    using T7 = std::add_lvalue_reference<int>::type;  // int&

    // C++14 简化写法
    using T8 = std::remove_const_t<const int>;    // int
    using T9 = std::add_pointer_t<int>;           // int*
}
```

**复合类型操作**：

```cpp
// 去除所有修饰符
template<typename T>
using RemoveAll = std::remove_cv_t<std::remove_reference_t<T>>;

void remove_all_demo() {
    using T = const int&;
    using Bare = RemoveAll<T>;  // int
}
```

**关键要点**：
- `<type_traits>` 提供丰富的类型操作工具
- `::value` 获取布尔结果，`::type` 获取转换后的类型
- C++14 引入 `_t` 和 `_v` 后缀简化写法

---

### 4.2 自定义类型萃取？

**问题描述**：如何实现自定义的类型检测？

**检测成员函数**：

```cpp
// 检测是否有 begin() 方法
template<typename T, typename = void>
struct has_begin : std::false_type {};

template<typename T>
struct has_begin<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type {};

// C++17 简化写法
template<typename T>
constexpr bool has_begin_v = has_begin<T>::value;

void has_begin_demo() {
    std::cout << has_begin_v<std::vector<int>> << "\n";  // true
    std::cout << has_begin_v<int> << "\n";               // false
}
```

**检测运算符**：

```cpp
// 检测是否支持 + 运算符
template<typename T, typename U, typename = void>
struct has_plus : std::false_type {};

template<typename T, typename U>
struct has_plus<T, U, std::void_t<decltype(std::declval<T>() + std::declval<U>())>>
    : std::true_type {};

void has_plus_demo() {
    std::cout << has_plus<int, int>::value << "\n";      // true
    std::cout << has_plus<int*, int*>::value << "\n";    // false
}
```

**检测类型成员**：

```cpp
// 检测是否有 value_type 类型成员
template<typename T, typename = void>
struct has_value_type : std::false_type {};

template<typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

void has_value_type_demo() {
    std::cout << has_value_type<std::vector<int>>::value << "\n";  // true
    std::cout << has_value_type<int>::value << "\n";               // false
}
```

**关键要点**：
- `std::void_t` (C++17) 简化 SFINAE 检测
- `std::declval<T>()` 用于获取类型表达式
- 自定义类型萃取用于泛型代码中的条件编译

---

## 5. C++20 Concepts

### 5.1 什么是 Concepts？

**问题描述**：C++20 引入的类型约束机制。

**基本用法**：

```cpp
#include <concepts>

// 定义 concept
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// 使用 concept 约束函数
template<Arithmetic T>
T add(T a, T b) {
    return a + b;
}

// 或使用 requires 子句
template<typename T>
    requires Arithmetic<T>
T subtract(T a, T b) {
    return a - b;
}

void concepts_demo() {
    std::cout << add(10, 20) << "\n";       // ✅
    std::cout << add(3.14, 2.71) << "\n";   // ✅
    // std::cout << add("a", "b") << "\n";  // ❌ 编译错误：清晰的错误信息
}
```

**复合 Concepts**：

```cpp
// 可加且可比较
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
};

template<typename T>
concept AddableAndComparable = Addable<T> && Comparable<T>;

// 使用
template<AddableAndComparable T>
T max_sum(T a, T b, T c) {
    T sum1 = a + b;
    T sum2 = b + c;
    return sum1 > sum2 ? sum1 : sum2;
}
```

**标准库 Concepts**：

```cpp
#include <concepts>
#include <ranges>

// std::integral：整数类型
template<std::integral T>
T double_value(T value) {
    return value * 2;
}

// std::floating_point：浮点类型
template<std::floating_point T>
T half_value(T value) {
    return value / 2;
}

// std::ranges::range：范围类型
template<std::ranges::range R>
void print_range(R&& range) {
    for (const auto& elem : range) {
        std::cout << elem << " ";
    }
    std::cout << "\n";
}
```

**关键要点**：
- Concepts 提供更清晰的模板约束
- 编译错误信息更友好
- 替代复杂的 SFINAE 和 `enable_if`

---

## 6. 常见模板技巧

### 6.1 CRTP (Curiously Recurring Template Pattern)？

**问题描述**：奇异递归模板模式的使用。

```cpp
// CRTP 基类
template<typename Derived>
class Base {
public:
    void interface() {
        // 调用派生类的实现
        static_cast<Derived*>(this)->implementation();
    }

    void common_func() {
        std::cout << "Common function\n";
    }
};

// 派生类
class Derived1 : public Base<Derived1> {
public:
    void implementation() {
        std::cout << "Derived1 implementation\n";
    }
};

class Derived2 : public Base<Derived2> {
public:
    void implementation() {
        std::cout << "Derived2 implementation\n";
    }
};

void crtp_demo() {
    Derived1 d1;
    d1.interface();  // Derived1 implementation

    Derived2 d2;
    d2.interface();  // Derived2 implementation
}
```

**CRTP 的优势**：

```cpp
// 编译期多态（无虚函数开销）
template<typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
};

class Circle : public Shape<Circle> {
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area_impl() const {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape<Rectangle> {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area_impl() const {
        return width * height;
    }
};

// 泛型函数
template<typename Shape>
void print_area(const Shape& shape) {
    std::cout << "Area: " << shape.area() << "\n";
}

void crtp_polymorphism() {
    Circle c(5.0);
    Rectangle r(4.0, 6.0);

    print_area(c);  // 无虚函数调用，编译期确定
    print_area(r);
}
```

**关键要点**：
- CRTP 实现编译期多态
- 避免虚函数的运行时开销
- 常用于性能敏感的泛型库

---

### 6.2 Tag Dispatch？

**问题描述**：使用类型标签选择不同的实现。

```cpp
#include <iterator>

// 标签类型
struct input_iterator_tag {};
struct random_access_iterator_tag {};

// 不同迭代器的实现
template<typename Iterator>
void advance_impl(Iterator& it, int n, input_iterator_tag) {
    // 输入迭代器：只能 ++
    while (n--) ++it;
}

template<typename Iterator>
void advance_impl(Iterator& it, int n, random_access_iterator_tag) {
    // 随机访问迭代器：可以直接 +=
    it += n;
}

// 主函数：根据迭代器类型分派
template<typename Iterator>
void advance(Iterator& it, int n) {
    advance_impl(it, n, typename std::iterator_traits<Iterator>::iterator_category{});
}

void tag_dispatch_demo() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();
    advance(it, 3);  // 使用随机访问版本
}
```

**关键要点**：
- Tag Dispatch 通过重载选择最优实现
- 常用于 STL 算法优化
- 比 SFINAE 更简洁

---

### 6.3 Policy-Based Design？

**问题描述**：使用策略类定制行为。

```cpp
// 存储策略
template<typename T>
class VectorStorage {
    std::vector<T> data;
public:
    void add(const T& item) { data.push_back(item); }
    T get(size_t index) const { return data[index]; }
    size_t size() const { return data.size(); }
};

template<typename T>
class DequeStorage {
    std::deque<T> data;
public:
    void add(const T& item) { data.push_back(item); }
    T get(size_t index) const { return data[index]; }
    size_t size() const { return data.size(); }
};

// 主类：使用策略
template<typename T, template<typename> class StoragePolicy = VectorStorage>
class Container : private StoragePolicy<T> {
public:
    using StoragePolicy<T>::add;
    using StoragePolicy<T>::get;
    using StoragePolicy<T>::size;
};

void policy_demo() {
    Container<int, VectorStorage> c1;  // 使用 vector
    Container<int, DequeStorage> c2;   // 使用 deque

    c1.add(10);
    c2.add(20);
}
```

**关键要点**：
- Policy-Based Design 允许灵活定制类行为
- 编译期选择策略，无运行时开销
- 常用于泛型库设计

---

## 7. 常见陷阱与最佳实践

### 7.1 模板代码膨胀

**问题描述**：每个模板实例化都生成一份代码，可能导致二进制体积增大。

```cpp
// ❌ 问题：为每个 N 生成不同的代码
template<int N>
void process_array(int* arr) {
    for (int i = 0; i < N; ++i) {
        arr[i] *= 2;
    }
}

// ✅ 改进：提取公共代码
void process_array_impl(int* arr, int n) {
    for (int i = 0; i < n; ++i) {
        arr[i] *= 2;
    }
}

template<int N>
void process_array(int* arr) {
    process_array_impl(arr, N);
}
```

---

### 7.2 编译时间优化

**问题描述**：复杂模板导致编译时间增加。

**技巧**：
1. **使用显式实例化**：将常用类型在一个编译单元实例化
2. **避免过深的模板递归**：限制递归深度
3. **使用 `extern template`**：避免重复实例化
4. **C++20 Modules**：减少头文件解析时间

```cpp
// 显式实例化
// my_template.cpp
template class MyClass<int>;
template class MyClass<double>;
```

---

## 总结

### 关键概念速查

| 概念 | 说明 |
|------|------|
| **SFINAE** | 模板替换失败不是错误，用于类型筛选 |
| **enable_if** | 条件编译的主要工具 |
| **变参模板** | 处理任意数量的模板参数 |
| **折叠表达式** | C++17 简化参数包展开 |
| **模板元编程** | 编译期计算和类型操作 |
| **constexpr** | 现代编译期计算方式 |
| **Type Traits** | 标准库类型萃取工具 |
| **Concepts (C++20)** | 类型约束，替代 SFINAE |
| **CRTP** | 编译期多态 |
| **Tag Dispatch** | 基于类型标签的函数分派 |

### 关键记忆点

1. **SFINAE** 用于编译期类型筛选和条件编译
2. **变参模板** 通过递归或折叠表达式展开参数包
3. **C++17 折叠表达式** 大幅简化参数包操作
4. **constexpr** 是现代编译期计算的首选方式
5. **Type Traits** 提供丰富的类型检查和转换工具
6. **C++20 Concepts** 提供更清晰的类型约束
7. **CRTP** 实现零开销的编译期多态
8. **模板代码膨胀** 是需要注意的性能问题

### 进阶方向

- **Concepts** 深入学习（C++20）
- **Ranges** 和 **Views**（C++20）
- **模板元编程库**：Boost.MPL、Brigand
- **表达式模板**：用于数值计算优化
- **编译期正则表达式**：CTRE
