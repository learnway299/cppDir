/**
 * @file generator_solution.cpp
 * @brief 生成器协程 - 参考答案
 *
 * 编译: g++ -std=c++20 -fcoroutines generator_solution.cpp
 */

#include "generator.h"
#include <vector>
#include <cassert>

namespace GeneratorImpl {

namespace Solution {

// ==================== 题目1: 完整 Generator 实现 ====================

template <typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        std::exception_ptr exception;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 惰性启动：创建时不执行
        std::suspend_always initial_suspend() noexcept { return {}; }

        // 结束时挂起，让调用者有机会清理
        std::suspend_always final_suspend() noexcept { return {}; }

        // co_yield 实现
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        void return_void() {}

        void unhandled_exception() {
            exception = std::current_exception();
        }

        void rethrow_if_exception() {
            if (exception) {
                std::rethrow_exception(exception);
            }
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    // 迭代器
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator() : handle_(nullptr) {}
        explicit Iterator(handle_type handle) : handle_(handle) {}

        T& operator*() const {
            return handle_.promise().current_value;
        }

        Iterator& operator++() {
            handle_.resume();
            if (handle_.done()) {
                handle_.promise().rethrow_if_exception();
                handle_ = nullptr;
            }
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return handle_ == other.handle_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        handle_type handle_;
    };

    Iterator begin() {
        if (handle_) {
            handle_.resume();
            if (handle_.done()) {
                handle_.promise().rethrow_if_exception();
                return end();
            }
        }
        return Iterator{handle_};
    }

    Iterator end() {
        return Iterator{};
    }

    // 构造和析构
    explicit Generator(handle_type handle) : handle_(handle) {}

    Generator(Generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Generator() {
        if (handle_) {
            handle_.destroy();
        }
    }

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    // 获取下一个值
    std::optional<T> next() {
        if (!handle_ || handle_.done()) {
            return std::nullopt;
        }
        handle_.resume();
        if (handle_.done()) {
            return std::nullopt;
        }
        return handle_.promise().current_value;
    }

private:
    handle_type handle_;
};

// ==================== 题目2: 斐波那契数列 ====================

Generator<long long> fibonacci(int n) {
    long long a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

// 无限斐波那契
Generator<long long> fibonacci_infinite() {
    long long a = 0, b = 1;
    while (true) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

// ==================== 题目3: 范围生成器 ====================

Generator<int> range(int start, int end, int step) {
    if (step > 0) {
        for (int i = start; i < end; i += step) {
            co_yield i;
        }
    } else if (step < 0) {
        for (int i = start; i > end; i += step) {
            co_yield i;
        }
    }
}

// ==================== 题目4: 无限生成器 ====================

Generator<int> natural_numbers() {
    int n = 0;
    while (true) {
        co_yield n++;
    }
}

Generator<int> count_from(int start) {
    while (true) {
        co_yield start++;
    }
}

// ==================== 题目5: Generator 操作 ====================

// take: 从生成器中取前 n 个元素
template <typename T>
Generator<T> take(Generator<T> gen, int n) {
    int count = 0;
    for (auto& value : gen) {
        if (count++ >= n) {
            break;
        }
        co_yield value;
    }
}

// map: 对每个元素应用函数
template <typename T, typename F>
auto map(Generator<T> gen, F func) -> Generator<decltype(func(std::declval<T>()))> {
    for (auto& value : gen) {
        co_yield func(value);
    }
}

// filter: 过滤元素
template <typename T, typename Pred>
Generator<T> filter(Generator<T> gen, Pred pred) {
    for (auto& value : gen) {
        if (pred(value)) {
            co_yield value;
        }
    }
}

// enumerate: 添加索引
template <typename T>
Generator<std::pair<size_t, T>> enumerate(Generator<T> gen) {
    size_t index = 0;
    for (auto& value : gen) {
        co_yield std::make_pair(index++, value);
    }
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Generator Tests ===" << std::endl;

    // 测试斐波那契数列
    {
        std::vector<long long> expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
        std::vector<long long> result;
        for (auto n : Solution::fibonacci(10)) {
            result.push_back(n);
        }
        assert(result == expected);
    }
    std::cout << "  Fibonacci: PASSED" << std::endl;

    // 测试 range
    {
        std::vector<int> expected = {0, 2, 4, 6, 8};
        std::vector<int> result;
        for (auto n : Solution::range(0, 10, 2)) {
            result.push_back(n);
        }
        assert(result == expected);
    }
    std::cout << "  Range: PASSED" << std::endl;

    // 测试 take
    {
        std::vector<int> result;
        for (auto n : Solution::take(Solution::natural_numbers(), 5)) {
            result.push_back(n);
        }
        assert(result.size() == 5);
        assert(result[0] == 0 && result[4] == 4);
    }
    std::cout << "  Take: PASSED" << std::endl;

    // 测试 map
    {
        std::vector<int> result;
        for (auto n : Solution::map(Solution::range(0, 5, 1), [](int x) { return x * x; })) {
            result.push_back(n);
        }
        std::vector<int> expected = {0, 1, 4, 9, 16};
        assert(result == expected);
    }
    std::cout << "  Map: PASSED" << std::endl;

    // 测试 filter
    {
        std::vector<int> result;
        for (auto n : Solution::filter(Solution::range(0, 10, 1), [](int x) { return x % 2 == 0; })) {
            result.push_back(n);
        }
        std::vector<int> expected = {0, 2, 4, 6, 8};
        assert(result == expected);
    }
    std::cout << "  Filter: PASSED" << std::endl;
}

} // namespace GeneratorImpl

/**
 * 关键要点：
 *
 * 1. Generator 结构：
 *    - promise_type：定义协程行为
 *    - coroutine_handle：控制协程执行
 *    - Iterator：提供遍历接口
 *
 * 2. promise_type 要点：
 *    - get_return_object(): 创建 Generator 对象
 *    - initial_suspend(): 决定是否立即执行
 *    - final_suspend(): 决定结束后是否挂起
 *    - yield_value(): 处理 co_yield
 *    - return_void()/return_value(): 处理 co_return
 *    - unhandled_exception(): 处理异常
 *
 * 3. 挂起点：
 *    - suspend_always: 总是挂起
 *    - suspend_never: 不挂起
 *    - 可自定义 awaitable
 *
 * 4. 惰性求值：
 *    - initial_suspend 返回 suspend_always
 *    - 值在需要时才计算
 *    - 支持无限序列
 *
 * 5. 生命周期：
 *    - 协程帧在堆上分配
 *    - handle.destroy() 释放资源
 *    - RAII 管理句柄
 *
 * 6. C++23 std::generator：
 *    - 标准库提供的 Generator
 *    - 更完善的实现
 */
