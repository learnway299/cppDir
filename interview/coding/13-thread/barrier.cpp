/**
 * @file barrier.cpp
 * @brief 屏障 (Barrier) - 面试题实现
 *
 * 屏障是一种同步原语，用于让一组线程在某个点上等待，
 * 直到所有线程都到达该点后才能继续执行。
 */

#include "barrier.h"

namespace Barrier {

// ==================== 面试者实现 ====================

// 题目1: 基本屏障
Barrier::Barrier(int count) : threshold_(count) {
    // TODO
}

void Barrier::wait() {
    // TODO
}

// 题目2: 带回调的屏障
BarrierWithCallback::BarrierWithCallback(int count, std::function<void()> callback)
    : threshold_(count), callback_(callback) {
    // TODO
}

void BarrierWithCallback::wait() {
    // TODO
}

// 题目3: 可重置的屏障
ResetableBarrier::ResetableBarrier(int count) {
    // TODO
}

void ResetableBarrier::wait() {
    // TODO
}

void ResetableBarrier::reset(int newCount) {
    // TODO
}

// 题目4: CountDownLatch
CountDownLatch::CountDownLatch(int count) {
    // TODO
}

void CountDownLatch::countDown() {
    // TODO
}

void CountDownLatch::wait() {
    // TODO
}

// 题目5: CyclicBarrier
CyclicBarrier::CyclicBarrier(int parties) {
    // TODO
}

int CyclicBarrier::await() {
    // TODO
    return 0;
}

int CyclicBarrier::await(int timeoutMs) {
    // TODO
    return -1;
}

void CyclicBarrier::reset() {
    // TODO
}

int CyclicBarrier::getNumberWaiting() const {
    // TODO
    return 0;
}

} // namespace Barrier
