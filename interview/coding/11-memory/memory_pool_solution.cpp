/**
 * @file memory_pool_solution.cpp
 * @brief 内存池实现 - 参考答案
 */

#include "memory_pool.h"
#include <iostream>
#include <cassert>
#include <new>
#include <utility>
#include <algorithm>

namespace MemoryPoolImpl {

namespace Solution {

// ==================== 固定大小内存池 ====================

class FixedSizePool {
public:
    FixedSizePool(size_t blockSize, size_t blockCount)
        : blockSize_(std::max(blockSize, sizeof(void*))),
          blockCount_(blockCount),
          freeCount_(blockCount) {

        // 分配原始内存
        memory_ = new char[blockSize_ * blockCount_];

        // 初始化空闲链表
        freeList_ = memory_;
        char* current = memory_;
        for (size_t i = 0; i < blockCount_ - 1; ++i) {
            char* next = current + blockSize_;
            *reinterpret_cast<char**>(current) = next;
            current = next;
        }
        *reinterpret_cast<char**>(current) = nullptr;
    }

    ~FixedSizePool() {
        delete[] memory_;
    }

    // 禁止拷贝
    FixedSizePool(const FixedSizePool&) = delete;
    FixedSizePool& operator=(const FixedSizePool&) = delete;

    void* allocate() {
        if (!freeList_) {
            return nullptr;  // 内存池已满
        }

        void* block = freeList_;
        freeList_ = *reinterpret_cast<char**>(freeList_);
        --freeCount_;
        return block;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;

        // 检查指针是否在池范围内
        char* p = static_cast<char*>(ptr);
        if (p < memory_ || p >= memory_ + blockSize_ * blockCount_) {
            return;  // 不属于此池
        }

        // 插入到空闲链表头部
        *reinterpret_cast<char**>(ptr) = freeList_;
        freeList_ = static_cast<char*>(ptr);
        ++freeCount_;
    }

    size_t available() const { return freeCount_; }
    size_t capacity() const { return blockCount_; }
    size_t blockSize() const { return blockSize_; }

private:
    char* memory_;       // 原始内存
    char* freeList_;     // 空闲链表头
    size_t blockSize_;   // 块大小
    size_t blockCount_;  // 块数量
    size_t freeCount_;   // 可用块数
};

// ==================== 模板化对象池 ====================

template <typename T, size_t BlockCount = 32>
class ObjectPool {
public:
    ObjectPool() : pool_(sizeof(T), BlockCount) {}

    ~ObjectPool() {
        // 注意：未释放的对象不会自动调用析构函数
    }

    template <typename... Args>
    T* construct(Args&&... args) {
        void* mem = pool_.allocate();
        if (!mem) return nullptr;

        try {
            return new (mem) T(std::forward<Args>(args)...);
        } catch (...) {
            pool_.deallocate(mem);
            throw;
        }
    }

    void destroy(T* ptr) {
        if (ptr) {
            ptr->~T();
            pool_.deallocate(ptr);
        }
    }

    size_t available() const { return pool_.available(); }

private:
    FixedSizePool pool_;
};

// ==================== 可增长内存池 ====================

class GrowablePool {
public:
    GrowablePool(size_t blockSize, size_t initialBlocks = 32)
        : blockSize_(std::max(blockSize, sizeof(void*))),
          growSize_(initialBlocks),
          freeList_(nullptr),
          totalBlocks_(0),
          freeCount_(0) {

        grow();
    }

    ~GrowablePool() {
        for (char* chunk : chunks_) {
            delete[] chunk;
        }
    }

    GrowablePool(const GrowablePool&) = delete;
    GrowablePool& operator=(const GrowablePool&) = delete;

    void* allocate() {
        if (!freeList_) {
            grow();
        }

        void* block = freeList_;
        freeList_ = *reinterpret_cast<char**>(freeList_);
        --freeCount_;
        return block;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;

        *reinterpret_cast<char**>(ptr) = freeList_;
        freeList_ = static_cast<char*>(ptr);
        ++freeCount_;
    }

    size_t available() const { return freeCount_; }
    size_t capacity() const { return totalBlocks_; }

private:
    void grow() {
        // 分配新的内存块
        char* newChunk = new char[blockSize_ * growSize_];
        chunks_.push_back(newChunk);

        // 将新块加入空闲链表
        char* current = newChunk;
        for (size_t i = 0; i < growSize_ - 1; ++i) {
            char* next = current + blockSize_;
            *reinterpret_cast<char**>(current) = next;
            current = next;
        }
        *reinterpret_cast<char**>(current) = freeList_;
        freeList_ = newChunk;

        totalBlocks_ += growSize_;
        freeCount_ += growSize_;
    }

    size_t blockSize_;
    size_t growSize_;
    char* freeList_;
    size_t totalBlocks_;
    size_t freeCount_;
    std::vector<char*> chunks_;
};

// ==================== 自旋锁 ====================

class SpinLock {
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

// ==================== 线程安全的内存池 ====================

class ThreadSafePool {
public:
    ThreadSafePool(size_t blockSize, size_t blockCount)
        : pool_(blockSize, blockCount) {}

    void* allocate() {
        lock_.lock();
        void* ptr = pool_.allocate();
        lock_.unlock();
        return ptr;
    }

    void deallocate(void* ptr) {
        lock_.lock();
        pool_.deallocate(ptr);
        lock_.unlock();
    }

    size_t available() {
        lock_.lock();
        size_t count = pool_.available();
        lock_.unlock();
        return count;
    }

private:
    FixedSizePool pool_;
    SpinLock lock_;
};

// ==================== 测试对象 ====================

struct TestObject {
    int id;
    double value;
    static int instanceCount;

    TestObject(int i, double v) : id(i), value(v) {
        ++instanceCount;
    }

    ~TestObject() {
        --instanceCount;
    }
};

int TestObject::instanceCount = 0;

} // namespace Solution

// ==================== 测试函数 ====================

void testMemoryPoolSolution() {
    std::cout << "=== Memory Pool Tests (Solution) ===" << std::endl;

    // 测试固定大小内存池
    {
        Solution::FixedSizePool pool(sizeof(int), 5);
        assert(pool.available() == 5);

        int* arr[5];
        for (int i = 0; i < 5; ++i) {
            arr[i] = static_cast<int*>(pool.allocate());
            *arr[i] = i * 10;
        }
        assert(pool.available() == 0);

        // 尝试分配第6个（应失败）
        void* extra = pool.allocate();
        assert(extra == nullptr);

        // 释放一些
        pool.deallocate(arr[2]);
        assert(pool.available() == 1);

        // 再次分配
        int* newPtr = static_cast<int*>(pool.allocate());
        assert(newPtr != nullptr);
        *newPtr = 999;
        assert(*newPtr == 999);
    }
    std::cout << "  FixedSizePool: PASSED" << std::endl;

    // 测试对象池
    Solution::TestObject::instanceCount = 0;
    {
        Solution::ObjectPool<Solution::TestObject, 3> pool;

        Solution::TestObject* obj1 = pool.construct(1, 1.1);
        Solution::TestObject* obj2 = pool.construct(2, 2.2);
        assert(obj1->id == 1);
        assert(obj2->id == 2);
        assert(Solution::TestObject::instanceCount == 2);

        pool.destroy(obj1);
        assert(Solution::TestObject::instanceCount == 1);
        assert(pool.available() == 2);

        Solution::TestObject* obj3 = pool.construct(3, 3.3);
        assert(obj3->id == 3);

        pool.destroy(obj2);
        pool.destroy(obj3);
    }
    assert(Solution::TestObject::instanceCount == 0);
    std::cout << "  ObjectPool: PASSED" << std::endl;

    // 测试可增长内存池
    {
        Solution::GrowablePool pool(sizeof(int), 3);
        assert(pool.capacity() == 3);

        std::vector<int*> ptrs;
        for (int i = 0; i < 10; ++i) {
            int* p = static_cast<int*>(pool.allocate());
            *p = i;
            ptrs.push_back(p);
        }
        assert(pool.capacity() >= 10);

        for (int* p : ptrs) {
            pool.deallocate(p);
        }
        assert(pool.available() == pool.capacity());
    }
    std::cout << "  GrowablePool: PASSED" << std::endl;

    // 测试线程安全内存池
    {
        Solution::ThreadSafePool pool(sizeof(int), 5);
        assert(pool.available() == 5);

        int* p1 = static_cast<int*>(pool.allocate());
        int* p2 = static_cast<int*>(pool.allocate());
        assert(pool.available() == 3);

        pool.deallocate(p1);
        pool.deallocate(p2);
        assert(pool.available() == 5);
    }
    std::cout << "  ThreadSafePool: PASSED" << std::endl;
}

} // namespace MemoryPoolImpl
