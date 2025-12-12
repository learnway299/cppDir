/**
 * @file basic_sync_solution.cpp
 * @brief 基础同步原语 - 解答
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>

/**
 * 题目1：线程安全计数器
 */
class Counter {
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++value_;
    }

    void decrement() {
        std::lock_guard<std::mutex> lock(mutex_);
        --value_;
    }

    int getValue() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }

    // 原子操作版本（更高效）
    void incrementAtomic() {
        atomicValue_.fetch_add(1, std::memory_order_relaxed);
    }

    int getAtomicValue() const {
        return atomicValue_.load(std::memory_order_relaxed);
    }

private:
    int value_ = 0;
    mutable std::mutex mutex_;
    std::atomic<int> atomicValue_{0};
};

/**
 * 题目2：事件（条件变量封装）
 */
class Event {
public:
    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return signaled_; });
    }

    template <typename Rep, typename Period>
    bool waitFor(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, timeout, [this] { return signaled_; });
    }

    void signal() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            signaled_ = true;
        }
        cv_.notify_one();
    }

    void signalAll() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            signaled_ = true;
        }
        cv_.notify_all();
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        signaled_ = false;
    }

    bool isSignaled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return signaled_;
    }

private:
    bool signaled_ = false;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

/**
 * 题目3：自旋锁
 *
 * 适用场景：锁持有时间很短，不想进入内核
 */
class SpinLock {
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
            // 可添加 yield 或 pause 指令优化
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

    bool try_lock() {
        return !flag_.test_and_set(std::memory_order_acquire);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

// 带退避的自旋锁（更高效）
class SpinLockWithBackoff {
public:
    void lock() {
        int backoff = 1;
        while (flag_.test_and_set(std::memory_order_acquire)) {
            for (int i = 0; i < backoff; ++i) {
                // CPU pause 指令（减少功耗和争用）
                #if defined(__x86_64__) || defined(_M_X64)
                __builtin_ia32_pause();
                #endif
            }
            backoff = std::min(backoff * 2, 1024);
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

/**
 * 题目4：计数信号量
 */
class Semaphore {
public:
    explicit Semaphore(int count = 0) : count_(count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }

    void release() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++count_;
        }
        cv_.notify_one();
    }

    bool try_acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ > 0) {
            --count_;
            return true;
        }
        return false;
    }

    template <typename Rep, typename Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (cv_.wait_for(lock, timeout, [this] { return count_ > 0; })) {
            --count_;
            return true;
        }
        return false;
    }

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// 二元信号量（等价于 mutex）
class BinarySemaphore {
public:
    BinarySemaphore(bool initial = true) : available_(initial) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return available_; });
        available_ = false;
    }

    void release() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            available_ = true;
        }
        cv_.notify_one();
    }

private:
    bool available_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

/**
 * 题目5：限时锁
 */
class TimedLock {
public:
    template <typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout) {
        return mutex_.try_lock_for(timeout);
    }

    void lock() {
        mutex_.lock();
    }

    void unlock() {
        mutex_.unlock();
    }

    bool try_lock() {
        return mutex_.try_lock();
    }

private:
    std::timed_mutex mutex_;
};

/**
 * 扩展：RAII 锁守卫
 */
template <typename Lockable>
class LockGuard {
public:
    explicit LockGuard(Lockable& lock) : lock_(lock) {
        lock_.lock();
    }

    ~LockGuard() {
        lock_.unlock();
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    Lockable& lock_;
};

int main() {
    std::cout << "=== 测试线程安全计数器 ===\n";
    {
        Counter counter;
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&counter] {
                for (int j = 0; j < 1000; ++j) {
                    counter.increment();
                }
            });
        }

        for (auto& t : threads) t.join();
        std::cout << "Counter value: " << counter.getValue() << " (expected 10000)\n";
    }

    std::cout << "\n=== 测试事件 ===\n";
    {
        Event event;

        std::thread waiter([&event] {
            std::cout << "Waiter: waiting for event...\n";
            event.wait();
            std::cout << "Waiter: event received!\n";
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Main: signaling event\n";
        event.signal();

        waiter.join();
    }

    std::cout << "\n=== 测试自旋锁 ===\n";
    {
        SpinLock spinlock;
        int sharedData = 0;
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&spinlock, &sharedData] {
                for (int j = 0; j < 1000; ++j) {
                    LockGuard<SpinLock> guard(spinlock);
                    ++sharedData;
                }
            });
        }

        for (auto& t : threads) t.join();
        std::cout << "SpinLock result: " << sharedData << " (expected 10000)\n";
    }

    std::cout << "\n=== 测试信号量 ===\n";
    {
        Semaphore sem(3);  // 最多 3 个并发
        std::atomic<int> concurrent{0};
        std::atomic<int> maxConcurrent{0};

        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&sem, &concurrent, &maxConcurrent, i] {
                sem.acquire();
                int c = ++concurrent;
                int expected = maxConcurrent.load();
                while (c > expected && !maxConcurrent.compare_exchange_weak(expected, c));

                std::cout << "Thread " << i << " acquired (concurrent: " << c << ")\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                --concurrent;
                sem.release();
            });
        }

        for (auto& t : threads) t.join();
        std::cout << "Max concurrent: " << maxConcurrent << " (expected <= 3)\n";
    }

    std::cout << "\n=== 测试限时锁 ===\n";
    {
        TimedLock lock;

        std::thread holder([&lock] {
            lock.lock();
            std::cout << "Holder: acquired lock\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            lock.unlock();
            std::cout << "Holder: released lock\n";
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::thread tryLocker([&lock] {
            std::cout << "TryLocker: trying to acquire...\n";
            if (lock.try_lock_for(std::chrono::milliseconds(100))) {
                std::cout << "TryLocker: acquired!\n";
                lock.unlock();
            } else {
                std::cout << "TryLocker: timeout!\n";
            }
        });

        holder.join();
        tryLocker.join();
    }

    return 0;
}
