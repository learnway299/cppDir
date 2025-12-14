/**
 * @file basic_sync.cpp
 * @brief 基础同步原语 - 面试题实现
 */

#include "basic_sync.h"

namespace BasicSync {

// ==================== 面试者实现 ====================

// 题目1: 线程安全计数器
void Counter::increment() {
    // TODO
}

void Counter::decrement() {
    // TODO
}

int Counter::getValue() const {
    // TODO
    return 0;
}

// 题目2: 事件
void Event::wait() {
    // TODO
}

void Event::signal() {
    // TODO
}

void Event::signalAll() {
    // TODO
}

void Event::reset() {
    // TODO
}

// 题目3: 自旋锁
void SpinLock::lock() {
    // TODO
}

void SpinLock::unlock() {
    // TODO
}

bool SpinLock::try_lock() {
    // TODO
    return false;
}

// 题目4: 信号量
Semaphore::Semaphore(int count) {
    // TODO
}

void Semaphore::acquire() {
    // TODO
}

void Semaphore::release() {
    // TODO
}

bool Semaphore::try_acquire() {
    // TODO
    return false;
}

// 题目5: 限时锁
template <typename Rep, typename Period>
bool TimedLock::try_lock_for(const std::chrono::duration<Rep, Period>& timeout) {
    // TODO
    return false;
}

void TimedLock::unlock() {
    // TODO
}

} // namespace BasicSync
