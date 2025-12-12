/**
 * @file read_write_lock_solution.cpp
 * @brief 读写锁 - 解答
 */
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <vector>

class ReadWriteLock {
public:
    void lock_read() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !writing_; });
        ++readers_;
    }

    void unlock_read() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (--readers_ == 0) {
            cv_.notify_all();
        }
    }

    void lock_write() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !writing_ && readers_ == 0; });
        writing_ = true;
    }

    void unlock_write() {
        std::lock_guard<std::mutex> lock(mutex_);
        writing_ = false;
        cv_.notify_all();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int readers_ = 0;
    bool writing_ = false;
};

// RAII 包装
class ReadLock {
public:
    explicit ReadLock(ReadWriteLock& rwlock) : rwlock_(rwlock) { rwlock_.lock_read(); }
    ~ReadLock() { rwlock_.unlock_read(); }
private:
    ReadWriteLock& rwlock_;
};

class WriteLock {
public:
    explicit WriteLock(ReadWriteLock& rwlock) : rwlock_(rwlock) { rwlock_.lock_write(); }
    ~WriteLock() { rwlock_.unlock_write(); }
private:
    ReadWriteLock& rwlock_;
};

int main() {
    ReadWriteLock rwlock;
    int sharedData = 0;
    std::vector<std::thread> threads;

    // 多个读者
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&rwlock, &sharedData, i] {
            for (int j = 0; j < 3; ++j) {
                ReadLock lock(rwlock);
                std::cout << "Reader " << i << " read: " << sharedData << "\n";
            }
        });
    }

    // 写者
    threads.emplace_back([&rwlock, &sharedData] {
        for (int j = 0; j < 5; ++j) {
            WriteLock lock(rwlock);
            ++sharedData;
            std::cout << "Writer wrote: " << sharedData << "\n";
        }
    });

    for (auto& t : threads) t.join();
    return 0;
}
