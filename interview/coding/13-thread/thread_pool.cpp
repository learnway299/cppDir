/**
 * @file thread_pool.cpp
 * @brief 线程池实现 - 面试题实现
 */

#include "thread_pool.h"

namespace ThreadPool {

// ==================== 面试者实现 ====================

ThreadPool::ThreadPool(size_t threadCount) {
    // TODO: 创建工作线程
}

ThreadPool::~ThreadPool() {
    // TODO: 关闭线程池
}

template <typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {
    using ReturnType = typename std::invoke_result<F, Args...>::type;
    // TODO
    return std::future<ReturnType>();
}

void ThreadPool::shutdown() {
    // TODO
}

size_t ThreadPool::size() const {
    // TODO
    return 0;
}

void ThreadPool::workerThread() {
    // TODO
}

// Explicit template instantiation
template auto ThreadPool::submit(std::function<int()>&&)
    -> std::future<int>;

} // namespace ThreadPool
