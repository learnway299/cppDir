/**
 * @file timer_wheel.h
 * @brief 时间轮 - 面试题
 *
 * 题目：实现时间轮定时器
 *
 * 要求：
 * 1. 实现 TimerWheel 类，支持以下操作：
 *    - TimerWheel(int slots, int tickMs): 构造函数，指定槽数和每个槽的时间
 *    - TimerId addTimer(int delayMs, Callback cb): 添加定时任务，返回定时器 ID
 *    - void cancelTimer(TimerId id): 取消定时任务
 *    - void tick(): 推进时间，执行到期的定时任务
 *
 * 2. 时间轮的特性：
 *    - 使用环形数组，每个槽存储一组定时器
 *    - 支持 rounds（圈数）机制处理长延迟
 *    - 每次 tick 移动到下一个槽
 *
 * 3. 应用场景：
 *    - 网络超时检测
 *    - 任务调度
 *    - 游戏 tick 系统
 *
 * 时间复杂度：
 * - addTimer: O(1)
 * - cancelTimer: O(N)，N 为该槽中的定时器数量
 * - tick: O(M)，M 为当前槽中的定时器数量
 *
 * 空间复杂度：O(slots + N)，N 为定时器总数
 */

#ifndef TIMER_WHEEL_H
#define TIMER_WHEEL_H

#include <functional>
#include <vector>
#include <list>
#include <unordered_map>
#include <cstdint>

namespace TimerWheelImpl {

class TimerWheel {
public:
    using Callback = std::function<void()>;
    using TimerId = uint64_t;

private:
    struct Timer {
        TimerId id;
        Callback callback;
        int rounds;  // 剩余圈数
    };

    std::vector<std::list<Timer>> wheel_;
    std::unordered_map<TimerId, std::list<Timer>::iterator> timerMap_;
    int slots_;
    int tickMs_;
    int currentSlot_;
    TimerId nextId_;

public:
    TimerWheel(int slots, int tickMs);

    TimerId addTimer(int delayMs, Callback cb);

    void cancelTimer(TimerId id);

    void tick();

    int getCurrentSlot() const;
};

void runTests();

} // namespace TimerWheelImpl

#endif // TIMER_WHEEL_H
