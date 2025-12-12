/**
 * @file unique_ptr.cpp
 * @brief unique_ptr 实现 - 面试题
 *
 * 实现一个简化版的 unique_ptr，支持：
 * 1. 独占所有权语义
 * 2. 移动语义
 * 3. 自定义删除器
 * 4. 数组特化
 */

#include <iostream>
#include <utility>

/**
 * 题目1：实现基础 UniquePtr
 *
 * 要求：
 * 1. 禁止拷贝构造和拷贝赋值
 * 2. 支持移动构造和移动赋值
 * 3. 析构时自动释放资源
 * 4. 支持 get()、release()、reset() 操作
 * 5. 支持 operator* 和 operator->
 */
template <typename T>
class UniquePtr {
public:
    // TODO: 实现构造函数
    explicit UniquePtr(T* ptr = nullptr) {
    }

    // TODO: 实现析构函数
    ~UniquePtr() {
    }

    // TODO: 禁止拷贝
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // TODO: 实现移动构造
    UniquePtr(UniquePtr&& other) noexcept {
    }

    // TODO: 实现移动赋值
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        return *this;
    }

    // TODO: 实现 get()
    T* get() const noexcept {
        return nullptr;
    }

    // TODO: 实现 release()
    T* release() noexcept {
        return nullptr;
    }

    // TODO: 实现 reset()
    void reset(T* ptr = nullptr) noexcept {
    }

    // TODO: 实现 operator*
    T& operator*() const {
        return *get();
    }

    // TODO: 实现 operator->
    T* operator->() const noexcept {
        return get();
    }

    // TODO: 实现 operator bool
    explicit operator bool() const noexcept {
        return false;
    }

private:
    T* ptr_;
};

/**
 * 题目2：实现带自定义删除器的 UniquePtr
 *
 * 要求：
 * 1. 支持自定义删除器类型作为模板参数
 * 2. 删除器可以是函数指针、lambda、仿函数
 */
template <typename T, typename Deleter>
class UniquePtrWithDeleter {
public:
    // TODO: 实现
};

/**
 * 题目3：实现数组特化版本
 *
 * 要求：
 * 1. 支持 operator[]
 * 2. 使用 delete[] 释放
 */
template <typename T>
class UniquePtrArray {
public:
    // TODO: 实现数组版本
    explicit UniquePtrArray(T* ptr = nullptr) {
    }

    ~UniquePtrArray() {
    }

    T& operator[](size_t index) const {
        return ptr_[index];
    }

private:
    T* ptr_;
};

/**
 * 题目4：实现 make_unique
 */
template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    // TODO: 实现 make_unique
    return UniquePtr<T>(nullptr);
}

int main() {
    // 测试代码
    return 0;
}
