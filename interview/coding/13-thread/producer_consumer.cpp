/**
 * @file producer_consumer.cpp
 * @brief 生产者消费者问题 - 面试题实现
 */

#include "producer_consumer.h"

namespace ProducerConsumer {

// ==================== 面试者实现 ====================

template <typename T>
BoundedBuffer<T>::BoundedBuffer(size_t capacity) {
    // TODO
}

template <typename T>
void BoundedBuffer<T>::put(T item) {
    // TODO
}

template <typename T>
T BoundedBuffer<T>::take() {
    // TODO
    return T();
}

template <typename T>
bool BoundedBuffer<T>::tryPut(T item) {
    // TODO
    return false;
}

template <typename T>
bool BoundedBuffer<T>::tryTake(T& item) {
    // TODO
    return false;
}

template <typename T>
size_t BoundedBuffer<T>::size() const {
    // TODO
    return 0;
}

// Explicit instantiation
template class BoundedBuffer<int>;

template <typename T>
TimedBoundedBuffer<T>::TimedBoundedBuffer(size_t capacity) {
    // TODO
}

template <typename T>
template <typename Rep, typename Period>
bool TimedBoundedBuffer<T>::putFor(T item, const std::chrono::duration<Rep, Period>& timeout) {
    // TODO
    return false;
}

template <typename T>
template <typename Rep, typename Period>
bool TimedBoundedBuffer<T>::takeFor(T& item, const std::chrono::duration<Rep, Period>& timeout) {
    // TODO
    return false;
}

// Explicit instantiation
template class TimedBoundedBuffer<int>;

} // namespace ProducerConsumer
