/**
 * @file spinlock.cpp
 * @brief 自旋锁 - 面试题
 *
 * 自旋锁是无锁编程的基础，适用于临界区很短的场景。
 */
#include <atomic>
#include <thread>

// 题目1: 实现基本的自旋锁
// 使用 atomic_flag 实现 TAS (Test-And-Set) 自旋锁
class SpinLock {
public:
    void lock() { /* TODO */ }
    void unlock() { /* TODO */ }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

// 题目2: 实现 TTAS (Test-Test-And-Set) 自旋锁
// 先检查再 TAS，减少缓存一致性流量
class TTASSpinLock {
public:
    void lock() { /* TODO */ }
    void unlock() { /* TODO */ }

private:
    std::atomic<bool> locked_{false};
};

// 题目3: 实现带退避的自旋锁
// 使用指数退避减少竞争
class BackoffSpinLock {
public:
    void lock() { /* TODO */ }
    void unlock() { /* TODO */ }

private:
    std::atomic<bool> locked_{false};
};

// 题目4: 实现 Ticket Lock
// 保证公平性（FIFO 顺序）
class TicketLock {
public:
    void lock() { /* TODO */ }
    void unlock() { /* TODO */ }

private:
    std::atomic<unsigned> next_ticket_{0};
    std::atomic<unsigned> now_serving_{0};
};

// 题目5: 实现可重入自旋锁
// 同一线程可以多次获取锁
class ReentrantSpinLock {
public:
    void lock() { /* TODO */ }
    void unlock() { /* TODO */ }

private:
    std::atomic<std::thread::id> owner_{};
    int count_{0};
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

int main() {
    return 0;
}
