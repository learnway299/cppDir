/**
 * @file type_traits.cpp
 * @brief 类型特征 - 面试题
 *
 * 类型特征是模板元编程的基础，用于在编译期查询和操作类型信息。
 */
#include <type_traits>

// 题目1: 实现 is_same
// 判断两个类型是否相同
template <typename T, typename U>
struct my_is_same {
    // TODO: 实现
    static constexpr bool value = false;
};

// 题目2: 实现 remove_const
// 移除类型的 const 修饰
template <typename T>
struct my_remove_const {
    // TODO: 实现
    using type = T;
};

// 题目3: 实现 remove_reference
// 移除类型的引用
template <typename T>
struct my_remove_reference {
    // TODO: 实现
    using type = T;
};

// 题目4: 实现 is_pointer
// 判断类型是否为指针
template <typename T>
struct my_is_pointer {
    // TODO: 实现
    static constexpr bool value = false;
};

// 题目5: 实现 is_array
// 判断类型是否为数组
template <typename T>
struct my_is_array {
    // TODO: 实现
    static constexpr bool value = false;
};

// 题目6: 实现 conditional
// 编译期条件选择类型
template <bool Cond, typename T, typename F>
struct my_conditional {
    // TODO: 实现
    using type = T;
};

// 题目7: 实现 enable_if
// SFINAE 工具
template <bool Cond, typename T = void>
struct my_enable_if {
    // TODO: 实现
};

// 题目8: 实现 decay
// 类型退化（数组->指针，函数->函数指针，移除 cv 和引用）
template <typename T>
struct my_decay {
    // TODO: 实现
    using type = T;
};

int main() {
    // 测试代码
    static_assert(my_is_same<int, int>::value, "");
    static_assert(!my_is_same<int, double>::value, "");

    return 0;
}
