/**
 * @file generator.cpp
 * @brief 生成器协程 - 面试题
 *
 * C++20 协程的基础应用：实现惰性求值的生成器。
 * 需要 C++20 编译器支持。
 */
#include <coroutine>
#include <iostream>
#include <optional>

// 题目1: 实现基本的 Generator 类型
// 支持 co_yield 产生值
template <typename T>
class Generator {
public:
    struct promise_type {
        // TODO: 实现 promise_type
        // - get_return_object()
        // - initial_suspend()
        // - final_suspend()
        // - yield_value()
        // - return_void()
        // - unhandled_exception()
    };

    // TODO: 实现迭代器接口
    // - begin()
    // - end()
    // - Iterator 类

private:
    // TODO: 存储协程句柄
};

// 题目2: 使用 Generator 实现斐波那契数列
Generator<int> fibonacci(int n) {
    // TODO: 使用 co_yield 生成斐波那契数列
    co_return;
}

// 题目3: 实现范围生成器
Generator<int> range(int start, int end, int step = 1) {
    // TODO: 生成 [start, end) 范围内的数
    co_return;
}

// 题目4: 实现无限生成器（如自然数）
Generator<int> natural_numbers() {
    // TODO: 生成无限的自然数序列
    co_return;
}

// 题目5: 实现生成器的 take 操作
// 从无限生成器中取前 n 个元素
template <typename T>
Generator<T> take(Generator<T> gen, int n) {
    // TODO: 实现
    co_return;
}

int main() {
    return 0;
}
