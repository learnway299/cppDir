/**
 * @file shared_ptr_solution.cpp
 * @brief shared_ptr 实现 - 解答
 */

#include <iostream>
#include <atomic>
#include <utility>

/**
 * 题目1：实现基础 SharedPtr（非线程安全版本）
 *
 * 关键点：
 * 1. 引用计数与指针分离存储
 * 2. 拷贝时增加计数，析构时减少计数
 * 3. 计数为 0 时释放资源
 */
template <typename T>
class SharedPtr {
public:
    explicit SharedPtr(T* ptr = nullptr)
        : ptr_(ptr), count_(ptr ? new long(1) : nullptr) {}

    ~SharedPtr() {
        release();
    }

    SharedPtr(const SharedPtr& other)
        : ptr_(other.ptr_), count_(other.count_) {
        if (count_) {
            ++(*count_);
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            if (count_) {
                ++(*count_);
            }
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(other.ptr_), count_(other.count_) {
        other.ptr_ = nullptr;
        other.count_ = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            other.ptr_ = nullptr;
            other.count_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    long use_count() const noexcept {
        return count_ ? *count_ : 0;
    }

    void reset(T* ptr = nullptr) {
        release();
        ptr_ = ptr;
        count_ = ptr ? new long(1) : nullptr;
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(count_, other.count_);
    }

    bool unique() const noexcept {
        return use_count() == 1;
    }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    void release() {
        if (count_ && --(*count_) == 0) {
            delete ptr_;
            delete count_;
        }
        ptr_ = nullptr;
        count_ = nullptr;
    }

    T* ptr_;
    long* count_;
};

/**
 * 题目2：实现线程安全的 SharedPtr
 *
 * 关键点：
 * 1. 使用 atomic<long> 保证计数的原子性
 * 2. 注意内存序的选择
 */
template <typename T>
class SharedPtrAtomic {
public:
    explicit SharedPtrAtomic(T* ptr = nullptr)
        : ptr_(ptr), count_(ptr ? new std::atomic<long>(1) : nullptr) {}

    ~SharedPtrAtomic() {
        release();
    }

    SharedPtrAtomic(const SharedPtrAtomic& other)
        : ptr_(other.ptr_), count_(other.count_) {
        if (count_) {
            count_->fetch_add(1, std::memory_order_relaxed);
        }
    }

    SharedPtrAtomic& operator=(const SharedPtrAtomic& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            if (count_) {
                count_->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    SharedPtrAtomic(SharedPtrAtomic&& other) noexcept
        : ptr_(other.ptr_), count_(other.count_) {
        other.ptr_ = nullptr;
        other.count_ = nullptr;
    }

    SharedPtrAtomic& operator=(SharedPtrAtomic&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            count_ = other.count_;
            other.ptr_ = nullptr;
            other.count_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    long use_count() const noexcept {
        return count_ ? count_->load(std::memory_order_relaxed) : 0;
    }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    void release() {
        if (count_) {
            // 使用 acq_rel 确保正确的同步
            if (count_->fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete ptr_;
                delete count_;
            }
        }
        ptr_ = nullptr;
        count_ = nullptr;
    }

    T* ptr_;
    std::atomic<long>* count_;
};

/**
 * 题目3：实现控制块（更接近标准库实现）
 *
 * 控制块特点：
 * 1. 同时管理强引用和弱引用计数
 * 2. 强引用为 0 时删除对象
 * 3. 弱引用也为 0 时删除控制块
 */
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

    bool release_strong() {
        if (strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr;
            ptr = nullptr;
            // 如果没有弱引用，删除控制块
            if (weak_count.load(std::memory_order_acquire) == 0) {
                return true;  // 需要删除控制块
            }
        }
        return false;
    }

    bool release_weak() {
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            // 如果强引用已经为 0，删除控制块
            if (strong_count.load(std::memory_order_acquire) == 0) {
                return true;
            }
        }
        return false;
    }
};

template <typename T>
class WeakPtrImpl;  // 前向声明

template <typename T>
class SharedPtrWithControlBlock {
public:
    friend class WeakPtrImpl<T>;

    explicit SharedPtrWithControlBlock(T* ptr = nullptr)
        : cb_(ptr ? new ControlBlock<T>(ptr) : nullptr) {}

    ~SharedPtrWithControlBlock() {
        if (cb_ && cb_->release_strong()) {
            delete cb_;
        }
    }

    SharedPtrWithControlBlock(const SharedPtrWithControlBlock& other)
        : cb_(other.cb_) {
        if (cb_) {
            cb_->add_strong();
        }
    }

    SharedPtrWithControlBlock& operator=(const SharedPtrWithControlBlock& other) {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) {
                delete cb_;
            }
            cb_ = other.cb_;
            if (cb_) {
                cb_->add_strong();
            }
        }
        return *this;
    }

    SharedPtrWithControlBlock(SharedPtrWithControlBlock&& other) noexcept
        : cb_(other.cb_) {
        other.cb_ = nullptr;
    }

    SharedPtrWithControlBlock& operator=(SharedPtrWithControlBlock&& other) noexcept {
        if (this != &other) {
            if (cb_ && cb_->release_strong()) {
                delete cb_;
            }
            cb_ = other.cb_;
            other.cb_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept {
        return cb_ ? cb_->ptr : nullptr;
    }

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
 * 题目4：实现 make_shared
 *
 * 优化版本：单次分配对象和控制块
 */
template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

// 测试类
struct TestClass {
    int value;
    TestClass(int v = 0) : value(v) {
        std::cout << "TestClass(" << value << ") constructed\n";
    }
    ~TestClass() {
        std::cout << "TestClass(" << value << ") destroyed\n";
    }
};

int main() {
    std::cout << "=== 测试基础 SharedPtr ===\n";
    {
        SharedPtr<TestClass> p1(new TestClass(1));
        std::cout << "p1 use_count: " << p1.use_count() << "\n";

        {
            SharedPtr<TestClass> p2 = p1;
            std::cout << "After copy, p1 use_count: " << p1.use_count() << "\n";
            std::cout << "p2 use_count: " << p2.use_count() << "\n";
        }

        std::cout << "After p2 destroyed, p1 use_count: " << p1.use_count() << "\n";
    }

    std::cout << "\n=== 测试线程安全 SharedPtr ===\n";
    {
        SharedPtrAtomic<TestClass> p1(new TestClass(2));
        SharedPtrAtomic<TestClass> p2 = p1;
        std::cout << "Atomic use_count: " << p1.use_count() << "\n";
    }

    std::cout << "\n=== 测试控制块版本 ===\n";
    {
        SharedPtrWithControlBlock<TestClass> p1(new TestClass(3));
        SharedPtrWithControlBlock<TestClass> p2 = p1;
        std::cout << "ControlBlock use_count: " << p1.use_count() << "\n";
    }

    std::cout << "\n=== 测试 make_shared ===\n";
    {
        auto p = makeShared<TestClass>(42);
        std::cout << "make_shared value: " << p->value << "\n";
    }

    return 0;
}
