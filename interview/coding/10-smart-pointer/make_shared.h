/**
 * @file make_shared.h
 * @brief make_shared/make_unique 实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 为什么使用 make_shared/make_unique
 * 2. make_unique 实现
 * 3. make_shared 实现（单次分配）
 * 4. make_shared 的缺点
 * 5. allocate_shared 实现
 */

#ifndef MAKE_SHARED_H
#define MAKE_SHARED_H

#include <memory>
#include <utility>
#include <atomic>
#include <cstddef>
#include <string>

namespace MakeSharedImpl {

// ==================== 测试类 ====================

struct TestClass {
    int value;
    std::string name;
    static int instanceCount;

    TestClass(int v, const std::string& n);
    ~TestClass();
};

// ==================== 题目1: 为什么使用 make_shared ====================

/**
 * 题目1: 为什么要使用 make_shared/make_unique？
 *
 * 问题1：异常安全
 *   foo(std::shared_ptr<int>(new int(42)), g());
 *   // 如果 g() 抛出异常，可能导致 new int(42) 内存泄漏
 *   // 原因：C++ 不保证函数参数的求值顺序
 *
 * 问题2：两次内存分配
 *   std::shared_ptr<int> p(new int(42));
 *   // new int 分配一次
 *   // shared_ptr 控制块分配一次
 *
 * 解决方案：
 *   foo(std::make_shared<int>(42), g());  // 异常安全
 *   auto p = std::make_shared<int>(42);   // 单次分配
 */

// ==================== 题目2: make_unique 实现 ====================

/**
 * 题目2: 实现 make_unique
 *
 * 要求：
 * 1. 完美转发参数到构造函数
 * 2. 返回 std::unique_ptr<T>
 *
 * 数组版本：
 * - make_unique<T[]>(size) 创建数组
 * - 使用 new T[size]() 进行值初始化
 *
 * 示例：
 *   auto p = myMakeUnique<std::string>("hello");
 *   auto arr = myMakeUniqueArray<int>(10);
 */
template <typename T, typename... Args>
std::unique_ptr<T> myMakeUnique(Args&&... args);

template <typename T>
std::unique_ptr<T[]> myMakeUniqueArray(size_t size);

// ==================== 题目3: make_shared 实现 ====================

/**
 * 题目3: 实现 make_shared（单次分配版本）
 *
 * 关键技术：
 * 1. 将对象和控制块合并分配（CombinedBlock）
 * 2. 使用 placement new 在预分配内存中构造对象
 * 3. 正确处理对齐
 *
 * 优势：
 * - 单次内存分配（更高效）
 * - 更好的缓存局部性
 * - 异常安全
 */

template <typename T>
struct alignas(std::max_align_t) CombinedBlock {
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;
    alignas(T) unsigned char storage[sizeof(T)];

    template <typename... Args>
    explicit CombinedBlock(Args&&... args);

    T* getPtr() noexcept;
    void destroyObject();
};

template <typename T>
class MySharedPtr {
public:
    MySharedPtr() noexcept;
    explicit MySharedPtr(CombinedBlock<T>* block) noexcept;
    ~MySharedPtr();

    MySharedPtr(const MySharedPtr& other) noexcept;
    MySharedPtr& operator=(const MySharedPtr& other) noexcept;

    MySharedPtr(MySharedPtr&& other) noexcept;
    MySharedPtr& operator=(MySharedPtr&& other) noexcept;

    T* get() const noexcept;
    long use_count() const noexcept;

    T& operator*() const;
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;

private:
    void release();
    CombinedBlock<T>* block_;
};

template <typename T, typename... Args>
MySharedPtr<T> myMakeShared(Args&&... args);

// ==================== 题目4: make_shared 的缺点 ====================

/**
 * 题目4: make_shared 的缺点
 *
 * 1. 不支持自定义删除器
 *    - make_shared 没有接受删除器的重载
 *    - 需要自定义删除器时必须使用 shared_ptr(ptr, deleter)
 *
 * 2. weak_ptr 延长内存占用
 *    - 对象和控制块一起分配
 *    - 只要有 weak_ptr 存在，整块内存都不能释放
 *    - 即使对象已被销毁，内存仍被占用
 *
 * 3. 大对象问题
 *    - 如果对象很大，weak_ptr 会长期占用大块内存
 *    - 使用 shared_ptr(new T) 可以分开释放
 *
 * 4. 不支持 {} 初始化
 *    - make_shared<T>({...}) 不能工作
 *    - 需要用 shared_ptr<T>(new T{...})
 */

// ==================== 题目5: allocate_shared ====================

/**
 * 题目5: allocate_shared 实现
 *
 * 使用自定义分配器的 make_shared
 * - 分配器需要满足 Allocator 概念
 * - 分配足够空间存储对象和控制块
 */
template <typename T, typename Allocator, typename... Args>
MySharedPtr<T> myAllocateShared(const Allocator& alloc, Args&&... args);

// ==================== 测试函数声明 ====================

void testMakeShared();          // 测试面试者实现
void testMakeSharedSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMakeShared();
    testMakeSharedSolution();
}

} // namespace MakeSharedImpl

#endif // MAKE_SHARED_H
