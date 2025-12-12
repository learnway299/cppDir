/**
 * @file thread_safe_queue.cpp
 * @brief 线程安全队列 - 面试题
 */
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template <typename T>
class ThreadSafeQueue {
public:
    void push(T value) { /* TODO */ }
    bool try_pop(T& value) { return false; }
    void wait_and_pop(T& value) { /* TODO */ }
    bool empty() const { return true; }
};

int main() { return 0; }
