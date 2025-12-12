/**
 * @file object_pool_solution.cpp
 * @brief 对象池实现 - 解答
 */

#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <iostream>
#include <functional>

/**
 * 题目1：实现简单对象池
 *
 * 对象预创建，获取和归还 O(1)
 */
template <typename T>
class SimpleObjectPool {
public:
    explicit SimpleObjectPool(size_t size) {
        // 预创建对象
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
    std::vector<std::unique_ptr<T>> objects_;  // 所有对象
    std::vector<T*> available_;                 // 可用对象
};

/**
 * 题目2：实现带重置功能的对象池
 *
 * 对象归还时重置状态，确保再次使用时是"干净"的
 */
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
            // 归还前重置对象
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

/**
 * 题目3：实现 RAII 包装的对象池
 *
 * 使用智能指针自动管理对象生命周期
 */
template <typename T>
class SmartObjectPool {
public:
    // 前向声明，Deleter 需要访问 pool
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

/**
 * 题目4：实现线程安全的对象池
 */
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

/**
 * 扩展：可增长的对象池
 */
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
                return nullptr;  // 达到最大容量
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
    std::mutex mutex_;
};

// 测试用对象
struct Connection {
    int id;
    bool active;

    Connection() : id(nextId_++), active(false) {
        std::cout << "Connection " << id << " created\n";
    }

    ~Connection() {
        std::cout << "Connection " << id << " destroyed\n";
    }

    void connect() {
        active = true;
        std::cout << "Connection " << id << " connected\n";
    }

    void disconnect() {
        active = false;
        std::cout << "Connection " << id << " disconnected\n";
    }

    void reset() {
        active = false;
        std::cout << "Connection " << id << " reset\n";
    }

    static int nextId_;
};

int Connection::nextId_ = 1;

int main() {
    std::cout << "=== 测试简单对象池 ===\n";
    {
        SimpleObjectPool<Connection> pool(3);
        std::cout << "Available: " << pool.available() << "\n";

        Connection* c1 = pool.acquire();
        c1->connect();

        Connection* c2 = pool.acquire();
        c2->connect();

        std::cout << "After acquire 2: available = " << pool.available() << "\n";

        pool.release(c1);
        std::cout << "After release 1: available = " << pool.available() << "\n";

        Connection* c3 = pool.acquire();
        std::cout << "c3 id: " << c3->id << " (should reuse c1)\n";
    }

    Connection::nextId_ = 1;  // 重置 ID

    std::cout << "\n=== 测试带重置的对象池 ===\n";
    {
        ResettableObjectPool<Connection> pool(2, [](Connection& c) {
            c.reset();
        });

        Connection* c1 = pool.acquire();
        c1->connect();
        std::cout << "c1 active: " << (c1->active ? "yes" : "no") << "\n";

        pool.release(c1);  // 会自动调用 reset

        Connection* c2 = pool.acquire();
        std::cout << "c2 active after reuse: " << (c2->active ? "yes" : "no") << "\n";
    }

    Connection::nextId_ = 1;

    std::cout << "\n=== 测试智能指针对象池 ===\n";
    {
        SmartObjectPool<Connection> pool(2);

        {
            auto c1 = pool.acquire();
            c1->connect();
            std::cout << "Inside scope: available = " << pool.available() << "\n";
            // c1 离开作用域时自动归还
        }

        std::cout << "After scope: available = " << pool.available() << "\n";

        auto c2 = pool.acquire();
        std::cout << "c2 id: " << c2->id << " (should reuse)\n";
    }

    return 0;
}
