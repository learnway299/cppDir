/**
 * @file hazard_pointer.h
 * @brief 风险指针 - 头文件
 *
 * Hazard Pointer 是一种无锁内存回收技术，用于解决无锁数据结构中的
 * 安全内存回收问题（即 ABA 问题的根源）。
 */
#ifndef HAZARD_POINTER_H
#define HAZARD_POINTER_H

#include <atomic>
#include <functional>
#include <thread>

namespace HazardPointer {

// 题目1: 实现基本的 Hazard Pointer 系统
// 每个线程可以保护一个指针不被回收
class HazardPointerDomain {
public:
    // 获取当前线程的 hazard pointer 槽位
    std::atomic<void*>& getHazardPointer();

    // 标记指针为待回收
    void retire(void* ptr, std::function<void(void*)> deleter);

    // 尝试回收退休列表中的指针
    void scan();
};

// 题目2: 使用 Hazard Pointer 实现安全的无锁栈
template <typename T>
class SafeLockFreeStack {
public:
    void push(const T& value);
    bool pop(T& value);

private:
    struct Node {
        T data;
        Node* next;
    };

    std::atomic<Node*> head_{nullptr};
};

// 题目3: 支持多个 Hazard Pointer（用于复杂数据结构）
template <size_t K>
class MultiHazardPointer {
public:
    // 获取第 i 个 HP
    std::atomic<void*>& get(size_t i);

    // 清除所有 HP
    void clearAll();
};

// ==================== Solution 类 ====================

// 题目1: 实现基本的 Hazard Pointer 系统
class HazardPointerDomainSolution {
public:
    std::atomic<void*>& getHazardPointer();
    void retire(void* ptr, std::function<void(void*)> deleter);
    void scan();
};

// 题目2: 使用 Hazard Pointer 实现安全的无锁栈
template <typename T>
class SafeLockFreeStackSolution {
public:
    void push(const T& value);
    bool pop(T& value);
    bool empty() const;

private:
    std::atomic<void*> head_{nullptr};
};

// 题目3: 支持多个 Hazard Pointer
template <size_t K>
class MultiHazardPointerSolution {
public:
    class Guard;
    void retire(void* ptr, std::function<void(void*)> deleter);
};

void runTests();

} // namespace HazardPointer

#endif // HAZARD_POINTER_H
