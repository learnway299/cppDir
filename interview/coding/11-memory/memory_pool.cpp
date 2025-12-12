/**
 * @file memory_pool.cpp
 * @brief 内存池实现 - 面试题
 *
 * 内存池用于高频小对象分配，减少系统调用和内存碎片
 */

#include <cstddef>
#include <cstdint>

/**
 * 题目1：实现固定大小内存池
 *
 * 要求：
 * 1. 预分配一大块内存
 * 2. 将内存分成固定大小的块
 * 3. 使用空闲链表管理
 * 4. O(1) 分配和释放
 */
class FixedSizePool {
public:
    // TODO: 实现构造函数
    // blockSize: 每个块的大小
    // blockCount: 块的数量
    FixedSizePool(size_t blockSize, size_t blockCount) {
    }

    // TODO: 实现析构函数
    ~FixedSizePool() {
    }

    // TODO: 分配一个块
    void* allocate() {
        return nullptr;
    }

    // TODO: 释放一个块
    void deallocate(void* ptr) {
    }

    // TODO: 返回可用块数量
    size_t available() const {
        return 0;
    }

private:
    // TODO: 定义成员变量
    // 提示：需要存储内存起始地址、空闲链表头、块大小等
};

/**
 * 题目2：实现模板化内存池
 *
 * 要求：
 * 1. 支持任意类型 T
 * 2. 自动计算块大小
 * 3. 支持 construct 和 destroy
 */
template <typename T, size_t BlockCount = 32>
class ObjectPool {
public:
    // TODO: 实现构造函数
    ObjectPool() {
    }

    // TODO: 实现析构函数
    ~ObjectPool() {
    }

    // TODO: 分配并构造对象
    template <typename... Args>
    T* construct(Args&&... args) {
        return nullptr;
    }

    // TODO: 析构并释放对象
    void destroy(T* ptr) {
    }

private:
    // TODO: 定义成员变量
};

/**
 * 题目3：实现可增长内存池
 *
 * 要求：
 * 1. 初始分配一块内存
 * 2. 内存不足时自动扩展
 * 3. 管理多个内存块
 */
class GrowablePool {
public:
    // TODO: 实现可增长内存池
    GrowablePool(size_t blockSize, size_t initialBlocks = 32) {
    }

    ~GrowablePool() {
    }

    void* allocate() {
        return nullptr;
    }

    void deallocate(void* ptr) {
    }

private:
    // TODO: 定义成员变量
    // 提示：需要存储所有分配的大块内存，以便析构时释放
};

int main() {
    return 0;
}
