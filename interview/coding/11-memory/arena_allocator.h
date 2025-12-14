/**
 * @file arena_allocator.h
 * @brief Arena Allocator 实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 基础 Arena Allocator
 * 2. 可增长的 Arena
 * 3. 带析构器的 Arena
 * 4. STL 兼容的 Arena Allocator
 */

#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

namespace ArenaAllocatorImpl {

// ==================== 题目1: 基础 Arena ====================

/**
 * 题目1: 实现基础 Arena Allocator
 *
 * 原理：
 * - 线性分配（bump allocator）
 * - 移动指针即可完成分配，O(1)
 * - 不支持单独释放，只能整体重置
 * - 支持内存对齐
 *
 * 应用场景：
 * - 编译器/解析器的 AST 节点
 * - 请求处理（请求结束后整体释放）
 * - 游戏帧内临时分配
 *
 * 示例：
 *   Arena arena(4096);
 *   int* arr = static_cast<int*>(arena.allocate(sizeof(int) * 100));
 *   auto* node = arena.create<ASTNode>(args...);
 *   arena.reset();  // 一次性释放所有
 */
class Arena {
public:
    explicit Arena(size_t size);
    ~Arena();

    // 禁止拷贝
    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));

    template <typename T, typename... Args>
    T* create(Args&&... args);

    void reset();

    size_t used() const;
    size_t remaining() const;
    size_t capacity() const;

private:
    char* memory_;
    size_t size_;
    size_t used_;
};

// ==================== 题目2: 可增长的 Arena ====================

/**
 * 题目2: 实现可增长的 Arena
 *
 * 特点：
 * 1. 初始分配一块内存
 * 2. 空间不足时分配新块
 * 3. 重置时保留第一块，释放其他块
 *
 * 与基础 Arena 对比：
 * - 基础 Arena 空间固定，分配失败返回 nullptr
 * - 可增长 Arena 自动扩展，几乎不会失败
 */
class GrowableArena {
public:
    explicit GrowableArena(size_t initialSize = 4096);
    ~GrowableArena();

    // 禁止拷贝
    GrowableArena(const GrowableArena&) = delete;
    GrowableArena& operator=(const GrowableArena&) = delete;

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));

    template <typename T, typename... Args>
    T* create(Args&&... args);

    void reset();
    size_t totalAllocated() const;

private:
    struct Block {
        char* memory;
        size_t size;
        size_t used;
    };

    void addBlock(size_t size);
    void* tryAllocate(size_t size, size_t alignment);

    std::vector<Block> blocks_;
    size_t defaultSize_;
};

// ==================== 题目3: 带析构器的 Arena ====================

/**
 * 题目3: 实现带析构器的 Arena
 *
 * 问题：
 * - 基础 Arena 不调用析构函数
 * - 如果对象持有资源（如 std::string），会泄漏
 *
 * 解决方案：
 * - 记录需要析构的对象
 * - reset 时逆序调用析构函数
 *
 * 优化：
 * - 只记录非平凡析构的对象
 * - 平凡析构类型（如 int）不需要记录
 */
class ArenaWithDestructors {
public:
    explicit ArenaWithDestructors(size_t size);
    ~ArenaWithDestructors();

    template <typename T, typename... Args>
    T* create(Args&&... args);

    void reset();
    size_t used() const;

private:
    struct Destructor {
        void (*destroy)(void*);
        void* ptr;
    };

    void callDestructors();

    Arena arena_;
    std::vector<Destructor> destructors_;
};

// ==================== 题目4: STL Arena Allocator ====================

/**
 * 题目4: 实现符合 STL 标准的 Arena Allocator
 *
 * 作用：
 * - 可用于 STL 容器（vector, list, map 等）
 * - 容器的内存分配由 Arena 管理
 *
 * 注意：
 * - deallocate 是空操作（Arena 不支持单独释放）
 * - 容器销毁时只调用析构函数，不释放内存
 * - 内存由 Arena 统一管理
 */
template <typename T>
class ArenaAllocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ArenaAllocator(Arena& arena) noexcept;

    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept;

    T* allocate(size_t n);
    void deallocate(T* p, size_t n) noexcept;

    template <typename U>
    bool operator==(const ArenaAllocator<U>& other) const noexcept;

    template <typename U>
    bool operator!=(const ArenaAllocator<U>& other) const noexcept;

    template <typename U>
    friend class ArenaAllocator;

private:
    Arena* arena_;
};

// ==================== 测试辅助类 ====================

struct TestObject {
    int id;
    std::string name;
    static int instanceCount;

    TestObject(int i, const char* n);
    ~TestObject();
};

// ==================== 测试函数声明 ====================

void testArenaAllocator();          // 测试面试者实现
void testArenaAllocatorSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testArenaAllocator();
    testArenaAllocatorSolution();
}

} // namespace ArenaAllocatorImpl

#endif // ARENA_ALLOCATOR_H
