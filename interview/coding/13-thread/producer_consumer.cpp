/**
 * @file producer_consumer.cpp
 * @brief 生产者消费者问题 - 面试题
 *
 * 经典多线程同步问题
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

/**
 * 题目1：实现基础生产者消费者模型
 *
 * 要求：
 * 1. 有界缓冲区
 * 2. 多生产者多消费者
 * 3. 缓冲区满时生产者等待
 * 4. 缓冲区空时消费者等待
 */
template <typename T>
class BoundedBuffer {
public:
    explicit BoundedBuffer(size_t capacity) {
        // TODO
    }

    // TODO: 生产者放入数据
    void put(T item) {
    }

    // TODO: 消费者取出数据
    T take() {
        return T();
    }

    // TODO: 非阻塞版本
    bool tryPut(T item) {
        return false;
    }

    bool tryTake(T& item) {
        return false;
    }

    // TODO: 返回当前大小
    size_t size() const {
        return 0;
    }

private:
    // TODO: 添加成员变量
};

/**
 * 题目2：实现带超时的生产者消费者
 *
 * 要求：支持超时等待
 */
template <typename T>
class TimedBoundedBuffer {
public:
    explicit TimedBoundedBuffer(size_t capacity) {
        // TODO
    }

    // TODO: 带超时的 put
    template <typename Rep, typename Period>
    bool putFor(T item, const std::chrono::duration<Rep, Period>& timeout) {
        return false;
    }

    // TODO: 带超时的 take
    template <typename Rep, typename Period>
    bool takeFor(T& item, const std::chrono::duration<Rep, Period>& timeout) {
        return false;
    }

private:
    // TODO: 添加成员变量
};

/**
 * 题目3：实现带优先级的生产者消费者
 *
 * 要求：高优先级消息优先被消费
 */
template <typename T>
class PriorityBuffer {
public:
    // TODO: 放入带优先级的数据
    void put(T item, int priority) {
    }

    // TODO: 取出最高优先级的数据
    T take() {
        return T();
    }

private:
    // TODO: 使用优先队列实现
};

/**
 * 题目4：实现支持关闭的生产者消费者
 *
 * 要求：
 * 1. 支持优雅关闭
 * 2. 关闭后生产者不能再放入
 * 3. 消费者可以取出剩余数据
 */
template <typename T>
class CloseableBuffer {
public:
    explicit CloseableBuffer(size_t capacity) {
        // TODO
    }

    // TODO: 放入数据，返回是否成功（关闭后返回 false）
    bool put(T item) {
        return false;
    }

    // TODO: 取出数据，返回是否成功（关闭且为空返回 false）
    bool take(T& item) {
        return false;
    }

    // TODO: 关闭缓冲区
    void close() {
    }

    // TODO: 检查是否关闭
    bool isClosed() const {
        return false;
    }

private:
    // TODO: 添加成员变量
};

int main() {
    return 0;
}
