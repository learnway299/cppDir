/**
 * @file barrier_solution.cpp
 * @brief 屏障 (Barrier) - 参考答案
 */

#include "barrier.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <cassert>

namespace Barrier {

// ==================== 参考答案实现 ====================

// 题目1: 基本屏障
BarrierSolution::BarrierSolution(int count)
    : threshold_(count), count_(count), generation_(0) {}

void BarrierSolution::wait() {
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

// 题目2: 带回调的屏障
BarrierWithCallbackSolution::BarrierWithCallbackSolution(int count, std::function<void()> callback)
    : threshold_(count), count_(count), generation_(0), callback_(callback) {}

void BarrierWithCallbackSolution::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    int gen = generation_;

    if (--count_ == 0) {
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

// 题目3: 可重置的屏障
ResetableBarrierSolution::ResetableBarrierSolution(int count)
    : threshold_(count), count_(count), generation_(0) {}

void ResetableBarrierSolution::wait() {
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

void ResetableBarrierSolution::reset(int newCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    threshold_ = newCount;
    count_ = newCount;
    ++generation_;
    cv_.notify_all();
}

// 题目4: CountDownLatch
CountDownLatchSolution::CountDownLatchSolution(int count) : count_(count) {}

void CountDownLatchSolution::countDown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (count_ > 0) {
        --count_;
        if (count_ == 0) {
            cv_.notify_all();
        }
    }
}

void CountDownLatchSolution::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return count_ == 0; });
}

bool CountDownLatchSolution::wait(int timeoutMs) {
    std::unique_lock<std::mutex> lock(mutex_);
    return cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                       [this] { return count_ == 0; });
}

int CountDownLatchSolution::getCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

// 题目5: CyclicBarrier
CyclicBarrierSolution::CyclicBarrierSolution(int parties)
    : parties_(parties), count_(parties), generation_(0), broken_(false) {}

int CyclicBarrierSolution::await() {
    std::unique_lock<std::mutex> lock(mutex_);
    int gen = generation_;
    int index = --count_;

    if (broken_) {
        throw std::runtime_error("Barrier is broken");
    }

    if (count_ == 0) {
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

int CyclicBarrierSolution::await(int timeoutMs) {
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

void CyclicBarrierSolution::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    broken_ = false;
    ++generation_;
    count_ = parties_;
    cv_.notify_all();
}

int CyclicBarrierSolution::getNumberWaiting() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return parties_ - count_;
}

bool CyclicBarrierSolution::isBroken() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return broken_;
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Barrier Tests ===" << std::endl;

    // 测试基本屏障
    {
        BarrierSolution barrier(3);
        std::atomic<int> counter{0};

        auto worker = [&barrier, &counter](int) {
            ++counter;
            barrier.wait();
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        assert(counter == 3);
    }
    std::cout << "  Basic Barrier: PASSED" << std::endl;

    // 测试带回调的屏障
    {
        std::atomic<int> callbackCount{0};
        BarrierWithCallbackSolution barrier(3, [&callbackCount] {
            ++callbackCount;
        });

        auto worker = [&barrier](int) {
            barrier.wait();
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        assert(callbackCount == 1);
    }
    std::cout << "  Barrier with Callback: PASSED" << std::endl;

    // 测试 CountDownLatch
    {
        CountDownLatchSolution latch(3);
        std::atomic<int> workersDone{0};

        std::thread waiter([&latch, &workersDone] {
            latch.wait();
            assert(workersDone == 3);
        });

        std::vector<std::thread> workers;
        for (int i = 0; i < 3; ++i) {
            workers.emplace_back([&latch, &workersDone] {
                ++workersDone;
                latch.countDown();
            });
        }

        waiter.join();
        for (auto& w : workers) {
            w.join();
        }
    }
    std::cout << "  CountDownLatch: PASSED" << std::endl;

    // 测试 CyclicBarrier (多阶段)
    {
        CyclicBarrierSolution barrier(3);
        std::atomic<int> phaseCount{0};

        auto worker = [&barrier, &phaseCount](int) {
            for (int phase = 0; phase < 2; ++phase) {
                ++phaseCount;
                barrier.await();
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 3; ++i) {
            threads.emplace_back(worker, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        assert(phaseCount == 6);
    }
    std::cout << "  CyclicBarrier: PASSED" << std::endl;
}

} // namespace Barrier
