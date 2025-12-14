/**
 * @file read_write_lock_solution.cpp
 * @brief 读写锁 - 参考答案
 */

#include "read_write_lock.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

namespace ReadWriteLock {

// ==================== 参考答案实现 ====================

ReadWriteLockSolution::ReadWriteLockSolution() : readers_(0), writer_(false) {}

void ReadWriteLockSolution::lock_read() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !writer_; });
    ++readers_;
}

void ReadWriteLockSolution::unlock_read() {
    std::lock_guard<std::mutex> lock(mutex_);
    --readers_;
    if (readers_ == 0) {
        cv_.notify_all();
    }
}

void ReadWriteLockSolution::lock_write() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !writer_ && readers_ == 0; });
    writer_ = true;
}

void ReadWriteLockSolution::unlock_write() {
    std::lock_guard<std::mutex> lock(mutex_);
    writer_ = false;
    cv_.notify_all();
}

// SharedMutex 封装
void SharedMutexWrapper::lock_read() {
    mutex_.lock_shared();
}

void SharedMutexWrapper::unlock_read() {
    mutex_.unlock_shared();
}

void SharedMutexWrapper::lock_write() {
    mutex_.lock();
}

void SharedMutexWrapper::unlock_write() {
    mutex_.unlock();
}

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Read Write Lock Tests ===" << std::endl;

    // 测试读写锁
    {
        ReadWriteLockSolution rwlock;
        int sharedData = 0;
        std::atomic<int> readCount{0};

        // 多个读者
        std::vector<std::thread> readers;
        for (int i = 0; i < 5; ++i) {
            readers.emplace_back([&rwlock, &sharedData, &readCount] {
                rwlock.lock_read();
                ++readCount;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                --readCount;
                rwlock.unlock_read();
            });
        }

        // 一个写者
        std::thread writer([&rwlock, &sharedData] {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            rwlock.lock_write();
            ++sharedData;
            rwlock.unlock_write();
        });

        for (auto& r : readers) r.join();
        writer.join();

        assert(sharedData == 1);
    }
    std::cout << "  ReadWriteLock: PASSED" << std::endl;

    // 测试 shared_mutex 封装
    {
        SharedMutexWrapper rwlock;
        int sharedData = 0;

        std::thread reader1([&rwlock, &sharedData] {
            rwlock.lock_read();
            int val = sharedData;
            rwlock.unlock_read();
        });

        std::thread writer([&rwlock, &sharedData] {
            rwlock.lock_write();
            ++sharedData;
            rwlock.unlock_write();
        });

        reader1.join();
        writer.join();

        assert(sharedData == 1);
    }
    std::cout << "  SharedMutexWrapper: PASSED" << std::endl;
}

} // namespace ReadWriteLock
