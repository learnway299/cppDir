/**
 * @file sfinae.h
 * @brief SFINAE - 题目说明与声明
 *
 * SFINAE (Substitution Failure Is Not An Error) 是 C++ 模板的核心特性，
 * 用于在编译期根据类型特征选择不同的函数重载或模板特化。
 *
 * 包含题目：
 * 1. 检测类型是否有 size() 成员函数
 * 2. 检测类型是否可打印
 * 3. 根据类型选择不同实现
 * 4. 检测成员类型
 * 5. 检测成员函数
 * 6. 检测两类型是否可相加
 * 7. SFINAE 函数重载选择
 */

#ifndef SFINAE_H
#define SFINAE_H

#include <type_traits>
#include <iostream>
#include <cstddef>

namespace SfinaeImpl {

// ==================== void_t 实现 ====================

#if __cplusplus < 201703L
template <typename...>
using void_t = void;
#else
template <typename... Ts>
using void_t = std::void_t<Ts...>;
#endif

// ==================== 题目1: 检测 size() 成员函数 ====================

/**
 * 题目1: 检测类型是否有 size() 成员函数
 *
 * 示例：
 *   has_size_v<std::vector<int>>  // true
 *   has_size_v<int>               // false
 */
template <typename T, typename = void>
struct has_size : std::false_type {};

// TODO: 添加特化版本

template <typename T>
constexpr bool has_size_v = has_size<T>::value;

// ==================== 题目2: 检测可打印类型 ====================

/**
 * 题目2: 检测类型是否可以使用 << 输出到 ostream
 *
 * 示例：
 *   is_printable_v<int>           // true
 *   is_printable_v<std::string>   // true
 *   is_printable_v<CustomClass>   // depends
 */
template <typename T, typename = void>
struct is_printable : std::false_type {};

// TODO: 添加特化版本

template <typename T>
constexpr bool is_printable_v = is_printable<T>::value;

// ==================== 题目3: 根据类型选择实现 ====================

/**
 * 题目3: 根据类型选择不同实现
 *
 * 对于整数类型使用位运算，对于其他类型使用普通运算
 */
template <typename T>
T multiply_by_two_enable_if(T value);

template <typename T>
T multiply_by_two_if_constexpr(T value);

template <typename T>
T multiply_by_two_tag_dispatch(T value);

// ==================== 题目4: 检测成员类型 ====================

/**
 * 题目4: 检测是否有 value_type 成员类型
 *
 * 示例：
 *   has_value_type_v<std::vector<int>>  // true
 *   has_value_type_v<int>               // false
 */
template <typename T, typename = void>
struct has_value_type : std::false_type {};

// TODO: 添加特化版本

template <typename T>
constexpr bool has_value_type_v = has_value_type<T>::value;

// ==================== 题目5: 检测成员函数 ====================

/**
 * 题目5: 检测是否有 begin()/end() 成员函数
 */
template <typename T, typename = void>
struct has_method_begin : std::false_type {};

// TODO: 添加特化版本

template <typename T>
constexpr bool has_method_begin_v = has_method_begin<T>::value;

template <typename T, typename = void>
struct has_method_end : std::false_type {};

// TODO: 添加特化版本

template <typename T>
constexpr bool has_method_end_v = has_method_end<T>::value;

// ==================== 题目6: 检测两类型可相加 ====================

/**
 * 题目6: 检测两个类型是否可以相加
 *
 * 示例：
 *   can_add_v<int, double>              // true
 *   can_add_v<std::string, const char*> // true
 *   can_add_v<std::vector<int>, int>    // false
 */
template <typename T, typename U, typename = void>
struct can_add : std::false_type {};

// TODO: 添加特化版本

template <typename T, typename U>
constexpr bool can_add_v = can_add<T, U>::value;

// 获取加法结果类型
template <typename T, typename U, typename = void>
struct add_result {};

// TODO: 添加特化版本

template <typename T, typename U>
using add_result_t = typename add_result<T, U>::type;

// ==================== 题目7: SFINAE 函数重载 ====================

/**
 * 题目7: 根据容器特性选择插入方式
 *
 * 如果容器有 push_back，使用 push_back
 * 否则如果有 insert，使用 insert
 */
template <typename C, typename = void>
struct has_push_back : std::false_type {};

// TODO: 添加特化版本

template <typename C>
constexpr bool has_push_back_v = has_push_back<C>::value;

template <typename C, typename = void>
struct has_insert : std::false_type {};

// TODO: 添加特化版本

template <typename C>
constexpr bool has_insert_v = has_insert<C>::value;

// 根据容器特性选择插入方式
template <typename C, typename V>
void insert_element(C& container, V&& value);

// ==================== 测试辅助类型 ====================

struct NoSize {};

struct WithSize {
    int size() { return 0; }
};

struct NotPrintable {};

// ==================== 测试函数 ====================

void runTests();

} // namespace SfinaeImpl

#endif // SFINAE_H
