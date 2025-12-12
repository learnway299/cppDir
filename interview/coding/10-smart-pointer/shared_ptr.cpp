/**
 * @file shared_ptr.cpp
 * @brief shared_ptr 实现 - 面试题
 *
 * 实现一个简化版的 shared_ptr，支持：
 * 1. 引用计数
 * 2. 拷贝和移动语义
 * 3. 线程安全的引用计数
 */

#include <iostream>
#include <atomic>

/**
 * 题目1：实现基础 SharedPtr（非线程安全版本）
 *
 * 要求：
 * 1. 支持拷贝构造和拷贝赋值
 * 2. 支持移动构造和移动赋值
 * 3. 引用计数为 0 时释放资源
 * 4. 支持 use_count() 返回引用计数
 */
template <typename T>
class SharedPtr {
public:
    // TODO: 实现构造函数
    explicit SharedPtr(T* ptr = nullptr) {
    }

    // TODO: 实现析构函数
    ~SharedPtr() {
    }

    // TODO: 实现拷贝构造
    SharedPtr(const SharedPtr& other) {
    }

    // TODO: 实现拷贝赋值
    SharedPtr& operator=(const SharedPtr& other) {
        return *this;
    }

    // TODO: 实现移动构造
    SharedPtr(SharedPtr&& other) noexcept {
    }

    // TODO: 实现移动赋值
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        return *this;
    }

    // TODO: 实现 get()
    T* get() const noexcept {
        return nullptr;
    }

    // TODO: 实现 use_count()
    long use_count() const noexcept {
        return 0;
    }

    // TODO: 实现 reset()
    void reset(T* ptr = nullptr) {
    }

    T& operator*() const {
        return *get();
    }

    T* operator->() const noexcept {
        return get();
    }

    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

private:
    T* ptr_;
    long* count_;  // 引用计数
};

/**
 * 题目2：实现线程安全的 SharedPtr
 *
 * 要求：
 * 1. 使用 atomic 实现线程安全的引用计数
 * 2. 保证引用计数的原子性操作
 */
template <typename T>
class SharedPtrAtomic {
public:
    // TODO: 实现线程安全版本
    explicit SharedPtrAtomic(T* ptr = nullptr) {
    }

private:
    T* ptr_;
    std::atomic<long>* count_;
};

/**
 * 题目3：实现控制块（标准库实现方式）
 *
 * 控制块包含：
 * 1. 强引用计数（shared_ptr 数量）
 * 2. 弱引用计数（weak_ptr 数量）
 * 3. 删除器
 * 4. 分配器
 */
struct ControlBlock {
    // TODO: 实现控制块
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;

    ControlBlock() : strong_count(1), weak_count(0) {}
};

template <typename T>
class SharedPtrWithControlBlock {
public:
    // TODO: 实现带控制块的 SharedPtr
};

/**
 * 题目4：实现 make_shared
 *
 * 要求：
 * 1. 单次内存分配（对象和控制块一起分配）
 * 2. 完美转发参数
 */
template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    // TODO: 实现 make_shared
    return SharedPtr<T>(nullptr);
}

int main() {
    return 0;
}
