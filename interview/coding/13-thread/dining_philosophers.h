/**
 * @file dining_philosophers.h
 * @brief 哲学家就餐问题 - 声明
 *
 * 经典的并发同步问题，考察死锁预防和资源分配策略。
 */

#ifndef DINING_PHILOSOPHERS_H
#define DINING_PHILOSOPHERS_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>

namespace DiningPhilosophers {

// ==================== 面试者实现 ====================

// 题目1: 基础版本 - 使用互斥锁解决
class DiningPhilosophers {
public:
    DiningPhilosophers();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    // TODO: 添加同步原语
};

// 题目2: 限制同时就餐人数
class DiningPhilosophersLimited {
public:
    DiningPhilosophersLimited();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    // TODO
};

// 题目3: 使用条件变量实现
class DiningPhilosophersCV {
public:
    DiningPhilosophersCV();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    // TODO
};

// ==================== 参考答案 ====================

class DiningPhilosophersOddEvenSolution {
public:
    DiningPhilosophersOddEvenSolution();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    std::vector<std::mutex> forks_;
};

class DiningPhilosophersLimitedSolution {
public:
    DiningPhilosophersLimitedSolution();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    std::vector<std::mutex> forks_;
    int seats_;
    std::mutex seatMutex_;
    std::condition_variable seatCv_;
};

class DiningPhilosophersCVSolution {
public:
    DiningPhilosophersCVSolution();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    std::vector<bool> forkAvailable_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class DiningPhilosophersStdLockSolution {
public:
    DiningPhilosophersStdLockSolution();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    std::vector<std::mutex> forks_;
};

class DiningPhilosophersScopedLockSolution {
public:
    DiningPhilosophersScopedLockSolution();
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork);
private:
    std::vector<std::mutex> forks_;
};

// 测试函数
void runTests();

} // namespace DiningPhilosophers

#endif // DINING_PHILOSOPHERS_H
