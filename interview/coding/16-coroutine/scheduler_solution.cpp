/**
 * @file scheduler_solution.cpp
 * @brief 协程调度器 - 参考答案
 */

#include "scheduler.h"
#include <iostream>
#include <cassert>
#include <deque>

namespace SchedulerImpl {

namespace Solution {

// ==================== 题目1: 简单调度器 ====================

class SimpleScheduler {
public:
    void schedule(std::coroutine_handle<> handle) {
        if (handle && !handle.done()) {
            readyQueue_.push(handle);
        }
    }

    void run() {
        while (!readyQueue_.empty()) {
            auto handle = readyQueue_.front();
            readyQueue_.pop();

            if (!handle.done()) {
                handle.resume();
            }
        }
    }

    bool has_pending() const {
        return !readyQueue_.empty();
    }

    // 获取调度器实例（用于 Yield）
    static SimpleScheduler*& current() {
        static thread_local SimpleScheduler* instance = nullptr;
        return instance;
    }

private:
    std::queue<std::coroutine_handle<>> readyQueue_;
};

// ==================== 题目2: 多线程调度器 ====================

class ThreadPoolScheduler {
public:
    explicit ThreadPoolScheduler(size_t numThreads) : running_(true) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }

    ~ThreadPoolScheduler() {
        shutdown();
    }

    void schedule(std::coroutine_handle<> handle) {
        if (!handle || handle.done()) return;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            taskQueue_.push(handle);
        }
        cv_.notify_one();
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();

        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    void wait_idle() {
        std::unique_lock<std::mutex> lock(mutex_);
        idleCv_.wait(lock, [this] {
            return taskQueue_.empty() && activeWorkers_ == 0;
        });
    }

private:
    void workerLoop() {
        while (true) {
            std::coroutine_handle<> handle;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] {
                    return !running_ || !taskQueue_.empty();
                });

                if (!running_ && taskQueue_.empty()) {
                    return;
                }

                handle = taskQueue_.front();
                taskQueue_.pop();
                ++activeWorkers_;
            }

            if (!handle.done()) {
                handle.resume();
            }

            {
                std::lock_guard<std::mutex> lock(mutex_);
                --activeWorkers_;
            }
            idleCv_.notify_all();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::coroutine_handle<>> taskQueue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::condition_variable idleCv_;
    std::atomic<bool> running_;
    size_t activeWorkers_ = 0;
};

// ==================== 题目3: 优先级调度器 ====================

class PriorityScheduler {
public:
    enum class Priority { Low = 0, Normal = 1, High = 2 };

    void schedule(std::coroutine_handle<> handle, Priority priority = Priority::Normal) {
        if (handle && !handle.done()) {
            queues_[static_cast<int>(priority)].push(handle);
        }
    }

    void run() {
        while (has_pending()) {
            // 从高优先级开始检查
            for (int p = 2; p >= 0; --p) {
                if (!queues_[p].empty()) {
                    auto handle = queues_[p].front();
                    queues_[p].pop();
                    if (!handle.done()) {
                        handle.resume();
                    }
                    break;
                }
            }
        }
    }

    bool has_pending() const {
        return !queues_[0].empty() || !queues_[1].empty() || !queues_[2].empty();
    }

private:
    std::queue<std::coroutine_handle<>> queues_[3];
};

// ==================== 题目4: Yield 操作 ====================

class YieldScheduler;

struct Yield {
    YieldScheduler* scheduler;

    bool await_ready() noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) noexcept;
    void await_resume() noexcept {}
};

class YieldScheduler {
public:
    void schedule(std::coroutine_handle<> handle) {
        if (handle && !handle.done()) {
            readyQueue_.push(handle);
        }
    }

    void run() {
        current_ = this;
        while (!readyQueue_.empty()) {
            auto handle = readyQueue_.front();
            readyQueue_.pop();
            if (!handle.done()) {
                handle.resume();
            }
        }
        current_ = nullptr;
    }

    Yield yield() {
        return Yield{this};
    }

    static YieldScheduler* current() {
        return current_;
    }

private:
    std::queue<std::coroutine_handle<>> readyQueue_;
    static inline thread_local YieldScheduler* current_ = nullptr;
};

void Yield::await_suspend(std::coroutine_handle<> handle) noexcept {
    if (scheduler) {
        scheduler->schedule(handle);
    }
}

// ==================== 题目5: 公平调度器（时间片轮转）====================

class FairScheduler {
public:
    explicit FairScheduler(int timeSliceMs = 10)
        : timeSliceMs_(timeSliceMs) {}

    void schedule(std::coroutine_handle<> handle) {
        if (handle && !handle.done()) {
            readyQueue_.push(handle);
        }
    }

    void run() {
        while (!readyQueue_.empty()) {
            auto handle = readyQueue_.front();
            readyQueue_.pop();

            if (!handle.done()) {
                // 设置时间片开始
                auto start = std::chrono::steady_clock::now();

                handle.resume();

                // 如果协程未完成且超过时间片，重新调度
                if (!handle.done()) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start);
                    if (elapsed.count() >= timeSliceMs_) {
                        // 重新排队
                        readyQueue_.push(handle);
                    }
                }
            }
        }
    }

private:
    std::queue<std::coroutine_handle<>> readyQueue_;
    int timeSliceMs_;
};

// ==================== Task 类型（用于测试）====================

template <typename T = void>
struct Task {
    struct promise_type {
        T value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T v) { value = v; }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }
};

// 测试辅助协程
Task<int> simpleTask(int id) {
    co_return id;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Scheduler Tests ===" << std::endl;

    // 简单调度器测试
    {
        Solution::SimpleScheduler scheduler;
        auto task1 = Solution::simpleTask(1);
        auto task2 = Solution::simpleTask(2);
        auto task3 = Solution::simpleTask(3);

        scheduler.schedule(task1.handle);
        scheduler.schedule(task2.handle);
        scheduler.schedule(task3.handle);
        scheduler.run();

        assert(task1.handle.done());
        assert(task2.handle.done());
        assert(task3.handle.done());
    }
    std::cout << "  SimpleScheduler: PASSED" << std::endl;

    // 优先级调度器测试
    {
        Solution::PriorityScheduler scheduler;

        auto lowTask = Solution::simpleTask(1);
        auto normalTask = Solution::simpleTask(2);
        auto highTask = Solution::simpleTask(3);

        scheduler.schedule(lowTask.handle, Solution::PriorityScheduler::Priority::Low);
        scheduler.schedule(normalTask.handle, Solution::PriorityScheduler::Priority::Normal);
        scheduler.schedule(highTask.handle, Solution::PriorityScheduler::Priority::High);

        scheduler.run();

        assert(lowTask.handle.done());
        assert(normalTask.handle.done());
        assert(highTask.handle.done());
    }
    std::cout << "  PriorityScheduler: PASSED" << std::endl;

    // 多线程调度器测试
    {
        Solution::ThreadPoolScheduler scheduler(2);

        auto task1 = Solution::simpleTask(1);
        auto task2 = Solution::simpleTask(2);

        scheduler.schedule(task1.handle);
        scheduler.schedule(task2.handle);

        scheduler.wait_idle();

        assert(task1.handle.done());
        assert(task2.handle.done());
    }
    std::cout << "  ThreadPoolScheduler: PASSED" << std::endl;
}

} // namespace SchedulerImpl

/**
 * 关键要点：
 *
 * 1. 调度器职责：
 *    - 管理就绪队列
 *    - 选择下一个执行的协程
 *    - 恢复协程执行
 *
 * 2. 调度策略：
 *    - FIFO：简单公平
 *    - 优先级：按重要性排序
 *    - 时间片轮转：防止饥饿
 *    - 工作窃取：负载均衡
 *
 * 3. Yield 实现：
 *    - await_suspend 将当前协程重新入队
 *    - 让出 CPU 给其他协程
 *    - 需要调度器协作
 *
 * 4. 多线程注意事项：
 *    - 队列访问需要同步
 *    - 条件变量避免忙等
 *    - 优雅关闭
 *
 * 5. 工作窃取：
 *    - 每个线程有自己的队列
 *    - 空闲时从其他队列窃取
 *    - 使用双端队列（本地 LIFO，窃取 FIFO）
 *
 * 6. 实际应用：
 *    - libcoro, cppcoro
 *    - folly::coro
 *    - 与事件循环集成
 */
