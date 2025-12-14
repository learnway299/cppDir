/**
 * @file sfinae_solution.cpp
 * @brief SFINAE - 参考答案
 */

#include "sfinae.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <set>

namespace SfinaeImpl {

namespace Solution {

// ==================== void_t 实现 ====================

#if __cplusplus < 201703L
template <typename...>
using void_t = void;
#else
template <typename... Ts>
using void_t = std::void_t<Ts...>;
#endif

// ==================== 题目1: 检测 size() 成员函数 ====================

template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, void_t<decltype(std::declval<T>().size())>> : std::true_type {};

template <typename T>
constexpr bool has_size_v = has_size<T>::value;

// ==================== 题目2: 检测可打印类型 ====================

template <typename T, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<T, void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

template <typename T>
constexpr bool is_printable_v = is_printable<T>::value;

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
template <typename T>
T multiply_impl(T value, std::true_type /* is_integral */) {
    return value << 1;
}

template <typename T>
T multiply_impl(T value, std::false_type /* is_integral */) {
    return value * 2;
}

template <typename T>
T multiply_by_two_v3(T value) {
    return multiply_impl(value, std::is_integral<T>{});
}

// ==================== 题目4: 检测成员类型 ====================

template <typename T, typename = void>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, void_t<typename T::value_type>> : std::true_type {};

template <typename T>
constexpr bool has_value_type_v = has_value_type<T>::value;

// ==================== 题目5: 检测成员函数 ====================

template <typename T, typename = void>
struct has_method_begin : std::false_type {};

template <typename T>
struct has_method_begin<T, void_t<decltype(std::declval<T>().begin())>>
    : std::true_type {};

template <typename T>
constexpr bool has_method_begin_v = has_method_begin<T>::value;

template <typename T, typename = void>
struct has_method_end : std::false_type {};

template <typename T>
struct has_method_end<T, void_t<decltype(std::declval<T>().end())>>
    : std::true_type {};

template <typename T>
constexpr bool has_method_end_v = has_method_end<T>::value;

// ==================== 题目6: 检测两类型可相加 ====================

template <typename T, typename U, typename = void>
struct can_add : std::false_type {};

template <typename T, typename U>
struct can_add<T, U, void_t<decltype(std::declval<T>() + std::declval<U>())>>
    : std::true_type {};

template <typename T, typename U>
constexpr bool can_add_v = can_add<T, U>::value;

// 获取加法结果类型
template <typename T, typename U, typename = void>
struct add_result {};

template <typename T, typename U>
struct add_result<T, U, void_t<decltype(std::declval<T>() + std::declval<U>())>> {
    using type = decltype(std::declval<T>() + std::declval<U>());
};

template <typename T, typename U>
using add_result_t = typename add_result<T, U>::type;

// ==================== 题目7: SFINAE 函数重载 ====================

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
    container.push_back(std::forward<V>(value));
}

template <typename C, typename V>
auto insert_element(C& container, V&& value)
    -> std::enable_if_t<!has_push_back<C>::value && has_insert<C>::value>
{
    container.insert(std::forward<V>(value));
}

// ==================== 测试辅助类型 ====================

struct NoSize {};
struct WithSize { int size() { return 0; } };
struct NotPrintable {};

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== SFINAE Tests ===" << std::endl;

    // has_size
    static_assert(Solution::has_size_v<std::vector<int>>, "");
    static_assert(!Solution::has_size_v<Solution::NoSize>, "");
    static_assert(Solution::has_size_v<Solution::WithSize>, "");
    std::cout << "  has_size: PASSED" << std::endl;

    // is_printable
    static_assert(Solution::is_printable_v<int>, "");
    static_assert(Solution::is_printable_v<std::string>, "");
    static_assert(!Solution::is_printable_v<Solution::NotPrintable>, "");
    std::cout << "  is_printable: PASSED" << std::endl;

    // multiply_by_two
    assert(Solution::multiply_by_two_v1(5) == 10);
    assert(Solution::multiply_by_two_v2(5) == 10);
    assert(Solution::multiply_by_two_v3(5) == 10);
    assert(Solution::multiply_by_two_v2(2.5) == 5.0);
    std::cout << "  multiply_by_two: PASSED" << std::endl;

    // has_value_type
    static_assert(Solution::has_value_type_v<std::vector<int>>, "");
    static_assert(!Solution::has_value_type_v<int>, "");
    std::cout << "  has_value_type: PASSED" << std::endl;

    // can_add
    static_assert(Solution::can_add_v<int, double>, "");
    static_assert(Solution::can_add_v<std::string, const char*>, "");
    static_assert(!Solution::can_add_v<std::vector<int>, int>, "");
    std::cout << "  can_add: PASSED" << std::endl;

    // insert_element
    {
        std::vector<int> vec;
        Solution::insert_element(vec, 42);
        assert(vec.size() == 1 && vec[0] == 42);

        std::set<int> s;
        Solution::insert_element(s, 42);
        assert(s.size() == 1 && *s.begin() == 42);
    }
    std::cout << "  insert_element: PASSED" << std::endl;
}

} // namespace SfinaeImpl

/**
 * 关键要点：
 *
 * 1. SFINAE 原理���
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
 */
