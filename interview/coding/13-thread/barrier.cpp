/**
 * @file barrier.cpp
 * @brief 屏障 (Barrier) - 面试题
 *
 * 屏障是一种同步原语，用于让一组线程在某个点上等待，
 * 直到所有线程都到达该点后才能继续执行。
 */
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

// 题目1: 实现基本的屏障
// 所有线程到达后才能继续，且屏障可重复使用
class Barrier {
public:
    explicit Barrier(int count) : threshold_(count) { /* TODO */ }

    // 阻塞直到所有线程到达
    void wait() { /* TODO */ }

private:
    int threshold_;
    // TODO: 添加其他成员
};

// 题目2: 带回调的屏障
// 当所有线程到达时，执行一个回调函数（由最后一个到达的线程执行）
class BarrierWithCallback {
public:
    BarrierWithCallback(int count, std::function<void()> callback)
        : threshold_(count), callback_(callback) { /* TODO */ }

    void wait() { /* TODO */ }

private:
    int threshold_;
    std::function<void()> callback_;
    // TODO: 添加其他成员
};

// 题目3: 可重置的屏障
// 支持动态改变等待线程数
class ResetableBarrier {
public:
    explicit ResetableBarrier(int count) { /* TODO */ }

    void wait() { /* TODO */ }
    void reset(int newCount) { /* TODO */ }

private:
    // TODO
};

// 题目4: 实现 CountDownLatch
// 一次性的屏障，等待计数器归零
class CountDownLatch {
public:
    explicit CountDownLatch(int count) { /* TODO */ }

    void countDown() { /* TODO */ }  // 计数器减1
    void wait() { /* TODO */ }       // 等待计数器归零

private:
    // TODO
};

// 题目5: 实现 CyclicBarrier (类似 Java)
// 可循环使用的屏障，支持超时等待
class CyclicBarrier {
public:
    explicit CyclicBarrier(int parties) { /* TODO */ }

    // 返回到达顺序（0表示最后一个）
    int await() { return 0; /* TODO */ }

    // 带超时的等待，超时返回 -1
    int await(int timeoutMs) { return 0; /* TODO */ }

    // 重置屏障
    void reset() { /* TODO */ }

    // 获取正在等待的线程数
    int getNumberWaiting() const { return 0; /* TODO */ }

private:
    // TODO
};

int main() {
    return 0;
}
