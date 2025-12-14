/**
 * @file read_write_lock.h
 * @brief 读写锁 - 声明
 */

#ifndef READ_WRITE_LOCK_H
#define READ_WRITE_LOCK_H

#include <mutex>
#include <condition_variable>
#include <shared_mutex>

namespace ReadWriteLock {

// ==================== 面试者实现 ====================

class ReadWriteLock {
public:
    void lock_read();
    void unlock_read();
    void lock_write();
    void unlock_write();

private:
    // TODO
};

// ==================== 参考答案 ====================

class ReadWriteLockSolution {
public:
    ReadWriteLockSolution();
    void lock_read();
    void unlock_read();
    void lock_write();
    void unlock_write();

private:
    int readers_;
    bool writer_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class SharedMutexWrapper {
public:
    void lock_read();
    void unlock_read();
    void lock_write();
    void unlock_write();

private:
    std::shared_mutex mutex_;
};

// 测试函数
void runTests();

} // namespace ReadWriteLock

#endif // READ_WRITE_LOCK_H
