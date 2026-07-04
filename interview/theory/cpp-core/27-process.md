 # 进程（Process）

## 概述

进程是操作系统资源分配的基本单位，理解进程、线程、协程的区别与联系是系统编程和面试的重要考点。

---

## 1. 进程基础概念

### 1.1 什么是进程？

**面试问题**：什么是进程？进程包含哪些资源？

**答案**：

进程是程序的一次执行实例，是操作系统进行资源分配和调度的基本单位。

```
┌─────────────────────────────────────────────────────────┐
│                      进程 (Process)                      │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────┐   │
│  │                 虚拟地址空间                      │   │
│  │  ┌──────────┬──────────┬──────────┬──────────┐  │   │
│  │  │   代码段  │   数据段  │    堆    │    栈    │  │   │
│  │  │  (Text)  │  (Data)  │  (Heap)  │  (Stack) │  │   │
│  │  └──────────┴──────────┴──────────┴──────────┘  │   │
│  └─────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────┤
│  进程控制块 (PCB - Process Control Block)               │
│  ├── PID (进程标识符)                                   │
│  ├── 进程状态 (运行/就绪/阻塞)                          │
│  ├── 程序计数器 (PC)                                    │
│  ├── CPU 寄存器                                         │
│  ├── 内存管理信息 (页表、段表)                          │
│  ├── 打开的文件描述符表                                 │
│  ├── 信号处理信息                                       │
│  └── 调度信息 (优先级、时间片)                          │
└─────────────────────────────────────────────────────────┘
```

**进程拥有的资源**：

| 资源类型 | 说明 |
|----------|------|
| 独立地址空间 | 32位：4GB，64位：理论 16 EB，实际 128 TB - 256 TB |
| 代码段 | 可执行指令 |
| 数据段 | 全局变量、静态变量 |
| 堆 | 动态分配的内存 |
| 栈 | 函数调用栈（每个线程有独立栈） |
| 文件描述符 | 打开的文件、socket、管道等 |
| 信号处理器 | 信号处理函数表 |
| 环境变量 | 继承自父进程或自定义 |

### 1.2 进程状态

**面试问题**：进程有哪些状态？状态之间如何转换？

**答案**：

```
                    ┌─────────────┐
                    │   创建态    │
                    │  (Created)  │
                    └──────┬──────┘
                           │ 分配资源完成
                           ▼
┌──────────────────────────────────────────────────────────┐
│                                                          │
│    ┌─────────────┐    调度    ┌─────────────┐           │
│    │   就绪态    │ ─────────► │   运行态    │           │
│    │   (Ready)   │            │  (Running)  │           │
│    └──────▲──────┘ ◄───────── └──────┬──────┘           │
│           │         时间片用完        │                  │
│           │                          │                  │
│           │         I/O完成          │ 等待I/O/资源     │
│           │         资源可用          ▼                  │
│           │          ┌─────────────┐                    │
│           └──────────│   阻塞态    │                    │
│                      │  (Blocked)  │                    │
│                      └─────────────┘                    │
│                                                          │
└──────────────────────────────────────────────────────────┘
                           │
                           │ 进程终止
                           ▼
                    ┌─────────────┐
                    │   终止态    │
                    │ (Terminated)│
                    └─────────────┘
```

```cpp
// Linux 进程状态
// 通过 /proc/[pid]/status 或 ps 命令查看

// R (Running)     - 运行或可运行（在运行队列中）
// S (Sleeping)    - 可中断睡眠（等待事件）
// D (Disk Sleep)  - 不可中断睡眠（通常等待 I/O）
// Z (Zombie)      - 僵尸进程（已终止但未被回收）
// T (Stopped)     - 停止（收到 SIGSTOP 等信号）
// X (Dead)        - 已死亡（不应该看到）
```

### 1.3 进程与线程的内存资源

**面试问题**：进程和线程各自可用的内存资源大小是多少？

**答案**：

**进程地址空间**：

| 架构 | 虚拟地址空间 | 用户空间 | 内核空间 | 说明 |
|------|-------------|----------|----------|------|
| **32位 Linux** | 4 GB | 3 GB | 1 GB | 3:1 划分（可配置为 2:2） |
| **32位 Windows** | 4 GB | 2 GB | 2 GB | 可开启 /3GB 模式 |
| **64位 Linux** | 256 TB | 128 TB | 128 TB | x86-64 目前使用 48 位地址 |
| **64位 Windows** | 256 TB | 128 TB | 128 TB | 与 Linux 类似 |

```
32位进程地址空间布局（Linux 3:1 划分）：

    0xFFFFFFFF ┌─────────────────────┐
               │    内核空间 (1GB)    │  ← 用户进程不可访问
    0xC0000000 ├─────────────────────┤
               │       栈 ↓          │  ← 向下增长
               │         ...         │
               │       ↑ mmap        │  ← 动态库、内存映射
               │         ...         │
               │       ↑ 堆          │  ← 向上增长
               ├─────────────────────┤
               │    BSS（未初始化）   │
               │    Data（已初始化）  │
               │    Text（代码段）    │
    0x08048000 ├─────────────────────┤  ← 程序加载地址
               │      保留区域        │
    0x00000000 └─────────────────────┘

64位进程地址空间布局：

    0xFFFFFFFFFFFFFFFF ┌──────────────────┐
                       │    内核空间       │  128 TB
    0xFFFF800000000000 ├──────────────────┤
                       │    非规范区域     │  ← 不可使用
    0x00007FFFFFFFFFFF ├──────────────────┤
                       │    用户空间       │  128 TB
    0x0000000000000000 └──────────────────┘
```

**线程栈大小**：

| 操作系统 | 默认栈大小 | 可配置范围 | 说明 |
|----------|-----------|-----------|------|
| **Linux** | 8 MB | 可调整 (ulimit -s) | 最小 16 KB (PTHREAD_STACK_MIN) |
| **Windows** | 1 MB | 编译时指定或运行时设置 | 可设置 reserve 和 commit 大小 |
| **macOS** | 512 KB (辅助线程) | 可调整 | 主线程 8 MB |

```cpp
#include <iostream>
#include <thread>

#ifdef __linux__
#include <pthread.h>
#include <sys/resource.h>

void checkStackSize() {
    // 方法 1：获取当前线程栈大小
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    size_t stackSize;
    pthread_attr_getstacksize(&attr, &stackSize);
    std::cout << "默认线程栈大小: " << stackSize / 1024 / 1024 << " MB\n";

    pthread_attr_destroy(&attr);

    // 方法 2：获取系统栈大小限制
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    std::cout << "栈大小软限制: " << rl.rlim_cur / 1024 / 1024 << " MB\n";
    std::cout << "栈大小硬限制: " << rl.rlim_max / 1024 / 1024 << " MB\n";
}

// 设置线程栈大小
void createThreadWithCustomStack() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    size_t customSize = 2 * 1024 * 1024;  // 2 MB
    pthread_attr_setstacksize(&attr, customSize);

    pthread_t thread;
    pthread_create(&thread, &attr, [](void*) -> void* {
        std::cout << "线程运行在自定义栈上\n";
        return nullptr;
    }, nullptr);

    pthread_join(thread, nullptr);
    pthread_attr_destroy(&attr);
}
#endif

#ifdef _WIN32
#include <windows.h>

void checkWindowsStackSize() {
    // Windows 默认栈大小由链接器设置
    // 可通过 /STACK:reserve[,commit] 设置
    // 或在 CreateThread 时指定

    // 运行时创建自定义栈大小的线程
    HANDLE thread = CreateThread(
        nullptr,
        512 * 1024,     // 512 KB 栈大小
        [](LPVOID) -> DWORD {
            std::cout << "Windows 线程运行中\n";
            return 0;
        },
        nullptr,
        0,              // STACK_SIZE_PARAM_IS_A_RESERVATION
        nullptr
    );

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}
#endif
```

**内存资源限制对比**：

| 资源 | 进程 | 线程 |
|------|------|------|
| **虚拟地址空间** | 独占全部（32位 4GB，64位 128TB+） | 共享进程地址空间 |
| **栈** | 主线程栈 8MB（Linux） | 每线程独立栈（默认 8MB Linux / 1MB Windows） |
| **堆** | 理论上可用全部用户空间 | 共享进程堆，无独立限制 |
| **物理内存** | 受系统物理内存和 swap 限制 | 共享进程的物理内存配额 |
| **线程数** | N/A | 受栈大小和地址空间限制 |

**线程数量上限计算**：

```cpp
// 理论线程数上限 = 可用地址空间 / 每线程栈大小

// 32位 Linux:
// 用户空间 3GB ≈ 3072 MB
// 每线程栈 8 MB
// 理论上限 = 3072 / 8 ≈ 384 个线程
// 实际更少（要留空间给堆、代码段等）

// 64位 Linux:
// 用户空间 128 TB
// 每线程栈 8 MB
// 理论上限 = 128 TB / 8 MB = 16,000,000+ 个线程
// 实际受限于：
// - 物理内存
// - /proc/sys/kernel/threads-max
// - /proc/sys/vm/max_map_count
// - ulimit -u (每用户最大进程数)

#include <thread>
#include <vector>
#include <atomic>

void testMaxThreads() {
    std::atomic<int> count{0};
    std::vector<std::thread> threads;

    try {
        while (true) {
            threads.emplace_back([&count]() {
                ++count;
                std::this_thread::sleep_for(std::chrono::hours(1));
            });
        }
    } catch (const std::system_error& e) {
        std::cout << "创建了 " << count << " 个线程后失败\n";
        std::cout << "错误: " << e.what() << "\n";
    }

    for (auto& t : threads) {
        if (t.joinable()) t.detach();
    }
}
```

**面试重点**：

| 问题 | 关键答案 |
|------|----------|
| 32位进程最大内存？ | 4GB 虚拟地址，用户空间 2-3GB |
| 64位进程最大内存？ | 理论 16 EB，实际 128-256 TB |
| 线程默认栈大小？ | Linux 8MB，Windows 1MB |
| 最多能创建多少线程？ | 受地址空间、物理内存、系统限制 |
| 为什么协程可以创建更多？ | 栈更小（KB 级），用户态管理 |

---

### 1.4 进程创建

**面试问题**：fork() 和 exec() 有什么区别？

**答案**：

```cpp
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

// fork() - 创建子进程，复制父进程
void forkDemo() {
    pid_t pid = fork();

    if (pid < 0) {
        // 错误
        std::cerr << "fork failed" << std::endl;
    } else if (pid == 0) {
        // 子进程
        std::cout << "Child process, PID: " << getpid()
                  << ", Parent PID: " << getppid() << std::endl;
    } else {
        // 父进程
        std::cout << "Parent process, PID: " << getpid()
                  << ", Child PID: " << pid << std::endl;

        // 等待子进程结束
        int status;
        waitpid(pid, &status, 0);
    }
}

// exec() 系列 - 替换当前进程映像
void execDemo() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程：执行新程序
        // exec 成功后，当前代码不再执行
        char* args[] = {"/bin/ls", "-la", nullptr};
        execv("/bin/ls", args);

        // 只有 exec 失败才会执行到这里
        std::cerr << "exec failed" << std::endl;
        _exit(1);
    } else {
        wait(nullptr);
    }
}
```

**fork() vs exec() 对比**：

| 特性 | fork() | exec() |
|------|--------|--------|
| 功能 | 创建新进程（复制父进程） | 替换当前进程映像 |
| PID | 子进程有新 PID | PID 不变 |
| 地址空间 | 复制父进程（COW） | 加载新程序 |
| 返回 | 父进程返回子 PID，子进程返回 0 | 成功不返回，失败返回 -1 |
| 代码继续执行 | 父子都继续执行 | 原代码不再执行 |

**Copy-on-Write (COW)**：

```cpp
// fork() 使用 COW 优化
// 1. fork 后父子进程共享相同的物理页
// 2. 任一进程写入时，才复制该页
// 3. 大大减少 fork 的开销

int globalVar = 100;

void cowDemo() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程修改变量
        globalVar = 200;  // 触发 COW，复制该页
        std::cout << "Child: " << globalVar << std::endl;  // 200
    } else {
        sleep(1);
        std::cout << "Parent: " << globalVar << std::endl;  // 100
        wait(nullptr);
    }
}
```

---

## 2. 进程 vs 线程 vs 协程

### 2.1 三者对比总览

**面试问题**：进程、线程、协程有什么区别？

**答案**：

```
┌────────────────────────────────────────────────────────────────┐
│                          操作系统                               │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                      进程 A                               │  │
│  │  ┌─────────────┬─────────────┬─────────────┐             │  │
│  │  │   线程 1    │   线程 2    │   线程 3    │             │  │
│  │  │ ┌───┬───┐  │ ┌───┬───┐  │ ┌───┬───┐  │             │  │
│  │  │ │协程│协程│  │ │协程│协程│  │ │   │   │  │             │  │
│  │  │ │ 1 │ 2 │  │ │ 3 │ 4 │  │ │   │   │  │             │  │
│  │  │ └───┴───┘  │ └───┴───┘  │ └───┴───┘  │             │  │
│  │  └─────────────┴─────────────┴─────────────┘             │  │
│  │                     共享地址空间                          │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                      进程 B                               │  │
│  │               （独立地址空间）                             │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────────────┘
```

| 特性 | 进程 (Process) | 线程 (Thread) | 协程 (Coroutine) |
|------|----------------|---------------|------------------|
| **调度者** | 操作系统内核 | 操作系统内核 | 用户态/程序员 |
| **调度方式** | 抢占式 | 抢占式 | 协作式（主动让出） |
| **切换开销** | 最大（切换地址空间、TLB 刷新） | 中等（保存寄存器、栈） | 最小（仅保存少量状态） |
| **并行性** | 真正并行（多 CPU） | 真正并行（多 CPU） | 单线程内并发，非并行 |
| **地址空间** | 独立 | 共享所属进程 | 共享所属线程 |
| **通信方式** | IPC（管道、共享内存等） | 共享内存、同步原语 | 直接访问（无需同步） |
| **创建开销** | 最大 | 中等 | 最小 |
| **栈大小** | MB 级 | MB 级（默认 1-8 MB） | KB 级（可自定义） |
| **数量上限** | 百~千级 | 千~万级 | 百万级 |
| **崩溃影响** | 仅影响本进程 | 整个进程崩溃 | 整个线程/进程崩溃 |
| **适用场景** | 隔离性要求高、多核利用 | 并行计算、I/O 密集 | 高并发 I/O、异步编程 |

### 2.2 调度与切换开销

**面试问题**：进程切换和线程切换的开销差异在哪里？

**答案**：

```cpp
// 进程上下文切换需要保存/恢复的内容
struct ProcessContext {
    // 1. CPU 寄存器（与线程切换相同）
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags;

    // 2. 进程特有（开销大的部分）
    uint64_t cr3;         // 页表基址寄存器 → 切换地址空间
    // TLB (Translation Lookaside Buffer) 需要刷新
    // 缓存可能失效（地址空间不同）

    // 3. 其他进程状态
    FileDescriptorTable* fdt;
    SignalHandlers* signals;
    // ...
};

// 线程上下文切换只需要保存/恢复
struct ThreadContext {
    // CPU 寄存器
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags;

    // 线程本地存储
    void* tls;

    // 无需切换地址空间！
};

// 协程切换只需要
struct CoroutineContext {
    void* stack_pointer;   // 栈指针
    void* instruction_ptr; // 指令指针
    // 可能还有少量寄存器
    // 完全在用户态完成，无系统调用
};
```

**切换耗时对比**（数量级）：

| 切换类型 | 大约耗时 | 说明 |
|----------|----------|------|
| 进程切换 | 1-10 μs | 涉及内核、TLB 刷新、缓存失效 |
| 线程切换 | 0.1-1 μs | 仅内核态切换，共享地址空间 |
| 协程切换 | 10-100 ns | 纯用户态，几条指令 |

### 2.3 内存与资源共享

**面试问题**：进程、线程、协程在内存共享上有什么区别？

**答案**：

```cpp
// 进程间 - 默认不共享，需要 IPC
// 进程 A                    进程 B
// ┌──────────────┐          ┌──────────────┐
// │ 地址空间 A    │          │ 地址空间 B    │
// │              │          │              │
// │ int x = 1;   │   ???    │ int x = 2;   │
// │              │ ◄──────► │              │
// └──────────────┘   IPC    └──────────────┘

// 线程间 - 共享地址空间，需要同步
// 线程 1              线程 2
// ┌────────┐         ┌────────┐
// │ 栈 1   │         │ 栈 2   │  ← 各自独立的栈
// └────────┘         └────────┘
//     │                  │
//     └────────┬─────────┘
//              ▼
// ┌─────────────────────────┐
// │    共享堆、全局变量       │  ← 需要互斥锁保护
// │    int shared_var;      │
// └─────────────────────────┘

// 协程间 - 同一线程内，天然共享
// 协程 A              协程 B
//     │                  │
//     └────────┬─────────┘
//              ▼
// ┌─────────────────────────┐
// │    同一线程的所有资源     │  ← 无需同步（单线程）
// │    直接访问             │
// └─────────────────────────┘
```

```cpp
#include <thread>
#include <mutex>
#include <coroutine>

// 线程间共享需要同步
int sharedCounter = 0;
std::mutex mtx;

void threadFunc() {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);  // 需要锁！
        ++sharedCounter;
    }
}

// 协程间共享无需同步（同一线程）
int coroutineCounter = 0;

struct SimpleCoroutine {
    struct promise_type {
        SimpleCoroutine get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

SimpleCoroutine coroutineFunc() {
    for (int i = 0; i < 1000; ++i) {
        ++coroutineCounter;  // 无需锁！同一线程
        co_await std::suspend_always{};  // 主动让出
    }
}
```

### 2.4 创建与销毁

**面试问题**：创建进程、线程、协程的开销分别是多少？

**答案**：

```cpp
#include <chrono>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

// 进程创建开销测试
void benchmarkProcessCreation() {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            _exit(0);  // 子进程立即退出
        }
        wait(nullptr);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "1000 processes: " << duration.count() << " μs" << std::endl;
    // 典型结果: 几十到几百毫秒
}

// 线程创建开销测试
void benchmarkThreadCreation() {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        std::thread t([](){});
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "1000 threads: " << duration.count() << " μs" << std::endl;
    // 典型结果: 几毫秒到几十毫秒
}

// 协程创建开销 - 通常只是一次函数调用 + 栈分配
// 比线程快 10-100 倍
```

**创建开销对比**：

| 类型 | 创建开销 | 内存占用 | 可创建数量 |
|------|----------|----------|-----------|
| 进程 | ~1 ms | 独立地址空间 | 数百~数千 |
| 线程 | ~50 μs | 默认 1-8 MB 栈 | 数千~数万 |
| 协程 | ~1 μs | 几 KB 栈 | 数十万~数百万 |

### 2.5 使用场景对比

**面试问题**：什么时候用进程、线程、协程？

**答案**：

```cpp
// 1. 进程 - 隔离性要求高
//    - 浏览器每个标签页一个进程（Chrome）
//    - 服务器 worker 进程
//    - 不信任的代码隔离执行

// 优点：崩溃不影响其他进程，安全隔离
// 缺点：开销大，通信复杂

// 2. 线程 - 并行计算、共享数据
//    - CPU 密集型并行任务
//    - GUI 程序（主线程 + 工作线程）
//    - 需要共享大量数据

// 优点：共享内存，真正并行
// 缺点：需要同步，一个线程崩溃影响整个进程

// 3. 协程 - 高并发 I/O
//    - 网络服务器处理大量连接
//    - 异步 I/O 操作
//    - 生成器、迭代器

// 优点：轻量、无锁、易编写
// 缺点：不能并行，需要协作让出

// 实际应用示例
class WebServer {
public:
    // 多进程模型 (Nginx prefork)
    // 每个进程处理独立请求，崩溃隔离
    void preforkModel();

    // 多线程模型 (传统 Java 服务器)
    // 每个请求一个线程
    void threadPerRequestModel();

    // 协程模型 (Go, Kotlin, C++20)
    // 单线程处理大量并发连接
    void coroutineModel();

    // 混合模型 (实际最常用)
    // 多进程 + 多线程 + 协程
    void hybridModel();
};
```

**决策流程图**：

```
需要什么？
    │
    ├─► 强隔离（安全、崩溃隔离）──────────► 进程
    │
    ├─► CPU 密集型并行计算 ───────────────► 线程
    │
    ├─► 大量并发 I/O（网络、文件）─────────► 协程
    │
    └─► 以上都需要 ──────────────────────► 混合模型
        (多进程 + 每进程多线程 + 协程)
```

---

## 3. 进程间通信（IPC）

### 3.1 IPC 机制概览

**面试问题**：有哪些进程间通信方式？各自的优缺点？

**答案**：

| IPC 方式 | 特点 | 适用场景 |
|----------|------|----------|
| **管道 (Pipe)** | 单向、父子进程 | 简单的单向数据流 |
| **命名管道 (FIFO)** | 单向、任意进程 | 无亲缘关系进程通信 |
| **消息队列** | 有类型、异步 | 消息传递、解耦 |
| **共享内存** | 最快、需同步 | 大量数据共享 |
| **信号量** | 同步原语 | 进程同步 |
| **信号 (Signal)** | 异步通知 | 事件通知、中断 |
| **Socket** | 网络/本地、双向 | 跨机器、灵活 |
| **内存映射文件** | 文件共享 | 大文件共享 |

### 3.2 管道 (Pipe)

```cpp
#include <unistd.h>
#include <cstring>
#include <iostream>

void pipeDemo() {
    int pipefd[2];  // pipefd[0] 读端, pipefd[1] 写端

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程：读取
        close(pipefd[1]);  // 关闭写端

        char buffer[256];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';
        std::cout << "Child received: " << buffer << std::endl;

        close(pipefd[0]);
    } else {
        // 父进程：写入
        close(pipefd[0]);  // 关闭读端

        const char* message = "Hello from parent!";
        write(pipefd[1], message, strlen(message));

        close(pipefd[1]);
        wait(nullptr);
    }
}

// 命名管道 (FIFO) - 无亲缘关系进程间通信
#include <sys/stat.h>
#include <fcntl.h>

void fifoDemo() {
    const char* fifoPath = "/tmp/my_fifo";

    // 创建命名管道
    mkfifo(fifoPath, 0666);

    // 进程 A（写入端）
    int fd = open(fifoPath, O_WRONLY);
    write(fd, "Hello FIFO!", 11);
    close(fd);

    // 进程 B（读取端）
    int fd2 = open(fifoPath, O_RDONLY);
    char buffer[256];
    read(fd2, buffer, sizeof(buffer));
    close(fd2);
}
```

### 3.3 共享内存

**面试问题**：共享内存是最快的 IPC 方式吗？为什么？

**答案**：

是的，共享内存是最快的 IPC 方式，因为：
1. 数据不需要在进程间复制
2. 直接访问同一块物理内存
3. 只有初始映射时需要系统调用

```cpp
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>

// POSIX 共享内存
void sharedMemoryDemo() {
    const char* shmName = "/my_shm";
    const size_t shmSize = 4096;

    // 创建/打开共享内存
    int fd = shm_open(shmName, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, shmSize);

    // 映射到进程地址空间
    void* ptr = mmap(nullptr, shmSize, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程读取
        sleep(1);  // 等待父进程写入
        std::cout << "Child read: " << static_cast<char*>(ptr) << std::endl;
    } else {
        // 父进程写入
        strcpy(static_cast<char*>(ptr), "Hello shared memory!");
        wait(nullptr);
    }

    // 清理
    munmap(ptr, shmSize);
    shm_unlink(shmName);
}

// 共享内存 + 信号量同步
struct SharedData {
    sem_t sem;
    int counter;
    char message[256];
};

void sharedMemoryWithSync() {
    const char* shmName = "/sync_shm";

    int fd = shm_open(shmName, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SharedData));

    SharedData* data = static_cast<SharedData*>(
        mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE,
             MAP_SHARED, fd, 0));

    // 初始化信号量（进程间共享）
    sem_init(&data->sem, 1, 1);  // 第二个参数 1 表示进程间共享

    pid_t pid = fork();

    if (pid == 0) {
        for (int i = 0; i < 1000; ++i) {
            sem_wait(&data->sem);
            data->counter++;
            sem_post(&data->sem);
        }
    } else {
        for (int i = 0; i < 1000; ++i) {
            sem_wait(&data->sem);
            data->counter++;
            sem_post(&data->sem);
        }
        wait(nullptr);
        std::cout << "Final counter: " << data->counter << std::endl;  // 2000
    }

    sem_destroy(&data->sem);
    munmap(data, sizeof(SharedData));
    shm_unlink(shmName);
}
```

### 3.4 消息队列

```cpp
#include <mqueue.h>
#include <cstring>
#include <iostream>

void messageQueueDemo() {
    const char* mqName = "/my_mq";

    // 创建消息队列
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(mqName, O_CREAT | O_RDWR, 0666, &attr);

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程：发送消息
        const char* msg = "Hello from child!";
        mq_send(mq, msg, strlen(msg) + 1, 0);
    } else {
        // 父进程：接收消息
        sleep(1);
        char buffer[256];
        mq_receive(mq, buffer, sizeof(buffer), nullptr);
        std::cout << "Parent received: " << buffer << std::endl;
        wait(nullptr);
    }

    mq_close(mq);
    mq_unlink(mqName);
}
```

### 3.5 信号 (Signal)

```cpp
#include <signal.h>
#include <iostream>
#include <unistd.h>

// 信号处理函数
volatile sig_atomic_t signalReceived = 0;

void signalHandler(int sig) {
    signalReceived = sig;
}

void signalDemo() {
    // 注册信号处理器
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, nullptr);

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程：等待信号
        while (signalReceived == 0) {
            pause();  // 等待信号
        }
        std::cout << "Child received signal: " << signalReceived << std::endl;
    } else {
        // 父进程：发送信号
        sleep(1);
        kill(pid, SIGUSR1);
        wait(nullptr);
    }
}

// 常用信号
// SIGINT   (2)  - Ctrl+C
// SIGTERM  (15) - 终止请求
// SIGKILL  (9)  - 强制终止（不可捕获）
// SIGSTOP  (19) - 暂停（不可捕获）
// SIGCHLD  (17) - 子进程状态改变
// SIGUSR1/2     - 用户自定义
```

### 3.6 Socket (本地)

```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

void unixSocketDemo() {
    const char* socketPath = "/tmp/my_socket";

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程：客户端
        sleep(1);  // 等待服务器启动

        int sock = socket(AF_UNIX, SOCK_STREAM, 0);

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

        connect(sock, (struct sockaddr*)&addr, sizeof(addr));

        const char* msg = "Hello via socket!";
        send(sock, msg, strlen(msg), 0);

        close(sock);
    } else {
        // 父进程：服务器
        int serverSock = socket(AF_UNIX, SOCK_STREAM, 0);

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

        unlink(socketPath);  // 删除旧的
        bind(serverSock, (struct sockaddr*)&addr, sizeof(addr));
        listen(serverSock, 1);

        int clientSock = accept(serverSock, nullptr, nullptr);

        char buffer[256];
        ssize_t n = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        buffer[n] = '\0';
        std::cout << "Server received: " << buffer << std::endl;

        close(clientSock);
        close(serverSock);
        unlink(socketPath);
        wait(nullptr);
    }
}
```

---

## 4. 僵尸进程与孤儿进程

### 4.1 僵尸进程

**面试问题**：什么是僵尸进程？如何避免？

**答案**：

```cpp
// 僵尸进程：子进程已终止，但父进程未调用 wait() 回收

void zombieDemo() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程立即退出
        std::cout << "Child exiting..." << std::endl;
        _exit(0);
    } else {
        // 父进程不调用 wait()
        std::cout << "Parent sleeping, child becomes zombie..." << std::endl;
        sleep(60);
        // 此时子进程是僵尸状态 (Z)
        // ps 命令可以看到 <defunct>
    }
}

// 解决方案 1：调用 wait()
void solution1() {
    pid_t pid = fork();
    if (pid > 0) {
        wait(nullptr);  // 等待子进程结束
    }
}

// 解决方案 2：忽略 SIGCHLD
void solution2() {
    signal(SIGCHLD, SIG_IGN);  // 系统自动回收

    pid_t pid = fork();
    if (pid == 0) {
        _exit(0);
    }
    // 无需 wait()
}

// 解决方案 3：SIGCHLD 处理函数中调用 wait
void sigchldHandler(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void solution3() {
    struct sigaction sa;
    sa.sa_handler = sigchldHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, nullptr);
}

// 解决方案 4：两次 fork（经典技巧）
void solution4() {
    pid_t pid = fork();

    if (pid == 0) {
        // 第一个子进程
        pid_t grandchild = fork();
        if (grandchild > 0) {
            _exit(0);  // 第一个子进程立即退出
        }
        // 孙进程执行实际任务
        // 它的父进程已退出，被 init 接管
        sleep(5);
        _exit(0);
    } else {
        wait(nullptr);  // 回收第一个子进程
    }
}
```

### 4.2 孤儿进程

**面试问题**：什么是孤儿进程？

**答案**：

```cpp
// 孤儿进程：父进程先于子进程终止
// 子进程被 init (PID 1) 或 systemd 接管

void orphanDemo() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程
        std::cout << "Child PID: " << getpid()
                  << ", Parent PID: " << getppid() << std::endl;

        sleep(5);  // 父进程在此期间退出

        std::cout << "After parent exit, Parent PID: "
                  << getppid() << std::endl;  // 变成 1 或 systemd 的 PID
    } else {
        // 父进程立即退出
        std::cout << "Parent exiting..." << std::endl;
        _exit(0);
    }
}

// 孤儿进程不是问题！
// - 会被 init/systemd 接管
// - 终止时会被正常回收
// - 与僵尸进程不同
```

---

## 5. Windows 进程

### 5.1 Windows 进程创建

```cpp
#ifdef _WIN32
#include <windows.h>
#include <iostream>

void windowsProcessDemo() {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // 创建进程
    if (CreateProcessA(
        nullptr,           // 应用程序名
        "notepad.exe",     // 命令行
        nullptr,           // 进程安全属性
        nullptr,           // 线程安全属性
        FALSE,             // 继承句柄
        0,                 // 创建标志
        nullptr,           // 环境变量
        nullptr,           // 当前目录
        &si,               // STARTUPINFO
        &pi                // PROCESS_INFORMATION
    )) {
        std::cout << "Process created, PID: " << pi.dwProcessId << std::endl;

        // 等待进程结束
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 获取退出码
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        std::cout << "Exit code: " << exitCode << std::endl;

        // 关闭句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// Windows vs Unix 进程创建对比
//
// Unix:  fork() + exec()  分两步
// Windows: CreateProcess() 一步完成
//
// Unix fork() 复制父进程
// Windows 没有 fork，直接创建新进程
#endif
```

---

## 6. 常见面试题

### Q1: 进程和线程的本质区别是什么？

**答案**：

| 方面 | 进程 | 线程 |
|------|------|------|
| **本质** | 资源分配的基本单位 | CPU 调度的基本单位 |
| **地址空间** | 独立的虚拟地址空间 | 共享所属进程的地址空间 |
| **资源** | 独立拥有资源 | 共享进程资源，仅有独立栈和寄存器 |
| **通信** | 需要 IPC | 直接读写共享内存 |
| **创建开销** | 大（复制页表、资源） | 小（仅分配栈和 TCB） |
| **切换开销** | 大（切换地址空间、刷新 TLB） | 小（仅切换栈和寄存器） |
| **崩溃影响** | 不影响其他进程 | 导致整个进程崩溃 |

---

### Q2: 为什么说协程是"用户态线程"？

**答案**：

```cpp
// 协程与线程的关系

// 1. 线程由内核调度
//    - 内核维护线程状态
//    - 切换需要系统调用
//    - 抢占式调度

// 2. 协程由用户态调度
//    - 用户程序维护协程状态
//    - 切换不需要系统调用
//    - 协作式调度（主动让出）

// 3. 协程运行在线程之上
//    - 多个协程共享一个线程
//    - 一个线程一次只运行一个协程
//    - 协程切换 = 函数调用级别的开销

// 4. 为什么叫"用户态线程"
//    - 从编程模型看，像多个并发执行的任务
//    - 从实现看，完全在用户态
//    - 早期的"绿色线程"就是这种模型

// M:N 模型
// M 个协程运行在 N 个线程上
// Go 语言的 goroutine 就是这种模型
```

---

### Q3: 多进程 vs 多线程 vs 协程，怎么选？

**答案**：

```
                   ┌─────────────────────────────┐
                   │        需要并发吗？          │
                   └─────────────┬───────────────┘
                                 │
                   ┌─────────────▼───────────────┐
                   │     需要真正的并行吗？       │
                   │    （利用多核 CPU）          │
                   └──────┬──────────────┬───────┘
                          │              │
                         是             否
                          │              │
            ┌─────────────▼─────┐   ┌───▼────────────────┐
            │  需要强隔离吗？    │   │     协程            │
            │ （崩溃不影响其他） │   │  （高并发 I/O）     │
            └──────┬──────┬─────┘   └────────────────────┘
                   │      │
                  是     否
                   │      │
         ┌─────────▼──┐  ┌▼──────────────┐
         │   多进程   │  │    多线程     │
         │ （隔离性） │  │ （共享数据）  │
         └────────────┘  └───────────────┘
```

**具体场景**：

| 场景 | 推荐方案 | 原因 |
|------|----------|------|
| 浏览器标签页 | 多进程 | 一个标签崩溃不影响其他 |
| 图像并行处理 | 多线程 | CPU 密集型，利用多核 |
| 高并发 Web 服务器 | 协程 | I/O 密集型，大量连接 |
| 数据库连接池 | 多线程 | 需要并行查询 |
| 微服务 | 多进程 | 独立部署、隔离 |
| GUI 程序 | 多线程 | 主线程 UI + 工作线程 |

---

### Q4: 共享内存为什么需要同步？

**答案**：

```cpp
// 共享内存本身不提供同步机制
// 多个进程同时读写会导致竞态条件

// 错误示例：无同步
struct SharedCounter {
    int value;  // 多个进程同时 ++value 会出错
};

// 正确示例：使用信号量同步
struct SyncedCounter {
    sem_t mutex;
    int value;
};

// 或使用文件锁、互斥量等

// 常用同步方式：
// 1. 信号量 (sem_t) - POSIX
// 2. 互斥量 (pthread_mutex_t) - 需要设置进程共享属性
// 3. 文件锁 (flock, fcntl)
// 4. 原子操作 (需要特殊支持)
```

---

### Q5: fork() 之后，父子进程共享什么？

**答案**：

```cpp
// fork() 后的共享情况

// 【复制的（各自独立）】
// - 地址空间（通过 COW 优化）
// - 栈
// - 堆
// - 数据段
// - 寄存器
// - 信号处理器（复制，但可以各自修改）

// 【共享的】
// - 代码段（只读，自然共享）
// - 打开的文件描述符（指向相同的文件表项）
// - mmap 的 MAP_SHARED 区域

// 文件描述符共享示例
void fdSharingDemo() {
    int fd = open("test.txt", O_RDWR);

    pid_t pid = fork();

    if (pid == 0) {
        write(fd, "child", 5);  // 子进程写入
    } else {
        sleep(1);
        write(fd, "parent", 6);  // 父进程写入
        // 两次写入到同一个文件！
        // 因为共享同一个文件表项（包括偏移量）
        wait(nullptr);
    }

    close(fd);
}
```

---

## 7. 总结

### 核心要点

| 概念 | 要点 |
|------|------|
| **进程** | 资源分配单位，独立地址空间，通过 IPC 通信 |
| **线程** | CPU 调度单位，共享地址空间，需要同步 |
| **协程** | 用户态调度，协作式，轻量级 |
| **IPC** | 管道、共享内存、消息队列、信号、Socket |
| **僵尸进程** | 子进程终止但未被 wait()，需要回收 |

### 面试必记

1. **进程 vs 线程 vs 协程**：调度者、开销、并行性
2. **fork() + exec()**：Unix 进程创建模式
3. **IPC 方式**：共享内存最快，但需要同步
4. **僵尸进程**：原因和解决方案
5. **COW**：fork() 的优化机制

---

## 8. 高频补充：主线程、进程退出、孤儿与僵尸

### Q1: 进程是不是必须有线程？

**答案：**
- 是。线程是执行单元，进程运行时至少有一个线程（通常称主线程）。
- 进程是资源容器，不脱离线程执行代码。

### Q2: 进程是否一定随着主线程结束而结束？

**答案：**
- 常见情况：`main` 返回或调用 `exit`，会结束整个进程（所有线程一起结束）。
- 但“主线程先结束”不必然导致进程立即结束：例如主线程调用 `pthread_exit`，其他线程可继续运行。
- 任何线程调用 `exit/_exit/abort` 都会终止整个进程。

### Q3: “主线程结束了，子进程还在运行”是孤儿线程吗？

**答案：**
- 不是，这是“孤儿进程”概念，不是“孤儿线程”。
- 孤儿进程会被 `init/systemd` 接管，本身不等于资源泄漏。

### Q4: 僵尸进程和孤儿进程的区别？

**答案：**
- 僵尸进程：子进程已退出，但父进程未 `wait` 回收，仍占用进程表项。
- 孤儿进程：父进程先退出，子进程被系统接管继续运行。
- 真正需要重点防的是僵尸进程，而不是孤儿进程。
