/**
 * @file arena_allocator.cpp
 * @brief Arena Allocator 实现 - 面试题
 *
 * Arena（竞技场）分配器：批量分配，整体释放
 * 适用场景：解析器、编译器、请求处理等短生命周期场景
 */

#include <cstddef>
#include <cstdint>

/**
 * 题目1：实现基础 Arena Allocator
 *
 * 要求：
 * 1. 线性分配（bump allocator）
 * 2. 不支持单独释放，只能整体重置
 * 3. 支持内存对齐
 */
class Arena {
public:
    // TODO: 实现构造函数
    explicit Arena(size_t size) {
    }

    // TODO: 实现析构函数
    ~Arena() {
    }

    // TODO: 分配内存（带对齐）
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        return nullptr;
    }

    // TODO: 分配并构造对象
    template <typename T, typename... Args>
    T* create(Args&&... args) {
        return nullptr;
    }

    // TODO: 重置 Arena（释放所有内存但保留缓冲区）
    void reset() {
    }

    // TODO: 返回已使用大小
    size_t used() const {
        return 0;
    }

    // TODO: 返回剩余大小
    size_t remaining() const {
        return 0;
    }

private:
    // TODO: 定义成员变量
    // 提示：需要存储内存起始地址、当前位置、总大小
};

/**
 * 题目2：实现可增长的 Arena
 *
 * 要求：
 * 1. 初始分配一块内存
 * 2. 空间不足时分配新块
 * 3. 重置时保留第一块，释放其他块
 */
class GrowableArena {
public:
    // TODO: 实现可增长 Arena
    explicit GrowableArena(size_t initialSize = 4096) {
    }

    ~GrowableArena() {
    }

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        return nullptr;
    }

    void reset() {
    }

private:
    // TODO: 定义成员变量
    // 提示：需要管理多个内存块
};

/**
 * 题目3：实现带析构器的 Arena
 *
 * 要求：
 * 1. 记录需要析构的对象
 * 2. reset 时调用所有析构函数
 */
class ArenaWithDestructors {
public:
    // TODO: 实现带析构器的 Arena
    explicit ArenaWithDestructors(size_t size) {
    }

    ~ArenaWithDestructors() {
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        return nullptr;
    }

    void reset() {
    }

private:
    // 析构函数包装器
    struct Destructor {
        void (*destroy)(void*);
        void* ptr;
    };

    // TODO: 定义成员变量
};

/**
 * 题目4：实现符合 STL 标准的 Arena Allocator
 *
 * 要求：
 * 1. 实现 allocator 接口
 * 2. 可用于 STL 容器
 */
template <typename T>
class ArenaAllocator {
public:
    using value_type = T;

    // TODO: 实现 STL allocator 接口
    explicit ArenaAllocator(Arena& arena) {
    }

    T* allocate(size_t n) {
        return nullptr;
    }

    void deallocate(T* p, size_t n) {
        // Arena 不支持单独释放
    }

private:
    // TODO: 定义成员变量
};

int main() {
    return 0;
}
