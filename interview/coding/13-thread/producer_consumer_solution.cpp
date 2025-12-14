/**
 * @file producer_consumer_solution.cpp
 * @brief 生产者消费者问题 - 参考答案
 */

#include "producer_consumer.h"
#include <cassert>

namespace ProducerConsumer {

// ==================== 参考答案实现 ====================

template <typename T>
BoundedBufferSolution<T>::BoundedBufferSolution(size_t capacity) : capacity_(capacity) {}

template <typename T>
void BoundedBufferSolution<T>::put(T item) {
    std::unique_lock<std::mutex> lock(mutex_);
    notFull_.wait(lock, [this] { return buffer_.size() < capacity_; });
    buffer_.push(std::move(item));
    notEmpty_.notify_one();
}

template <typename T>
T BoundedBufferSolution<T>::take() {
    std::unique_lock<std::mutex> lock(mutex_);
    notEmpty_.wait(lock, [this] { return !buffer_.empty(); });
    T item = std::move(buffer_.front());
    buffer_.pop();
    notFull_.notify_one();
    return item;
}

template <typename T>
bool BoundedBufferSolution<T>::tryPut(T item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.size() >= capacity_) return false;
    buffer_.push(std::move(item));
    notEmpty_.notify_one();
    return true;
}

template <typename T>
bool BoundedBufferSolution<T>::tryTake(T& item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.empty()) return false;
    item = std::move(buffer_.front());
    buffer_.pop();
    notFull_.notify_one();
    return true;
}

template <typename T>
size_t BoundedBufferSolution<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

template <typename T>
bool BoundedBufferSolution<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.empty();
}

template <typename T>
bool BoundedBufferSolution<T>::full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size() >= capacity_;
}

// Explicit instantiation
template class BoundedBufferSolution<int>;

template <typename T>
TimedBoundedBufferSolution<T>::TimedBoundedBufferSolution(size_t capacity) : capacity_(capacity) {}

template <typename T>
template <typename Rep, typename Period>
bool TimedBoundedBufferSolution<T>::putFor(T item, const std::chrono::duration<Rep, Period>& timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!notFull_.wait_for(lock, timeout, [this] { return buffer_.size() < capacity_; })) {
        return false;
    }
    buffer_.push(std::move(item));
    notEmpty_.notify_one();
    return true;
}

template <typename T>
template <typename Rep, typename Period>
bool TimedBoundedBufferSolution<T>::takeFor(T& item, const std::chrono::duration<Rep, Period>& timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!notEmpty_.wait_for(lock, timeout, [this] { return !buffer_.empty(); })) {
        return false;
    }
    item = std::move(buffer_.front());
    buffer_.pop();
    notFull_.notify_one();
    return true;
}

template <typename T>
void TimedBoundedBufferSolution<T>::put(T item) {
    std::unique_lock<std::mutex> lock(mutex_);
    notFull_.wait(lock, [this] { return buffer_.size() < capacity_; });
    buffer_.push(std::move(item));
    notEmpty_.notify_one();
}

template <typename T>
T TimedBoundedBufferSolution<T>::take() {
    std::unique_lock<std::mutex> lock(mutex_);
    notEmpty_.wait(lock, [this] { return !buffer_.empty(); });
    T item = std::move(buffer_.front());
    buffer_.pop();
    notFull_.notify_one();
    return item;
}

// Explicit instantiation
template class TimedBoundedBufferSolution<int>;
template bool TimedBoundedBufferSolution<int>::putFor(int, const std::chrono::milliseconds&);
template bool TimedBoundedBufferSolution<int>::takeFor(int&, const std::chrono::milliseconds&);

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Producer Consumer Tests ===" << std::endl;

    // 测试基础生产者消费者
    {
        BoundedBufferSolution<int> buffer(5);
        std::atomic<int> produced{0};
        std::atomic<int> consumed{0};

        std::thread producer([&buffer, &produced] {
            for (int i = 0; i < 10; ++i) {
                buffer.put(i);
                ++produced;
            }
        });

        std::thread consumer([&buffer, &consumed] {
            for (int i = 0; i < 10; ++i) {
                buffer.take();
                ++consumed;
            }
        });

        producer.join();
        consumer.join();

        assert(produced == 10);
        assert(consumed == 10);
        assert(buffer.empty());
    }
    std::cout << "  BoundedBuffer: PASSED" << std::endl;

    // 测试带超时的生产者消费者
    {
        TimedBoundedBufferSolution<int> buffer(2);

        buffer.put(1);
        buffer.put(2);

        bool putSuccess = buffer.putFor(3, std::chrono::milliseconds(10));
        assert(!putSuccess);  // 应该超时

        int item;
        bool takeSuccess = buffer.takeFor(item, std::chrono::milliseconds(100));
        assert(takeSuccess && item == 1);
    }
    std::cout << "  TimedBoundedBuffer: PASSED" << std::endl;
}

} // namespace ProducerConsumer
