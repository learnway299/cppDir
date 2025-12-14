/**
 * @file task_solution.cpp
 * @brief Task 协程 - 参考答案
 */

#include "task.h"
#include <cassert>
#include <vector>
#include <functional>

namespace TaskImpl {

namespace Solution {

// ==================== 题目1: 完整 Task<T> 实现 ====================

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

class TaskVoid {
public:
    struct promise_type {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation;

        TaskVoid get_return_object() {
            return TaskVoid{std::coroutine_handle<promise_type>::from_promise(*this)};
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

    explicit TaskVoid(handle_type handle) : handle_(handle) {}
    TaskVoid(TaskVoid&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    ~TaskVoid() { if (handle_) handle_.destroy(); }

    TaskVoid(const TaskVoid&) = delete;
    TaskVoid& operator=(const TaskVoid&) = delete;

private:
    handle_type handle_;
};

// ==================== 题目3: 简单异步操作 ====================

Task<int> async_add(int a, int b) {
    co_return a + b;
}

Task<int> async_multiply(int a, int b) {
    co_return a * b;
}

// ==================== 题目4: 任务链 ====================

Task<int> chained_task() {
    int sum = co_await async_add(1, 2);           // 3
    int product = co_await async_multiply(sum, 4); // 12
    int final_result = co_await async_add(product, 10); // 22
    co_return final_result;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Task Tests ===" << std::endl;

    // 简单任务
    {
        auto task1 = Solution::async_add(3, 4);
        assert(task1.get() == 7);
    }
    std::cout << "  Simple Task: PASSED" << std::endl;

    // 乘法任务
    {
        auto task2 = Solution::async_multiply(3, 4);
        assert(task2.get() == 12);
    }
    std::cout << "  Multiply Task: PASSED" << std::endl;

    // 链式任务
    {
        auto task3 = Solution::chained_task();
        // 1+2=3, 3*4=12, 12+10=22
        assert(task3.get() == 22);
    }
    std::cout << "  Chained Task: PASSED" << std::endl;
}

} // namespace TaskImpl

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
