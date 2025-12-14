/**
 * @file unique_ptr.cpp
 * @brief unique_ptr 实现 - 面试者实现文件
 */

#include "unique_ptr.h"
#include <iostream>

namespace SmartPointer {

// ==================== TestClass 实现 ====================

int TestClass::instanceCount = 0;

TestClass::TestClass(int v) : value(v) {
    ++instanceCount;
}

TestClass::~TestClass() {
    --instanceCount;
}

// ==================== UniquePtr 实现 ====================

template <typename T>
UniquePtr<T>::UniquePtr(T* ptr) noexcept : ptr_(ptr) {
    // TODO: 已实现基本构造
}

template <typename T>
UniquePtr<T>::~UniquePtr() {
    // TODO: 实现析构函数，释放资源
}

template <typename T>
UniquePtr<T>::UniquePtr(UniquePtr&& other) noexcept : ptr_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator=(std::nullptr_t) noexcept {
    // TODO: 实现 nullptr 赋值
    return *this;
}

template <typename T>
T* UniquePtr<T>::get() const noexcept {
    // TODO: 返回原始指针
    return nullptr;
}

template <typename T>
T* UniquePtr<T>::release() noexcept {
    // TODO: 释放所有权并返回指针
    return nullptr;
}

template <typename T>
void UniquePtr<T>::reset(T* ptr) noexcept {
    // TODO: 重置指针
}

template <typename T>
void UniquePtr<T>::swap(UniquePtr& other) noexcept {
    // TODO: 交换指针
}

template <typename T>
T& UniquePtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* UniquePtr<T>::operator->() const noexcept {
    return ptr_;
}

template <typename T>
UniquePtr<T>::operator bool() const noexcept {
    // TODO: 返回是否持有有效指针
    return false;
}

// ==================== UniquePtrWithDeleter 实现 ====================

template <typename T, typename Deleter>
UniquePtrWithDeleter<T, Deleter>::UniquePtrWithDeleter(T* ptr, Deleter d)
    : ptr_(ptr), deleter_(std::move(d)) {
    // TODO: 基本实现
}

template <typename T, typename Deleter>
UniquePtrWithDeleter<T, Deleter>::~UniquePtrWithDeleter() {
    // TODO: 使用删除器释放资源
}

template <typename T, typename Deleter>
UniquePtrWithDeleter<T, Deleter>::UniquePtrWithDeleter(UniquePtrWithDeleter&& other) noexcept
    : ptr_(nullptr), deleter_(std::move(other.deleter_)) {
    // TODO: 实现移动构造
}

template <typename T, typename Deleter>
UniquePtrWithDeleter<T, Deleter>& UniquePtrWithDeleter<T, Deleter>::operator=(UniquePtrWithDeleter&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T, typename Deleter>
T* UniquePtrWithDeleter<T, Deleter>::get() const noexcept {
    return ptr_;
}

template <typename T, typename Deleter>
T* UniquePtrWithDeleter<T, Deleter>::release() noexcept {
    // TODO: 实现
    return nullptr;
}

template <typename T, typename Deleter>
void UniquePtrWithDeleter<T, Deleter>::reset(T* ptr) noexcept {
    // TODO: 实现
}

template <typename T, typename Deleter>
Deleter& UniquePtrWithDeleter<T, Deleter>::get_deleter() noexcept {
    return deleter_;
}

template <typename T, typename Deleter>
const Deleter& UniquePtrWithDeleter<T, Deleter>::get_deleter() const noexcept {
    return deleter_;
}

template <typename T, typename Deleter>
T& UniquePtrWithDeleter<T, Deleter>::operator*() const {
    return *ptr_;
}

template <typename T, typename Deleter>
T* UniquePtrWithDeleter<T, Deleter>::operator->() const noexcept {
    return ptr_;
}

template <typename T, typename Deleter>
UniquePtrWithDeleter<T, Deleter>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

// ==================== UniquePtrArray 实现 ====================

template <typename T>
UniquePtrArray<T>::UniquePtrArray(T* ptr) noexcept : ptr_(ptr) {
}

template <typename T>
UniquePtrArray<T>::~UniquePtrArray() {
    // TODO: 使用 delete[] 释放
}

template <typename T>
UniquePtrArray<T>::UniquePtrArray(UniquePtrArray&& other) noexcept : ptr_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
UniquePtrArray<T>& UniquePtrArray<T>::operator=(UniquePtrArray&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
T* UniquePtrArray<T>::get() const noexcept {
    return ptr_;
}

template <typename T>
T* UniquePtrArray<T>::release() noexcept {
    // TODO: 实现
    return nullptr;
}

template <typename T>
void UniquePtrArray<T>::reset(T* ptr) noexcept {
    // TODO: 实现
}

template <typename T>
T& UniquePtrArray<T>::operator[](size_t index) const {
    return ptr_[index];
}

template <typename T>
UniquePtrArray<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

// ==================== make_unique 实现 ====================

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    // TODO: 实现 make_unique
    return UniquePtr<T>(nullptr);
}

template <typename T>
UniquePtrArray<T> makeUniqueArray(size_t size) {
    // TODO: 实现数组版 make_unique
    return UniquePtrArray<T>(nullptr);
}

// 显式实例化常用类型
template class UniquePtr<TestClass>;
template class UniquePtr<int>;
template class UniquePtrArray<int>;

// ==================== 测试函数 ====================

void testUniquePtr() {
    std::cout << "=== Unique Ptr Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace SmartPointer
