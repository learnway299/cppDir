/**
 * @file mpmc_queue.cpp
 * @brief 多生产者多消费者队列 - 面试题实现
 */
#include "mpmc_queue.h"

namespace MPMCQueue {

// ==================== 面试者实现区域 ====================

// 题目1: 实现有界 MPMC 队列
template <typename T, size_t Capacity>
MPMCQueue<T, Capacity>::MPMCQueue() {
    // TODO: 初始化序列号
}

template <typename T, size_t Capacity>
bool MPMCQueue<T, Capacity>::push(const T& value) {
    // TODO: 实现无锁 push
    return false;
}

template <typename T, size_t Capacity>
bool MPMCQueue<T, Capacity>::pop(T& value) {
    // TODO: 实现无锁 pop
    return false;
}

// 题目2: Michael & Scott 无锁队列
template <typename T>
MSQueue<T>::MSQueue() {
    // TODO: 初始化哨兵节点
}

template <typename T>
MSQueue<T>::~MSQueue() {
    // TODO: 清理节点
}

template <typename T>
void MSQueue<T>::push(const T& value) {
    // TODO: 实现 M&S 队列的 push
}

template <typename T>
bool MSQueue<T>::pop(T& value) {
    // TODO: 实现 M&S 队列的 pop
    return false;
}

// 题目3: 带背压的 MPMC 队列
template <typename T, size_t Capacity>
void MPMCQueueBlocking<T, Capacity>::push(const T& value) {
    // TODO: 阻塞直到成功 push
}

template <typename T, size_t Capacity>
T MPMCQueueBlocking<T, Capacity>::pop() {
    // TODO: 阻塞直到成功 pop
    return T{};
}

template <typename T, size_t Capacity>
bool MPMCQueueBlocking<T, Capacity>::try_push(const T& value) {
    // TODO: 非阻塞 push
    return false;
}

template <typename T, size_t Capacity>
bool MPMCQueueBlocking<T, Capacity>::try_pop(T& value) {
    // TODO: 非阻塞 pop
    return false;
}

// 显式实例化常用模板
template class MPMCQueue<int, 1024>;
template class MSQueue<int>;
template class MPMCQueueBlocking<int, 1024>;

} // namespace MPMCQueue
