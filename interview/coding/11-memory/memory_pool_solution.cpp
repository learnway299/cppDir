/**
 * @file memory_pool_solution.cpp
 * @brief 内存池实现 - 解答
 */

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <new>
#include <utility>

/**
 * 题目1：实现固定大小内存池
 *
 * 原理：
 * 1. 预分配连续内存块
 * 2. 空闲块通过链表串联
 * 3. 每个空闲块的前几字节存储下一个空闲块的指针
 */
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

/**
 * 题目2：实现模板化内存池
 *
 * 支持对象构造和析构
 */
template <typename T, size_t BlockCount = 32>
class ObjectPool {
public:
    ObjectPool() : pool_(sizeof(T), BlockCount) {}

    ~ObjectPool() {
        // 注意：这里不会自动调用未释放对象的析构函数
        // 使用者需要确保所有对象都被 destroy
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

/**
 * 题目3：实现可增长内存池
 *
 * 内存不足时自动扩展
 */
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
    std::vector<char*> chunks_;  // 所有分配的大块
};

/**
 * 扩展：线程安全的内存池（使用自旋锁）
 */
#include <atomic>

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

private:
    FixedSizePool pool_;
    SpinLock lock_;
};

// 测试类
struct TestObject {
    int id;
    double value;

    TestObject(int i, double v) : id(i), value(v) {
        std::cout << "TestObject(" << id << ") constructed\n";
    }

    ~TestObject() {
        std::cout << "TestObject(" << id << ") destroyed\n";
    }
};

int main() {
    std::cout << "=== 测试固定大小内存池 ===\n";
    {
        FixedSizePool pool(sizeof(int), 5);
        std::cout << "Initial available: " << pool.available() << "\n";

        int* arr[5];
        for (int i = 0; i < 5; ++i) {
            arr[i] = static_cast<int*>(pool.allocate());
            *arr[i] = i * 10;
            std::cout << "Allocated: " << *arr[i]
                      << ", available: " << pool.available() << "\n";
        }

        // 尝试分配第6个（应失败）
        void* extra = pool.allocate();
        std::cout << "Extra allocation: " << (extra ? "success" : "failed") << "\n";

        // 释放一些
        pool.deallocate(arr[2]);
        std::cout << "After dealloc, available: " << pool.available() << "\n";

        // 再次分配
        int* newPtr = static_cast<int*>(pool.allocate());
        *newPtr = 999;
        std::cout << "New allocation: " << *newPtr << "\n";
    }

    std::cout << "\n=== 测试对象池 ===\n";
    {
        ObjectPool<TestObject, 3> pool;

        TestObject* obj1 = pool.construct(1, 1.1);
        TestObject* obj2 = pool.construct(2, 2.2);

        std::cout << "obj1: id=" << obj1->id << ", value=" << obj1->value << "\n";
        std::cout << "obj2: id=" << obj2->id << ", value=" << obj2->value << "\n";

        pool.destroy(obj1);
        std::cout << "After destroy obj1, available: " << pool.available() << "\n";

        TestObject* obj3 = pool.construct(3, 3.3);
        std::cout << "obj3: id=" << obj3->id << ", value=" << obj3->value << "\n";

        pool.destroy(obj2);
        pool.destroy(obj3);
    }

    std::cout << "\n=== 测试可增长内存池 ===\n";
    {
        GrowablePool pool(sizeof(int), 3);
        std::cout << "Initial capacity: " << pool.capacity() << "\n";

        std::vector<int*> ptrs;
        for (int i = 0; i < 10; ++i) {
            int* p = static_cast<int*>(pool.allocate());
            *p = i;
            ptrs.push_back(p);
            std::cout << "Allocated " << i
                      << ", capacity: " << pool.capacity()
                      << ", available: " << pool.available() << "\n";
        }

        for (int* p : ptrs) {
            pool.deallocate(p);
        }
        std::cout << "After dealloc all, available: " << pool.available() << "\n";
    }

    return 0;
}
