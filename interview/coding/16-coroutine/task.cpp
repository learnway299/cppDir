/**
 * @file task.cpp
 * @brief Task 协程 - 面试题
 *
 * Task 是一种表示异步操作的协程类型，支持 co_await。
 */
#include <coroutine>
#include <iostream>
#include <optional>
#include <exception>

// 题目1: 实现基本的 Task<T> 类型
// 表示一个返回 T 类型结果的异步任务
template <typename T>
class Task {
public:
    struct promise_type {
        // TODO: 实现 promise_type
    };

    // TODO: 实现 Task 类
    // - operator co_await()
    // - get() 获取结果

private:
    // TODO
};

// Task<void> 特化
template <>
class Task<void> {
public:
    struct promise_type {
        // TODO: 实现
    };

    // TODO: 实现
};

// 题目2: 实现简单的异步操作
Task<int> async_add(int a, int b) {
    // TODO: 模拟异步计算
    co_return a + b;
}

// 题目3: 实现任务链
Task<int> chained_task() {
    // TODO: 链式调用多个异步任务
    co_return 0;
}

// 题目4: 实现 when_all - 等待所有任务完成
template <typename... Tasks>
Task<void> when_all(Tasks... tasks) {
    // TODO: 实现
    co_return;
}

// 题目5: 实现 when_any - 等待任意一个任务完成
template <typename T>
Task<T> when_any(Task<T> task1, Task<T> task2) {
    // TODO: 实现
    co_return T{};
}

int main() {
    return 0;
}
