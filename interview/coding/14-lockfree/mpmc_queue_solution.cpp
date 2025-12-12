/**
 * @file mpmc_queue_solution.cpp
 * @brief 多生产者多消费者队列 - 解答
 */
#include <atomic>
#include <cstddef>
#include <array>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <mutex>

constexpr size_t CACHE_LINE_SIZE = 64;

// ==================== 题目1: Dmitry Vyukov 的有界 MPMC 队列 ====================
// 这是目前已知性能最好的有界 MPMC 队列实现之一
template <typename T, size_t Capacity>
class MPMCQueueBounded {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

public:
    MPMCQueueBounded() {
        for (size_t i = 0; i < Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    bool push(const T& value) {
        Cell* cell;
        size_t pos = enqueuePos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & (Capacity - 1)];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

            if (diff == 0) {
                // 槽位可用，尝试占用
                if (enqueuePos_.compare_exchange_weak(pos, pos + 1,
                                                      std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                // 队列满了
                return false;
            } else {
                // 其他生产者正在写入，更新 pos 重试
                pos = enqueuePos_.load(std::memory_order_relaxed);
            }
        }

        cell->data = value;
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        Cell* cell;
        size_t pos = dequeuePos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & (Capacity - 1)];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

            if (diff == 0) {
                // 数据可用，尝试消费
                if (dequeuePos_.compare_exchange_weak(pos, pos + 1,
                                                      std::memory_order_relaxed)) {
                    break;
                }
            } else if (diff < 0) {
                // 队列空了
                return false;
            } else {
                // 其他消费者正在读取，更新 pos 重试
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

private:
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    alignas(CACHE_LINE_SIZE) std::array<Cell, Capacity> buffer_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> enqueuePos_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> dequeuePos_{0};
};

// ==================== 题目2: Michael & Scott 无锁队列 ====================
template <typename T>
class MSQueue {
public:
    MSQueue() {
        Node* dummy = new Node();
        head_.store(dummy);
        tail_.store(dummy);
    }

    ~MSQueue() {
        T dummy;
        while (pop(dummy)) {}
        delete head_.load();
    }

    void push(const T& value) {
        Node* newNode = new Node(value);

        while (true) {
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = tail->next.load(std::memory_order_acquire);

            if (tail == tail_.load(std::memory_order_acquire)) {
                if (next == nullptr) {
                    // tail 确实是最后一个节点，尝试添加
                    if (tail->next.compare_exchange_weak(next, newNode,
                                                         std::memory_order_release,
                                                         std::memory_order_relaxed)) {
                        // 成功添加，尝试更新 tail（失败也没关系）
                        tail_.compare_exchange_strong(tail, newNode,
                                                      std::memory_order_release,
                                                      std::memory_order_relaxed);
                        return;
                    }
                } else {
                    // tail 落后了，帮助更新
                    tail_.compare_exchange_weak(tail, next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed);
                }
            }
        }
    }

    bool pop(T& value) {
        while (true) {
            Node* head = head_.load(std::memory_order_acquire);
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = head->next.load(std::memory_order_acquire);

            if (head == head_.load(std::memory_order_acquire)) {
                if (head == tail) {
                    if (next == nullptr) {
                        // 队列为空
                        return false;
                    }
                    // tail 落后了，帮助更新
                    tail_.compare_exchange_weak(tail, next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed);
                } else {
                    // 读取数据
                    value = next->data;
                    if (head_.compare_exchange_weak(head, next,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed)) {
                        delete head;  // 注意：实际应用需要安全内存回收
                        return true;
                    }
                }
            }
        }
    }

private:
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};

        Node() = default;
        Node(const T& value) : data(value) {}
    };

    alignas(CACHE_LINE_SIZE) std::atomic<Node*> head_;
    alignas(CACHE_LINE_SIZE) std::atomic<Node*> tail_;
};

// ==================== 题目3: 带阻塞的 MPMC 队列 ====================
template <typename T, size_t Capacity>
class MPMCQueueBlocking {
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

// ==================== 测试代码 ====================
template <typename Queue>
void benchmark(const char* name, int numProducers, int numConsumers, int opsPerThread) {
    Queue queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};
    std::atomic<long long> sum{0};

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;

    // 生产者
    for (int i = 0; i < numProducers; ++i) {
        threads.emplace_back([&queue, &produced, opsPerThread, i] {
            for (int j = 0; j < opsPerThread; ++j) {
                int value = i * opsPerThread + j;
                while (!queue.push(value)) {
                    std::this_thread::yield();
                }
                produced.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 消费者
    int totalOps = numProducers * opsPerThread;
    for (int i = 0; i < numConsumers; ++i) {
        threads.emplace_back([&queue, &consumed, &sum, totalOps, numConsumers, i] {
            int value;
            int myOps = totalOps / numConsumers + (i < totalOps % numConsumers ? 1 : 0);
            for (int j = 0; j < myOps; ++j) {
                while (!queue.pop(value)) {
                    std::this_thread::yield();
                }
                sum.fetch_add(value, std::memory_order_relaxed);
                consumed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << name << " (" << numProducers << "P/" << numConsumers << "C): "
              << duration.count() << "ms, produced=" << produced
              << ", consumed=" << consumed << "\n";
}

int main() {
    const int OPS_PER_THREAD = 100000;

    std::cout << "=== MPMC Queue Benchmark ===\n\n";

    benchmark<MPMCQueueBounded<int, 1024>>("Bounded MPMC", 2, 2, OPS_PER_THREAD);
    benchmark<MPMCQueueBounded<int, 1024>>("Bounded MPMC", 4, 4, OPS_PER_THREAD);
    benchmark<MSQueue<int>>("M&S Queue", 2, 2, OPS_PER_THREAD);
    benchmark<MSQueue<int>>("M&S Queue", 4, 4, OPS_PER_THREAD);

    std::cout << "\n=== Blocking Queue Test ===\n";
    benchmark<MPMCQueueBlocking<int, 1024>>("Blocking MPMC", 2, 2, OPS_PER_THREAD);

    return 0;
}

/**
 * 关键要点：
 *
 * 1. Vyukov 有界队列：
 *    - 使用序列号标记槽位状态
 *    - sequence == pos: 可写入
 *    - sequence == pos + 1: 可读取
 *    - sequence == pos + Capacity: 已消费，可重用
 *    - 不需要 Hazard Pointer
 *
 * 2. Michael & Scott 队列：
 *    - 经典的无锁队列算法
 *    - 使用哨兵节点简化边界条件
 *    - 帮助机制：落后的 tail 由其他线程更新
 *    - 需要安全内存回收（HP、RCU 等）
 *
 * 3. 性能对比：
 *    - 有界队列通常更快（缓存友好）
 *    - 无界队列更灵活但有内存开销
 *    - 阻塞队列适合背压场景
 *
 * 4. 容量要求：
 *    - Vyukov 队列要求 2 的幂（位运算优化）
 *    - M&S 队列无容量限制
 *
 * 5. 内存序：
 *    - CAS 操作使用 relaxed 成功序（已在循环中）
 *    - 读取数据前用 acquire
 *    - 写入数据后用 release
 *
 * 6. 实际应用建议：
 *    - 高吞吐场景：Vyukov 有界队列
 *    - 无界需求：M&S 队列 + Hazard Pointer
 *    - 简单场景：带锁的阻塞队列
 */
