/**
 * @file shared_ptr_solution.cpp
 * @brief shared_ptr 实现 - 参考答案
 */

#include "shared_ptr.h"
#include <iostream>
#include <cassert>

namespace SharedPtrImpl {

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

// ==================== 基础 SharedPtr ====================

template <typename T>
class SharedPtr {
public:
    explicit SharedPtr(T* ptr = nullptr)
        : ptr_(ptr), count_(ptr ? new long(1) : nullptr) {}

    ~SharedPtr() {
        release();
    }

    SharedPtr(const SharedPtr& other)
        : ptr_(other.ptr_), count_(other.count_) {
        if (count_) {
            ++(*count_);
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            if (count_) {
                ++(*count_);
            }
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(other.ptr_), count_(other.count_) {
        other.ptr_ = nullptr;
        other.count_ = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            other.ptr_ = nullptr;
            other.count_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    long use_count() const noexcept {
        return count_ ? *count_ : 0;
    }

    bool unique() const noexcept {
        return use_count() == 1;
    }

    void reset(T* ptr = nullptr) {
        release();
        ptr_ = ptr;
        count_ = ptr ? new long(1) : nullptr;
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(count_, other.count_);
    }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    void release() {
        if (count_ && --(*count_) == 0) {
            delete ptr_;
            delete count_;
        }
        ptr_ = nullptr;
        count_ = nullptr;
    }

    T* ptr_;
    long* count_;
};

// ==================== 线程安全的 SharedPtr ====================

template <typename T>
class SharedPtrAtomic {
public:
    explicit SharedPtrAtomic(T* ptr = nullptr)
        : ptr_(ptr), count_(ptr ? new std::atomic<long>(1) : nullptr) {}

    ~SharedPtrAtomic() {
        release();
    }

    SharedPtrAtomic(const SharedPtrAtomic& other)
        : ptr_(other.ptr_), count_(other.count_) {
        if (count_) {
            count_->fetch_add(1, std::memory_order_relaxed);
        }
    }

    SharedPtrAtomic& operator=(const SharedPtrAtomic& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            if (count_) {
                count_->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    SharedPtrAtomic(SharedPtrAtomic&& other) noexcept
        : ptr_(other.ptr_), count_(other.count_) {
        other.ptr_ = nullptr;
        other.count_ = nullptr;
    }

    SharedPtrAtomic& operator=(SharedPtrAtomic&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            other.ptr_ = nullptr;
            other.count_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    long use_count() const noexcept {
        return count_ ? count_->load(std::memory_order_relaxed) : 0;
    }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    void release() {
        if (count_) {
            // 使用 acq_rel 确保正确的同步
            if (count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete ptr_;
                delete count_;
            }
        }
        ptr_ = nullptr;
        count_ = nullptr;
    }

    T* ptr_;
    std::atomic<long>* count_;
};

// ==================== 控制块 ====================

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

    bool release_strong() {
        if (strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr;
            ptr = nullptr;
            // 如果没有弱引用，删除控制块
            if (weak_count.load(std::memory_order_acquire) == 0) {
                return true;  // 需要删除控制块
            }
        }
        return false;
    }

    bool release_weak() {
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            // 如果强引用已经为 0，删除控制块
            if (strong_count.load(std::memory_order_acquire) == 0) {
                return true;
            }
        }
        return false;
    }
};

template <typename T>
class SharedPtrWithControlBlock {
public:
    explicit SharedPtrWithControlBlock(T* ptr = nullptr)
        : cb_(ptr ? new ControlBlock<T>(ptr) : nullptr) {}

    ~SharedPtrWithControlBlock() {
        if (cb_ && cb_->release_strong()) {
            delete cb_;
        }
    }

    SharedPtrWithControlBlock(const SharedPtrWithControlBlock& other)
        : cb_(other.cb_) {
        if (cb_) {
            cb_->add_strong();
        }
    }

    SharedPtrWithControlBlock& operator=(const SharedPtrWithControlBlock& other) {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) {
                delete cb_;
            }
            cb_ = other.cb_;
            if (cb_) {
                cb_->add_strong();
            }
        }
        return *this;
    }

    SharedPtrWithControlBlock(SharedPtrWithControlBlock&& other) noexcept
        : cb_(other.cb_) {
        other.cb_ = nullptr;
    }

    SharedPtrWithControlBlock& operator=(SharedPtrWithControlBlock&& other) noexcept {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) {
                delete cb_;
            }
            cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept {
        return cb_ ? cb_->ptr : nullptr;
    }

    long use_count() const noexcept {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    T& operator*() const { return *get(); }
    T* operator->() const noexcept { return get(); }
    explicit operator bool() const noexcept { return get() != nullptr; }

private:
    ControlBlock<T>* cb_;
};

// ==================== make_shared ====================

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace Solution

// ==================== 测试函数 ====================

void testSharedPtrSolution() {
    std::cout << "=== Shared Ptr Tests (Solution) ===" << std::endl;

    // 测试基础 SharedPtr
    TestClass::instanceCount = 0;
    {
        Solution::SharedPtr<TestClass> p1(new TestClass(1));
        assert(p1.use_count() == 1);
        assert(TestClass::instanceCount == 1);

        {
            Solution::SharedPtr<TestClass> p2 = p1;
            assert(p1.use_count() == 2);
            assert(p2.use_count() == 2);
        }

        assert(p1.use_count() == 1);
    }
    assert(TestClass::instanceCount == 0);
    std::cout << "  SharedPtr basic: PASSED" << std::endl;

    // 测试移动语义
    {
        Solution::SharedPtr<TestClass> p1(new TestClass(2));
        Solution::SharedPtr<TestClass> p2 = std::move(p1);
        assert(!p1);
        assert(p2.use_count() == 1);
    }
    std::cout << "  SharedPtr move: PASSED" << std::endl;

    // 测试 reset
    TestClass::instanceCount = 0;
    {
        Solution::SharedPtr<TestClass> p1(new TestClass(1));
        assert(TestClass::instanceCount == 1);
        p1.reset(new TestClass(2));
        assert(TestClass::instanceCount == 1);
        assert(p1->value == 2);
    }
    assert(TestClass::instanceCount == 0);
    std::cout << "  SharedPtr reset: PASSED" << std::endl;

    // 测试线程安全 SharedPtr
    TestClass::instanceCount = 0;
    {
        Solution::SharedPtrAtomic<TestClass> p1(new TestClass(3));
        Solution::SharedPtrAtomic<TestClass> p2 = p1;
        assert(p1.use_count() == 2);
    }
    assert(TestClass::instanceCount == 0);
    std::cout << "  SharedPtrAtomic: PASSED" << std::endl;

    // 测试控制块版本
    TestClass::instanceCount = 0;
    {
        Solution::SharedPtrWithControlBlock<TestClass> p1(new TestClass(4));
        Solution::SharedPtrWithControlBlock<TestClass> p2 = p1;
        assert(p1.use_count() == 2);
    }
    assert(TestClass::instanceCount == 0);
    std::cout << "  SharedPtrWithControlBlock: PASSED" << std::endl;

    // 测试 make_shared
    {
        auto p = Solution::makeShared<TestClass>(42);
        assert(p->value == 42);
        assert(p.use_count() == 1);
    }
    std::cout << "  makeShared: PASSED" << std::endl;
}

} // namespace SharedPtrImpl
