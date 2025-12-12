/**
 * @file custom_allocator.cpp
 * @brief 自定义分配器 - 面试题
 *
 * 实现符合 C++ 标准的自定义分配器
 */

#include <cstddef>
#include <memory>
#include <vector>
#include <map>

/**
 * 题目1：实现简单的日志分配器
 *
 * 要求：
 * 1. 符合 C++ allocator 要求
 * 2. 记录所有分配和释放操作
 * 3. 可用于 STL 容器
 */
template <typename T>
class LoggingAllocator {
public:
    using value_type = T;

    // TODO: 实现构造函数
    LoggingAllocator() noexcept {}

    template <typename U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    // TODO: 实现 allocate
    T* allocate(std::size_t n) {
        return nullptr;
    }

    // TODO: 实现 deallocate
    void deallocate(T* p, std::size_t n) noexcept {
    }

    // TODO: 实现比较运算符
    template <typename U>
    bool operator==(const LoggingAllocator<U>&) const noexcept {
        return true;
    }

    template <typename U>
    bool operator!=(const LoggingAllocator<U>&) const noexcept {
        return false;
    }
};

/**
 * 题目2：实现栈分配器
 *
 * 要求：
 * 1. 从栈上的固定缓冲区分配
 * 2. 超出缓冲区时回退到堆分配
 * 3. 适合小容器优化
 */
template <typename T, std::size_t N>
class StackAllocator {
public:
    using value_type = T;

    // TODO: 实现栈分配器
    StackAllocator() noexcept {}

    template <typename U>
    StackAllocator(const StackAllocator<U, N>&) noexcept {}

    T* allocate(std::size_t n) {
        return nullptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
    }

private:
    // TODO: 定义栈上缓冲区
    // alignas(T) char buffer_[N * sizeof(T)];
};

/**
 * 题目3：实现对齐分配器
 *
 * 要求：
 * 1. 支持自定义对齐要求
 * 2. 用于 SIMD 或缓存行对齐
 */
template <typename T, std::size_t Alignment>
class AlignedAllocator {
public:
    using value_type = T;

    // TODO: 实现对齐分配器
    AlignedAllocator() noexcept {}

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    T* allocate(std::size_t n) {
        return nullptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
    }
};

/**
 * 题目4：实现共享内存分配器
 *
 * 要求：
 * 1. 从预分配的共享内存区域分配
 * 2. 支持进程间共享
 */
template <typename T>
class SharedMemoryAllocator {
public:
    using value_type = T;

    // TODO: 实现共享内存分配器
    explicit SharedMemoryAllocator(void* sharedMemory, std::size_t size) {
    }

    T* allocate(std::size_t n) {
        return nullptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
    }
};

/**
 * 题目5：实现统计分配器
 *
 * 要求：
 * 1. 记录分配次数、释放次数
 * 2. 记录峰值内存使用
 * 3. 检测内存泄漏
 */
template <typename T>
class StatsAllocator {
public:
    using value_type = T;

    // TODO: 实现统计分配器
    StatsAllocator() noexcept {}

    T* allocate(std::size_t n) {
        return nullptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
    }

    // TODO: 返回统计信息
    struct Stats {
        size_t allocations;
        size_t deallocations;
        size_t bytesAllocated;
        size_t bytesFreed;
        size_t peakUsage;
    };

    static Stats getStats() {
        return {};
    }
};

int main() {
    return 0;
}
