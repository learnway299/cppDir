/**
 * @file variadic_templates.cpp
 * @brief 变参模板 - 面试者实现文件
 */

#include "variadic_templates.h"

namespace VariadicTemplatesImpl {

// ==================== 题目1: print 函数 ====================

// 递归终止条件
void print_recursive() {
    std::cout << "\n";
}

template <typename T, typename... Args>
void print_recursive(T first, Args... rest) {
    // TODO: 打印 first，然后递归调用
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ", ";
    }
    print_recursive(rest...);
}

// 折叠表达式版本 (C++17)
template <typename... Args>
void print_fold(Args... args) {
    // TODO: 使用折叠表达式打印所有参数
}

// ==================== 题目2: sum 函数 ====================

template <typename T>
T sum_recursive(T value) {
    return value;
}

template <typename T, typename... Args>
auto sum_recursive(T first, Args... rest) {
    // TODO: 递归求和
    return 0;
}

template <typename... Args>
auto sum_fold(Args... args) {
    // TODO: 使用折叠表达式求和
    return 0;
}

// ==================== 题目3: count_args ====================

template <typename... Args>
constexpr std::size_t count_args(Args...) {
    // TODO: 使用 sizeof...
    return 0;
}

template <typename... Args>
constexpr std::size_t count_types() {
    // TODO: 使用 sizeof...
    return 0;
}

// ==================== 题目4: make_tuple ====================

template <typename... Args>
auto my_make_tuple(Args&&... args) {
    // TODO: 返回 std::tuple
    return std::tuple<>();
}

// ==================== 题目5: apply ====================

template <typename F, typename Tuple, std::size_t... I>
auto apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
    // TODO: 展开 tuple 调用函数
    return std::forward<F>(f)();
}

template <typename F, typename Tuple>
auto my_apply(F&& f, Tuple&& t) {
    // TODO: 创建 index_sequence 并调用 apply_impl
    return 0;
}

// ==================== 折叠表达式工具 ====================

template <typename... Args>
constexpr bool all_true(Args... args) {
    // TODO: 使用折叠表达式
    return true;
}

template <typename... Args>
constexpr bool any_true(Args... args) {
    // TODO: 使用折叠表达式
    return false;
}

template <typename... Args>
void print_with_separator(const char* sep, Args... args) {
    // TODO: 使用折叠表达式打印带分隔符
}

// 显式模板实例化
template void print_recursive<int>(int);
template void print_recursive<int, int, int>(int, int, int);
template int sum_recursive<int>(int);
template auto sum_recursive<int, int, int>(int, int, int);

} // namespace VariadicTemplatesImpl
