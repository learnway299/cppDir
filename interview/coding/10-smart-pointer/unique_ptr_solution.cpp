/**
 * @file unique_ptr_solution.cpp
 * @brief unique_ptr 实现 - 参考答案
 */

#include "unique_ptr.h"
#include <iostream>
#include <cassert>
#include <memory>

namespace SmartPointer {

// ==================== UniquePtr Solution 实现 ====================

// 由于模板类的特殊性，这里提供完整的参考实现类

namespace Solution {

template <typename T>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) noexcept : ptr_(ptr) {}

    ~UniquePtr() {
        delete ptr_;
    }

    // 禁止拷贝
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 移动构造
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    // 移动赋值
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // 支持 nullptr 赋值
    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    T* get() const noexcept {
        return ptr_;
    }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        T* old = ptr_;
        ptr_ = ptr;
        delete old;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const noexcept {
        return ptr_;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtrWithDeleter {
public:
    explicit UniquePtrWithDeleter(T* ptr = nullptr, Deleter d = Deleter())
        : ptr_(ptr), deleter_(std::move(d)) {}

    ~UniquePtrWithDeleter() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }

    UniquePtrWithDeleter(const UniquePtrWithDeleter&) = delete;
    UniquePtrWithDeleter& operator=(const UniquePtrWithDeleter&) = delete;

    UniquePtrWithDeleter(UniquePtrWithDeleter&& other) noexcept
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }

    UniquePtrWithDeleter& operator=(UniquePtrWithDeleter&& other) noexcept {
        if (this != &other) {
            if (ptr_) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }
    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        T* old = ptr_;
        ptr_ = ptr;
        if (old) {
            deleter_(old);
        }
    }

    Deleter& get_deleter() noexcept { return deleter_; }
    const Deleter& get_deleter() const noexcept { return deleter_; }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    T* ptr_;
    Deleter deleter_;
};

template <typename T>
class UniquePtrArray {
public:
    explicit UniquePtrArray(T* ptr = nullptr) noexcept : ptr_(ptr) {}

    ~UniquePtrArray() {
        delete[] ptr_;
    }

    UniquePtrArray(const UniquePtrArray&) = delete;
    UniquePtrArray& operator=(const UniquePtrArray&) = delete;

    UniquePtrArray(UniquePtrArray&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    UniquePtrArray& operator=(UniquePtrArray&& other) noexcept {
        if (this != &other) {
            delete[] ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        T* old = ptr_;
        ptr_ = ptr;
        delete[] old;
    }

    T& operator[](size_t index) const {
        return ptr_[index];
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
UniquePtrArray<T> makeUniqueArray(size_t size) {
    return UniquePtrArray<T>(new T[size]());
}

} // namespace Solution

// ==================== 测试函数 ====================

void testUniquePtrSolution() {
    std::cout << "=== Unique Ptr Tests (Solution) ===\n";

    // 测试基础 UniquePtr
    TestClass::instanceCount = 0;
    {
        Solution::UniquePtr<TestClass> p1(new TestClass(1));
        assert(p1->value == 1);
        assert(TestClass::instanceCount == 1);

        Solution::UniquePtr<TestClass> p2 = std::move(p1);
        assert(!p1);
        assert(p2->value == 1);
        assert(TestClass::instanceCount == 1);

        p2.reset(new TestClass(2));
        assert(p2->value == 2);
        assert(TestClass::instanceCount == 1);
    }
    assert(TestClass::instanceCount == 0);
    std::cout << "  UniquePtr basic: PASSED\n";

    // 测试 release
    {
        Solution::UniquePtr<TestClass> p(new TestClass(3));
        TestClass* raw = p.release();
        assert(!p);
        assert(raw->value == 3);
        delete raw;
    }
    std::cout << "  UniquePtr release: PASSED\n";

    // 测试自定义删除器
    {
        bool deleterCalled = false;
        auto customDeleter = [&deleterCalled](TestClass* p) {
            deleterCalled = true;
            delete p;
        };

        {
            Solution::UniquePtrWithDeleter<TestClass, decltype(customDeleter)>
                p(new TestClass(4), customDeleter);
            assert(p->value == 4);
        }
        assert(deleterCalled);
    }
    std::cout << "  UniquePtrWithDeleter: PASSED\n";

    // 测试数组版本
    {
        Solution::UniquePtrArray<int> arr(new int[5]);
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        assert(arr[0] == 0);
        assert(arr[1] == 10);
        assert(arr[4] == 40);
    }
    std::cout << "  UniquePtrArray: PASSED\n";

    // 测试 make_unique
    {
        auto p = Solution::makeUnique<TestClass>(42);
        assert(p->value == 42);
    }
    std::cout << "  makeUnique: PASSED\n";
}

} // namespace SmartPointer
