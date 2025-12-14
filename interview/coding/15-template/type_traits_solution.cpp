/**
 * @file type_traits_solution.cpp
 * @brief 类型特征 - 参考答案
 */

#include "type_traits.h"
#include <iostream>
#include <cassert>

namespace TypeTraitsImpl {

namespace Solution {

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

template <typename T, typename U>
struct my_is_same : false_type {};

template <typename T>
struct my_is_same<T, T> : true_type {};

template <typename T, typename U>
constexpr bool my_is_same_v = my_is_same<T, U>::value;

// ==================== 题目2: remove_const ====================

template <typename T>
struct my_remove_const { using type = T; };

template <typename T>
struct my_remove_const<const T> { using type = T; };

template <typename T>
using my_remove_const_t = typename my_remove_const<T>::type;

// remove_volatile
template <typename T>
struct my_remove_volatile { using type = T; };

template <typename T>
struct my_remove_volatile<volatile T> { using type = T; };

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

template <typename T>
struct my_remove_reference { using type = T; };

template <typename T>
struct my_remove_reference<T&> { using type = T; };

template <typename T>
struct my_remove_reference<T&&> { using type = T; };

template <typename T>
using my_remove_reference_t = typename my_remove_reference<T>::type;

// ==================== 题目4: is_pointer ====================

template <typename T>
struct my_is_pointer_helper : false_type {};

template <typename T>
struct my_is_pointer_helper<T*> : true_type {};

template <typename T>
struct my_is_pointer : my_is_pointer_helper<my_remove_cv_t<T>> {};

template <typename T>
constexpr bool my_is_pointer_v = my_is_pointer<T>::value;

// ==================== 题目5: is_array ====================

template <typename T>
struct my_is_array : false_type {};

template <typename T>
struct my_is_array<T[]> : true_type {};

template <typename T, std::size_t N>
struct my_is_array<T[N]> : true_type {};

template <typename T>
constexpr bool my_is_array_v = my_is_array<T>::value;

// extent: 获取数组维度
template <typename T, unsigned N = 0>
struct my_extent : integral_constant<std::size_t, 0> {};

template <typename T>
struct my_extent<T[], 0> : integral_constant<std::size_t, 0> {};

template <typename T, unsigned N>
struct my_extent<T[], N> : my_extent<T, N - 1> {};

template <typename T, std::size_t I>
struct my_extent<T[I], 0> : integral_constant<std::size_t, I> {};

template <typename T, std::size_t I, unsigned N>
struct my_extent<T[I], N> : my_extent<T, N - 1> {};

// ==================== 题目6: conditional ====================

template <bool Cond, typename T, typename F>
struct my_conditional { using type = T; };

template <typename T, typename F>
struct my_conditional<false, T, F> { using type = F; };

template <bool Cond, typename T, typename F>
using my_conditional_t = typename my_conditional<Cond, T, F>::type;

// ==================== 题目7: enable_if ====================

template <bool Cond, typename T = void>
struct my_enable_if {};

template <typename T>
struct my_enable_if<true, T> { using type = T; };

template <bool Cond, typename T = void>
using my_enable_if_t = typename my_enable_if<Cond, T>::type;

// ==================== 题目8: decay ====================

template <typename T>
struct my_decay {
private:
    using U = my_remove_reference_t<T>;
public:
    using type = my_conditional_t<
        my_is_array<U>::value,
        typename std::remove_extent<U>::type*,
        my_conditional_t<
            std::is_function<U>::value,
            typename std::add_pointer<U>::type,
            my_remove_cv_t<U>
        >
    >;
};

template <typename T>
using my_decay_t = typename my_decay<T>::type;

// ==================== 更多常用特征 ====================

// is_void
template <typename T>
struct my_is_void : my_is_same<void, my_remove_cv_t<T>> {};

// is_integral (需要枚举所有整数类型)
template <typename T>
struct my_is_integral : false_type {};

template <> struct my_is_integral<bool> : true_type {};
template <> struct my_is_integral<char> : true_type {};
template <> struct my_is_integral<signed char> : true_type {};
template <> struct my_is_integral<unsigned char> : true_type {};
template <> struct my_is_integral<short> : true_type {};
template <> struct my_is_integral<unsigned short> : true_type {};
template <> struct my_is_integral<int> : true_type {};
template <> struct my_is_integral<unsigned int> : true_type {};
template <> struct my_is_integral<long> : true_type {};
template <> struct my_is_integral<unsigned long> : true_type {};
template <> struct my_is_integral<long long> : true_type {};
template <> struct my_is_integral<unsigned long long> : true_type {};

// is_reference
template <typename T>
struct my_is_lvalue_reference : false_type {};

template <typename T>
struct my_is_lvalue_reference<T&> : true_type {};

template <typename T>
struct my_is_rvalue_reference : false_type {};

template <typename T>
struct my_is_rvalue_reference<T&&> : true_type {};

template <typename T>
struct my_is_reference : integral_constant<bool,
    my_is_lvalue_reference<T>::value || my_is_rvalue_reference<T>::value> {};

// add_const, add_volatile, add_cv
template <typename T>
struct my_add_const { using type = const T; };

template <typename T>
struct my_add_volatile { using type = volatile T; };

template <typename T>
struct my_add_cv { using type = const volatile T; };

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Type Traits Tests ===" << std::endl;

    // is_same
    static_assert(Solution::my_is_same_v<int, int>, "");
    static_assert(!Solution::my_is_same_v<int, double>, "");
    static_assert(!Solution::my_is_same_v<int, const int>, "");
    std::cout << "  is_same: PASSED" << std::endl;

    // remove_const
    static_assert(Solution::my_is_same_v<Solution::my_remove_const_t<const int>, int>, "");
    static_assert(Solution::my_is_same_v<Solution::my_remove_const_t<const int*>, const int*>, "");
    static_assert(Solution::my_is_same_v<Solution::my_remove_const_t<int* const>, int*>, "");
    std::cout << "  remove_const: PASSED" << std::endl;

    // remove_reference
    static_assert(Solution::my_is_same_v<Solution::my_remove_reference_t<int&>, int>, "");
    static_assert(Solution::my_is_same_v<Solution::my_remove_reference_t<int&&>, int>, "");
    std::cout << "  remove_reference: PASSED" << std::endl;

    // is_pointer
    static_assert(Solution::my_is_pointer_v<int*>, "");
    static_assert(Solution::my_is_pointer_v<const int*>, "");
    static_assert(!Solution::my_is_pointer_v<int>, "");
    static_assert(!Solution::my_is_pointer_v<int&>, "");
    std::cout << "  is_pointer: PASSED" << std::endl;

    // is_array
    static_assert(Solution::my_is_array_v<int[]>, "");
    static_assert(Solution::my_is_array_v<int[10]>, "");
    static_assert(!Solution::my_is_array_v<int*>, "");
    std::cout << "  is_array: PASSED" << std::endl;

    // conditional
    static_assert(Solution::my_is_same_v<Solution::my_conditional_t<true, int, double>, int>, "");
    static_assert(Solution::my_is_same_v<Solution::my_conditional_t<false, int, double>, double>, "");
    std::cout << "  conditional: PASSED" << std::endl;

    // decay
    static_assert(Solution::my_is_same_v<Solution::my_decay_t<int&>, int>, "");
    static_assert(Solution::my_is_same_v<Solution::my_decay_t<const int&>, int>, "");
    static_assert(Solution::my_is_same_v<Solution::my_decay_t<int[10]>, int*>, "");
    std::cout << "  decay: PASSED" << std::endl;
}

} // namespace TypeTraitsImpl

/**
 * 关键要点：
 *
 * 1. 模板特化模式：
 *    - 主模板定义默认行为
 *    - 偏特化/全特化处理特殊情况
 *    - 利用 SFINAE 选择正确的特化
 *
 * 2. integral_constant 模式：
 *    - 统一的类型特征基类
 *    - 提供 value、type、operator()
 *    - true_type 和 false_type 是常用别名
 *
 * 3. 常见实现技巧：
 *    - 移除修饰符：特化匹配带修饰符的类型
 *    - 添加修饰符：直接在 type 中添加
 *    - 检测特征：偏特化匹配目标模式
 *
 * 4. void_t 技巧 (C++17)：
 *    - template <typename...> using void_t = void;
 *    - 用于检测表达式是否有效
 *    - SFINAE 友好
 *
 * 5. 变量模板 (C++14)：
 *    - template <typename T> inline constexpr bool xxx_v = xxx<T>::value;
 *    - 更简洁的使用方式
 *
 * 6. 别名模板 (C++11)：
 *    - template <typename T> using xxx_t = typename xxx<T>::type;
 *    - 避免 typename 和 ::type
 */
