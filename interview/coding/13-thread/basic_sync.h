/**
 * @file basic_sync.h
 * @brief 基础同步原语 - 声明
 *
 * 包含题目：
 * 1. mutex 互斥锁使用
 * 2. condition_variable 条件变量
 * 3. 实现简单的自旋锁
 * 4. 实现信号量
 */

#ifndef BASIC_SYNC_H
#define BASIC_SYNC_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>

namespace BasicSync {

// ==================== 面试者实现 ====================

/**
 * 题目1：使用 mutex 保护共享数据
 * 场景：多线程计数器
 */
class Counter {
public:
    void increment();
    void decrement();
    int getValue() const;

private:
    int value_ = 0;
    // TODO: 添加同步原语
};

/**
 * 题目2：使用条件变量实现等待/通知
 * 场景：一个线程等待某个条件，另一个线程通知
 */
class Event {
public:
    void wait();
    void signal();
    void signalAll();
    void reset();

private:
    // TODO: 添加成员变量
};

/**
 * 题目3：实现自旋锁
 * 要求：
 * 1. 使用 atomic_flag
 * 2. 实现 lock() 和 unlock()
 */
class SpinLock {
public:
    void lock();
    void unlock();
    bool try_lock();

private:
    // TODO: 添加成员变量
};

/**
 * 题目4：实现计数信号量
 * 要求：
 * 1. 支持 acquire（P 操作）
 * 2. 支持 release（V 操作）
 */
class Semaphore {
public:
    explicit Semaphore(int count);
    void acquire();
    void release();
    bool try_acquire();

private:
    // TODO: 添加成员变量
};

/**
 * 题目5：实现限时锁
 * 场景：尝试获取锁，超时则放弃
 */
class TimedLock {
public:
    template <typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout);
    void unlock();

private:
    // TODO: 添加成员变量
};

// ==================== 参考答案 ====================

class CounterSolution {
public:
    void increment();
    void decrement();
    int getValue() const;
    void incrementAtomic();
    int getAtomicValue() const;
    void reset();

private:
    int value_ = 0;
    mutable std::mutex mutex_;
    std::atomic<int> atomicValue_{0};
};

class EventSolution {
public:
    void wait();
    template <typename Rep, typename Period>
    bool waitFor(const std::chrono::duration<Rep, Period>& timeout);
    void signal();
    void signalAll();
    void reset();
    bool isSignaled() const;

private:
    bool signaled_ = false;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

class SpinLockSolution {
public:
    void lock();
    void unlock();
    bool try_lock();

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

class SemaphoreSolution {
public:
    explicit SemaphoreSolution(int count = 0);
    void acquire();
    void release();
    bool try_acquire();
    template <typename Rep, typename Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& timeout);
    int getCount() const;

private:
    int count_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

class BinarySemaphoreSolution {
public:
    explicit BinarySemaphoreSolution(bool initial = true);
    void acquire();
    void release();
    bool isAvailable() const;

private:
    bool available_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

class TimedLockSolution {
public:
    template <typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout);
    void lock();
    void unlock();
    bool try_lock();

private:
    std::timed_mutex mutex_;
};

template <typename Lockable>
class LockGuard {
public:
    explicit LockGuard(Lockable& lock);
    ~LockGuard();
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    Lockable& lock_;
};

// 测试函数
void runTests();

} // namespace BasicSync

#endif // BASIC_SYNC_H
