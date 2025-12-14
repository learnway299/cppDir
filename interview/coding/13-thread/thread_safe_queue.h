/**
 * @file thread_safe_queue.h
 * @brief 线程安全队列 - 声明
 */

#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace ThreadSafeQueue {

// ==================== 面试者实现 ====================

template <typename T>
class ThreadSafeQueue {
public:
    void push(T value);
    bool try_pop(T& value);
    void wait_and_pop(T& value);
    bool empty() const;

private:
    // TODO
};

// ==================== 参考答案 ====================

template <typename T>
class ThreadSafeQueueSolution {
public:
    void push(T value);
    bool try_pop(T& value);
    void wait_and_pop(T& value);
    std::shared_ptr<T> wait_and_pop();
    bool empty() const;
    size_t size() const;

private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable cv_;
};

// 测试函数
void runTests();

} // namespace ThreadSafeQueue

#endif // THREAD_SAFE_QUEUE_H
