/**
 * @file memory_pool.h
 * @brief 内存池实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 固定大小内存池
 * 2. 模板化对象池
 * 3. 可增长内存池
 * 4. 线程安全内存池
 */

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <atomic>

namespace MemoryPoolImpl {

// ==================== 题目1: 固定大小内存池 ====================

/**
 * 题目1: 实现固定大小内存池
 *
 * 原理：
 * 1. 预分配一大块连续内存
 * 2. 将内存分成固定大小的块
 * 3. 使用空闲链表管理（每个空闲块存储下一个空闲块的指针）
 * 4. O(1) 分配和释放
 *
 * 关键技术：
 * - 块大小必须至少能存储一个指针
 * - 使用链表头插法进行分配和回收
 * - 检查释放的指针是否属于池内
 *
 * 示例：
 *   FixedSizePool pool(sizeof(int), 100);  // 100个int大小的块
 *   int* p = static_cast<int*>(pool.allocate());
 *   *p = 42;
 *   pool.deallocate(p);
 */
class FixedSizePool {
public:
    FixedSizePool(size_t blockSize, size_t blockCount);
    ~FixedSizePool();

    // 禁止拷贝
    FixedSizePool(const FixedSizePool&) = delete;
    FixedSizePool& operator=(const FixedSizePool&) = delete;

    void* allocate();
    void deallocate(void* ptr);

    size_t available() const;
    size_t capacity() const;
    size_t blockSize() const;

private:
    char* memory_;       // 原始内存
    char* freeList_;     // 空闲链表头
    size_t blockSize_;   // 块大小
    size_t blockCount_;  // 块数量
    size_t freeCount_;   // 可用块数
};

// ==================== 题目2: 模板化对象池 ====================

/**
 * 题目2: 实现模板化对象池
 *
 * 要求：
 * 1. 支持任意类型 T
 * 2. 自动计算块大小
 * 3. construct: 分配内存 + 构造对象
 * 4. destroy: 析构对象 + 释放内存
 *
 * 优势：
 * - 类型安全
 * - 自动管理构造/析构
 * - 异常安全（构造失败会归还内存）
 *
 * 示例：
 *   ObjectPool<std::string, 10> pool;
 *   std::string* s = pool.construct("hello");
 *   pool.destroy(s);
 */
template <typename T, size_t BlockCount = 32>
class ObjectPool {
public:
    ObjectPool();
    ~ObjectPool();

    template <typename... Args>
    T* construct(Args&&... args);

    void destroy(T* ptr);

    size_t available() const;

private:
    FixedSizePool pool_;
};

// ==================== 题目3: 可增长内存池 ====================

/**
 * 题目3: 实现可增长内存池
 *
 * 特点：
 * 1. 初始分配一块内存
 * 2. 内存不足时自动扩展（分配新的大块）
 * 3. 析构时释放所有分配的内存块
 *
 * 与固定内存池对比：
 * - 固定池：容量固定，分配失败返回 nullptr
 * - 可增长池：自动扩展，几乎不会失败
 *
 * 示例：
 *   GrowablePool pool(sizeof(int), 10);  // 初始10个块
 *   for (int i = 0; i < 100; i++) {
 *       pool.allocate();  // 自动扩展
 *   }
 */
class GrowablePool {
public:
    GrowablePool(size_t blockSize, size_t initialBlocks = 32);
    ~GrowablePool();

    // 禁止拷贝
    GrowablePool(const GrowablePool&) = delete;
    GrowablePool& operator=(const GrowablePool&) = delete;

    void* allocate();
    void deallocate(void* ptr);

    size_t available() const;
    size_t capacity() const;

private:
    void grow();

    size_t blockSize_;
    size_t growSize_;
    char* freeList_;
    size_t totalBlocks_;
    size_t freeCount_;
    std::vector<char*> chunks_;  // 所有分配的大块
};

// ==================== 题目4: 线程安全内存池 ====================

/**
 * 题目4: 实现线程安全内存池
 *
 * 方法：使用自旋锁保护关键区域
 *
 * 注意事项：
 * - 自旋锁适合短临界区
 * - 高竞争场景可能需要更复杂的策略
 */
class SpinLock {
public:
    void lock();
    void unlock();

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

class ThreadSafePool {
public:
    ThreadSafePool(size_t blockSize, size_t blockCount);

    void* allocate();
    void deallocate(void* ptr);
    size_t available();

private:
    FixedSizePool pool_;
    SpinLock lock_;
};

// ==================== 测试辅助 ====================

struct TestObject {
    int id;
    double value;
    static int instanceCount;

    TestObject(int i, double v);
    ~TestObject();
};

// ==================== 测试函数声明 ====================

void testMemoryPool();          // 测试面试者实现
void testMemoryPoolSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testMemoryPool();
    testMemoryPoolSolution();
}

} // namespace MemoryPoolImpl

#endif // MEMORY_POOL_H
