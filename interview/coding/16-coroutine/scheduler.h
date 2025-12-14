/**
 * @file scheduler.h
 * @brief 协程调度器 - 题目说明与声明
 *
 * 实现协程调度器，管理多个协程的执行。
 *
 * 包含题目：
 * 1. 简单调度器（FIFO）
 * 2. 多线程调度器
 * 3. 优先级调度器
 * 4. Yield 操作
 * 5. 公平调度器（时间片轮转）
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <coroutine>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <chrono>

namespace SchedulerImpl {

// ==================== 题目1: 简单调度器 ====================

/**
 * 题目1: 实现简单的协程调度器
 *
 * 支持协程的提交和执行（FIFO 顺序）
 *
 * 示例：
 *   SimpleScheduler scheduler;
 *   scheduler.schedule(task1.handle);
 *   scheduler.schedule(task2.handle);
 *   scheduler.run();  // 按顺序执行 task1, task2
 */
class SimpleScheduler {
public:
    // 提交协程到调度器
    void schedule(std::coroutine_handle<> handle);

    // 运行所有已提交的协程
    void run();

    // 是否还有待执行的协程
    bool has_pending() const;

    // 获取调度器实例（用于 Yield）
    static SimpleScheduler*& current();

private:
    std::queue<std::coroutine_handle<>> readyQueue_;
};

// ==================== 题目2: 多线程调度器 ====================

/**
 * 题目2: 实现多线程调度器
 *
 * 支持在多个线程上执行协程
 *
 * 示例：
 *   ThreadPoolScheduler scheduler(4);  // 4 个工作线程
 *   scheduler.schedule(task.handle);
 *   scheduler.wait_idle();  // 等待所有任务完成
 */
class ThreadPoolScheduler {
public:
    explicit ThreadPoolScheduler(size_t numThreads);
    ~ThreadPoolScheduler();

    void schedule(std::coroutine_handle<> handle);
    void shutdown();
    void wait_idle();

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::coroutine_handle<>> taskQueue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::condition_variable idleCv_;
    std::atomic<bool> running_;
    size_t activeWorkers_ = 0;
};

// ==================== 题目3: 优先级调度器 ====================

/**
 * 题目3: 实现优先级调度器
 *
 * 支持不同优先级的协程，高优先级先执行
 *
 * 示例：
 *   PriorityScheduler scheduler;
 *   scheduler.schedule(task1.handle, Priority::Low);
 *   scheduler.schedule(task2.handle, Priority::High);
 *   scheduler.run();  // task2 先执行
 */
class PriorityScheduler {
public:
    enum class Priority { Low = 0, Normal = 1, High = 2 };

    void schedule(std::coroutine_handle<> handle, Priority priority = Priority::Normal);
    void run();
    bool has_pending() const;

private:
    std::queue<std::coroutine_handle<>> queues_[3];
};

// ==================== 题目4: Yield 操作 ====================

class YieldScheduler;

/**
 * 题目4: 实现 yield 操作
 *
 * 让出执行权给其他协程
 *
 * 示例：
 *   Task<void> task(YieldScheduler& sched) {
 *       co_await sched.yield();  // 让出执行权
 *       // 继续执行
 *   }
 */
struct Yield {
    YieldScheduler* scheduler;

    bool await_ready() noexcept;
    void await_suspend(std::coroutine_handle<> handle) noexcept;
    void await_resume() noexcept;
};

class YieldScheduler {
public:
    void schedule(std::coroutine_handle<> handle);
    void run();
    Yield yield();
    static YieldScheduler* current();

private:
    std::queue<std::coroutine_handle<>> readyQueue_;
    static inline thread_local YieldScheduler* current_ = nullptr;
};

// ==================== 题目5: 公平调度器 ====================

/**
 * 题目5: 实现公平调度
 *
 * 使用时间片轮转，防止协程饥饿
 *
 * 示例：
 *   FairScheduler scheduler(10);  // 10ms 时间片
 *   scheduler.schedule(task1.handle);
 *   scheduler.schedule(task2.handle);
 *   scheduler.run();
 */
class FairScheduler {
public:
    explicit FairScheduler(int timeSliceMs = 10);

    void schedule(std::coroutine_handle<> handle);
    void run();

private:
    std::queue<std::coroutine_handle<>> readyQueue_;
    int timeSliceMs_;
};

// ==================== 测试辅助类型 ====================

template <typename T>
struct SchedulerTask {
    struct promise_type {
        T value;
        std::exception_ptr exception;

        SchedulerTask get_return_object() {
            return SchedulerTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T v) { value = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    SchedulerTask(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~SchedulerTask() { if (handle) handle.destroy(); }
};

// void 特化版本
template <>
struct SchedulerTask<void> {
    struct promise_type {
        std::exception_ptr exception;

        SchedulerTask get_return_object() {
            return SchedulerTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    SchedulerTask(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~SchedulerTask() { if (handle) handle.destroy(); }
};

// ==================== 测试函数 ====================

void runTests();

} // namespace SchedulerImpl

#endif // SCHEDULER_H
