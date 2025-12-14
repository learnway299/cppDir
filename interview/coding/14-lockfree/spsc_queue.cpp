/**
 * @file spsc_queue.cpp
 * @brief 单生产者单消费者队列 - 面试题实现
 */
#include "spsc_queue.h"

namespace SPSCQueue {

// ==================== 面试者实现区域 ====================

// 题目1: 实现基于环形缓冲区的 SPSC 队列
template <typename T, size_t Capacity>
bool SPSCQueue<T, Capacity>::push(const T& value) {
    // TODO: 生产者 push
    return false;
}

template <typename T, size_t Capacity>
bool SPSCQueue<T, Capacity>::pop(T& value) {
    // TODO: 消费者 pop
    return false;
}

template <typename T, size_t Capacity>
bool SPSCQueue<T, Capacity>::empty() const {
    // TODO: 检查是否为空
    return true;
}

template <typename T, size_t Capacity>
bool SPSCQueue<T, Capacity>::full() const {
    // TODO: 检查是否已满
    return false;
}

template <typename T, size_t Capacity>
size_t SPSCQueue<T, Capacity>::size() const {
    // TODO: 返回当前大小
    return 0;
}

// 题目2: 优化缓存行对齐
template <typename T, size_t Capacity>
bool SPSCQueueOptimized<T, Capacity>::push(const T& value) {
    // TODO: 使用缓存行对齐优化
    return false;
}

template <typename T, size_t Capacity>
bool SPSCQueueOptimized<T, Capacity>::pop(T& value) {
    // TODO: 使用缓存行对齐优化
    return false;
}

// 题目3: 支持批量操作
template <typename T, size_t Capacity>
size_t SPSCQueueBatch<T, Capacity>::push_batch(const T* values, size_t count) {
    // TODO: 批量 push
    return 0;
}

template <typename T, size_t Capacity>
size_t SPSCQueueBatch<T, Capacity>::pop_batch(T* values, size_t maxCount) {
    // TODO: 批量 pop
    return 0;
}

// 显式实例化常用模板
template class SPSCQueue<int, 1024>;
template class SPSCQueueOptimized<int, 1024>;
template class SPSCQueueBatch<int, 1024>;

} // namespace SPSCQueue
