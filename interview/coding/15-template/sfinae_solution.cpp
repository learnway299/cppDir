/**
 * @file sfinae_solution.cpp
 * @brief SFINAE - 解答
 */
#include <type_traits>
#include <iostream>
#include <vector>
#include <string>
#include <set>

// ==================== void_t 实现（C++17 之前）====================
#if __cplusplus < 201703L
template <typename...>
using void_t = void;
#else
using std::void_t;
#endif

// ==================== 题目1: 检测 size() 成员函数 ====================
template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, void_t<decltype(std::declval<T>().size())>> : std::true_type {};

template <typename T>
inline constexpr bool has_size_v = has_size<T>::value;

// 更严格的版本：检查 size() 返回类型是否可转换为 size_t
template <typename T, typename = void>
struct has_size_strict : std::false_type {};

template <typename T>
struct has_size_strict<T, std::enable_if_t<
    std::is_convertible<decltype(std::declval<T>().size()), std::size_t>::value
>> : std::true_type {};

// ==================== 题目2: 检测可打印类型 ====================
template <typename T, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<T, void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_printable_v = is_printable<T>::value;

// ==================== 题目3: 根据类型选择实现 ====================
// 方法1: enable_if
template <typename T>
std::enable_if_t<std::is_integral_v<T>, T>
multiply_by_two_v1(T value) {
    return value << 1;  // 位运算
}

template <typename T>
std::enable_if_t<!std::is_integral_v<T>, T>
multiply_by_two_v1(T value) {
    return value * 2;  // 普通运算
}

// 方法2: if constexpr (C++17)
template <typename T>
T multiply_by_two_v2(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value << 1;
    } else {
        return value * 2;
    }
}

// 方法3: tag dispatch
namespace detail {
    template <typename T>
    T multiply_impl(T value, std::true_type /* is_integral */) {
        return value << 1;
    }

    template <typename T>
    T multiply_impl(T value, std::false_type /* is_integral */) {
        return value * 2;
    }
}

template <typename T>
T multiply_by_two_v3(T value) {
    return detail::multiply_impl(value, std::is_integral<T>{});
}

// ==================== 题目4: 检测成员类型 ====================
template <typename T, typename = void>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, void_t<typename T::value_type>> : std::true_type {};

template <typename T>
inline constexpr bool has_value_type_v = has_value_type<T>::value;

// 通用宏来生成检测器
#define DEFINE_HAS_TYPE(TypeName) \
    template <typename T, typename = void> \
    struct has_##TypeName : std::false_type {}; \
    template <typename T> \
    struct has_##TypeName<T, void_t<typename T::TypeName>> : std::true_type {}; \
    template <typename T> \
    inline constexpr bool has_##TypeName##_v = has_##TypeName<T>::value;

DEFINE_HAS_TYPE(iterator)
DEFINE_HAS_TYPE(const_iterator)
DEFINE_HAS_TYPE(size_type)

// ==================== 题目5: 检测成员函数（通用模式）====================
// 检测任意成员函数的宏
#define DEFINE_HAS_METHOD(MethodName) \
    template <typename T, typename = void> \
    struct has_method_##MethodName : std::false_type {}; \
    template <typename T> \
    struct has_method_##MethodName<T, void_t<decltype(std::declval<T>().MethodName())>> \
        : std::true_type {}; \
    template <typename T> \
    inline constexpr bool has_method_##MethodName##_v = has_method_##MethodName<T>::value;

DEFINE_HAS_METHOD(begin)
DEFINE_HAS_METHOD(end)
DEFINE_HAS_METHOD(clear)

// ==================== 题目6: 检测可默认构造 ====================
template <typename T, typename = void>
struct is_default_constructible_impl : std::false_type {};

template <typename T>
struct is_default_constructible_impl<T, void_t<decltype(T())>> : std::true_type {};

// 注意：标准库已有 std::is_default_constructible

// ==================== 题目7: 检测两类型可相加 ====================
template <typename T, typename U, typename = void>
struct can_add : std::false_type {};

template <typename T, typename U>
struct can_add<T, U, void_t<decltype(std::declval<T>() + std::declval<U>())>>
    : std::true_type {};

template <typename T, typename U>
inline constexpr bool can_add_v = can_add<T, U>::value;

// 获取加法结果类型
template <typename T, typename U, typename = void>
struct add_result {};

template <typename T, typename U>
struct add_result<T, U, void_t<decltype(std::declval<T>() + std::declval<U>())>> {
    using type = decltype(std::declval<T>() + std::declval<U>());
};

template <typename T, typename U>
using add_result_t = typename add_result<T, U>::type;

// ==================== 题目8: SFINAE 函数重载 ====================
// 检测 push_back
template <typename C, typename = void>
struct has_push_back : std::false_type {};

template <typename C>
struct has_push_back<C, void_t<decltype(
    std::declval<C>().push_back(std::declval<typename C::value_type>())
)>> : std::true_type {};

// 检测 insert
template <typename C, typename = void>
struct has_insert : std::false_type {};

template <typename C>
struct has_insert<C, void_t<decltype(
    std::declval<C>().insert(std::declval<typename C::value_type>())
)>> : std::true_type {};

// 根据容器特性选择插入方式
template <typename C, typename V>
auto insert_element(C& container, V&& value)
    -> std::enable_if_t<has_push_back<C>::value>
{
    std::cout << "Using push_back\n";
    container.push_back(std::forward<V>(value));
}

template <typename C, typename V>
auto insert_element(C& container, V&& value)
    -> std::enable_if_t<!has_push_back<C>::value && has_insert<C>::value>
{
    std::cout << "Using insert\n";
    container.insert(std::forward<V>(value));
}

// ==================== C++20 concepts 替代方案 ====================
#if __cplusplus >= 202002L
#include <concepts>

template <typename T>
concept HasSize = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept Printable = requires(T t, std::ostream& os) {
    { os << t } -> std::same_as<std::ostream&>;
};

template <typename T, typename U>
concept Addable = requires(T t, U u) {
    t + u;
};
#endif

// ==================== 测试代码 ====================
struct NoSize {};
struct WithSize { int size() { return 0; } };

struct NotPrintable {};

int main() {
    std::cout << "=== SFINAE Tests ===\n\n";

    // has_size
    std::cout << "has_size<std::vector<int>>: " << has_size_v<std::vector<int>> << "\n";
    std::cout << "has_size<NoSize>: " << has_size_v<NoSize> << "\n";
    std::cout << "has_size<WithSize>: " << has_size_v<WithSize> << "\n";

    // is_printable
    std::cout << "\nis_printable<int>: " << is_printable_v<int> << "\n";
    std::cout << "is_printable<std::string>: " << is_printable_v<std::string> << "\n";
    std::cout << "is_printable<NotPrintable>: " << is_printable_v<NotPrintable> << "\n";

    // multiply_by_two
    std::cout << "\nmultiply_by_two(5): " << multiply_by_two_v2(5) << "\n";
    std::cout << "multiply_by_two(2.5): " << multiply_by_two_v2(2.5) << "\n";

    // has_value_type
    std::cout << "\nhas_value_type<std::vector<int>>: " << has_value_type_v<std::vector<int>> << "\n";
    std::cout << "has_value_type<int>: " << has_value_type_v<int> << "\n";

    // can_add
    std::cout << "\ncan_add<int, double>: " << can_add_v<int, double> << "\n";
    std::cout << "can_add<std::string, const char*>: " << can_add_v<std::string, const char*> << "\n";
    std::cout << "can_add<std::vector<int>, int>: " << can_add_v<std::vector<int>, int> << "\n";

    // insert_element
    std::cout << "\n";
    std::vector<int> vec;
    insert_element(vec, 42);

    std::set<int> s;
    insert_element(s, 42);

    return 0;
}

/**
 * 关键要点：
 *
 * 1. SFINAE 原理：
 *    - 模板参数替换失败不是错误
 *    - 只是将该重载从候选集中移除
 *    - 必须是替换失败，不是硬错误
 *
 * 2. void_t 技巧：
 *    - template <typename...> using void_t = void;
 *    - 用于检测表达式是否合法
 *    - 合法时特化匹配，不合法时 SFINAE
 *
 * 3. declval 使用：
 *    - std::declval<T>() 不需要构造对象
 *    - 只用于 decltype 和 sizeof 中
 *    - 可以用于不可构造的类型
 *
 * 4. enable_if 使用：
 *    - 返回类型位置：std::enable_if_t<条件, 返回类型>
 *    - 模板参数位置：typename = std::enable_if_t<条件>
 *    - 函数参数位置：较少使用
 *
 * 5. C++17 if constexpr：
 *    - 编译期条件分支
 *    - 比 enable_if 更直观
 *    - 不需要多个函数重载
 *
 * 6. C++20 concepts：
 *    - 更清晰的约束语法
 *    - 更好的错误信息
 *    - 完全替代 SFINAE
 *
 * 7. 常见陷阱：
 *    - 硬错误 vs SFINAE 错误
 *    - 模板参数推导失败的时机
 *    - enable_if 的多个重载冲突
 */
