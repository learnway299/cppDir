/**
 * @file unique_ptr.h
 * @brief unique_ptr 实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 基础 UniquePtr 实现
 * 2. 带自定义删除器的 UniquePtr
 * 3. 数组特化版本
 * 4. make_unique 实现
 */

#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include <utility>
#include <cstddef>
#include <functional>

namespace SmartPointer {

// ==================== 测试辅助类 ====================

struct TestClass {
    int value;
    static int instanceCount;

    TestClass(int v = 0);
    ~TestClass();
};

// ==================== 题目1: 基础 UniquePtr ====================

/**
 * 题目1: 实现基础 UniquePtr
 *
 * 要求：
 * 1. 独占所有权语义 - 禁止拷贝构造和拷贝赋值
 * 2. 支持移动构造和移动赋值
 * 3. 析构时自动释放资源 (RAII)
 * 4. 支持 get()、release()、reset() 操作
 * 5. 支持 operator* 和 operator->
 * 6. 支持 operator bool 显式转换
 *
 * 关键点：
 * - 移动后原指针置为 nullptr
 * - reset 时先保存旧指针再删除，防止自我赋值问题
 *
 * 示例:
 *   UniquePtr<int> p1(new int(42));
 *   UniquePtr<int> p2 = std::move(p1);  // p1 变为空
 *   *p2 = 100;  // 修改值
 */
template <typename T>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) noexcept;
    ~UniquePtr();

    // 禁止拷贝
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 移动语义
    UniquePtr(UniquePtr&& other) noexcept;
    UniquePtr& operator=(UniquePtr&& other) noexcept;
    UniquePtr& operator=(std::nullptr_t) noexcept;

    // 访问操作
    T* get() const noexcept;
    T* release() noexcept;
    void reset(T* ptr = nullptr) noexcept;
    void swap(UniquePtr& other) noexcept;

    // 解引用操作
    T& operator*() const;
    T* operator->() const noexcept;

    // 布尔转换
    explicit operator bool() const noexcept;

private:
    T* ptr_;
};

// ==================== 题目2: 带自定义删除器的 UniquePtr ====================

/**
 * 题目2: 实现带自定义删除器的 UniquePtr
 *
 * 要求：
 * 1. 支持自定义删除器类型作为模板参数
 * 2. 删除器可以是函数指针、lambda、仿函数
 * 3. 提供 get_deleter() 访问删除器
 *
 * 优点：
 * - 零开销抽象（删除器类型编译期确定）
 * - 可用于管理非 new 分配的资源
 *
 * 示例:
 *   auto deleter = [](FILE* f) { fclose(f); };
 *   UniquePtrWithDeleter<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
 */
template <typename T, typename Deleter>
class UniquePtrWithDeleter {
public:
    explicit UniquePtrWithDeleter(T* ptr = nullptr, Deleter d = Deleter());
    ~UniquePtrWithDeleter();

    UniquePtrWithDeleter(const UniquePtrWithDeleter&) = delete;
    UniquePtrWithDeleter& operator=(const UniquePtrWithDeleter&) = delete;

    UniquePtrWithDeleter(UniquePtrWithDeleter&& other) noexcept;
    UniquePtrWithDeleter& operator=(UniquePtrWithDeleter&& other) noexcept;

    T* get() const noexcept;
    T* release() noexcept;
    void reset(T* ptr = nullptr) noexcept;

    Deleter& get_deleter() noexcept;
    const Deleter& get_deleter() const noexcept;

    T& operator*() const;
    T* operator->() const noexcept;
    explicit operator bool() const noexcept;

private:
    T* ptr_;
    Deleter deleter_;
};

// ==================== 题目3: 数组特化版本 ====================

/**
 * 题目3: 实现数组特化版本
 *
 * 要求：
 * 1. 使用 delete[] 释放内存
 * 2. 支持 operator[] 下标访问
 * 3. 禁用 operator* 和 operator->
 *
 * 示例:
 *   UniquePtrArray<int> arr(new int[10]);
 *   arr[0] = 42;
 */
template <typename T>
class UniquePtrArray {
public:
    explicit UniquePtrArray(T* ptr = nullptr) noexcept;
    ~UniquePtrArray();

    UniquePtrArray(const UniquePtrArray&) = delete;
    UniquePtrArray& operator=(const UniquePtrArray&) = delete;

    UniquePtrArray(UniquePtrArray&& other) noexcept;
    UniquePtrArray& operator=(UniquePtrArray&& other) noexcept;

    T* get() const noexcept;
    T* release() noexcept;
    void reset(T* ptr = nullptr) noexcept;

    T& operator[](size_t index) const;
    explicit operator bool() const noexcept;

private:
    T* ptr_;
};

// ==================== 题目4: make_unique ====================

/**
 * 题目4: 实现 make_unique
 *
 * 优点：
 * 1. 异常安全 - 不会泄漏内存
 * 2. 避免裸 new - 代码更现代
 * 3. 代码更简洁
 *
 * 示例:
 *   auto p = makeUnique<std::string>("hello");
 *   auto arr = makeUniqueArray<int>(10);
 */
template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args);

template <typename T>
UniquePtrArray<T> makeUniqueArray(size_t size);

// Solution 版本声明
template <typename T, typename... Args>
UniquePtr<T> makeUniqueSolution(Args&&... args);

template <typename T>
UniquePtrArray<T> makeUniqueArraySolution(size_t size);

// ==================== 测试函数声明 ====================

void testUniquePtr();          // 测试面试者实现
void testUniquePtrSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testUniquePtr();
    testUniquePtrSolution();
}

} // namespace SmartPointer

#endif // UNIQUE_PTR_H
