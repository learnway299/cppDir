/**
 * @file weak_ptr.h
 * @brief weak_ptr 实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. WeakPtr 基本实现
 * 2. 解决循环引用问题
 * 3. enable_shared_from_this 实现
 */

#ifndef WEAK_PTR_H
#define WEAK_PTR_H

#include <atomic>
#include <utility>
#include <cstddef>

namespace WeakPtrImpl {

// ==================== 控制块 ====================

/**
 * 控制块结构（支持 weak_ptr）
 *
 * 包含：
 * 1. 指向对象的指针
 * 2. 强引用计数（shared_ptr 数量）
 * 3. 弱引用计数（weak_ptr 数量）
 *
 * 生命周期：
 * - 强引用为 0 时删除对象
 * - 强引用和弱引用都为 0 时删除控制块
 */
template <typename T>
struct ControlBlock {
    T* ptr;
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;

    explicit ControlBlock(T* p);
    void add_strong();
    void add_weak();
    bool release_strong();  // 返回是否需要删除控制块
    bool release_weak();
    bool try_add_strong();  // 尝试增加强引用（用于 lock）
};

// ==================== 前向声明 ====================

template <typename T> class SharedPtr;
template <typename T> class WeakPtr;

// ==================== SharedPtr（支持 WeakPtr） ====================

template <typename T>
class SharedPtr {
public:
    friend class WeakPtr<T>;

    SharedPtr() noexcept;
    explicit SharedPtr(T* ptr);
    explicit SharedPtr(ControlBlock<T>* cb);  // 从控制块构造（用于 lock）
    ~SharedPtr();

    SharedPtr(const SharedPtr& other);
    SharedPtr& operator=(const SharedPtr& other);

    SharedPtr(SharedPtr&& other) noexcept;
    SharedPtr& operator=(SharedPtr&& other) noexcept;

    T* get() const noexcept;
    long use_count() const noexcept;

    T& operator*() const;
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;

private:
    ControlBlock<T>* cb_;
};

// ==================== 题目1: WeakPtr ====================

/**
 * 题目1: 实现 WeakPtr
 *
 * 要求：
 * 1. 不增加强引用计数
 * 2. 只增加弱引用计数
 * 3. 支持 lock() 返回 SharedPtr
 * 4. 支持 expired() 检查是否过期
 *
 * 关键点：
 * - lock() 时需要原子地检查并增加强引用
 * - 使用 CAS 操作保证线程安全
 *
 * 示例：
 *   SharedPtr<int> sp(new int(42));
 *   WeakPtr<int> wp(sp);
 *   if (auto locked = wp.lock()) {
 *       std::cout << *locked << std::endl;
 *   }
 */
template <typename T>
class WeakPtr {
public:
    WeakPtr() noexcept;
    WeakPtr(const SharedPtr<T>& shared);
    ~WeakPtr();

    WeakPtr(const WeakPtr& other);
    WeakPtr& operator=(const WeakPtr& other);
    WeakPtr& operator=(const SharedPtr<T>& shared);

    WeakPtr(WeakPtr&& other) noexcept;
    WeakPtr& operator=(WeakPtr&& other) noexcept;

    bool expired() const noexcept;
    SharedPtr<T> lock() const noexcept;
    long use_count() const noexcept;
    void reset() noexcept;
    void swap(WeakPtr& other) noexcept;

private:
    ControlBlock<T>* cb_;
};

// ==================== 题目2: 解决循环引用 ====================

/**
 * 题目2: 解决循环引用问题
 *
 * 场景：双向链表或图结构中的相互引用
 *
 * 问题演示：
 *   struct Node {
 *       SharedPtr<Node> next;
 *       SharedPtr<Node> prev;  // 导致循环引用，内存泄漏
 *   };
 *
 * 解决方案：
 *   struct Node {
 *       SharedPtr<Node> next;  // 强引用向前
 *       WeakPtr<Node> prev;    // 弱引用向后，打破循环
 *   };
 */
struct NodeWithCycle {
    int value;
    SharedPtr<NodeWithCycle> next;  // 强引用
    WeakPtr<NodeWithCycle> prev;    // 弱引用打破循环
    static int instanceCount;

    NodeWithCycle(int v);
    ~NodeWithCycle();
};

// ==================== 题目3: enable_shared_from_this ====================

/**
 * 题目3: 实现 enable_shared_from_this
 *
 * 作用：允许从成员函数中安全地获取 shared_ptr<this>
 *
 * 问题：
 *   class Foo {
 *       void bar() {
 *           // 错误！会创建独立的引用计数
 *           SharedPtr<Foo> self(this);
 *       }
 *   };
 *
 * 解决方案：
 *   class Foo : public EnableSharedFromThis<Foo> {
 *       void bar() {
 *           SharedPtr<Foo> self = shared_from_this();  // 正确
 *       }
 *   };
 *
 * 实现原理：
 * - 内部存储一个 WeakPtr
 * - 在 SharedPtr 构造时设置 weak_this_
 * - shared_from_this() 调用 weak_this_.lock()
 */
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> shared_from_this();
    WeakPtr<T> weak_from_this() const noexcept;

protected:
    EnableSharedFromThis() = default;
    EnableSharedFromThis(const EnableSharedFromThis&) = default;
    EnableSharedFromThis& operator=(const EnableSharedFromThis&) = default;
    ~EnableSharedFromThis() = default;

private:
    template <typename U> friend class SharedPtr;
    mutable WeakPtr<T> weak_this_;
};

// ==================== 测试函数声明 ====================

void testWeakPtr();          // 测试面试者实现
void testWeakPtrSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testWeakPtr();
    testWeakPtrSolution();
}

} // namespace WeakPtrImpl

#endif // WEAK_PTR_H
