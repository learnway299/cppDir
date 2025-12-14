/**
 * @file basic_sync_solution.cpp
 * @brief 基础同步原语 - 参考答案
 */

#include "basic_sync.h"
#include <cassert>

namespace BasicSync {

// ==================== 参考答案实现 ====================

// 题目1: 线程安全计数器
void CounterSolution::increment() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++value_;
}

void CounterSolution::decrement() {
    std::lock_guard<std::mutex> lock(mutex_);
    --value_;
}

int CounterSolution::getValue() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return value_;
}

void CounterSolution::incrementAtomic() {
    atomicValue_.fetch_add(1, std::memory_order_relaxed);
}

int CounterSolution::getAtomicValue() const {
    return atomicValue_.load(std::memory_order_relaxed);
}

void CounterSolution::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    value_ = 0;
    atomicValue_.store(0, std::memory_order_relaxed);
}

// 题目2: 事件
void EventSolution::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return signaled_; });
}

template <typename Rep, typename Period>
bool EventSolution::waitFor(const std::chrono::duration<Rep, Period>& timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    return cv_.wait_for(lock, timeout, [this] { return signaled_; });
}

void EventSolution::signal() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        signaled_ = true;
    }
    cv_.notify_one();
}

void EventSolution::signalAll() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        signaled_ = true;
    }
    cv_.notify_all();
}

void EventSolution::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    signaled_ = false;
}

bool EventSolution::isSignaled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return signaled_;
}

// 题目3: 自旋锁
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

// 题目4: 计数信号量
SemaphoreSolution::SemaphoreSolution(int count) : count_(count) {}

void SemaphoreSolution::acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return count_ > 0; });
    --count_;
}

void SemaphoreSolution::release() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
    }
    cv_.notify_one();
}

bool SemaphoreSolution::try_acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (count_ > 0) {
        --count_;
        return true;
    }
    return false;
}

template <typename Rep, typename Period>
bool SemaphoreSolution::try_acquire_for(const std::chrono::duration<Rep, Period>& timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (cv_.wait_for(lock, timeout, [this] { return count_ > 0; })) {
        --count_;
        return true;
    }
    return false;
}

int SemaphoreSolution::getCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

// 二元信号量
BinarySemaphoreSolution::BinarySemaphoreSolution(bool initial) : available_(initial) {}

void BinarySemaphoreSolution::acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return available_; });
    available_ = false;
}

void BinarySemaphoreSolution::release() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        available_ = true;
    }
    cv_.notify_one();
}

bool BinarySemaphoreSolution::isAvailable() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return available_;
}

// 题目5: 限时锁
template <typename Rep, typename Period>
bool TimedLockSolution::try_lock_for(const std::chrono::duration<Rep, Period>& timeout) {
    return mutex_.try_lock_for(timeout);
}

void TimedLockSolution::lock() {
    mutex_.lock();
}

void TimedLockSolution::unlock() {
    mutex_.unlock();
}

bool TimedLockSolution::try_lock() {
    return mutex_.try_lock();
}

// RAII 锁守卫
template <typename Lockable>
LockGuard<Lockable>::LockGuard(Lockable& lock) : lock_(lock) {
    lock_.lock();
}

template <typename Lockable>
LockGuard<Lockable>::~LockGuard() {
    lock_.unlock();
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Basic Sync Tests ===" << std::endl;

    // 测试线程安全计数器
    {
        CounterSolution counter;
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&counter] {
                for (int j = 0; j < 1000; ++j) {
                    counter.increment();
                }
            });
        }

        for (auto& t : threads) t.join();
        assert(counter.getValue() == 10000);
    }
    std::cout << "  Counter: PASSED" << std::endl;

    // 测试事件
    {
        EventSolution event;
        bool received = false;

        std::thread waiter([&event, &received] {
            event.wait();
            received = true;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        assert(!received);

        event.signal();
        waiter.join();
        assert(received);
        assert(event.isSignaled());

        event.reset();
        assert(!event.isSignaled());
    }
    std::cout << "  Event: PASSED" << std::endl;

    // 测试自旋锁
    {
        SpinLockSolution spinlock;
        int sharedData = 0;
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&spinlock, &sharedData] {
                for (int j = 0; j < 1000; ++j) {
                    LockGuard<SpinLockSolution> guard(spinlock);
                    ++sharedData;
                }
            });
        }

        for (auto& t : threads) t.join();
        assert(sharedData == 10000);
    }
    std::cout << "  SpinLock: PASSED" << std::endl;

    // 测试信号量
    {
        SemaphoreSolution sem(3);
        std::atomic<int> maxConcurrent{0};
        std::atomic<int> concurrent{0};
        std::vector<std::thread> threads;

        for (int i = 0; i < 6; ++i) {
            threads.emplace_back([&sem, &concurrent, &maxConcurrent] {
                sem.acquire();
                int c = ++concurrent;
                int expected = maxConcurrent.load();
                while (c > expected && !maxConcurrent.compare_exchange_weak(expected, c));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                --concurrent;
                sem.release();
            });
        }

        for (auto& t : threads) t.join();
        assert(maxConcurrent <= 3);
    }
    std::cout << "  Semaphore: PASSED" << std::endl;

    // 测试二元信号量
    {
        BinarySemaphoreSolution bsem(true);
        assert(bsem.isAvailable());

        bsem.acquire();
        assert(!bsem.isAvailable());

        bsem.release();
        assert(bsem.isAvailable());
    }
    std::cout << "  BinarySemaphore: PASSED" << std::endl;

    // 测试限时锁
    {
        TimedLockSolution lock;
        lock.lock();

        std::atomic<bool> acquired{false};
        std::thread tryLocker([&lock, &acquired] {
            acquired = lock.try_lock_for(std::chrono::milliseconds(10));
        });

        tryLocker.join();
        assert(!acquired);

        lock.unlock();

        bool success = lock.try_lock_for(std::chrono::milliseconds(100));
        assert(success);
        lock.unlock();
    }
    std::cout << "  TimedLock: PASSED" << std::endl;
}

} // namespace BasicSync
