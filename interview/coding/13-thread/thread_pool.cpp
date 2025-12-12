/**
 * @file thread_pool.cpp
 * @brief 线程池实现 - 面试题
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <future>

/**
 * 题目：实现线程池
 *
 * 要求：
 * 1. 固定数量工作线程
 * 2. 支持提交任务
 * 3. 支持获取返回值（future）
 * 4. 支持优雅关闭
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount) {
        // TODO: 创建工作线程
    }

    ~ThreadPool() {
        // TODO: 关闭线程池
    }

    // TODO: 提交任务
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
        using ReturnType = typename std::result_of<F(Args...)>::type;
        return std::future<ReturnType>();
    }

    // TODO: 关闭线程池
    void shutdown() {
    }

    // TODO: 返回线程数量
    size_t size() const {
        return 0;
    }

private:
    // TODO: 工作线程函数
    void workerThread() {
    }

    // TODO: 定义成员变量
};

int main() {
    return 0;
}
