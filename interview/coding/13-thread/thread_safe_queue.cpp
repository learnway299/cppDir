/**
 * @file thread_safe_queue.cpp
 * @brief 线程安全队列 - 面试题实现
 */

#include "thread_safe_queue.h"

namespace ThreadSafeQueue {

// ==================== 面试者实现 ====================

template <typename T>
void ThreadSafeQueue<T>::push(T value) {
    // TODO
}

template <typename T>
bool ThreadSafeQueue<T>::try_pop(T& value) {
    // TODO
    return false;
}

template <typename T>
void ThreadSafeQueue<T>::wait_and_pop(T& value) {
    // TODO
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const {
    // TODO
    return true;
}

// Explicit instantiation
template class ThreadSafeQueue<int>;

} // namespace ThreadSafeQueue
