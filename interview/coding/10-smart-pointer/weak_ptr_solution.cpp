/**
 * @file weak_ptr_solution.cpp
 * @brief weak_ptr 实现 - 解答
 */

#include <iostream>
#include <atomic>
#include <utility>

// 控制块
template <typename T>
struct ControlBlock {
    T* ptr;
    std::atomic<long> strong_count;
    std::atomic<long> weak_count;

    explicit ControlBlock(T* p) : ptr(p), strong_count(1), weak_count(0) {}

    void add_strong() {
        strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    void add_weak() {
        weak_count.fetch_add(1, std::memory_order_relaxed);
    }

    // 返回是否需要删除控制块
    bool release_strong() {
        if (strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr;
            ptr = nullptr;
            if (weak_count.load(std::memory_order_acquire) == 0) {
                return true;
            }
        }
        return false;
    }

    bool release_weak() {
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (strong_count.load(std::memory_order_acquire) == 0) {
                return true;
            }
        }
        return false;
    }

    // 尝试增加强引用（用于 weak_ptr::lock）
    bool try_add_strong() {
        long count = strong_count.load(std::memory_order_relaxed);
        while (count > 0) {
            if (strong_count.compare_exchange_weak(
                    count, count + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed)) {
                return true;
            }
        }
        return false;
    }
};

// 前向声明
template <typename T> class WeakPtr;

/**
 * SharedPtr（支持 WeakPtr）
 */
template <typename T>
class SharedPtr {
public:
    friend class WeakPtr<T>;

    SharedPtr() noexcept : cb_(nullptr) {}

    explicit SharedPtr(T* ptr) : cb_(ptr ? new ControlBlock<T>(ptr) : nullptr) {}

    // 从 WeakPtr 构造（私有，由 lock 调用）
    SharedPtr(ControlBlock<T>* cb) : cb_(cb) {}

    ~SharedPtr() {
        if (cb_ && cb_->release_strong()) {
            delete cb_;
        }
    }

    SharedPtr(const SharedPtr& other) : cb_(other.cb_) {
        if (cb_) cb_->add_strong();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) delete cb_;
            cb_ = other.cb_;
            if (cb_) cb_->add_strong();
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) noexcept : cb_(other.cb_) {
        other.cb_ = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) delete cb_;
            cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return cb_ ? cb_->ptr : nullptr; }
    long use_count() const noexcept {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    T& operator*() const { return *get(); }
    T* operator->() const noexcept { return get(); }
    explicit operator bool() const noexcept { return get() != nullptr; }

private:
    ControlBlock<T>* cb_;
};

/**
 * 题目1：实现 WeakPtr
 *
 * 关键点：
 * 1. 不增加强引用计数
 * 2. lock() 时检查并安全地增加强引用
 */
template <typename T>
class WeakPtr {
public:
    WeakPtr() noexcept : cb_(nullptr) {}

    WeakPtr(const SharedPtr<T>& shared) : cb_(shared.cb_) {
        if (cb_) cb_->add_weak();
    }

    ~WeakPtr() {
        if (cb_ && cb_->release_weak()) {
            delete cb_;
        }
    }

    WeakPtr(const WeakPtr& other) : cb_(other.cb_) {
        if (cb_) cb_->add_weak();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            if (cb_ && cb_->release_weak()) delete cb_;
            cb_ = other.cb_;
            if (cb_) cb_->add_weak();
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& shared) {
        if (cb_ && cb_->release_weak()) delete cb_;
        cb_ = shared.cb_;
        if (cb_) cb_->add_weak();
        return *this;
    }

    WeakPtr(WeakPtr&& other) noexcept : cb_(other.cb_) {
        other.cb_ = nullptr;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            if (cb_ && cb_->release_weak()) delete cb_;
            cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    bool expired() const noexcept {
        return !cb_ || cb_->strong_count.load(std::memory_order_relaxed) == 0;
    }

    SharedPtr<T> lock() const noexcept {
        if (cb_ && cb_->try_add_strong()) {
            return SharedPtr<T>(cb_);
        }
        return SharedPtr<T>();
    }

    long use_count() const noexcept {
        return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
    }

    void reset() noexcept {
        if (cb_ && cb_->release_weak()) {
            delete cb_;
        }
        cb_ = nullptr;
    }

    void swap(WeakPtr& other) noexcept {
        std::swap(cb_, other.cb_);
    }

private:
    ControlBlock<T>* cb_;
};

/**
 * 题目2：解决循环引用问题
 *
 * 场景演示：使用 weak_ptr 打破循环
 */
struct NodeWeak {
    int value;
    SharedPtr<NodeWeak> next;      // 强引用向前
    WeakPtr<NodeWeak> prev;        // 弱引用向后，打破循环

    NodeWeak(int v) : value(v) {
        std::cout << "NodeWeak(" << value << ") constructed\n";
    }
    ~NodeWeak() {
        std::cout << "NodeWeak(" << value << ") destroyed\n";
    }
};

// 错误示例：会导致内存泄漏
struct NodeBad {
    int value;
    SharedPtr<NodeBad> next;
    SharedPtr<NodeBad> prev;  // 双向强引用 -> 循环引用!

    NodeBad(int v) : value(v) {
        std::cout << "NodeBad(" << value << ") constructed\n";
    }
    ~NodeBad() {
        std::cout << "NodeBad(" << value << ") destroyed\n";
    }
};

/**
 * 题目3：实现 enable_shared_from_this
 *
 * 允许从成员函数安全地获取 shared_ptr<this>
 */
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> shared_from_this() {
        return weak_this_.lock();
    }

    WeakPtr<T> weak_from_this() const noexcept {
        return weak_this_;
    }

protected:
    EnableSharedFromThis() = default;
    EnableSharedFromThis(const EnableSharedFromThis&) = default;
    EnableSharedFromThis& operator=(const EnableSharedFromThis&) = default;
    ~EnableSharedFromThis() = default;

private:
    template <typename U> friend class SharedPtrESFT;
    mutable WeakPtr<T> weak_this_;
};

// 支持 enable_shared_from_this 的 SharedPtr
template <typename T>
class SharedPtrESFT {
public:
    explicit SharedPtrESFT(T* ptr) : ptr_(ptr) {
        if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
            // 初始化 weak_this_
            // 实际实现需要更复杂的处理
        }
    }
    // ... 其他实现
private:
    SharedPtr<T> ptr_;
};

int main() {
    std::cout << "=== 测试 WeakPtr ===\n";
    {
        SharedPtr<int> sp(new int(42));
        WeakPtr<int> wp(sp);

        std::cout << "sp use_count: " << sp.use_count() << "\n";
        std::cout << "wp expired: " << (wp.expired() ? "yes" : "no") << "\n";

        if (auto locked = wp.lock()) {
            std::cout << "Locked value: " << *locked << "\n";
            std::cout << "After lock, use_count: " << sp.use_count() << "\n";
        }
    }

    std::cout << "\n=== 测试 WeakPtr 过期 ===\n";
    {
        WeakPtr<int> wp;
        {
            SharedPtr<int> sp(new int(100));
            wp = sp;
            std::cout << "Inside: expired = " << (wp.expired() ? "yes" : "no") << "\n";
        }
        std::cout << "Outside: expired = " << (wp.expired() ? "yes" : "no") << "\n";

        auto locked = wp.lock();
        std::cout << "Lock result: " << (locked ? "success" : "failed") << "\n";
    }

    std::cout << "\n=== 正确使用 WeakPtr 打破循环引用 ===\n";
    {
        SharedPtr<NodeWeak> n1(new NodeWeak(1));
        SharedPtr<NodeWeak> n2(new NodeWeak(2));

        n1->next = n2;
        n2->prev = n1;  // weak_ptr，不增加引用计数

        std::cout << "n1 use_count: " << n1.use_count() << "\n";
        std::cout << "n2 use_count: " << n2.use_count() << "\n";

        // 访问 prev
        if (auto prev = n2->prev.lock()) {
            std::cout << "n2's prev value: " << prev->value << "\n";
        }
    }
    std::cout << "Nodes should be destroyed above\n";

    std::cout << "\n=== 循环引用导致内存泄漏（演示）===\n";
    std::cout << "(Commented out to avoid leak)\n";
    /*
    {
        SharedPtr<NodeBad> n1(new NodeBad(1));
        SharedPtr<NodeBad> n2(new NodeBad(2));

        n1->next = n2;
        n2->prev = n1;  // 循环引用!

        // 离开作用域时，n1 和 n2 的引用计数仍为 1
        // 导致内存泄漏
    }
    */

    return 0;
}
