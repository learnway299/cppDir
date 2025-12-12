/**
 * @file hazard_pointer_solution.cpp
 * @brief 风险指针 - 解答
 */
#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <algorithm>

// ==================== 题目1: 基本 Hazard Pointer 系统 ====================
class HazardPointerDomain {
public:
    static constexpr size_t MAX_THREADS = 128;
    static constexpr size_t RETIRE_THRESHOLD = 2 * MAX_THREADS;

    HazardPointerDomain() {
        for (auto& hp : hazardPointers_) {
            hp.ptr.store(nullptr);
            hp.active.store(false);
        }
    }

    // 获取当前线程的 hazard pointer 槽位
    std::atomic<void*>& getHazardPointer() {
        // 查找或分配槽位
        thread_local size_t myIndex = allocateSlot();
        return hazardPointers_[myIndex].ptr;
    }

    // 标记指针为待回收
    void retire(void* ptr, std::function<void(void*)> deleter) {
        RetiredNode node{ptr, std::move(deleter)};

        // 加入线程本地退休列表
        thread_local std::vector<RetiredNode> retiredList;
        retiredList.push_back(std::move(node));

        // 达到阈值时尝试回收
        if (retiredList.size() >= RETIRE_THRESHOLD) {
            scan(retiredList);
        }
    }

    // 扫描并回收
    void scan(std::vector<RetiredNode>& retiredList) {
        // 收集所有活跃的 hazard pointer
        std::unordered_set<void*> hazardSet;
        for (const auto& hp : hazardPointers_) {
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
                // 安全回收
                it->deleter(it->ptr);
                it = retiredList.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    struct HazardPointerSlot {
        std::atomic<void*> ptr{nullptr};
        std::atomic<bool> active{false};
    };

    struct RetiredNode {
        void* ptr;
        std::function<void(void*)> deleter;
    };

    size_t allocateSlot() {
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            bool expected = false;
            if (hazardPointers_[i].active.compare_exchange_strong(
                    expected, true, std::memory_order_acq_rel)) {
                return i;
            }
        }
        throw std::runtime_error("No available hazard pointer slots");
    }

    std::array<HazardPointerSlot, MAX_THREADS> hazardPointers_;
};

// 全局 HP 域
HazardPointerDomain& getHPDomain() {
    static HazardPointerDomain domain;
    return domain;
}

// ==================== 题目2: 使用 HP 的安全无锁栈 ====================
template <typename T>
class SafeLockFreeStack {
public:
    ~SafeLockFreeStack() {
        while (Node* node = head_.load()) {
            head_.store(node->next);
            delete node;
        }
    }

    void push(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = head_.load(std::memory_order_relaxed);
        while (!head_.compare_exchange_weak(newNode->next, newNode,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {}
    }

    bool pop(T& value) {
        auto& hp = getHPDomain().getHazardPointer();

        Node* oldHead;
        do {
            oldHead = head_.load(std::memory_order_relaxed);
            if (!oldHead) {
                return false;
            }

            // 保护 oldHead
            hp.store(oldHead, std::memory_order_release);

            // 验证 head 没有改变
            if (head_.load(std::memory_order_acquire) != oldHead) {
                continue;
            }

            // 尝试 pop
        } while (!head_.compare_exchange_weak(oldHead, oldHead->next,
                                              std::memory_order_acquire,
                                              std::memory_order_relaxed));

        // 清除 HP
        hp.store(nullptr, std::memory_order_release);

        value = oldHead->data;

        // 安全回收
        getHPDomain().retire(oldHead, [](void* p) { delete static_cast<Node*>(p); });

        return true;
    }

private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};
};

// ==================== 题目3: 多 Hazard Pointer ====================
template <size_t K>
class MultiHazardPointer {
public:
    static constexpr size_t MAX_THREADS = 128;
    static constexpr size_t RETIRE_THRESHOLD = 2 * K * MAX_THREADS;

    MultiHazardPointer() {
        for (auto& record : records_) {
            record.active.store(false);
            for (auto& hp : record.hazardPtrs) {
                hp.store(nullptr);
            }
        }
    }

    // RAII 包装类
    class Guard {
    public:
        Guard(MultiHazardPointer& owner) : owner_(owner), index_(owner.allocateSlot()) {}
        ~Guard() {
            clearAll();
            owner_.records_[index_].active.store(false, std::memory_order_release);
        }

        std::atomic<void*>& get(size_t i) {
            return owner_.records_[index_].hazardPtrs[i];
        }

        void clearAll() {
            for (auto& hp : owner_.records_[index_].hazardPtrs) {
                hp.store(nullptr, std::memory_order_release);
            }
        }

    private:
        MultiHazardPointer& owner_;
        size_t index_;
    };

    // 退休指针
    void retire(void* ptr, std::function<void(void*)> deleter) {
        thread_local std::vector<RetiredNode> retiredList;
        retiredList.push_back({ptr, std::move(deleter)});

        if (retiredList.size() >= RETIRE_THRESHOLD) {
            scan(retiredList);
        }
    }

private:
    struct HPRecord {
        std::atomic<bool> active{false};
        std::array<std::atomic<void*>, K> hazardPtrs;
    };

    struct RetiredNode {
        void* ptr;
        std::function<void(void*)> deleter;
    };

    size_t allocateSlot() {
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            bool expected = false;
            if (records_[i].active.compare_exchange_strong(expected, true)) {
                return i;
            }
        }
        throw std::runtime_error("No available HP record");
    }

    void scan(std::vector<RetiredNode>& retiredList) {
        std::unordered_set<void*> hazardSet;
        for (const auto& record : records_) {
            if (record.active.load(std::memory_order_acquire)) {
                for (const auto& hp : record.hazardPtrs) {
                    void* p = hp.load(std::memory_order_acquire);
                    if (p) {
                        hazardSet.insert(p);
                    }
                }
            }
        }

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

    std::array<HPRecord, MAX_THREADS> records_;
};

// ==================== 测试代码 ====================
void testSafeStack() {
    std::cout << "=== Safe Lock-Free Stack Test ===\n";

    SafeLockFreeStack<int> stack;
    std::atomic<int> sum{0};

    std::vector<std::thread> threads;

    // 生产者
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&stack, i] {
            for (int j = 0; j < 1000; ++j) {
                stack.push(i * 1000 + j);
            }
        });
    }

    // 消费者
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&stack, &sum] {
            int value;
            for (int j = 0; j < 1000; ++j) {
                while (!stack.pop(value)) {
                    std::this_thread::yield();
                }
                sum.fetch_add(value, std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Total sum: " << sum << "\n";
}

int main() {
    testSafeStack();

    std::cout << "\n=== Multi-HP Example ===\n";
    MultiHazardPointer<2> mhp;
    {
        auto guard = MultiHazardPointer<2>::Guard(mhp);
        int x = 42;
        guard.get(0).store(&x);
        std::cout << "HP[0] protected: " << guard.get(0).load() << "\n";
        guard.clearAll();
    }
    std::cout << "Guard released\n";

    return 0;
}

/**
 * 关键要点：
 *
 * 1. Hazard Pointer 原理：
 *    - 线程在访问共享对象前，将指针发布到 HP 槽位
 *    - 其他线程在回收对象前，检查所有 HP
 *    - 如果对象在任何 HP 中，延迟回收
 *
 * 2. HP 的组成部分：
 *    - HP 数组：每个线程一个或多个槽位
 *    - 退休列表：待回收的指针列表
 *    - Scan 函数：检查并回收安全的指针
 *
 * 3. 使用模式：
 *    - 访问前：hp.store(ptr)
 *    - 验证：确认 ptr 仍然有效
 *    - 访问后：hp.store(nullptr)
 *    - 回收时：retire(ptr, deleter)
 *
 * 4. 性能考虑：
 *    - HP 数组大小固定（O(线程数)）
 *    - Scan 操作 O(退休列表 × HP 数组)
 *    - 退休阈值影响内存占用和 scan 频率
 *
 * 5. 与其他方案对比：
 *    - HP: 确定性回收，内存占用可控
 *    - RCU: 更低开销，但回收延迟不确定
 *    - Reference Counting: 简单，但有引用计数开销
 *
 * 6. 多 HP 使用场景：
 *    - 遍历链表：需要同时保护当前和下一个节点
 *    - 复杂数据结构：需要保护多个指针
 *
 * 7. C++26 std::hazard_pointer：
 *    - 标准化的 HP 实现
 *    - 更高效的实现
 */
