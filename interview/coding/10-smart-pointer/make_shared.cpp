/**
 * @file make_shared.cpp
 * @brief make_shared/make_unique 实现 - 面试者实现文件
 */

#include "make_shared.h"
#include <new>
#include <iostream>

namespace MakeSharedImpl {

// ==================== TestClass 实现 ====================

int TestClass::instanceCount = 0;

TestClass::TestClass(int v, const std::string& n) : value(v), name(n) {
    ++instanceCount;
}

TestClass::~TestClass() {
    --instanceCount;
}

// ==================== make_unique 实现 ====================

template <typename T, typename... Args>
std::unique_ptr<T> myMakeUnique(Args&&... args) {
    // TODO: 实现 make_unique
    // 提示：使用完美转发
    return std::unique_ptr<T>(nullptr);
}

template <typename T>
std::unique_ptr<T[]> myMakeUniqueArray(size_t size) {
    // TODO: 实现数组版本
    // 提示：使用 new T[size]() 进行值初始化
    return std::unique_ptr<T[]>(nullptr);
}

// ==================== CombinedBlock 实现 ====================

template <typename T>
template <typename... Args>
CombinedBlock<T>::CombinedBlock(Args&&... args)
    : strong_count(1), weak_count(0) {
    // TODO: 使用 placement new 在 storage 中构造对象
}

template <typename T>
T* CombinedBlock<T>::getPtr() noexcept {
    // TODO: 返回指向对象的指针
    return nullptr;
}

template <typename T>
void CombinedBlock<T>::destroyObject() {
    // TODO: 调用析构函数（不释放内存）
}

// ==================== MySharedPtr 实现 ====================

template <typename T>
MySharedPtr<T>::MySharedPtr() noexcept : block_(nullptr) {
}

template <typename T>
MySharedPtr<T>::MySharedPtr(CombinedBlock<T>* block) noexcept : block_(block) {
}

template <typename T>
MySharedPtr<T>::~MySharedPtr() {
    // TODO: 释放资源
}

template <typename T>
MySharedPtr<T>::MySharedPtr(const MySharedPtr& other) noexcept : block_(nullptr) {
    // TODO: 实现拷贝构造
}

template <typename T>
MySharedPtr<T>& MySharedPtr<T>::operator=(const MySharedPtr& other) noexcept {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
MySharedPtr<T>::MySharedPtr(MySharedPtr&& other) noexcept : block_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
MySharedPtr<T>& MySharedPtr<T>::operator=(MySharedPtr&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
T* MySharedPtr<T>::get() const noexcept {
    // TODO: 返回对象指针
    return nullptr;
}

template <typename T>
long MySharedPtr<T>::use_count() const noexcept {
    // TODO: 返回引用计数
    return 0;
}

template <typename T>
T& MySharedPtr<T>::operator*() const {
    return *get();
}

template <typename T>
T* MySharedPtr<T>::operator->() const noexcept {
    return get();
}

template <typename T>
MySharedPtr<T>::operator bool() const noexcept {
    return block_ != nullptr;
}

template <typename T>
void MySharedPtr<T>::release() {
    // TODO: 减少引用计数，可能销毁对象和控制块
}

// ==================== myMakeShared 实现 ====================

template <typename T, typename... Args>
MySharedPtr<T> myMakeShared(Args&&... args) {
    // TODO: 实现 make_shared
    // 提示：创建 CombinedBlock 并返回 MySharedPtr
    return MySharedPtr<T>();
}

// ==================== 测试函数 ====================

void testMakeShared() {
    std::cout << "=== Make Shared Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace MakeSharedImpl
