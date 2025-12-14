/**
 * @file spinlock_solution.cpp
 * @brief 自旋锁 - 参考答案
 */
#include "spinlock.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

namespace Spinlock {

// ==================== 题目1: 基本 TAS 自旋锁 ====================

void SpinLockSolution::lock() {
    while (flag_.test_and_set(std::memory_order_acquire)) {
        // 自旋等待
    }
}

void SpinLockSolution::unlock() {
    flag_.clear(std::memory_order_release);
}

bool SpinLockSolution::try_lock() {
    return !flag_.test_and_set(std::memory_order_acquire);
}

// ==================== 题目2: TTAS 自旋锁 ====================

void TTASSpinLockSolution::lock() {
    while (true) {
        while (locked_.load(std::memory_order_relaxed)) {
            std::this_thread::yield();
        }
        bool expected = false;
        if (locked_.compare_exchange_weak(expected, true,
                                          std::memory_order_acquire,
                                          std::memory_order_relaxed)) {
            return;
        }
    }
}

void TTASSpinLockSolution::unlock() {
    locked_.store(false, std::memory_order_release);
}

// ==================== 题目3: 带退避的自旋锁 ====================

void BackoffSpinLockSolution::lock() {
    static constexpr int MIN_BACKOFF = 1;
    static constexpr int MAX_BACKOFF = 1024;

    int backoff = MIN_BACKOFF;

    while (true) {
        while (locked_.load(std::memory_order_relaxed)) {
            std::this_thread::yield();
        }

        bool expected = false;
        if (locked_.compare_exchange_weak(expected, true,
                                          std::memory_order_acquire)) {
            return;
        }

        for (int i = 0; i < backoff; ++i) {
            std::this_thread::yield();
        }
        backoff = std::min(backoff * 2, MAX_BACKOFF);
    }
}

void BackoffSpinLockSolution::unlock() {
    locked_.store(false, std::memory_order_release);
}

// ==================== 题目4: Ticket Lock ====================

void TicketLockSolution::lock() {
    unsigned myTicket = next_ticket_.fetch_add(1, std::memory_order_relaxed);
    while (now_serving_.load(std::memory_order_acquire) != myTicket) {
        std::this_thread::yield();
    }
}

void TicketLockSolution::unlock() {
    now_serving_.fetch_add(1, std::memory_order_release);
}

// ==================== 题目5: 可重入自旋锁 ====================

void ReentrantSpinLockSolution::lock() {
    std::thread::id self = std::this_thread::get_id();

    if (owner_.load(std::memory_order_relaxed) == self) {
        ++count_;
        return;
    }

    while (flag_.test_and_set(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    owner_.store(self, std::memory_order_relaxed);
    count_ = 1;
}

void ReentrantSpinLockSolution::unlock() {
    if (--count_ == 0) {
        owner_.store(std::thread::id{}, std::memory_order_relaxed);
        flag_.clear(std::memory_order_release);
    }
}

bool ReentrantSpinLockSolution::try_lock() {
    std::thread::id self = std::this_thread::get_id();

    if (owner_.load(std::memory_order_relaxed) == self) {
        ++count_;
        return true;
    }

    if (!flag_.test_and_set(std::memory_order_acquire)) {
        owner_.store(self, std::memory_order_relaxed);
        count_ = 1;
        return true;
    }
    return false;
}

// ==================== RAII 包装 ====================

template <typename Lock>
class LockGuard {
public:
    explicit LockGuard(Lock& lock) : lock_(lock) { lock_.lock(); }
    ~LockGuard() { lock_.unlock(); }
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
private:
    Lock& lock_;
};

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Spinlock Tests ===" << std::endl;

    // 测试基本自旋锁
    {
        SpinLockSolution lock;
        std::atomic<int> counter{0};
        std::vector<std::thread> threads;

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&lock, &counter] {
                for (int j = 0; j < 1000; ++j) {
                    LockGuard<SpinLockSolution> guard(lock);
                    ++counter;
                }
            });
        }

        for (auto& t : threads) t.join();
        assert(counter == 4000);
    }
    std::cout << "  TAS SpinLock: PASSED" << std::endl;

    // 测试 TTAS 自旋锁
    {
        TTASSpinLockSolution lock;
        std::atomic<int> counter{0};
        std::vector<std::thread> threads;

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&lock, &counter] {
                for (int j = 0; j < 1000; ++j) {
                    LockGuard<TTASSpinLockSolution> guard(lock);
                    ++counter;
                }
            });
        }

        for (auto& t : threads) t.join();
        assert(counter == 4000);
    }
    std::cout << "  TTAS SpinLock: PASSED" << std::endl;

    // 测试 Ticket Lock
    {
        TicketLockSolution lock;
        std::atomic<int> counter{0};
        std::vector<std::thread> threads;

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&lock, &counter] {
                for (int j = 0; j < 1000; ++j) {
                    LockGuard<TicketLockSolution> guard(lock);
                    ++counter;
                }
            });
        }

        for (auto& t : threads) t.join();
        assert(counter == 4000);
    }
    std::cout << "  Ticket Lock: PASSED" << std::endl;

    // 测试可重入自旋锁
    {
        ReentrantSpinLockSolution lock;

        lock.lock();
        lock.lock();  // 重入
        lock.unlock();
        lock.unlock();

        assert(lock.try_lock());
        assert(lock.try_lock());  // 重入
        lock.unlock();
        lock.unlock();
    }
    std::cout << "  Reentrant SpinLock: PASSED" << std::endl;

    std::cout << "=== All Spinlock Tests Passed ===" << std::endl;
}

} // namespace Spinlock
