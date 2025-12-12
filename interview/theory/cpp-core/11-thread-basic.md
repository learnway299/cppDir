# 11 - 多线程基础 (Thread Basics)

## 简介

C++11 引入了标准的多线程库，提供了跨平台的线程、同步原语和并发工具。理解多线程编程对于编写高性能、响应式的现代应用至关重要。

**核心特点**：
- **`std::thread`**：线程管理
- **互斥锁**：保护共享资源
- **条件变量**：线程间通信
- **原子操作**：无锁编程基础

---

## 1. `std::thread` 基础

### 1.1 如何创建和管理线程？

**问题描述**：线程的创建、启动和结束。

**基本用法**：

```cpp
#include <iostream>
#include <thread>

// 线程函数
void hello() {
    std::cout << "Hello from thread!\n";
}

void thread_basic() {
    // 创建线程
    std::thread t(hello);

    // 等待线程完成
    t.join();

    std::cout << "Thread finished\n";
}
```

**带参数的线程**：

```cpp
void print_sum(int a, int b) {
    std::cout << "Sum: " << (a + b) << "\n";
}

void print_message(const std::string& msg) {
    std::cout << msg << "\n";
}

void thread_with_args() {
    // 按值传递参数
    std::thread t1(print_sum, 10, 20);

    // 字符串参数
    std::string msg = "Hello Thread";
    std::thread t2(print_message, msg);

    t1.join();
    t2.join();
}
```

**使用 Lambda 表达式**：

```cpp
void thread_lambda() {
    int x = 42;

    // Lambda 作为线程函数
    std::thread t([x]() {
        std::cout << "Value: " << x << "\n";
    });

    t.join();

    // 捕获引用（注意生命周期）
    std::thread t2([&x]() {
        x += 10;
    });
    t2.join();

    std::cout << "Modified x: " << x << "\n";  // 52
}
```

**成员函数作为线程函数**：

```cpp
class Worker {
public:
    void do_work(int n) {
        for (int i = 0; i < n; ++i) {
            std::cout << "Working... " << i << "\n";
        }
    }
};

void thread_member_function() {
    Worker worker;

    // 传递成员函数和对象指针
    std::thread t(&Worker::do_work, &worker, 5);
    t.join();
}
```

**关键要点**：
- 线程创建后立即开始执行
- 必须调用 `join()` 或 `detach()`，否则程序终止时抛异常
- 参数默认按值拷贝到线程内部

---

### 1.2 `join()` vs `detach()` 的区别？

**问题描述**：两种线程等待方式的差异。

```cpp
void join_vs_detach() {
    // join(): 阻塞当前线程，等待子线程完成
    std::thread t1([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Thread 1 finished\n";
    });
    t1.join();  // 主线程等待 t1 完成
    std::cout << "After join\n";

    // detach(): 分离线程，后台运行
    std::thread t2([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Thread 2 finished\n";
    });
    t2.detach();  // t2 在后台运行，主线程不等待
    std::cout << "After detach\n";

    // 注意：主线程结束时，detach 的线程可能还在运行
    std::this_thread::sleep_for(std::chrono::seconds(2));  // 等待 detach 线程
}
```

**RAII 风格的线程管理**：

```cpp
class ThreadGuard {
    std::thread& t;
public:
    explicit ThreadGuard(std::thread& t_) : t(t_) {}

    ~ThreadGuard() {
        if (t.joinable()) {
            t.join();  // 确保线程被 join
        }
    }

    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};

void raii_thread() {
    std::thread t([]() {
        std::cout << "Working...\n";
    });

    ThreadGuard guard(t);
    // 即使发生异常，析构函数也会 join 线程
}
```

**关键要点**：
- **`join()`**：阻塞等待线程完成，可以获取线程执行结果
- **`detach()`**：分离线程，主线程不再管理，线程独立运行
- 线程对象销毁前必须调用 `join()` 或 `detach()`
- `joinable()` 检查线程是否可以 join

**常见陷阱**：
```cpp
// ❌ 错误：忘记 join 或 detach
void bad_thread() {
    std::thread t([]() { /* ... */ });
    // 函数结束时 t 析构，程序 terminate
}

// ❌ 错误：detach 后访问局部变量
void bad_detach() {
    int local_var = 42;
    std::thread t([&local_var]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << local_var << "\n";  // ❌ local_var 可能已销毁
    });
    t.detach();
    // 函数结束，local_var 销毁
}
```

---

### 1.3 线程标识和线程数量？

**问题描述**：如何获取线程 ID 和硬件并发数？

```cpp
#include <thread>

void thread_id_demo() {
    // 获取当前线程 ID
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::cout << "Main thread ID: " << main_thread_id << "\n";

    std::thread t([]() {
        std::thread::id worker_id = std::this_thread::get_id();
        std::cout << "Worker thread ID: " << worker_id << "\n";
    });

    // 获取线程对象的 ID
    std::cout << "Thread object ID: " << t.get_id() << "\n";
    t.join();

    // 硬件并发数（CPU 核心数）
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "Hardware concurrency: " << n << "\n";
}
```

**线程休眠**：

```cpp
void thread_sleep() {
    std::cout << "Start\n";

    // 休眠 1 秒
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 休眠到指定时间点
    auto wake_time = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    std::this_thread::sleep_until(wake_time);

    // 主动让出 CPU
    std::this_thread::yield();

    std::cout << "End\n";
}
```

---

## 2. 互斥锁 (Mutex)

### 2.1 `std::mutex` 的基本使用？

**问题描述**：如何保护共享资源？

**竞态条件问题**：

```cpp
#include <mutex>
#include <vector>

// ❌ 问题代码：竞态条件
int counter = 0;

void bad_increment() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;  // 非原子操作，多线程不安全
    }
}

void race_condition_demo() {
    std::thread t1(bad_increment);
    std::thread t2(bad_increment);
    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << "\n";  // 结果不确定！可能 < 200000
}
```

**使用 mutex 解决**：

```cpp
int safe_counter = 0;
std::mutex mtx;

void safe_increment() {
    for (int i = 0; i < 100000; ++i) {
        mtx.lock();
        ++safe_counter;
        mtx.unlock();
    }
}

void mutex_demo() {
    std::thread t1(safe_increment);
    std::thread t2(safe_increment);
    t1.join();
    t2.join();

    std::cout << "Safe counter: " << safe_counter << "\n";  // 200000
}
```

**关键要点**：
- `lock()`：获取锁，阻塞直到成功
- `unlock()`：释放锁
- `try_lock()`：尝试获取锁，立即返回成功/失败

**常见陷阱**：
```cpp
// ❌ 错误：忘记 unlock
void bad_mutex() {
    mtx.lock();
    if (some_condition) {
        return;  // ❌ 锁未释放，死锁！
    }
    mtx.unlock();
}

// ❌ 错误：异常安全
void bad_exception() {
    mtx.lock();
    risky_operation();  // 可能抛异常
    mtx.unlock();       // ❌ 异常时不会执行，死锁！
}
```

---

### 2.2 `std::lock_guard` 和 `std::unique_lock`？

**问题描述**：RAII 风格的锁管理。

**`std::lock_guard`**：

```cpp
std::mutex mtx;
int counter = 0;

void increment_with_guard() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);  // 构造时 lock
        ++counter;
        // 作用域结束时自动 unlock
    }
}

// ✅ 异常安全
void exception_safe() {
    std::lock_guard<std::mutex> lock(mtx);
    risky_operation();  // 即使抛异常，lock 析构也会 unlock
}
```

**`std::unique_lock`**：

```cpp
void unique_lock_demo() {
    std::unique_lock<std::mutex> lock(mtx);

    // 手动 unlock
    lock.unlock();

    // 手动 lock
    lock.lock();

    // 延迟加锁
    std::unique_lock<std::mutex> lock2(mtx, std::defer_lock);
    // ... 做一些不需要锁的操作
    lock2.lock();  // 需要时再加锁

    // 尝试加锁
    std::unique_lock<std::mutex> lock3(mtx, std::try_to_lock);
    if (lock3.owns_lock()) {
        // 成功获取锁
    }

    // 转移所有权
    std::unique_lock<std::mutex> lock4 = std::move(lock);
}
```

**`lock_guard` vs `unique_lock` 对比**：

| 特性 | `lock_guard` | `unique_lock` |
|------|-------------|---------------|
| RAII | ✅ | ✅ |
| 手动 lock/unlock | ❌ | ✅ |
| 移动语义 | ❌ | ✅ |
| 条件变量 | ❌ | ✅ |
| 性能 | 更快（简单） | 稍慢（灵活） |

**C++17 `std::scoped_lock`**：

```cpp
// 同时锁定多个 mutex，避免死锁
std::mutex mtx1, mtx2;

void scoped_lock_demo() {
    std::scoped_lock lock(mtx1, mtx2);  // 同时锁定，避免死锁
    // ... 使用 mtx1 和 mtx2 保护的资源
}
```

**关键要点**：
- **`lock_guard`**：简单、轻量、不可移动
- **`unique_lock`**：灵活、可移动、支持条件变量
- **`scoped_lock`** (C++17)：多锁管理，避免死锁

---

### 2.3 如何避免死锁？

**问题描述**：多个线程相互等待对方持有的锁。

**死锁示例**：

```cpp
std::mutex mtx1, mtx2;

// ❌ 死锁场景
void thread1() {
    std::lock_guard<std::mutex> lock1(mtx1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> lock2(mtx2);  // 等待 mtx2
}

void thread2() {
    std::lock_guard<std::mutex> lock2(mtx2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> lock1(mtx1);  // 等待 mtx1 → 死锁！
}
```

**解决方案 1：固定锁顺序**：

```cpp
// ✅ 总是按相同顺序加锁
void thread1_fixed() {
    std::lock_guard<std::mutex> lock1(mtx1);
    std::lock_guard<std::mutex> lock2(mtx2);
}

void thread2_fixed() {
    std::lock_guard<std::mutex> lock1(mtx1);  // 顺序一致
    std::lock_guard<std::mutex> lock2(mtx2);
}
```

**解决方案 2：`std::lock` 同时加锁**：

```cpp
void thread_with_std_lock() {
    // 同时锁定多个 mutex，避免死锁
    std::unique_lock<std::mutex> lock1(mtx1, std::defer_lock);
    std::unique_lock<std::mutex> lock2(mtx2, std::defer_lock);

    std::lock(lock1, lock2);  // 原子地锁定两个 mutex
    // ... 使用资源
}
```

**解决方案 3：`std::scoped_lock` (C++17)**：

```cpp
void thread_scoped_lock() {
    std::scoped_lock lock(mtx1, mtx2);  // 自动避免死锁
    // ... 使用资源
}
```

**避免死锁的原则**：
1. **固定加锁顺序**：所有线程按相同顺序加锁
2. **同时加锁**：使用 `std::lock` 或 `std::scoped_lock`
3. **避免嵌套锁**：尽量只持有一个锁
4. **使用超时**：`try_lock_for` / `try_lock_until`
5. **层次锁**：给锁分配优先级

---

## 3. 条件变量 (Condition Variable)

### 3.1 `std::condition_variable` 的使用？

**问题描述**：线程间如何通信和同步？

**基本用法**：

```cpp
#include <condition_variable>
#include <queue>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void wait_thread() {
    std::unique_lock<std::mutex> lock(mtx);

    // 等待条件满足
    cv.wait(lock, []{ return ready; });

    std::cout << "Condition met, proceeding...\n";
}

void notify_thread() {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }

    cv.notify_one();  // 通知一个等待的线程
}

void condition_variable_demo() {
    std::thread t1(wait_thread);
    std::thread t2(notify_thread);

    t1.join();
    t2.join();
}
```

**条件变量的三个关键操作**：
1. **`wait(lock, predicate)`**：等待条件满足
2. **`notify_one()`**：唤醒一个等待的线程
3. **`notify_all()`**：唤醒所有等待的线程

**为什么需要 predicate？**

```cpp
// ❌ 错误：没有 predicate，可能虚假唤醒
cv.wait(lock);  // 可能在条件不满足时被唤醒

// ✅ 正确：使用 predicate
cv.wait(lock, []{ return ready; });  // 等价于：
// while (!ready) {
//     cv.wait(lock);
// }
```

**关键要点**：
- **必须使用 `unique_lock`**：条件变量需要手动 unlock/lock
- **虚假唤醒**：`wait` 可能在条件不满足时返回，必须使用 predicate
- **通知前先修改状态**：确保等待线程看到最新状态

---

### 3.2 生产者-消费者模型？

**问题描述**：经典的线程同步问题。

```cpp
#include <queue>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv_not_empty;
std::condition_variable cv_not_full;
const size_t MAX_BUFFER_SIZE = 10;
bool finished = false;

// 生产者
void producer(int id) {
    for (int i = 0; i < 20; ++i) {
        std::unique_lock<std::mutex> lock(mtx);

        // 等待缓冲区不满
        cv_not_full.wait(lock, []{ return buffer.size() < MAX_BUFFER_SIZE; });

        buffer.push(i);
        std::cout << "Producer " << id << " produced: " << i << "\n";

        lock.unlock();
        cv_not_empty.notify_one();  // 通知消费者

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 消费者
void consumer(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // 等待缓冲区不空或生产结束
        cv_not_empty.wait(lock, []{ return !buffer.empty() || finished; });

        if (buffer.empty() && finished) {
            break;  // 生产结束且缓冲区空，退出
        }

        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumer " << id << " consumed: " << item << "\n";

        lock.unlock();
        cv_not_full.notify_one();  // 通知生产者

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

void producer_consumer_demo() {
    std::thread p1(producer, 1);
    std::thread c1(consumer, 1);
    std::thread c2(consumer, 2);

    p1.join();

    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
    }
    cv_not_empty.notify_all();  // 通知所有消费者

    c1.join();
    c2.join();
}
```

**关键要点**：
- 使用两个条件变量分别表示"不满"和"不空"
- 生产者等待缓冲区不满，消费者等待缓冲区不空
- 通知时要解锁，避免不必要的竞争

---

## 4. 其他同步原语

### 4.1 `std::call_once` 的使用？

**问题描述**：如何确保函数只执行一次（线程安全的单例）？

```cpp
#include <mutex>

std::once_flag flag;

void initialize() {
    std::cout << "Initialization (called once)\n";
}

void thread_func() {
    std::call_once(flag, initialize);  // 只有一个线程会执行
    std::cout << "Thread " << std::this_thread::get_id() << " running\n";
}

void call_once_demo() {
    std::thread t1(thread_func);
    std::thread t2(thread_func);
    std::thread t3(thread_func);

    t1.join();
    t2.join();
    t3.join();
}
```

**线程安全的单例模式**：

```cpp
class Singleton {
private:
    Singleton() = default;
    static std::once_flag init_flag;
    static Singleton* instance;

    static void init_instance() {
        instance = new Singleton();
    }

public:
    static Singleton& get_instance() {
        std::call_once(init_flag, init_instance);
        return *instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

std::once_flag Singleton::init_flag;
Singleton* Singleton::instance = nullptr;

// C++11 更简单的方式（局部静态变量线程安全）
class SimpleSingleton {
public:
    static SimpleSingleton& get_instance() {
        static SimpleSingleton instance;  // C++11 保证线程安全
        return instance;
    }

private:
    SimpleSingleton() = default;
};
```

---

### 4.2 `std::future` 和 `std::promise`？

**问题描述**：如何在线程间传递异步结果？

**基本用法**：

```cpp
#include <future>

void future_promise_demo() {
    // promise：生产者设置值
    std::promise<int> promise;

    // future：消费者获取值
    std::future<int> future = promise.get_future();

    // 异步线程设置值
    std::thread t([&promise]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        promise.set_value(42);  // 设置结果
    });

    std::cout << "Waiting for result...\n";
    int result = future.get();  // 阻塞等待结果
    std::cout << "Result: " << result << "\n";

    t.join();
}
```

**传递异常**：

```cpp
void promise_exception() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread t([&promise]() {
        try {
            throw std::runtime_error("Error occurred");
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });

    try {
        int result = future.get();
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }

    t.join();
}
```

**`std::async`**：

```cpp
int compute(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return x * x;
}

void async_demo() {
    // 异步执行
    std::future<int> future1 = std::async(std::launch::async, compute, 10);

    // 延迟执行（调用 get/wait 时才执行）
    std::future<int> future2 = std::async(std::launch::deferred, compute, 20);

    // 自动选择
    std::future<int> future3 = std::async(compute, 30);

    std::cout << "Result 1: " << future1.get() << "\n";  // 100
    std::cout << "Result 2: " << future2.get() << "\n";  // 400
    std::cout << "Result 3: " << future3.get() << "\n";  // 900
}
```

**`std::packaged_task`**：

```cpp
void packaged_task_demo() {
    // 打包任务
    std::packaged_task<int(int)> task(compute);
    std::future<int> future = task.get_future();

    // 在线程中执行
    std::thread t(std::move(task), 5);

    std::cout << "Result: " << future.get() << "\n";  // 25
    t.join();
}
```

**关键要点**：
- **`std::promise`**：生产者，设置异步结果
- **`std::future`**：消费者，获取异步结果
- **`std::async`**：简化异步任务启动
- **`std::packaged_task`**：可移动的任务包装

---

## 5. 线程安全问题

### 5.1 什么是线程安全？

**问题描述**：多线程环境下的正确性。

**线程不安全的代码**：

```cpp
class Counter {
    int count = 0;

public:
    void increment() {
        ++count;  // 非原子操作，线程不安全
    }

    int get() const {
        return count;  // 读取也可能不安全
    }
};
```

**线程安全的版本**：

```cpp
class SafeCounter {
    int count = 0;
    mutable std::mutex mtx;

public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        ++count;
    }

    int get() const {
        std::lock_guard<std::mutex> lock(mtx);
        return count;
    }
};
```

**常见线程安全问题**：

1. **竞态条件 (Race Condition)**：多个线程访问共享资源，结果取决于执行顺序
2. **数据竞争 (Data Race)**：至少一个线程写，另一个读/写，没有同步
3. **死锁 (Deadlock)**：线程相互等待对方持有的资源
4. **活锁 (Livelock)**：线程不断改变状态以响应其他线程
5. **优先级反转**：低优先级线程持有高优先级线程需要的锁

---

### 5.2 线程安全的单例模式？

**问题描述**：经典设计模式的线程安全实现。

**方式 1：C++11 静态局部变量（推荐）**：

```cpp
class Singleton {
public:
    static Singleton& instance() {
        static Singleton inst;  // C++11 保证线程安全
        return inst;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
```

**方式 2：`std::call_once`**：

```cpp
class Singleton2 {
private:
    static std::once_flag init_flag;
    static Singleton2* instance;

    Singleton2() = default;

    static void init() {
        instance = new Singleton2();
    }

public:
    static Singleton2& get_instance() {
        std::call_once(init_flag, init);
        return *instance;
    }
};
```

**方式 3：双检锁（过时，不推荐）**：

```cpp
// ❌ 双检锁在 C++ 中有问题（内存序）
class BadSingleton {
private:
    static BadSingleton* instance;
    static std::mutex mtx;

public:
    static BadSingleton* get_instance() {
        if (instance == nullptr) {  // 第一次检查（无锁）
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr) {  // 第二次检查（有锁）
                instance = new BadSingleton();  // ❌ 可能有内存序问题
            }
        }
        return instance;
    }
};
```

---

## 6. 常见陷阱与最佳实践

### 6.1 常见陷阱

**陷阱 1：忘记 join/detach**：

```cpp
// ❌ 错误：线程对象析构时未 join/detach
void bad_thread() {
    std::thread t([]() { /* ... */ });
    // 函数结束，t 析构 → std::terminate
}
```

**陷阱 2：死锁**：

```cpp
// ❌ 错误：嵌套加锁导致死锁
std::mutex mtx;

void func_a() {
    std::lock_guard<std::mutex> lock(mtx);
    func_b();  // func_b 也需要 mtx → 死锁
}

void func_b() {
    std::lock_guard<std::mutex> lock(mtx);
    // ...
}
```

**陷阱 3：条件变量虚假唤醒**：

```cpp
// ❌ 错误：未检查条件
cv.wait(lock);  // 可能虚假唤醒

// ✅ 正确：使用 predicate
cv.wait(lock, []{ return ready; });
```

**陷阱 4：按值捕获引用**：

```cpp
// ❌ 错误：lambda 按值捕获局部变量的引用
void bad_capture() {
    int x = 42;
    int& ref = x;

    std::thread t([ref]() {  // 捕获引用
        std::cout << ref << "\n";  // ❌ ref 可能已失效
    });

    t.detach();
}
```

---

### 6.2 最佳实践

**1. 优先使用 RAII 锁管理**：

```cpp
// ✅ 使用 lock_guard / unique_lock
std::lock_guard<std::mutex> lock(mtx);
```

**2. 锁的粒度要小**：

```cpp
// ❌ 错误：持锁时间过长
void bad_lock_scope() {
    std::lock_guard<std::mutex> lock(mtx);
    compute_something();  // 耗时操作
    slow_io_operation();  // IO 操作
}

// ✅ 正确：缩小锁的作用域
void good_lock_scope() {
    auto result = compute_something();  // 在锁外计算

    {
        std::lock_guard<std::mutex> lock(mtx);
        update_shared_data(result);  // 只在必要时持锁
    }

    slow_io_operation();  // IO 在锁外
}
```

**3. 避免在持锁时调用外部代码**：

```cpp
// ❌ 错误：持锁时调用回调（可能死锁）
void bad_callback() {
    std::lock_guard<std::mutex> lock(mtx);
    user_callback();  // 用户回调可能再次请求 mtx
}

// ✅ 正确：先拷贝回调，再在锁外调用
void good_callback() {
    auto callback_copy = [&]() {
        std::lock_guard<std::mutex> lock(mtx);
        return user_callback;
    }();

    callback_copy();  // 锁外调用
}
```

**4. 使用线程池避免频繁创建线程**：

```cpp
// ❌ 错误：为每个任务创建线程
for (int i = 0; i < 1000; ++i) {
    std::thread t(task, i);
    t.detach();  // 创建 1000 个线程，开销大
}

// ✅ 正确：使用线程池（C++标准库无，需第三方库）
```

---

## 7. 性能考虑

### 7.1 锁的性能影响

**锁的开销**：
- **无竞争**：~25 ns
- **有竞争**：~100 ns - 几微秒
- **上下文切换**：几微秒到几毫秒

**减少锁竞争**：

```cpp
// ❌ 高竞争：所有线程竞争同一个锁
std::mutex global_mtx;
int global_counter = 0;

void high_contention() {
    for (int i = 0; i < 1000000; ++i) {
        std::lock_guard<std::mutex> lock(global_mtx);
        ++global_counter;
    }
}

// ✅ 低竞争：每个线程用局部变量，最后合并
void low_contention(int& local_counter) {
    for (int i = 0; i < 1000000; ++i) {
        ++local_counter;  // 无锁
    }
}

void final_merge(int local_counter) {
    std::lock_guard<std::mutex> lock(global_mtx);
    global_counter += local_counter;  // 只锁一次
}
```

---

## 总结

### 关键概念速查

| 概念 | 说明 |
|------|------|
| **`std::thread`** | 线程管理，join/detach |
| **`std::mutex`** | 互斥锁，保护共享资源 |
| **`std::lock_guard`** | RAII 风格，简单锁管理 |
| **`std::unique_lock`** | 灵活锁管理，支持条件变量 |
| **`std::condition_variable`** | 线程间通信和同步 |
| **`std::call_once`** | 确保函数只执行一次 |
| **`std::future`/`promise`** | 异步结果传递 |
| **`std::async`** | 简化异步任务启动 |

### 关键记忆点

1. **线程必须 join 或 detach**，否则程序终止
2. **使用 RAII 锁管理**，避免忘记 unlock
3. **条件变量必须使用 predicate**，防止虚假唤醒
4. **固定锁顺序或使用 `std::lock`** 避免死锁
5. **减小锁粒度**，提高并发性能
6. **C++11 局部静态变量线程安全**，单例模式首选
7. **避免在持锁时调用外部代码**
8. **`unique_lock` 比 `lock_guard` 灵活**，但开销稍大

### 进阶方向

- **原子操作**：`std::atomic`（下一章）
- **内存模型**：`memory_order`
- **无锁编程**：Lock-free data structures
- **线程池**：任务队列和工作线程管理
- **C++20 协程**：`co_await`、`co_yield`
