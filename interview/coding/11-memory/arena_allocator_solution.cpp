/**
 * @file arena_allocator_solution.cpp
 * @brief Arena Allocator 实现 - 参考答案
 */

#include "arena_allocator.h"
#include <iostream>
#include <cassert>
#include <new>
#include <utility>
#include <algorithm>
#include <type_traits>

namespace ArenaAllocatorImpl {

namespace Solution {

// ==================== 基础 Arena ====================

class Arena {
public:
    explicit Arena(size_t size)
        : size_(size), used_(0) {
        memory_ = new char[size];
    }

    ~Arena() {
        delete[] memory_;
    }

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        // 对齐当前位置
        size_t currentAddr = reinterpret_cast<size_t>(memory_ + used_);
        size_t alignedAddr = (currentAddr + alignment - 1) & ~(alignment - 1);
        size_t padding = alignedAddr - currentAddr;

        if (used_ + padding + size > size_) {
            return nullptr;  // 空间不足
        }

        used_ += padding;
        void* result = memory_ + used_;
        used_ += size;
        return result;
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* mem = allocate(sizeof(T), alignof(T));
        if (!mem) return nullptr;
        return new (mem) T(std::forward<Args>(args)...);
    }

    void reset() {
        used_ = 0;
    }

    size_t used() const { return used_; }
    size_t remaining() const { return size_ - used_; }
    size_t capacity() const { return size_; }

private:
    char* memory_;
    size_t size_;
    size_t used_;
};

// ==================== 可增长的 Arena ====================

class GrowableArena {
public:
    explicit GrowableArena(size_t initialSize = 4096)
        : defaultSize_(initialSize) {
        addBlock(initialSize);
    }

    ~GrowableArena() {
        for (auto& block : blocks_) {
            delete[] block.memory;
        }
    }

    GrowableArena(const GrowableArena&) = delete;
    GrowableArena& operator=(const GrowableArena&) = delete;

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        void* result = tryAllocate(size, alignment);
        if (result) return result;

        size_t newBlockSize = std::max(defaultSize_, size + alignment);
        addBlock(newBlockSize);

        return tryAllocate(size, alignment);
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* mem = allocate(sizeof(T), alignof(T));
        if (!mem) return nullptr;
        return new (mem) T(std::forward<Args>(args)...);
    }

    void reset() {
        while (blocks_.size() > 1) {
            delete[] blocks_.back().memory;
            blocks_.pop_back();
        }
        if (!blocks_.empty()) {
            blocks_[0].used = 0;
        }
    }

    size_t totalAllocated() const {
        size_t total = 0;
        for (auto& block : blocks_) {
            total += block.size;
        }
        return total;
    }

private:
    struct Block {
        char* memory;
        size_t size;
        size_t used;
    };

    void addBlock(size_t size) {
        Block block;
        block.memory = new char[size];
        block.size = size;
        block.used = 0;
        blocks_.push_back(block);
    }

    void* tryAllocate(size_t size, size_t alignment) {
        if (blocks_.empty()) return nullptr;

        Block& current = blocks_.back();
        size_t currentAddr = reinterpret_cast<size_t>(current.memory + current.used);
        size_t alignedAddr = (currentAddr + alignment - 1) & ~(alignment - 1);
        size_t padding = alignedAddr - currentAddr;

        if (current.used + padding + size > current.size) {
            return nullptr;
        }

        current.used += padding;
        void* result = current.memory + current.used;
        current.used += size;
        return result;
    }

    std::vector<Block> blocks_;
    size_t defaultSize_;
};

// ==================== 带析构器的 Arena ====================

class ArenaWithDestructors {
public:
    explicit ArenaWithDestructors(size_t size)
        : arena_(size) {}

    ~ArenaWithDestructors() {
        callDestructors();
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* mem = arena_.allocate(sizeof(T), alignof(T));
        if (!mem) return nullptr;

        T* obj = new (mem) T(std::forward<Args>(args)...);

        if (!std::is_trivially_destructible<T>::value) {
            destructors_.push_back({
                [](void* p) { static_cast<T*>(p)->~T(); },
                obj
            });
        }

        return obj;
    }

    void reset() {
        callDestructors();
        arena_.reset();
    }

    size_t used() const { return arena_.used(); }

private:
    struct Destructor {
        void (*destroy)(void*);
        void* ptr;
    };

    void callDestructors() {
        for (auto it = destructors_.rbegin(); it != destructors_.rend(); ++it) {
            it->destroy(it->ptr);
        }
        destructors_.clear();
    }

    Arena arena_;
    std::vector<Destructor> destructors_;
};

// ==================== STL Arena Allocator ====================

template <typename T>
class ArenaAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ArenaAllocator(Arena& arena) noexcept : arena_(&arena) {}

    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept
        : arena_(other.arena_) {}

    T* allocate(size_t n) {
        void* mem = arena_->allocate(n * sizeof(T), alignof(T));
        if (!mem) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(mem);
    }

    void deallocate(T*, size_t) noexcept {
        // Arena 不支持单独释放
    }

    template <typename U>
    bool operator==(const ArenaAllocator<U>& other) const noexcept {
        return arena_ == other.arena_;
    }

    template <typename U>
    bool operator!=(const ArenaAllocator<U>& other) const noexcept {
        return arena_ != other.arena_;
    }

    template <typename U>
    friend class ArenaAllocator;

private:
    Arena* arena_;
};

// ==================== 测试对象 ====================

struct TestObject {
    int id;
    std::string name;
    static int instanceCount;

    TestObject(int i, const char* n) : id(i), name(n) {
        ++instanceCount;
    }

    ~TestObject() {
        --instanceCount;
    }
};

int TestObject::instanceCount = 0;

} // namespace Solution

// ==================== 测试函数 ====================

void testArenaAllocatorSolution() {
    std::cout << "=== Arena Allocator Tests (Solution) ===" << std::endl;

    // 测试基础 Arena
    {
        Solution::Arena arena(1024);

        int* arr = static_cast<int*>(arena.allocate(sizeof(int) * 10));
        for (int i = 0; i < 10; ++i) {
            arr[i] = i * 10;
        }
        assert(arr[5] == 50);

        double* d = static_cast<double*>(arena.allocate(sizeof(double), alignof(double)));
        *d = 3.14159;
        assert(*d > 3.14 && *d < 3.15);

        arena.reset();
        assert(arena.used() == 0);
    }
    std::cout << "  Arena basic: PASSED" << std::endl;

    // 测试可增长 Arena
    {
        Solution::GrowableArena arena(64);  // 小初始大小

        for (int i = 0; i < 100; ++i) {
            int* p = arena.create<int>(i);
            assert(*p == i);
        }

        assert(arena.totalAllocated() > 64);  // 应该有多个块

        arena.reset();
    }
    std::cout << "  GrowableArena: PASSED" << std::endl;

    // 测试带析构器的 Arena
    Solution::TestObject::instanceCount = 0;
    {
        Solution::ArenaWithDestructors arena(4096);

        Solution::TestObject* obj1 = arena.create<Solution::TestObject>(1, "first");
        Solution::TestObject* obj2 = arena.create<Solution::TestObject>(2, "second");
        Solution::TestObject* obj3 = arena.create<Solution::TestObject>(3, "third");

        assert(Solution::TestObject::instanceCount == 3);
        assert(obj1->id == 1);
        assert(obj2->name == "second");

        arena.reset();
        assert(Solution::TestObject::instanceCount == 0);
    }
    std::cout << "  ArenaWithDestructors: PASSED" << std::endl;

    // 测试 STL Arena Allocator
    {
        Solution::Arena arena(4096);
        Solution::ArenaAllocator<int> alloc(arena);

        std::vector<int, Solution::ArenaAllocator<int>> vec(alloc);
        vec.reserve(100);

        for (int i = 0; i < 100; ++i) {
            vec.push_back(i);
        }

        assert(vec.size() == 100);
        assert(vec[50] == 50);
        assert(arena.used() > 0);
    }
    std::cout << "  ArenaAllocator (STL): PASSED" << std::endl;

    // 测试对齐
    {
        Solution::Arena arena(256);

        char* c = static_cast<char*>(arena.allocate(1));
        double* d = static_cast<double*>(arena.allocate(sizeof(double), alignof(double)));

        // 验证对齐
        assert(reinterpret_cast<uintptr_t>(d) % alignof(double) == 0);
        (void)c;
    }
    std::cout << "  Arena alignment: PASSED" << std::endl;
}

} // namespace ArenaAllocatorImpl
