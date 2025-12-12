/**
 * @file scheduler.cpp
 * @brief 协程调度器 - 面试题
 *
 * 实现协程调度器，管理多个协程的执行。
 */
#include <coroutine>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

// 题目1: 实现简单的协程调度器
// 支持协程的提交和执行
class SimpleScheduler {
public:
    // 提交协程到调度器
    void schedule(std::coroutine_handle<> handle) { /* TODO */ }

    // 运行所有已提交的协程
    void run() { /* TODO */ }

    // 是否还有待执行的协程
    bool has_pending() const { return false; /* TODO */ }

private:
    // TODO: 协程队列
};

// 题目2: 实现多线程调度器
// 支持在多个线程上执行协程
class ThreadPoolScheduler {
public:
    explicit ThreadPoolScheduler(size_t numThreads) { /* TODO */ }
    ~ThreadPoolScheduler() { /* TODO */ }

    void schedule(std::coroutine_handle<> handle) { /* TODO */ }

    void shutdown() { /* TODO */ }

private:
    // TODO: 线程池
    // TODO: 任务队列
    // TODO: 同步原语
};

// 题目3: 实现优先级调度器
// 支持不同优先级的协程
class PriorityScheduler {
public:
    enum class Priority { Low, Normal, High };

    void schedule(std::coroutine_handle<> handle, Priority priority = Priority::Normal) {
        /* TODO */
    }

    void run() { /* TODO */ }

private:
    // TODO: 优先级队列
};

// 题目4: 实现 yield 操作
// 让出执行权给其他协程
struct Yield {
    // TODO: 实现 awaitable 接口
};

// 题目5: 实现公平调度
// 使用时间片轮转
class FairScheduler {
public:
    void schedule(std::coroutine_handle<> handle) { /* TODO */ }
    void run() { /* TODO */ }

private:
    // TODO
};

int main() {
    return 0;
}
