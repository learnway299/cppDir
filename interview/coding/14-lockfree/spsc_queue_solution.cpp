/**
 * @file spsc_queue_solution.cpp
 * @brief 单生产者单消费者队列 - 解答
 */
#include <atomic>
#include <cstddef>
#include <array>
#include <iostream>
#include <thread>
#include <chrono>
#include <new>  // for std::hardware_destructive_interference_size

// 缓存行大小（通常 64 字节）
#ifdef __cpp_lib_hardware_interference_size
    constexpr size_t CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
#else
    constexpr size_t CACHE_LINE_SIZE = 64;
#endif

// ==================== 题目1: 基本 SPSC 队列 ====================
template <typename T, size_t Capacity>
class SPSCQueueBasic {
public:
    SPSCQueueBasic() = default;

    bool push(const T& value) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t nextTail = (currentTail + 1) % Capacity;

        // 检查是否满了
        if (nextTail == head_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[currentTail] = value;
        tail_.store(nextTail, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        size_t currentHead = head_.load(std::memory_order_relaxed);

        // 检查是否空了
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

// ==================== 题目2: 缓存行优化版本 ====================
template <typename T, size_t Capacity>
class SPSCQueueOptimized {
public:
    SPSCQueueOptimized() = default;

    bool push(const T& value) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t nextTail = (currentTail + 1) % Capacity;

        // 使用本地缓存的 head 减少跨核通信
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

        // 使用本地缓存的 tail 减少跨核通信
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

private:
    // 数据缓冲区
    std::array<T, Capacity> buffer_;

    // 生产者端变量（同一缓存行）
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0};
    size_t cachedHead_{0};  // 本地缓存的 head

    // 消费者端变量（同一缓存行）
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0};
    size_t cachedTail_{0};  // 本地缓存的 tail

    // 填充以避免尾部伪共享
    char padding_[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>) - sizeof(size_t)];
};

// ==================== 题目3: 支持批量操作 ====================
template <typename T, size_t Capacity>
class SPSCQueueBatch {
public:
    // 批量推入，返回实际推入数量
    size_t push_batch(const T* values, size_t count) {
        size_t currentTail = tail_.load(std::memory_order_relaxed);
        size_t currentHead = head_.load(std::memory_order_acquire);

        // 计算可用空间
        size_t available = (currentHead - currentTail - 1 + Capacity) % Capacity;
        size_t toPush = std::min(count, available);

        for (size_t i = 0; i < toPush; ++i) {
            buffer_[(currentTail + i) % Capacity] = values[i];
        }

        tail_.store((currentTail + toPush) % Capacity, std::memory_order_release);
        return toPush;
    }

    // 批量弹出，返回实际弹出数量
    size_t pop_batch(T* values, size_t maxCount) {
        size_t currentHead = head_.load(std::memory_order_relaxed);
        size_t currentTail = tail_.load(std::memory_order_acquire);

        // 计算可用元素
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

private:
    std::array<T, Capacity> buffer_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0};
};

// ==================== 无界 SPSC 队列（链表实现）====================
template <typename T>
class SPSCQueueUnbounded {
public:
    SPSCQueueUnbounded() {
        Node* dummy = new Node();
        head_.store(dummy);
        tail_ = dummy;
    }

    ~SPSCQueueUnbounded() {
        T dummy;
        while (pop(dummy)) {}
        delete head_.load();
    }

    void push(const T& value) {
        Node* newNode = new Node(value);
        tail_->next.store(newNode, std::memory_order_release);
        tail_ = newNode;
    }

    bool pop(T& value) {
        Node* head = head_.load(std::memory_order_relaxed);
        Node* next = head->next.load(std::memory_order_acquire);

        if (next == nullptr) {
            return false;
        }

        value = next->data;
        head_.store(next, std::memory_order_release);
        delete head;
        return true;
    }

private:
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};

        Node() = default;
        Node(const T& value) : data(value) {}
    };

    alignas(CACHE_LINE_SIZE) std::atomic<Node*> head_;
    alignas(CACHE_LINE_SIZE) Node* tail_;
};

// ==================== 测试代码 ====================
template <typename Queue>
void benchmark(const char* name, int numOps) {
    Queue queue;
    std::atomic<bool> done{false};
    std::atomic<long long> sum{0};

    auto start = std::chrono::high_resolution_clock::now();

    std::thread producer([&] {
        for (int i = 0; i < numOps; ++i) {
            while (!queue.push(i)) {
                std::this_thread::yield();
            }
        }
        done.store(true, std::memory_order_release);
    });

    std::thread consumer([&] {
        int value;
        int count = 0;
        while (count < numOps) {
            if (queue.pop(value)) {
                sum.fetch_add(value, std::memory_order_relaxed);
                ++count;
            }
        }
    });

    producer.join();
    consumer.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    long long expected = (long long)(numOps - 1) * numOps / 2;
    std::cout << name << ": " << duration.count() << "ms, "
              << "sum=" << sum << " (expected=" << expected << ")\n";
}

int main() {
    const int NUM_OPS = 1000000;

    std::cout << "=== SPSC Queue Benchmark ===\n\n";

    benchmark<SPSCQueueBasic<int, 1024>>("Basic SPSC", NUM_OPS);
    benchmark<SPSCQueueOptimized<int, 1024>>("Optimized SPSC", NUM_OPS);
    benchmark<SPSCQueueBatch<int, 1024>>("Batch SPSC", NUM_OPS);
    benchmark<SPSCQueueUnbounded<int>>("Unbounded SPSC", NUM_OPS);

    return 0;
}

/**
 * 关键要点：
 *
 * 1. SPSC 队列的优势：
 *    - 不需要 CAS，只需要原子 load/store
 *    - 生产者只写 tail，消费者只写 head
 *    - 内存序要求较低
 *
 * 2. 环形缓冲区：
 *    - 实际容量为 Capacity - 1（区分满和空）
 *    - 使用取模运算处理环绕
 *    - 可以用位运算优化（Capacity 为 2 的幂）
 *
 * 3. 伪共享优化：
 *    - head 和 tail 分别对齐到不同缓存行
 *    - 避免两个核心频繁使缓存失效
 *    - 使用 alignas(64) 或 padding
 *
 * 4. 本地缓存优化：
 *    - 生产者缓存 head 的旧值
 *    - 消费者缓存 tail 的旧值
 *    - 减少跨核的原子操作
 *
 * 5. 内存序：
 *    - 写入数据后用 release 更新索引
 *    - 读取索引后用 acquire 读取数据
 *    - 本地变量可用 relaxed
 *
 * 6. 批量操作：
 *    - 减少原子操作次数
 *    - 提高缓存利用率
 *    - 特别适合高吞吐场景
 */
