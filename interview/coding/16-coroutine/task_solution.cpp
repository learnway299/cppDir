/**
 * @file task_solution.cpp
 * @brief Task 协程 - 解答
 */
#include <coroutine>
#include <iostream>
#include <optional>
#include <exception>
#include <variant>
#include <vector>
#include <functional>

// ==================== 题目1: 基本 Task<T> 实现 ====================
template <typename T>
class Task {
public:
    struct promise_type {
        std::variant<std::monostate, T, std::exception_ptr> result;
        std::coroutine_handle<> continuation;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 立即执行到第一个挂起点
        std::suspend_never initial_suspend() noexcept { return {}; }

        // 结束时检查是否有等待者
        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                    if (h.promise().continuation) {
                        return h.promise().continuation;
                    }
                    return std::noop_coroutine();
                }

                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }

        void return_value(T value) {
            result.template emplace<1>(std::move(value));
        }

        void unhandled_exception() {
            result.template emplace<2>(std::current_exception());
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    // Awaiter for co_await
    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept {
            return handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
            handle.promise().continuation = awaiting;
            return handle;
        }

        T await_resume() {
            auto& result = handle.promise().result;
            if (std::holds_alternative<std::exception_ptr>(result)) {
                std::rethrow_exception(std::get<std::exception_ptr>(result));
            }
            return std::get<T>(result);
        }
    };

    Awaiter operator co_await() {
        return Awaiter{handle_};
    }

    // 同步获取结果
    T get() {
        if (!handle_.done()) {
            handle_.resume();
        }
        auto& result = handle_.promise().result;
        if (std::holds_alternative<std::exception_ptr>(result)) {
            std::rethrow_exception(std::get<std::exception_ptr>(result));
        }
        return std::get<T>(result);
    }

    explicit Task(handle_type handle) : handle_(handle) {}

    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

private:
    handle_type handle_;
};

// ==================== Task<void> 特化 ====================
template <>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_never initial_suspend() noexcept { return {}; }

        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }
                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                    if (h.promise().continuation) {
                        return h.promise().continuation;
                    }
                    return std::noop_coroutine();
                }
                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }

        void return_void() {}

        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept { return handle.done(); }

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

    void get() {
        if (!handle_.done()) {
            handle_.resume();
        }
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
    }

    explicit Task(handle_type handle) : handle_(handle) {}
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    ~Task() { if (handle_) handle_.destroy(); }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

private:
    handle_type handle_;
};

// ==================== 题目2: 简单异步操作 ====================
Task<int> async_add(int a, int b) {
    std::cout << "Computing " << a << " + " << b << "\n";
    co_return a + b;
}

Task<int> async_multiply(int a, int b) {
    std::cout << "Computing " << a << " * " << b << "\n";
    co_return a * b;
}

// ==================== 题目3: 任务链 ====================
Task<int> chained_task() {
    std::cout << "Starting chained task\n";

    int sum = co_await async_add(1, 2);
    std::cout << "Sum = " << sum << "\n";

    int product = co_await async_multiply(sum, 4);
    std::cout << "Product = " << product << "\n";

    int final_result = co_await async_add(product, 10);
    std::cout << "Final = " << final_result << "\n";

    co_return final_result;
}

// ==================== 题目4: when_all (简化版) ====================
// 注意：完整实现需要调度器支持
Task<void> when_all_simple(std::vector<Task<void>> tasks) {
    for (auto& task : tasks) {
        co_await task;
    }
}

// 收集结果的版本
template <typename T>
Task<std::vector<T>> collect(std::vector<Task<T>> tasks) {
    std::vector<T> results;
    results.reserve(tasks.size());
    for (auto& task : tasks) {
        results.push_back(co_await task);
    }
    co_return results;
}

// ==================== 自定义 Awaitable ====================
// 可恢复的简单 awaitable
struct SuspendAndResume {
    bool await_ready() noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        // 模拟异步操作：立即恢复
        handle.resume();
    }

    void await_resume() noexcept {}
};

// 带值的 awaitable
template <typename T>
struct ValueAwaitable {
    T value;

    bool await_ready() noexcept { return true; }
    void await_suspend(std::coroutine_handle<>) noexcept {}
    T await_resume() noexcept { return value; }
};

// 延迟执行的 awaitable
struct DelayedExecution {
    std::function<void()> callback;

    bool await_ready() noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        callback = [handle]() mutable { handle.resume(); };
        // 在真实场景中，这里会将 callback 注册到事件循环
    }

    void await_resume() noexcept {}

    void complete() {
        if (callback) callback();
    }
};

// ==================== 测试代码 ====================
Task<void> example_coroutine() {
    std::cout << "Step 1\n";
    int result = co_await async_add(10, 20);
    std::cout << "Step 2: result = " << result << "\n";
    result = co_await async_multiply(result, 2);
    std::cout << "Step 3: result = " << result << "\n";
}

int main() {
    std::cout << "=== Task Coroutine Tests ===\n\n";

    // 简单任务
    std::cout << "--- Simple Task ---\n";
    auto task1 = async_add(3, 4);
    std::cout << "Result: " << task1.get() << "\n\n";

    // 链式任务
    std::cout << "--- Chained Task ---\n";
    auto task2 = chained_task();
    std::cout << "Final result: " << task2.get() << "\n\n";

    // void 任务
    std::cout << "--- Void Task ---\n";
    auto task3 = example_coroutine();
    task3.get();

    return 0;
}

/**
 * 关键要点：
 *
 * 1. Task vs Generator：
 *    - Generator: 产生多个值 (co_yield)
 *    - Task: 产生一个结果 (co_return)
 *
 * 2. Awaitable 三部曲：
 *    - await_ready(): 是否立即可用
 *    - await_suspend(): 挂起时做什么
 *    - await_resume(): 恢复时返回什么
 *
 * 3. 任务续体 (Continuation)：
 *    - 存储等待当前任务的协程句柄
 *    - final_suspend 中恢复等待者
 *    - 形成任务链
 *
 * 4. 错误处理：
 *    - unhandled_exception 捕获异常
 *    - 存储 exception_ptr
 *    - await_resume 中重新抛出
 *
 * 5. 同步 vs 异步：
 *    - get(): 同步等待结果
 *    - co_await: 异步等待
 *
 * 6. 调度器集成：
 *    - 真实异步需要事件循环
 *    - await_suspend 返回 handle 控制执行流
 *    - noop_coroutine 表示不继续执行
 */
