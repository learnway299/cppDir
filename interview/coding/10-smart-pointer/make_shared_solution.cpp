/**
 * @file make_shared_solution.cpp
 * @brief make_shared/make_unique 实现 - 解答
 */

#include <iostream>
#include <memory>
#include <utility>
#include <atomic>
#include <new>

/**
 * 题目1：为什么要使用 make_shared/make_unique？
 *
 * 原因1：异常安全
 * - shared_ptr<T>(new T(...)) 中，new 和 shared_ptr 构造是两个步骤
 * - 如果中间有异常，new 的内存可能泄漏
 *
 * 原因2：效率（make_shared）
 * - new T + shared_ptr 需要两次内存分配
 * - make_shared 只需一次分配（对象和控制块连续存储）
 *
 * 原因3：代码简洁
 * - 避免重复类型名
 */

// 演示异常安全问题
int g() {
    throw std::runtime_error("exception");
    return 0;
}

void foo(std::shared_ptr<int> p, int x) {
    std::cout << "foo called\n";
}

void exceptionSafetyDemo() {
    // 危险代码（C++17 前）：
    // foo(std::shared_ptr<int>(new int(42)), g());
    //
    // 求值顺序可能是：
    // 1. new int(42)
    // 2. g() 抛出异常
    // 3. shared_ptr 构造未执行
    // 结果：内存泄漏！

    // 安全代码：
    // foo(std::make_shared<int>(42), g());
    // make_shared 是单个函数调用，要么完全执行，要么不执行
}

/**
 * 题目2：实现 make_unique
 */
template <typename T, typename... Args>
std::unique_ptr<T> myMakeUnique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 数组版本（未知大小）
template <typename T>
std::unique_ptr<T[]> myMakeUniqueArray(size_t size) {
    return std::unique_ptr<T[]>(new T[size]());
}

// 禁止已知大小数组（如 make_unique<int[5]>()）
template <typename T, typename... Args>
typename std::enable_if<std::extent<T>::value != 0>::type
myMakeUnique(Args&&...) = delete;

/**
 * 题目3：实现 make_shared（单次分配版本）
 *
 * 关键：控制块和对象一起分配
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

template <typename T, typename... Args>
MySharedPtr<T> myMakeShared(Args&&... args) {
    auto* block = new CombinedBlock<T>(std::forward<Args>(args)...);
    return MySharedPtr<T>(block);
}

/**
 * 题目4：make_shared 的缺点
 */
void makeSharedDrawbacks() {
    std::cout << "=== make_shared 缺点分析 ===\n";

    // 缺点1：不支持自定义删除器
    std::cout << "\n1. 不支持自定义删除器\n";
    // 必须使用构造函数：
    auto fileDeleter = [](FILE* f) {
        if (f) fclose(f);
    };
    std::shared_ptr<FILE> file(fopen("test.txt", "w"), fileDeleter);

    // 缺点2：weak_ptr 延长内存占用
    std::cout << "\n2. weak_ptr 延长内存占用\n";
    struct LargeObject {
        char data[1024 * 1024];  // 1MB
        ~LargeObject() {
            std::cout << "LargeObject destroyed\n";
        }
    };

    std::weak_ptr<LargeObject> wp;
    {
        // make_shared: 对象和控制块一起分配
        auto sp = std::make_shared<LargeObject>();
        wp = sp;
        std::cout << "sp going out of scope...\n";
    }
    std::cout << "Object destroyed, but memory still held by weak_ptr\n";
    // 1MB 内存仍被占用，直到所有 weak_ptr 也销毁

    // 解决方案：对大对象使用 shared_ptr(new T)
    {
        std::shared_ptr<LargeObject> sp(new LargeObject());
        // 对象和控制块分别分配
        // 当 strong_count = 0 时，对象内存立即释放
    }

    // 缺点3：不能使用 {} 初始化
    std::cout << "\n3. 不能使用列表初始化\n";
    // auto v = std::make_shared<std::vector<int>>({1, 2, 3});  // 编译错误
    auto v = std::make_shared<std::vector<int>>(std::initializer_list<int>{1, 2, 3});
}

/**
 * 题目5：allocate_shared 实现示意
 */
template <typename T, typename Alloc, typename... Args>
std::shared_ptr<T> myAllocateShared(const Alloc& alloc, Args&&... args) {
    // 实际实现需要使用 allocator_traits
    // 这里简化为使用标准库
    return std::allocate_shared<T>(alloc, std::forward<Args>(args)...);
}

// 测试类
struct TestClass {
    int value;
    std::string name;

    TestClass(int v, const std::string& n) : value(v), name(n) {
        std::cout << "TestClass(" << value << ", " << name << ") constructed\n";
    }

    ~TestClass() {
        std::cout << "TestClass(" << value << ", " << name << ") destroyed\n";
    }
};

int main() {
    std::cout << "=== 测试 myMakeUnique ===\n";
    {
        auto p = myMakeUnique<TestClass>(42, "unique");
        std::cout << "Value: " << p->value << ", Name: " << p->name << "\n";
    }

    std::cout << "\n=== 测试 myMakeUniqueArray ===\n";
    {
        auto arr = myMakeUniqueArray<int>(5);
        for (int i = 0; i < 5; ++i) {
            arr[i] = i * 10;
        }
        for (int i = 0; i < 5; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n=== 测试 myMakeShared ===\n";
    {
        auto p1 = myMakeShared<TestClass>(100, "shared1");
        std::cout << "p1 use_count: " << p1.use_count() << "\n";

        auto p2 = p1;
        std::cout << "After copy, use_count: " << p1.use_count() << "\n";

        p2 = myMakeShared<TestClass>(200, "shared2");
        std::cout << "After reassign, p1 use_count: " << p1.use_count() << "\n";
    }

    std::cout << "\n=== 分析 make_shared 缺点 ===\n";
    makeSharedDrawbacks();

    return 0;
}
