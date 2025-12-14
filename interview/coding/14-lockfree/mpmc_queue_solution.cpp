/**
 * @file mpmc_queue_solution.cpp
 * @brief 多生产者多消费者队列 - 参考答案
 */
#include "mpmc_queue.h"
#include <array>
#include <iostream>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <cassert>

namespace MPMCQueue {

static constexpr size_t CACHE_LINE_SIZE = 64;

// ==================== 题目1: Dmitry Vyukov 的有界 MPMC 队列 ====================

template <typename T, size_t Capacity>
struct alignas(CACHE_LINE_SIZE) Cell {
    std::atomic<size_t> sequence;
    T data;
};

template <typename T, size_t Capacity>
class MPMCQueueBoundedSolution {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

public:
    MPMCQueueBoundedSolution() {
        for (size_t i = 0; i < Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    bool push(const T& value) {
        Cell<T, Capacity>* cell;
        size_t pos = enqueuePos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & (Capacity - 1)];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (diff == 0) {
                if (enqueuePos_.compare_exchange_weak(pos, pos + 1,
                                                      std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                pos = enqueuePos_.load(std::memory_order_relaxed);
            }
        }

        cell->data = value;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        Cell<T, Capacity>* cell;
        size_t pos = dequeuePos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & (Capacity - 1)];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

            if (diff == 0) {
                if (dequeuePos_.compare_exchange_weak(pos, pos + 1,
                                                      std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                pos = dequeuePos_.load(std::memory_order_relaxed);
            }
        }

        value = cell->data;
        cell->sequence.store(pos + Capacity, std::memory_order_release);
        return true;
    }

    size_t size() const {
        size_t head = dequeuePos_.load(std::memory_order_relaxed);
        size_t tail = enqueuePos_.load(std::memory_order_relaxed);
        return tail - head;
    }

    bool empty() const {
        return size() == 0;
    }

private:
    alignas(CACHE_LINE_SIZE) std::array<Cell<T, Capacity>, Capacity> buffer_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> enqueuePos_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> dequeuePos_{0};
};

// 显式实例化
template class MPMCQueueBoundedSolution<int, 1024>;

// ==================== 题目2: Michael & Scott 无锁队列 ====================

template <typename T>
struct MSNode {
    T data;
    std::atomic<MSNode*> next{nullptr};

    MSNode() = default;
    MSNode(const T& value) : data(value) {}
};

template <typename T>
class MSQueueSolution {
public:
    MSQueueSolution() {
        MSNode<T>* dummy = new MSNode<T>();
        head_.store(dummy);
        tail_.store(dummy);
    }

    ~MSQueueSolution() {
        T dummy;
        while (pop(dummy)) {}
        delete head_.load();
    }

    void push(const T& value) {
        MSNode<T>* newNode = new MSNode<T>(value);

        while (true) {
            MSNode<T>* tail = tail_.load(std::memory_order_acquire);
            MSNode<T>* next = tail->next.load(std::memory_order_acquire);

            if (tail == tail_.load(std::memory_order_acquire)) {
                if (next == nullptr) {
                    if (tail->next.compare_exchange_weak(next, newNode,
                                                         std::memory_order_release,
                                                         std::memory_order_relaxed)) {
                        tail_.compare_exchange_strong(tail, newNode,
                                                      std::memory_order_release,
                                                      std::memory_order_relaxed);
                        return;
                    }
                } else {
                    tail_.compare_exchange_weak(tail, next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed);
                }
            }
        }
    }

    bool pop(T& value) {
        while (true) {
            MSNode<T>* head = head_.load(std::memory_order_acquire);
            MSNode<T>* tail = tail_.load(std::memory_order_acquire);
            MSNode<T>* next = head->next.load(std::memory_order_acquire);

            if (head == head_.load(std::memory_order_acquire)) {
                if (head == tail) {
                    if (next == nullptr) {
                        return false;
                    }
                    tail_.compare_exchange_weak(tail, next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed);
                } else {
                    value = next->data;
                    if (head_.compare_exchange_weak(head, next,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed)) {
                        delete head;
                        return true;
                    }
                }
            }
        }
    }

    bool empty() const {
        MSNode<T>* head = head_.load(std::memory_order_relaxed);
        return head->next.load(std::memory_order_relaxed) == nullptr;
    }

private:
    alignas(CACHE_LINE_SIZE) std::atomic<MSNode<T>*> head_;
    alignas(CACHE_LINE_SIZE) std::atomic<MSNode<T>*> tail_;
};

// 显式实例化
template class MSQueueSolution<int>;

// ==================== 题目3: 带阻塞的 MPMC 队列 ====================

template <typename T, size_t Capacity>
class MPMCQueueBlockingSolution {
public:
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return size_ < Capacity; });

        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % Capacity;
        ++size_;

        lock.unlock();
        notEmpty_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return size_ > 0; });

        T value = buffer_[head_];
        head_ = (head_ + 1) % Capacity;
        --size_;

        lock.unlock();
        notFull_.notify_one();
        return value;
    }

    bool try_push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ >= Capacity) {
            return false;
        }

        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % Capacity;
        ++size_;

        notEmpty_.notify_one();
        return true;
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size_ == 0) {
            return false;
        }

        value = buffer_[head_];
        head_ = (head_ + 1) % Capacity;
        --size_;

        notFull_.notify_one();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

private:
    std::array<T, Capacity> buffer_;
    size_t head_{0};
    size_t tail_{0};
    size_t size_{0};
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

// 显式实例化
template class MPMCQueueBlockingSolution<int, 1024>;

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== MPMC Queue Tests ===" << std::endl;

    // 测试有界 MPMC 队列
    {
        MPMCQueueBoundedSolution<int, 1024> queue;
        std::atomic<int> produced{0};
        std::atomic<int> consumed{0};

        std::vector<std::thread> threads;

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&queue, &produced, i] {
                for (int j = 0; j < 5000; ++j) {
                    int value = i * 5000 + j;
                    while (!queue.push(value)) {
                        std::this_thread::yield();
                    }
                    ++produced;
                }
            });
        }

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&queue, &consumed] {
                int value;
                for (int j = 0; j < 5000; ++j) {
                    while (!queue.pop(value)) {
                        std::this_thread::yield();
                    }
                    ++consumed;
                }
            });
        }

        for (auto& t : threads) t.join();

        assert(produced == 10000);
        assert(consumed == 10000);
    }
    std::cout << "  Bounded MPMC Queue: PASSED" << std::endl;

    // 测试 M&S 队列
    {
        MSQueueSolution<int> queue;
        std::atomic<int> produced{0};
        std::atomic<int> consumed{0};

        std::vector<std::thread> threads;

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&queue, &produced, i] {
                for (int j = 0; j < 5000; ++j) {
                    queue.push(i * 5000 + j);
                    ++produced;
                }
            });
        }

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&queue, &consumed] {
                int value;
                for (int j = 0; j < 5000; ++j) {
                    while (!queue.pop(value)) {
                        std::this_thread::yield();
                    }
                    ++consumed;
                }
            });
        }

        for (auto& t : threads) t.join();

        assert(produced == 10000);
        assert(consumed == 10000);
        assert(queue.empty());
    }
    std::cout << "  M&S Queue: PASSED" << std::endl;

    // 测试阻塞队列
    {
        MPMCQueueBlockingSolution<int, 1024> queue;

        queue.push(1);
        queue.push(2);
        queue.push(3);

        assert(queue.pop() == 1);
        assert(queue.pop() == 2);
        assert(queue.pop() == 3);

        int value;
        assert(!queue.try_pop(value));
    }
    std::cout << "  Blocking MPMC Queue: PASSED" << std::endl;

    std::cout << "=== All MPMC Queue Tests Passed ===" << std::endl;
}

} // namespace MPMCQueue
