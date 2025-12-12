/**
 * @file arena_allocator_solution.cpp
 * @brief Arena Allocator 实现 - 解答
 */

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <new>
#include <utility>

/**
 * 题目1：实现基础 Arena Allocator
 *
 * 原理：
 * - 线性分配，移动指针即可
 * - O(1) 分配，无释放操作
 * - 适合短生命周期、大量小对象
 */
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

/**
 * 题目2：实现可增长的 Arena
 *
 * 空间不足时分配新的内存块
 */
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
        // 尝试在当前块分配
        void* result = tryAllocate(size, alignment);
        if (result) return result;

        // 当前块不够，创建新块
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
        // 保留第一块，释放其他块
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

/**
 * 题目3：实现带析构器的 Arena
 *
 * 记录需要析构的对象，reset 时调用析构函数
 */
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

        // 如果 T 有非平凡析构函数，记录它
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
        // 逆序调用析构函数
        for (auto it = destructors_.rbegin(); it != destructors_.rend(); ++it) {
            it->destroy(it->ptr);
        }
        destructors_.clear();
    }

    Arena arena_;
    std::vector<Destructor> destructors_;
};

/**
 * 题目4：实现符合 STL 标准的 Arena Allocator
 */
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

    void deallocate(T* p, size_t n) noexcept {
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

// 测试用结构
struct TestObject {
    int id;
    std::string name;

    TestObject(int i, const char* n) : id(i), name(n) {
        std::cout << "TestObject(" << id << ", " << name << ") constructed\n";
    }

    ~TestObject() {
        std::cout << "TestObject(" << id << ", " << name << ") destroyed\n";
    }
};

int main() {
    std::cout << "=== 测试基础 Arena ===\n";
    {
        Arena arena(1024);

        int* arr = static_cast<int*>(arena.allocate(sizeof(int) * 10));
        for (int i = 0; i < 10; ++i) {
            arr[i] = i * 10;
        }

        std::cout << "Used: " << arena.used() << ", Remaining: " << arena.remaining() << "\n";

        double* d = static_cast<double*>(arena.allocate(sizeof(double), alignof(double)));
        *d = 3.14159;
        std::cout << "Double value: " << *d << "\n";

        arena.reset();
        std::cout << "After reset, used: " << arena.used() << "\n";
    }

    std::cout << "\n=== 测试可增长 Arena ===\n";
    {
        GrowableArena arena(64);  // 小初始大小

        for (int i = 0; i < 100; ++i) {
            int* p = arena.create<int>(i);
        }

        std::cout << "Total allocated: " << arena.totalAllocated() << " bytes\n";

        arena.reset();
        std::cout << "After reset, total: " << arena.totalAllocated() << " bytes\n";
    }

    std::cout << "\n=== 测试带析构器的 Arena ===\n";
    {
        ArenaWithDestructors arena(4096);

        TestObject* obj1 = arena.create<TestObject>(1, "first");
        TestObject* obj2 = arena.create<TestObject>(2, "second");
        TestObject* obj3 = arena.create<TestObject>(3, "third");

        std::cout << "Objects created, used: " << arena.used() << "\n";

        std::cout << "Calling reset...\n";
        arena.reset();  // 会调用所有析构函数
        std::cout << "Reset done\n";
    }

    std::cout << "\n=== 测试 STL Arena Allocator ===\n";
    {
        Arena arena(4096);
        ArenaAllocator<int> alloc(arena);

        std::vector<int, ArenaAllocator<int>> vec(alloc);
        vec.reserve(100);

        for (int i = 0; i < 100; ++i) {
            vec.push_back(i);
        }

        std::cout << "Vector size: " << vec.size() << "\n";
        std::cout << "Arena used: " << arena.used() << "\n";

        // 注意：vector 析构时会调用 deallocate，但 Arena 忽略它
        // 整个 arena 会在作用域结束时释放
    }

    return 0;
}
