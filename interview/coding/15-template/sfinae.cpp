/**
 * @file sfinae.cpp
 * @brief SFINAE - 面试者实现文件
 */

#include "sfinae.h"
#include <vector>
#include <set>

namespace SfinaeImpl {

// ==================== 题目3: 根据类型选择实现 ====================

// 方法1: enable_if (需要两个重载)
template <typename T>
std::enable_if_t<std::is_integral_v<T>, T>
multiply_by_two_enable_if_impl(T value) {
    // TODO: 整数类型使用位运算
    return value * 2;
}

template <typename T>
std::enable_if_t<!std::is_integral_v<T>, T>
multiply_by_two_enable_if_impl(T value) {
    // TODO: 其他类型使用普通运算
    return value * 2;
}

template <typename T>
T multiply_by_two_enable_if(T value) {
    return multiply_by_two_enable_if_impl(value);
}

// 方法2: if constexpr (C++17)
template <typename T>
T multiply_by_two_if_constexpr(T value) {
    // TODO: 使用 if constexpr 实现
    return value * 2;
}

// 方法3: tag dispatch
template <typename T>
T multiply_impl(T value, std::true_type /* is_integral */) {
    // TODO: 整数类型使用位运算
    return value * 2;
}

template <typename T>
T multiply_impl(T value, std::false_type /* is_integral */) {
    // TODO: 其他类型使用普通运算
    return value * 2;
}

template <typename T>
T multiply_by_two_tag_dispatch(T value) {
    return multiply_impl(value, std::is_integral<T>{});
}

// ==================== 题目7: SFINAE 函数重载 ====================

// 有 push_back 的容器
template <typename C, typename V>
auto insert_element_impl(C& container, V&& value, int)
    -> decltype(container.push_back(std::forward<V>(value)), void())
{
    // TODO: 使用 push_back
}

// 有 insert 的容器
template <typename C, typename V>
auto insert_element_impl(C& container, V&& value, long)
    -> decltype(container.insert(std::forward<V>(value)), void())
{
    // TODO: 使用 insert
}

template <typename C, typename V>
void insert_element(C& container, V&& value) {
    insert_element_impl(container, std::forward<V>(value), 0);
}

// 显式模板实例化
template int multiply_by_two_enable_if<int>(int);
template double multiply_by_two_enable_if<double>(double);
template int multiply_by_two_if_constexpr<int>(int);
template double multiply_by_two_if_constexpr<double>(double);
template int multiply_by_two_tag_dispatch<int>(int);
template double multiply_by_two_tag_dispatch<double>(double);
template void insert_element<std::vector<int>, int>(std::vector<int>&, int&&);
template void insert_element<std::set<int>, int>(std::set<int>&, int&&);

} // namespace SfinaeImpl
