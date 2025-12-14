/**
 * @file type_traits.h
 * @brief 类型特征 - 题目说明与声明
 *
 * 类型特征是模板元编程的基础，用于在编译期查询和操作类型信息。
 *
 * 包含题目：
 * 1. is_same - 判断两个类型是否相同
 * 2. remove_const - 移除 const 修饰
 * 3. remove_reference - 移除引用
 * 4. is_pointer - 判断是否为指针
 * 5. is_array - 判断是否为数组
 * 6. conditional - 编译期条件选择
 * 7. enable_if - SFINAE 工具
 * 8. decay - 类型退化
 */

#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <type_traits>
#include <cstddef>

namespace TypeTraitsImpl {

// ==================== 基础工具 ====================

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

// ==================== 题目1: is_same ====================

/**
 * 题目1: 实现 is_same
 *
 * 判断两个类型是否完全相同
 *
 * 示例：
 *   my_is_same_v<int, int>        // true
 *   my_is_same_v<int, const int>  // false
 */
template <typename T, typename U>
struct my_is_same : false_type {};

// TODO: 添加特化版本

template <typename T, typename U>
constexpr bool my_is_same_v = my_is_same<T, U>::value;

// ==================== 题目2: remove_const ====================

/**
 * 题目2: 实现 remove_const
 *
 * 移除类型的 const 修饰
 *
 * 示例：
 *   my_remove_const_t<const int>     // int
 *   my_remove_const_t<const int*>    // const int* (顶层 const)
 *   my_remove_const_t<int* const>    // int* (移除顶层 const)
 */
template <typename T>
struct my_remove_const { using type = T; };

// TODO: 添加特化版本

template <typename T>
using my_remove_const_t = typename my_remove_const<T>::type;

// remove_volatile
template <typename T>
struct my_remove_volatile { using type = T; };

// TODO: 添加特化版本

template <typename T>
using my_remove_volatile_t = typename my_remove_volatile<T>::type;

// remove_cv
template <typename T>
struct my_remove_cv {
    using type = typename my_remove_volatile<typename my_remove_const<T>::type>::type;
};

template <typename T>
using my_remove_cv_t = typename my_remove_cv<T>::type;

// ==================== 题目3: remove_reference ====================

/**
 * 题目3: 实现 remove_reference
 *
 * 移除类型的引用修饰
 *
 * 示例：
 *   my_remove_reference_t<int&>   // int
 *   my_remove_reference_t<int&&>  // int
 */
template <typename T>
struct my_remove_reference { using type = T; };

// TODO: 添加 T& 和 T&& 的特化版本

template <typename T>
using my_remove_reference_t = typename my_remove_reference<T>::type;

// ==================== 题目4: is_pointer ====================

/**
 * 题目4: 实现 is_pointer
 *
 * 判断类型是否为指针（需要处理 cv 修饰的指针）
 *
 * 示例：
 *   my_is_pointer_v<int*>         // true
 *   my_is_pointer_v<const int*>   // true
 *   my_is_pointer_v<int* const>   // true
 *   my_is_pointer_v<int>          // false
 */
template <typename T>
struct my_is_pointer_helper : false_type {};

// TODO: 添加 T* 的特化版本

template <typename T>
struct my_is_pointer : my_is_pointer_helper<my_remove_cv_t<T>> {};

template <typename T>
constexpr bool my_is_pointer_v = my_is_pointer<T>::value;

// ==================== 题目5: is_array ====================

/**
 * 题目5: 实现 is_array
 *
 * 判断类型是否为数组
 *
 * 示例：
 *   my_is_array_v<int[]>   // true
 *   my_is_array_v<int[10]> // true
 *   my_is_array_v<int*>    // false
 */
template <typename T>
struct my_is_array : false_type {};

// TODO: 添加 T[] 和 T[N] 的特化版本

template <typename T>
constexpr bool my_is_array_v = my_is_array<T>::value;

// ==================== 题目6: conditional ====================

/**
 * 题目6: 实现 conditional
 *
 * 编译期条件选择类型
 *
 * 示例：
 *   my_conditional_t<true, int, double>   // int
 *   my_conditional_t<false, int, double>  // double
 */
template <bool Cond, typename T, typename F>
struct my_conditional { using type = T; };

// TODO: 添加 false 情况的特化版本

template <bool Cond, typename T, typename F>
using my_conditional_t = typename my_conditional<Cond, T, F>::type;

// ==================== 题目7: enable_if ====================

/**
 * 题目7: 实现 enable_if
 *
 * SFINAE 工具，条件为 true 时提供 type
 *
 * 示例：
 *   my_enable_if_t<true, int>   // int
 *   my_enable_if_t<false, int>  // 编译错误（无 type）
 */
template <bool Cond, typename T = void>
struct my_enable_if {};

// TODO: 添加 true 情况的特化版本

template <bool Cond, typename T = void>
using my_enable_if_t = typename my_enable_if<Cond, T>::type;

// ==================== 题目8: decay ====================

/**
 * 题目8: 实现 decay
 *
 * 类型退化：
 * - 移除引用
 * - 数组 -> 指针
 * - 函数 -> 函数指针
 * - 移除 cv 限定符
 *
 * 示例：
 *   my_decay_t<int&>          // int
 *   my_decay_t<const int&>    // int
 *   my_decay_t<int[10]>       // int*
 *   my_decay_t<int(int)>      // int(*)(int)
 */
template <typename T>
struct my_decay {
    // TODO: 实现
    using type = T;
};

template <typename T>
using my_decay_t = typename my_decay<T>::type;

// ==================== 测试函数 ====================

void runTests();

} // namespace TypeTraitsImpl

#endif // TYPE_TRAITS_H
