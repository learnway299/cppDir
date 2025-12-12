/**
 * @file object_pool.cpp
 * @brief 对象池实现 - 面试题
 *
 * 对象池专门用于管理特定类型对象的创建和复用
 */

#include <cstddef>
#include <vector>
#include <memory>

/**
 * 题目1：实现简单对象池
 *
 * 要求：
 * 1. 预创建一定数量的对象
 * 2. acquire() 获取对象
 * 3. release() 归还对象
 * 4. 支持对象复用
 */
template <typename T>
class SimpleObjectPool {
public:
    // TODO: 实现构造函数，预创建 size 个对象
    explicit SimpleObjectPool(size_t size) {
    }

    // TODO: 获取一个对象
    T* acquire() {
        return nullptr;
    }

    // TODO: 归还一个对象
    void release(T* obj) {
    }

    // TODO: 返回可用对象数
    size_t available() const {
        return 0;
    }

private:
    // TODO: 定义成员变量
};

/**
 * 题目2：实现带重置功能的对象池
 *
 * 要求：
 * 1. 对象归还时自动重置状态
 * 2. 支持自定义重置函数
 */
template <typename T>
class ResettableObjectPool {
public:
    using ResetFunc = void(*)(T&);

    // TODO: 实现带重置函数的对象池
    explicit ResettableObjectPool(size_t size, ResetFunc resetFunc = nullptr) {
    }

    T* acquire() {
        return nullptr;
    }

    void release(T* obj) {
    }

private:
    // TODO: 定义成员变量
};

/**
 * 题目3：实现 RAII 包装的对象池
 *
 * 要求：
 * 1. acquire() 返回智能指针
 * 2. 智能指针析构时自动归还对象
 * 3. 支持自定义删除器
 */
template <typename T>
class SmartObjectPool {
public:
    // 自定义删除器，归还而非删除
    struct Deleter {
        // TODO: 实现删除器
    };

    using Ptr = std::unique_ptr<T, Deleter>;

    explicit SmartObjectPool(size_t size) {
    }

    // TODO: 获取对象，返回智能指针
    Ptr acquire() {
        return nullptr;
    }

private:
    // TODO: 定义成员变量
};

/**
 * 题目4：实现线程安全的对象池
 *
 * 要求：
 * 1. 支持多线程并发访问
 * 2. 使用互斥锁保护
 */
template <typename T>
class ThreadSafeObjectPool {
public:
    explicit ThreadSafeObjectPool(size_t size) {
    }

    T* acquire() {
        return nullptr;
    }

    void release(T* obj) {
    }

private:
    // TODO: 定义成员变量
    // 提示：使用 std::mutex
};

int main() {
    return 0;
}
