/**
 * @file producer_consumer_solution.cpp
 * @brief 生产者消费者问题 - 解答
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <atomic>

/**
 * 题目1：基础生产者消费者
 */
template <typename T>
class BoundedBuffer {
public:
    explicit BoundedBuffer(size_t capacity) : capacity_(capacity) {}

    void put(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return buffer_.size() < capacity_; });
        buffer_.push(std::move(item));
        notEmpty_.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !buffer_.empty(); });
        T item = std::move(buffer_.front());
        buffer_.pop();
        notFull_.notify_one();
        return item;
    }

    bool tryPut(T item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (buffer_.size() >= capacity_) return false;
        buffer_.push(std::move(item));
        notEmpty_.notify_one();
        return true;
    }

    bool tryTake(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (buffer_.empty()) return false;
        item = std::move(buffer_.front());
        buffer_.pop();
        notFull_.notify_one();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.empty();
    }

    bool full() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size() >= capacity_;
    }

private:
    std::queue<T> buffer_;
    size_t capacity_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

/**
 * 题目2：带超时的生产者消费者
 */
template <typename T>
class TimedBoundedBuffer {
public:
    explicit TimedBoundedBuffer(size_t capacity) : capacity_(capacity) {}

    template <typename Rep, typename Period>
    bool putFor(T item, const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!notFull_.wait_for(lock, timeout,
                               [this] { return buffer_.size() < capacity_; })) {
            return false;
        }
        buffer_.push(std::move(item));
        notEmpty_.notify_one();
        return true;
    }

    template <typename Rep, typename Period>
    bool takeFor(T& item, const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!notEmpty_.wait_for(lock, timeout,
                                [this] { return !buffer_.empty(); })) {
            return false;
        }
        item = std::move(buffer_.front());
        buffer_.pop();
        notFull_.notify_one();
        return true;
    }

    void put(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return buffer_.size() < capacity_; });
        buffer_.push(std::move(item));
        notEmpty_.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !buffer_.empty(); });
        T item = std::move(buffer_.front());
        buffer_.pop();
        notFull_.notify_one();
        return item;
    }

private:
    std::queue<T> buffer_;
    size_t capacity_;
    std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

/**
 * 题目3：带优先级的生产者消费者
 */
template <typename T>
class PriorityBuffer {
public:
    explicit PriorityBuffer(size_t capacity = SIZE_MAX) : capacity_(capacity) {}

    void put(T item, int priority) {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return buffer_.size() < capacity_; });
        buffer_.push({std::move(item), priority});
        notEmpty_.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !buffer_.empty(); });
        T item = std::move(const_cast<PriorityItem&>(buffer_.top()).item);
        buffer_.pop();
        notFull_.notify_one();
        return item;
    }

    bool tryTake(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (buffer_.empty()) return false;
        item = std::move(const_cast<PriorityItem&>(buffer_.top()).item);
        buffer_.pop();
        notFull_.notify_one();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }

private:
    struct PriorityItem {
        T item;
        int priority;

        bool operator<(const PriorityItem& other) const {
            return priority < other.priority;  // 大优先级在前
        }
    };

    std::priority_queue<PriorityItem> buffer_;
    size_t capacity_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

/**
 * 题目4：支持关闭的生产者消费者
 */
template <typename T>
class CloseableBuffer {
public:
    explicit CloseableBuffer(size_t capacity)
        : capacity_(capacity), closed_(false) {}

    bool put(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] {
            return closed_ || buffer_.size() < capacity_;
        });

        if (closed_) return false;

        buffer_.push(std::move(item));
        notEmpty_.notify_one();
        return true;
    }

    bool take(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] {
            return closed_ || !buffer_.empty();
        });

        if (buffer_.empty()) return false;  // 关闭且为空

        item = std::move(buffer_.front());
        buffer_.pop();
        notFull_.notify_one();
        return true;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        notEmpty_.notify_all();
        notFull_.notify_all();
    }

    bool isClosed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }

    bool isClosedAndEmpty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_ && buffer_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }

private:
    std::queue<T> buffer_;
    size_t capacity_;
    bool closed_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

int main() {
    std::cout << "=== 测试基础生产者消费者 ===\n";
    {
        BoundedBuffer<int> buffer(5);
        std::atomic<int> produced{0}, consumed{0};

        std::vector<std::thread> producers, consumers;

        // 3 个生产者
        for (int i = 0; i < 3; ++i) {
            producers.emplace_back([&buffer, &produced, i] {
                for (int j = 0; j < 10; ++j) {
                    buffer.put(i * 100 + j);
                    ++produced;
                }
            });
        }

        // 2 个消费者
        for (int i = 0; i < 2; ++i) {
            consumers.emplace_back([&buffer, &consumed] {
                for (int j = 0; j < 15; ++j) {
                    int item = buffer.take();
                    ++consumed;
                    (void)item;
                }
            });
        }

        for (auto& t : producers) t.join();
        for (auto& t : consumers) t.join();

        std::cout << "Produced: " << produced << ", Consumed: " << consumed << "\n";
    }

    std::cout << "\n=== 测试带超时的生产者消费者 ===\n";
    {
        TimedBoundedBuffer<int> buffer(2);

        // 先填满
        buffer.put(1);
        buffer.put(2);

        // 超时测试
        auto start = std::chrono::steady_clock::now();
        bool success = buffer.putFor(3, std::chrono::milliseconds(100));
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "putFor result: " << (success ? "success" : "timeout")
                  << " after " << elapsed.count() << "ms\n";
    }

    std::cout << "\n=== 测试带优先级的生产者消费者 ===\n";
    {
        PriorityBuffer<std::string> buffer;

        buffer.put("Low priority", 1);
        buffer.put("High priority", 10);
        buffer.put("Medium priority", 5);

        std::cout << "Take order:\n";
        for (int i = 0; i < 3; ++i) {
            std::cout << "  " << buffer.take() << "\n";
        }
    }

    std::cout << "\n=== 测试可关闭的生产者消费者 ===\n";
    {
        CloseableBuffer<int> buffer(10);

        std::thread producer([&buffer] {
            for (int i = 0; i < 5; ++i) {
                if (buffer.put(i)) {
                    std::cout << "Produced: " << i << "\n";
                } else {
                    std::cout << "Producer: buffer closed\n";
                    break;
                }
            }
        });

        std::thread consumer([&buffer] {
            int item;
            while (buffer.take(item)) {
                std::cout << "Consumed: " << item << "\n";
            }
            std::cout << "Consumer: buffer closed and empty\n";
        });

        producer.join();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Closing buffer...\n";
        buffer.close();

        consumer.join();
    }

    return 0;
}
