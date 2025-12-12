/**
 * @file dining_philosophers_solution.cpp
 * @brief 哲学家就餐问题 - 解答
 */
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>
#include <iostream>
#include <semaphore>
#include <chrono>
#include <array>

// ==================== 解法1: 资源分级（奇偶编号策略）====================
// 奇数编号哲学家先拿左筷子，偶数编号先拿右筷子
// 这打破了循环等待条件，避免死锁
class DiningPhilosophersOddEven {
public:
    DiningPhilosophersOddEven() : forks_(5) {}

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % 5;

        if (philosopher % 2 == 0) {
            // 偶数：先拿左，后拿右
            forks_[left].lock();
            pickLeftFork();
            forks_[right].lock();
            pickRightFork();
        } else {
            // 奇数：先拿右，后拿左
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

private:
    std::vector<std::mutex> forks_;
};

// ==================== 解法2: 限制并发数 ====================
// 最多允许4个哲学家同时尝试拿筷子
// 使用信号量或计数器实现
class DiningPhilosophersLimited {
public:
    DiningPhilosophersLimited() : forks_(5), seats_(4) {}

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % 5;

        // 获取就餐许可（最多4人）
        {
            std::unique_lock<std::mutex> lock(seatMutex_);
            seatCv_.wait(lock, [this] { return seats_ > 0; });
            --seats_;
        }

        // 拿起筷子
        forks_[left].lock();
        pickLeftFork();
        forks_[right].lock();
        pickRightFork();

        eat();

        // 放下筷子
        putRightFork();
        forks_[right].unlock();
        putLeftFork();
        forks_[left].unlock();

        // 释放就餐许可
        {
            std::lock_guard<std::mutex> lock(seatMutex_);
            ++seats_;
        }
        seatCv_.notify_one();
    }

private:
    std::vector<std::mutex> forks_;
    int seats_;
    std::mutex seatMutex_;
    std::condition_variable seatCv_;
};

// C++20 信号量版本
#if __cplusplus >= 202002L
class DiningPhilosophersLimitedSem {
public:
    DiningPhilosophersLimitedSem() : forks_(5), seats_(4) {}

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % 5;

        seats_.acquire();  // 获取座位

        forks_[left].lock();
        pickLeftFork();
        forks_[right].lock();
        pickRightFork();

        eat();

        putRightFork();
        forks_[right].unlock();
        putLeftFork();
        forks_[left].unlock();

        seats_.release();  // 释放座位
    }

private:
    std::vector<std::mutex> forks_;
    std::counting_semaphore<4> seats_;
};
#endif

// ==================== 解法3: 条件变量（原子获取两只筷子）====================
// 只有当两只筷子都可用时才拿起，否则等待
class DiningPhilosophersCV {
public:
    DiningPhilosophersCV() : forkAvailable_(5, true) {}

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % 5;

        // 原子地获取两只筷子
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

        // 释放两只筷子
        {
            std::lock_guard<std::mutex> lock(mutex_);
            forkAvailable_[left] = true;
            forkAvailable_[right] = true;
        }
        cv_.notify_all();
    }

private:
    std::vector<bool> forkAvailable_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// ==================== 解法4: std::lock 避免死锁 ====================
// 使用 std::lock 同时锁定两个互斥锁
class DiningPhilosophersStdLock {
public:
    DiningPhilosophersStdLock() : forks_(5) {}

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % 5;

        // std::lock 使用死锁避免算法同时锁定两个锁
        std::lock(forks_[left], forks_[right]);
        std::lock_guard<std::mutex> lgLeft(forks_[left], std::adopt_lock);
        std::lock_guard<std::mutex> lgRight(forks_[right], std::adopt_lock);

        pickLeftFork();
        pickRightFork();
        eat();
        putLeftFork();
        putRightFork();
    }

private:
    std::vector<std::mutex> forks_;
};

// C++17 scoped_lock 版本
class DiningPhilosophersScopedLock {
public:
    DiningPhilosophersScopedLock() : forks_(5) {}

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        int left = philosopher;
        int right = (philosopher + 1) % 5;

        // scoped_lock 自动使用死锁避免算法
        std::scoped_lock lock(forks_[left], forks_[right]);

        pickLeftFork();
        pickRightFork();
        eat();
        putLeftFork();
        putRightFork();
    }

private:
    std::vector<std::mutex> forks_;
};

// ==================== 测试代码 ====================
void testPhilosophers() {
    DiningPhilosophersOddEven dp;

    auto philosopher = [&dp](int id) {
        for (int i = 0; i < 3; ++i) {
            dp.wantsToEat(
                id,
                [id] { std::cout << "P" << id << " picks left\n"; },
                [id] { std::cout << "P" << id << " picks right\n"; },
                [id] {
                    std::cout << "P" << id << " eats\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                },
                [id] { std::cout << "P" << id << " puts left\n"; },
                [id] { std::cout << "P" << id << " puts right\n"; }
            );
            // 思考
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(philosopher, i);
    }
    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    std::cout << "=== Dining Philosophers Test ===\n";
    testPhilosophers();
    std::cout << "\nAll philosophers finished eating!\n";
    return 0;
}

/**
 * 关键要点：
 *
 * 1. 死锁的四个必要条件：
 *    - 互斥：资源不能共享
 *    - 持有并等待：持有资源的同时等待其他资源
 *    - 不可抢占：资源不能被强制释放
 *    - 循环等待：存在循环的等待链
 *
 * 2. 打破死锁的策略：
 *    a) 资源分级（奇偶策略）：打破循环等待
 *    b) 限制并发：最多4人同时拿筷子
 *    c) 原子获取：要么同时获得两只筷子，要么都不获得
 *    d) std::lock：使用标准库的死锁避免算法
 *
 * 3. 各解法对比：
 *    - 奇偶策略：简单高效，无额外开销
 *    - 限制并发：直观，但需要信号量/条件变量
 *    - 条件变量：灵活，但可能导致饥饿
 *    - std::lock：最简洁，推荐使用
 *
 * 4. 饥饿问题：
 *    - 某些解法可能导致某个哲学家长期无法吃饭
 *    - 可通过公平调度或超时机制解决
 *
 * 5. 实际应用：
 *    - 数据库事务的多资源锁定
 *    - 分布式系统的资源分配
 *    - 多线程程序的锁顺序设计
 */
