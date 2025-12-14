/**
 * @file thread_pool.h
 * @brief 线程池实现 - 声明
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <memory>

namespace ThreadPool {

// ==================== 面试者实现 ====================

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount);
    ~ThreadPool();

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    void shutdown();
    size_t size() const;

private:
    void workerThread();
    // TODO: 定义成员变量
};

// ==================== 参考答案 ====================

class ThreadPoolSolution {
public:
    explicit ThreadPoolSolution(size_t threadCount);
    ~ThreadPoolSolution();

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    void shutdown();
    size_t size() const;
    size_t pendingTasks() const;

private:
    void workerThread();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;
};

// 测试函数
void runTests();

} // namespace ThreadPool

#endif // THREAD_POOL_H
