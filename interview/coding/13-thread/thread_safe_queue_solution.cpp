/**
 * @file thread_safe_queue_solution.cpp
 * @brief 线程安全队列 - 解答
 */
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <thread>
#include <vector>

template <typename T>
class ThreadSafeQueue {
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cv_.notify_one();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        value = std::move(queue_.front());
        queue_.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        auto result = std::make_shared<T>(std::move(queue_.front()));
        queue_.pop();
        return result;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable cv_;
};

int main() {
    ThreadSafeQueue<int> queue;
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&queue, i] {
            for (int j = 0; j < 10; ++j) {
                queue.push(i * 100 + j);
            }
        });
    }

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([&queue] {
            for (int j = 0; j < 25; ++j) {
                int value;
                queue.wait_and_pop(value);
                std::cout << "Popped: " << value << "\n";
            }
        });
    }

    for (auto& t : threads) t.join();
    return 0;
}
