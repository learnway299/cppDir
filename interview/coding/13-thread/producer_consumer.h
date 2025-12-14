/**
 * @file producer_consumer.h
 * @brief 生产者消费者问题 - 声明
 */

#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>

namespace ProducerConsumer {

// ==================== 面试者实现 ====================

template <typename T>
class BoundedBuffer {
public:
    explicit BoundedBuffer(size_t capacity);
    void put(T item);
    T take();
    bool tryPut(T item);
    bool tryTake(T& item);
    size_t size() const;

private:
    // TODO
};

template <typename T>
class TimedBoundedBuffer {
public:
    explicit TimedBoundedBuffer(size_t capacity);
    template <typename Rep, typename Period>
    bool putFor(T item, const std::chrono::duration<Rep, Period>& timeout);
    template <typename Rep, typename Period>
    bool takeFor(T& item, const std::chrono::duration<Rep, Period>& timeout);

private:
    // TODO
};

// ==================== 参考答案 ====================

template <typename T>
class BoundedBufferSolution {
public:
    explicit BoundedBufferSolution(size_t capacity);
    void put(T item);
    T take();
    bool tryPut(T item);
    bool tryTake(T& item);
    size_t size() const;
    bool empty() const;
    bool full() const;

private:
    std::queue<T> buffer_;
    size_t capacity_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

template <typename T>
class TimedBoundedBufferSolution {
public:
    explicit TimedBoundedBufferSolution(size_t capacity);
    template <typename Rep, typename Period>
    bool putFor(T item, const std::chrono::duration<Rep, Period>& timeout);
    template <typename Rep, typename Period>
    bool takeFor(T& item, const std::chrono::duration<Rep, Period>& timeout);
    void put(T item);
    T take();

private:
    std::queue<T> buffer_;
    size_t capacity_;
    std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

// 测试函数
void runTests();

} // namespace ProducerConsumer

#endif // PRODUCER_CONSUMER_H
