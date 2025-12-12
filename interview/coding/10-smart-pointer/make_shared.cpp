/**
 * @file make_shared.cpp
 * @brief make_shared/make_unique 实现 - 面试题
 *
 * 深入理解智能指针工厂函数的实现原理和优势
 */

#include <iostream>
#include <memory>
#include <utility>

/**
 * 题目1：为什么要使用 make_shared/make_unique？
 *
 * 分析以下代码的问题：
 */
void problematicCode() {
    // 问题1：异常安全
    // 如果 g() 抛出异常，可能导致内存泄漏
    // foo(std::shared_ptr<int>(new int(42)), g());

    // 问题2：两次内存分配
    // new int 分配一次
    // shared_ptr 控制块分配一次
    // std::shared_ptr<int> p(new int(42));

    // TODO: 解释为什么 make_shared 能解决这些问题
}

/**
 * 题目2：实现 make_unique
 *
 * 要求：
 * 1. 完美转发参数
 * 2. 支持单对象和数组
 */
template <typename T, typename... Args>
std::unique_ptr<T> myMakeUnique(Args&&... args) {
    // TODO: 实现 make_unique
    return nullptr;
}

// 数组版本（已知大小）
template <typename T>
std::unique_ptr<T[]> myMakeUniqueArray(size_t size) {
    // TODO: 实现数组版本
    return nullptr;
}

/**
 * 题目3：实现 make_shared
 *
 * 要求：
 * 1. 单次内存分配（对象和控制块一起分配）
 * 2. 完美转发参数
 */

// 简化的控制块（用于演示）
template <typename T>
struct SharedControlBlock {
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;
    alignas(T) char storage[sizeof(T)];  // 对象存储

    template <typename... Args>
    SharedControlBlock(Args&&... args)
        : strong_count(1), weak_count(0) {
        new (storage) T(std::forward<Args>(args)...);
    }

    T* getPtr() {
        return reinterpret_cast<T*>(storage);
    }

    void destroy() {
        getPtr()->~T();
    }
};

template <typename T>
class MySharedPtr {
public:
    // TODO: 实现支持 make_shared 的 SharedPtr
};

template <typename T, typename... Args>
MySharedPtr<T> myMakeShared(Args&&... args) {
    // TODO: 实现 make_shared
    return MySharedPtr<T>();
}

/**
 * 题目4：make_shared 的缺点
 *
 * 分析 make_shared 在以下场景的问题：
 * 1. 自定义删除器
 * 2. weak_ptr 延长控制块生命周期
 * 3. 大对象分配
 */
void makeSharedDrawbacks() {
    // 场景1：不能使用自定义删除器
    // auto p = std::make_shared<FILE>(fopen("test.txt", "r"));  // 错误!

    // 场景2：weak_ptr 延长内存占用
    // std::weak_ptr<LargeObject> wp;
    // {
    //     auto sp = std::make_shared<LargeObject>();
    //     wp = sp;
    // }
    // 此时对象已销毁，但内存仍被占用（因为控制块和对象在一起）

    // TODO: 解释这些问题的原因和解决方案
}

/**
 * 题目5：allocate_shared 实现
 *
 * 要求：支持自定义分配器
 */
template <typename T, typename Alloc, typename... Args>
std::shared_ptr<T> myAllocateShared(const Alloc& alloc, Args&&... args) {
    // TODO: 实现 allocate_shared
    return nullptr;
}

int main() {
    return 0;
}
