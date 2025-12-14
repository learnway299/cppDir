/**
 * @file weak_ptr_solution.cpp
 * @brief weak_ptr 实现 - 参考答案
 */

#include "weak_ptr.h"
#include <iostream>
#include <cassert>

namespace WeakPtrImpl {

// ==================== 测试类 ====================

struct TestClass {
    int value;
    static int instanceCount;

    TestClass(int v = 0) : value(v) {
        ++instanceCount;
    }
    ~TestClass() {
        --instanceCount;
    }
};

int TestClass::instanceCount = 0;

namespace Solution {

// ==================== ControlBlock ====================

template <typename T>
struct ControlBlock {
    T* ptr;
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;

    explicit ControlBlock(T* p) : ptr(p), strong_count(1), weak_count(0) {}

    void add_strong() {
        strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    void add_weak() {
        weak_count.fetch_add(1, std::memory_order_relaxed);
    }

    // 返回是否需要删除控制块
    bool release_strong() {
        if (strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr;
            ptr = nullptr;
            if (weak_count.load(std::memory_order_acquire) == 0) {
                return true;
            }
        }
        return false;
    }

    bool release_weak() {
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (strong_count.load(std::memory_order_acquire) == 0) {
                return true;
            }
        }
        return false;
    }

    // 尝试增加强引用（用于 weak_ptr::lock）
    bool try_add_strong() {
        long count = strong_count.load(std::memory_order_relaxed);
        while (count > 0) {
            if (strong_count.compare_exchange_weak(
                    count, count + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed)) {
                return true;
            }
        }
        return false;
    }
};

// 前向声明
template <typename T> class WeakPtr;

// ==================== SharedPtr（支持 WeakPtr） ====================

template <typename T>
class SharedPtr {
public:
    friend class WeakPtr<T>;

    SharedPtr() noexcept : cb_(nullptr) {}

    explicit SharedPtr(T* ptr) : cb_(ptr ? new ControlBlock<T>(ptr) : nullptr) {}

    // 从控制块构造（用于 lock）
    explicit SharedPtr(ControlBlock<T>* cb) : cb_(cb) {}

    ~SharedPtr() {
        if (cb_ && cb_->release_strong()) {
            delete cb_;
        }
    }

    SharedPtr(const SharedPtr& other) : cb_(other.cb_) {
        if (cb_) cb_->add_strong();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) delete cb_;
            cb_ = other.cb_;
            if (cb_) cb_->add_strong();
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) noexcept : cb_(other.cb_) {
        other.cb_ = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) delete cb_;
            cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return cb_ ? cb_->ptr : nullptr; }
    long use_count() const noexcept {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    T& operator*() const { return *get(); }
    T* operator->() const noexcept { return get(); }
    explicit operator bool() const noexcept { return get() != nullptr; }

private:
    ControlBlock<T>* cb_;
};

// ==================== WeakPtr ====================

template <typename T>
class WeakPtr {
public:
    WeakPtr() noexcept : cb_(nullptr) {}

    WeakPtr(const SharedPtr<T>& shared) : cb_(shared.cb_) {
        if (cb_) cb_->add_weak();
    }

    ~WeakPtr() {
        if (cb_ && cb_->release_weak()) {
            delete cb_;
        }
    }

    WeakPtr(const WeakPtr& other) : cb_(other.cb_) {
        if (cb_) cb_->add_weak();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            if (cb_ && cb_->release_weak()) delete cb_;
            cb_ = other.cb_;
            if (cb_) cb_->add_weak();
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& shared) {
        if (cb_ && cb_->release_weak()) delete cb_;
        cb_ = shared.cb_;
        if (cb_) cb_->add_weak();
        return *this;
    }

    WeakPtr(WeakPtr&& other) noexcept : cb_(other.cb_) {
        other.cb_ = nullptr;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            if (cb_ && cb_->release_weak()) delete cb_;
            cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    bool expired() const noexcept {
        return !cb_ || cb_->strong_count.load(std::memory_order_relaxed) == 0;
    }

    SharedPtr<T> lock() const noexcept {
        if (cb_ && cb_->try_add_strong()) {
            return SharedPtr<T>(cb_);
        }
        return SharedPtr<T>();
    }

    long use_count() const noexcept {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    void reset() noexcept {
        if (cb_ && cb_->release_weak()) {
            delete cb_;
        }
        cb_ = nullptr;
    }

    void swap(WeakPtr& other) noexcept {
        std::swap(cb_, other.cb_);
    }

private:
    ControlBlock<T>* cb_;
};

// ==================== NodeWithCycle ====================

struct NodeWithCycle {
    int value;
    SharedPtr<NodeWithCycle> next;      // 强引用向前
    WeakPtr<NodeWithCycle> prev;        // 弱引用向后，打破循环
    static int instanceCount;

    NodeWithCycle(int v) : value(v) {
        ++instanceCount;
    }
    ~NodeWithCycle() {
        --instanceCount;
    }
};

int NodeWithCycle::instanceCount = 0;

// ==================== EnableSharedFromThis ====================

template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> shared_from_this() {
        return weak_this_.lock();
    }

    WeakPtr<T> weak_from_this() const noexcept {
        return weak_this_;
    }

protected:
    EnableSharedFromThis() = default;
    EnableSharedFromThis(const EnableSharedFromThis&) = default;
    EnableSharedFromThis& operator=(const EnableSharedFromThis&) = default;
    ~EnableSharedFromThis() = default;

private:
    mutable WeakPtr<T> weak_this_;
};

} // namespace Solution

// ==================== 测试函数 ====================

void testWeakPtrSolution() {
    std::cout << "=== Weak Ptr Tests (Solution) ===" << std::endl;

    // 测试 WeakPtr 基本功能
    TestClass::instanceCount = 0;
    {
        Solution::SharedPtr<TestClass> sp(new TestClass(42));
        Solution::WeakPtr<TestClass> wp(sp);

        assert(sp.use_count() == 1);
        assert(!wp.expired());

        if (auto locked = wp.lock()) {
            assert(locked->value == 42);
            assert(sp.use_count() == 2);
        }
        assert(sp.use_count() == 1);
    }
    assert(TestClass::instanceCount == 0);
    std::cout << "  WeakPtr basic: PASSED" << std::endl;

    // 测试 WeakPtr 过期
    {
        Solution::WeakPtr<TestClass> wp;
        {
            Solution::SharedPtr<TestClass> sp(new TestClass(100));
            wp = sp;
            assert(!wp.expired());
        }
        assert(wp.expired());
        auto locked = wp.lock();
        assert(!locked);
    }
    std::cout << "  WeakPtr expiration: PASSED" << std::endl;

    // 测试打破循环引用
    Solution::NodeWithCycle::instanceCount = 0;
    {
        Solution::SharedPtr<Solution::NodeWithCycle> n1(new Solution::NodeWithCycle(1));
        Solution::SharedPtr<Solution::NodeWithCycle> n2(new Solution::NodeWithCycle(2));

        n1->next = n2;
        n2->prev = n1;  // weak_ptr，不增加引用计数

        assert(n1.use_count() == 1);
        assert(n2.use_count() == 2);

        // 访问 prev
        if (auto prev = n2->prev.lock()) {
            assert(prev->value == 1);
        }
    }
    assert(Solution::NodeWithCycle::instanceCount == 0);
    std::cout << "  WeakPtr break cycle: PASSED" << std::endl;

    // 测试 lock 的线程安全性（CAS）
    {
        Solution::SharedPtr<TestClass> sp(new TestClass(5));
        Solution::WeakPtr<TestClass> wp(sp);

        // 模拟多次 lock
        for (int i = 0; i < 10; ++i) {
            auto locked = wp.lock();
            assert(locked);
            assert(locked->value == 5);
        }
        assert(sp.use_count() == 1);
    }
    std::cout << "  WeakPtr lock CAS: PASSED" << std::endl;

    // 测试 WeakPtr 拷贝和移动
    {
        Solution::SharedPtr<TestClass> sp(new TestClass(7));
        Solution::WeakPtr<TestClass> wp1(sp);
        Solution::WeakPtr<TestClass> wp2 = wp1;  // 拷贝

        assert(!wp1.expired());
        assert(!wp2.expired());

        Solution::WeakPtr<TestClass> wp3 = std::move(wp1);  // 移动
        assert(!wp3.expired());
    }
    std::cout << "  WeakPtr copy/move: PASSED" << std::endl;

    // 测试 WeakPtr reset
    {
        Solution::SharedPtr<TestClass> sp(new TestClass(8));
        Solution::WeakPtr<TestClass> wp(sp);
        assert(!wp.expired());

        wp.reset();
        assert(wp.expired());
        assert(wp.use_count() == 0);
    }
    std::cout << "  WeakPtr reset: PASSED" << std::endl;
}

} // namespace WeakPtrImpl
