/**
 * @file type_erasure_impl.cpp
 * @brief 类型擦除实现 - 面试题
 *
 * 类型擦除是一种技术，允许在运行时处理不同类型的对象，
 * 同时隐藏具体类型信息。std::function 和 std::any 是典型例子。
 */
#include <memory>
#include <iostream>

// 题目1: 实现简化版 std::function<R(Args...)>
// 可以存储任何可调用对象（函数、lambda、函数对象）
template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
    Function() = default;

    // TODO: 实现构造函数，接受任意可调用对象

    // TODO: 实现 operator()

    // TODO: 实现 operator bool

private:
    // TODO: 定义内部类型擦除机制
};

// 题目2: 实现简化版 std::any
// 可以存储任何类型的值
class Any {
public:
    Any() = default;

    // TODO: 实现模板构造函数

    // TODO: 实现 type() 获取类型信息

    // TODO: 实现 has_value()

    // TODO: 实现 reset()

    // TODO: 实现 any_cast 友元函数

private:
    // TODO: 定义内部类型擦除机制
};

// 题目3: 实现类型安全的多态容器
// 存储不同类型但满足相同概念的对象
// 例如：所有可以 draw() 的对象
class Drawable {
public:
    // TODO: 实现类型擦除的 Drawable
    void draw() const { /* TODO */ }

private:
    // TODO
};

// 题目4: 实现带 Small Buffer Optimization (SBO) 的类型擦除
// 小对象直接存储在栈上，大对象存储在堆上
template <typename Signature, std::size_t BufferSize = 32>
class SmallFunction;

// TODO: 实现

int main() {
    return 0;
}
