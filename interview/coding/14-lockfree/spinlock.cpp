/**
 * @file spinlock.cpp
 * @brief 自旋锁 - 面试题实现
 */
#include "spinlock.h"

namespace Spinlock {

// ==================== 面试者实现区域 ====================

// 题目1: 实现基本的自旋锁
void SpinLock::lock() {
    // TODO: 使用 atomic_flag 实现 TAS 自旋锁
}

void SpinLock::unlock() {
    // TODO: 释放锁
}

// 题目2: 实现 TTAS 自旋锁
void TTASSpinLock::lock() {
    // TODO: 先测试再 TAS，减少缓存一致性流量
}

void TTASSpinLock::unlock() {
    // TODO: 释放锁
}

// 题目3: 实现带退避的自旋锁
void BackoffSpinLock::lock() {
    // TODO: 使用指数退避减少竞争
}

void BackoffSpinLock::unlock() {
    // TODO: 释放锁
}

// 题目4: 实现 Ticket Lock
void TicketLock::lock() {
    // TODO: 获取票号并等待
}

void TicketLock::unlock() {
    // TODO: 服务下一个票号
}

// 题目5: 实现可重入自旋锁
void ReentrantSpinLock::lock() {
    // TODO: 检查是否是同一线程重入
}

void ReentrantSpinLock::unlock() {
    // TODO: 递减计数或释放锁
}

} // namespace Spinlock
