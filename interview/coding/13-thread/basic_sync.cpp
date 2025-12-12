/**
 * @file basic_sync.cpp
 * @brief 基础同步原语 - 面试题
 *
 * 包含题目：
 * 1. mutex 互斥锁使用
 * 2. condition_variable 条件变量
 * 3. 实现简单的自旋锁
 * 4. 实现信号量
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

/**
 * 题目1：使用 mutex 保护共享数据
 *
 * 场景：多线程计数器
 */
class Counter {
public:
    // TODO: 实现线程安全的 increment
    void increment() {
    }

    // TODO: 实现线程安全的 decrement
    void decrement() {
    }

    // TODO: 实现 getValue
    int getValue() const {
        return 0;
    }

private:
    int value_ = 0;
    // TODO: 添加同步原语
};

/**
 * 题目2：使用条件变量实现等待/通知
 *
 * 场景：一个线程等待某个条件，另一个线程通知
 */
class Event {
public:
    // TODO: 等待事件
    void wait() {
    }

    // TODO: 触发事件
    void signal() {
    }

    // TODO: 触发所有等待者
    void signalAll() {
    }

    // TODO: 重置事件
    void reset() {
    }

private:
    // TODO: 添加成员变量
};

/**
 * 题目3：实现自旋锁
 *
 * 要求：
 * 1. 使用 atomic_flag
 * 2. 实现 lock() 和 unlock()
 */
class SpinLock {
public:
    // TODO: 实现 lock
    void lock() {
    }

    // TODO: 实现 unlock
    void unlock() {
    }

    // TODO: 实现 try_lock
    bool try_lock() {
        return false;
    }

private:
    // TODO: 添加成员变量
};

/**
 * 题目4：实现计数信号量
 *
 * 要求：
 * 1. 支持 acquire（P 操作）
 * 2. 支持 release（V 操作）
 */
class Semaphore {
public:
    explicit Semaphore(int count) {
        // TODO
    }

    // TODO: 实现 acquire
    void acquire() {
    }

    // TODO: 实现 release
    void release() {
    }

    // TODO: 实现 try_acquire
    bool try_acquire() {
        return false;
    }

private:
    // TODO: 添加成员变量
};

/**
 * 题目5：实现限时锁
 *
 * 场景：尝试获取锁，超时则放弃
 */
class TimedLock {
public:
    // TODO: 带超时的 lock
    template <typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout) {
        return false;
    }

    void unlock() {
    }

private:
    // TODO: 添加成员变量
};

int main() {
    return 0;
}
