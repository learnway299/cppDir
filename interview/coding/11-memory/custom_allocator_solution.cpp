/**
 * @file custom_allocator_solution.cpp
 * @brief 自定义分配器 - 解答
 */

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <new>
#include <atomic>

/**
 * 题目1：实现简单的日志分配器
 *
 * 记录所有分配和释放操作
 */
template <typename T>
class LoggingAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    LoggingAllocator() noexcept = default;

    template <typename U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        std::cout << "[Allocator] Allocating " << n << " objects of "
                  << typeid(T).name() << " (" << n * sizeof(T) << " bytes)\n";

        T* ptr = static_cast<T*>(std::malloc(n * sizeof(T)));
        if (!ptr) {
            throw std::bad_alloc();
        }

        std::cout << "[Allocator] Address: " << ptr << "\n";
        return ptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        std::cout << "[Allocator] Deallocating " << n << " objects at "
                  << p << " (" << n * sizeof(T) << " bytes)\n";
        std::free(p);
    }

    template <typename U>
    bool operator==(const LoggingAllocator<U>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const LoggingAllocator<U>&) const noexcept { return false; }
};

/**
 * 题目2：实现栈分配器
 *
 * 小对象从栈缓冲区分配，大对象回退到堆
 */
template <typename T, std::size_t N>
class StackAllocator {
public:
    using value_type = T;

    StackAllocator() noexcept : used_(0) {}

    template <typename U>
    StackAllocator(const StackAllocator<U, N>&) noexcept : used_(0) {}

    T* allocate(std::size_t n) {
        std::size_t bytes = n * sizeof(T);

        // 尝试从栈缓冲区分配
        if (used_ + bytes <= sizeof(buffer_)) {
            T* ptr = reinterpret_cast<T*>(buffer_ + used_);
            used_ += bytes;
            std::cout << "[StackAlloc] Stack allocation: " << n << " objects\n";
            return ptr;
        }

        // 回退到堆分配
        std::cout << "[StackAlloc] Heap fallback: " << n << " objects\n";
        return static_cast<T*>(std::malloc(bytes));
    }

    void deallocate(T* p, std::size_t n) noexcept {
        // 检查是否是栈分配
        char* ptr = reinterpret_cast<char*>(p);
        if (ptr >= buffer_ && ptr < buffer_ + sizeof(buffer_)) {
            // 栈分配，简单方案不回收（实际应用需更复杂处理）
            std::cout << "[StackAlloc] Stack deallocation (no-op)\n";
            return;
        }

        // 堆分配
        std::cout << "[StackAlloc] Heap deallocation\n";
        std::free(p);
    }

    template <typename U>
    bool operator==(const StackAllocator<U, N>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const StackAllocator<U, N>&) const noexcept { return false; }

private:
    alignas(T) char buffer_[N * sizeof(T)];
    std::size_t used_;
};

/**
 * 题目3：实现对齐分配器
 *
 * 支持自定义对齐要求（如64字节缓存行对齐）
 */
template <typename T, std::size_t Alignment = alignof(T)>
class AlignedAllocator {
public:
    using value_type = T;

    static_assert(Alignment >= alignof(T), "Alignment must be >= alignof(T)");
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

        std::cout << "[AlignedAlloc] Allocated " << bytes << " bytes at "
                  << ptr << " (alignment: " << Alignment << ")\n";

        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t n) noexcept {
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

/**
 * 题目4：实现共享内存分配器（简化版）
 *
 * 从预分配的内存区域分配
 */
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
        // 对齐
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

    void deallocate(T* p, std::size_t n) noexcept {
        // 简单实现不支持释放
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

/**
 * 题目5：实现统计分配器
 *
 * 记录分配统计信息
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

        void reset() {
            allocations = 0;
            deallocations = 0;
            bytesAllocated = 0;
            bytesFreed = 0;
            currentUsage = 0;
            peakUsage = 0;
        }

        void print() const {
            std::cout << "=== Allocator Stats ===\n"
                      << "Allocations: " << allocations << "\n"
                      << "Deallocations: " << deallocations << "\n"
                      << "Bytes allocated: " << bytesAllocated << "\n"
                      << "Bytes freed: " << bytesFreed << "\n"
                      << "Current usage: " << currentUsage << "\n"
                      << "Peak usage: " << peakUsage << "\n";

            if (allocations != deallocations) {
                std::cout << "WARNING: Possible memory leak!\n";
            }
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

        // 更新峰值
        size_t current = stats_.currentUsage.load();
        size_t peak = stats_.peakUsage.load();
        while (current > peak) {
            stats_.peakUsage.compare_exchange_weak(peak, current);
            peak = stats_.peakUsage.load();
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

int main() {
    std::cout << "=== 测试日志分配器 ===\n";
    {
        std::vector<int, LoggingAllocator<int>> vec;
        vec.reserve(5);
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
    }

    std::cout << "\n=== 测试栈分配器 ===\n";
    {
        // 小数组使用栈缓冲区
        std::vector<int, StackAllocator<int, 16>> smallVec;
        smallVec.push_back(1);
        smallVec.push_back(2);

        // 大数组回退到堆
        std::vector<int, StackAllocator<int, 4>> largeVec;
        largeVec.reserve(100);
    }

    std::cout << "\n=== 测试对齐分配器 ===\n";
    {
        // 64 字节对齐（缓存行）
        std::vector<double, AlignedAllocator<double, 64>> alignedVec;
        alignedVec.reserve(10);
        alignedVec.push_back(1.0);

        std::cout << "Address: " << alignedVec.data()
                  << " (aligned to 64: " << (reinterpret_cast<size_t>(alignedVec.data()) % 64 == 0)
                  << ")\n";
    }

    std::cout << "\n=== 测试统计分配器 ===\n";
    {
        StatsAllocator<int>::getStats().reset();

        std::vector<int, StatsAllocator<int>> vec1;
        vec1.reserve(100);
        for (int i = 0; i < 100; ++i) {
            vec1.push_back(i);
        }

        std::vector<int, StatsAllocator<int>> vec2;
        vec2.reserve(50);

        vec2.clear();
        vec2.shrink_to_fit();

        StatsAllocator<int>::getStats().print();
    }

    std::cout << "\nAfter all vectors destroyed:\n";
    StatsAllocator<int>::getStats().print();

    return 0;
}
