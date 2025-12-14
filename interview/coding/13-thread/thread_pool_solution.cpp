/**
 * @file thread_pool_solution.cpp
 * @brief 线程池实现 - 参考答案
 */

#include "thread_pool.h"
#include <cassert>

namespace ThreadPool {

// ==================== 参考答案实现 ====================

ThreadPoolSolution::ThreadPoolSolution(size_t threadCount) : stop_(false) {
    for (size_t i = 0; i < threadCount; ++i) {
        workers_.emplace_back([this] { workerThread(); });
    }
}

ThreadPoolSolution::~ThreadPoolSolution() {
    shutdown();
}

template <typename F, typename... Args>
auto ThreadPoolSolution::submit(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {

    using ReturnType = typename std::invoke_result<F, Args...>::type;

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stop_) {
            throw std::runtime_error("ThreadPool is stopped");
        }
        tasks_.emplace([task]() { (*task)(); });
    }

    cv_.notify_one();
    return result;
}

void ThreadPoolSolution::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stop_) return;
        stop_ = true;
    }
    cv_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPoolSolution::size() const { return workers_.size(); }

size_t ThreadPoolSolution::pendingTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.size();
}

void ThreadPoolSolution::workerThread() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty()) {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        task();
    }
}

// Explicit template instantiation
template auto ThreadPoolSolution::submit(std::function<int()>&&)
    -> std::future<int>;

// ==================== 测试函数 ====================

void runTests() {
    std::cout << "=== Thread Pool Tests ===" << std::endl;

    // 测试基本功能
    {
        ThreadPoolSolution pool(4);

        auto future1 = pool.submit([] { return 42; });
        auto future2 = pool.submit([] { return 100; });

        assert(future1.get() == 42);
        assert(future2.get() == 100);
    }
    std::cout << "  Basic Functionality: PASSED" << std::endl;

    // 测试多任务并发执行
    {
        ThreadPoolSolution pool(4);
        std::atomic<int> counter{0};

        std::vector<std::future<void>> futures;
        for (int i = 0; i < 100; ++i) {
            futures.push_back(pool.submit([&counter] {
                ++counter;
            }));
        }

        for (auto& f : futures) {
            f.get();
        }

        assert(counter == 100);
    }
    std::cout << "  Concurrent Execution: PASSED" << std::endl;

    // 测试带返回值的任务
    {
        ThreadPoolSolution pool(2);

        auto future = pool.submit([](int a, int b) {
            return a + b;
        }, 10, 20);

        assert(future.get() == 30);
    }
    std::cout << "  Tasks with Return Values: PASSED" << std::endl;
}

} // namespace ThreadPool
