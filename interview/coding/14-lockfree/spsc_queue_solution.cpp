/**
 * @file spsc_queue_solution.cpp
 * @brief 单生产者单消费者队列 - 参考答案
 */
#include "spsc_queue.h"
#include <array>
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

namespace SPSCQueue {

static constexpr size_t CACHE_LINE_SIZE = 64;

// ==================== 题目1: 基本 SPSC 队列 ====================

template <typename T, size_t Capacity>
class SPSCQueueBasicSolution {
public:
    SPSCQueueBasicSolution() = default;

    bool push(const T& value) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t nextTail = (currentTail + 1) % Capacity;

        if (nextTail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[currentTail] = value;
        tail_.store(nextTail, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        size_t currentHead = head_.load(std::memory_order_relaxed);

        if (currentHead == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        value = buffer_[currentHead];
        head_.store((currentHead + 1) % Capacity, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head_.load(std::memory_order_relaxed) ==
               tail_.load(std::memory_order_relaxed);
    }

    bool full() const {
        return ((tail_.load(std::memory_order_relaxed) + 1) % Capacity) ==
               head_.load(std::memory_order_relaxed);
    }

    size_t size() const {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail - head + Capacity) % Capacity;
    }

private:
    std::array<T, Capacity> buffer_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};

// 显式实例化
template class SPSCQueueBasicSolution<int, 1024>;

// ==================== 题目2: 缓存行优化版本 ====================

template <typename T, size_t Capacity>
class SPSCQueueOptimizedSolution {
public:
    SPSCQueueOptimizedSolution() = default;

    bool push(const T& value) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t nextTail = (currentTail + 1) % Capacity;

        if (nextTail == cachedHead_) {
            cachedHead_ = head_.load(std::memory_order_acquire);
            if (nextTail == cachedHead_) {
                return false;
            }
        }

        buffer_[currentTail] = value;
        tail_.store(nextTail, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        size_t currentHead = head_.load(std::memory_order_relaxed);

        if (currentHead == cachedTail_) {
            cachedTail_ = tail_.load(std::memory_order_acquire);
            if (currentHead == cachedTail_) {
                return false;
            }
        }

        value = buffer_[currentHead];
        head_.store((currentHead + 1) % Capacity, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head_.load(std::memory_order_relaxed) ==
               tail_.load(std::memory_order_relaxed);
    }

private:
    std::array<T, Capacity> buffer_;

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0};
    size_t cachedHead_{0};

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0};
    size_t cachedTail_{0};
};

// 显式实例化
template class SPSCQueueOptimizedSolution<int, 1024>;

// ==================== 题目3: 支持批量操作 ====================

template <typename T, size_t Capacity>
class SPSCQueueBatchSolution {
public:
    size_t push_batch(const T* values, size_t count) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t currentHead = head_.load(std::memory_order_acquire);

        size_t available = (currentHead - currentTail - 1 + Capacity) % Capacity;
        size_t toPush = std::min(count, available);

        for (size_t i = 0; i < toPush; ++i) {
            buffer_[(currentTail + i) % Capacity] = values[i];
        }

        tail_.store((currentTail + toPush) % Capacity, std::memory_order_release);
        return toPush;
    }

    size_t pop_batch(T* values, size_t maxCount) {
        size_t currentHead = head_.load(std::memory_order_relaxed);
        size_t currentTail = tail_.load(std::memory_order_acquire);

        size_t available = (currentTail - currentHead + Capacity) % Capacity;
        size_t toPop = std::min(maxCount, available);

        for (size_t i = 0; i < toPop; ++i) {
            values[i] = buffer_[(currentHead + i) % Capacity];
        }

        head_.store((currentHead + toPop) % Capacity, std::memory_order_release);
        return toPop;
    }

    bool push(const T& value) {
        return push_batch(&value, 1) == 1;
    }

    bool pop(T& value) {
        return pop_batch(&value, 1) == 1;
    }

    bool empty() const {
        return head_.load(std::memory_order_relaxed) ==
               tail_.load(std::memory_order_relaxed);
    }

private:
    std::array<T, Capacity> buffer_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0};
};

// 显式实例化
template class SPSCQueueBatchSolution<int, 1024>;

// ==================== 题目4: 无界 SPSC 队列（链表实现）====================

template <typename T>
struct SPSCNode {
    T data;
    std::atomic<SPSCNode*> next{nullptr};

    SPSCNode() = default;
    SPSCNode(const T& value) : data(value) {}
};

template <typename T>
class SPSCQueueUnboundedSolution {
public:
    SPSCQueueUnboundedSolution() {
        SPSCNode<T>* dummy = new SPSCNode<T>();
        head_.store(dummy);
        tail_ = dummy;
    }

    ~SPSCQueueUnboundedSolution() {
        T dummy;
        while (pop(dummy)) {}
        delete head_.load();
    }

    void push(const T& value) {
        SPSCNode<T>* newNode = new SPSCNode<T>(value);
        tail_->next.store(newNode, std::memory_order_release);
        tail_ = newNode;
    }

    bool pop(T& value) {
        SPSCNode<T>* head = head_.load(std::memory_order_relaxed);
        SPSCNode<T>* next = head->next.load(std::memory_order_acquire);

        if (next == nullptr) {
            return false;
        }

        value = next->data;
        head_.store(next, std::memory_order_release);
        delete head;
        return true;
    }

    bool empty() const {
        SPSCNode<T>* head = head_.load(std::memory_order_relaxed);
        return head->next.load(std::memory_order_relaxed) == nullptr;
    }

private:
    alignas(CACHE_LINE_SIZE) std::atomic<SPSCNode<T>*> head_;
    alignas(CACHE_LINE_SIZE) SPSCNode<T>* tail_;
};

// 显式实例化
template class SPSCQueueUnboundedSolution<int>;

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== SPSC Queue Tests ===" << std::endl;

    // 测试基本 SPSC 队列
    {
        SPSCQueueBasicSolution<int, 1024> queue;
        std::atomic<long long> sum{0};

        std::thread producer([&queue] {
            for (int i = 0; i < 10000; ++i) {
                while (!queue.push(i)) {
                    std::this_thread::yield();
                }
            }
        });

        std::thread consumer([&queue, &sum] {
            int value;
            int count = 0;
            while (count < 10000) {
                if (queue.pop(value)) {
                    sum.fetch_add(value, std::memory_order_relaxed);
                    ++count;
                }
            }
        });

        producer.join();
        consumer.join();

        long long expected = (long long)9999 * 10000 / 2;
        assert(sum == expected);
        assert(queue.empty());
    }
    std::cout << "  Basic SPSC Queue: PASSED" << std::endl;

    // 测试优化版 SPSC 队列
    {
        SPSCQueueOptimizedSolution<int, 1024> queue;
        std::atomic<long long> sum{0};

        std::thread producer([&queue] {
            for (int i = 0; i < 10000; ++i) {
                while (!queue.push(i)) {
                    std::this_thread::yield();
                }
            }
        });

        std::thread consumer([&queue, &sum] {
            int value;
            int count = 0;
            while (count < 10000) {
                if (queue.pop(value)) {
                    sum.fetch_add(value, std::memory_order_relaxed);
                    ++count;
                }
            }
        });

        producer.join();
        consumer.join();

        long long expected = (long long)9999 * 10000 / 2;
        assert(sum == expected);
    }
    std::cout << "  Optimized SPSC Queue: PASSED" << std::endl;

    // 测试无界 SPSC 队列
    {
        SPSCQueueUnboundedSolution<int> queue;

        queue.push(1);
        queue.push(2);
        queue.push(3);

        int value;
        assert(queue.pop(value) && value == 1);
        assert(queue.pop(value) && value == 2);
        assert(queue.pop(value) && value == 3);
        assert(!queue.pop(value));
        assert(queue.empty());
    }
    std::cout << "  Unbounded SPSC Queue: PASSED" << std::endl;

    std::cout << "=== All SPSC Queue Tests Passed ===" << std::endl;
}

} // namespace SPSCQueue
