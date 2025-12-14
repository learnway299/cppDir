/**
 * @file lockfree_stack.h
 * @brief 无锁栈 - 头文件
 *
 * 无锁栈是无锁数据结构的经典入门例子，使用 CAS 操作实现。
 */
#ifndef LOCKFREE_STACK_H
#define LOCKFREE_STACK_H

#include <atomic>
#include <memory>

namespace LockFreeStack {

// 题目1: 实现基本的无锁栈
// 使用 compare_exchange 实现 push 和 pop
template <typename T>
class LockFreeStack {
public:
    void push(const T& value);
    bool pop(T& result);
    bool empty() const;

private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};
};

// 题目2: 解决 ABA 问题
// 使用带版本号的指针
template <typename T>
class LockFreeStackABA {
public:
    void push(const T& value);
    bool pop(T& result);

private:
    struct Node {
        T data;
        Node* next;
    };
};

// 题目3: 使用 shared_ptr 的无锁栈
// 利用 atomic<shared_ptr> 简化内存管理
template <typename T>
class LockFreeStackShared {
public:
    void push(const T& value);
    std::shared_ptr<T> pop();

private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
    };
};

// ==================== Solution 类 ====================

template <typename T>
class LockFreeStackBasicSolution {
public:
    void push(const T& value);
    bool pop(T& result);
    bool empty() const;

private:
    std::atomic<void*> head_{nullptr};
};

template <typename T>
class LockFreeStackABASolution {
public:
    void push(const T& value);
    bool pop(T& result);
    bool empty() const;

private:
    // 使用足够大的存储空间来存放 TaggedPointer
    alignas(16) char head_[16]{};
};

template <typename T>
class LockFreeStackSimpleSolution {
public:
    void push(const T& value);
    bool pop(T& result);
    bool empty() const;

private:
    std::shared_ptr<void> head_{nullptr};
};

void runTests();

} // namespace LockFreeStack

#endif // LOCKFREE_STACK_H
