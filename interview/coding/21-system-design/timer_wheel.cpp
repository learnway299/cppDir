/**
 * @file timer_wheel.cpp
 * @brief 时间轮 - 面试题
 */
#include <functional>
#include <cstdint>

// 题目: 实现时间轮定时器
// 支持:
// - addTimer(delay, callback): 添加定时任务
// - tick(): 推进时间
// - cancelTimer(id): 取消定时任务
class TimerWheel {
public:
    using Callback = std::function<void()>;
    using TimerId = uint64_t;

    TimerWheel(int slots, int tickMs) {
        // TODO
    }

    TimerId addTimer(int delayMs, Callback cb) {
        // TODO
        return 0;
    }

    void cancelTimer(TimerId id) {
        // TODO
    }

    void tick() {
        // TODO
    }
};

int main() { return 0; }
