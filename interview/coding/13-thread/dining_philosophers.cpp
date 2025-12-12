/**
 * @file dining_philosophers.cpp
 * @brief 哲学家就餐问题 - 面试题
 *
 * 经典的并发同步问题，考察死锁预防和资源分配策略。
 *
 * 问题描述：
 * - 5个哲学家围坐在圆桌旁
 * - 每个哲学家之间有一只筷子（共5只）
 * - 哲学家需要同时拿起左右两只筷子才能吃饭
 * - 吃完后放下筷子继续思考
 * - 设计算法避免死锁和饥饿
 */
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>

// 题目1: 基础版本 - 使用互斥锁解决
// 要求：避免死锁，保证每个哲学家都能吃到饭
class DiningPhilosophers {
public:
    DiningPhilosophers() {
        // TODO: 初始化 5 只筷子
    }

    // 哲学家编号 0-4，函数回调用于打印动作
    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        // TODO: 实现哲学家就餐逻辑
        // 1. 拿起筷子（避免死锁）
        // 2. 吃饭
        // 3. 放下筷子
    }

private:
    // TODO: 添加同步原语（互斥锁数组等）
};

// 题目2: 限制同时就餐人数
// 最多允许4个哲学家同时尝试拿筷子
class DiningPhilosophersLimited {
public:
    DiningPhilosophersLimited() { /* TODO */ }

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        // TODO
    }

private:
    // TODO
};

// 题目3: 使用条件变量实现
// 只有当两只筷子都可用时才拿起
class DiningPhilosophersCV {
public:
    DiningPhilosophersCV() { /* TODO */ }

    void wantsToEat(int philosopher,
                    std::function<void()> pickLeftFork,
                    std::function<void()> pickRightFork,
                    std::function<void()> eat,
                    std::function<void()> putLeftFork,
                    std::function<void()> putRightFork) {
        // TODO
    }

private:
    // TODO
};

int main() {
    return 0;
}
