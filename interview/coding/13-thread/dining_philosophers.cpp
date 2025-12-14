/**
 * @file dining_philosophers.cpp
 * @brief 哲学家就餐问题 - 面试题实现
 */

#include "dining_philosophers.h"

namespace DiningPhilosophers {

// ==================== 面试者实现 ====================

// 题目1: 基础版本
DiningPhilosophers::DiningPhilosophers() {
    // TODO
}

void DiningPhilosophers::wantsToEat(int philosopher,
                                     std::function<void()> pickLeftFork,
                                     std::function<void()> pickRightFork,
                                     std::function<void()> eat,
                                     std::function<void()> putLeftFork,
                                     std::function<void()> putRightFork) {
    // TODO
}

// 题目2: 限制同时就餐人数
DiningPhilosophersLimited::DiningPhilosophersLimited() {
    // TODO
}

void DiningPhilosophersLimited::wantsToEat(int philosopher,
                                            std::function<void()> pickLeftFork,
                                            std::function<void()> pickRightFork,
                                            std::function<void()> eat,
                                            std::function<void()> putLeftFork,
                                            std::function<void()> putRightFork) {
    // TODO
}

// 题目3: 使用条件变量实现
DiningPhilosophersCV::DiningPhilosophersCV() {
    // TODO
}

void DiningPhilosophersCV::wantsToEat(int philosopher,
                                       std::function<void()> pickLeftFork,
                                       std::function<void()> pickRightFork,
                                       std::function<void()> eat,
                                       std::function<void()> putLeftFork,
                                       std::function<void()> putRightFork) {
    // TODO
}

} // namespace DiningPhilosophers
