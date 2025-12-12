/**
 * @file unique_ptr_solution.cpp
 * @brief unique_ptr 实现 - 解答
 */

#include <iostream>
#include <utility>
#include <functional>

/**
 * 题目1：实现基础 UniquePtr
 *
 * 关键点：
 * 1. 独占所有权 - 禁止拷贝
 * 2. 移动语义 - 转移所有权
 * 3. RAII - 析构时释放资源
 */
template <typename T>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) noexcept : ptr_(ptr) {}

    ~UniquePtr() {
        delete ptr_;
    }

    // 禁止拷贝
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 移动构造
    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    // 移动赋值
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // 支持 nullptr 赋值
    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    T* get() const noexcept {
        return ptr_;
    }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        T* old = ptr_;
        ptr_ = ptr;
        delete old;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const noexcept {
        return ptr_;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

/**
 * 题目2：实现带自定义删除器的 UniquePtr
 *
 * 删除器作为模板参数，支持零开销抽象
 */
template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtrWithDeleter {
public:
    explicit UniquePtrWithDeleter(T* ptr = nullptr, Deleter d = Deleter())
        : ptr_(ptr), deleter_(std::move(d)) {}

    ~UniquePtrWithDeleter() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }

    UniquePtrWithDeleter(const UniquePtrWithDeleter&) = delete;
    UniquePtrWithDeleter& operator=(const UniquePtrWithDeleter&) = delete;

    UniquePtrWithDeleter(UniquePtrWithDeleter&& other) noexcept
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }

    UniquePtrWithDeleter& operator=(UniquePtrWithDeleter&& other) noexcept {
        if (this != &other) {
            if (ptr_) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }
    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        T* old = ptr_;
        ptr_ = ptr;
        if (old) {
            deleter_(old);
        }
    }

    Deleter& get_deleter() noexcept { return deleter_; }
    const Deleter& get_deleter() const noexcept { return deleter_; }

    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    T* ptr_;
    Deleter deleter_;
};

/**
 * 题目3：实现数组特化版本
 *
 * 关键点：
 * 1. 使用 delete[] 释放
 * 2. 支持 operator[]
 * 3. 禁用 operator* 和 operator->
 */
template <typename T>
class UniquePtrArray {
public:
    explicit UniquePtrArray(T* ptr = nullptr) noexcept : ptr_(ptr) {}

    ~UniquePtrArray() {
        delete[] ptr_;
    }

    UniquePtrArray(const UniquePtrArray&) = delete;
    UniquePtrArray& operator=(const UniquePtrArray&) = delete;

    UniquePtrArray(UniquePtrArray&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    UniquePtrArray& operator=(UniquePtrArray&& other) noexcept {
        if (this != &other) {
            delete[] ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        T* old = ptr_;
        ptr_ = ptr;
        delete[] old;
    }

    T& operator[](size_t index) const {
        return ptr_[index];
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

/**
 * 题目4：实现 make_unique
 *
 * 优点：
 * 1. 异常安全
 * 2. 避免裸 new
 * 3. 代码更简洁
 */
template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

// 数组版本
template <typename T>
UniquePtrArray<T> makeUniqueArray(size_t size) {
    return UniquePtrArray<T>(new T[size]());
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
    std::cout << "=== 测试基础 UniquePtr ===\n";
    {
        UniquePtr<TestClass> p1(new TestClass(1));
        std::cout << "p1 value: " << p1->value << "\n";

        UniquePtr<TestClass> p2 = std::move(p1);
        std::cout << "After move, p1 is " << (p1 ? "valid" : "null") << "\n";
        std::cout << "p2 value: " << p2->value << "\n";

        p2.reset(new TestClass(2));
        std::cout << "After reset, p2 value: " << p2->value << "\n";
    }

    std::cout << "\n=== 测试自定义删除器 ===\n";
    {
        auto customDeleter = [](TestClass* p) {
            std::cout << "Custom deleter called\n";
            delete p;
        };

        UniquePtrWithDeleter<TestClass, decltype(customDeleter)>
            p(new TestClass(3), customDeleter);
    }

    std::cout << "\n=== 测试数组版本 ===\n";
    {
        UniquePtrArray<int> arr(new int[5]);
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        for (int i = 0; i < 5; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n=== 测试 make_unique ===\n";
    {
        auto p = makeUnique<TestClass>(42);
        std::cout << "make_unique value: " << p->value << "\n";
    }

    return 0;
}
