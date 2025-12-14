/**
 * @file coroutine_pool.h
 * @brief 协程池 - 题目说明与声明
 *
 * 实现协程池，复用协程对象以减少内存分配。
 *
 * 包含题目：
 * 1. 协程内存池
 * 2. 带自定义分配器的 Task
 * 3. 可复用 Task
 * 4. 协程缓存
 * 5. 协程池化执行器
 */

#ifndef COROUTINE_POOL_H
#define COROUTINE_POOL_H

#include <coroutine>
#include <queue>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <stack>
#include <optional>

namespace CoroutinePoolImpl {

// ==================== 题目1: 协程内存池 ====================

/**
 * 题目1: 实现协程内存池
 *
 * 预分配内存块，用于协程帧的分配
 *
 * 示例：
 *   CoroutineMemoryPool pool;
 *   void* ptr = pool.allocate(256);
 *   pool.deallocate(ptr, 256);
 */
class CoroutineMemoryPool {
public:
    static constexpr size_t BLOCK_SIZE = 256;
    static constexpr size_t POOL_SIZE = 100;

    CoroutineMemoryPool();
    ~CoroutineMemoryPool();

    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);

    size_t getAllocations() const { return allocations_; }
    size_t getFallbackAllocations() const { return fallbackAllocations_; }

private:
    std::stack<char*> freeBlocks_;
    std::mutex mutex_;
    size_t allocations_ = 0;
    size_t fallbackAllocations_ = 0;
};

// 全局内存池获取函数
CoroutineMemoryPool& getMemoryPool();

// ==================== 题目2: 带自定义分配器的 Task ====================

/**
 * 题目2: 实现带自定义分配器的 Task
 *
 * 使用内存池分配协程帧
 *
 * 示例：
 *   PooledTask<int> task = someCoroutine();
 *   int result = task.get();
 */
template <typename T>
class PooledTask {
public:
    struct promise_type {
        T value;
        std::exception_ptr exception;
        std::function<void()> onComplete;

        PooledTask get_return_object();
        std::suspend_never initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept;
        void return_value(T v) { value = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }

        // 自定义分配器
        void* operator new(size_t size);
        void operator delete(void* ptr, size_t size);
    };

    using handle_type = std::coroutine_handle<promise_type>;

    PooledTask(handle_type h) : handle_(h) {}
    PooledTask(PooledTask&& other) noexcept;
    ~PooledTask();

    T get();
    void setCompletionCallback(std::function<void()> callback);

private:
    handle_type handle_;
};

// ==================== 题目3: 可复用 Task ====================

/**
 * 题目3: 实现可复用的 Task
 *
 * Task 完成后自动归还到池中
 */
template <typename T>
class ReusableTask {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::optional<T> result;
        std::exception_ptr exception;
        std::function<void(handle_type)> recycler;
        bool reused = false;

        ReusableTask get_return_object();
        std::suspend_always initial_suspend() noexcept { return {}; }
        auto final_suspend() noexcept;
        void return_value(T v) { result = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    ReusableTask(handle_type h) : handle_(h) {}

    void setRecycler(std::function<void(handle_type)> recycler);
    void start();
    handle_type handle() { return handle_; }

private:
    handle_type handle_;
};

// ==================== 题目4: 协程缓存 ====================

/**
 * 题目4: 实现协程缓存
 *
 * 根据协程签名缓存协程对象
 */
template <typename Signature>
class CoroutineCache;

template <typename R, typename... Args>
class CoroutineCache<R(Args...)> {
public:
    using TaskType = PooledTask<R>;
    using FactoryFunc = std::function<TaskType(Args...)>;

    explicit CoroutineCache(FactoryFunc factory, size_t maxSize = 10);

    TaskType acquire(Args... args);

private:
    FactoryFunc factory_;
    size_t maxSize_;
};

// ==================== 题目5: 协程池化执行器 ====================

/**
 * 题目5: 实现协程池化的执行器
 *
 * 使用线程池执行协程任务
 *
 * 示例：
 *   PooledExecutor executor(4);
 *   auto task = executor.execute([]() { return 42; });
 *   int result = task.get();
 */
class PooledExecutor {
public:
    explicit PooledExecutor(size_t numWorkers);
    ~PooledExecutor();

    template <typename F>
    auto execute(F&& func) -> PooledTask<decltype(func())>;

    void shutdown();

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::coroutine_handle<>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_;
};

// ==================== 简单协程池 ====================

class SimpleCoroutinePool {
public:
    explicit SimpleCoroutinePool(size_t poolSize);

    struct Stats {
        size_t totalCreated = 0;
        size_t totalReused = 0;
        size_t currentActive = 0;
    };

    Stats getStats() const { return stats_; }

private:
    Stats stats_;
};

// ==================== 测试函数 ====================

void runTests();

} // namespace CoroutinePoolImpl

#endif // COROUTINE_POOL_H
