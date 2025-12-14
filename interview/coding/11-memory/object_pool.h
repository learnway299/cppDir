/**
 * @file object_pool.h
 * @brief 对象池实现 - 题目说明与声明
 *
 * 包含题目：
 * 1. 简单对象池
 * 2. 带重置功能的对象池
 * 3. RAII 包装的对象池
 * 4. 线程安全的对象池
 * 5. 可增长的对象池
 */

#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

namespace ObjectPoolImpl {

// ==================== 题目1: 简单对象池 ====================

/**
 * 题目1: 实现简单对象池
 *
 * 特点：
 * 1. 预创建一定数量的对象
 * 2. acquire() 获取对象，O(1)
 * 3. release() 归还对象，O(1)
 * 4. 对象复用，避免频繁创建销毁
 *
 * 应用场景：
 * - 数据库连接池
 * - 线程池
 * - 游戏对象池（子弹、特效等）
 *
 * 示例：
 *   SimpleObjectPool<Connection> pool(10);
 *   Connection* conn = pool.acquire();
 *   conn->execute("SELECT ...");
 *   pool.release(conn);
 */
template <typename T>
class SimpleObjectPool {
public:
    explicit SimpleObjectPool(size_t size);

    T* acquire();
    void release(T* obj);

    size_t available() const;
    size_t capacity() const;

private:
    std::vector<std::unique_ptr<T>> objects_;  // 所有对象
    std::vector<T*> available_;                 // 可用对象
};

// ==================== 题目2: 带重置功能的对象池 ====================

/**
 * 题目2: 实现带重置功能的对象池
 *
 * 特点：
 * 1. 对象归还时自动重置状态
 * 2. 支持自定义重置函数
 * 3. 确保对象再次使用时是"干净"的
 *
 * 示例：
 *   ResettableObjectPool<Buffer> pool(10, [](Buffer& b) {
 *       b.clear();
 *   });
 */
template <typename T>
class ResettableObjectPool {
public:
    using ResetFunc = std::function<void(T&)>;

    explicit ResettableObjectPool(size_t size, ResetFunc resetFunc = nullptr);

    T* acquire();
    void release(T* obj);

    size_t available() const;

private:
    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
    ResetFunc resetFunc_;
};

// ==================== 题目3: RAII 包装的对象池 ====================

/**
 * 题目3: 实现 RAII 包装的对象池
 *
 * 特点：
 * 1. acquire() 返回智能指针
 * 2. 智能指针析构时自动归还对象
 * 3. 避免忘记 release 导致的泄漏
 *
 * 实现要点：
 * - 自定义删除器，归还而非删除
 * - 删除器需要持有池的指针
 *
 * 示例：
 *   SmartObjectPool<Connection> pool(10);
 *   {
 *       auto conn = pool.acquire();  // 返回 unique_ptr
 *       conn->execute("...");
 *   }  // 自动归还
 */
template <typename T>
class SmartObjectPool {
public:
    class Deleter {
    public:
        explicit Deleter(SmartObjectPool* pool = nullptr);
        void operator()(T* ptr) const;

    private:
        SmartObjectPool* pool_;
    };

    using Ptr = std::unique_ptr<T, Deleter>;

    explicit SmartObjectPool(size_t size);

    Ptr acquire();
    size_t available() const;

private:
    void release(T* obj);

    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
};

// ==================== 题目4: 线程安全的对象池 ====================

/**
 * 题目4: 实现线程安全的对象池
 *
 * 特点：
 * 1. 支持多线程并发访问
 * 2. 使用互斥锁保护
 * 3. acquire/release 都是线程安全的
 */
template <typename T>
class ThreadSafeObjectPool {
public:
    explicit ThreadSafeObjectPool(size_t size);

    T* acquire();
    void release(T* obj);
    size_t available() const;

private:
    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
    mutable std::mutex mutex_;
};

// ==================== 题目5: 可增长的对象池 ====================

/**
 * 题目5: 实现可增长的对象池
 *
 * 特点：
 * 1. 初始创建一定数量对象
 * 2. 不够用时自动增长
 * 3. 有最大容量限制
 */
template <typename T>
class GrowableObjectPool {
public:
    explicit GrowableObjectPool(size_t initialSize = 16, size_t maxSize = 1024);

    T* acquire();
    void release(T* obj);
    size_t capacity() const;

private:
    void grow(size_t count);

    std::vector<std::unique_ptr<T>> objects_;
    std::vector<T*> available_;
    size_t maxSize_;
    mutable std::mutex mutex_;
};

// ==================== 测试辅助类 ====================

struct Connection {
    int id;
    bool active;
    static int nextId_;
    static int instanceCount;

    Connection();
    ~Connection();

    void connect();
    void disconnect();
    void reset();
};

// ==================== 测试函数声明 ====================

void testObjectPool();          // 测试面试者实现
void testObjectPoolSolution();  // 测试解答实现

// runTests 在头文件 inline 定义，调用两个测试函数
inline void runTests() {
    testObjectPool();
    testObjectPoolSolution();
}

} // namespace ObjectPoolImpl

#endif // OBJECT_POOL_H
