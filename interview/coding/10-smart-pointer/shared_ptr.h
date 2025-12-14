/**
 * @file shared_ptr.h
 * @brief shared_ptr 实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 基础 SharedPtr（非线程安全版本）
 * 2. 线程安全的 SharedPtr
 * 3. 控制块实现
 * 4. make_shared 实现
 */

#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <atomic>
#include <utility>
#include <cstddef>

namespace SharedPtrImpl {

// ==================== 题目1: 基础 SharedPtr ====================

/**
 * 题目1: 实现基础 SharedPtr（非线程安全版本）
 *
 * 要求：
 * 1. 支持拷贝构造和拷贝赋值 - 拷贝时增加引用计数
 * 2. 支持移动构造和移动赋值 - 转移所有权，不改变计数
 * 3. 引用计数为 0 时自动释放资源
 * 4. 支持 use_count() 返回当前引用计数
 * 5. 支持 reset() 重置指针
 *
 * 关键点：
 * - 引用计数与指针分离存储（需要动态分配）
 * - 注意自赋值检查
 * - 析构前先减少计数再判断是否删除
 */
template <typename T>
class SharedPtr {
public:
    explicit SharedPtr(T* ptr = nullptr);
    ~SharedPtr();

    SharedPtr(const SharedPtr& other);
    SharedPtr& operator=(const SharedPtr& other);

    SharedPtr(SharedPtr&& other) noexcept;
    SharedPtr& operator=(SharedPtr&& other) noexcept;

    T* get() const noexcept;
    long use_count() const noexcept;
    bool unique() const noexcept;
    void reset(T* ptr = nullptr);
    void swap(SharedPtr& other) noexcept;

    T& operator*() const;
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;

private:
    void release();
    T* ptr_;
    long* count_;
};

// ==================== 题目2: 线程安全的 SharedPtr ====================

/**
 * 题目2: 实现线程安全的 SharedPtr
 *
 * 要求：
 * 1. 使用 std::atomic<long> 实现线程安全的引用计数
 * 2. 保证引用计数增减的原子性
 * 3. 注意内存序的选择
 *
 * 内存序说明：
 * - fetch_add: 可用 relaxed（只需原子性）
 * - fetch_sub: 使用 acq_rel（需要与删除同步）
 */
template <typename T>
class SharedPtrAtomic {
public:
    explicit SharedPtrAtomic(T* ptr = nullptr);
    ~SharedPtrAtomic();

    SharedPtrAtomic(const SharedPtrAtomic& other);
    SharedPtrAtomic& operator=(const SharedPtrAtomic& other);

    SharedPtrAtomic(SharedPtrAtomic&& other) noexcept;
    SharedPtrAtomic& operator=(SharedPtrAtomic&& other) noexcept;

    T* get() const noexcept;
    long use_count() const noexcept;

    T& operator*() const;
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;

private:
    void release();
    T* ptr_;
    std::atomic<long>* count_;
};

// ==================== 题目3: 控制块 ====================

/**
 * 题目3: 实现控制块（更接近标准库实现）
 *
 * 控制块包含：
 * 1. 强引用计数（shared_ptr 数量）
 * 2. 弱引用计数（weak_ptr 数量）
 * 3. 指向被管理对象的指针
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
    bool release_strong();  // 返回 true 表示需要删除控制块
    bool release_weak();
};

template <typename T>
class SharedPtrWithControlBlock {
public:
    explicit SharedPtrWithControlBlock(T* ptr = nullptr);
    ~SharedPtrWithControlBlock();

    SharedPtrWithControlBlock(const SharedPtrWithControlBlock& other);
    SharedPtrWithControlBlock& operator=(const SharedPtrWithControlBlock& other);

    SharedPtrWithControlBlock(SharedPtrWithControlBlock&& other) noexcept;
    SharedPtrWithControlBlock& operator=(SharedPtrWithControlBlock&& other) noexcept;

    T* get() const noexcept;
    long use_count() const noexcept;

    T& operator*() const;
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;

private:
    ControlBlock<T>* cb_;
};

// ==================== 题目4: make_shared ====================

/**
 * 题目4: 实现 make_shared
 *
 * 优点：
 * 1. 异常安全
 * 2. 单次内存分配（优化版本可将对象和控制块一起分配）
 * 3. 代码更简洁
 */
template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args);

// ==================== 测试函数声明 ====================

void testSharedPtr();          // 测试面试者实现
void testSharedPtrSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testSharedPtr();
    testSharedPtrSolution();
}

} // namespace SharedPtrImpl

#endif // SHARED_PTR_H
