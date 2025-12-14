/**
 * @file spsc_queue.h
 * @brief 单生产者单消费者队列 - 头文件
 *
 * SPSC (Single Producer Single Consumer) 队列是最简单的无锁队列，
 * 因为只有一个生产者和一个消费者，所以不需要复杂的同步机制。
 */
#ifndef SPSC_QUEUE_H
#define SPSC_QUEUE_H

#include <atomic>
#include <cstddef>

namespace SPSCQueue {

// 题目1: 实现基于环形缓冲区的 SPSC 队列
// 使用数组实现，固定容量
template <typename T, size_t Capacity>
class SPSCQueue {
public:
    SPSCQueue() = default;

    // 生产者调用，返回是否成功
    bool push(const T& value);

    // 消费者调用，返回是否成功
    bool pop(T& value);

    bool empty() const;
    bool full() const;
    size_t size() const;

private:
    T buffer_[Capacity];
    std::atomic<size_t> head_{0};  // 消费者读取位置
    std::atomic<size_t> tail_{0};  // 生产者写入位置
};

// 题目2: 优化缓存行对齐
// 避免 head 和 tail 的伪共享 (false sharing)
template <typename T, size_t Capacity>
class SPSCQueueOptimized {
public:
    bool push(const T& value);
    bool pop(T& value);
};

// 题目3: 支持批量操作
template <typename T, size_t Capacity>
class SPSCQueueBatch {
public:
    // 批量推入
    size_t push_batch(const T* values, size_t count);

    // 批量弹出
    size_t pop_batch(T* values, size_t maxCount);
};

// ==================== Solution 类（定义在 spsc_queue_solution.cpp）====================

void runTests();

} // namespace SPSCQueue

#endif // SPSC_QUEUE_H
