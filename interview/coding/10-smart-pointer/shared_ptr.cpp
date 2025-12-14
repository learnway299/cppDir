/**
 * @file shared_ptr.cpp
 * @brief shared_ptr 实现 - 面试者实现文件
 */

#include "shared_ptr.h"
#include <iostream>

namespace SharedPtrImpl {

// ==================== SharedPtr 实现 ====================

template <typename T>
SharedPtr<T>::SharedPtr(T* ptr) : ptr_(ptr), count_(ptr ? new long(1) : nullptr) {
    // TODO: 基本实现已提供
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    // TODO: 实现析构，减少计数，计数为0时释放资源
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other) : ptr_(nullptr), count_(nullptr) {
    // TODO: 实现拷贝构造，增加引用计数
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept : ptr_(nullptr), count_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
T* SharedPtr<T>::get() const noexcept {
    return ptr_;
}

template <typename T>
long SharedPtr<T>::use_count() const noexcept {
    // TODO: 返回引用计数
    return 0;
}

template <typename T>
bool SharedPtr<T>::unique() const noexcept {
    return use_count() == 1;
}

template <typename T>
void SharedPtr<T>::reset(T* ptr) {
    // TODO: 实现 reset
}

template <typename T>
void SharedPtr<T>::swap(SharedPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(count_, other.count_);
}

template <typename T>
T& SharedPtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* SharedPtr<T>::operator->() const noexcept {
    return ptr_;
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
void SharedPtr<T>::release() {
    // TODO: 实现释放逻辑
}

// ==================== SharedPtrAtomic 实现 ====================

template <typename T>
SharedPtrAtomic<T>::SharedPtrAtomic(T* ptr)
    : ptr_(ptr), count_(ptr ? new std::atomic<long>(1) : nullptr) {
}

template <typename T>
SharedPtrAtomic<T>::~SharedPtrAtomic() {
    // TODO: 实现析构
}

template <typename T>
SharedPtrAtomic<T>::SharedPtrAtomic(const SharedPtrAtomic& other)
    : ptr_(nullptr), count_(nullptr) {
    // TODO: 实现拷贝构造
}

template <typename T>
SharedPtrAtomic<T>& SharedPtrAtomic<T>::operator=(const SharedPtrAtomic& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
SharedPtrAtomic<T>::SharedPtrAtomic(SharedPtrAtomic&& other) noexcept
    : ptr_(nullptr), count_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
SharedPtrAtomic<T>& SharedPtrAtomic<T>::operator=(SharedPtrAtomic&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
T* SharedPtrAtomic<T>::get() const noexcept {
    return ptr_;
}

template <typename T>
long SharedPtrAtomic<T>::use_count() const noexcept {
    // TODO: 实现
    return 0;
}

template <typename T>
T& SharedPtrAtomic<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* SharedPtrAtomic<T>::operator->() const noexcept {
    return ptr_;
}

template <typename T>
SharedPtrAtomic<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
void SharedPtrAtomic<T>::release() {
    // TODO: 实现原子释放逻辑
}

// ==================== ControlBlock 实现 ====================

template <typename T>
ControlBlock<T>::ControlBlock(T* p) : ptr(p), strong_count(1), weak_count(0) {
}

template <typename T>
void ControlBlock<T>::add_strong() {
    // TODO: 原子增加强引用
}

template <typename T>
void ControlBlock<T>::add_weak() {
    // TODO: 原子增加弱引用
}

template <typename T>
bool ControlBlock<T>::release_strong() {
    // TODO: 原子减少强引用，返回是否需要删除控制块
    return false;
}

template <typename T>
bool ControlBlock<T>::release_weak() {
    // TODO: 原子减少弱引用
    return false;
}

// ==================== SharedPtrWithControlBlock 实现 ====================

template <typename T>
SharedPtrWithControlBlock<T>::SharedPtrWithControlBlock(T* ptr)
    : cb_(ptr ? new ControlBlock<T>(ptr) : nullptr) {
}

template <typename T>
SharedPtrWithControlBlock<T>::~SharedPtrWithControlBlock() {
    // TODO: 实现析构
}

template <typename T>
SharedPtrWithControlBlock<T>::SharedPtrWithControlBlock(const SharedPtrWithControlBlock& other)
    : cb_(nullptr) {
    // TODO: 实现拷贝构造
}

template <typename T>
SharedPtrWithControlBlock<T>& SharedPtrWithControlBlock<T>::operator=(const SharedPtrWithControlBlock& other) {
    // TODO: 实现拷贝赋值
    return *this;
}

template <typename T>
SharedPtrWithControlBlock<T>::SharedPtrWithControlBlock(SharedPtrWithControlBlock&& other) noexcept
    : cb_(nullptr) {
    // TODO: 实现移动构造
}

template <typename T>
SharedPtrWithControlBlock<T>& SharedPtrWithControlBlock<T>::operator=(SharedPtrWithControlBlock&& other) noexcept {
    // TODO: 实现移动赋值
    return *this;
}

template <typename T>
T* SharedPtrWithControlBlock<T>::get() const noexcept {
    return cb_ ? cb_->ptr : nullptr;
}

template <typename T>
long SharedPtrWithControlBlock<T>::use_count() const noexcept {
    return cb_ ? cb_->strong_count.load(std::memory_order_relaxed) : 0;
}

template <typename T>
T& SharedPtrWithControlBlock<T>::operator*() const {
    return *get();
}

template <typename T>
T* SharedPtrWithControlBlock<T>::operator->() const noexcept {
    return get();
}

template <typename T>
SharedPtrWithControlBlock<T>::operator bool() const noexcept {
    return get() != nullptr;
}

// ==================== make_shared 实现 ====================

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
    // TODO: 实现 make_shared
    return SharedPtr<T>(nullptr);
}

// ==================== 测试函数 ====================

void testSharedPtr() {
    std::cout << "=== Shared Ptr Tests (User Implementation) ===\n";
    // TODO: 添加面试者实现的测试
    std::cout << "  (No tests yet - implement your solutions first)\n";
}

} // namespace SharedPtrImpl
