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

### 1.2 将函数放到线程执行的各种方式

**面试问题**：C++ 中有哪些方式可以将函数放到线程中执行？

**答案**：

```cpp
#include <iostream>
#include <thread>
#include <functional>
#include <future>

// ============================================
// 方式 1：普通函数
// ============================================
void normalFunction(int x) {
    std::cout << "普通函数: " << x << "\n";
}

void useNormalFunction() {
    std::thread t(normalFunction, 42);
    t.join();
}

// ============================================
// 方式 2：Lambda 表达式（最常用）
// ============================================
void useLambda() {
    int value = 100;

    // 2.1 无捕获 Lambda
    std::thread t1([]() {
        std::cout << "无捕获 Lambda\n";
    });

    // 2.2 值捕获
    std::thread t2([value]() {
        std::cout << "值捕获: " << value << "\n";
    });

    // 2.3 引用捕获（注意生命周期！）
    std::thread t3([&value]() {
        value += 10;  // 修改外部变量
    });

    // 2.4 泛型 Lambda (C++14)
    std::thread t4([](auto x) {
        std::cout << "泛型 Lambda: " << x << "\n";
    }, 3.14);

    // 2.5 带参数的 Lambda
    std::thread t5([](int a, int b) {
        std::cout << "带参数: " << a + b << "\n";
    }, 10, 20);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
}

// ============================================
// 方式 3：成员函数
// ============================================
class Worker {
public:
    int id;

    Worker(int id) : id(id) {}

    void doWork(int count) {
        std::cout << "Worker " << id << " 执行 " << count << " 次\n";
    }

    void operator()(int x) {
        std::cout << "Worker " << id << " 仿函数调用: " << x << "\n";
    }

    static void staticWork(int x) {
        std::cout << "静态成员函数: " << x << "\n";
    }
};

void useMemberFunction() {
    Worker worker(1);

    // 3.1 普通成员函数：传递 &类::方法, 对象指针/引用, 参数...
    std::thread t1(&Worker::doWork, &worker, 5);

    // 3.2 使用 std::ref 传递引用
    std::thread t2(&Worker::doWork, std::ref(worker), 3);

    // 3.3 传递对象副本（对象会被复制）
    std::thread t3(&Worker::doWork, worker, 2);

    // 3.4 使用智能指针
    auto workerPtr = std::make_shared<Worker>(2);
    std::thread t4(&Worker::doWork, workerPtr, 4);

    // 3.5 静态成员函数（与普通函数相同）
    std::thread t5(&Worker::staticWork, 100);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
}

// ============================================
// 方式 4：仿函数（函数对象）
// ============================================
struct Functor {
    int multiplier;

    Functor(int m) : multiplier(m) {}

    void operator()(int x) const {
        std::cout << "仿函数: " << x * multiplier << "\n";
    }
};

void useFunctor() {
    Functor f(10);

    // 4.1 传递仿函数对象（会被复制）
    std::thread t1(f, 5);

    // 4.2 传递引用
    std::thread t2(std::ref(f), 3);

    // 4.3 移动语义
    std::thread t3(std::move(f), 2);

    // 4.4 直接构造（注意：需要额外括号避免解析歧义）
    std::thread t4((Functor(5)), 4);  // 注意括号！

    t1.join(); t2.join(); t3.join(); t4.join();
}

// ============================================
// 方式 5：std::function
// ============================================
void useStdFunction() {
    // 5.1 包装普通函数
    std::function<void(int)> fn1 = normalFunction;
    std::thread t1(fn1, 1);

    // 5.2 包装 Lambda
    std::function<void()> fn2 = []() {
        std::cout << "std::function 包装 Lambda\n";
    };
    std::thread t2(fn2);

    // 5.3 包装成员函数
    Worker worker(3);
    std::function<void(int)> fn3 = std::bind(&Worker::doWork, &worker, std::placeholders::_1);
    std::thread t3(fn3, 7);

    t1.join(); t2.join(); t3.join();
}

// ============================================
// 方式 6：std::bind
// ============================================
void useStdBind() {
    Worker worker(4);

    // 6.1 绑定普通函数
    auto bound1 = std::bind(normalFunction, 99);
    std::thread t1(bound1);

    // 6.2 绑定成员函数
    auto bound2 = std::bind(&Worker::doWork, &worker, 10);
    std::thread t2(bound2);

    // 6.3 部分绑定（占位符）
    auto bound3 = std::bind(&Worker::doWork, &worker, std::placeholders::_1);
    std::thread t3(bound3, 20);

    t1.join(); t2.join(); t3.join();
}

// ============================================
// 方式 7：std::async（返回 future）
// ============================================
int compute(int x) {
    return x * x;
}

void useAsync() {
    // 7.1 异步执行（可能新线程）
    std::future<int> f1 = std::async(std::launch::async, compute, 10);
    std::cout << "async 结果: " << f1.get() << "\n";

    // 7.2 延迟执行（调用 get 时执行）
    std::future<int> f2 = std::async(std::launch::deferred, compute, 5);
    std::cout << "deferred 结果: " << f2.get() << "\n";

    // 7.3 Lambda
    auto f3 = std::async(std::launch::async, [](int a, int b) {
        return a + b;
    }, 3, 4);
    std::cout << "Lambda async: " << f3.get() << "\n";
}

// ============================================
// 方式 8：std::packaged_task
// ============================================
void usePackagedTask() {
    // 8.1 包装可调用对象
    std::packaged_task<int(int)> task(compute);
    std::future<int> result = task.get_future();

    // 在线程中执行
    std::thread t(std::move(task), 8);
    t.join();

    std::cout << "packaged_task 结果: " << result.get() << "\n";

    // 8.2 包装 Lambda
    std::packaged_task<std::string()> task2([]() {
        return std::string("Hello from packaged_task");
    });
    auto f2 = task2.get_future();

    std::thread t2(std::move(task2));
    t2.join();
    std::cout << f2.get() << "\n";
}

// ============================================
// 方式 9：函数指针
// ============================================
void useFunctionPointer() {
    // 9.1 普通函数指针
    void (*funcPtr)(int) = normalFunction;
    std::thread t1(funcPtr, 55);

    // 9.2 使用 typedef / using
    using FuncType = void(*)(int);
    FuncType fp = normalFunction;
    std::thread t2(fp, 66);

    t1.join(); t2.join();
}

// ============================================
// 方式 10：std::invoke (C++17)
// ============================================
#if __cplusplus >= 201703L
void useInvoke() {
    Worker worker(5);

    // std::invoke 可统一调用所有可调用对象
    std::thread t([&worker]() {
        std::invoke(&Worker::doWork, worker, 15);
        std::invoke(normalFunction, 77);
        std::invoke([](int x) { std::cout << "invoke Lambda: " << x << "\n"; }, 88);
    });
    t.join();
}
#endif
```

**各种方式对比**：

| 方式 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| **普通函数** | 简单直接 | 不灵活 | 简单任务 |
| **Lambda** | 灵活、可捕获上下文 | 注意生命周期 | 最常用，推荐 |
| **成员函数** | 面向对象 | 语法稍复杂 | 类方法并行执行 |
| **仿函数** | 可携带状态 | 需要额外定义类 | 复杂状态管理 |
| **std::function** | 类型擦除、通用 | 有运行时开销 | 回调、存储可调用对象 |
| **std::bind** | 预绑定参数 | 语法繁琐 | C++11 兼容，现代代码用 Lambda |
| **std::async** | 返回 future、简洁 | 可能不创建新线程 | 需要返回值的异步任务 |
| **std::packaged_task** | 分离任务和执行 | 较繁琐 | 任务队列、线程池 |
| **函数指针** | C 兼容 | 不灵活 | C 风格接口 |

**参数传递注意事项**：

```cpp
void parameterPassing() {
    int value = 42;
    std::string str = "hello";

    // ❌ 危险：引用参数默认按值传递！
    auto refFunc = [](int& x) { x += 10; };
    // std::thread t1(refFunc, value);  // 编译错误或未定义行为

    // ✅ 正确：使用 std::ref
    std::thread t2(refFunc, std::ref(value));
    t2.join();
    std::cout << "修改后: " << value << "\n";  // 52

    // ✅ 移动语义
    auto moveFunc = [](std::string s) {
        std::cout << "移动的字符串: " << s << "\n";
    };
    std::thread t3(moveFunc, std::move(str));
    t3.join();
    // str 现在为空

    // ✅ 指针传递
    int* ptr = &value;
    std::thread t4([](int* p) { *p = 100; }, ptr);
    t4.join();
}
```

**面试重点**：

| 问题 | 答案 |
|------|------|
| 最推荐哪种方式？ | Lambda，灵活且直观 |
| 成员函数怎么传？ | `&类::方法, 对象指针, 参数...` |
| 引用参数怎么传？ | 使用 `std::ref()` 或 `std::cref()` |
| std::async 一定创建新线程吗？ | 不一定，`std::launch::deferred` 延迟执行 |
| 仿函数传递会被复制吗？ | 是，用 `std::ref` 避免复制 |
| std::bind vs Lambda？ | 现代 C++ 推荐 Lambda |

**Runnable 与 Thread 的区别**：

这是 Java 中的经典问题，C++ 中也有类似概念：

| 概念 | Java | C++ 对应 |
|------|------|----------|
| **Thread** | `Thread` 类 | `std::thread` |
| **Runnable** | `Runnable` 接口 | 可调用对象（函数、Lambda、仿函数） |

**核心区别**：

| 方面 | Thread（线程） | Runnable（任务） |
|------|----------------|------------------|
| **本质** | 执行单元（系统资源） | 任务描述（代码逻辑） |
| **职责** | 负责执行 | 负责定义"做什么" |
| **资源** | 有栈、寄存器、系统开销 | 无系统资源开销 |
| **复用** | 不可复用（执行完销毁） | 可复用（同一任务多次执行） |
| **数量** | 受系统限制（百~万级） | 无限制 |
| **关系** | 1 个线程执行 1 个任务 | 1 个任务可被多个线程执行 |

```cpp
#include <thread>
#include <functional>
#include <queue>
#include <mutex>

// ============================================
// C++ 中的 "Runnable" 模式
// ============================================

// 方式 1：使用 std::function 作为 Runnable
using Runnable = std::function<void()>;

void executeTask(Runnable task) {
    std::thread t(task);  // 线程执行任务
    t.join();
}

// 方式 2：定义 Runnable 接口（类似 Java）
class IRunnable {
public:
    virtual ~IRunnable() = default;
    virtual void run() = 0;
};

class MyTask : public IRunnable {
    int id;
public:
    MyTask(int id) : id(id) {}

    void run() override {
        std::cout << "Task " << id << " 执行中\n";
    }
};

void runWithInterface() {
    MyTask task(1);

    // 线程执行任务
    std::thread t(&IRunnable::run, &task);
    t.join();

    // 同一个任务可以再次执行
    std::thread t2(&IRunnable::run, &task);
    t2.join();
}

// 方式 3：任务与线程分离（线程池模式）
class ThreadPool {
    std::queue<Runnable> tasks;
    std::mutex mtx;
    // ... 工作线程

public:
    // 提交任务（Runnable），不关心哪个线程执行
    void submit(Runnable task) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(std::move(task));
    }
};

// 使用示例
void runnableDemo() {
    // 定义任务（Runnable）
    Runnable task1 = []() { std::cout << "任务 1\n"; };
    Runnable task2 = []() { std::cout << "任务 2\n"; };

    // 同一个任务，不同线程执行
    std::thread t1(task1);
    std::thread t2(task1);  // 复用同一个任务

    // 同一个线程，执行不同任务
    // （需要线程池实现）

    t1.join();
    t2.join();
}
```

**为什么要分离 Runnable 和 Thread？**

```cpp
// ❌ 紧耦合：任务和线程绑定
void badDesign() {
    for (int i = 0; i < 1000; ++i) {
        std::thread t([i]() {
            processRequest(i);
        });
        t.detach();  // 创建 1000 个线程！
    }
}

// ✅ 解耦：任务和线程分离
void goodDesign() {
    ThreadPool pool(4);  // 只有 4 个线程

    for (int i = 0; i < 1000; ++i) {
        pool.submit([i]() {  // 提交 1000 个任务
            processRequest(i);
        });
    }
    // 4 个线程复用，执行 1000 个任务
}
```

**面试回答要点**：

| 问题 | 答案 |
|------|------|
| Runnable 是什么？ | 任务的抽象，描述"做什么"，不涉及执行 |
| Thread 是什么？ | 执行单元，负责"怎么执行"，是系统资源 |
| 为什么要分离？ | 解耦、复用线程、减少创建开销 |
| C++ 中如何实现？ | `std::function<void()>` 或仿函数作为任务 |
| 典型应用？ | 线程池、任务队列 |

---

### 1.3 `join()` vs `detach()` 的区别？

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

### 1.4 线程的资源模型：独占与共享

**问题描述**：线程有哪些资源是独占的，哪些是与进程共享的？

**资源分类**：

| 资源类型 | 线程独占 | 进程共享 | 说明 |
|----------|----------|----------|------|
| **栈空间** | ✅ | ❌ | 每个线程有独立的栈 |
| **寄存器** | ✅ | ❌ | 包括 PC、SP 等 |
| **线程 ID** | ✅ | ❌ | 每个线程唯一 |
| **错误码 (errno)** | ✅ | ❌ | 线程本地存储 |
| **信号掩码** | ✅ | ❌ | 每个线程可独立设置 |
| **堆内存** | ❌ | ✅ | 所有线程共享 |
| **全局/静态变量** | ❌ | ✅ | 所有线程可访问 |
| **代码段** | ❌ | ✅ | 所有线程执行同一份代码 |
| **文件描述符** | ❌ | ✅ | 打开的文件共享 |
| **信号处理器** | ❌ | ✅ | 进程级别设置 |
| **虚拟地址空间** | ❌ | ✅ | 共享同一地址空间 |

**线程独占资源详解**：

```cpp
#include <thread>
#include <iostream>

// 1. 栈空间 - 每个线程独立
void stack_demo() {
    int local_var = 42;  // 在当前线程栈上
    // 其他线程不能直接访问 local_var（除非显式传递地址）

    std::cout << "Thread " << std::this_thread::get_id()
              << " local_var at: " << &local_var << "\n";
}

void thread_stack_example() {
    std::thread t1(stack_demo);
    std::thread t2(stack_demo);
    t1.join();
    t2.join();
    // 两个线程的 local_var 地址不同
}

// 2. 线程本地存储 (Thread-Local Storage, TLS)
thread_local int tls_counter = 0;  // 每个线程有独立副本

void tls_demo() {
    ++tls_counter;  // 只影响当前线程的副本
    std::cout << "Thread " << std::this_thread::get_id()
              << " tls_counter = " << tls_counter << "\n";
}

void thread_local_example() {
    std::thread t1([]() { tls_demo(); tls_demo(); });  // 输出 1, 2
    std::thread t2([]() { tls_demo(); tls_demo(); });  // 输出 1, 2
    t1.join();
    t2.join();
    // 主线程的 tls_counter 仍为 0
}
```

**进程共享资源详解**：

```cpp
#include <mutex>

// 1. 堆内存 - 所有线程共享
int* shared_ptr = nullptr;
std::mutex mtx;

void heap_demo() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!shared_ptr) {
        shared_ptr = new int(100);  // 任何线程都可分配
    }
    *shared_ptr += 1;  // 需要同步保护
}

// 2. 全局/静态变量 - 共享
int global_counter = 0;

void global_demo() {
    // ❌ 危险：未保护的共享变量
    ++global_counter;  // 数据竞争！
}

// 3. 文件描述符 - 共享
#include <fstream>
std::ofstream shared_file;

void file_demo() {
    std::lock_guard<std::mutex> lock(mtx);
    shared_file << "Thread " << std::this_thread::get_id() << "\n";
}
```

**线程共享的方式**：

```cpp
// 方式 1：通过参数传递共享数据
void share_by_parameter(int& shared_data) {
    ++shared_data;  // 需要外部同步
}

// 方式 2：通过全局/静态变量
static int static_data = 0;

void share_by_global() {
    ++static_data;  // 需要同步
}

// 方式 3：通过堆内存
void share_by_heap(std::shared_ptr<int> ptr) {
    ++(*ptr);  // 需要同步
}

// 方式 4：通过线程本地存储（不共享，但可传递）
thread_local int local_data = 0;
// 每个线程有独立副本，不需要同步
```

**栈大小限制**：

```cpp
#include <thread>
#include <pthread.h>

void check_stack_size() {
    // 获取默认栈大小（POSIX 系统）
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    size_t stack_size;
    pthread_attr_getstacksize(&attr, &stack_size);
    std::cout << "Default stack size: " << stack_size / 1024 << " KB\n";
    // Linux 默认通常是 8 MB

    pthread_attr_destroy(&attr);
}

// 设置栈大小（POSIX）
void custom_stack_size() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024 * 1024);  // 1 MB

    pthread_t thread;
    pthread_create(&thread, &attr, [](void*) -> void* {
        // 使用 1MB 栈的线程
        return nullptr;
    }, nullptr);

    pthread_join(thread, nullptr);
    pthread_attr_destroy(&attr);
}
```

**线程与进程内存资源大小**：

| 资源 | 进程 | 线程 |
|------|------|------|
| **虚拟地址空间** | 32位: 4GB (用户空间 2-3GB)；64位: 128-256 TB | 共享所属进程地址空间 |
| **栈** | 主线程栈（与进程相同） | 独立栈，Linux 默认 8MB，Windows 默认 1MB |
| **堆** | 理论可用全部用户空间 | 共享进程堆，无独立限制 |
| **TLS** | N/A | 每线程独立，通常几 KB |

```
进程地址空间中的线程布局（64位 Linux）：

    高地址
    ┌─────────────────────────────────────────┐
    │              内核空间                    │
    ├─────────────────────────────────────────┤
    │    主线程栈 ↓     (8 MB)                │
    │    ─────────────                        │
    │    Guard Page (防栈溢出)                 │
    ├─────────────────────────────────────────┤
    │    线程 1 栈 ↓    (8 MB)                │
    │    Guard Page                           │
    ├─────────────────────────────────────────┤
    │    线程 2 栈 ↓    (8 MB)                │
    │    Guard Page                           │
    ├─────────────────────────────────────────┤
    │           ...更多线程栈...               │
    ├─────────────────────────────────────────┤
    │    mmap 区域（动态库、内存映射）          │
    ├─────────────────────────────────────────┤
    │         ↑ 堆 (共享)                      │
    ├─────────────────────────────────────────┤
    │    BSS、Data、Text (共享)                │
    └─────────────────────────────────────────┘
    低地址
```

**线程数量上限**：

```cpp
// 理论线程数 = 可用地址空间 / 每线程栈大小

// 32位系统:
// 用户空间约 3 GB，每线程栈 8 MB
// 理论上限 ≈ 3072 MB / 8 MB ≈ 384 个
// 实际约 300 个左右（还有堆、代码段等）

// 64位系统:
// 用户空间 128 TB+，理论上百万级
// 实际受限于：
// - 物理内存 + swap
// - 系统参数 (/proc/sys/kernel/threads-max)
// - 每用户进程限制 (ulimit -u)

// 查看系统限制
void checkThreadLimits() {
    // Linux: cat /proc/sys/kernel/threads-max
    // Linux: ulimit -u
    std::cout << "hardware_concurrency: "
              << std::thread::hardware_concurrency() << "\n";
}
```

**面试重点**：

| 问题 | 答案 |
|------|------|
| 线程栈大小是多少？ | Linux 默认 8MB，Windows 默认 1MB |
| 32位系统最多多少线程？ | 约 300-400 个（受地址空间限制） |
| 64位系统最多多少线程？ | 理论百万级，实际受物理内存和系统参数限制 |
| 线程能访问其他线程的栈吗？ | 技术上可以（共享地址空间），但不推荐 |
| `thread_local` 在哪里存储？ | TLS 段，每个线程有独立副本 |
| 堆内存需要同步吗？ | 是，多线程访问共享堆数据需要同步 |
| errno 是线程安全的吗？ | 是，现代系统 errno 是 thread-local |
| 为什么协程能创建更多？ | 栈小（KB 级），用户态管理无系统开销 |

---

### 1.5 线程标识和线程数量？

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
| **`thread_local`** | 线程本地存储，每个线程独立副本 |
| **线程池** | 复用线程，减少创建/销毁开销 |
| **CPU Affinity** | 线程绑定到特定 CPU 核心 |
| **`std::jthread`** (C++20) | 自动 join，支持取消 |

### 关键记忆点

1. **线程必须 join 或 detach**，否则程序终止
2. **使用 RAII 锁管理**，避免忘记 unlock
3. **条件变量必须使用 predicate**，防止虚假唤醒
4. **固定锁顺序或使用 `std::lock`** 避免死锁
5. **减小锁粒度**，提高并发性能
6. **C++11 局部静态变量线程安全**，单例模式首选
7. **避免在持锁时调用外部代码**
8. **`unique_lock` 比 `lock_guard` 灵活**，但开销稍大
9. **线程独占**：栈、寄存器、TLS、线程 ID、信号掩码
10. **线程共享**：堆、全局变量、代码段、文件描述符
11. **线程创建开销大**，使用线程池复用
12. **协作式取消**优于强制终止线程

---

## 8. 线程深入考察

### 8.1 线程的创建与销毁开销

**问题描述**：创建线程的代价有多大？

**线程创建开销**：

| 操作 | 大致耗时 | 说明 |
|------|----------|------|
| 创建线程 | 10-100 μs | 分配栈、初始化 TLS、内核调度 |
| 线程上下文切换 | 1-10 μs | 保存/恢复寄存器、刷新 TLB |
| 线程销毁 | 5-50 μs | 回收资源、通知内核 |
| 内存分配（栈） | 8 MB（默认） | 虚拟内存，按需分配物理页 |

**线程创建过程**：

```cpp
// 创建线程时系统做了什么？
// 1. 分配线程栈空间（虚拟内存）
// 2. 创建线程控制块 (TCB)
// 3. 初始化线程本地存储 (TLS)
// 4. 设置线程属性（优先级、亲和性等）
// 5. 将线程加入调度队列
// 6. 内核分配线程 ID

void thread_creation_cost() {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < 1000; ++i) {
        threads.emplace_back([]() {});  // 空线程
    }
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "创建 1000 个线程耗时: " << duration.count() << " μs\n";
    // 典型值：10-100 ms
}
```

**为什么需要线程池？**

```cpp
// ❌ 错误：为每个任务创建线程
void bad_design() {
    for (int i = 0; i < 10000; ++i) {
        std::thread t(process_request, i);
        t.detach();  // 创建 10000 个线程，开销巨大
    }
}

// ✅ 正确：使用线程池复用线程
class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop = false;

public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cv.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop = true;
        }
        cv.notify_all();
        for (auto& w : workers) w.join();
    }
};
```

---

### 8.2 线程调度与优先级

**问题描述**：操作系统如何调度线程？

**调度策略**：

| 策略 | 特点 | 适用场景 |
|------|------|----------|
| **SCHED_OTHER** | 时间片轮转，默认策略 | 普通应用 |
| **SCHED_FIFO** | 实时，先进先出 | 实时系统 |
| **SCHED_RR** | 实时，轮转 | 实时系统 |

**设置线程优先级（POSIX）**：

```cpp
#include <pthread.h>
#include <sched.h>

void set_thread_priority() {
    pthread_t thread = pthread_self();

    // 获取当前调度策略和优先级
    int policy;
    sched_param param;
    pthread_getschedparam(thread, &policy, &param);

    std::cout << "当前策略: " << policy << "\n";
    std::cout << "当前优先级: " << param.sched_priority << "\n";

    // 设置优先级（需要 root 权限）
    param.sched_priority = 10;
    int result = pthread_setschedparam(thread, SCHED_FIFO, &param);
    if (result != 0) {
        std::cerr << "设置优先级失败（可能需要 root）\n";
    }
}

// Windows 设置线程优先级
#ifdef _WIN32
#include <windows.h>

void set_thread_priority_windows() {
    HANDLE thread = GetCurrentThread();

    // 设置优先级
    SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST);

    // 可选值：
    // THREAD_PRIORITY_LOWEST (-2)
    // THREAD_PRIORITY_BELOW_NORMAL (-1)
    // THREAD_PRIORITY_NORMAL (0)
    // THREAD_PRIORITY_ABOVE_NORMAL (1)
    // THREAD_PRIORITY_HIGHEST (2)
    // THREAD_PRIORITY_TIME_CRITICAL (15)
}
#endif
```

---

### 8.3 线程亲和性 (CPU Affinity)

**问题描述**：如何将线程绑定到特定 CPU 核心？

**为什么需要亲和性？**
- **提升缓存命中率**：线程始终在同一核心运行
- **减少上下文切换开销**：避免核心迁移
- **NUMA 优化**：访问本地内存更快

```cpp
#include <thread>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>

void set_cpu_affinity(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    pthread_t thread = pthread_self();
    int result = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

    if (result == 0) {
        std::cout << "线程绑定到 CPU " << cpu_id << "\n";
    } else {
        std::cerr << "绑定失败\n";
    }
}

void affinity_demo() {
    unsigned int num_cpus = std::thread::hardware_concurrency();
    std::cout << "CPU 核心数: " << num_cpus << "\n";

    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_cpus; ++i) {
        threads.emplace_back([i]() {
            set_cpu_affinity(i);
            // 执行计算密集型任务
        });
    }

    for (auto& t : threads) t.join();
}
#endif

#ifdef _WIN32
#include <windows.h>

void set_cpu_affinity_windows(int cpu_id) {
    HANDLE thread = GetCurrentThread();
    DWORD_PTR mask = 1ULL << cpu_id;
    SetThreadAffinityMask(thread, mask);
}
#endif
```

---

### 8.4 线程安全的初始化

**问题描述**：如何保证全局/静态变量的线程安全初始化？

**C++11 静态局部变量**：

```cpp
// C++11 保证静态局部变量初始化是线程安全的
class Singleton {
public:
    static Singleton& instance() {
        static Singleton inst;  // 线程安全
        return inst;
    }

private:
    Singleton() {
        std::cout << "初始化（只调用一次）\n";
    }
};

// 底层实现（编译器生成）：
// static bool initialized = false;
// static std::mutex init_mutex;
// if (!initialized) {
//     std::lock_guard<std::mutex> lock(init_mutex);
//     if (!initialized) {
//         inst = Singleton();
//         initialized = true;
//     }
// }
```

**全局变量初始化顺序问题**：

```cpp
// ❌ 危险：跨编译单元的全局变量初始化顺序未定义
// file1.cpp
extern int global_b;
int global_a = global_b + 1;  // global_b 可能还未初始化

// file2.cpp
int global_b = 10;

// ✅ 解决方案：使用函数包装
int& get_global_a() {
    static int a = get_global_b() + 1;  // 确保 b 先初始化
    return a;
}

int& get_global_b() {
    static int b = 10;
    return b;
}
```

---

### 8.5 线程取消与中断

**问题描述**：如何安全地终止线程？

**C++ 没有直接的线程取消机制**：

```cpp
// ❌ 错误：强制终止线程（不安全）
// pthread_cancel(thread);  // POSIX
// TerminateThread(handle);  // Windows
// 原因：资源泄漏、锁未释放、数据损坏

// ✅ 正确：使用标志位协作取消
class Worker {
    std::atomic<bool> should_stop{false};
    std::thread worker_thread;

public:
    void start() {
        worker_thread = std::thread([this]() {
            while (!should_stop.load()) {
                // 执行工作
                do_work();

                // 定期检查取消标志
                if (should_stop.load()) break;
            }
            std::cout << "线程正常退出\n";
        });
    }

    void stop() {
        should_stop.store(true);
        if (worker_thread.joinable()) {
            worker_thread.join();
        }
    }

private:
    void do_work() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};
```

**C++20 `std::stop_token`**：

```cpp
#include <stop_token>
#include <jthread>

void worker_with_stop_token(std::stop_token token) {
    while (!token.stop_requested()) {
        // 执行工作
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "收到停止请求\n";
}

void jthread_demo() {
    std::jthread worker(worker_with_stop_token);
    // std::jthread 析构时自动请求停止并 join

    std::this_thread::sleep_for(std::chrono::seconds(1));
    worker.request_stop();  // 请求停止
    // 或让 jthread 析构时自动停止
}
```

---

### 8.6 线程与信号处理

**问题描述**：多线程程序如何处理信号？

**信号处理的复杂性**：

| 问题 | 说明 |
|------|------|
| 信号可能发送到任意线程 | 除非显式指定 |
| 信号处理器是全局的 | 所有线程共享 |
| 信号处理器中只能调用异步信号安全函数 | 很多函数不能用 |

```cpp
#include <signal.h>
#include <thread>

// ✅ 推荐：专用线程处理信号
void signal_handler_thread() {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);

    // 阻塞信号
    pthread_sigmask(SIG_BLOCK, &sigset, nullptr);

    int sig;
    while (true) {
        int result = sigwait(&sigset, &sig);
        if (result == 0) {
            std::cout << "收到信号: " << sig << "\n";
            if (sig == SIGTERM) break;
        }
    }
}

void multi_thread_signal_handling() {
    // 在主线程阻塞信号
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &sigset, nullptr);

    // 创建专用信号处理线程
    std::thread sig_thread(signal_handler_thread);

    // 其他工作线程...
    std::thread worker([]() {
        while (true) {
            // 工作...
        }
    });

    sig_thread.join();
}
```

---

### 8.7 线程与异常

**问题描述**：线程中的异常如何处理？

```cpp
#include <exception>
#include <future>

// ❌ 问题：线程中未捕获的异常会调用 std::terminate
void bad_exception_handling() {
    std::thread t([]() {
        throw std::runtime_error("线程异常");  // ❌ 程序终止
    });
    t.join();
}

// ✅ 方案 1：线程内捕获异常
void catch_in_thread() {
    std::exception_ptr eptr;

    std::thread t([&eptr]() {
        try {
            throw std::runtime_error("线程异常");
        } catch (...) {
            eptr = std::current_exception();
        }
    });
    t.join();

    if (eptr) {
        try {
            std::rethrow_exception(eptr);
        } catch (const std::exception& e) {
            std::cout << "捕获到线程异常: " << e.what() << "\n";
        }
    }
}

// ✅ 方案 2：使用 std::async（自动传播异常）
void use_async() {
    auto future = std::async(std::launch::async, []() {
        throw std::runtime_error("异步异常");
        return 42;
    });

    try {
        int result = future.get();  // 异常在这里重新抛出
    } catch (const std::exception& e) {
        std::cout << "捕获到异常: " << e.what() << "\n";
    }
}
```

---

### 8.8 面试高频深入问题

| 问题 | 关键点 |
|------|--------|
| 线程和进程的区别？ | 资源共享、创建开销、通信方式 |
| 线程栈溢出如何检测？ | Guard page、栈探测 |
| 为什么 errno 是线程安全的？ | 现代实现使用 TLS |
| 线程局部存储原理？ | TLS 段、__thread 关键字 |
| std::thread 析构时为什么 terminate？ | 强制选择 join/detach，避免资源泄漏 |
| 如何限制线程数量？ | 线程池、信号量 |
| 线程饥饿如何解决？ | 公平锁、优先级调整 |
| 多线程程序如何 debug？ | GDB、TSan、日志 |

### 进阶方向

- **原子操作**：`std::atomic`（下一章）
- **内存模型**：`memory_order`
- **无锁编程**：Lock-free data structures
- **线程池**：任务队列和工作线程管理
- **C++20 协程**：`co_await`、`co_yield`
