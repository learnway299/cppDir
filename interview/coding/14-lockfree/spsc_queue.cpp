/**
 * @file spsc_queue.cpp
 * @brief 单生产者单消费者队列 - 面试题
 *
 * SPSC (Single Producer Single Consumer) 队列是最简单的无锁队列，
 * 因为只有一个生产者和一个消费者，所以不需要复杂的同步机制。
 */
#include <atomic>
#include <cstddef>

// 题目1: 实现基于环形缓冲区的 SPSC 队列
// 使用数组实现，固定容量
template <typename T, size_t Capacity>
class SPSCQueue {
public:
    SPSCQueue() = default;

    // 生产者调用，返回是否成功
    bool push(const T& value) { return false; /* TODO */ }

    // 消费者调用，返回是否成功
    bool pop(T& value) { return false; /* TODO */ }

    bool empty() const { return true; /* TODO */ }
    bool full() const { return false; /* TODO */ }
    size_t size() const { return 0; /* TODO */ }

private:
    T buffer_[Capacity];
    std::atomic<size_t> head_{0};  // 消费者读取位置
    std::atomic<size_t> tail_{0};  // 生产者写入位置
};

// 题目2: 优化缓存行对齐
// 避免 head 和 tail 的伪共享 (false sharing)
template <typename T, size_t Capacity>
class SPSCQueueOptimized {
public:
    bool push(const T& value) { return false; /* TODO */ }
    bool pop(T& value) { return false; /* TODO */ }

private:
    // TODO: 使用 alignas 或 padding 避免伪共享
};

// 题目3: 支持批量操作
template <typename T, size_t Capacity>
class SPSCQueueBatch {
public:
    // 批量推入
    size_t push_batch(const T* values, size_t count) { return 0; /* TODO */ }

    // 批量弹出
    size_t pop_batch(T* values, size_t maxCount) { return 0; /* TODO */ }

private:
    // TODO
};

int main() {
    return 0;
}
