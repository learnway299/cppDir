/**
 * @file mpmc_queue.h
 * @brief 多生产者多消费者队列 - 头文件
 *
 * MPMC (Multi-Producer Multi-Consumer) 队列是最复杂的无锁队列，
 * 需要处理多个生产者和消费者之间的竞争。
 */
#ifndef MPMC_QUEUE_H
#define MPMC_QUEUE_H

#include <atomic>
#include <cstddef>

namespace MPMCQueue {

// 题目1: 实现有界 MPMC 队列
// 使用序列号避免 ABA 问题
template <typename T, size_t Capacity>
class MPMCQueue {
public:
    MPMCQueue();
    bool push(const T& value);
    bool pop(T& value);

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
    MSQueue();
    ~MSQueue();
    void push(const T& value);
    bool pop(T& value);

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
    void push(const T& value);
    T pop();
    bool try_push(const T& value);
    bool try_pop(T& value);
};

// ==================== Solution 类声明（实现在 .cpp 文件中）====================
// 注意：这些类在 mpmc_queue_solution.cpp 中完整定义

void runTestsMPMCBounded();
void runTestsMSQueue();
void runTestsMPMCBlocking();

void runTests();

} // namespace MPMCQueue

#endif // MPMC_QUEUE_H
