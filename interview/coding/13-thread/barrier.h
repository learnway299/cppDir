/**
 * @file barrier.h
 * @brief 屏障 (Barrier) - 声明
 *
 * 屏障是一种同步原语，用于让一组线程在某个点上等待，
 * 直到所有线程都到达该点后才能继续执行。
 */

#ifndef BARRIER_H
#define BARRIER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace Barrier {

// ==================== 面试者实现 ====================

// 题目1: 实现基本的屏障
// 所有线程到达后才能继续，且屏障可重复使用
class Barrier {
public:
    explicit Barrier(int count);

    // 阻塞直到所有线程到达
    void wait();

private:
    int threshold_;
    // TODO: 添加其他成员
};

// 题目2: 带回调的屏障
// 当所有线程到达时，执行一个回调函数（由最后一个到达的线程执行）
class BarrierWithCallback {
public:
    BarrierWithCallback(int count, std::function<void()> callback);

    void wait();

private:
    int threshold_;
    std::function<void()> callback_;
    // TODO: 添加其他成员
};

// 题目3: 可重置的屏障
// 支持动态改变等待线程数
class ResetableBarrier {
public:
    explicit ResetableBarrier(int count);

    void wait();
    void reset(int newCount);

private:
    // TODO
};

// 题目4: 实现 CountDownLatch
// 一次性的屏障，等待计数器归零
class CountDownLatch {
public:
    explicit CountDownLatch(int count);

    void countDown();  // 计数器减1
    void wait();       // 等待计数器归零

private:
    // TODO
};

// 题目5: 实现 CyclicBarrier (类似 Java)
// 可循环使用的屏障，支持超时等待
class CyclicBarrier {
public:
    explicit CyclicBarrier(int parties);

    // 返回到达顺序（0表示最后一个）
    int await();

    // 带超时的等待，超时返回 -1
    int await(int timeoutMs);

    // 重置屏障
    void reset();

    // 获取正在等待的线程数
    int getNumberWaiting() const;

private:
    // TODO
};

// ==================== 参考答案 ====================

class BarrierSolution {
public:
    explicit BarrierSolution(int count);
    void wait();
private:
    int threshold_;
    int count_;
    int generation_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class BarrierWithCallbackSolution {
public:
    BarrierWithCallbackSolution(int count, std::function<void()> callback);
    void wait();
private:
    int threshold_;
    int count_;
    int generation_;
    std::function<void()> callback_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class ResetableBarrierSolution {
public:
    explicit ResetableBarrierSolution(int count);
    void wait();
    void reset(int newCount);
private:
    int threshold_;
    int count_;
    int generation_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class CountDownLatchSolution {
public:
    explicit CountDownLatchSolution(int count);
    void countDown();
    void wait();
    bool wait(int timeoutMs);
    int getCount() const;
private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};

class CyclicBarrierSolution {
public:
    explicit CyclicBarrierSolution(int parties);
    int await();
    int await(int timeoutMs);
    void reset();
    int getNumberWaiting() const;
    bool isBroken() const;
private:
    int parties_;
    int count_;
    int generation_;
    bool broken_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

// 测试函数
void runTests();

} // namespace Barrier

#endif // BARRIER_H
