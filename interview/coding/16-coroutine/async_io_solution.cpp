/**
 * @file async_io_solution.cpp
 * @brief 异步 IO - 解答
 */
#include <coroutine>
#include <functional>
#include <queue>
#include <chrono>
#include <string>
#include <optional>
#include <iostream>
#include <thread>
#include <map>
#include <memory>

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
template <>
class Task<void> {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        Task get_return_object() { return Task{handle_type::from_promise(*this)}; }
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

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    ~Task() { if (handle_) handle_.destroy(); }

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
                auto sleepTime = timers_.begin()->first - now;
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

    // 运行协程
    template <typename T>
    void run_until_complete(Task<T>& task) {
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
        loop.schedule_after(100, [this, handle]() mutable {
            result = "Content of " + filename + ": Hello, World!";
            handle.resume();
        });
    }

    std::string await_resume() noexcept {
        return std::move(result);
    }
};

Task<std::string> async_read_file(EventLoop& loop, const std::string& filename) {
    std::cout << "Starting to read: " << filename << "\n";
    auto content = co_await FileReadAwaiter{loop, filename, ""};
    std::cout << "Finished reading: " << filename << "\n";
    co_return content;
}

// ==================== 题目4: 异步 HTTP 请求（模拟）====================
struct HttpGetAwaiter {
    EventLoop& loop;
    std::string url;
    std::string response;

    bool await_ready() noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        // 模拟网络延迟
        loop.schedule_after(200, [this, handle]() mutable {
            response = R"({"status": "ok", "url": ")" + url + R"("})";
            handle.resume();
        });
    }

    std::string await_resume() noexcept {
        return std::move(response);
    }
};

Task<std::string> async_http_get(EventLoop& loop, const std::string& url) {
    std::cout << "Fetching: " << url << "\n";
    auto response = co_await HttpGetAwaiter{loop, url, ""};
    std::cout << "Received response from: " << url << "\n";
    co_return response;
}

// ==================== 题目5: 超时包装器 ====================
template <typename T>
Task<std::optional<T>> with_timeout(EventLoop& loop, Task<T> task, int timeout_ms) {
    // 简化实现：使用标志位
    // 真实实现需要取消机制
    std::cout << "Starting task with " << timeout_ms << "ms timeout\n";

    // 这是简化版本，实际需要竞争机制
    task.start();
    co_return std::nullopt;  // 简化：总是超时
}

// ==================== 综合示例 ====================
Task<void> example_async_workflow(EventLoop& loop) {
    std::cout << "\n=== Async Workflow Start ===\n";

    // 异步睡眠
    std::cout << "Sleeping for 100ms...\n";
    co_await async_sleep(loop, 100);
    std::cout << "Woke up!\n";

    // 异步读文件
    auto content = co_await async_read_file(loop, "test.txt");
    std::cout << "File content: " << content << "\n";

    // 异步 HTTP 请求
    auto response = co_await async_http_get(loop, "https://api.example.com/data");
    std::cout << "HTTP response: " << response << "\n";

    // 并行请求（简化版）
    std::cout << "\nParallel requests:\n";
    auto task1 = async_http_get(loop, "https://api1.example.com");
    auto task2 = async_http_get(loop, "https://api2.example.com");

    task1.start();
    task2.start();

    std::cout << "\n=== Async Workflow End ===\n";
}

int main() {
    std::cout << "=== Async IO Tests ===\n";

    EventLoop loop;

    // 简单睡眠测试
    std::cout << "\n--- Sleep Test ---\n";
    auto sleepTask = [](EventLoop& loop) -> Task<void> {
        std::cout << "Before sleep\n";
        co_await async_sleep(loop, 50);
        std::cout << "After sleep\n";
    }(loop);

    loop.run_until_complete(sleepTask);

    // 完整工作流测试
    std::cout << "\n--- Workflow Test ---\n";
    auto workflowTask = example_async_workflow(loop);
    loop.run_until_complete(workflowTask);

    return 0;
}

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
