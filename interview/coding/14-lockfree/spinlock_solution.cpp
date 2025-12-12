/**
 * @file spinlock_solution.cpp
 * @brief 自旋锁 - 解答
 */
#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

// ==================== 题目1: 基本 TAS 自旋锁 ====================
class SpinLock {
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
            // 可选：添加 pause 指令减少功耗
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();  // 或 _mm_pause()
            #endif
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

    // 尝试获取锁，不阻塞
    bool try_lock() {
        return !flag_.test_and_set(std::memory_order_acquire);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

// ==================== 题目2: TTAS 自旋锁 ====================
// 先测试再 TAS，减少缓存一致性流量
class TTASSpinLock {
public:
    void lock() {
        while (true) {
            // 先只读检查（在本地缓存中）
            while (locked_.load(std::memory_order_relaxed)) {
                // 自旋，不产生缓存一致性流量
                std::this_thread::yield();
            }
            // 尝试获取（CAS 操作）
            bool expected = false;
            if (locked_.compare_exchange_weak(expected, true,
                                              std::memory_order_acquire,
                                              std::memory_order_relaxed)) {
                return;
            }
        }
    }

    void unlock() {
        locked_.store(false, std::memory_order_release);
    }

private:
    std::atomic<bool> locked_{false};
};

// ==================== 题目3: 带退避的自旋锁 ====================
class BackoffSpinLock {
public:
    void lock() {
        int backoff = MIN_BACKOFF;

        while (true) {
            // TTAS 检查
            while (locked_.load(std::memory_order_relaxed)) {
                std::this_thread::yield();
            }

            bool expected = false;
            if (locked_.compare_exchange_weak(expected, true,
                                              std::memory_order_acquire)) {
                return;
            }

            // 指数退避
            for (int i = 0; i < backoff; ++i) {
                std::this_thread::yield();
            }
            backoff = std::min(backoff * 2, MAX_BACKOFF);
        }
    }

    void unlock() {
        locked_.store(false, std::memory_order_release);
    }

private:
    static constexpr int MIN_BACKOFF = 1;
    static constexpr int MAX_BACKOFF = 1024;
    std::atomic<bool> locked_{false};
};

// ==================== 题目4: Ticket Lock ====================
// 保证公平性，按到达顺序获取锁
class TicketLock {
public:
    void lock() {
        // 获取票号
        unsigned myTicket = next_ticket_.fetch_add(1, std::memory_order_relaxed);
        // 等待叫号
        while (now_serving_.load(std::memory_order_acquire) != myTicket) {
            std::this_thread::yield();
        }
    }

    void unlock() {
        // 叫下一个号
        now_serving_.fetch_add(1, std::memory_order_release);
    }

private:
    std::atomic<unsigned> next_ticket_{0};
    std::atomic<unsigned> now_serving_{0};
};

// 带本地自旋优化的 Ticket Lock
class ProportionalTicketLock {
public:
    void lock() {
        unsigned myTicket = next_ticket_.fetch_add(1, std::memory_order_relaxed);
        while (true) {
            unsigned serving = now_serving_.load(std::memory_order_acquire);
            if (serving == myTicket) {
                return;
            }
            // 按距离比例退避
            unsigned distance = myTicket - serving;
            for (unsigned i = 0; i < distance * BACKOFF_FACTOR; ++i) {
                std::this_thread::yield();
            }
        }
    }

    void unlock() {
        now_serving_.fetch_add(1, std::memory_order_release);
    }

private:
    static constexpr unsigned BACKOFF_FACTOR = 10;
    std::atomic<unsigned> next_ticket_{0};
    std::atomic<unsigned> now_serving_{0};
};

// ==================== 题目5: 可重入自旋锁 ====================
class ReentrantSpinLock {
public:
    void lock() {
        std::thread::id self = std::this_thread::get_id();

        // 检查是否已经持有锁
        if (owner_.load(std::memory_order_relaxed) == self) {
            ++count_;
            return;
        }

        // 获取锁
        while (flag_.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        owner_.store(self, std::memory_order_relaxed);
        count_ = 1;
    }

    void unlock() {
        if (--count_ == 0) {
            owner_.store(std::thread::id{}, std::memory_order_relaxed);
            flag_.clear(std::memory_order_release);
        }
    }

    bool try_lock() {
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

private:
    std::atomic<std::thread::id> owner_{};
    int count_{0};
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

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

// ==================== 测试代码 ====================
template <typename Lock>
void benchmarkLock(const char* name, Lock& lock, int numThreads, int iterations) {
    std::atomic<long long> counter{0};

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&lock, &counter, iterations] {
            for (int j = 0; j < iterations; ++j) {
                LockGuard<Lock> guard(lock);
                ++counter;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << name << ": " << duration.count() << "ms, counter=" << counter << "\n";
}

int main() {
    const int NUM_THREADS = 4;
    const int ITERATIONS = 100000;

    std::cout << "=== Spinlock Benchmark ===\n";
    std::cout << "Threads: " << NUM_THREADS << ", Iterations: " << ITERATIONS << "\n\n";

    {
        SpinLock lock;
        benchmarkLock("TAS SpinLock", lock, NUM_THREADS, ITERATIONS);
    }

    {
        TTASSpinLock lock;
        benchmarkLock("TTAS SpinLock", lock, NUM_THREADS, ITERATIONS);
    }

    {
        BackoffSpinLock lock;
        benchmarkLock("Backoff SpinLock", lock, NUM_THREADS, ITERATIONS);
    }

    {
        TicketLock lock;
        benchmarkLock("Ticket Lock", lock, NUM_THREADS, ITERATIONS);
    }

    std::cout << "\n=== Reentrant SpinLock Test ===\n";
    {
        ReentrantSpinLock lock;
        lock.lock();
        std::cout << "First lock acquired\n";
        lock.lock();
        std::cout << "Second lock acquired (reentrant)\n";
        lock.unlock();
        std::cout << "First unlock\n";
        lock.unlock();
        std::cout << "Second unlock\n";
    }

    return 0;
}

/**
 * 关键要点：
 *
 * 1. TAS vs TTAS：
 *    - TAS: 每次循环都执行原子操作，产生大量缓存一致性流量
 *    - TTAS: 先只读检查，减少写操作，对缓存更友好
 *
 * 2. 退避策略：
 *    - 固定退避：简单但不适应负载变化
 *    - 指数退避：逐渐增加等待时间，减少竞争
 *    - 比例退避：根据等待距离调整
 *
 * 3. 公平性：
 *    - TAS/TTAS: 不公平，可能导致饥饿
 *    - Ticket Lock: FIFO 顺序，保证公平
 *    - 公平锁通常性能略低
 *
 * 4. 内存序选择：
 *    - acquire: 获取锁时，保证后续操作不会重排到前面
 *    - release: 释放锁时，保证之前操作不会重排到后面
 *    - relaxed: 不需要同步的辅助操作
 *
 * 5. 使用场景：
 *    - 临界区很短（几十到几百条指令）
 *    - 锁竞争不激烈
 *    - 不能休眠的上下文（如中断处理）
 *
 * 6. 注意事项：
 *    - 自旋锁不适合长临界区（浪费 CPU）
 *    - 持有自旋锁时不能调用可能阻塞的函数
 *    - 单核系统上自旋锁通常退化为禁止中断
 */
