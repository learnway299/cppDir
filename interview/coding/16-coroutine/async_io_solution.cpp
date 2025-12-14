/**
 * @file async_io_solution.cpp
 * @brief 异步 IO - 参考答案
 */

#include "async_io.h"
#include <iostream>
#include <thread>
#include <cassert>

namespace AsyncIOImpl {

namespace Solution {

// ==================== Task<T> 实现 ====================

template <typename T>
class Task {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::optional<T> result;
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        Task get_return_object() {
            return Task{handle_type::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept { return {}; }

        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }
                std::coroutine_handle<> await_suspend(handle_type h) noexcept {
                    if (h.promise().continuation) {
                        return h.promise().continuation;
                    }
                    return std::noop_coroutine();
                }
                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }

        void return_value(T value) { result = std::move(value); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    struct Awaiter {
        handle_type handle;
        bool await_ready() noexcept { return false; }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
            handle.promise().continuation = awaiting;
            return handle;
        }
        T await_resume() {
            if (handle.promise().exception) {
                std::rethrow_exception(handle.promise().exception);
            }
            return std::move(*handle.promise().result);
        }
    };

    Awaiter operator co_await() { return Awaiter{handle_}; }

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    ~Task() { if (handle_) handle_.destroy(); }

    void start() { if (handle_ && !handle_.done()) handle_.resume(); }

private:
    handle_type handle_;
};

// Task<void> 特化
class TaskVoid {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        TaskVoid get_return_object() { return TaskVoid{handle_type::from_promise(*this)}; }
        std::suspend_always initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }
                std::coroutine_handle<> await_suspend(handle_type h) noexcept {
                    return h.promise().continuation ? h.promise().continuation : std::noop_coroutine();
                }
                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

    struct Awaiter {
        handle_type handle;
        bool await_ready() noexcept { return false; }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
            handle.promise().continuation = awaiting;
            return handle;
        }
        void await_resume() {
            if (handle.promise().exception) {
                std::rethrow_exception(handle.promise().exception);
            }
        }
    };

    Awaiter operator co_await() { return Awaiter{handle_}; }

    explicit TaskVoid(handle_type handle) : handle_(handle) {}
    TaskVoid(TaskVoid&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    ~TaskVoid() { if (handle_) handle_.destroy(); }

    void start() { if (handle_ && !handle_.done()) handle_.resume(); }

private:
    handle_type handle_;
};

// ==================== 题目2: 事件循环 ====================

class EventLoop {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

    void run() {
        running_ = true;
        while (running_) {
            // 处理定时器
            auto now = std::chrono::steady_clock::now();
            while (!timers_.empty() && timers_.begin()->first <= now) {
                auto it = timers_.begin();
                auto task = std::move(it->second);
                timers_.erase(it);
                task();
            }

            // 处理即时任务
            while (!tasks_.empty()) {
                auto task = std::move(tasks_.front());
                tasks_.pop();
                task();
            }

            // 如果没有任务，短暂睡眠
            if (tasks_.empty() && (timers_.empty() || timers_.begin()->first > now)) {
                if (timers_.empty()) {
                    break;  // 没有更多任务
                }
                auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(timers_.begin()->first - now);
                std::this_thread::sleep_for(std::min(sleepTime, std::chrono::milliseconds(10)));
            }
        }
    }

    void schedule(std::function<void()> task) {
        tasks_.push(std::move(task));
    }

    void schedule_after(int ms, std::function<void()> task) {
        auto when = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
        timers_.emplace(when, std::move(task));
    }

    void stop() {
        running_ = false;
    }

    template <typename TaskType>
    void run_until_complete(TaskType& task) {
        bool done = false;
        schedule([&task, &done, this]() {
            task.start();
            done = true;
            stop();
        });
        run();
    }

private:
    std::queue<std::function<void()>> tasks_;
    std::multimap<TimePoint, std::function<void()>> timers_;
    bool running_ = false;
};

// ==================== 题目1: async_sleep ====================

struct SleepAwaiter {
    EventLoop& loop;
    int ms;

    bool await_ready() noexcept { return ms <= 0; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        loop.schedule_after(ms, [handle]() mutable {
            handle.resume();
        });
    }

    void await_resume() noexcept {}
};

SleepAwaiter async_sleep(EventLoop& loop, int ms) {
    return SleepAwaiter{loop, ms};
}

// ==================== 题目3: 异步文件读取（模拟）====================

struct FileReadAwaiter {
    EventLoop& loop;
    std::string filename;
    std::string result;

    bool await_ready() noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        // 模拟异步读取
        loop.schedule_after(50, [this, handle]() mutable {
            result = "Content of " + filename;
            handle.resume();
        });
    }

    std::string await_resume() noexcept {
        return std::move(result);
    }
};

Task<std::string> async_read_file(EventLoop& loop, const std::string& filename) {
    auto content = co_await FileReadAwaiter{loop, filename, ""};
    co_return content;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Async IO Tests ===" << std::endl;

    Solution::EventLoop loop;

    // 测试事件循环基本功能
    {
        bool taskRan = false;
        loop.schedule([&taskRan]() {
            taskRan = true;
        });
        loop.run();
        assert(taskRan);
    }
    std::cout << "  EventLoop schedule: PASSED" << std::endl;

    // 测试定时器
    {
        auto start = std::chrono::steady_clock::now();
        bool timerFired = false;

        Solution::EventLoop loop2;
        loop2.schedule_after(50, [&timerFired, &loop2]() {
            timerFired = true;
            loop2.stop();
        });
        loop2.run();

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        assert(timerFired);
        assert(elapsed.count() >= 40);  // Allow some tolerance
    }
    std::cout << "  EventLoop timer: PASSED" << std::endl;
}

} // namespace AsyncIOImpl

/**
 * 关键要点：
 *
 * 1. 事件循环：
 *    - 管理任务队列和定时器
 *    - 按顺序执行就绪任务
 *    - 处理定时器到期
 *
 * 2. 自定义 Awaiter：
 *    - await_suspend 注册到事件循环
 *    - 事件完成时调用 handle.resume()
 *    - await_resume 返回结果
 *
 * 3. 异步操作模式：
 *    - 创建 Awaiter 对象
 *    - 在 await_suspend 中启动异步操作
 *    - 操作完成时恢复协程
 *
 * 4. Task 启动：
 *    - initial_suspend = suspend_always
 *    - 需要显式调用 start() 或 resume()
 *    - 允许延迟执行
 *
 * 5. 真实异步 IO：
 *    - 使用 epoll/kqueue/IOCP
 *    - 与操作系统异步 API 集成
 *    - 如 boost::asio, libuv
 *
 * 6. 并行执行：
 *    - 需要调度器支持
 *    - when_all, when_any 组合器
 *    - 取消和超时机制
 */
