# 15 - 现代 C++ 特性 (Modern C++)

## 简介

从 C++11 开始，C++ 进入了现代化时代，每个新标准都带来了重要的语言和库特性。本章总结 C++11/14/17/20/23 的核心特性。

**各版本发布时间**：
- C++11：2011 年（革命性更新）
- C++14：2014 年（小幅改进）
- C++17：2017 年（重要更新）
- C++20：2020 年（革命性更新）
- C++23：2023 年（稳步改进）

---

## 1. C++11 核心特性

### 1.1 auto 类型推导？

**问题描述**：让编译器自动推导类型。

```cpp
// C++98：冗长的类型声明
std::vector<std::string>::iterator it = vec.begin();
std::map<std::string, std::vector<int>>::const_iterator mit = map.begin();

// C++11：auto 简化
auto it = vec.begin();
auto mit = map.begin();

// 必须初始化
auto x;  // ❌ 错误：无法推导类型
auto y = 10;  // ✅

// auto 推导规则
int x = 10;
auto a = x;        // int
auto& b = x;       // int&
const auto& c = x; // const int&
auto* d = &x;      // int*
```

**适用场景**：
- 复杂类型名称
- lambda 表达式
- 迭代器
- 模板返回类型

**注意事项**：
```cpp
const int x = 10;
auto y = x;  // int（丢失 const）
auto& z = x; // const int&（保留 const）

int arr[5];
auto a = arr;  // int*（数组退化）
```

---

### 1.2 Lambda 表达式？

**问题描述**：匿名函数对象。

**基本语法**：

```cpp
// [捕获列表](参数列表) -> 返回类型 { 函数体 }

// 最简单的 lambda
auto func = []() { std::cout << "Hello\n"; };
func();

// 带参数
auto add = [](int a, int b) { return a + b; };
int sum = add(3, 4);  // 7

// 返回类型推导
auto multiply = [](int a, int b) { return a * b; };  // 自动推导为 int

// 显式返回类型
auto divide = [](double a, double b) -> double {
    return a / b;
};
```

**捕获列表**：

```cpp
int x = 10, y = 20;

// 按值捕获
auto f1 = [x, y]() { return x + y; };

// 按引用捕获
auto f2 = [&x, &y]() { x += y; };

// 混合捕获
auto f3 = [x, &y]() { return x + y; };

// 捕获所有（按值）
auto f4 = [=]() { return x + y; };

// 捕获所有（按引用）
auto f5 = [&]() { x += y; };

// 捕获 this（成员函数中）
class MyClass {
    int value = 42;
public:
    void func() {
        auto f = [this]() { return value; };  // 捕获 this
        auto g = [*this]() { return value; };  // C++17：按值捕获 this
    }
};
```

**mutable lambda**：

```cpp
int x = 10;
auto f = [x]() mutable {
    ++x;  // 修改捕获的副本
    return x;
};

std::cout << f() << "\n";  // 11
std::cout << x << "\n";    // 10（原始 x 未变）
```

**与 STL 算法结合**：

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// 查找
auto it = std::find_if(vec.begin(), vec.end(),
                       [](int x) { return x > 3; });

// 计数
int count = std::count_if(vec.begin(), vec.end(),
                          [](int x) { return x % 2 == 0; });

// 排序
std::sort(vec.begin(), vec.end(),
          [](int a, int b) { return a > b; });  // 降序
```

---

### 1.3 右值引用与移动语义？

**问题描述**：避免不必要的拷贝，提升性能。

**左值 vs 右值**：

```cpp
int x = 10;     // x 是左值
int y = x + 5;  // x + 5 是右值（临时值）

int& lref = x;       // 左值引用
// int& lref2 = 10;  // ❌ 错误：不能绑定到右值

int&& rref = 10;     // 右值引用
int&& rref2 = x + 5; // ✅ 绑定到右值
// int&& rref3 = x;  // ❌ 错误：不能绑定到左值
```

**移动构造和移动赋值**：

```cpp
class MyString {
    char* data;
    size_t size;

public:
    // 拷贝构造（深拷贝）
    MyString(const MyString& other) : size(other.size) {
        data = new char[size];
        std::copy(other.data, other.data + size, data);
    }

    // 移动构造（转移所有权）
    MyString(MyString&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;  // 转移所有权
        other.size = 0;
    }

    // 拷贝赋值
    MyString& operator=(const MyString& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new char[size];
            std::copy(other.data, other.data + size, data);
        }
        return *this;
    }

    // 移动赋值
    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~MyString() {
        delete[] data;
    }
};
```

**`std::move` 和 `std::forward`**：

```cpp
MyString str1("hello");
MyString str2 = std::move(str1);  // 显式转换为右值，调用移动构造

// str1 现在处于有效但未指定的状态
// 不要再使用 str1 的值

// std::forward：完美转发
template<typename T>
void wrapper(T&& arg) {
    func(std::forward<T>(arg));  // 保持参数的值类别
}
```

**何时使用移动**：

```cpp
std::vector<MyString> vec;

// ❌ 拷贝：不必要的开销
MyString str("data");
vec.push_back(str);  // 拷贝

// ✅ 移动：高效
vec.push_back(std::move(str));  // 移动

// ✅ 原地构造：更高效
vec.emplace_back("data");  // 直接构造，无拷贝/移动
```

---

### 1.4 智能指针？

**问题描述**：自动管理内存，避免泄漏。

**`std::unique_ptr`**：

```cpp
#include <memory>

// 独占所有权
std::unique_ptr<int> ptr1(new int(10));
std::unique_ptr<int> ptr2 = std::make_unique<int>(20);  // C++14 推荐

// 不能拷贝
// auto ptr3 = ptr1;  // ❌ 错误

// 可以移动
auto ptr3 = std::move(ptr1);  // ✅
// ptr1 现在是 nullptr

// 数组
std::unique_ptr<int[]> arr(new int[10]);
std::unique_ptr<int[]> arr2 = std::make_unique<int[]>(10);  // C++14

// 自定义删除器
auto deleter = [](FILE* fp) { if (fp) fclose(fp); };
std::unique_ptr<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
```

**`std::shared_ptr`**：

```cpp
// 共享所有权（引用计数）
std::shared_ptr<int> ptr1 = std::make_shared<int>(10);
std::shared_ptr<int> ptr2 = ptr1;  // 引用计数 = 2

std::cout << ptr1.use_count() << "\n";  // 2

// 循环引用问题
struct Node {
    std::shared_ptr<Node> next;  // ❌ 可能导致循环引用
};
```

**`std::weak_ptr`**：

```cpp
// 打破循环引用
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;  // ✅ 使用 weak_ptr
};

std::shared_ptr<int> sptr = std::make_shared<int>(10);
std::weak_ptr<int> wptr = sptr;  // 不增加引用计数

// 使用前需要转换为 shared_ptr
if (auto sp = wptr.lock()) {  // 原子地检查并转换
    std::cout << *sp << "\n";
}
```

---

### 1.5 nullptr？

**问题描述**：替代 NULL，类型安全的空指针。

```cpp
// C++98：NULL 的问题
void func(int x) { std::cout << "int\n"; }
void func(int* p) { std::cout << "pointer\n"; }

func(NULL);  // ❌ 可能调用 func(int)（NULL 是 0）

// C++11：nullptr
func(nullptr);  // ✅ 调用 func(int*)

// nullptr 的类型是 std::nullptr_t
int* p1 = nullptr;
char* p2 = nullptr;
void* p3 = nullptr;

// nullptr 可以转换为任意指针类型
```

---

### 1.6 Range-based for 循环？

**问题描述**：简化容器遍历。

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// C++98
for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << " ";
}

// C++11：range-for
for (int x : vec) {
    std::cout << x << " ";  // 按值（拷贝）
}

for (const int& x : vec) {
    std::cout << x << " ";  // 按 const 引用（推荐）
}

for (int& x : vec) {
    x *= 2;  // 修改元素
}

// 适用于所有支持 begin()/end() 的容器
std::map<std::string, int> map;
for (const auto& [key, value] : map) {  // C++17 结构化绑定
    std::cout << key << ": " << value << "\n";
}
```

---

### 1.7 初始化列表？

**问题描述**：统一的初始化语法。

```cpp
#include <initializer_list>

// 统一初始化
int x{10};
int y = {20};
int z(30);

std::vector<int> vec{1, 2, 3, 4, 5};
std::map<std::string, int> map{{"Alice", 25}, {"Bob", 30}};

// 防止窄化转换
int a{3.14};  // ❌ 错误：double 到 int 的窄化转换
int b(3.14);  // ✅ 合法，但会截断

// 自定义类
class MyClass {
public:
    MyClass(std::initializer_list<int> list) {
        for (int x : list) {
            // ...
        }
    }
};

MyClass obj{1, 2, 3, 4, 5};
```

---

### 1.8 其他 C++11 重要特性

**`constexpr`**：

```cpp
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int fact5 = factorial(5);  // 编译期计算
int arr[factorial(4)];  // 数组大小
```

**`static_assert`**：

```cpp
static_assert(sizeof(int) == 4, "int must be 4 bytes");
static_assert(std::is_arithmetic<int>::value, "int must be arithmetic");
```

**`using` 类型别名**：

```cpp
// C++98
typedef std::map<std::string, std::vector<int>> MyMap;

// C++11：更清晰
using MyMap = std::map<std::string, std::vector<int>>;

// 模板别名（typedef 做不到）
template<typename T>
using Vec = std::vector<T>;

Vec<int> v1;
Vec<std::string> v2;
```

**委托构造**：

```cpp
class MyClass {
    int x, y;

public:
    MyClass() : MyClass(0, 0) {}  // 委托给另一个构造函数
    MyClass(int x) : MyClass(x, 0) {}
    MyClass(int x, int y) : x(x), y(y) {}
};
```

**默认和删除函数**：

```cpp
class NonCopyable {
public:
    NonCopyable() = default;  // 使用默认实现
    NonCopyable(const NonCopyable&) = delete;  // 禁止拷贝
    NonCopyable& operator=(const NonCopyable&) = delete;
};
```

---

## 2. C++14 特性

### 2.1 泛型 Lambda？

**问题描述**：lambda 参数使用 auto。

```cpp
// C++11：必须显式指定类型
auto f1 = [](int x) { return x * 2; };

// C++14：泛型 lambda
auto f2 = [](auto x) { return x * 2; };

f2(10);     // int
f2(3.14);   // double
f2("hello");  // 编译错误（字符串不支持 *）

// 多个 auto 参数
auto add = [](auto a, auto b) { return a + b; };
add(1, 2);       // int
add(1.5, 2.5);   // double
```

---

### 2.2 返回类型推导？

**问题描述**：函数返回类型使用 auto。

```cpp
// C++11：需要尾置返回类型
auto add(int a, int b) -> int {
    return a + b;
}

// C++14：直接使用 auto
auto multiply(int a, int b) {
    return a * b;  // 自动推导为 int
}

// 多个 return 语句必须类型一致
auto func(bool flag) {
    if (flag)
        return 1;    // int
    else
        return 2.0;  // ❌ 错误：类型不一致
}
```

---

### 2.3 `std::make_unique`？

**问题描述**：安全创建 `unique_ptr`。

```cpp
// C++11
std::unique_ptr<int> ptr1(new int(10));

// C++14：推荐方式
auto ptr2 = std::make_unique<int>(10);

// 异常安全
func(std::unique_ptr<int>(new int(10)), may_throw());  // ❌ 可能泄漏
func(std::make_unique<int>(10), may_throw());          // ✅ 安全
```

---

### 2.4 二进制字面量和数字分隔符？

```cpp
// 二进制字面量
int binary = 0b1010'1010;  // 170

// 数字分隔符
int million = 1'000'000;
double pi = 3.141'592'653;
int hex = 0xFF'FF'FF'FF;
```

---

## 3. C++17 特性

### 3.1 结构化绑定？

**问题描述**：解包元组、pair、结构体。

```cpp
// pair
std::pair<int, std::string> get_person() {
    return {25, "Alice"};
}

auto [age, name] = get_person();
std::cout << name << " is " << age << " years old\n";

// map 遍历
std::map<std::string, int> map{{"Alice", 25}, {"Bob", 30}};
for (const auto& [key, value] : map) {
    std::cout << key << ": " << value << "\n";
}

// 数组
int arr[3] = {1, 2, 3};
auto [a, b, c] = arr;

// 结构体
struct Point { int x, y; };
Point p{10, 20};
auto [x, y] = p;
```

---

### 3.2 `if constexpr`？

**问题描述**：编译期条件分支。

```cpp
template<typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer<T>::value) {
        return *t;  // 指针类型：解引用
    } else {
        return t;   // 非指针类型：直接返回
    }
}

int x = 10;
int* p = &x;

auto v1 = get_value(x);   // 10
auto v2 = get_value(p);   // 10

// 区别于普通 if
template<typename T>
void func(T t) {
    if (std::is_pointer<T>::value) {
        *t;  // ❌ 如果 T 不是指针，编译错误
    }

    if constexpr (std::is_pointer<T>::value) {
        *t;  // ✅ 如果 T 不是指针，这个分支不编译
    }
}
```

---

### 3.3 `std::optional`？

**问题描述**：表示可能没有值的类型。

```cpp
#include <optional>

std::optional<int> parse_int(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return std::nullopt;  // 表示没有值
    }
}

void optional_demo() {
    auto result = parse_int("123");

    if (result) {  // 或 result.has_value()
        std::cout << "Parsed: " << *result << "\n";
    } else {
        std::cout << "Parse failed\n";
    }

    // value_or：提供默认值
    int value = result.value_or(0);

    // value：如果没有值会抛异常
    // int v = result.value();  // 可能抛 std::bad_optional_access
}
```

---

### 3.4 `std::variant`？

**问题描述**：类型安全的联合体。

```cpp
#include <variant>

std::variant<int, double, std::string> var;

var = 10;           // 持有 int
var = 3.14;         // 持有 double
var = "hello";      // 持有 string

// 访问
if (std::holds_alternative<int>(var)) {
    int x = std::get<int>(var);
}

// 访问者模式
std::visit([](auto&& arg) {
    std::cout << arg << "\n";
}, var);

// 类型安全的错误处理
std::variant<int, std::string> safe_divide(int a, int b) {
    if (b == 0) {
        return "Division by zero";
    }
    return a / b;
}
```

---

### 3.5 `std::any`？

**问题描述**：类型安全的 `void*`。

```cpp
#include <any>

std::any a = 10;
a = 3.14;
a = std::string("hello");

// 类型检查
if (a.type() == typeid(std::string)) {
    std::string s = std::any_cast<std::string>(a);
}

// 错误的类型转换会抛异常
try {
    int x = std::any_cast<int>(a);  // a 现在是 string
} catch (const std::bad_any_cast& e) {
    std::cout << "Bad cast\n";
}
```

---

### 3.6 `std::string_view`？

**问题描述**：非拥有的字符串视图。

```cpp
#include <string_view>

void print_string(std::string_view sv) {  // 无拷贝
    std::cout << sv << "\n";
}

std::string str = "hello";
print_string(str);        // 从 string
print_string("world");    // 从字面量
print_string(str.c_str());  // 从 C 字符串

// 注意：不拥有数据
std::string_view sv;
{
    std::string temp = "temporary";
    sv = temp;
}
// sv 现在悬空！
```

---

### 3.7 折叠表达式？

**问题描述**：简化变参模板。

```cpp
// C++11：递归
template<typename T>
T sum(T t) {
    return t;
}

template<typename T, typename... Args>
T sum(T first, Args... args) {
    return first + sum(args...);
}

// C++17：折叠表达式
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // 一元右折叠
}

sum(1, 2, 3, 4, 5);  // 15

// 其他折叠形式
template<typename... Args>
void print(Args... args) {
    ((std::cout << args << " "), ...);  // 逗号折叠
}
```

---

### 3.8 其他 C++17 特性

**`inline` 变量**：

```cpp
// header.h
inline int global_var = 10;  // 可以在头文件中定义

inline const std::string greeting = "Hello";
```

**`std::filesystem`**：

```cpp
#include <filesystem>
namespace fs = std::filesystem;

fs::path p = "test.txt";
if (fs::exists(p)) {
    std::cout << "File size: " << fs::file_size(p) << "\n";
}

for (const auto& entry : fs::directory_iterator(".")) {
    std::cout << entry.path() << "\n";
}
```

**类模板参数推导 (CTAD)**：

```cpp
// C++14
std::pair<int, double> p1(1, 3.14);

// C++17
std::pair p2(1, 3.14);  // 自动推导为 pair<int, double>
std::vector v{1, 2, 3};  // vector<int>
```

---

## 4. C++20 特性

### 4.1 Concepts？

**问题描述**：约束模板参数。

```cpp
#include <concepts>

// 定义 concept
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// 使用 concept
template<Arithmetic T>
T add(T a, T b) {
    return a + b;
}

add(1, 2);       // ✅
add(1.5, 2.5);   // ✅
// add("a", "b");  // ❌ 编译错误，清晰的错误信息

// requires 子句
template<typename T>
    requires std::integral<T>
T multiply(T a, T b) {
    return a * b;
}
```

---

### 4.2 Ranges？

**问题描述**：组合式的容器操作。

```cpp
#include <ranges>
namespace views = std::views;

std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 过滤 + 变换 + 取前 5 个
auto result = vec
    | views::filter([](int x) { return x % 2 == 0; })
    | views::transform([](int x) { return x * x; })
    | views::take(5);

for (int x : result) {
    std::cout << x << " ";  // 4 16 36 64 100
}

// 惰性求值
auto infinite = views::iota(1)  // 无限序列：1, 2, 3, ...
    | views::filter([](int x) { return x % 2 == 0; })
    | views::take(10);  // 只取前 10 个偶数
```

---

### 4.3 协程 (Coroutines)？

**问题描述**：暂停和恢复执行的函数。

```cpp
#include <coroutine>

// 简单的生成器
template<typename T>
struct Generator {
    struct promise_type {
        T current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }

        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
    };

    std::coroutine_handle<promise_type> handle;

    ~Generator() {
        if (handle) handle.destroy();
    }

    bool next() {
        handle.resume();
        return !handle.done();
    }

    T value() const {
        return handle.promise().current_value;
    }
};

// 使用协程
Generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;  // 暂停并返回值
        auto temp = a;
        a = b;
        b = temp + b;
    }
}

void coroutine_demo() {
    auto gen = fibonacci();
    for (int i = 0; i < 10; ++i) {
        gen.next();
        std::cout << gen.value() << " ";
    }
}
```

---

### 4.4 Modules？

**问题描述**：替代头文件，加速编译。

```cpp
// math.cppm（模块接口）
export module math;

export int add(int a, int b) {
    return a + b;
}

export int multiply(int a, int b) {
    return a * b;
}

// main.cpp（使用模块）
import math;

int main() {
    int sum = add(3, 4);
    int product = multiply(3, 4);
}

// 优点：
// - 更快的编译速度
// - 更好的封装
// - 避免宏污染
```

---

### 4.5 三路比较运算符 (Spaceship Operator)？

**问题描述**：简化比较运算符的实现。

```cpp
#include <compare>

struct Point {
    int x, y;

    // 默认三路比较
    auto operator<=>(const Point&) const = default;

    // 自动生成 ==, !=, <, <=, >, >=
};

Point p1{1, 2}, p2{3, 4};
auto result = p1 <=> p2;  // std::strong_ordering

if (result < 0) {
    std::cout << "p1 < p2\n";
} else if (result > 0) {
    std::cout << "p1 > p2\n";
} else {
    std::cout << "p1 == p2\n";
}

// 自定义比较
struct Person {
    std::string name;
    int age;

    auto operator<=>(const Person& other) const {
        if (auto cmp = name <=> other.name; cmp != 0)
            return cmp;
        return age <=> other.age;
    }
};
```

---

### 4.6 其他 C++20 特性

**`consteval`**：强制编译期求值

```cpp
consteval int square(int n) {
    return n * n;
}

constexpr int x = square(5);  // ✅ 编译期
int y = square(5);            // ✅ 编译期
// int z = square(y);         // ❌ 错误：y 不是编译期常量
```

**`constinit`**：保证编译期初始化

```cpp
constinit int global_var = 42;  // 保证编译期初始化
```

**`std::span`**：数组视图

```cpp
#include <span>

void print_array(std::span<const int> arr) {
    for (int x : arr) {
        std::cout << x << " ";
    }
}

int arr[5] = {1, 2, 3, 4, 5};
std::vector<int> vec = {6, 7, 8};

print_array(arr);  // C 数组
print_array(vec);  // vector
```

**`std::format`**：类型安全的格式化

```cpp
#include <format>

std::string msg = std::format("Hello, {}! You are {} years old.", "Alice", 25);
// "Hello, Alice! You are 25 years old."
```

---

## 5. C++23 特性

### 5.1 `std::expected`？

**问题描述**：类型安全的错误处理。

```cpp
#include <expected>

std::expected<int, std::string> safe_divide(int a, int b) {
    if (b == 0) {
        return std::unexpected("Division by zero");
    }
    return a / b;
}

void expected_demo() {
    auto result = safe_divide(10, 2);

    if (result) {
        std::cout << "Result: " << *result << "\n";
    } else {
        std::cout << "Error: " << result.error() << "\n";
    }
}
```

---

### 5.2 `std::print`？

```cpp
#include <print>

std::print("Hello, {}!\n", "World");
std::println("Value: {}", 42);
```

---

### 5.3 多维下标运算符？

```cpp
struct Matrix {
    int operator[](int i, int j) {  // C++23：多维下标
        return data[i][j];
    }
};

Matrix m;
int value = m[2, 3];  // 二维索引
```

---

### 5.4 其他 C++23 特性

- **`if consteval`**：检查编译期上下文
- **Deducing this**：简化 CRTP
- **`std::stacktrace`**：获取调用栈
- **Ranges 改进**：更多算法和视图

---

## 6. 特性总结

### 各版本核心特性速查

| 版本 | 核心特性 |
|------|---------|
| **C++11** | auto、lambda、右值引用、智能指针、nullptr、range-for、constexpr |
| **C++14** | 泛型 lambda、返回类型推导、`make_unique`、二进制字面量 |
| **C++17** | 结构化绑定、`if constexpr`、`optional`、`variant`、`string_view`、折叠表达式 |
| **C++20** | Concepts、Ranges、协程、Modules、三路比较、`span`、`format` |
| **C++23** | `expected`、`print`、多维下标、`if consteval`、Ranges 改进 |

### 最常用特性（按使用频率）

1. **auto** - 类型推导
2. **range-for** - 简化遍历
3. **lambda** - 匿名函数
4. **智能指针** - 自动内存管理
5. **移动语义** - 性能优化
6. **结构化绑定** (C++17) - 解包
7. **`optional`** (C++17) - 可选值
8. **Concepts** (C++20) - 类型约束
9. **Ranges** (C++20) - 组合式操作

### 学习建议

**初学者必学**：
- auto、lambda、range-for
- 智能指针（`unique_ptr`、`shared_ptr`）
- nullptr
- 初始化列表

**进阶必学**：
- 右值引用和移动语义
- 完美转发
- 结构化绑定
- `optional` 和 `variant`

**高级特性**：
- Concepts
- Ranges
- 协程
- Modules

### 迁移建议

**从 C++98/03 到 C++11**：
1. 用 `auto` 替代冗长类型
2. 用 `nullptr` 替代 NULL
3. 用智能指针替代裸指针
4. 用 range-for 替代迭代器循环
5. 用 lambda 替代函数对象

**从 C++11 到 C++17**：
1. 用结构化绑定简化代码
2. 用 `if constexpr` 替代 SFINAE
3. 用 `optional` 替代 nullable 指针
4. 用 `string_view` 优化字符串传递

**从 C++17 到 C++20**：
1. 用 Concepts 替代 SFINAE
2. 用 Ranges 简化算法
3. 用协程简化异步代码
4. 用 Modules 加速编译

---

## 总结

**C++ 现代化的核心价值**：
- **更安全**：智能指针、RAII、类型安全
- **更简洁**：auto、lambda、range-for、结构化绑定
- **更高效**：移动语义、编译期计算、零开销抽象
- **更易维护**：Concepts、Modules、清晰的错误信息

**关键原则**：
1. **优先使用现代特性**：不要停留在 C++98
2. **逐步迁移**：不必一次性使用所有新特性
3. **根据项目需求**：选择合适的 C++ 标准版本
4. **持续学习**：C++ 仍在快速发展

现代 C++ 让代码更安全、更简洁、更高效。掌握这些特性是成为 C++ 高手的必经之路！
