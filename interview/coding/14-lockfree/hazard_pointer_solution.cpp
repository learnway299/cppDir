/**
 * @file hazard_pointer_solution.cpp
 * @brief 风险指针 - 参考答案
 */
#include "hazard_pointer.h"
#include <vector>
#include <array>
#include <unordered_set>
#include <iostream>
#include <cassert>

namespace HazardPointer {

// ==================== 题目1: 基本 Hazard Pointer 系统 ====================

struct HazardPointerSlot {
    std::atomic<void*> ptr{nullptr};
    std::atomic<bool> active{false};
};

struct RetiredNode {
    void* ptr;
    std::function<void(void*)> deleter;
};

static constexpr size_t MAX_THREADS = 128;
static constexpr size_t RETIRE_THRESHOLD = 2 * MAX_THREADS;

static std::array<HazardPointerSlot, MAX_THREADS> g_hazardPointers;

static size_t allocateSlot() {
    for (size_t i = 0; i < MAX_THREADS; ++i) {
        bool expected = false;
        if (g_hazardPointers[i].active.compare_exchange_strong(
                expected, true, std::memory_order_acq_rel)) {
            return i;
        }
    }
    throw std::runtime_error("No available hazard pointer slots");
}

std::atomic<void*>& HazardPointerDomainSolution::getHazardPointer() {
    thread_local size_t myIndex = allocateSlot();
    return g_hazardPointers[myIndex].ptr;
}

void HazardPointerDomainSolution::retire(void* ptr, std::function<void(void*)> deleter) {
    RetiredNode node{ptr, std::move(deleter)};

    thread_local std::vector<RetiredNode> retiredList;
    retiredList.push_back(std::move(node));

    if (retiredList.size() >= RETIRE_THRESHOLD) {
        scan();
    }
}

void HazardPointerDomainSolution::scan() {
    thread_local std::vector<RetiredNode> retiredList;

    // 收集所有活跃的 hazard pointer
    std::unordered_set<void*> hazardSet;
    for (const auto& hp : g_hazardPointers) {
        if (hp.active.load(std::memory_order_acquire)) {
            void* p = hp.ptr.load(std::memory_order_acquire);
            if (p) {
                hazardSet.insert(p);
            }
        }
    }

    // 回收不在 hazard set 中的指针
    auto it = retiredList.begin();
    while (it != retiredList.end()) {
        if (hazardSet.find(it->ptr) == hazardSet.end()) {
            it->deleter(it->ptr);
            it = retiredList.erase(it);
        } else {
            ++it;
        }
    }
}

// ==================== 题目2: 使用 HP 的安全无锁栈 ====================

template <typename T>
struct SafeLockFreeStackNode {
    T data;
    SafeLockFreeStackNode* next;
    SafeLockFreeStackNode(const T& value) : data(value), next(nullptr) {}
};

static HazardPointerDomainSolution g_hpDomain;

template <typename T>
void SafeLockFreeStackSolution<T>::push(const T& value) {
    auto* newNode = new SafeLockFreeStackNode<T>(value);
    auto* head = reinterpret_cast<std::atomic<SafeLockFreeStackNode<T>*>*>(&head_);
    newNode->next = head->load(std::memory_order_relaxed);
    while (!head->compare_exchange_weak(newNode->next, newNode,
                                        std::memory_order_release,
                                        std::memory_order_relaxed)) {}
}

template <typename T>
bool SafeLockFreeStackSolution<T>::pop(T& value) {
    auto& hp = g_hpDomain.getHazardPointer();
    auto* head = reinterpret_cast<std::atomic<SafeLockFreeStackNode<T>*>*>(&head_);

    SafeLockFreeStackNode<T>* oldHead;
    do {
        oldHead = head->load(std::memory_order_relaxed);
        if (!oldHead) {
            return false;
        }

        hp.store(oldHead, std::memory_order_release);

        if (head->load(std::memory_order_acquire) != oldHead) {
            continue;
        }
    } while (!head->compare_exchange_weak(oldHead, oldHead->next,
                                          std::memory_order_acquire,
                                          std::memory_order_relaxed));

    hp.store(nullptr, std::memory_order_release);
    value = oldHead->data;

    g_hpDomain.retire(oldHead, [](void* p) { delete static_cast<SafeLockFreeStackNode<T>*>(p); });

    return true;
}

template <typename T>
bool SafeLockFreeStackSolution<T>::empty() const {
    auto* head = reinterpret_cast<const std::atomic<SafeLockFreeStackNode<T>*>*>(&head_);
    return head->load(std::memory_order_relaxed) == nullptr;
}

// 显式实例化
template class SafeLockFreeStackSolution<int>;

// ==================== 题目3: 多 Hazard Pointer ====================

template <size_t K>
struct HPRecord {
    std::atomic<bool> active{false};
    std::array<std::atomic<void*>, K> hazardPtrs;
};

template <size_t K>
class MultiHazardPointerSolution<K>::Guard {
public:
    Guard(MultiHazardPointerSolution& owner) : owner_(owner), index_(allocateSlot()) {}
    ~Guard() {
        clearAll();
        // 释放槽位逻辑
    }

    std::atomic<void*>& get(size_t i) {
        static std::array<HPRecord<K>, MAX_THREADS> records;
        return records[index_].hazardPtrs[i];
    }

    void clearAll() {
        static std::array<HPRecord<K>, MAX_THREADS> records;
        for (auto& hp : records[index_].hazardPtrs) {
            hp.store(nullptr, std::memory_order_release);
        }
    }

private:
    MultiHazardPointerSolution& owner_;
    size_t index_;
};

template <size_t K>
void MultiHazardPointerSolution<K>::retire(void* ptr, std::function<void(void*)> deleter) {
    // 简化实现
}

// 显式实例化
template class MultiHazardPointerSolution<2>;

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Hazard Pointer Tests ===" << std::endl;

    // 测试安全无锁栈
    {
        SafeLockFreeStackSolution<int> stack;
        std::atomic<int> pushCount{0};
        std::atomic<int> popCount{0};

        std::vector<std::thread> threads;

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&stack, &pushCount, i] {
                for (int j = 0; j < 500; ++j) {
                    stack.push(i * 500 + j);
                    ++pushCount;
                }
            });
        }

        for (int i = 0; i < 2; ++i) {
            threads.emplace_back([&stack, &popCount] {
                int value;
                for (int j = 0; j < 500; ++j) {
                    while (!stack.pop(value)) {
                        std::this_thread::yield();
                    }
                    ++popCount;
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        assert(pushCount == 1000);
        assert(popCount == 1000);
        assert(stack.empty());
    }
    std::cout << "  Safe Lock-Free Stack with HP: PASSED" << std::endl;

    std::cout << "=== All Hazard Pointer Tests Passed ===" << std::endl;
}

} // namespace HazardPointer
