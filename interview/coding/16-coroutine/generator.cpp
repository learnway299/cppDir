/**
 * @file generator.cpp
 * @brief 生成器协程 - 面试者实现文件
 */

#include "generator.h"
#include <vector>

namespace GeneratorImpl {

// ==================== Generator 实现 ====================

template <typename T>
Generator<T> Generator<T>::promise_type::get_return_object() {
    return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
}

template <typename T>
std::suspend_always Generator<T>::promise_type::yield_value(T value) {
    current_value = std::move(value);
    return {};
}

template <typename T>
void Generator<T>::promise_type::unhandled_exception() {
    exception = std::current_exception();
}

template <typename T>
void Generator<T>::promise_type::rethrow_if_exception() {
    if (exception) {
        std::rethrow_exception(exception);
    }
}

// Iterator 实现
template <typename T>
T& Generator<T>::Iterator::operator*() const {
    return handle_.promise().current_value;
}

template <typename T>
typename Generator<T>::Iterator& Generator<T>::Iterator::operator++() {
    // TODO: resume 协程，检查是否完成
    return *this;
}

template <typename T>
bool Generator<T>::Iterator::operator==(const Iterator& other) const {
    return handle_ == other.handle_;
}

template <typename T>
bool Generator<T>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

// Generator 成员函数
template <typename T>
typename Generator<T>::Iterator Generator<T>::begin() {
    // TODO: resume 协程获取第一个值
    return Iterator{};
}

template <typename T>
typename Generator<T>::Iterator Generator<T>::end() {
    return Iterator{};
}

template <typename T>
Generator<T>::Generator(Generator&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

template <typename T>
Generator<T>& Generator<T>::operator=(Generator&& other) noexcept {
    if (this != &other) {
        if (handle_) {
            handle_.destroy();
        }
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

template <typename T>
Generator<T>::~Generator() {
    if (handle_) {
        handle_.destroy();
    }
}

template <typename T>
std::optional<T> Generator<T>::next() {
    // TODO: 返回下一个值或 nullopt
    return std::nullopt;
}

template <typename T>
bool Generator<T>::advance() {
    if (handle_ && !handle_.done()) {
        handle_.resume();
        return !handle_.done();
    }
    return false;
}

template <typename T>
T Generator<T>::value() const {
    return handle_.promise().current_value;
}

// ==================== 题目2: 斐波那契数列 ====================

Generator<long long> fibonacci(int n) {
    // TODO: 使用 co_yield 生成斐波那契数列
    long long a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        // TODO: 计算下一个斐波那契数
    }
}

Generator<long long> fibonacci_infinite() {
    // TODO: 生成无限斐波那契数列
    co_yield 0;
}

// ==================== 题目3: 范围生成器 ====================

Generator<int> range(int start, int end, int step) {
    // TODO: 生成 [start, end) 范围内的数
    for (int i = start; i < end; i += step) {
        co_yield i;
    }
}

// ==================== 题目4: 无限生成器 ====================

Generator<int> natural_numbers() {
    // TODO: 生成无限的自然数序列
    int n = 0;
    while (true) {
        co_yield n++;
    }
}

Generator<int> count_from(int start) {
    // TODO: 从 start 开始计数
    co_yield start;
}

// ==================== 题目5: Generator 操作 ====================

template <typename T>
Generator<T> take(Generator<T> gen, int n) {
    // TODO: 从生成器中取前 n 个元素
    co_return;
}

// 显式模板实例化
template class Generator<int>;
template class Generator<long long>;
template class Generator<std::pair<size_t, int>>;

} // namespace GeneratorImpl
