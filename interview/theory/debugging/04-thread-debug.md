# 多线程调试 (Multi-threading Debugging)

多线程问题是最难调试的问题类型之一，包括数据竞争、死锁、活锁等。

---

## 1. 多线程问题概述

### 1.1 问题分类

```
┌─────────────────────────────────────────────────────────────┐
│                   多线程问题分类                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 数据竞争 (Data Race)                                    │
│     多个线程同时访问共享数据，至少一个是写操作                │
│     无同步保护                                               │
│                                                             │
│  2. 死锁 (Deadlock)                                         │
│     两个或多个线程互相等待对方持有的锁                       │
│     所有线程都无法继续                                       │
│                                                             │
│  3. 活锁 (Livelock)                                         │
│     线程不断重试但永远无法成功                               │
│     消耗 CPU 但无进展                                        │
│                                                             │
│  4. 饥饿 (Starvation)                                       │
│     某些线程长期得不到资源                                   │
│     优先级反转是典型场景                                     │
│                                                             │
│  5. 原子性违反 (Atomicity Violation)                        │
│     复合操作未作为原子操作执行                               │
│                                                             │
│  6. 顺序违反 (Order Violation)                              │
│     操作执行顺序与预期不符                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 问题代码示例

```cpp
// 1. 数据竞争
int counter = 0;
void data_race() {
    // 两个线程同时执行
    for (int i = 0; i < 100000; i++) {
        counter++;  // 读-改-写，非原子操作
    }
}

// 2. 死锁
std::mutex mutex_a, mutex_b;
void thread_1() {
    std::lock_guard<std::mutex> lock_a(mutex_a);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lock_b(mutex_b);  // 等待 B
}
void thread_2() {
    std::lock_guard<std::mutex> lock_b(mutex_b);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lock_a(mutex_a);  // 等待 A
}

// 3. 原子性违反
class BankAccount {
    int balance_ = 100;
    std::mutex mutex_;
public:
    void transfer(BankAccount& to, int amount) {
        // 错误：两个操作应该是原子的
        mutex_.lock();
        balance_ -= amount;
        mutex_.unlock();

        to.mutex_.lock();
        to.balance_ += amount;
        to.mutex_.unlock();
        // 中间状态可见：钱从 A 扣除但未到 B
    }
};

// 4. 顺序违反
bool ready = false;
int data = 0;

void producer() {
    data = 42;          // (1)
    ready = true;       // (2)
    // 编译器/CPU 可能重排序 (2) 在 (1) 之前
}

void consumer() {
    while (!ready);     // (3)
    assert(data == 42); // (4) 可能失败
}

// 5. TOCTOU (Time-of-check to time-of-use)
void toctou_race() {
    if (file_exists("config.txt")) {   // 检查
        // 另一线程可能在此时删除文件
        read_file("config.txt");        // 使用
    }
}
```

---

## 2. 数据竞争检测

### 2.1 ThreadSanitizer (TSan)

```bash
# 编译启用 TSan
clang++ -fsanitize=thread -g -O1 main.cpp -o MyApp
# 或
g++ -fsanitize=thread -g -O1 main.cpp -o MyApp

# 运行
./MyApp

# TSan 输出示例：
# ==================
# WARNING: ThreadSanitizer: data race (pid=12345)
#   Write of size 4 at 0x000000601078 by thread T1:
#     #0 increment main.cpp:10
#     #1 thread_func main.cpp:20
#
#   Previous write of size 4 at 0x000000601078 by thread T2:
#     #0 increment main.cpp:10
#     #1 thread_func main.cpp:20
#
#   Location is global 'counter' of size 4 at 0x000000601078
#
#   Thread T1 (tid=12346, running) created by main thread at:
#     #0 pthread_create
#     #1 main main.cpp:30
#
#   Thread T2 (tid=12347, running) created by main thread at:
#     #0 pthread_create
#     #1 main main.cpp:31
# ==================
```

### 2.2 TSan 环境变量

```bash
# 控制 TSan 行为
export TSAN_OPTIONS="history_size=7:halt_on_error=0:log_path=tsan.log"

# 常用选项：
# halt_on_error=0        出错不停止
# history_size=7         保留历史深度（0-7，越大越耗内存）
# second_deadlock_stack=1  显示死锁第二个栈
# log_path=tsan.log      输出到文件
# suppressions=tsan.supp 忽略已知问题
# detect_deadlocks=1     检测死锁
```

### 2.3 TSan Suppressions

```
# tsan.supp - 忽略已知的竞争

# 忽略特定函数
race:known_benign_race

# 忽略特定文件
race_top:third_party/*

# 忽略特定调用栈
race:^init_system$

# 死锁忽略
deadlock:legacy_lock_function
```

```bash
# 使用 suppressions
TSAN_OPTIONS="suppressions=tsan.supp" ./MyApp
```

### 2.4 Helgrind (Valgrind)

```bash
# 使用 Helgrind 检测竞争
valgrind --tool=helgrind ./MyApp

# 输出示例：
# ==12345== Possible data race during read of size 4 at 0x601078
# ==12345==    at 0x400567: increment (main.cpp:10)
# ==12345==    by 0x400789: thread_func (main.cpp:20)
# ==12345==  This conflicts with a previous write of size 4 by thread #2
# ==12345==    at 0x400567: increment (main.cpp:10)
# ==12345==    by 0x400789: thread_func (main.cpp:20)
```

---

## 3. 死锁分析

### 3.1 死锁条件

```
┌─────────────────────────────────────────────────────────────┐
│                   死锁四个必要条件                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 互斥 (Mutual Exclusion)                                 │
│     资源只能被一个线程持有                                   │
│                                                             │
│  2. 持有并等待 (Hold and Wait)                              │
│     持有资源的同时等待其他资源                               │
│                                                             │
│  3. 不可抢占 (No Preemption)                                │
│     资源不能被强制释放                                       │
│                                                             │
│  4. 循环等待 (Circular Wait)                                │
│     存在循环等待链                                          │
│                                                             │
│  ┌─────────┐  等待 B  ┌─────────┐                          │
│  │ Thread1 │ ───────→ │ Thread2 │                          │
│  │ 持有 A  │          │ 持有 B  │                          │
│  └─────────┘ ←─────── └─────────┘                          │
│              等待 A                                         │
│                                                             │
│  打破任一条件即可避免死锁                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Windows 死锁调试

```
// WinDbg 死锁分析

// 1. 查看所有线程
~*k

// 2. 查看线程等待状态
!locks

// 3. 查看关键区
!cs -l        // 列出所有关键区
!cs <address> // 查看特定关键区

// 4. 查看等待链
!analyze -hang

// 5. 死锁检测
!deadlock

// 输出示例：
// Deadlock detected (2 threads, 2 locks):
//
// Thread 1:
//   waiting on critical section at 0x00123456
//   held by thread 2
//   while holding critical section at 0x00123ABC
//
// Thread 2:
//   waiting on critical section at 0x00123ABC
//   held by thread 1
//   while holding critical section at 0x00123456
```

### 3.3 Linux 死锁调试

```bash
# GDB 死锁分析

# 1. 附加到进程
gdb -p <pid>

# 2. 查看所有线程
(gdb) info threads

# 3. 查看所有线程调用栈
(gdb) thread apply all bt

# 4. 切换到特定线程
(gdb) thread 2

# 5. 查看 pthread 锁状态
(gdb) print (pthread_mutex_t)mutex
# 查看 __owner 字段显示持有者线程 ID

# 6. 使用 pstack 查看线程栈（无需 GDB）
pstack <pid>
```

### 3.4 LLDB 死锁调试

```bash
# LLDB 死锁分析

# 1. 附加到进程
lldb -p <pid>

# 2. 查看所有线程
(lldb) thread list

# 3. 查看所有线程调用栈
(lldb) thread backtrace all

# 4. 切换线程
(lldb) thread select 2

# 5. 分析等待状态
# 查看函数参数中的锁地址
(lldb) frame variable
```

### 3.5 pstack/gstack 快速诊断

```bash
# pstack - 无需 GDB 即可查看进程所有线程调用栈
# 适用于生产环境快速诊断

# 基本用法
pstack <pid>

# 输出示例（死锁场景）：
# Thread 3 (Thread 0x7f1234567700 (LWP 12346)):
# #0  __lll_lock_wait () at lowlevellock.S:135
# #1  pthread_mutex_lock () at pthread_mutex_lock.c:80
# #2  std::mutex::lock() at mutex:135
# #3  transfer_money(Account&, Account&, int) at bank.cpp:45
# #4  thread_func() at main.cpp:20
#
# Thread 2 (Thread 0x7f1234568700 (LWP 12345)):
# #0  __lll_lock_wait () at lowlevellock.S:135
# #1  pthread_mutex_lock () at pthread_mutex_lock.c:80
# #2  std::mutex::lock() at mutex:135
# #3  transfer_money(Account&, Account&, int) at bank.cpp:45
# #4  thread_func() at main.cpp:25

# 关键特征识别：
# 1. 多个线程都在 __lll_lock_wait（futex 等待）
# 2. 调用栈显示在等待锁
# 3. 互相等待对方持有的资源

# 多次采样对比（确认是死锁而非慢操作）
for i in {1..5}; do
    echo "=== Sample $i ===" >> stacks.log
    pstack <pid> >> stacks.log
    sleep 1
done
# 如果多次采样栈都相同 → 死锁
# 如果栈在变化 → 可能是慢操作或活锁

# 使用 gstack（某些系统上是 pstack 的别名）
gstack <pid>

# 结合 grep 快速过滤
pstack <pid> | grep -A5 "__lll_lock_wait\|pthread_mutex_lock\|futex"
```

```
┌─────────────────────────────────────────────────────────────┐
│              pstack 死锁诊断速查表                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  等待函数                          含义                     │
│  ─────────────────────────────────────────────────────────  │
│  __lll_lock_wait                  等待 mutex/futex         │
│  pthread_mutex_lock               获取互斥锁                │
│  pthread_rwlock_rdlock/wrlock     读写锁等待                │
│  pthread_cond_wait                条件变量等待              │
│  sem_wait                         信号量等待                │
│  futex                            低级同步原语              │
│                                                             │
│  诊断步骤：                                                  │
│  1. pstack <pid> 获取所有线程栈                             │
│  2. 找出在等待锁的线程                                      │
│  3. 从栈中识别锁的地址/变量名                               │
│  4. 确认是否形成循环等待                                    │
│  5. 多次采样确认是死锁（栈不变）还是慢操作（栈在变）         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.6 死锁日志分析技巧

```cpp
// === 1. 锁操作日志封装 ===
// 在关键系统中添加锁日志，便于事后分析

#include <chrono>
#include <thread>
#include <sstream>
#include <fstream>

class LoggedMutex {
public:
    explicit LoggedMutex(const std::string& name) : name_(name) {}

    void lock() {
        auto tid = std::this_thread::get_id();
        auto start = std::chrono::steady_clock::now();

        log("LOCK_WAIT", tid);
        mutex_.lock();
        auto elapsed = std::chrono::steady_clock::now() - start;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        log("LOCK_ACQUIRED", tid, ms);

        // 警告：等待时间过长可能是死锁前兆
        if (ms > 1000) {
            log("LOCK_SLOW_WARNING", tid, ms);
        }
    }

    void unlock() {
        auto tid = std::this_thread::get_id();
        log("UNLOCK", tid);
        mutex_.unlock();
    }

    bool try_lock() {
        auto tid = std::this_thread::get_id();
        bool success = mutex_.try_lock();
        log(success ? "TRY_LOCK_SUCCESS" : "TRY_LOCK_FAILED", tid);
        return success;
    }

private:
    void log(const char* action, std::thread::id tid, long ms = -1) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%H:%M:%S")
            << " [" << tid << "] "
            << action << " " << name_;
        if (ms >= 0) {
            oss << " (" << ms << "ms)";
        }
        oss << "\n";

        // 写入日志（生产环境应使用异步日志）
        std::cerr << oss.str();
    }

    std::mutex mutex_;
    std::string name_;
};

// 使用示例
LoggedMutex account_a_lock("account_A");
LoggedMutex account_b_lock("account_B");
```

```
┌─────────────────────────────────────────────────────────────┐
│              死锁日志分析模式                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  正常日志模式：                                              │
│  10:00:01 [T1] LOCK_WAIT account_A                          │
│  10:00:01 [T1] LOCK_ACQUIRED account_A (0ms)                │
│  10:00:01 [T1] LOCK_WAIT account_B                          │
│  10:00:01 [T1] LOCK_ACQUIRED account_B (0ms)                │
│  10:00:01 [T1] UNLOCK account_B                             │
│  10:00:01 [T1] UNLOCK account_A                             │
│                                                             │
│  死锁日志模式：                                              │
│  10:00:01 [T1] LOCK_WAIT account_A                          │
│  10:00:01 [T1] LOCK_ACQUIRED account_A (0ms)                │
│  10:00:01 [T2] LOCK_WAIT account_B                          │
│  10:00:01 [T2] LOCK_ACQUIRED account_B (0ms)                │
│  10:00:01 [T1] LOCK_WAIT account_B      ← T1 等待 B         │
│  10:00:01 [T2] LOCK_WAIT account_A      ← T2 等待 A（死锁） │
│  ... 无后续 LOCK_ACQUIRED ...           ← 确认死锁          │
│                                                             │
│  分析命令：                                                  │
│  # 找出等待但未获取的锁                                     │
│  grep LOCK_WAIT app.log | sort > waits.txt                  │
│  grep LOCK_ACQUIRED app.log | sort > acquired.txt           │
│  diff waits.txt acquired.txt                                │
│                                                             │
│  # 分析特定时间段的锁行为                                    │
│  grep "10:00:01" app.log | grep -E "LOCK_WAIT|LOCK_ACQ"     │
│                                                             │
│  # 找出等待时间过长的锁                                     │
│  grep LOCK_SLOW_WARNING app.log                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```cpp
// === 2. 死锁检测日志分析脚本 ===

// Python 脚本示例（analyze_deadlock.py）
/*
#!/usr/bin/env python3
import re
from collections import defaultdict

def analyze_lock_log(log_file):
    # 解析日志
    pattern = r'(\d+:\d+:\d+) \[([^\]]+)\] (\w+) (\w+)'

    thread_holds = defaultdict(set)   # 线程持有的锁
    thread_waits = {}                  # 线程等待的锁
    lock_holder = {}                   # 锁的持有者

    with open(log_file) as f:
        for line in f:
            match = re.match(pattern, line)
            if not match:
                continue

            time, tid, action, lock = match.groups()

            if action == 'LOCK_ACQUIRED':
                thread_holds[tid].add(lock)
                lock_holder[lock] = tid
                if tid in thread_waits:
                    del thread_waits[tid]

            elif action == 'LOCK_WAIT':
                thread_waits[tid] = lock

            elif action == 'UNLOCK':
                thread_holds[tid].discard(lock)
                if lock in lock_holder:
                    del lock_holder[lock]

    # 检测死锁：等待的锁被其他等待线程持有
    print("=== 潜在死锁分析 ===")
    for tid, waiting_lock in thread_waits.items():
        if waiting_lock in lock_holder:
            holder = lock_holder[waiting_lock]
            print(f"线程 {tid} 等待锁 {waiting_lock}，被线程 {holder} 持有")

            # 检查是否形成循环
            if holder in thread_waits:
                holder_waiting = thread_waits[holder]
                if holder_waiting in thread_holds.get(tid, set()):
                    print(f"  ⚠️ 死锁！线程 {holder} 等待 {holder_waiting}，被 {tid} 持有")

if __name__ == '__main__':
    import sys
    analyze_lock_log(sys.argv[1])
*/
```

```bash
# === 3. 生产环境死锁日志收集 ===

# 当程序卡死时，收集诊断信息
#!/bin/bash
PID=$1
OUTDIR="deadlock_$(date +%Y%m%d_%H%M%S)"
mkdir -p $OUTDIR

# 1. 多次采样线程栈
for i in {1..10}; do
    pstack $PID > "$OUTDIR/stack_$i.txt" 2>&1
    sleep 0.5
done

# 2. 收集 /proc 信息
cat /proc/$PID/status > "$OUTDIR/status.txt"
cat /proc/$PID/maps > "$OUTDIR/maps.txt"
ls -la /proc/$PID/fd > "$OUTDIR/fd.txt"

# 3. 收集锁相关信息
cat /proc/$PID/stack > "$OUTDIR/kernel_stack.txt" 2>/dev/null

# 4. strace 追踪系统调用（可选，可能影响性能）
timeout 5 strace -p $PID -f -e futex 2>&1 | head -100 > "$OUTDIR/strace.txt"

# 5. 生成 core dump（可选）
# gcore $PID

echo "诊断信息已收集到 $OUTDIR"
```

```
┌─────────────────────────────────────────────────────────────┐
│              死锁日志分析最佳实践                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  预防性日志：                                                │
│  1. 关键锁添加 LOCK_WAIT/LOCK_ACQUIRED/UNLOCK 日志          │
│  2. 记录等待时间，设置慢锁告警阈值                          │
│  3. 日志包含时间戳、线程 ID、锁名称                         │
│  4. 使用异步日志避免影响性能                                 │
│                                                             │
│  实时监控：                                                  │
│  1. 定期采样 pstack 检查线程状态                            │
│  2. 监控锁等待时间指标                                      │
│  3. 设置死锁检测告警（等待超时）                            │
│                                                             │
│  事后分析：                                                  │
│  1. 找出最后的 LOCK_WAIT 但无对应 LOCK_ACQUIRED             │
│  2. 画出锁等待图，检测循环                                  │
│  3. 多次 pstack 采样确认栈不变                              │
│  4. 结合业务逻辑分析锁顺序问题                              │
│                                                             │
│  日志格式建议：                                              │
│  [时间戳] [线程ID] [动作] [锁名] [等待时间ms] [调用位置]    │
│  例：                                                        │
│  2024-01-15 10:00:01.123 [T-1234] LOCK_ACQ mutex_A 5ms      │
│      at transfer() bank.cpp:42                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.7 死锁检测代码

```cpp
// 运行时死锁检测
#include <mutex>
#include <thread>
#include <map>
#include <set>
#include <vector>

class DeadlockDetector {
public:
    void before_lock(void* mutex) {
        std::lock_guard<std::mutex> lock(detector_mutex_);
        auto tid = std::this_thread::get_id();

        // 检查是否会形成循环
        if (would_cause_cycle(tid, mutex)) {
            fprintf(stderr, "Potential deadlock detected!\n");
            print_wait_graph();
            // abort() 或记录日志
        }

        // 记录等待关系
        waiting_for_[tid] = mutex;
    }

    void after_lock(void* mutex) {
        std::lock_guard<std::mutex> lock(detector_mutex_);
        auto tid = std::this_thread::get_id();

        waiting_for_.erase(tid);
        held_by_[mutex] = tid;
        locks_held_[tid].insert(mutex);
    }

    void before_unlock(void* mutex) {
        std::lock_guard<std::mutex> lock(detector_mutex_);
        auto tid = std::this_thread::get_id();

        held_by_.erase(mutex);
        locks_held_[tid].erase(mutex);
    }

private:
    bool would_cause_cycle(std::thread::id tid, void* mutex) {
        // BFS 检测是否会形成循环
        std::set<std::thread::id> visited;
        std::vector<std::thread::id> queue;

        if (held_by_.count(mutex)) {
            queue.push_back(held_by_[mutex]);
        }

        while (!queue.empty()) {
            auto current = queue.back();
            queue.pop_back();

            if (current == tid) return true;  // 发现循环
            if (visited.count(current)) continue;
            visited.insert(current);

            // 检查 current 线程在等待什么
            if (waiting_for_.count(current)) {
                void* waiting_mutex = waiting_for_[current];
                if (held_by_.count(waiting_mutex)) {
                    queue.push_back(held_by_[waiting_mutex]);
                }
            }
        }
        return false;
    }

    std::mutex detector_mutex_;
    std::map<std::thread::id, void*> waiting_for_;
    std::map<void*, std::thread::id> held_by_;
    std::map<std::thread::id, std::set<void*>> locks_held_;
};
```

---

## 4. 死锁预防

### 4.1 锁顺序

```cpp
// 固定锁顺序避免死锁
class BankAccount {
public:
    int id;
    std::mutex mutex;
    int balance;

    static void transfer(BankAccount& from, BankAccount& to, int amount) {
        // 按 ID 顺序加锁
        BankAccount* first = &from;
        BankAccount* second = &to;
        if (from.id > to.id) {
            std::swap(first, second);
        }

        std::lock_guard<std::mutex> lock1(first->mutex);
        std::lock_guard<std::mutex> lock2(second->mutex);

        from.balance -= amount;
        to.balance += amount;
    }
};
```

### 4.2 std::lock 和 std::scoped_lock

```cpp
// 使用 std::lock 同时加多个锁（避免死锁）
void safe_transfer(BankAccount& from, BankAccount& to, int amount) {
    // std::lock 会避免死锁
    std::lock(from.mutex, to.mutex);
    std::lock_guard<std::mutex> lock1(from.mutex, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(to.mutex, std::adopt_lock);

    from.balance -= amount;
    to.balance += amount;
}

// C++17: std::scoped_lock（推荐）
void safe_transfer_cpp17(BankAccount& from, BankAccount& to, int amount) {
    std::scoped_lock lock(from.mutex, to.mutex);
    from.balance -= amount;
    to.balance += amount;
}
```

### 4.3 超时锁

```cpp
// 使用 try_lock 避免死锁
void transfer_with_timeout(BankAccount& from, BankAccount& to, int amount) {
    while (true) {
        std::unique_lock<std::mutex> lock1(from.mutex, std::defer_lock);
        std::unique_lock<std::mutex> lock2(to.mutex, std::defer_lock);

        if (lock1.try_lock()) {
            if (lock2.try_lock()) {
                // 成功获取两个锁
                from.balance -= amount;
                to.balance += amount;
                return;
            }
            // 未能获取第二个锁，释放第一个
        }

        // 退避重试
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
    }
}
```

### 4.4 层级锁

```cpp
// 层级锁：只能按层级顺序加锁
class HierarchicalMutex {
public:
    explicit HierarchicalMutex(unsigned level) : level_(level) {}

    void lock() {
        check_hierarchy();
        mutex_.lock();
        update_hierarchy();
    }

    void unlock() {
        if (this_thread_level_ != level_) {
            throw std::logic_error("Mutex hierarchy violated");
        }
        this_thread_level_ = prev_level_;
        mutex_.unlock();
    }

    bool try_lock() {
        check_hierarchy();
        if (!mutex_.try_lock()) return false;
        update_hierarchy();
        return true;
    }

private:
    void check_hierarchy() {
        if (this_thread_level_ <= level_) {
            throw std::logic_error("Mutex hierarchy violated");
        }
    }

    void update_hierarchy() {
        prev_level_ = this_thread_level_;
        this_thread_level_ = level_;
    }

    std::mutex mutex_;
    unsigned const level_;
    unsigned prev_level_ = 0;
    static thread_local unsigned this_thread_level_;
};

thread_local unsigned HierarchicalMutex::this_thread_level_ = UINT_MAX;

// 使用
HierarchicalMutex high_level(10000);
HierarchicalMutex mid_level(5000);
HierarchicalMutex low_level(1000);

void do_work() {
    std::lock_guard<HierarchicalMutex> l1(high_level);  // OK
    std::lock_guard<HierarchicalMutex> l2(mid_level);   // OK
    std::lock_guard<HierarchicalMutex> l3(low_level);   // OK

    // std::lock_guard<HierarchicalMutex> l4(high_level);  // 异常！
}
```

---

## 5. 内存顺序与原子操作

### 5.1 内存顺序问题

```cpp
// 错误示例：无同步的共享数据
bool ready = false;
int data = 0;

void producer() {
    data = 42;          // (1)
    ready = true;       // (2)
}

void consumer() {
    while (!ready);     // (3)
    assert(data == 42); // (4) 可能失败！
}

// 问题：
// 1. 编译器可能重排序 (1) 和 (2)
// 2. CPU 可能重排序内存访问
// 3. 其他 CPU 可能以不同顺序观察到修改
```

### 5.2 正确的同步

```cpp
#include <atomic>

// 方法 1: 使用 atomic + 默认顺序 (seq_cst)
std::atomic<bool> ready{false};
std::atomic<int> data{0};

void producer() {
    data.store(42);
    ready.store(true);
}

void consumer() {
    while (!ready.load());
    assert(data.load() == 42);  // 保证成功
}

// 方法 2: 使用 release-acquire 语义
std::atomic<bool> ready{false};
int data = 0;  // 非原子

void producer() {
    data = 42;                              // (1)
    ready.store(true, std::memory_order_release);  // (2)
    // release 保证 (1) 在 (2) 之前完成
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)); // (3)
    // acquire 保证看到 release 之前的所有写入
    assert(data == 42);  // (4) 保证成功
}
```

### 5.3 内存顺序选项

```cpp
// std::memory_order 选项

// 1. memory_order_seq_cst（默认，最强）
// 所有线程看到相同的操作顺序
atomic.store(val, std::memory_order_seq_cst);
atomic.load(std::memory_order_seq_cst);

// 2. memory_order_acquire
// 本线程后续读写不能重排到此操作之前
atomic.load(std::memory_order_acquire);

// 3. memory_order_release
// 本线程之前的读写不能重排到此操作之后
atomic.store(val, std::memory_order_release);

// 4. memory_order_acq_rel
// 同时具有 acquire 和 release 语义
atomic.exchange(val, std::memory_order_acq_rel);

// 5. memory_order_relaxed
// 只保证原子性，不保证顺序
atomic.store(val, std::memory_order_relaxed);
atomic.load(std::memory_order_relaxed);

// 6. memory_order_consume（不推荐使用）
// 数据依赖关系保证
```

---

## 6. 调试工具实战

### 6.1 WinDbg 线程分析

```
// WinDbg 多线程调试命令

// 1. 列出所有线程
~*

// 2. 显示所有线程调用栈
~*k

// 3. 切换到特定线程
~2s

// 4. 查看线程信息
!thread

// 5. 查看线程等待原因
!thread <address> 8

// 6. 查看同步对象
!handle

// 7. 查看关键区状态
!cs -l

// 8. 查看 SRW 锁
!srwlock <address>

// 9. 死锁分析
!analyze -hang

// 10. 设置线程特定断点
~2 bp MyApp!Function
```

### 6.2 GDB 线程分析

```bash
# GDB 多线程调试

# 1. 查看所有线程
(gdb) info threads

# 2. 切换线程
(gdb) thread 2

# 3. 所有线程执行命令
(gdb) thread apply all bt

# 4. 设置线程特定断点
(gdb) break function thread 2

# 5. 控制线程执行
(gdb) set scheduler-locking on  # 只运行当前线程
(gdb) set scheduler-locking off # 所有线程运行

# 6. 查看线程本地存储
(gdb) info tls

# 7. 条件断点（特定线程）
(gdb) break function if $_thread == 2

# 8. 观察点（数据竞争）
(gdb) watch variable
```

### 6.3 LLDB 线程分析

```bash
# LLDB 多线程调试

# 1. 查看所有线程
(lldb) thread list

# 2. 切换线程
(lldb) thread select 2

# 3. 所有线程调用栈
(lldb) thread backtrace all

# 4. 设置线程条件断点
(lldb) breakpoint set -n function -x 2

# 5. 观察变量
(lldb) watchpoint set variable counter

# 6. 线程步进控制
(lldb) thread step-over  # 当前线程步过
(lldb) thread continue   # 继续当前线程
```

---

## 7. 常见多线程 Bug 模式

### 7.1 双重检查锁定 (DCLP)

```cpp
// 错误的 DCLP
class Singleton {
    static Singleton* instance_;
    static std::mutex mutex_;
public:
    static Singleton* get_instance() {
        if (instance_ == nullptr) {              // (1) 第一次检查
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance_ == nullptr) {          // (2) 第二次检查
                instance_ = new Singleton();     // (3) 问题所在！
            }
        }
        return instance_;
    }
};

// 问题：
// (3) 可能被重排为：
// 1. 分配内存
// 2. 将指针赋值给 instance_（此时对象未构造完成）
// 3. 调用构造函数
// 其他线程在 (1) 看到非空指针，但对象未完全构造

// 正确的 DCLP (C++11)
class Singleton {
    static std::atomic<Singleton*> instance_;
    static std::mutex mutex_;
public:
    static Singleton* get_instance() {
        Singleton* p = instance_.load(std::memory_order_acquire);
        if (p == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            p = instance_.load(std::memory_order_relaxed);
            if (p == nullptr) {
                p = new Singleton();
                instance_.store(p, std::memory_order_release);
            }
        }
        return p;
    }
};

// 更简单的方式：Meyers' Singleton（C++11 保证线程安全）
class Singleton {
public:
    static Singleton& get_instance() {
        static Singleton instance;  // C++11 保证线程安全初始化
        return instance;
    }
};
```

### 7.2 ABA 问题

```cpp
// ABA 问题示例（无锁栈）
std::atomic<Node*> head;

void push(Node* node) {
    node->next = head.load();
    while (!head.compare_exchange_weak(node->next, node));
}

Node* pop() {
    Node* old_head = head.load();
    while (old_head &&
           !head.compare_exchange_weak(old_head, old_head->next));
    return old_head;
}

// ABA 问题场景：
// 1. 线程 1 读取 head = A，准备 CAS 为 A->next
// 2. 线程 1 被挂起
// 3. 线程 2 执行 pop A，pop B，push A（A 回到栈顶，但 next 已变）
// 4. 线程 1 恢复，CAS 成功（head 仍是 A），但 A->next 已变
// 5. 数据结构损坏

// 解决：使用版本号/标签
template<typename T>
struct TaggedPointer {
    T* ptr;
    uintptr_t tag;  // 每次操作递增
};

std::atomic<TaggedPointer<Node>> head;
```

### 7.3 虚假唤醒

```cpp
// 错误：未处理虚假唤醒
std::condition_variable cv;
std::mutex mutex;
bool ready = false;

void consumer() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock);  // 可能虚假唤醒！
    process_data();
}

// 正确：使用谓词
void consumer() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, []{ return ready; });  // 带谓词，自动处理虚假唤醒
    process_data();
}

// 或手动循环
void consumer() {
    std::unique_lock<std::mutex> lock(mutex);
    while (!ready) {  // 循环检查条件
        cv.wait(lock);
    }
    process_data();
}
```

---

## 8. 面试深度问题

### Q1: 数据竞争和竞态条件有什么区别？

**答案**：

```
数据竞争 (Data Race)：
- 严格的技术定义
- 两个线程同时访问同一内存位置
- 至少一个是写操作
- 没有同步
- 是未定义行为 (UB)

竞态条件 (Race Condition)：
- 更广泛的概念
- 程序行为依赖于执行时序
- 不一定是 UB
- 例如：检查后使用（TOCTOU）

关系：
- 数据竞争一定是竞态条件
- 竞态条件不一定是数据竞争

示例：

// 数据竞争
int x = 0;
Thread1: x = 1;
Thread2: x = 2;  // UB

// 竞态条件但无数据竞争
std::atomic<int> x{0};
Thread1: if (x == 0) x = 1;
Thread2: if (x == 0) x = 2;
// 有同步，但结果不确定
```

### Q2: 如何调试只在生产环境出现的多线程问题？

**答案**：

```
策略：

1. 日志增强
   - 添加详细的线程 ID 和时间戳日志
   - 记录锁获取/释放
   - 记录关键状态变化

2. 崩溃分析
   - 收集 dump 文件
   - 分析所有线程调用栈
   - 查看锁状态

3. 复现策略
   - 增加并发度
   - 添加随机延迟
   - 使用 stress 测试工具

4. 运行时检测
   - 部署带 TSan 的测试版本（性能影响大）
   - 采样式检测

5. 代码审查
   - 重点检查共享数据访问
   - 检查锁的使用范围

6. 监控指标
   - 锁等待时间
   - 线程状态分布
   - 资源使用模式

7. 渐进定位
   - 二分禁用代码
   - 缩小问题范围
```

### Q3: TSan 是如何工作的？

**答案**：

```
TSan 工作原理：

1. 编译时插桩
   每次内存访问前插入检测代码
   记录访问的地址、大小、类型（读/写）、线程

2. Shadow Memory（影子内存）
   每 8 字节应用内存 → 4 个 Shadow Word（32 字节）
   记录最近 4 次访问的元信息

3. 向量时钟 (Vector Clock)
   每个线程维护向量时钟
   同步操作传递向量时钟

4. 竞争检测
   新访问与 Shadow Memory 中的历史访问比较：
   - 不同线程
   - 至少一个是写
   - 向量时钟不可比（无同步关系）
   → 报告数据竞争

5. 同步识别
   Hook pthread/std::mutex 等同步原语
   正确建立 happens-before 关系

性能开销：
- 运行时间 5-15x
- 内存 5-10x
- 不适合生产环境
```

### Q4: 如何避免死锁？

**答案**：

```
方法：

1. 锁顺序
   - 所有线程按相同顺序获取锁
   - 使用资源 ID 排序

2. 一次性获取
   - std::lock / std::scoped_lock
   - 同时尝试所有锁

3. 超时机制
   - try_lock_for
   - 获取失败释放已有锁，重试

4. 层级锁
   - 只能获取更低层级的锁
   - 运行时检查

5. 避免嵌套锁
   - 减少锁的粒度
   - 使用无锁数据结构

6. 死锁检测
   - 运行时检测等待图
   - 发现环路报警

7. 设计原则
   - 最小化锁持有时间
   - 避免在锁内调用外部代码
   - 优先使用不可变数据
```

### Q5: 什么是 memory_order，为什么需要它？

**答案**：

```
为什么需要：

1. 编译器优化
   - 重排序指令
   - 消除"无用"操作

2. CPU 乱序执行
   - 指令流水线
   - 写缓冲区

3. 缓存一致性延迟
   - 多核缓存同步
   - MESI 协议延迟

memory_order 选项：

┌─────────────────────────────────────────────────────────────┐
│  顺序              保证                      性能           │
├─────────────────────────────────────────────────────────────┤
│  seq_cst          全局顺序一致               最慢          │
│  acquire          本线程后续操作不重排到前    中等          │
│  release          本线程之前操作不重排到后    中等          │
│  acq_rel          acquire + release          中等          │
│  relaxed          只保证原子性               最快          │
└─────────────────────────────────────────────────────────────┘

使用建议：
- 默认使用 seq_cst（最安全）
- 性能关键路径才考虑放松
- 必须完全理解语义才能使用放松顺序
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   多线程调试知识图谱                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  问题类型                                                   │
│  ├── 数据竞争                                               │
│  ├── 死锁                                                   │
│  ├── 活锁与饥饿                                             │
│  └── 内存顺序问题                                           │
│                                                             │
│  检测工具                                                   │
│  ├── ThreadSanitizer (TSan)                                │
│  ├── Helgrind (Valgrind)                                   │
│  ├── WinDbg (!locks, !cs)                                  │
│  ├── pstack/gstack 快速诊断                                │
│  └── 运行时死锁检测                                         │
│                                                             │
│  预防手段                                                   │
│  ├── 锁顺序                                                 │
│  ├── std::scoped_lock                                      │
│  ├── 层级锁                                                 │
│  └── 无锁数据结构                                           │
│                                                             │
│  调试技巧                                                   │
│  ├── 线程调用栈分析                                         │
│  ├── 锁状态检查                                             │
│  ├── 数据断点                                               │
│  ├── 日志时序分析                                           │
│  └── 死锁日志分析（LoggedMutex）                            │
│                                                             │
│  面试重点                                                   │
│  ├── 数据竞争 vs 竞态条件                                   │
│  ├── 死锁条件与预防                                         │
│  ├── pstack 死锁诊断流程                                    │
│  ├── TSan 原理                                             │
│  ├── memory_order 语义                                     │
│  └── 生产环境调试策略                                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
