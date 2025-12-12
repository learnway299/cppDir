/**
 * @file sfinae.cpp
 * @brief SFINAE - 面试题
 *
 * SFINAE (Substitution Failure Is Not An Error) 是 C++ 模板的核心特性，
 * 用于在编译期根据类型特征选择不同的函数重载或模板特化。
 */
#include <type_traits>
#include <iostream>

// 题目1: 检测类型是否有 size() 成员函数
template <typename T, typename = void>
struct has_size : std::false_type {};

// TODO: 添加特化来检测 size() 成员

// 题目2: 检测类型是否可以使用 << 输出到 ostream
template <typename T, typename = void>
struct is_printable : std::false_type {};

// TODO: 添加特化

// 题目3: 实现根据类型选择不同实现的函数
// 对于整数类型使用位运算，对于其他类型使用普通运算
template <typename T>
T multiply_by_two(T value) {
    // TODO: 使用 if constexpr 或 enable_if 实现
    return value * 2;
}

// 题目4: 检测类是否有指定名称的成员类型
// 例如检测是否有 value_type
template <typename T, typename = void>
struct has_value_type : std::false_type {};

// TODO: 添加特化

// 题目5: 实现 void_t（如果没有 C++17）
// template <typename...>
// using void_t = void;

// 题目6: 检测类型是否可默认构造
template <typename T, typename = void>
struct is_default_constructible : std::false_type {};

// TODO: 添加特化

// 题目7: 检测两个类型是否可以相加
template <typename T, typename U, typename = void>
struct can_add : std::false_type {};

// TODO: 添加特化

// 题目8: 使用 SFINAE 实现函数重载（tag dispatch 替代方案）
struct Container {
    void push_back(int) {}
};

struct NonContainer {
    void insert(int) {}
};

// 根据是否有 push_back 选择不同的插入方式
template <typename C>
void insert_element(C& container, int value) {
    // TODO: 使用 SFINAE 选择 push_back 或 insert
}

int main() {
    return 0;
}
