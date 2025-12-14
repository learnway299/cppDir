/**
 * @file lockfree_stack.cpp
 * @brief 无锁栈 - 面试题实现
 */
#include "lockfree_stack.h"

namespace LockFreeStack {

// ==================== 面试者实现区域 ====================

// 题目1: 实现基本的无锁栈
template <typename T>
void LockFreeStack<T>::push(const T& value) {
    // TODO: 使用 CAS 实现无锁 push
}

template <typename T>
bool LockFreeStack<T>::pop(T& result) {
    // TODO: 使用 CAS 实现无锁 pop
    return false;
}

template <typename T>
bool LockFreeStack<T>::empty() const {
    // TODO: 检查栈是否为空
    return true;
}

// 题目2: 解决 ABA 问题
template <typename T>
void LockFreeStackABA<T>::push(const T& value) {
    // TODO: 使用带版本号的指针实现 push
}

template <typename T>
bool LockFreeStackABA<T>::pop(T& result) {
    // TODO: 使用带版本号的指针实现 pop
    return false;
}

// 题目3: 使用 shared_ptr 的无锁栈
template <typename T>
void LockFreeStackShared<T>::push(const T& value) {
    // TODO: 使用 shared_ptr 实现 push
}

template <typename T>
std::shared_ptr<T> LockFreeStackShared<T>::pop() {
    // TODO: 使用 shared_ptr 实现 pop
    return nullptr;
}

// 显式实例化常用模板
template class LockFreeStack<int>;
template class LockFreeStackABA<int>;
template class LockFreeStackShared<int>;

} // namespace LockFreeStack
