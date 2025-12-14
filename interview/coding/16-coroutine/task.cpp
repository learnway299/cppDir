/**
 * @file task.cpp
 * @brief Task 协程 - 面试者实现文件
 */

#include "task.h"

namespace TaskImpl {

// ==================== Task<T> 实现 ====================

template <typename T>
Task<T> Task<T>::promise_type::get_return_object() {
    return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
}

template <typename T>
auto Task<T>::promise_type::final_suspend() noexcept {
    struct FinalAwaiter {
        bool await_ready() noexcept { return false; }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
            // TODO: 如果有 continuation，恢复它
            return std::noop_coroutine();
        }

        void await_resume() noexcept {}
    };
    return FinalAwaiter{};
}

template <typename T>
void Task<T>::promise_type::return_value(T value) {
    result.template emplace<1>(std::move(value));
}

template <typename T>
void Task<T>::promise_type::unhandled_exception() {
    result.template emplace<2>(std::current_exception());
}

// Awaiter 实现
template <typename T>
bool Task<T>::Awaiter::await_ready() noexcept {
    return handle.done();
}

template <typename T>
std::coroutine_handle<> Task<T>::Awaiter::await_suspend(std::coroutine_handle<> awaiting) noexcept {
    // TODO: 设置 continuation
    return handle;
}

template <typename T>
T Task<T>::Awaiter::await_resume() {
    // TODO: 检查异常，返回结果
    return T{};
}

template <typename T>
typename Task<T>::Awaiter Task<T>::operator co_await() {
    return Awaiter{handle_};
}

template <typename T>
T Task<T>::get() {
    // TODO: 同步获取结果
    return T{};
}

template <typename T>
Task<T>::Task(Task&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

template <typename T>
Task<T>& Task<T>::operator=(Task&& other) noexcept {
    if (this != &other) {
        if (handle_) handle_.destroy();
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

template <typename T>
Task<T>::~Task() {
    if (handle_) {
        handle_.destroy();
    }
}

// ==================== Task<void> 实现 ====================

Task<void> Task<void>::promise_type::get_return_object() {
    return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
}

auto Task<void>::promise_type::final_suspend() noexcept {
    struct FinalAwaiter {
        bool await_ready() noexcept { return false; }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
            // TODO: 如果有 continuation，恢复它
            return std::noop_coroutine();
        }
        void await_resume() noexcept {}
    };
    return FinalAwaiter{};
}

void Task<void>::promise_type::return_void() {}

void Task<void>::promise_type::unhandled_exception() {
    exception = std::current_exception();
}

bool Task<void>::Awaiter::await_ready() noexcept {
    return handle.done();
}

std::coroutine_handle<> Task<void>::Awaiter::await_suspend(std::coroutine_handle<> awaiting) noexcept {
    // TODO: 设置 continuation
    return handle;
}

void Task<void>::Awaiter::await_resume() {
    // TODO: 检查异常
}

Task<void>::Awaiter Task<void>::operator co_await() {
    return Awaiter{handle_};
}

void Task<void>::get() {
    // TODO: 同步等待并检查异常
}

Task<void>::Task(Task&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
}

Task<void>::~Task() {
    if (handle_) handle_.destroy();
}

// ==================== 题目3: 异步操作 ====================

Task<int> async_add(int a, int b) {
    // TODO: 实现异步加法
    co_return a + b;
}

Task<int> async_multiply(int a, int b) {
    // TODO: 实现异步乘法
    co_return a * b;
}

// ==================== 题目4: 任务链 ====================

Task<int> chained_task() {
    // TODO: 实现链式调用
    // 1+2=3, 3*4=12, 12+10=22
    co_return 0;
}

// 显式模板实例化
template class Task<int>;
template class Task<long long>;

} // namespace TaskImpl
