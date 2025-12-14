/**
 * @file custom_allocator_solution.cpp
 * @brief 自定义分配器 - 参考答案
 */

#include "custom_allocator.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <new>

namespace CustomAllocatorImpl {

namespace Solution {

// ==================== 日志分配器 ====================

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
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        T* ptr = static_cast<T*>(std::malloc(n * sizeof(T)));
        if (!ptr) {
            throw std::bad_alloc();
        }
        ++allocCount;
        return ptr;
    }

    void deallocate(T* p, std::size_t) noexcept {
        ++deallocCount;
        std::free(p);
    }

    template <typename U>
    bool operator==(const LoggingAllocator<U>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const LoggingAllocator<U>&) const noexcept { return false; }

    static void reset() {
        allocCount = 0;
        deallocCount = 0;
    }
};

template <typename T>
size_t LoggingAllocator<T>::allocCount = 0;

template <typename T>
size_t LoggingAllocator<T>::deallocCount = 0;

// ==================== 栈分配器（简化版） ====================

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
    StackAllocator(const StackAllocator<U, N>&) noexcept {}

    // 简化版：直接使用堆分配
    // 真正的栈分配器需要追踪缓冲区状态
    T* allocate(std::size_t n) {
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t) noexcept {
        std::free(p);
    }

    template <typename U>
    bool operator==(const StackAllocator<U, N>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const StackAllocator<U, N>&) const noexcept { return false; }
};

// ==================== 对齐分配器 ====================

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
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    T* allocate(std::size_t n) {
        std::size_t bytes = n * sizeof(T);

#if defined(_WIN32)
        void* ptr = _aligned_malloc(bytes, Alignment);
#else
        void* ptr = std::aligned_alloc(Alignment, bytes);
#endif

        if (!ptr) {
            throw std::bad_alloc();
        }

        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) noexcept {
#if defined(_WIN32)
        _aligned_free(p);
#else
        std::free(p);
#endif
    }

    template <typename U>
    bool operator==(const AlignedAllocator<U, Alignment>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const AlignedAllocator<U, Alignment>&) const noexcept { return false; }
};

// ==================== 共享内存分配器 ====================

template <typename T>
class SharedMemoryAllocator {
public:
    using value_type = T;

    explicit SharedMemoryAllocator(void* sharedMemory, std::size_t size)
        : memory_(static_cast<char*>(sharedMemory)),
          size_(size),
          used_(0) {}

    template <typename U>
    SharedMemoryAllocator(const SharedMemoryAllocator<U>& other) noexcept
        : memory_(other.memory_), size_(other.size_), used_(other.used_) {}

    T* allocate(std::size_t n) {
        // 计算对齐
        std::size_t currentAddr = reinterpret_cast<std::size_t>(memory_ + used_);
        std::size_t alignedAddr = (currentAddr + alignof(T) - 1) & ~(alignof(T) - 1);
        std::size_t padding = alignedAddr - currentAddr;

        std::size_t bytes = n * sizeof(T);
        if (used_ + padding + bytes > size_) {
            throw std::bad_alloc();
        }

        used_ += padding;
        T* ptr = reinterpret_cast<T*>(memory_ + used_);
        used_ += bytes;
        return ptr;
    }

    void deallocate(T*, std::size_t) noexcept {
        // Arena 风格，不支持单独释放
    }

    template <typename U>
    bool operator==(const SharedMemoryAllocator<U>& other) const noexcept {
        return memory_ == other.memory_;
    }

    template <typename U>
    bool operator!=(const SharedMemoryAllocator<U>& other) const noexcept {
        return memory_ != other.memory_;
    }

    template <typename U>
    friend class SharedMemoryAllocator;

private:
    char* memory_;
    std::size_t size_;
    std::size_t used_;
};

// ==================== 统计分配器 ====================

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

        void reset() {
            allocations = 0;
            deallocations = 0;
            bytesAllocated = 0;
            bytesFreed = 0;
            currentUsage = 0;
            peakUsage = 0;
        }
    };

    StatsAllocator() noexcept = default;

    template <typename U>
    StatsAllocator(const StatsAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        std::size_t bytes = n * sizeof(T);

        T* ptr = static_cast<T*>(std::malloc(bytes));
        if (!ptr) {
            throw std::bad_alloc();
        }

        stats_.allocations++;
        stats_.bytesAllocated += bytes;
        stats_.currentUsage += bytes;

        // 更新峰值（使用 CAS）
        size_t current = stats_.currentUsage.load();
        size_t peak = stats_.peakUsage.load();
        while (current > peak) {
            if (stats_.peakUsage.compare_exchange_weak(peak, current)) {
                break;
            }
        }

        return ptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        if (p) {
            std::size_t bytes = n * sizeof(T);
            stats_.deallocations++;
            stats_.bytesFreed += bytes;
            stats_.currentUsage -= bytes;
            std::free(p);
        }
    }

    template <typename U>
    bool operator==(const StatsAllocator<U>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const StatsAllocator<U>&) const noexcept { return false; }

    static Stats& getStats() { return stats_; }

private:
    static Stats stats_;
};

template <typename T>
typename StatsAllocator<T>::Stats StatsAllocator<T>::stats_;

} // namespace Solution

// ==================== 测试函数 ====================

void testCustomAllocatorSolution() {
    std::cout << "=== Custom Allocator Tests (Solution) ===" << std::endl;

    // 测试日志分配器
    Solution::LoggingAllocator<int>::reset();
    {
        std::vector<int, Solution::LoggingAllocator<int>> vec;
        vec.reserve(5);
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
    }
    assert(Solution::LoggingAllocator<int>::allocCount > 0);
    assert(Solution::LoggingAllocator<int>::deallocCount > 0);
    std::cout << "  LoggingAllocator: PASSED" << std::endl;

    // 测试栈分配器
    {
        std::vector<int, Solution::StackAllocator<int, 16>> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        assert(vec.size() == 3);
    }
    std::cout << "  StackAllocator: PASSED" << std::endl;

    // 测试对齐分配器
    {
        std::vector<double, Solution::AlignedAllocator<double, 64>> alignedVec;
        alignedVec.reserve(10);
        alignedVec.push_back(1.0);

        // 验证 64 字节对齐
        assert(reinterpret_cast<size_t>(alignedVec.data()) % 64 == 0);
    }
    std::cout << "  AlignedAllocator: PASSED" << std::endl;

    // 测试共享内存分配器
    {
        char sharedMem[1024];
        Solution::SharedMemoryAllocator<int> alloc(sharedMem, sizeof(sharedMem));

        std::vector<int, Solution::SharedMemoryAllocator<int>> vec(alloc);
        vec.reserve(10);
        for (int i = 0; i < 10; ++i) {
            vec.push_back(i);
        }
        assert(vec.size() == 10);
        assert(vec[5] == 5);

        // 验证分配在共享内存中
        char* dataPtr = reinterpret_cast<char*>(vec.data());
        assert(dataPtr >= sharedMem && dataPtr < sharedMem + sizeof(sharedMem));
    }
    std::cout << "  SharedMemoryAllocator: PASSED" << std::endl;

    // 测试统计分配器
    Solution::StatsAllocator<int>::getStats().reset();
    {
        std::vector<int, Solution::StatsAllocator<int>> vec;
        vec.reserve(100);
        for (int i = 0; i < 100; ++i) {
            vec.push_back(i);
        }
    }
    auto& stats = Solution::StatsAllocator<int>::getStats();
    assert(stats.allocations > 0);
    assert(stats.allocations == stats.deallocations);
    assert(stats.bytesAllocated == stats.bytesFreed);
    assert(stats.currentUsage == 0);
    std::cout << "  StatsAllocator: PASSED" << std::endl;
}

} // namespace CustomAllocatorImpl
