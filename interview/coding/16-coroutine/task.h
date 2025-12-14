/**
 * @file task.h
 * @brief Task 协程 - 题目说明与声明
 *
 * Task 是一种表示异步操作的协程类型，支持 co_await。
 * 与 Generator 不同，Task 返回单个结果而非多个值。
 *
 * 包含题目：
 * 1. 基本 Task<T> 实现
 * 2. Task<void> 特化
 * 3. 支持 co_await 的 Awaiter
 * 4. 任务链（链式调用）
 */

#ifndef TASK_H
#define TASK_H

#include <coroutine>
#include <iostream>
#include <optional>
#include <exception>
#include <variant>

namespace TaskImpl {

// ==================== 题目1: 基本 Task<T> 实现 ====================

/**
 * 题目1: 实现基本的 Task<T> 类型
 *
 * 表示一个返回 T 类型结果的异步任务，��持 co_await
 *
 * 示例：
 *   Task<int> async_add(int a, int b) {
 *       co_return a + b;
 *   }
 *
 *   Task<int> caller() {
 *       int result = co_await async_add(1, 2);
 *       co_return result;
 *   }
 */
template <typename T>
class Task {
public:
    struct promise_type {
        std::variant<std::monostate, T, std::exception_ptr> result;
        std::coroutine_handle<> continuation;

        Task get_return_object();
        std::suspend_never initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept;
        void return_value(T value);
        void unhandled_exception();
    };

    using handle_type = std::coroutine_handle<promise_type>;

    // Awaiter for co_await
    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept;
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept;
        T await_resume();
    };

    Awaiter operator co_await();

    // 同步获取结果
    T get();

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept;
    Task& operator=(Task&& other) noexcept;
    ~Task();

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

private:
    handle_type handle_;
};

// ==================== 题目2: Task<void> 特化 ====================

/**
 * 题目2: 实现 Task<void> 特化
 *
 * 表示没有返回值的异步任务
 *
 * 示例：
 *   Task<void> async_print() {
 *       std::cout << "Hello" << std::endl;
 *       co_return;
 *   }
 */
template <>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        Task get_return_object();
        std::suspend_never initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept;
        void return_void();
        void unhandled_exception();
    };

    using handle_type = std::coroutine_handle<promise_type>;

    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept;
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept;
        void await_resume();
    };

    Awaiter operator co_await();

    void get();

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept;
    ~Task();

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

private:
    handle_type handle_;
};

// ==================== 题目3: 异步操作函数 ====================

/**
 * 题目3: 实现简单的异步操作
 */
Task<int> async_add(int a, int b);
Task<int> async_multiply(int a, int b);

// ==================== 题目4: 任务链 ====================

/**
 * 题目4: 实现任务链
 *
 * 链式调用多个异步任务
 *
 * 示例：
 *   // 1+2=3, 3*4=12, 12+10=22
 *   auto result = chained_task().get();
 */
Task<int> chained_task();

// ==================== 测试函数 ====================

void runTests();

} // namespace TaskImpl

#endif // TASK_H
