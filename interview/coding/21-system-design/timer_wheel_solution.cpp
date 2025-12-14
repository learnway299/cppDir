/**
 * @file timer_wheel_solution.cpp
 * @brief 时间轮 - 参考解答
 */

#include "timer_wheel.h"
#include <iostream>

namespace TimerWheelImpl {
namespace Solution {

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
    int currentSlot_ = 0;
    TimerId nextId_ = 1;

public:
    TimerWheel(int slots, int tickMs) : slots_(slots), tickMs_(tickMs), wheel_(slots) {}

    TimerId addTimer(int delayMs, Callback cb) {
        int ticks = delayMs / tickMs_;
        int slot = (currentSlot_ + ticks) % slots_;
        int rounds = ticks / slots_;

        Timer timer{nextId_++, std::move(cb), rounds};
        wheel_[slot].push_back(timer);

        auto it = --wheel_[slot].end();
        timerMap_[timer.id] = it;

        return timer.id;
    }

    void cancelTimer(TimerId id) {
        auto it = timerMap_.find(id);
        if (it != timerMap_.end()) {
            // 找到所在的slot并删除
            for (auto& slot : wheel_) {
                for (auto timer_it = slot.begin(); timer_it != slot.end(); ++timer_it) {
                    if (timer_it->id == id) {
                        slot.erase(timer_it);
                        timerMap_.erase(it);
                        return;
                    }
                }
            }
        }
    }

    void tick() {
        auto& slot = wheel_[currentSlot_];
        auto it = slot.begin();

        while (it != slot.end()) {
            if (it->rounds > 0) {
                --it->rounds;
                ++it;
            } else {
                // 执行回调
                it->callback();
                timerMap_.erase(it->id);
                it = slot.erase(it);
            }
        }

        currentSlot_ = (currentSlot_ + 1) % slots_;
    }

    int getCurrentSlot() const { return currentSlot_; }
};

void runSolutionTests() {
    std::cout << "=== Timer Wheel Solution ===" << std::endl;

    TimerWheel tw(8, 100);  // 8个槽，每个槽100ms

    // 添加定时器
    auto id1 = tw.addTimer(300, []{ std::cout << "Timer 1 fired!"; });
    auto id2 = tw.addTimer(500, []{ std::cout << "Timer 2 fired!"; });
    auto id3 = tw.addTimer(1000, []{ std::cout << "Timer 3 fired!"; });

    std::cout << "Added timers: " << id1 << ", " << id2 << ", " << id3 << std::endl;

    // 取消一个定时器
    tw.cancelTimer(id2);
    std::cout << "Cancelled timer " << id2 << std::endl;

    // 模拟时间推进
    std::cout << "\nSimulating ticks:" << std::endl;
    for (int i = 0; i < 12; ++i) {
        std::cout << "Tick " << i << " (slot " << tw.getCurrentSlot() << "): ";
        tw.tick();
        std::cout << std::endl;
    }

    std::cout << "\nTimer wheel tests completed!" << std::endl;
}

} // namespace Solution

void runTests() {
    Solution::runSolutionTests();
}

} // namespace TimerWheelImpl
