/**
 * @file weak_ptr.cpp
 * @brief weak_ptr 实现 - 面试题
 *
 * 实现一个简化版的 weak_ptr，解决循环引用问题
 */

#include <iostream>
#include <atomic>

// 前向声明
template <typename T> class SharedPtr;
template <typename T> class WeakPtr;

/**
 * 控制块结构（支持 weak_ptr）
 */
template <typename T>
struct ControlBlock {
    T* ptr;
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;

    explicit ControlBlock(T* p) : ptr(p), strong_count(1), weak_count(0) {}
};

/**
 * 题目1：实现 WeakPtr
 *
 * 要求：
 * 1. 不增加强引用计数
 * 2. 只增加弱引用计数
 * 3. 支持 lock() 返回 SharedPtr
 * 4. 支持 expired() 检查是否过期
 */
template <typename T>
class WeakPtr {
public:
    // TODO: 实现默认构造
    WeakPtr() noexcept {
    }

    // TODO: 从 SharedPtr 构造
    WeakPtr(const SharedPtr<T>& shared) {
    }

    // TODO: 实现析构函数
    ~WeakPtr() {
    }

    // TODO: 实现拷贝构造
    WeakPtr(const WeakPtr& other) {
    }

    // TODO: 实现拷贝赋值
    WeakPtr& operator=(const WeakPtr& other) {
        return *this;
    }

    // TODO: 实现从 SharedPtr 赋值
    WeakPtr& operator=(const SharedPtr<T>& shared) {
        return *this;
    }

    // TODO: 实现 expired()
    bool expired() const noexcept {
        return true;
    }

    // TODO: 实现 lock()
    SharedPtr<T> lock() const noexcept {
        return SharedPtr<T>();
    }

    // TODO: 实现 use_count()
    long use_count() const noexcept {
        return 0;
    }

    // TODO: 实现 reset()
    void reset() noexcept {
    }

private:
    ControlBlock<T>* cb_ = nullptr;
};

/**
 * 题目2：解决循环引用问题
 *
 * 场景：两个对象相互引用
 * 要求：使用 weak_ptr 打破循环
 */
struct Node {
    int value;
    // 使用 weak_ptr 避免循环引用
    // TODO: 定义正确的指针类型
    // SharedPtr<Node> next;  // 这会导致循环引用
    // WeakPtr<Node> next;    // 应该使用 weak_ptr

    Node(int v) : value(v) {}
    ~Node() {
        std::cout << "Node(" << value << ") destroyed\n";
    }
};

/**
 * 题目3：实现 enable_shared_from_this
 *
 * 要求：
 * 1. 允许从 this 指针获取 shared_ptr
 * 2. 避免创建多个独立的引用计数
 */
template <typename T>
class EnableSharedFromThis {
public:
    // TODO: 实现 shared_from_this()
    SharedPtr<T> shared_from_this() {
        return SharedPtr<T>();
    }

    // TODO: 实现 weak_from_this()
    WeakPtr<T> weak_from_this() {
        return weak_this_;
    }

protected:
    EnableSharedFromThis() = default;
    ~EnableSharedFromThis() = default;

private:
    template <typename U> friend class SharedPtr;
    WeakPtr<T> weak_this_;
};

int main() {
    return 0;
}
