/**
 * @file barrier_solution.cpp
 * @brief 屏障 (Barrier) - 解答
 */
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>

// ==================== 题目1: 基本屏障 ====================
class Barrier {
public:
    explicit Barrier(int count)
        : threshold_(count), count_(count), generation_(0) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;

        if (--count_ == 0) {
            // 最后一个到达的线程重置屏障
            ++generation_;
            count_ = threshold_;
            cv_.notify_all();
        } else {
            // 等待其他线程
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }

private:
    int threshold_;
    int count_;
    int generation_;  // 用于区分不同轮次
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 题目2: 带回调的屏障 ====================
class BarrierWithCallback {
public:
    BarrierWithCallback(int count, std::function<void()> callback)
        : threshold_(count), count_(count), generation_(0), callback_(callback) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;

        if (--count_ == 0) {
            // 最后一个线程执行回调
            if (callback_) {
                callback_();
            }
            ++generation_;
            count_ = threshold_;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }

private:
    int threshold_;
    int count_;
    int generation_;
    std::function<void()> callback_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 题目3: 可重置的屏障 ====================
class ResetableBarrier {
public:
    explicit ResetableBarrier(int count)
        : threshold_(count), count_(count), generation_(0) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;

        if (--count_ == 0) {
            ++generation_;
            count_ = threshold_;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }

    void reset(int newCount) {
        std::lock_guard<std::mutex> lock(mutex_);
        threshold_ = newCount;
        count_ = newCount;
        ++generation_;  // 唤醒所有等待的线程
        cv_.notify_all();
    }

private:
    int threshold_;
    int count_;
    int generation_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 题目4: CountDownLatch ====================
class CountDownLatch {
public:
    explicit CountDownLatch(int count) : count_(count) {}

    void countDown() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (count_ > 0) {
            --count_;
            if (count_ == 0) {
                cv_.notify_all();
            }
        }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ == 0; });
    }

    // 带超时的等待
    bool wait(int timeoutMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [this] { return count_ == 0; });
    }

    int getCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};

// ==================== 题目5: CyclicBarrier ====================
class CyclicBarrier {
public:
    explicit CyclicBarrier(int parties)
        : parties_(parties), count_(parties), generation_(0), broken_(false) {}

    int await() {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;
        int index = --count_;

        if (broken_) {
            throw std::runtime_error("Barrier is broken");
        }

        if (count_ == 0) {
            // 最后一个到达
            ++generation_;
            count_ = parties_;
            cv_.notify_all();
            return 0;
        } else {
            cv_.wait(lock, [this, gen] {
                return gen != generation_ || broken_;
            });
            if (broken_) {
                throw std::runtime_error("Barrier is broken");
            }
            return parties_ - index;
        }
    }

    int await(int timeoutMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        int gen = generation_;
        int index = --count_;

        if (broken_) {
            return -1;
        }

        if (count_ == 0) {
            ++generation_;
            count_ = parties_;
            cv_.notify_all();
            return 0;
        } else {
            bool success = cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                        [this, gen] { return gen != generation_ || broken_; });
            if (!success || broken_) {
                broken_ = true;
                cv_.notify_all();
                return -1;
            }
            return parties_ - index;
        }
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        broken_ = false;
        ++generation_;
        count_ = parties_;
        cv_.notify_all();
    }

    int getNumberWaiting() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return parties_ - count_;
    }

    bool isBroken() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return broken_;
    }

private:
    int parties_;
    int count_;
    int generation_;
    bool broken_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== C++20 std::barrier 示例 ====================
#if __cplusplus >= 202002L
#include <barrier>

void testStdBarrier() {
    auto onCompletion = []() noexcept {
        std::cout << "All threads reached barrier\n";
    };

    std::barrier barrier(3, onCompletion);

    auto worker = [&barrier](int id) {
        for (int i = 0; i < 2; ++i) {
            std::cout << "Thread " << id << " phase " << i << "\n";
            barrier.arrive_and_wait();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) {
        t.join();
    }
}
#endif

// ==================== 测试代码 ====================
int main() {
    std::cout << "=== Test 1: Basic Barrier ===\n";
    {
        Barrier barrier(3);
        auto worker = [&barrier](int id) {
            std::cout << "Thread " << id << " before barrier\n";
            barrier.wait();
            std::cout << "Thread " << id << " after barrier\n";
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    std::cout << "\n=== Test 2: Barrier with Callback ===\n";
    {
        BarrierWithCallback barrier(3, [] {
            std::cout << ">>> Callback: All threads arrived! <<<\n";
        });

        auto worker = [&barrier](int id) {
            std::this_thread::sleep_for(std::chrono::milliseconds(id * 100));
            std::cout << "Thread " << id << " arriving\n";
            barrier.wait();
            std::cout << "Thread " << id << " continuing\n";
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    std::cout << "\n=== Test 3: CountDownLatch ===\n";
    {
        CountDownLatch latch(3);

        // 主线程等待
        std::thread waiter([&latch] {
            std::cout << "Main thread waiting...\n";
            latch.wait();
            std::cout << "Main thread: All workers done!\n";
        });

        // 工作线程
        std::vector<std::thread> workers;
        for (int i = 0; i < 3; ++i) {
            workers.emplace_back([&latch, i] {
                std::this_thread::sleep_for(std::chrono::milliseconds((i + 1) * 100));
                std::cout << "Worker " << i << " done\n";
                latch.countDown();
            });
        }

        waiter.join();
        for (auto& w : workers) {
            w.join();
        }
    }

    std::cout << "\n=== Test 4: CyclicBarrier (multiple phases) ===\n";
    {
        CyclicBarrier barrier(3);

        auto worker = [&barrier](int id) {
            for (int phase = 0; phase < 2; ++phase) {
                std::cout << "Thread " << id << " phase " << phase << " start\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(id * 50));
                int arrivalIndex = barrier.await();
                std::cout << "Thread " << id << " phase " << phase
                          << " done (arrival: " << arrivalIndex << ")\n";
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    return 0;
}

/**
 * 关键要点：
 *
 * 1. Barrier vs CountDownLatch：
 *    - Barrier: 可重用，所有线程互相等待
 *    - CountDownLatch: 一次性，主线程等待工作线程
 *
 * 2. Generation 计数：
 *    - 用于区分不同轮次的等待
 *    - 避免虚假唤醒导致的问题
 *    - 每次屏障释放时递增
 *
 * 3. 回调机制：
 *    - 由最后一个到达的线程执行
 *    - 常用于阶段完成时的聚合操作
 *
 * 4. 超时处理：
 *    - 使用 wait_for 实现
 *    - 超时时需要标记 broken 状态
 *    - 通知其他等待线程
 *
 * 5. C++20 std::barrier：
 *    - 标准库提供的屏障实现
 *    - 支持完成回调
 *    - 更高效的实现
 *
 * 6. 应用场景：
 *    - 并行计算的阶段同步
 *    - 多线程测试的同步启动
 *    - 游戏中的帧同步
 */
