/**
 * @file make_shared_solution.cpp
 * @brief make_shared/make_unique 实现 - 参考答案
 */

#include "make_shared.h"
#include <iostream>
#include <cassert>
#include <new>

namespace MakeSharedImpl {

namespace Solution {

// ==================== make_unique ====================

/**
 * 题目1: myMakeUnique - 实现std::make_unique
 * 时间复杂度: O(1) - 一次分配 + 构造
 * 空间复杂度: O(1) - 栈上创建unique_ptr
 */
template <typename T, typename... Args>
std::unique_ptr<T> myMakeUnique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * 题目2: myMakeUniqueArray - 数组版本make_unique
 * 时间复杂度: O(n) - 分配并默认初始化n个元素
 * 空间复杂度: O(1) - 栈上创建unique_ptr
 */
// 数组版本（未知大小）
template <typename T>
std::unique_ptr<T[]> myMakeUniqueArray(size_t size) {
    return std::unique_ptr<T[]>(new T[size]());
}

// ==================== CombinedBlock ====================

/**
 * 题目3: CombinedBlock - 合并分配块
 * 核心优化: 控制块与对象存储在连续内存中
 * 构造: O(1) - 原地构造
 * 空间: sizeof(控制块) + sizeof(T) - 一次分配
 */
template <typename T>
struct alignas(std::max_align_t) CombinedBlock {
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;
    alignas(T) unsigned char storage[sizeof(T)];

    template <typename... Args>
    explicit CombinedBlock(Args&&... args)
        : strong_count(1), weak_count(0) {
        new (storage) T(std::forward<Args>(args)...);
    }

    T* getPtr() noexcept {
        return reinterpret_cast<T*>(storage);
    }

    void destroyObject() {
        getPtr()->~T();
    }
};

// ==================== MySharedPtr ====================

/**
 * 题目4: MySharedPtr - 配合CombinedBlock的SharedPtr
 * 所有操作: O(1) - 原子计数操作
 * 空间复杂度: O(1) - 仅存储块指针
 * 优势: make_shared创建时只需一次内存分配
 */
template <typename T>
class MySharedPtr {
public:
    MySharedPtr() noexcept : block_(nullptr) {}

    // make_shared 专用构造
    explicit MySharedPtr(CombinedBlock<T>* block) noexcept : block_(block) {}

    ~MySharedPtr() {
        release();
    }

    MySharedPtr(const MySharedPtr& other) noexcept : block_(other.block_) {
        if (block_) {
            block_->strong_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    MySharedPtr& operator=(const MySharedPtr& other) noexcept {
        if (this != &other) {
            release();
            block_ = other.block_;
            if (block_) {
                block_->strong_count.fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    MySharedPtr(MySharedPtr&& other) noexcept : block_(other.block_) {
        other.block_ = nullptr;
    }

    MySharedPtr& operator=(MySharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            block_ = other.block_;
            other.block_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept {
        return block_ ? block_->getPtr() : nullptr;
    }

    long use_count() const noexcept {
        return block_ ? block_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    T& operator*() const { return *get(); }
    T* operator->() const noexcept { return get(); }
    explicit operator bool() const noexcept { return block_ != nullptr; }

private:
    void release() {
        if (block_) {
            if (block_->strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                block_->destroyObject();
                // 如果没有 weak_ptr，删除整个块
                if (block_->weak_count.load(std::memory_order_acquire) == 0) {
                    delete block_;
                }
            }
        }
        block_ = nullptr;
    }

    CombinedBlock<T>* block_;
};

// ==================== myMakeShared ====================

/**
 * 题目5: myMakeShared - 实现std::make_shared
 * 时间复杂度: O(1) - 单次分配 + 原地构造
 * 空间复杂度: O(1) - 栈上创建MySharedPtr
 * 优势: 相比shared_ptr(new T)减少一次分配，提高缓存局部性
 */
template <typename T, typename... Args>
MySharedPtr<T> myMakeShared(Args&&... args) {
    auto* block = new CombinedBlock<T>(std::forward<Args>(args)...);
    return MySharedPtr<T>(block);
}

// ==================== 测试类 ====================

struct TestClass {
    int value;
    std::string name;
    static int instanceCount;

    TestClass(int v, const std::string& n) : value(v), name(n) {
        ++instanceCount;
    }

    ~TestClass() {
        --instanceCount;
    }
};

int TestClass::instanceCount = 0;

} // namespace Solution

// ==================== 测试函数 ====================

void testMakeSharedSolution() {
    std::cout << "=== Make Shared Tests (Solution) ===" << std::endl;

    // 测试 myMakeUnique
    Solution::TestClass::instanceCount = 0;
    {
        auto p = Solution::myMakeUnique<Solution::TestClass>(42, std::string("unique"));
        assert(p->value == 42);
        assert(p->name == "unique");
        assert(Solution::TestClass::instanceCount == 1);
    }
    assert(Solution::TestClass::instanceCount == 0);
    std::cout << "  myMakeUnique: PASSED" << std::endl;

    // 测试 myMakeUniqueArray
    {
        auto arr = Solution::myMakeUniqueArray<int>(5);
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        assert(arr[0] == 0);
        assert(arr[4] == 40);
    }
    std::cout << "  myMakeUniqueArray: PASSED" << std::endl;

    // 测试 myMakeShared
    Solution::TestClass::instanceCount = 0;
    {
        auto p1 = Solution::myMakeShared<Solution::TestClass>(100, std::string("shared1"));
        assert(p1.use_count() == 1);
        assert(p1->value == 100);

        auto p2 = p1;
        assert(p1.use_count() == 2);

        p2 = Solution::myMakeShared<Solution::TestClass>(200, std::string("shared2"));
        assert(p1.use_count() == 1);
    }
    assert(Solution::TestClass::instanceCount == 0);
    std::cout << "  myMakeShared: PASSED" << std::endl;

    // 测试移动语义
    {
        auto p1 = Solution::myMakeShared<Solution::TestClass>(300, std::string("move"));
        auto p2 = std::move(p1);
        assert(!p1);
        assert(p2.use_count() == 1);
    }
    std::cout << "  myMakeShared move: PASSED" << std::endl;

    // 测试单次分配优势
    {
        // 使用 make_shared 时，控制块和对象只需一次分配
        // 这比 shared_ptr(new T) 的两次分配更高效
        auto p = Solution::myMakeShared<int>(42);
        assert(*p == 42);
    }
    std::cout << "  single allocation: PASSED" << std::endl;

    // 测试拷贝赋值
    {
        auto p1 = Solution::myMakeShared<int>(1);
        auto p2 = Solution::myMakeShared<int>(2);
        assert(p1.use_count() == 1);
        assert(p2.use_count() == 1);

        p1 = p2;
        assert(p1.use_count() == 2);
        assert(p2.use_count() == 2);
        assert(*p1 == 2);
    }
    std::cout << "  copy assignment: PASSED" << std::endl;
}

} // namespace MakeSharedImpl
