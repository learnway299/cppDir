/**
 * @file lockfree_stack_solution.cpp
 * @brief 无锁栈 - 解答
 */
#include <atomic>
#include <memory>
#include <iostream>
#include <thread>
#include <vector>

// ==================== 题目1: 基本无锁栈 ====================
// 注意：此实现存在 ABA 问题和内存泄漏问题，仅用于演示基本原理
template <typename T>
class LockFreeStackBasic {
public:
    ~LockFreeStackBasic() {
        while (Node* node = head_.load()) {
            head_.store(node->next);
            delete node;
        }
    }

    void push(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = head_.load(std::memory_order_relaxed);
        // CAS: 如果 head 没变，就将 head 指向新节点
        while (!head_.compare_exchange_weak(newNode->next, newNode,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {
            // 失败时 newNode->next 会被更新为当前 head
        }
    }

    bool pop(T& result) {
        Node* oldHead = head_.load(std::memory_order_relaxed);
        while (oldHead &&
               !head_.compare_exchange_weak(oldHead, oldHead->next,
                                            std::memory_order_acquire,
                                            std::memory_order_relaxed)) {
            // 失败时 oldHead 会被更新
        }
        if (oldHead) {
            result = oldHead->data;
            delete oldHead;  // 危险！可能有其他线程还在访问
            return true;
        }
        return false;
    }

    bool empty() const {
        return head_.load(std::memory_order_relaxed) == nullptr;
    }

private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};
};

// ==================== 题目2: 解决 ABA 问题（带版本号）====================
template <typename T>
class LockFreeStackABA {
public:
    ~LockFreeStackABA() {
        T dummy;
        while (pop(dummy)) {}
    }

    void push(const T& value) {
        Node* newNode = new Node(value);
        TaggedPointer oldHead = head_.load(std::memory_order_relaxed);
        TaggedPointer newHead;

        do {
            newNode->next = oldHead.ptr;
            newHead.ptr = newNode;
            newHead.tag = oldHead.tag + 1;
        } while (!head_.compare_exchange_weak(oldHead, newHead,
                                              std::memory_order_release,
                                              std::memory_order_relaxed));
    }

    bool pop(T& result) {
        TaggedPointer oldHead = head_.load(std::memory_order_relaxed);
        TaggedPointer newHead;

        do {
            if (oldHead.ptr == nullptr) {
                return false;
            }
            newHead.ptr = oldHead.ptr->next;
            newHead.tag = oldHead.tag + 1;
        } while (!head_.compare_exchange_weak(oldHead, newHead,
                                              std::memory_order_acquire,
                                              std::memory_order_relaxed));

        result = oldHead.ptr->data;
        // 延迟删除：这里仍有问题，实际应用需要 hazard pointer 或 epoch-based reclamation
        delete oldHead.ptr;
        return true;
    }

private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    // 带版本号的指针
    struct TaggedPointer {
        Node* ptr{nullptr};
        unsigned long tag{0};

        bool operator==(const TaggedPointer& other) const {
            return ptr == other.ptr && tag == other.tag;
        }
    };

    std::atomic<TaggedPointer> head_{TaggedPointer{}};
};

// ==================== 题目3: 使用引用计数的无锁栈 ====================
// 使用内部和外部引用计数解决内存回收问题
template <typename T>
class LockFreeStackRefCount {
public:
    ~LockFreeStackRefCount() {
        while (pop()) {}
    }

    void push(const T& value) {
        CountedNodePtr newNode;
        newNode.ptr = new Node(value);
        newNode.externalCount = 1;
        newNode.ptr->next = head_.load(std::memory_order_relaxed);

        while (!head_.compare_exchange_weak(newNode.ptr->next, newNode,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {}
    }

    std::shared_ptr<T> pop() {
        CountedNodePtr oldHead = head_.load(std::memory_order_relaxed);

        while (true) {
            increaseHeadCount(oldHead);
            Node* ptr = oldHead.ptr;
            if (!ptr) {
                return std::shared_ptr<T>();
            }

            if (head_.compare_exchange_strong(oldHead, ptr->next,
                                              std::memory_order_relaxed)) {
                std::shared_ptr<T> result;
                result.swap(ptr->data);

                int countIncrease = oldHead.externalCount - 2;
                if (ptr->internalCount.fetch_add(countIncrease,
                                                 std::memory_order_release) == -countIncrease) {
                    delete ptr;
                }
                return result;
            } else if (ptr->internalCount.fetch_sub(1, std::memory_order_relaxed) == 1) {
                ptr->internalCount.load(std::memory_order_acquire);
                delete ptr;
            }
        }
    }

private:
    struct Node;

    struct CountedNodePtr {
        int externalCount{0};
        Node* ptr{nullptr};
    };

    struct Node {
        std::shared_ptr<T> data;
        std::atomic<int> internalCount{0};
        CountedNodePtr next;

        Node(const T& value) : data(std::make_shared<T>(value)) {}
    };

    void increaseHeadCount(CountedNodePtr& oldCounter) {
        CountedNodePtr newCounter;
        do {
            newCounter = oldCounter;
            ++newCounter.externalCount;
        } while (!head_.compare_exchange_strong(oldCounter, newCounter,
                                                std::memory_order_acquire,
                                                std::memory_order_relaxed));
        oldCounter.externalCount = newCounter.externalCount;
    }

    std::atomic<CountedNodePtr> head_;
};

// ==================== 简化版：使用 shared_ptr 的无锁栈 ====================
// C++20 提供 std::atomic<std::shared_ptr<T>>
template <typename T>
class LockFreeStackSimple {
public:
    void push(const T& value) {
        auto newNode = std::make_shared<Node>();
        newNode->data = value;
        newNode->next = head_.load();
        while (!head_.compare_exchange_weak(newNode->next, newNode)) {}
    }

    bool pop(T& result) {
        auto oldHead = head_.load();
        while (oldHead && !head_.compare_exchange_weak(oldHead, oldHead->next)) {}
        if (oldHead) {
            result = oldHead->data;
            return true;
        }
        return false;
    }

    bool empty() const {
        return head_.load() == nullptr;
    }

private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
    };

    std::atomic<std::shared_ptr<Node>> head_{nullptr};
};

// ==================== 测试代码 ====================
template <typename Stack>
void testStack(const char* name) {
    std::cout << "=== Testing " << name << " ===\n";

    Stack stack;
    std::atomic<int> pushCount{0};
    std::atomic<int> popCount{0};

    const int NUM_THREADS = 4;
    const int OPS_PER_THREAD = 10000;

    std::vector<std::thread> threads;

    // 生产者线程
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&stack, &pushCount, i] {
            for (int j = 0; j < OPS_PER_THREAD; ++j) {
                stack.push(i * OPS_PER_THREAD + j);
                pushCount.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // 消费者线程
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&stack, &popCount] {
            int value;
            for (int j = 0; j < OPS_PER_THREAD; ++j) {
                while (!stack.pop(value)) {
                    std::this_thread::yield();
                }
                popCount.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Push count: " << pushCount << "\n";
    std::cout << "Pop count: " << popCount << "\n";
    std::cout << "Stack empty: " << (stack.empty() ? "yes" : "no") << "\n\n";
}

int main() {
    testStack<LockFreeStackBasic<int>>("Basic Lock-Free Stack");
    testStack<LockFreeStackABA<int>>("ABA-Safe Lock-Free Stack");

    return 0;
}

/**
 * 关键要点：
 *
 * 1. CAS 循环模式：
 *    - 加载当前值
 *    - 基于当前值准备新值
 *    - CAS 尝试更新
 *    - 失败则重试
 *
 * 2. ABA 问题：
 *    - 线程1读取 A
 *    - 线程2: A -> B -> A
 *    - 线程1 CAS 成功，但状态已变化
 *    - 解决：版本号、hazard pointer、epoch-based reclamation
 *
 * 3. 内存回收问题：
 *    - 删除节点时其他线程可能还在访问
 *    - 解决方案：
 *      a) 引用计数
 *      b) Hazard Pointer
 *      c) Epoch-based Reclamation (RCU)
 *      d) 使用 GC 的语言
 *
 * 4. 内存序选择：
 *    - push: release 语义，确保节点数据对其他线程可见
 *    - pop: acquire 语义，确保看到最新的节点数据
 *
 * 5. compare_exchange_weak vs strong：
 *    - weak: 可能虚假失败，在循环中使用更高效
 *    - strong: 不会虚假失败，单次使用时选择
 *
 * 6. 实际应用：
 *    - 简单场景：使用 std::atomic<shared_ptr>
 *    - 高性能场景：使用专业库（如 Boost.Lockfree, Folly）
 */
