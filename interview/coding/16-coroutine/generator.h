/**
 * @file generator.h
 * @brief 生成器协程 - 题目说明与声明
 *
 * C++20 协程的基础应用：实现惰性求值的生成器。
 * 需要 C++20 编译器支持 (-std=c++20 -fcoroutines)
 *
 * 包含题目：
 * 1. 基本 Generator 类型（支持 co_yield）
 * 2. 斐波那契数列生成器
 * 3. 范围生成器
 * 4. 无限生成器
 * 5. take/map/filter 操作
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <coroutine>
#include <iostream>
#include <optional>
#include <exception>
#include <utility>

namespace GeneratorImpl {

// ==================== 题目1: 基本 Generator 实现 ====================

/**
 * 题目1: 实现基本的 Generator 类型
 *
 * 支持 co_yield 产生值，提供迭代器接口
 *
 * 示例：
 *   Generator<int> count_to(int n) {
 *       for (int i = 0; i < n; ++i) {
 *           co_yield i;
 *       }
 *   }
 *   for (auto x : count_to(5)) {
 *       std::cout << x << " ";  // 0 1 2 3 4
 *   }
 */
template <typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        std::exception_ptr exception;

        Generator get_return_object();
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value);
        void return_void() {}
        void unhandled_exception();
        void rethrow_if_exception();
    };

    using handle_type = std::coroutine_handle<promise_type>;

    // 迭代器类
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator() : handle_(nullptr) {}
        explicit Iterator(handle_type handle) : handle_(handle) {}

        T& operator*() const;
        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

    private:
        handle_type handle_;
    };

    Iterator begin();
    Iterator end();

    explicit Generator(handle_type handle) : handle_(handle) {}
    Generator(Generator&& other) noexcept;
    Generator& operator=(Generator&& other) noexcept;
    ~Generator();

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    // 获取下一个值
    std::optional<T> next();

    // 简单接口
    bool advance();
    T value() const;

private:
    handle_type handle_;
};

// ==================== 题目2: 斐波那契数列 ====================

/**
 * 题目2: 使用 Generator 实现斐波那契数列
 *
 * 示例：
 *   for (auto n : fibonacci(10)) {
 *       std::cout << n << " ";  // 0 1 1 2 3 5 8 13 21 34
 *   }
 */
Generator<long long> fibonacci(int n);

// 无限斐波那契
Generator<long long> fibonacci_infinite();

// ==================== 题目3: 范围生成器 ====================

/**
 * 题目3: 实现范围生成器
 *
 * 生成 [start, end) 范围内的数，支持步长
 *
 * 示例：
 *   for (auto n : range(0, 10, 2)) {
 *       std::cout << n << " ";  // 0 2 4 6 8
 *   }
 */
Generator<int> range(int start, int end, int step = 1);

// ==================== 题目4: 无限生成器 ====================

/**
 * 题目4: 实现无限生成器
 *
 * 生成无限的自然数序列
 *
 * 示例：
 *   auto gen = natural_numbers();
 *   for (int i = 0; i < 5; ++i) {
 *       gen.advance();
 *       std::cout << gen.value() << " ";  // 0 1 2 3 4
 *   }
 */
Generator<int> natural_numbers();
Generator<int> count_from(int start);

// ==================== 题目5: Generator 操作 ====================

/**
 * 题目5: 实现 take 操作
 *
 * 从生成器中取前 n 个元素
 */
template <typename T>
Generator<T> take(Generator<T> gen, int n);

/**
 * map: 对每个元素应用函数
 */
template <typename T, typename F>
auto map(Generator<T> gen, F func) -> Generator<decltype(func(std::declval<T>()))>;

/**
 * filter: 过滤元素
 */
template <typename T, typename Pred>
Generator<T> filter(Generator<T> gen, Pred pred);

/**
 * enumerate: 添加索引
 */
template <typename T>
Generator<std::pair<size_t, T>> enumerate(Generator<T> gen);

// ==================== 测试函数 ====================

void runTests();

} // namespace GeneratorImpl

#endif // GENERATOR_H
