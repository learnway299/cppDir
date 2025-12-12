/**
 * @file hazard_pointer.cpp
 * @brief 风险指针 - 面试题
 *
 * Hazard Pointer 是一种无锁内存回收技术，用于解决无锁数据结构中的
 * 安全内存回收问题（即 ABA 问题的根源）。
 */
#include <atomic>
#include <functional>
#include <thread>

// 题目1: 实现基本的 Hazard Pointer 系统
// 每个线程可以保护一个指针不被回收
class HazardPointerDomain {
public:
    // 获取当前线程的 hazard pointer 槽位
    std::atomic<void*>& getHazardPointer() {
        // TODO: 返回当前线程可用的 HP 槽位
        static std::atomic<void*> dummy{nullptr};
        return dummy;
    }

    // 标记指针为待回收
    void retire(void* ptr, std::function<void(void*)> deleter) {
        // TODO: 将 ptr 加入退休列表，合适时机回收
    }

    // 尝试回收退休列表中的指针
    void scan() {
        // TODO: 检查退休列表，回收不在任何 HP 中的指针
    }

private:
    // TODO: 定义数据结构
};

// 题目2: 使用 Hazard Pointer 实现安全的无锁栈
template <typename T>
class SafeLockFreeStack {
public:
    void push(const T& value) { /* TODO */ }
    bool pop(T& value) { return false; /* TODO */ }

private:
    struct Node {
        T data;
        Node* next;
    };

    std::atomic<Node*> head_{nullptr};
    // TODO: 集成 Hazard Pointer
};

// 题目3: 支持多个 Hazard Pointer（用于复杂数据结构）
template <size_t K>
class MultiHazardPointer {
public:
    // 获取第 i 个 HP
    std::atomic<void*>& get(size_t i) {
        // TODO
        static std::atomic<void*> dummy{nullptr};
        return dummy;
    }

    // 清除所有 HP
    void clearAll() { /* TODO */ }

private:
    // TODO
};

int main() {
    return 0;
}
