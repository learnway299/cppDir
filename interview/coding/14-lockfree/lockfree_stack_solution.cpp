/**
 * @file lockfree_stack_solution.cpp
 * @brief 无锁栈 - 参考答案
 */
#include "lockfree_stack.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

namespace LockFreeStack {

// ==================== 题目1: 基本无锁栈 ====================

template <typename T>
struct LockFreeStackNode {
    T data;
    LockFreeStackNode* next;
    LockFreeStackNode(const T& value) : data(value), next(nullptr) {}
};

template <typename T>
void LockFreeStackBasicSolution<T>::push(const T& value) {
    LockFreeStackNode<T>* newNode = new LockFreeStackNode<T>(value);
    std::atomic<LockFreeStackNode<T>*>* head =
        reinterpret_cast<std::atomic<LockFreeStackNode<T>*>*>(&head_);
    newNode->next = head->load(std::memory_order_relaxed);
    while (!head->compare_exchange_weak(newNode->next, newNode,
                                        std::memory_order_release,
                                        std::memory_order_relaxed)) {}
}

template <typename T>
bool LockFreeStackBasicSolution<T>::pop(T& result) {
    std::atomic<LockFreeStackNode<T>*>* head =
        reinterpret_cast<std::atomic<LockFreeStackNode<T>*>*>(&head_);
    LockFreeStackNode<T>* oldHead = head->load(std::memory_order_relaxed);
    while (oldHead &&
           !head->compare_exchange_weak(oldHead, oldHead->next,
                                        std::memory_order_acquire,
                                        std::memory_order_relaxed)) {}
    if (oldHead) {
        result = oldHead->data;
        delete oldHead;
        return true;
    }
    return false;
}

template <typename T>
bool LockFreeStackBasicSolution<T>::empty() const {
    auto* head = reinterpret_cast<const std::atomic<LockFreeStackNode<T>*>*>(&head_);
    return head->load(std::memory_order_relaxed) == nullptr;
}

// ==================== 题目2: 解决 ABA 问题（带版本号）====================
// 使用 64 位打包指针：高 16 位存 tag，低 48 位存指针（x64 用户空间指针只用 48 位）

template <typename T>
class PackedPointer {
    static constexpr uintptr_t PTR_MASK = 0x0000FFFFFFFFFFFF;  // 低 48 位
    static constexpr int TAG_SHIFT = 48;
    uintptr_t packed_{0};

public:
    PackedPointer() = default;
    PackedPointer(LockFreeStackNode<T>* ptr, uint16_t tag) {
        packed_ = (static_cast<uintptr_t>(tag) << TAG_SHIFT) |
                  (reinterpret_cast<uintptr_t>(ptr) & PTR_MASK);
    }

    LockFreeStackNode<T>* ptr() const {
        return reinterpret_cast<LockFreeStackNode<T>*>(packed_ & PTR_MASK);
    }

    uint16_t tag() const {
        return static_cast<uint16_t>(packed_ >> TAG_SHIFT);
    }

    bool operator==(const PackedPointer& other) const {
        return packed_ == other.packed_;
    }
};

template <typename T>
void LockFreeStackABASolution<T>::push(const T& value) {
    LockFreeStackNode<T>* newNode = new LockFreeStackNode<T>(value);
    std::atomic<PackedPointer<T>>* head =
        reinterpret_cast<std::atomic<PackedPointer<T>>*>(&head_);
    PackedPointer<T> oldHead = head->load(std::memory_order_relaxed);
    PackedPointer<T> newHead;

    do {
        newNode->next = oldHead.ptr();
        newHead = PackedPointer<T>(newNode, oldHead.tag() + 1);
    } while (!head->compare_exchange_weak(oldHead, newHead,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
}

template <typename T>
bool LockFreeStackABASolution<T>::pop(T& result) {
    std::atomic<PackedPointer<T>>* head =
        reinterpret_cast<std::atomic<PackedPointer<T>>*>(&head_);
    PackedPointer<T> oldHead = head->load(std::memory_order_relaxed);
    PackedPointer<T> newHead;

    do {
        if (oldHead.ptr() == nullptr) {
            return false;
        }
        newHead = PackedPointer<T>(oldHead.ptr()->next, oldHead.tag() + 1);
    } while (!head->compare_exchange_weak(oldHead, newHead,
                                          std::memory_order_acquire,
                                          std::memory_order_relaxed));

    result = oldHead.ptr()->data;
    delete oldHead.ptr();
    return true;
}

template <typename T>
bool LockFreeStackABASolution<T>::empty() const {
    auto* head = reinterpret_cast<const std::atomic<PackedPointer<T>>*>(&head_);
    return head->load(std::memory_order_relaxed).ptr() == nullptr;
}

// ==================== 题目3: 使用 shared_ptr 的无锁栈 ====================

template <typename T>
struct SharedNode {
    T data;
    std::shared_ptr<SharedNode> next;
};

template <typename T>
void LockFreeStackSimpleSolution<T>::push(const T& value) {
    auto newNode = std::make_shared<SharedNode<T>>();
    newNode->data = value;
    std::atomic<std::shared_ptr<SharedNode<T>>>* head =
        reinterpret_cast<std::atomic<std::shared_ptr<SharedNode<T>>>*>(&head_);
    newNode->next = head->load();
    while (!head->compare_exchange_weak(newNode->next, newNode)) {}
}

template <typename T>
bool LockFreeStackSimpleSolution<T>::pop(T& result) {
    std::atomic<std::shared_ptr<SharedNode<T>>>* head =
        reinterpret_cast<std::atomic<std::shared_ptr<SharedNode<T>>>*>(&head_);
    auto oldHead = head->load();
    while (oldHead && !head->compare_exchange_weak(oldHead, oldHead->next)) {}
    if (oldHead) {
        result = oldHead->data;
        return true;
    }
    return false;
}

template <typename T>
bool LockFreeStackSimpleSolution<T>::empty() const {
    auto* head = reinterpret_cast<const std::atomic<std::shared_ptr<SharedNode<T>>>*>(&head_);
    return head->load() == nullptr;
}

// 显式实例化
template class LockFreeStackBasicSolution<int>;
template class LockFreeStackABASolution<int>;
template class LockFreeStackSimpleSolution<int>;

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Lock-Free Stack Tests ===" << std::endl;

    // 测试基本无锁栈
    {
        LockFreeStackBasicSolution<int> stack;
        std::atomic<int> pushCount{0};
        std::atomic<int> popCount{0};

        std::vector<std::thread> threads;

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&stack, &pushCount] {
                for (int j = 0; j < 1000; ++j) {
                    stack.push(j);
                    ++pushCount;
                }
            });
        }

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&stack, &popCount] {
                int value;
                for (int j = 0; j < 1000; ++j) {
                    while (!stack.pop(value)) {
                        std::this_thread::yield();
                    }
                    ++popCount;
                }
            });
        }

        for (auto& t : threads) t.join();

        assert(pushCount == 2000);
        assert(popCount == 2000);
        assert(stack.empty());
    }
    std::cout << "  Basic Lock-Free Stack: PASSED" << std::endl;

    // 测试 ABA 安全的无锁栈
    {
        LockFreeStackABASolution<int> stack;
        std::atomic<int> pushCount{0};
        std::atomic<int> popCount{0};

        std::vector<std::thread> threads;

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&stack, &pushCount] {
                for (int j = 0; j < 1000; ++j) {
                    stack.push(j);
                    ++pushCount;
                }
            });
        }

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&stack, &popCount] {
                int value;
                for (int j = 0; j < 1000; ++j) {
                    while (!stack.pop(value)) {
                        std::this_thread::yield();
                    }
                    ++popCount;
                }
            });
        }

        for (auto& t : threads) t.join();

        assert(pushCount == 2000);
        assert(popCount == 2000);
        assert(stack.empty());
    }
    std::cout << "  ABA-Safe Lock-Free Stack: PASSED" << std::endl;

    // 测试简单无锁栈
    {
        LockFreeStackSimpleSolution<int> stack;

        stack.push(1);
        stack.push(2);
        stack.push(3);

        int value;
        assert(stack.pop(value) && value == 3);
        assert(stack.pop(value) && value == 2);
        assert(stack.pop(value) && value == 1);
        assert(!stack.pop(value));
        assert(stack.empty());
    }
    std::cout << "  Simple Lock-Free Stack: PASSED" << std::endl;

    std::cout << "=== All Lock-Free Stack Tests Passed ===" << std::endl;
}

} // namespace LockFreeStack
