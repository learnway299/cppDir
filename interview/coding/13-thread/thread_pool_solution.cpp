/**
 * @file thread_pool_solution.cpp
 * @brief 线程池实现 - 解答
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <memory>

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount) : stop_(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers_.emplace_back([this] { workerThread(); });
        }
    }

    ~ThreadPool() {
        shutdown();
    }

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
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

    void shutdown() {
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

    size_t size() const { return workers_.size(); }

    size_t pendingTasks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }

private:
    void workerThread() {
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

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;
};

// 带优先级的线程池
class PriorityThreadPool {
public:
    explicit PriorityThreadPool(size_t threadCount) : stop_(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers_.emplace_back([this] { workerThread(); });
        }
    }

    ~PriorityThreadPool() {
        shutdown();
    }

    template <typename F>
    void submit(F&& f, int priority = 0) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push({std::forward<F>(f), priority});
        }
        cv_.notify_one();
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) {
            if (w.joinable()) w.join();
        }
    }

private:
    struct Task {
        std::function<void()> func;
        int priority;
        bool operator<(const Task& other) const {
            return priority < other.priority;
        }
    };

    void workerThread() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                if (stop_ && tasks_.empty()) return;
                task = std::move(const_cast<Task&>(tasks_.top()).func);
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers_;
    std::priority_queue<Task> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;
};

int main() {
    std::cout << "=== 测试线程池 ===\n";
    {
        ThreadPool pool(4);

        std::vector<std::future<int>> results;
        for (int i = 0; i < 10; ++i) {
            results.push_back(pool.submit([i] {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                return i * i;
            }));
        }

        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "Result[" << i << "] = " << results[i].get() << "\n";
        }
    }

    std::cout << "\n=== 测试优先级线程池 ===\n";
    {
        PriorityThreadPool pool(2);
        std::mutex printMutex;

        for (int i = 0; i < 5; ++i) {
            pool.submit([i, &printMutex] {
                std::lock_guard<std::mutex> lock(printMutex);
                std::cout << "Low priority task " << i << "\n";
            }, 1);
        }

        for (int i = 0; i < 5; ++i) {
            pool.submit([i, &printMutex] {
                std::lock_guard<std::mutex> lock(printMutex);
                std::cout << "High priority task " << i << "\n";
            }, 10);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
