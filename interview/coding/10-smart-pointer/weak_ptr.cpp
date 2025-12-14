/**
 * @file weak_ptr.cpp
 * @brief weak_ptr 实现 - 面试者实现文件
 */

#include "weak_ptr.h"
#include <iostream>

namespace WeakPtrImpl {

// ==================== ControlBlock 实现 ====================

template <typename T>
ControlBlock<T>::ControlBlock(T* p) : ptr(p), strong_count(1), weak_count(0) {
}

template <typename T>
void ControlBlock<T>::add_strong() {
    // TODO: 原子增加强引用计数
}

template <typename T>
void ControlBlock<T>::add_weak() {
    // TODO: 原子增加弱引用计数
}

template <typename T>
bool ControlBlock<T>::release_strong() {
    // TODO: 原子减少强引用计数
    // 返回是否需要删除控制块
    return false;
}

template <typename T>
bool ControlBlock<T>::release_weak() {
    // TODO: 原子减少弱引用计数
    return false;
}

template <typename T>
bool ControlBlock<T>::try_add_strong() {
    // TODO: 尝试增加强引用（用于 lock）
    // 使用 CAS 操作，只有当 strong_count > 0 时才增加
    return false;
}

// ==================== SharedPtr 实现 ====================

template <typename T>
SharedPtr<T>::SharedPtr() noexcept : cb_(nullptr) {
}

template <typename T>
SharedPtr<T>::SharedPtr(T* ptr)
    : cb_(ptr ? new ControlBlock<T>(ptr) : nullptr) {
}

template <typename T>
SharedPtr<T>::SharedPtr(ControlBlock<T>* cb) : cb_(cb) {
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    // TODO: 释放强引用，可能需要删除控制块
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other) : cb_(other.cb_) {
    // TODO: 增加强引用计数
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept : cb_(other.cb_) {
    // TODO: 实现移动构造
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
T* SharedPtr<T>::get() const noexcept {
    return cb_ ? cb_->ptr : nullptr;
}

template <typename T>
long SharedPtr<T>::use_count() const noexcept {
    // TODO: 返回强引用计数
    return 0;
}

template <typename T>
T& SharedPtr<T>::operator*() const {
    return *get();
}

template <typename T>
T* SharedPtr<T>::operator->() const noexcept {
    return get();
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return get() != nullptr;
}

// ==================== WeakPtr 实现 ====================

template <typename T>
WeakPtr<T>::WeakPtr() noexcept : cb_(nullptr) {
}

template <typename T>
WeakPtr<T>::WeakPtr(const SharedPtr<T>& shared) : cb_(nullptr) {
    // TODO: 从 SharedPtr 构造，增加弱引用计数
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
    // TODO: 减少弱引用计数，可能需要删除控制块
}

template <typename T>
WeakPtr<T>::WeakPtr(const WeakPtr& other) : cb_(nullptr) {
    // TODO: 实现拷贝构造
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& shared) {
    // TODO: 从 SharedPtr 赋值
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept : cb_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
bool WeakPtr<T>::expired() const noexcept {
    // TODO: 检查是否过期（强引用计数是否为0）
    return true;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
    // TODO: 尝试获取 SharedPtr
    // 使用 try_add_strong() 原子地增加强引用
    return SharedPtr<T>();
}

template <typename T>
long WeakPtr<T>::use_count() const noexcept {
    // TODO: 返回强引用计数
    return 0;
}

template <typename T>
void WeakPtr<T>::reset() noexcept {
    // TODO: 重置 WeakPtr
}

template <typename T>
void WeakPtr<T>::swap(WeakPtr& other) noexcept {
    std::swap(cb_, other.cb_);
}

// ==================== NodeWithCycle 实现 ====================

int NodeWithCycle::instanceCount = 0;

NodeWithCycle::NodeWithCycle(int v) : value(v) {
    ++instanceCount;
}

NodeWithCycle::~NodeWithCycle() {
    --instanceCount;
}

// ==================== EnableSharedFromThis 实现 ====================

template <typename T>
SharedPtr<T> EnableSharedFromThis<T>::shared_from_this() {
    // TODO: 从 weak_this_ 获取 SharedPtr
    return SharedPtr<T>();
}

template <typename T>
WeakPtr<T> EnableSharedFromThis<T>::weak_from_this() const noexcept {
    return weak_this_;
}

// ==================== 测试函数 ====================

void testWeakPtr() {
    std::cout << "=== Weak Ptr Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace WeakPtrImpl
