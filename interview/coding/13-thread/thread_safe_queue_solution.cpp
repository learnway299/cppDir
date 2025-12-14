/**
 * @file thread_safe_queue_solution.cpp
 * @brief 线程安全队列 - 参考答案
 */

#include "thread_safe_queue.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

namespace ThreadSafeQueue {

// ==================== 参考答案实现 ====================

template <typename T>
void ThreadSafeQueueSolution<T>::push(T value) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
    }
    cv_.notify_one();
}

template <typename T>
bool ThreadSafeQueueSolution<T>::try_pop(T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return false;
    }
    value = std::move(queue_.front());
    queue_.pop();
    return true;
}

template <typename T>
void ThreadSafeQueueSolution<T>::wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty(); });
    value = std::move(queue_.front());
    queue_.pop();
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueueSolution<T>::wait_and_pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty(); });
    auto result = std::make_shared<T>(std::move(queue_.front()));
    queue_.pop();
    return result;
}

template <typename T>
bool ThreadSafeQueueSolution<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template <typename T>
size_t ThreadSafeQueueSolution<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

// Explicit instantiation
template class ThreadSafeQueueSolution<int>;

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Thread Safe Queue Tests ===" << std::endl;

    // 测试基本push和pop
    {
        ThreadSafeQueueSolution<int> queue;

        std::thread producer([&queue] {
            for (int i = 0; i < 10; ++i) {
                queue.push(i);
            }
        });

        std::thread consumer([&queue] {
            for (int i = 0; i < 10; ++i) {
                int value;
                queue.wait_and_pop(value);
                assert(value >= 0 && value < 10);
            }
        });

        producer.join();
        consumer.join();

        assert(queue.empty());
    }
    std::cout << "  Push and Pop: PASSED" << std::endl;

    // 测试try_pop
    {
        ThreadSafeQueueSolution<int> queue;
        queue.push(42);

        int value;
        bool success = queue.try_pop(value);
        assert(success && value == 42);

        success = queue.try_pop(value);
        assert(!success);
    }
    std::cout << "  Try Pop: PASSED" << std::endl;

    // 测试多生产者多消费者
    {
        ThreadSafeQueueSolution<int> queue;
        std::atomic<int> produced{0};
        std::atomic<int> consumed{0};

        std::vector<std::thread> threads;

        // 3个生产者
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back([&queue, &produced] {
                for (int j = 0; j < 10; ++j) {
                    queue.push(j);
                    ++produced;
                }
            });
        }

        // 3个消费者
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back([&queue, &consumed] {
                for (int j = 0; j < 10; ++j) {
                    int value;
                    queue.wait_and_pop(value);
                    ++consumed;
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        assert(produced == 30);
        assert(consumed == 30);
        assert(queue.empty());
    }
    std::cout << "  Multi Producer-Consumer: PASSED" << std::endl;
}

} // namespace ThreadSafeQueue
