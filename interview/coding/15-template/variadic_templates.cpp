/**
 * @file variadic_templates.cpp
 * @brief 变参模板 - 面试题
 *
 * 变参模板 (Variadic Templates) 允许模板接受任意数量的类型参数或值参数。
 */
#include <iostream>
#include <tuple>

// 题目1: 实现 print 函数，打印任意数量的参数
template <typename... Args>
void print(Args... args) {
    // TODO: 实现
}

// 题目2: 实现 sum 函数，计算任意数量参数的和
template <typename... Args>
auto sum(Args... args) {
    // TODO: 实现
    return 0;
}

// 题目3: 实现 count_args，返回参数个数
template <typename... Args>
constexpr std::size_t count_args(Args... args) {
    // TODO: 实现
    return 0;
}

// 题目4: 实现 make_tuple（简化版）
// template <typename... Args>
// auto my_make_tuple(Args&&... args) { /* TODO */ }

// 题目5: 实现 apply - 将 tuple 展开为函数参数
// template <typename F, typename Tuple>
// auto my_apply(F&& f, Tuple&& t) { /* TODO */ }

// 题目6: 实现 all_same - 检查所有类型是否相同
template <typename T, typename... Rest>
struct all_same {
    // TODO: 实现
    static constexpr bool value = true;
};

// 题目7: 实现 contains_type - 检查类型列表中是否包含指定类型
template <typename T, typename... List>
struct contains_type {
    // TODO: 实现
    static constexpr bool value = false;
};

// 题目8: 实现 type_at - 获取类型列表中指定索引的类型
template <std::size_t I, typename... Types>
struct type_at {
    // TODO: 实现
};

int main() {
    return 0;
}
