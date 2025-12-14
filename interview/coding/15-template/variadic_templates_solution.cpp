/**
 * @file variadic_templates_solution.cpp
 * @brief 变参模板 - 参考答案
 */

#include "variadic_templates.h"
#include <cassert>

namespace VariadicTemplatesImpl {

namespace Solution {

// ==================== 题目1: print 函数 ====================

// 方法1: 递归展开
void print_v1() {
    std::cout << "\n";
}

template <typename T, typename... Args>
void print_v1(T first, Args... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ", ";
    }
    print_v1(rest...);
}

// 方法2: 折叠表达式 (C++17)
template <typename... Args>
void print_v2(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

// 方法3: 逗号运算符技巧 (C++11)
template <typename... Args>
void print_v3(Args... args) {
    int dummy[] = { (std::cout << args << " ", 0)... };
    (void)dummy;
    std::cout << "\n";
}

// ==================== 题目2: sum 函数 ====================

// 方法1: 递归
template <typename T>
T sum_v1(T value) {
    return value;
}

template <typename T, typename... Args>
auto sum_v1(T first, Args... rest) {
    return first + sum_v1(rest...);
}

// 方法2: 折叠表达式 (C++17)
template <typename... Args>
auto sum_v2(Args... args) {
    return (args + ...);
}

// 方法3: 带初始值的折叠表达式
template <typename... Args>
auto sum_v3(Args... args) {
    return (0 + ... + args);  // 处理空参数列表
}

// ==================== 题目3: count_args ====================

template <typename... Args>
constexpr std::size_t count_args(Args...) {
    return sizeof...(Args);
}

template <typename... Args>
constexpr std::size_t count_types() {
    return sizeof...(Args);
}

// ==================== 题目4: make_tuple ====================

template <typename... Args>
auto my_make_tuple(Args&&... args) {
    return std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
}

// ==================== 题目5: apply ====================

template <typename F, typename Tuple, std::size_t... I>
auto apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple>
auto my_apply(F&& f, Tuple&& t) {
    constexpr auto size = std::tuple_size_v<std::decay_t<Tuple>>;
    return apply_impl(
        std::forward<F>(f),
        std::forward<Tuple>(t),
        std::make_index_sequence<size>{}
    );
}

// ==================== 题目6: all_same ====================

template <typename...>
struct all_same : std::true_type {};

template <typename T>
struct all_same<T> : std::true_type {};

template <typename T, typename U, typename... Rest>
struct all_same<T, U, Rest...>
    : std::conditional_t<std::is_same_v<T, U>, all_same<T, Rest...>, std::false_type> {};

template <typename... Types>
constexpr bool all_same_v = all_same<Types...>::value;

// ==================== 题目7: contains_type ====================

template <typename T, typename... List>
struct contains_type : std::false_type {};

template <typename T, typename First, typename... Rest>
struct contains_type<T, First, Rest...>
    : std::conditional_t<std::is_same_v<T, First>, std::true_type, contains_type<T, Rest...>> {};

template <typename T, typename... List>
constexpr bool contains_type_v = contains_type<T, List...>::value;

// ==================== 题目8: type_at ====================

template <std::size_t I, typename T, typename... Rest>
struct type_at_impl {
    using type = typename type_at_impl<I - 1, Rest...>::type;
};

template <typename T, typename... Rest>
struct type_at_impl<0, T, Rest...> {
    using type = T;
};

template <std::size_t I, typename... Types>
struct type_at {
    static_assert(I < sizeof...(Types), "Index out of bounds");
    using type = typename type_at_impl<I, Types...>::type;
};

template <std::size_t I, typename... Types>
using type_at_t = typename type_at<I, Types...>::type;

// ==================== 更多变参模板工具 ====================

// push_front: 在类型列表前添加类型
template <typename T, typename Tuple>
struct push_front;

template <typename T, typename... Types>
struct push_front<T, std::tuple<Types...>> {
    using type = std::tuple<T, Types...>;
};

// push_back: 在类型列表后添加类型
template <typename T, typename Tuple>
struct push_back;

template <typename T, typename... Types>
struct push_back<T, std::tuple<Types...>> {
    using type = std::tuple<Types..., T>;
};

// ==================== 折叠表达式示例 (C++17) ====================

template <typename... Args>
constexpr bool all_true(Args... args) {
    return (... && args);
}

template <typename... Args>
constexpr bool any_true(Args... args) {
    return (... || args);
}

template <typename... Args>
void print_with_separator(const char* sep, Args... args) {
    bool first = true;
    ((std::cout << (first ? "" : sep) << args, first = false), ...);
    std::cout << "\n";
}

// ==================== 测试辅助函数 ====================

int add(int a, int b, int c) {
    return a + b + c;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Variadic Templates Tests ===" << std::endl;

    // sum
    assert(Solution::sum_v2(1, 2, 3, 4, 5) == 15);
    assert(Solution::sum_v1(1, 2, 3) == 6);
    std::cout << "  sum: PASSED" << std::endl;

    // count_args
    static_assert(Solution::count_args(1, 2, 3) == 3, "");
    static_assert(Solution::count_types<int, double, char>() == 3, "");
    std::cout << "  count_args: PASSED" << std::endl;

    // make_tuple
    auto t = Solution::my_make_tuple(1, 2.5, std::string("hello"));
    assert(std::get<0>(t) == 1);
    assert(std::get<1>(t) == 2.5);
    assert(std::get<2>(t) == "hello");
    std::cout << "  make_tuple: PASSED" << std::endl;

    // apply
    auto result = Solution::my_apply(Solution::add, std::make_tuple(1, 2, 3));
    assert(result == 6);
    std::cout << "  apply: PASSED" << std::endl;

    // all_same
    static_assert(Solution::all_same_v<int, int, int>, "");
    static_assert(!Solution::all_same_v<int, int, double>, "");
    std::cout << "  all_same: PASSED" << std::endl;

    // contains_type
    static_assert(Solution::contains_type_v<int, char, int, double>, "");
    static_assert(!Solution::contains_type_v<float, char, int, double>, "");
    std::cout << "  contains_type: PASSED" << std::endl;

    // type_at
    static_assert(std::is_same_v<Solution::type_at_t<0, int, double, char>, int>, "");
    static_assert(std::is_same_v<Solution::type_at_t<2, int, double, char>, char>, "");
    std::cout << "  type_at: PASSED" << std::endl;

    // Fold expressions
    static_assert(Solution::all_true(true, true, true), "");
    static_assert(!Solution::all_true(true, false, true), "");
    static_assert(Solution::any_true(false, true, false), "");
    std::cout << "  fold expressions: PASSED" << std::endl;
}

} // namespace VariadicTemplatesImpl

/**
 * 关键要点：
 *
 * 1. 变参模板基础：
 *    - typename... Args 声明类型参数包
 *    - Args... args 声明函数参数包
 *    - sizeof...(Args) 获取参数个数
 *
 * 2. 参数包展开方式：
 *    a) 递归展开：逐个处理，需要终止条件
 *    b) 折叠表达式 (C++17)：更简洁
 *    c) 逗号运算符技巧：C++11 兼容方案
 *    d) 初始化列表展开
 *
 * 3. 折叠表达式 (C++17)：
 *    - (... op pack): 左折叠
 *    - (pack op ...): 右折叠
 *    - (init op ... op pack): 带初始值左折叠
 *    - (pack op ... op init): 带初始值右折叠
 *
 * 4. 索引序列：
 *    - std::index_sequence<0, 1, 2, ...>
 *    - std::make_index_sequence<N>
 *    - 用于 tuple 展开等场景
 *
 * 5. 完美转发：
 *    - Args&&... args
 *    - std::forward<Args>(args)...
 *    - 保持值类别
 *
 * 6. 类型列表操作：
 *    - 使用 std::tuple 作为类型列表容器
 *    - 递归特化实现各种操作
 *    - push_front, push_back, reverse, transform, filter 等
 */
