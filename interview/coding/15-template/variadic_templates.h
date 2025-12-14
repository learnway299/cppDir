/**
 * @file variadic_templates.h
 * @brief 变参模板 - 题目说明与声明
 *
 * 变参模板 (Variadic Templates) 允许模板接受任意数量的类型参数或值参数。
 *
 * 包含题目：
 * 1. print - 打印任意数量的参数
 * 2. sum - 计算任意数量参数的和
 * 3. count_args - 返回参数个数
 * 4. make_tuple - 创建 tuple
 * 5. apply - 将 tuple 展开为函数参数
 * 6. all_same - 检查所有类型是否相同
 * 7. contains_type - 检查类型列表是否包含指定类型
 * 8. type_at - 获取类型列表中指定索引的类型
 */

#ifndef VARIADIC_TEMPLATES_H
#define VARIADIC_TEMPLATES_H

#include <iostream>
#include <tuple>
#include <utility>
#include <type_traits>

namespace VariadicTemplatesImpl {

// ==================== 题目1: print 函数 ====================

/**
 * 题目1: 实现 print 函数，打印任意数量的参数
 *
 * 示例：
 *   print(1, 2.5, "hello");  // 输出: 1, 2.5, hello
 */
void print_recursive();

template <typename T, typename... Args>
void print_recursive(T first, Args... rest);

template <typename... Args>
void print_fold(Args... args);

// ==================== 题目2: sum 函数 ====================

/**
 * 题目2: 实现 sum 函数，计算任意数量参数的和
 *
 * 示例：
 *   sum(1, 2, 3, 4, 5)  // 返回 15
 */
template <typename T>
T sum_recursive(T value);

template <typename T, typename... Args>
auto sum_recursive(T first, Args... rest);

template <typename... Args>
auto sum_fold(Args... args);

// ==================== 题目3: count_args ====================

/**
 * 题目3: 实现 count_args，返回参数个数
 *
 * 示例：
 *   count_args(1, 2, 3)  // 返回 3
 */
template <typename... Args>
constexpr std::size_t count_args(Args...);

template <typename... Args>
constexpr std::size_t count_types();

// ==================== 题目4: make_tuple ====================

/**
 * 题目4: 实现 make_tuple（简化版）
 *
 * 示例：
 *   auto t = my_make_tuple(1, 2.5, "hello");
 */
template <typename... Args>
auto my_make_tuple(Args&&... args);

// ==================== 题目5: apply ====================

/**
 * 题目5: 实现 apply - 将 tuple 展开为函数参数
 *
 * 示例：
 *   int add(int a, int b, int c) { return a + b + c; }
 *   auto t = std::make_tuple(1, 2, 3);
 *   my_apply(add, t);  // 返回 6
 */
template <typename F, typename Tuple, std::size_t... I>
auto apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>);

template <typename F, typename Tuple>
auto my_apply(F&& f, Tuple&& t);

// ==================== 题目6: all_same ====================

/**
 * 题目6: 实现 all_same - 检查所有类型是否相同
 *
 * 示例：
 *   all_same_v<int, int, int>     // true
 *   all_same_v<int, int, double>  // false
 */
template <typename...>
struct all_same : std::true_type {};

// TODO: 添加特化版本

template <typename... Types>
constexpr bool all_same_v = all_same<Types...>::value;

// ==================== 题目7: contains_type ====================

/**
 * 题目7: 实现 contains_type - 检查类型列表中是否包含指定类型
 *
 * 示例：
 *   contains_type_v<int, char, int, double>   // true
 *   contains_type_v<float, char, int, double> // false
 */
template <typename T, typename... List>
struct contains_type : std::false_type {};

// TODO: 添加特化版本

template <typename T, typename... List>
constexpr bool contains_type_v = contains_type<T, List...>::value;

// ==================== 题目8: type_at ====================

/**
 * 题目8: 实现 type_at - 获取类型列表中指定索引的类型
 *
 * 示例：
 *   type_at_t<0, int, double, char>  // int
 *   type_at_t<2, int, double, char>  // char
 */
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

// ==================== 折叠表达式工具 (C++17) ====================

template <typename... Args>
constexpr bool all_true(Args... args);

template <typename... Args>
constexpr bool any_true(Args... args);

template <typename... Args>
void print_with_separator(const char* sep, Args... args);

// ==================== 测试函数 ====================

void runTests();

} // namespace VariadicTemplatesImpl

#endif // VARIADIC_TEMPLATES_H
