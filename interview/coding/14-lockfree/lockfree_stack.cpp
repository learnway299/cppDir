/**
 * @file lockfree_stack.cpp
 * @brief 无锁栈 - 面试题
 *
 * 无锁栈是无锁数据结构的经典入门例子，使用 CAS 操作实现。
 */
#include <atomic>
#include <memory>

// 题目1: 实现基本的无锁栈
// 使用 compare_exchange 实现 push 和 pop
template <typename T>
class LockFreeStack {
public:
    void push(const T& value) { /* TODO */ }

    bool pop(T& result) { return false; /* TODO */ }

    bool empty() const { return true; /* TODO */ }

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
    void push(const T& value) { /* TODO */ }
    bool pop(T& result) { return false; /* TODO */ }

private:
    struct Node {
        T data;
        Node* next;
    };

    // TODO: 定义带版本号的指针结构
};

// 题目3: 使用 shared_ptr 的无锁栈
// 利用 atomic<shared_ptr> 简化内存管理
template <typename T>
class LockFreeStackShared {
public:
    void push(const T& value) { /* TODO */ }

    std::shared_ptr<T> pop() { return nullptr; /* TODO */ }

private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
    };

    // TODO
};

int main() {
    return 0;
}
