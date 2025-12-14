/**
 * @file dining_philosophers_solution.cpp
 * @brief 哲学家就餐问题 - 参考答案
 */

#include "dining_philosophers.h"
#include <iostream>
#include <chrono>
#include <atomic>
#include <cassert>

namespace DiningPhilosophers {

// ==================== 参考答案实现 ====================

// 解法1: 奇偶编号策略
DiningPhilosophersOddEvenSolution::DiningPhilosophersOddEvenSolution() : forks_(5) {}

void DiningPhilosophersOddEvenSolution::wantsToEat(int philosopher,
                                                     std::function<void()> pickLeftFork,
                                                     std::function<void()> pickRightFork,
                                                     std::function<void()> eat,
                                                     std::function<void()> putLeftFork,
                                                     std::function<void()> putRightFork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;

    if (philosopher % 2 == 0) {
        forks_[left].lock();
        pickLeftFork();
        forks_[right].lock();
        pickRightFork();
    } else {
        forks_[right].lock();
        pickRightFork();
        forks_[left].lock();
        pickLeftFork();
    }

    eat();

    putLeftFork();
    forks_[left].unlock();
    putRightFork();
    forks_[right].unlock();
}

// 解法2: 限制并发数
DiningPhilosophersLimitedSolution::DiningPhilosophersLimitedSolution() : forks_(5), seats_(4) {}

void DiningPhilosophersLimitedSolution::wantsToEat(int philosopher,
                                                     std::function<void()> pickLeftFork,
                                                     std::function<void()> pickRightFork,
                                                     std::function<void()> eat,
                                                     std::function<void()> putLeftFork,
                                                     std::function<void()> putRightFork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;

    {
        std::unique_lock<std::mutex> lock(seatMutex_);
        seatCv_.wait(lock, [this] { return seats_ > 0; });
        --seats_;
    }

    forks_[left].lock();
    pickLeftFork();
    forks_[right].lock();
    pickRightFork();

    eat();

    putRightFork();
    forks_[right].unlock();
    putLeftFork();
    forks_[left].unlock();

    {
        std::lock_guard<std::mutex> lock(seatMutex_);
        ++seats_;
    }
    seatCv_.notify_one();
}

// 解法3: 条件变量（原子获取两只筷子）
DiningPhilosophersCVSolution::DiningPhilosophersCVSolution() : forkAvailable_(5, true) {}

void DiningPhilosophersCVSolution::wantsToEat(int philosopher,
                                                std::function<void()> pickLeftFork,
                                                std::function<void()> pickRightFork,
                                                std::function<void()> eat,
                                                std::function<void()> putLeftFork,
                                                std::function<void()> putRightFork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this, left, right] {
            return forkAvailable_[left] && forkAvailable_[right];
        });
        forkAvailable_[left] = false;
        forkAvailable_[right] = false;
    }

    pickLeftFork();
    pickRightFork();
    eat();
    putLeftFork();
    putRightFork();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        forkAvailable_[left] = true;
        forkAvailable_[right] = true;
    }
    cv_.notify_all();
}

// 解法4: std::lock
DiningPhilosophersStdLockSolution::DiningPhilosophersStdLockSolution() : forks_(5) {}

void DiningPhilosophersStdLockSolution::wantsToEat(int philosopher,
                                                     std::function<void()> pickLeftFork,
                                                     std::function<void()> pickRightFork,
                                                     std::function<void()> eat,
                                                     std::function<void()> putLeftFork,
                                                     std::function<void()> putRightFork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;

    std::lock(forks_[left], forks_[right]);
    std::lock_guard<std::mutex> lgLeft(forks_[left], std::adopt_lock);
    std::lock_guard<std::mutex> lgRight(forks_[right], std::adopt_lock);

    pickLeftFork();
    pickRightFork();
    eat();
    putLeftFork();
    putRightFork();
}

// 解法5: scoped_lock
DiningPhilosophersScopedLockSolution::DiningPhilosophersScopedLockSolution() : forks_(5) {}

void DiningPhilosophersScopedLockSolution::wantsToEat(int philosopher,
                                                        std::function<void()> pickLeftFork,
                                                        std::function<void()> pickRightFork,
                                                        std::function<void()> eat,
                                                        std::function<void()> putLeftFork,
                                                        std::function<void()> putRightFork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;

    std::scoped_lock lock(forks_[left], forks_[right]);

    pickLeftFork();
    pickRightFork();
    eat();
    putLeftFork();
    putRightFork();
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Dining Philosophers Tests ===" << std::endl;

    // 测试奇偶策略
    {
        DiningPhilosophersOddEvenSolution dp;
        std::atomic<int> eatCount{0};

        auto philosopher = [&dp, &eatCount](int id) {
            for (int i = 0; i < 3; ++i) {
                dp.wantsToEat(id, [] {}, [] {}, [&eatCount] { ++eatCount; }, [] {}, [] {});
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back(philosopher, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        assert(eatCount == 15);
    }
    std::cout << "  OddEven Strategy: PASSED" << std::endl;

    // 测试限制并发策略
    {
        DiningPhilosophersLimitedSolution dp;
        std::atomic<int> eatCount{0};

        auto philosopher = [&dp, &eatCount](int id) {
            for (int i = 0; i < 3; ++i) {
                dp.wantsToEat(id, [] {}, [] {}, [&eatCount] { ++eatCount; }, [] {}, [] {});
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back(philosopher, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        assert(eatCount == 15);
    }
    std::cout << "  Limited Seats Strategy: PASSED" << std::endl;

    // 测试条件变量策略
    {
        DiningPhilosophersCVSolution dp;
        std::atomic<int> eatCount{0};

        auto philosopher = [&dp, &eatCount](int id) {
            for (int i = 0; i < 3; ++i) {
                dp.wantsToEat(id, [] {}, [] {}, [&eatCount] { ++eatCount; }, [] {}, [] {});
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back(philosopher, i);
        }
        for (auto& t : threads) {
            t.join();
        }
        assert(eatCount == 15);
    }
    std::cout << "  Condition Variable Strategy: PASSED" << std::endl;
}

} // namespace DiningPhilosophers
