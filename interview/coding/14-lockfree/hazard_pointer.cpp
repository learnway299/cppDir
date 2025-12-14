/**
 * @file hazard_pointer.cpp
 * @brief 风险指针 - 面试题实现
 */
#include "hazard_pointer.h"

namespace HazardPointer {

// ==================== 面试者实现区域 ====================

// 题目1: 实现基本的 Hazard Pointer 系统
std::atomic<void*>& HazardPointerDomain::getHazardPointer() {
    // TODO: 返回当前线程可用的 HP 槽位
    static std::atomic<void*> dummy{nullptr};
    return dummy;
}

void HazardPointerDomain::retire(void* ptr, std::function<void(void*)> deleter) {
    // TODO: 将 ptr 加入退休列表，合适时机回收
}

void HazardPointerDomain::scan() {
    // TODO: 检查退休列表，回收不在任何 HP 中的指针
}

// 题目2: 使用 Hazard Pointer 实现安全的无锁栈
template <typename T>
void SafeLockFreeStack<T>::push(const T& value) {
    // TODO: 实现无锁 push
}

template <typename T>
bool SafeLockFreeStack<T>::pop(T& value) {
    // TODO: 使用 Hazard Pointer 保护节点后 pop
    return false;
}

// 题目3: 支持多个 Hazard Pointer
template <size_t K>
std::atomic<void*>& MultiHazardPointer<K>::get(size_t i) {
    // TODO: 返回第 i 个 HP
    static std::atomic<void*> dummy{nullptr};
    return dummy;
}

template <size_t K>
void MultiHazardPointer<K>::clearAll() {
    // TODO: 清除所有 HP
}

// 显式实例化常用模板
template class SafeLockFreeStack<int>;
template class MultiHazardPointer<2>;

} // namespace HazardPointer
