/**
 * @file async_io.h
 * @brief 异步 IO - 题目说明与声明
 *
 * 使用协程实现异步 IO 操作。
 *
 * 包含题目：
 * 1. async_sleep 异步等待
 * 2. 事件循环 EventLoop
 * 3. 异步文件读取（模拟）
 * 4. 异步网络请求（模拟）
 */

#ifndef ASYNC_IO_H
#define ASYNC_IO_H

#include <coroutine>
#include <functional>
#include <queue>
#include <chrono>
#include <string>
#include <optional>
#include <map>
#include <memory>

namespace AsyncIOImpl {

// ==================== Task 类型 ====================

template <typename T>
class Task {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::optional<T> result;
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        Task get_return_object();
        std::suspend_always initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept;
        void return_value(T value) { result = std::move(value); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    struct Awaiter {
        handle_type handle;
        bool await_ready() noexcept;
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept;
        T await_resume();
    };

    Awaiter operator co_await();

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept;
    ~Task();

    void start();

private:
    handle_type handle_;
};

// Task<void> 特化
template <>
class Task<void> {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        Task get_return_object();
        std::suspend_always initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept;
        void return_void();
        void unhandled_exception() { exception = std::current_exception(); }
    };

    struct Awaiter {
        handle_type handle;
        bool await_ready() noexcept;
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept;
        void await_resume();
    };

    Awaiter operator co_await();

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept;
    ~Task();

    void start();

private:
    handle_type handle_;
};

// ==================== 题目2: 事件循环 ====================

/**
 * 题目2: 实现简单的事件循环
 *
 * 管理任务队列和定时器
 *
 * 示例：
 *   EventLoop loop;
 *   loop.schedule([]() { std::cout << "Hello"; });
 *   loop.schedule_after(100, []() { std::cout << "World"; });
 *   loop.run();
 */
class EventLoop {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

    // 运行事件循环
    void run();

    // 调度一个即时任务
    void schedule(std::function<void()> task);

    // 调度一个延迟任务
    void schedule_after(int ms, std::function<void()> task);

    // 停止事件循环
    void stop();

    // 运行协程直到完成
    template <typename TaskType>
    void run_until_complete(TaskType& task);

private:
    std::queue<std::function<void()>> tasks_;
    std::multimap<TimePoint, std::function<void()>> timers_;
    bool running_ = false;
};

// ==================== 题目1: async_sleep ====================

/**
 * 题目1: 实现 async_sleep
 *
 * 异步等待指定毫秒数
 *
 * 示例：
 *   Task<void> example(EventLoop& loop) {
 *       co_await async_sleep(loop, 100);  // 等待 100ms
 *       std::cout << "Done" << std::endl;
 *   }
 */
struct SleepAwaiter {
    EventLoop& loop;
    int ms;

    bool await_ready() noexcept;
    void await_suspend(std::coroutine_handle<> handle) noexcept;
    void await_resume() noexcept;
};

SleepAwaiter async_sleep(EventLoop& loop, int ms);

// ==================== 题目3: 异步文件读取 ====================

/**
 * 题目3: 实现异步文件读取（模拟）
 *
 * 模拟异步读取文件内容
 */
struct FileReadAwaiter {
    EventLoop& loop;
    std::string filename;
    std::string result;

    bool await_ready() noexcept;
    void await_suspend(std::coroutine_handle<> handle) noexcept;
    std::string await_resume() noexcept;
};

Task<std::string> async_read_file(EventLoop& loop, const std::string& filename);

// ==================== 测试函数 ====================

void runTests();

} // namespace AsyncIOImpl

#endif // ASYNC_IO_H
