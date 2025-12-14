/**
 * @file object_pool_solution.cpp
 * @brief 对象池实现 - 参考答案
 */

#include "object_pool.h"
#include <iostream>
#include <cassert>

namespace ObjectPoolImpl {

namespace Solution {

// ==================== 简单对象池 ====================

template <typename T>
class SimpleObjectPool {
public:
    explicit SimpleObjectPool(size_t size) {
        objects_.reserve(size);
        available_.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            objects_.push_back(std::make_unique<T>());
            available_.push_back(objects_.back().get());
        }
    }

    T* acquire() {
        if (available_.empty()) {
            return nullptr;
        }

        T* obj = available_.back();
        available_.pop_back();
        return obj;
    }

    void release(T* obj) {
        if (obj) {
            available_.push_back(obj);
        }
    }

    size_t available() const {
        return available_.size();
    }

    size_t capacity() const {
        return objects_.size();
    }

private:
    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
};

// ==================== 带重置功能的对象池 ====================

template <typename T>
class ResettableObjectPool {
public:
    using ResetFunc = std::function<void(T&)>;

    explicit ResettableObjectPool(size_t size, ResetFunc resetFunc = nullptr)
        : resetFunc_(std::move(resetFunc)) {

        objects_.reserve(size);
        available_.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            objects_.push_back(std::make_unique<T>());
            available_.push_back(objects_.back().get());
        }
    }

    T* acquire() {
        if (available_.empty()) {
            return nullptr;
        }

        T* obj = available_.back();
        available_.pop_back();
        return obj;
    }

    void release(T* obj) {
        if (obj) {
            if (resetFunc_) {
                resetFunc_(*obj);
            }
            available_.push_back(obj);
        }
    }

    size_t available() const { return available_.size(); }

private:
    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
    ResetFunc resetFunc_;
};

// ==================== RAII 包装的对象池 ====================

template <typename T>
class SmartObjectPool {
public:
    class Deleter {
    public:
        explicit Deleter(SmartObjectPool* pool = nullptr) : pool_(pool) {}

        void operator()(T* ptr) const {
            if (pool_ && ptr) {
                pool_->release(ptr);
            }
        }

    private:
        SmartObjectPool* pool_;
    };

    using Ptr = std::unique_ptr<T, Deleter>;

    explicit SmartObjectPool(size_t size) {
        objects_.reserve(size);
        available_.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            objects_.push_back(std::make_unique<T>());
            available_.push_back(objects_.back().get());
        }
    }

    Ptr acquire() {
        if (available_.empty()) {
            return Ptr(nullptr, Deleter(this));
        }

        T* obj = available_.back();
        available_.pop_back();
        return Ptr(obj, Deleter(this));
    }

    size_t available() const { return available_.size(); }

private:
    void release(T* obj) {
        if (obj) {
            available_.push_back(obj);
        }
    }

    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
};

// ==================== 线程安全的对象池 ====================

template <typename T>
class ThreadSafeObjectPool {
public:
    explicit ThreadSafeObjectPool(size_t size) {
        objects_.reserve(size);
        available_.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            objects_.push_back(std::make_unique<T>());
            available_.push_back(objects_.back().get());
        }
    }

    T* acquire() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (available_.empty()) {
            return nullptr;
        }

        T* obj = available_.back();
        available_.pop_back();
        return obj;
    }

    void release(T* obj) {
        if (obj) {
            std::lock_guard<std::mutex> lock(mutex_);
            available_.push_back(obj);
        }
    }

    size_t available() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_.size();
    }

private:
    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
    mutable std::mutex mutex_;
};

// ==================== 可增长的对象池 ====================

template <typename T>
class GrowableObjectPool {
public:
    explicit GrowableObjectPool(size_t initialSize = 16, size_t maxSize = 1024)
        : maxSize_(maxSize) {
        grow(initialSize);
    }

    T* acquire() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (available_.empty()) {
            size_t newSize = std::min(objects_.size(), maxSize_ - objects_.size());
            if (newSize == 0) {
                return nullptr;
            }
            grow(newSize);
        }

        T* obj = available_.back();
        available_.pop_back();
        return obj;
    }

    void release(T* obj) {
        if (obj) {
            std::lock_guard<std::mutex> lock(mutex_);
            available_.push_back(obj);
        }
    }

    size_t capacity() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return objects_.size();
    }

private:
    void grow(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            objects_.push_back(std::make_unique<T>());
            available_.push_back(objects_.back().get());
        }
    }

    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
    size_t maxSize_;
    mutable std::mutex mutex_;
};

// ==================== 测试用对象 ====================

struct Connection {
    int id;
    bool active;
    static int nextId_;
    static int instanceCount;

    Connection() : id(nextId_++), active(false) {
        ++instanceCount;
    }

    ~Connection() {
        --instanceCount;
    }

    void connect() {
        active = true;
    }

    void disconnect() {
        active = false;
    }

    void reset() {
        active = false;
    }
};

int Connection::nextId_ = 1;
int Connection::instanceCount = 0;

} // namespace Solution

// ==================== 测试函数 ====================

void testObjectPoolSolution() {
    std::cout << "=== Object Pool Tests (Solution) ===" << std::endl;

    // 测试简单对象池
    Solution::Connection::nextId_ = 1;
    Solution::Connection::instanceCount = 0;
    {
        Solution::SimpleObjectPool<Solution::Connection> pool(3);
        assert(pool.available() == 3);

        Solution::Connection* c1 = pool.acquire();
        c1->connect();
        assert(c1->active == true);

        Solution::Connection* c2 = pool.acquire();
        c2->connect();
        assert(pool.available() == 1);

        pool.release(c1);
        assert(pool.available() == 2);

        Solution::Connection* c3 = pool.acquire();
        assert(c3->id == c1->id);  // 重用 c1
    }
    assert(Solution::Connection::instanceCount == 0);
    std::cout << "  SimpleObjectPool: PASSED" << std::endl;

    // 测试带重置的对象池
    Solution::Connection::nextId_ = 1;
    {
        Solution::ResettableObjectPool<Solution::Connection> pool(2, [](Solution::Connection& c) {
            c.reset();
        });

        Solution::Connection* c1 = pool.acquire();
        c1->connect();
        assert(c1->active == true);

        pool.release(c1);  // 会自动调用 reset

        Solution::Connection* c2 = pool.acquire();
        assert(c2->active == false);  // 已被重置
    }
    std::cout << "  ResettableObjectPool: PASSED" << std::endl;

    // 测试智能指针对象池
    Solution::Connection::nextId_ = 1;
    {
        Solution::SmartObjectPool<Solution::Connection> pool(2);

        {
            auto c1 = pool.acquire();
            c1->connect();
            assert(pool.available() == 1);
            // c1 离开作用域时自动归还
        }

        assert(pool.available() == 2);

        auto c2 = pool.acquire();
        assert(c2->id == 1);  // 重用
    }
    std::cout << "  SmartObjectPool: PASSED" << std::endl;

    // 测试线程安全对象池
    Solution::Connection::nextId_ = 1;
    {
        Solution::ThreadSafeObjectPool<Solution::Connection> pool(3);
        assert(pool.available() == 3);

        Solution::Connection* c1 = pool.acquire();
        Solution::Connection* c2 = pool.acquire();
        assert(pool.available() == 1);

        pool.release(c1);
        pool.release(c2);
        assert(pool.available() == 3);
    }
    std::cout << "  ThreadSafeObjectPool: PASSED" << std::endl;

    // 测试可增长对象池
    Solution::Connection::nextId_ = 1;
    {
        Solution::GrowableObjectPool<Solution::Connection> pool(2, 10);
        assert(pool.capacity() == 2);

        std::vector<Solution::Connection*> acquired;
        for (int i = 0; i < 5; ++i) {
            Solution::Connection* c = pool.acquire();
            assert(c != nullptr);
            acquired.push_back(c);
        }
        assert(pool.capacity() >= 5);

        for (Solution::Connection* c : acquired) {
            pool.release(c);
        }
    }
    std::cout << "  GrowableObjectPool: PASSED" << std::endl;
}

} // namespace ObjectPoolImpl
