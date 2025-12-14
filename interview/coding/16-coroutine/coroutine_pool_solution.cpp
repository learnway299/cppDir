/**
 * @file coroutine_pool_solution.cpp
 * @brief 协程池 - 参考答案
 */

#include "coroutine_pool.h"
#include <iostream>
#include <cassert>
#include <chrono>

namespace CoroutinePoolImpl {

namespace Solution {

// ==================== 题目1: 协程内存池 ====================

class CoroutineMemoryPool {
public:
    static constexpr size_t BLOCK_SIZE = 256;
    static constexpr size_t POOL_SIZE = 100;

    CoroutineMemoryPool() {
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            freeBlocks_.push(new char[BLOCK_SIZE]);
        }
    }

    ~CoroutineMemoryPool() {
        while (!freeBlocks_.empty()) {
            delete[] freeBlocks_.top();
            freeBlocks_.pop();
        }
    }

    void* allocate(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size <= BLOCK_SIZE && !freeBlocks_.empty()) {
            void* ptr = freeBlocks_.top();
            freeBlocks_.pop();
            ++allocations_;
            return ptr;
        }
        // 超大或池空，使用默认分配
        ++fallbackAllocations_;
        return ::operator new(size);
    }

    void deallocate(void* ptr, size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size <= BLOCK_SIZE && freeBlocks_.size() < POOL_SIZE) {
            freeBlocks_.push(static_cast<char*>(ptr));
        } else {
            ::operator delete(ptr);
        }
    }

    size_t getAllocations() const { return allocations_; }
    size_t getFallbackAllocations() const { return fallbackAllocations_; }

private:
    std::stack<char*> freeBlocks_;
    std::mutex mutex_;
    size_t allocations_ = 0;
    size_t fallbackAllocations_ = 0;
};

// 全局内存池
CoroutineMemoryPool& getMemoryPool() {
    static CoroutineMemoryPool pool;
    return pool;
}

// ==================== 题目2: 带自定义分配器的 Task ====================

template <typename T>
class PooledTask {
public:
    struct promise_type {
        T value;
        std::exception_ptr exception;
        std::function<void()> onComplete;

        PooledTask get_return_object() {
            return PooledTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_never initial_suspend() noexcept { return {}; }

        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }
                void await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                    if (h.promise().onComplete) {
                        h.promise().onComplete();
                    }
                }
                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }

        void return_value(T v) { value = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }

        // 自定义分配器
        void* operator new(size_t size) {
            return getMemoryPool().allocate(size);
        }

        void operator delete(void* ptr, size_t size) {
            getMemoryPool().deallocate(ptr, size);
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    PooledTask(handle_type h) : handle_(h) {}

    PooledTask(PooledTask&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    ~PooledTask() {
        if (handle_) {
            handle_.destroy();
        }
    }

    T get() {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return handle_.promise().value;
    }

    void setCompletionCallback(std::function<void()> callback) {
        handle_.promise().onComplete = std::move(callback);
    }

private:
    handle_type handle_;
};

// ==================== 题目3: 可复用 Task ====================

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

        ReusableTask get_return_object() {
            return ReusableTask{handle_type::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept { return {}; }

        auto final_suspend() noexcept {
            struct FinalAwaiter {
                bool await_ready() noexcept { return false; }
                void await_suspend(handle_type h) noexcept {
                    // 重置状态
                    h.promise().result.reset();
                    h.promise().exception = nullptr;
                    // 归还到池中
                    if (h.promise().recycler) {
                        h.promise().recycler(h);
                    }
                }
                void await_resume() noexcept {}
            };
            return FinalAwaiter{};
        }

        void return_value(T v) { result = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    ReusableTask(handle_type h) : handle_(h) {}

    void setRecycler(std::function<void(handle_type)> recycler) {
        handle_.promise().recycler = std::move(recycler);
    }

    void start() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
        }
    }

    handle_type handle() { return handle_; }

private:
    handle_type handle_;
};

// ==================== 题目5: 协程池化执行器 ====================

class PooledExecutor {
public:
    explicit PooledExecutor(size_t numWorkers)
        : running_(true) {
        for (size_t i = 0; i < numWorkers; ++i) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }

    ~PooledExecutor() {
        shutdown();
    }

    template <typename F>
    auto execute(F&& func) -> PooledTask<decltype(func())> {
        using R = decltype(func());

        auto task = [](F f, PooledExecutor*) -> PooledTask<R> {
            co_return f();
        }(std::forward<F>(func), this);

        return task;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();

        for (auto& w : workers_) {
            if (w.joinable()) {
                w.join();
            }
        }
    }

private:
    void workerLoop() {
        while (running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(100), [this] {
                return !running_ || !tasks_.empty();
            });

            if (!running_ && tasks_.empty()) break;

            if (!tasks_.empty()) {
                auto handle = tasks_.front();
                tasks_.pop();
                lock.unlock();

                if (!handle.done()) {
                    handle.resume();
                }
            }
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::coroutine_handle<>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_;
};

// ==================== 简单协程池 ====================

class SimpleCoroutinePool {
public:
    explicit SimpleCoroutinePool(size_t) {}

    struct Stats {
        size_t totalCreated = 0;
        size_t totalReused = 0;
        size_t currentActive = 0;
    };

    Stats getStats() const { return stats_; }

private:
    Stats stats_;
};

// ==================== 测试辅助协程 ====================

PooledTask<int> computeTask(int x) {
    co_return x * x;
}

} // namespace Solution

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Coroutine Pool Tests ===" << std::endl;

    // 测试内存池分配
    {
        std::vector<Solution::PooledTask<int>> tasks;
        for (int i = 0; i < 10; ++i) {
            tasks.push_back(Solution::computeTask(i));
        }

        int sum = 0;
        for (auto& task : tasks) {
            sum += task.get();
        }
        // 0 + 1 + 4 + 9 + 16 + 25 + 36 + 49 + 64 + 81 = 285
        assert(sum == 285);
    }
    std::cout << "  Memory Pool Allocation: PASSED" << std::endl;

    // 测试池化执行器
    {
        Solution::PooledExecutor executor(2);

        auto task1 = executor.execute([]() {
            return 42;
        });

        auto task2 = executor.execute([]() {
            return 100;
        });

        assert(task1.get() == 42);
        assert(task2.get() == 100);
    }
    std::cout << "  Pooled Executor: PASSED" << std::endl;

    // 测试简单协程池
    {
        Solution::SimpleCoroutinePool pool(10);
        auto stats = pool.getStats();
        assert(stats.currentActive == 0);
    }
    std::cout << "  Simple Coroutine Pool: PASSED" << std::endl;
}

} // namespace CoroutinePoolImpl

/**
 * 关键要点：
 *
 * 1. 协程内存管理：
 *    - 协程帧由编译器在堆上分配
 *    - 可通过 promise_type 自定义 operator new/delete
 *    - 使用内存池减少分配开销
 *
 * 2. 协程复用的挑战：
 *    - 协程状态包含局部变量
 *    - 需要重置所有状态
 *    - 编译器生成的代码难以修改
 *
 * 3. 实用方案：
 *    - 内存池分配协程帧
 *    - Task 完成后回调归还
 *    - 控制最大并发数
 *
 * 4. 自定义分配器：
 *    - promise_type 中定义 operator new
 *    - 可以使用任何内存池实现
 *    - 注意线程安全
 *
 * 5. 执行器模式：
 *    - 工作线程池
 *    - 任务队列
 *    - 协程调度
 *
 * 6. 性能考虑：
 *    - 减少内存分配
 *    - 缓存友好的布局
 *    - 避免伪共享
 *
 * 7. 实际库参考：
 *    - folly::coro::Task
 *    - cppcoro
 *    - libcoro
 */
