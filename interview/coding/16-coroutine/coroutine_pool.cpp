/**
 * @file coroutine_pool.cpp
 * @brief 协程池 - 面试题
 *
 * 实现协程池，复用协程对象以减少内存分配。
 */
#include <coroutine>
#include <queue>
#include <functional>
#include <memory>

// 题目1: 实现基本的协程池
// 预分配协程对象，复用而不是每次创建销毁
class CoroutinePool {
public:
    explicit CoroutinePool(size_t poolSize) { /* TODO */ }

    // 获取一个协程来执行任务
    std::coroutine_handle<> acquire() { return nullptr; /* TODO */ }

    // 归还协程到池中
    void release(std::coroutine_handle<> handle) { /* TODO */ }

private:
    // TODO: 协程对象池
};

// 题目2: 实现可复用的 Task
// Task 完成后自动归还到池中
template <typename T>
class PooledTask {
public:
    struct promise_type {
        // TODO: 实现 promise_type，支持复用
    };

    // TODO: 实现 Task 接口

private:
    // TODO
};

// 题目3: 实现协程缓存
// 根据协程签名缓存协程对象
class CoroutineCache {
public:
    // TODO
};

// 题目4: 实现自定义分配器
// 控制协程帧的内存分配
struct PoolAllocator {
    // TODO: 实现 operator new 和 operator delete
};

// 题目5: 实现协程池化的执行器
class PooledExecutor {
public:
    explicit PooledExecutor(size_t poolSize) { /* TODO */ }

    template <typename F>
    void execute(F&& func) { /* TODO */ }

private:
    // TODO
};

int main() {
    return 0;
}
