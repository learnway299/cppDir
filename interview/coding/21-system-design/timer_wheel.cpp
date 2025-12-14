/**
 * @file timer_wheel.cpp
 * @brief 时间轮 - 面试题实现
 */

#include "timer_wheel.h"

namespace TimerWheelImpl {

TimerWheel::TimerWheel(int slots, int tickMs)
    : slots_(slots), tickMs_(tickMs), wheel_(slots), currentSlot_(0), nextId_(1) {
    // TODO: 初始化成员变量（已在初始化列表中完成）
}

TimerWheel::TimerId TimerWheel::addTimer(int delayMs, Callback cb) {
    // TODO: 实现添加定时器
    // 提示：
    // 1. 计算延迟对应的 tick 数：ticks = delayMs / tickMs_
    // 2. 计算应该放入的槽位：slot = (currentSlot_ + ticks) % slots_
    // 3. 计算需要转的圈数：rounds = ticks / slots_
    // 4. 创建 Timer 对象并添加到对应槽的链表
    // 5. 记录到 timerMap_ 以便快速取消
    // 6. 返回定时器 ID
    return 0;
}

void TimerWheel::cancelTimer(TimerWheel::TimerId id) {
    // TODO: 实现取消定时器
    // 提示：
    // 1. 在 timerMap_ 中查找定时器
    // 2. 如果找到，遍历所有槽找到对应的定时器并删除
    // 3. 从 timerMap_ 中移除记录
}

void TimerWheel::tick() {
    // TODO: 实现时间推进
    // 提示：
    // 1. 获取当前槽的定时器列表
    // 2. 遍历列表中的每个定时器
    // 3. 如果 rounds > 0，减少 rounds 并继续
    // 4. 如果 rounds == 0，执行回调并删除定时器
    // 5. 将 currentSlot_ 移动到下一个槽（环形）
}

int TimerWheel::getCurrentSlot() const {
    return currentSlot_;
}

} // namespace TimerWheelImpl
