/**
 * @file scheduler.cpp
 * @brief 协程调度器 - 面试者实现文件
 */

#include "scheduler.h"
#include <iostream>

namespace SchedulerImpl {

// ==================== 题目1: 简单调度器 ====================

void SimpleScheduler::schedule(std::coroutine_handle<> handle) {
    // TODO: 将协程加入就绪队列
}

void SimpleScheduler::run() {
    // TODO: 执行就绪队列中的所有协程
}

bool SimpleScheduler::has_pending() const {
    return !readyQueue_.empty();
}

SimpleScheduler*& SimpleScheduler::current() {
    static thread_local SimpleScheduler* instance = nullptr;
    return instance;
}

// ==================== 题目2: 多线程调度器 ====================

ThreadPoolScheduler::ThreadPoolScheduler(size_t numThreads) : running_(true) {
    // TODO: 创建工作线程
}

ThreadPoolScheduler::~ThreadPoolScheduler() {
    shutdown();
}

void ThreadPoolScheduler::schedule(std::coroutine_handle<> handle) {
    // TODO: 将协程加入任务队列并通知工作线程
}

void ThreadPoolScheduler::shutdown() {
    // TODO: 停止所有工作线程
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
    }
    cv_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPoolScheduler::wait_idle() {
    // TODO: 等待所有任务完成
}

void ThreadPoolScheduler::workerLoop() {
    // TODO: 工作线程循环
    // 1. 等待任务
    // 2. 取出任务
    // 3. 执行协程
}

// ==================== 题目3: 优先级调度器 ====================

void PriorityScheduler::schedule(std::coroutine_handle<> handle, Priority priority) {
    // TODO: 按优先级将协程加入对应队列
}

void PriorityScheduler::run() {
    // TODO: 优先执行高优先级协程
}

bool PriorityScheduler::has_pending() const {
    return !queues_[0].empty() || !queues_[1].empty() || !queues_[2].empty();
}

// ==================== 题目4: Yield 操作 ====================

bool Yield::await_ready() noexcept {
    return false;  // 总是挂起
}

void Yield::await_suspend(std::coroutine_handle<> handle) noexcept {
    // TODO: 将当前协程重新入队
}

void Yield::await_resume() noexcept {}

void YieldScheduler::schedule(std::coroutine_handle<> handle) {
    if (handle && !handle.done()) {
        readyQueue_.push(handle);
    }
}

void YieldScheduler::run() {
    // TODO: 设置 current_ 并执行所有协程
}

Yield YieldScheduler::yield() {
    return Yield{this};
}

YieldScheduler* YieldScheduler::current() {
    return current_;
}

// ==================== 题目5: 公平调度器 ====================

FairScheduler::FairScheduler(int timeSliceMs)
    : timeSliceMs_(timeSliceMs) {}

void FairScheduler::schedule(std::coroutine_handle<> handle) {
    // TODO: 将协程加入队列
}

void FairScheduler::run() {
    // TODO: 使用时间片轮转执行协程
}

} // namespace SchedulerImpl
