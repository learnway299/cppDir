/**
 * @file async_io.cpp
 * @brief 异步 IO - 面试题
 *
 * 使用协程实现异步 IO 操作。
 */
#include <coroutine>
#include <functional>
#include <queue>
#include <chrono>
#include <string>

// 前置声明
template <typename T> class Task;
class EventLoop;

// 题目1: 实现 async_sleep
// 异步等待指定毫秒
// Task<void> async_sleep(EventLoop& loop, int ms);

// 题目2: 实现简单的事件循环
class EventLoop {
public:
    // 运行事件循环
    void run() { /* TODO */ }

    // 调度一个任务
    void schedule(std::function<void()> task) { /* TODO */ }

    // 调度一个延迟任务
    void schedule_after(int ms, std::function<void()> task) { /* TODO */ }

    // 停止事件循环
    void stop() { /* TODO */ }

private:
    // TODO: 任务队列
    // TODO: 定时器管理
};

// 题目3: 实现异步文件读取（模拟）
// Task<std::string> async_read_file(EventLoop& loop, const std::string& filename);

// 题目4: 实现异步网络请求（模拟）
// Task<std::string> async_http_get(EventLoop& loop, const std::string& url);

// 题目5: 实现超时包装器
// template <typename T>
// Task<std::optional<T>> with_timeout(Task<T> task, int timeout_ms);

int main() {
    return 0;
}
