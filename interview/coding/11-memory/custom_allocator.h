/**
 * @file custom_allocator.h
 * @brief 自定义分配器 - 题目说明与声明
 *
 * 包含题目：
 * 1. 日志分配器
 * 2. 栈分配器
 * 3. 对齐分配器
 * 4. 共享内存分配器
 * 5. 统计分配器
 */

#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <atomic>

namespace CustomAllocatorImpl {

// ==================== 题目1: 日志分配器 ====================

/**
 * 题目1: 实现日志分配器
 *
 * 特点：
 * 1. 符合 C++ allocator 要求
 * 2. 记录所有分配和释放操作
 * 3. 可用于 STL 容器
 *
 * C++11 最小 allocator 要求：
 * - value_type 类型别名
 * - allocate(n) 分配 n 个对象的内存
 * - deallocate(p, n) 释放内存
 * - 构造函数（包括从其他类型的 allocator 构造）
 * - operator== 和 operator!=
 */
template <typename T>
class LoggingAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static size_t allocCount;
    static size_t deallocCount;

    LoggingAllocator() noexcept = default;

    template <typename U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n) noexcept;

    template <typename U>
    bool operator==(const LoggingAllocator<U>&) const noexcept;

    template <typename U>
    bool operator!=(const LoggingAllocator<U>&) const noexcept;

    static void reset();
};

// ==================== 题目2: 栈分配器 ====================

/**
 * 题目2: 实现栈分配器
 *
 * 特点：
 * 1. 从栈上的固定缓冲区分配
 * 2. 超出缓冲区时回退到堆分配
 * 3. 适合小容器优化（Small Buffer Optimization）
 *
 * 注意：
 * - STL 容器可能在构造时就需要分配内存
 * - rebind 机制用于容器内部节点的分配
 */
template <typename T, std::size_t N>
class StackAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    StackAllocator() noexcept = default;

    template <typename U>
    StackAllocator(const StackAllocator<U, N>&) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n) noexcept;

    template <typename U>
    bool operator==(const StackAllocator<U, N>&) const noexcept;

    template <typename U>
    bool operator!=(const StackAllocator<U, N>&) const noexcept;
};

// ==================== 题目3: 对齐分配器 ====================

/**
 * 题目3: 实现对齐分配器
 *
 * 应用场景：
 * 1. SIMD 操作需要 16/32/64 字节对齐
 * 2. 避免缓存行假共享（false sharing）需要 64 字节对齐
 *
 * 实现方法：
 * - Windows: _aligned_malloc / _aligned_free
 * - POSIX: aligned_alloc / free（或 posix_memalign）
 */
template <typename T, std::size_t Alignment = alignof(T)>
class AlignedAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };

    static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be power of 2");

    AlignedAllocator() noexcept = default;

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n) noexcept;

    template <typename U>
    bool operator==(const AlignedAllocator<U, Alignment>&) const noexcept;

    template <typename U>
    bool operator!=(const AlignedAllocator<U, Alignment>&) const noexcept;
};

// ==================== 题目4: 共享内存分配器 ====================

/**
 * 题目4: 实现共享内存分配器
 *
 * 特点：
 * 1. 从预分配的内存区域分配
 * 2. 支持进程间共享
 *
 * 应用场景：
 * - 进程间通信（IPC）
 * - 内存映射文件
 */
template <typename T>
class SharedMemoryAllocator {
public:
    using value_type = T;

    explicit SharedMemoryAllocator(void* sharedMemory, std::size_t size);

    template <typename U>
    SharedMemoryAllocator(const SharedMemoryAllocator<U>& other) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n) noexcept;

    template <typename U>
    bool operator==(const SharedMemoryAllocator<U>& other) const noexcept;

    template <typename U>
    bool operator!=(const SharedMemoryAllocator<U>& other) const noexcept;

    template <typename U>
    friend class SharedMemoryAllocator;

private:
    char* memory_;
    std::size_t size_;
    std::size_t used_;
};

// ==================== 题目5: 统计分配器 ====================

/**
 * 题目5: 实现统计分配器
 *
 * 统计内容：
 * 1. 分配次数和释放次数
 * 2. 分配和释放的字节数
 * 3. 当前使用量
 * 4. 峰值使用量
 *
 * 应用场景：
 * - 内存泄漏检测
 * - 性能分析
 */
template <typename T>
class StatsAllocator {
public:
    using value_type = T;

    struct Stats {
        std::atomic<size_t> allocations{0};
        std::atomic<size_t> deallocations{0};
        std::atomic<size_t> bytesAllocated{0};
        std::atomic<size_t> bytesFreed{0};
        std::atomic<size_t> currentUsage{0};
        std::atomic<size_t> peakUsage{0};

        void reset();
    };

    StatsAllocator() noexcept = default;

    template <typename U>
    StatsAllocator(const StatsAllocator<U>&) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n) noexcept;

    template <typename U>
    bool operator==(const StatsAllocator<U>&) const noexcept;

    template <typename U>
    bool operator!=(const StatsAllocator<U>&) const noexcept;

    static Stats& getStats();

private:
    static Stats stats_;
};

// ==================== 测试函数声明 ====================

void testCustomAllocator();          // 测试面试者实现
void testCustomAllocatorSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testCustomAllocator();
    testCustomAllocatorSolution();
}

} // namespace CustomAllocatorImpl

#endif // CUSTOM_ALLOCATOR_H
