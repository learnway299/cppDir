/**
 * @file timer_wheel_solution.cpp
 * @brief 时间轮 - 解答
 */
#include <functional>
#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
using namespace std;

class TimerWheel {
public:
    using Callback = function<void()>;
    using TimerId = uint64_t;

private:
    struct Timer {
        TimerId id;
        Callback callback;
        int rounds;  // 剩余圈数
    };

    vector<list<Timer>> wheel_;
    unordered_map<TimerId, list<Timer>::iterator> timerMap_;
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

        Timer timer{nextId_++, move(cb), rounds};
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

int main() {
    TimerWheel tw(8, 100);  // 8个槽，每个槽100ms

    // 添加定时器
    auto id1 = tw.addTimer(300, []{ cout << "Timer 1 fired!\n"; });
    auto id2 = tw.addTimer(500, []{ cout << "Timer 2 fired!\n"; });
    auto id3 = tw.addTimer(1000, []{ cout << "Timer 3 fired!\n"; });

    cout << "Added timers: " << id1 << ", " << id2 << ", " << id3 << "\n";

    // 取消一个定时器
    tw.cancelTimer(id2);
    cout << "Cancelled timer " << id2 << "\n";

    // 模拟时间推进
    cout << "\nSimulating ticks:\n";
    for (int i = 0; i < 12; ++i) {
        cout << "Tick " << i << " (slot " << tw.getCurrentSlot() << "): ";
        tw.tick();
        cout << "\n";
    }

    return 0;
}
