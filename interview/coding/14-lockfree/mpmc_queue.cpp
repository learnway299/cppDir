/**
 * @file mpmc_queue.cpp
 * @brief 多生产者多消费者队列 - 面试题
 *
 * MPMC (Multi-Producer Multi-Consumer) 队列是最复杂的无锁队列，
 * 需要处理多个生产者和消费者之间的竞争。
 */
#include <atomic>
#include <cstddef>

// 题目1: 实现有界 MPMC 队列
// 使用序列号避免 ABA 问题
template <typename T, size_t Capacity>
class MPMCQueue {
public:
    MPMCQueue() { /* TODO: 初始化序列号 */ }

    bool push(const T& value) { return false; /* TODO */ }
    bool pop(T& value) { return false; /* TODO */ }

private:
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    Cell buffer_[Capacity];
    std::atomic<size_t> enqueuePos_{0};
    std::atomic<size_t> dequeuePos_{0};
};

// 题目2: Michael & Scott 无锁队列
// 经典的无锁队列算法（链表实现）
template <typename T>
class MSQueue {
public:
    MSQueue() { /* TODO: 初始化哨兵节点 */ }
    ~MSQueue() { /* TODO: 清理 */ }

    void push(const T& value) { /* TODO */ }
    bool pop(T& value) { return false; /* TODO */ }

private:
    struct Node {
        T data;
        std::atomic<Node*> next{nullptr};
    };

    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
};

// 题目3: 带背压的 MPMC 队列
// 当队列满时生产者等待，当队列空时消费者等待
template <typename T, size_t Capacity>
class MPMCQueueBlocking {
public:
    void push(const T& value) { /* TODO: 阻塞直到成功 */ }
    T pop() { return T{}; /* TODO: 阻塞直到成功 */ }

    bool try_push(const T& value) { return false; /* TODO */ }
    bool try_pop(T& value) { return false; /* TODO */ }

private:
    // TODO
};

int main() {
    return 0;
}
