# 25 - 反射与元编程 (Reflection and Metaprogramming)

## 简介

C++ 不像 Java/C# 那样原生支持反射，但可以通过模板元编程、宏、第三方库实现类似功能。C++26 可能引入静态反射。

**核心概念**：
- **模板元编程**：编译期计算
- **类型萃取**：`std::is_*`、`std::enable_if`
- **RTTI**：`typeid`、`dynamic_cast`
- **手动反射**：宏、代码生成

---

## 1. RTTI（运行时类型信息）

### 1.1 typeid？

```cpp
#include <typeinfo>
#include <iostream>

class Base {
    virtual ~Base() = default;  // 必须有虚函数
};

class Derived : public Base {};

void typeid_demo() {
    Base* ptr = new Derived();

    // 获取类型信息
    const std::type_info& ti = typeid(*ptr);

    std::cout << "类型名: " << ti.name() << "\n";  // 编译器相关
    std::cout << "哈希码: " << ti.hash_code() << "\n";

    // 类型比较
    if (typeid(*ptr) == typeid(Derived)) {
        std::cout << "是 Derived 类型\n";
    }

    delete ptr;
}
```

**注意事项**：
- 需要启用 RTTI（编译选项 `-frtti`，默认开启）
- `type_info::name()` 返回的字符串是实现定义的（GCC 返回 mangled name）
- 只能用于多态类型（有虚函数）

### 1.2 dynamic_cast？

```cpp
Base* ptr = new Derived();

// 向下转换
Derived* derived = dynamic_cast<Derived*>(ptr);
if (derived) {
    std::cout << "转换成功\n";
} else {
    std::cout << "转换失败\n";
}

// 引用转换（失败会抛异常）
try {
    Derived& ref = dynamic_cast<Derived&>(*ptr);
} catch (const std::bad_cast& e) {
    std::cout << "转换失败: " << e.what() << "\n";
}

delete ptr;
```

---

## 2. 模板元编程基础

### 2.1 编译期计算？

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

int main() {
    constexpr int fact5 = Factorial<5>::value;  // 120（编译期计算）
}
```

**C++11 constexpr**：
```cpp
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int fact5 = factorial(5);  // 编译期计算
```

### 2.2 类型列表（Type List）？

```cpp
// 类型列表
template<typename... Types>
struct TypeList {};

// 获取列表长度
template<typename List>
struct Length;

template<typename... Types>
struct Length<TypeList<Types...>> {
    static constexpr size_t value = sizeof...(Types);
};

// 使用
using MyTypes = TypeList<int, double, std::string>;
constexpr size_t len = Length<MyTypes>::value;  // 3
```

### 2.3 类型选择（Type Selection）？

```cpp
// 条件类型选择
template<bool Condition, typename TrueType, typename FalseType>
struct Conditional {
    using type = TrueType;
};

template<typename TrueType, typename FalseType>
struct Conditional<false, TrueType, FalseType> {
    using type = FalseType;
};

// 使用（类似 std::conditional）
using Type = Conditional<sizeof(int) == 4, int, long>::type;  // int
```

---

## 3. 类型萃取（Type Traits）

### 3.1 标准类型萃取？

```cpp
#include <type_traits>

// 检查类型属性
static_assert(std::is_integral<int>::value);
static_assert(std::is_pointer<int*>::value);
static_assert(std::is_class<std::string>::value);

// 类型变换
using NoCV = std::remove_cv<const volatile int>::type;  // int
using NoRef = std::remove_reference<int&>::type;        // int
using Ptr = std::add_pointer<int>::type;                // int*

// C++14: _t 和 _v 简写
using NoCV = std::remove_cv_t<const volatile int>;  // int
constexpr bool is_int = std::is_integral_v<int>;    // true
```

### 3.2 常用类型萃取？

| 萃取 | 说明 | 示例 |
|------|------|------|
| `is_integral` | 是否为整数类型 | `is_integral_v<int>` → `true` |
| `is_floating_point` | 是否为浮点类型 | `is_floating_point_v<double>` → `true` |
| `is_pointer` | 是否为指针 | `is_pointer_v<int*>` → `true` |
| `is_reference` | 是否为引用 | `is_reference_v<int&>` → `true` |
| `is_const` | 是否为 const | `is_const_v<const int>` → `true` |
| `is_class` | 是否为类 | `is_class_v<std::string>` → `true` |
| `is_base_of` | 是否为基类 | `is_base_of_v<Base, Derived>` → `true` |
| `is_same` | 类型是否相同 | `is_same_v<int, int>` → `true` |

### 3.3 SFINAE（Substitution Failure Is Not An Error）？

```cpp
// 检测成员函数是否存在
template<typename T>
auto has_size(int) -> decltype(std::declval<T>().size(), std::true_type{});

template<typename T>
std::false_type has_size(...);

template<typename T>
constexpr bool has_size_v = decltype(has_size<T>(0))::value;

// 使用
static_assert(has_size_v<std::vector<int>>);   // true
static_assert(!has_size_v<int>);               // false
```

**C++17：if constexpr**：
```cpp
template<typename T>
void print(const T& value) {
    if constexpr (has_size_v<T>) {
        std::cout << "容器大小: " << value.size() << "\n";
    } else {
        std::cout << "值: " << value << "\n";
    }
}
```

---

## 4. C++20 Concepts

### 4.1 定义 Concept？

```cpp
#include <concepts>

// 自定义 concept
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept HasSize = requires(T t) {
    { t.size() } -> std::convertible_to<size_t>;
};

// 使用
template<Numeric T>
T add(T a, T b) {
    return a + b;
}

template<HasSize Container>
void print_size(const Container& c) {
    std::cout << "大小: " << c.size() << "\n";
}
```

### 4.2 标准 Concepts？

```cpp
#include <concepts>

// 标准 concepts
template<std::integral T>
T multiply(T a, T b) {
    return a * b;
}

template<std::copyable T>
void process(T value) {
    T copy = value;
}

template<std::ranges::range R>
void print_range(const R& r) {
    for (const auto& elem : r) {
        std::cout << elem << " ";
    }
}
```

---

## 5. 手动实现反射

### 5.1 使用宏注册类信息？

```cpp
#include <string>
#include <map>
#include <functional>

class ReflectionRegistry {
public:
    using Creator = std::function<void*()>;

    static void register_class(const std::string& name, Creator creator) {
        registry()[name] = creator;
    }

    static void* create(const std::string& name) {
        auto it = registry().find(name);
        if (it != registry().end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    static std::map<std::string, Creator>& registry() {
        static std::map<std::string, Creator> reg;
        return reg;
    }
};

// 注册宏
#define REGISTER_CLASS(ClassName) \
    static struct ClassName##_Registrar { \
        ClassName##_Registrar() { \
            ReflectionRegistry::register_class(#ClassName, []() -> void* { \
                return new ClassName(); \
            }); \
        } \
    } ClassName##_registrar_instance;

// 使用
class MyClass {
public:
    void hello() { std::cout << "Hello from MyClass\n"; }
};

REGISTER_CLASS(MyClass)

int main() {
    void* ptr = ReflectionRegistry::create("MyClass");
    MyClass* obj = static_cast<MyClass*>(ptr);
    obj->hello();
    delete obj;
}
```

### 5.2 成员变量反射？

```cpp
#include <string_view>
#include <tuple>

// 使用 structured binding 实现简单反射
struct Person {
    std::string name;
    int age;
    double salary;
};

template<typename T>
constexpr auto reflect_members(T& obj) {
    auto& [a, b, c] = obj;
    return std::tie(a, b, c);
}

void print_person(Person& p) {
    auto members = reflect_members(p);

    std::cout << "Name: " << std::get<0>(members) << "\n";
    std::cout << "Age: " << std::get<1>(members) << "\n";
    std::cout << "Salary: " << std::get<2>(members) << "\n";
}
```

---

## 6. 第三方反射库

### 6.1 Boost.Fusion？

```cpp
#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/include/for_each.hpp>

struct Person {
    std::string name;
    int age;
};

BOOST_FUSION_ADAPT_STRUCT(
    Person,
    (std::string, name)
    (int, age)
)

void print_person(const Person& p) {
    boost::fusion::for_each(p, [](const auto& field) {
        std::cout << field << "\n";
    });
}
```

### 6.2 rttr（Run Time Type Reflection）？

```cpp
#include <rttr/registration>

struct Person {
    std::string name;
    int age;

    void say_hello() {
        std::cout << "Hello, I'm " << name << "\n";
    }
};

RTTR_REGISTRATION {
    rttr::registration::class_<Person>("Person")
        .constructor<>()
        .property("name", &Person::name)
        .property("age", &Person::age)
        .method("say_hello", &Person::say_hello);
}

// 使用反射
rttr::type t = rttr::type::get<Person>();
for (auto& prop : t.get_properties()) {
    std::cout << "属性: " << prop.get_name() << "\n";
}

// 动态创建对象
rttr::variant var = t.create();
Person* p = var.get_value<Person*>();
```

### 6.3 Ponder？

```cpp
#include <ponder/ponder.hpp>

struct Person {
    std::string name;
    int age;
};

PONDER_TYPE(Person)

void declare_person() {
    ponder::Class::declare<Person>("Person")
        .property("name", &Person::name)
        .property("age", &Person::age);
}

// 使用
const ponder::Class& metaclass = ponder::classByType<Person>();
Person p{"Alice", 30};

ponder::UserObject obj(p);
obj.set("name", "Bob");
std::string name = obj.get("name").to<std::string>();
```

---

## 7. 序列化应用

### 7.1 JSON 序列化（使用宏）？

```cpp
#include <nlohmann/json.hpp>

struct Person {
    std::string name;
    int age;
    double salary;
};

// 使用 nlohmann/json 的宏
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Person, name, age, salary)

// 序列化
Person p{"Alice", 30, 50000.0};
nlohmann::json j = p;
std::string json_str = j.dump();

// 反序列化
nlohmann::json j2 = nlohmann::json::parse(json_str);
Person p2 = j2.get<Person>();
```

### 7.2 二进制序列化？

```cpp
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

struct Person {
    std::string name;
    int age;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(name, age);
    }
};

// 序列化
std::ostringstream oss;
{
    cereal::BinaryOutputArchive archive(oss);
    Person p{"Alice", 30};
    archive(p);
}

// 反序列化
std::istringstream iss(oss.str());
{
    cereal::BinaryInputArchive archive(iss);
    Person p;
    archive(p);
}
```

---

## 8. C++26 静态反射（提案）

### 8.1 预计功能？

```cpp
// C++26（提案，语法可能变化）
#include <meta>

struct Person {
    std::string name;
    int age;
};

void print_members() {
    for (auto member : std::meta::members_of(reflexpr(Person))) {
        std::cout << std::meta::name_of(member) << "\n";
    }
}

// 动态访问成员
Person p{"Alice", 30};
for (auto member : std::meta::members_of(reflexpr(Person))) {
    auto value = std::meta::get(p, member);
    // ...
}
```

---

## 9. 元编程应用

### 9.1 编译期字符串处理？

```cpp
template<size_t N>
struct ConstString {
    char data[N];

    constexpr ConstString(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = str[i];
        }
    }

    constexpr size_t length() const {
        return N - 1;
    }
};

// C++20
template<ConstString str>
struct Message {
    static void print() {
        std::cout << str.data << "\n";
    }
};

Message<"Hello, World!">::print();
```

### 9.2 元函数组合？

```cpp
template<typename... Ts>
struct Compose;

template<typename T>
struct Compose<T> {
    using type = T;
};

template<typename First, typename... Rest>
struct Compose<First, Rest...> {
    using type = typename First::template apply<typename Compose<Rest...>::type>;
};

// 使用
template<typename T>
struct AddPointer {
    template<typename U>
    using apply = U*;
};

template<typename T>
struct AddConst {
    template<typename U>
    using apply = const U;
};

using Result = Compose<AddPointer<void>, AddConst<void>>::type;  // const void*
```

---

## 总结

### 反射方案对比

| 方案 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| RTTI | 内置，无需额外代码 | 只支持多态类型，信息有限 | 简单类型判断 |
| 模板元编程 | 编译期，零开销 | 代码复杂，编译慢 | 库开发、类型计算 |
| 宏注册 | 简单易用 | 需手动注册，易出错 | 小型项目 |
| 第三方库 | 功能完整 | 增加依赖 | 需要完整反射的项目 |
| C++26 静态反射 | 标准支持，功能强大 | 尚未发布 | 未来项目 |

### 关键记忆点

1. **RTTI**：`typeid`、`dynamic_cast`（需要虚函数）
2. **类型萃取**：`std::is_*`、`std::remove_*`
3. **SFINAE**：替换失败不是错误
4. **C++17**：`if constexpr`
5. **C++20**：Concepts（类型约束）
6. **手动反射**：宏注册、代码生成
7. **第三方库**：rttr、Ponder、Boost.Fusion
8. **元编程**：模板递归、类型列表

---

## 参考资源

- [C++ Type Traits](https://en.cppreference.com/w/cpp/header/type_traits)
- [C++20 Concepts](https://en.cppreference.com/w/cpp/language/constraints)
- [rttr Library](https://www.rttr.org/)
- [Ponder Library](https://billyquith.github.io/ponder/)
- [C++ Reflection Proposal](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2996r1.html)
