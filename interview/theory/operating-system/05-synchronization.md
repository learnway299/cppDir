# 05 - 进程同步 (Process Synchronization)

## 简介

进程同步是协调多个进程或线程对共享资源访问的机制，确保数据一致性和程序正确性。

**核心概念**：
- **临界区**：访问共享资源的代码段
- **互斥**：同一时刻只有一个进程可以进入临界区
- **同步**：协调进程执行顺序
- **死锁**：多个进程互相等待资源

---

## 1. 同步基础

### 1.1 竞态条件

**面试问题**：什么是竞态条件？如何避免？

**答案**：

**竞态条件 (Race Condition)**：多个进程/线程并发访问共享资源，结果取决于执行顺序。

```cpp
// 竞态条件示例
int counter = 0;

// 线程 1                     // 线程 2
void increment() {            void increment() {
    counter++;                    counter++;
}                             }

// counter++ 实际上是三个操作：
// 1. 读取 counter
// 2. counter + 1
// 3. 写回 counter

// 可能的执行序列：
// T1: 读取 counter (0)
// T2: 读取 counter (0)
// T1: +1 得到 1
// T2: +1 得到 1
// T1: 写回 counter = 1
// T2: 写回 counter = 1  // 期望 2，实际 1
```

---

### 1.2 临界区与互斥

**面试问题**：临界区问题的解决方案需要满足哪些条件？

**答案**：

**临界区 (Critical Section)**：访问共享资源的代码段。

**解决方案必须满足的条件**：

| 条件 | 说明 |
|------|------|
| **互斥** | 同一时刻只有一个进程在临界区 |
| **进步** | 不在临界区的进程不能阻止其他进程进入 |
| **有限等待** | 进程等待进入临界区的时间有限 |

```
临界区代码结构：

┌─────────────────────────────────────┐
│         进入区 (Entry Section)      │
│         获取锁/许可                  │
├─────────────────────────────────────┤
│         临界区 (Critical Section)   │
│         访问共享资源                 │
├─────────────────────────────────────┤
│         退出区 (Exit Section)       │
│         释放锁/许可                  │
├─────────────────────────────────────┤
│         剩余区 (Remainder Section)  │
│         其他代码                     │
└─────────────────────────────────────┘
```

---

## 2. 互斥锁

### 2.1 互斥锁原理

**面试问题**：互斥锁的实现原理是什么？

**答案**：

**互斥锁 (Mutex)**：保证同一时刻只有一个线程可以进入临界区。

```cpp
#include <mutex>

std::mutex mtx;
int counter = 0;

void safe_increment() {
    mtx.lock();    // 获取锁
    counter++;     // 临界区
    mtx.unlock();  // 释放锁
}

// 更安全的写法：RAII
void safe_increment_raii() {
    std::lock_guard<std::mutex> lock(mtx);
    counter++;
}  // 自动释放锁
```

**互斥锁实现（简化版）**：

```cpp
class SpinLock {
    std::atomic<bool> locked{false};

public:
    void lock() {
        // 自旋等待
        while (locked.exchange(true, std::memory_order_acquire)) {
            // 等待锁释放
        }
    }

    void unlock() {
        locked.store(false, std::memory_order_release);
    }
};
```

---

### 2.2 读写锁

**面试问题**：什么是读写锁？适用什么场景？

**答案**：

**读写锁 (Read-Write Lock)**：允许多个读者同时读，但写者独占。

| 请求 | 当前状态 | 是否允许 |
|------|----------|----------|
| 读 | 无锁 | 允许 |
| 读 | 有读锁 | 允许 |
| 读 | 有写锁 | 阻塞 |
| 写 | 无锁 | 允许 |
| 写 | 有读锁 | 阻塞 |
| 写 | 有写锁 | 阻塞 |

```cpp
#include <shared_mutex>

std::shared_mutex rw_mutex;
int data = 0;

void reader() {
    std::shared_lock<std::shared_mutex> lock(rw_mutex);
    // 多个读者可以同时读
    std::cout << data << std::endl;
}

void writer() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex);
    // 写者独占
    data++;
}
```

**适用场景**：读多写少的场景（如缓存、配置）

---

### 2.3 自旋锁 vs 互斥锁

**面试问题**：自旋锁和互斥锁有什么区别？

**答案**：

| 特性 | 自旋锁 | 互斥锁 |
|------|--------|--------|
| **等待方式** | 忙等待（循环检查） | 睡眠等待 |
| **CPU 占用** | 高（一直占用） | 低（挂起释放） |
| **适用场景** | 临界区短、多核 | 临界区长 |
| **开销** | 无上下文切换 | 有上下文切换 |
| **中断上下文** | 可使用 | 不可使用 |

```cpp
// 自旋锁：适合临界区很短的情况
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// 互斥锁：适合临界区较长的情况
std::mutex mtx;  // 获取失败会挂起线程
```

---

## 3. 信号量

### 3.1 信号量原理

**面试问题**：什么是信号量？P/V 操作是什么？

**答案**：

**信号量 (Semaphore)**：一个整数变量，用于控制对共享资源的访问。

**P/V 操作**（来自荷兰语）：
- **P (wait/down)**：等待/减少信号量
- **V (signal/up)**：释放/增加信号量

```cpp
#include <semaphore>

// C++20 counting_semaphore
std::counting_semaphore<10> sem(10);  // 初始值 10

void access_resource() {
    sem.acquire();  // P 操作：信号量 -1，若 <0 则阻塞
    // 访问资源
    sem.release();  // V 操作：信号量 +1，可能唤醒等待线程
}
```

**信号量实现原理**：

```cpp
class Semaphore {
    int count;
    std::mutex mtx;
    std::condition_variable cv;

public:
    Semaphore(int n) : count(n) {}

    void P() {  // wait
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return count > 0; });
        count--;
    }

    void V() {  // signal
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};
```

---

### 3.2 信号量类型

| 类型 | 初始值 | 用途 |
|------|--------|------|
| **二元信号量** | 0 或 1 | 等价于互斥锁 |
| **计数信号量** | N | 控制并发数（如连接池） |

```cpp
// 二元信号量实现互斥
std::binary_semaphore mutex_sem(1);

void critical_section() {
    mutex_sem.acquire();  // 进入临界区
    // 临界区代码
    mutex_sem.release();  // 离开临界区
}

// 计数信号量控制并发数
std::counting_semaphore<5> connection_pool(5);  // 最多 5 个连接

void use_connection() {
    connection_pool.acquire();  // 获取连接
    // 使用连接
    connection_pool.release();  // 释放连接
}
```

---

## 4. 条件变量

### 4.1 条件变量原理

**面试问题**：什么是条件变量？如何使用？

**答案**：

**条件变量 (Condition Variable)**：允许线程在特定条件满足时被唤醒。

```cpp
#include <condition_variable>
#include <mutex>
#include <queue>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> data_queue;

// 生产者
void producer(int value) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        data_queue.push(value);
    }
    cv.notify_one();  // 唤醒一个等待线程
}

// 消费者
int consumer() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return !data_queue.empty(); });  // 等待条件满足

    int value = data_queue.front();
    data_queue.pop();
    return value;
}
```

**条件变量使用规范**：

```cpp
// 1. 必须与互斥锁配合使用
std::mutex mtx;
std::condition_variable cv;
bool ready = false;

// 等待方：
std::unique_lock<std::mutex> lock(mtx);
cv.wait(lock, [] { return ready; });  // 释放锁并等待

// 通知方：
{
    std::lock_guard<std::mutex> lock(mtx);
    ready = true;
}
cv.notify_one();  // 或 notify_all()
```

**虚假唤醒 (Spurious Wakeup)**：

```cpp
// 错误写法（可能虚假唤醒）
cv.wait(lock);
// 条件可能不满足！

// 正确写法（带谓词）
cv.wait(lock, [] { return condition; });
// 等价于：
while (!condition) {
    cv.wait(lock);
}
```

---

## 5. 死锁

### 5.1 死锁的四个必要条件

**面试问题**：死锁的四个必要条件是什么？

**答案**：

| 条件 | 说明 |
|------|------|
| **互斥** | 资源不能共享，只能独占 |
| **占有并等待** | 持有资源的同时等待其他资源 |
| **不可抢占** | 资源只能由持有者主动释放 |
| **循环等待** | 存在进程等待资源的环形链 |

```
死锁示例：

线程 A              线程 B
────────            ────────
获取锁 1             获取锁 2
    │                   │
    ▼                   ▼
请求锁 2 ────────────► 请求锁 1
(等待 B)             (等待 A)
    │                   │
    └───── 死锁 ────────┘

┌─────┐            ┌─────┐
│  A  │───等待锁2──►│  B  │
│持有1│            │持有2│
└──▲──┘            └──┬──┘
   │                  │
   └────等待锁1───────┘
```

---

### 5.2 死锁处理策略

**面试问题**：如何预防、避免和检测死锁？

**答案**：

#### 1. 死锁预防（破坏必要条件）

```cpp
// 方法 1：破坏"占有并等待"——一次性获取所有资源
std::scoped_lock lock(mutex1, mutex2);  // C++17

// 方法 2：破坏"循环等待"——按固定顺序获取锁
// 约定：总是先获取 mutex1，再获取 mutex2

// 方法 3：破坏"不可抢占"——使用 try_lock
if (mutex1.try_lock()) {
    if (mutex2.try_lock()) {
        // 成功获取两个锁
    } else {
        mutex1.unlock();  // 获取失败，释放已有锁
    }
}
```

#### 2. 死锁避免（银行家算法）

```cpp
// 银行家算法：检查资源分配是否会导致不安全状态

struct BankerAlgorithm {
    int available[M];      // 可用资源
    int max[N][M];         // 最大需求
    int allocation[N][M];  // 已分配
    int need[N][M];        // 还需要 = max - allocation

    bool isSafe() {
        bool finish[N] = {false};
        int work[M];
        std::copy(available, available + M, work);

        while (true) {
            bool found = false;
            for (int i = 0; i < N; i++) {
                if (!finish[i] && canFinish(i, work)) {
                    // 进程 i 可以完成
                    for (int j = 0; j < M; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                }
            }
            if (!found) break;
        }

        return std::all_of(finish, finish + N, [](bool f) { return f; });
    }

    bool canFinish(int i, int work[]) {
        for (int j = 0; j < M; j++) {
            if (need[i][j] > work[j]) return false;
        }
        return true;
    }
};
```

#### 3. 死锁检测与恢复

```cpp
// 检测：构建资源分配图，检测是否有环
// 恢复策略：
// 1. 终止进程
// 2. 抢占资源
// 3. 回滚

// Linux 工具检测死锁
// 1. gdb: info threads, bt
// 2. valgrind --tool=helgrind
// 3. lockdep (内核)
```

---

### 5.3 避免死锁的编程实践

```cpp
// 1. 使用 std::scoped_lock 同时获取多个锁
std::mutex m1, m2;
{
    std::scoped_lock lock(m1, m2);  // 避免死锁
    // 临界区
}

// 2. 使用 std::lock + std::lock_guard 的 adopt_lock
std::lock(m1, m2);  // 同时获取，内部处理顺序
std::lock_guard<std::mutex> lg1(m1, std::adopt_lock);
std::lock_guard<std::mutex> lg2(m2, std::adopt_lock);

// 3. 超时机制
std::timed_mutex tm;
if (tm.try_lock_for(std::chrono::seconds(1))) {
    // 获取锁成功
    tm.unlock();
} else {
    // 超时，避免永久等待
}

// 4. 层次锁（按固定顺序）
class HierarchicalMutex {
    std::mutex mtx;
    int hierarchy_value;
    static thread_local int this_thread_hierarchy;

public:
    void lock() {
        if (hierarchy_value >= this_thread_hierarchy) {
            throw std::logic_error("违反锁层次");
        }
        mtx.lock();
        this_thread_hierarchy = hierarchy_value;
    }
};
```

---

## 6. 经典同步问题

### 6.1 生产者-消费者问题

**面试问题**：如何实现生产者-消费者模型？

**答案**：

```cpp
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class BoundedQueue {
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable not_empty;
    std::condition_variable not_full;
    size_t max_size;

public:
    BoundedQueue(size_t size) : max_size(size) {}

    void produce(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return queue.size() < max_size; });

        queue.push(item);
        not_empty.notify_one();
    }

    T consume() {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return !queue.empty(); });

        T item = queue.front();
        queue.pop();
        not_full.notify_one();
        return item;
    }
};

// 使用
BoundedQueue<int> buffer(10);

void producer() {
    for (int i = 0; i < 100; i++) {
        buffer.produce(i);
    }
}

void consumer() {
    for (int i = 0; i < 100; i++) {
        int value = buffer.consume();
    }
}
```

---

### 6.2 读者-写者问题

**面试问题**：读者-写者问题有哪些变体？

**答案**：

**变体**：
1. **读者优先**：只要有读者，写者等待
2. **写者优先**：有写者等待时，新读者等待
3. **公平**：按到达顺序

```cpp
// 读者优先实现
class ReaderWriterLock {
    std::mutex mtx;
    std::mutex write_mtx;
    int reader_count = 0;

public:
    void read_lock() {
        mtx.lock();
        reader_count++;
        if (reader_count == 1) {
            write_mtx.lock();  // 第一个读者阻止写者
        }
        mtx.unlock();
    }

    void read_unlock() {
        mtx.lock();
        reader_count--;
        if (reader_count == 0) {
            write_mtx.unlock();  // 最后一个读者允许写者
        }
        mtx.unlock();
    }

    void write_lock() {
        write_mtx.lock();
    }

    void write_unlock() {
        write_mtx.unlock();
    }
};

// 推荐：使用 C++17 std::shared_mutex
std::shared_mutex rw_mtx;

void reader() {
    std::shared_lock lock(rw_mtx);  // 共享锁
    // 读取
}

void writer() {
    std::unique_lock lock(rw_mtx);  // 独占锁
    // 写入
}
```

---

### 6.3 哲学家就餐问题

**面试问题**：如何解决哲学家就餐问题？

**答案**：

```
五个哲学家围坐，每人需要两把叉子才能吃饭。

       [哲学家 0]
    叉子4      叉子0
[哲学家4]          [哲学家1]
    叉子3      叉子1
       [哲学家 3]──叉子2──[哲学家 2]

死锁场景：每人同时拿起左边叉子
```

**解决方案**：

```cpp
#include <mutex>
#include <thread>

std::mutex forks[5];

// 方案 1：破坏循环等待（奇偶编号）
void philosopher(int id) {
    int left = id;
    int right = (id + 1) % 5;

    // 奇数哲学家先拿右边，偶数先拿左边
    if (id % 2 == 0) {
        forks[left].lock();
        forks[right].lock();
    } else {
        forks[right].lock();
        forks[left].lock();
    }

    // 吃饭
    eat();

    forks[left].unlock();
    forks[right].unlock();
}

// 方案 2：使用 std::scoped_lock
void philosopher_safe(int id) {
    int left = id;
    int right = (id + 1) % 5;

    std::scoped_lock lock(forks[left], forks[right]);  // 原子获取两把叉子
    eat();
}

// 方案 3：限制同时进餐人数
std::counting_semaphore<4> seats(4);  // 最多 4 人同时进餐

void philosopher_limited(int id) {
    seats.acquire();  // 获取座位

    int left = id;
    int right = (id + 1) % 5;

    forks[left].lock();
    forks[right].lock();

    eat();

    forks[right].unlock();
    forks[left].unlock();

    seats.release();
}
```

---

## 7. 原子操作与无锁编程

### 7.1 原子操作

**面试问题**：什么是原子操作？C++ 中如何使用？

**答案**：

**原子操作**：不可分割的操作，在执行过程中不会被中断。

```cpp
#include <atomic>

std::atomic<int> counter{0};

void increment() {
    counter++;  // 原子操作
    counter.fetch_add(1);  // 显式原子加
}

// 常用原子操作
std::atomic<int> x{0};

x.load();                    // 原子读
x.store(10);                 // 原子写
x.exchange(20);              // 原子交换，返回旧值
x.compare_exchange_strong(expected, desired);  // CAS

// 内存序
x.load(std::memory_order_acquire);
x.store(10, std::memory_order_release);
x.fetch_add(1, std::memory_order_relaxed);
```

---

### 7.2 CAS 操作

**面试问题**：什么是 CAS？如何实现无锁数据结构？

**答案**：

**CAS (Compare-And-Swap)**：比较并交换，原子地比较内存值与期望值，相等则替换。

```cpp
// CAS 原理
bool compare_and_swap(int* addr, int expected, int new_value) {
    // 以下是原子操作
    if (*addr == expected) {
        *addr = new_value;
        return true;
    }
    return false;
}

// C++ 中使用 CAS
std::atomic<int> x{0};
int expected = 0;
int desired = 1;

// 如果 x == expected，则 x = desired
bool success = x.compare_exchange_strong(expected, desired);
// success = true, x = 1
// 如果失败，expected 被更新为 x 的当前值
```

**无锁栈示例**：

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    std::atomic<Node*> head{nullptr};

public:
    void push(const T& data) {
        Node* new_node = new Node(data);
        new_node->next = head.load();

        // CAS 循环，直到成功
        while (!head.compare_exchange_weak(new_node->next, new_node)) {
            // 失败时 new_node->next 已被更新为新的 head
        }
    }

    bool pop(T& result) {
        Node* old_head = head.load();

        while (old_head &&
               !head.compare_exchange_weak(old_head, old_head->next)) {
            // 失败时 old_head 已被更新
        }

        if (old_head) {
            result = old_head->data;
            delete old_head;  // 注意：这里有 ABA 问题风险
            return true;
        }
        return false;
    }
};
```

---

### 7.3 ABA 问题

**面试问题**：什么是 ABA 问题？如何解决？

**答案**：

**ABA 问题**：值从 A 变为 B 再变回 A，CAS 无法检测到中间变化。

```
时间    线程 1                   线程 2
─────   ──────                  ──────
t1      读取值 A
t2                              修改为 B
t3                              修改回 A
t4      CAS(A→C) 成功           (但实际状态已变化)
```

**解决方案**：

```cpp
// 方案 1：使用版本号/标记
template<typename T>
struct Stamped {
    T value;
    uint64_t stamp;  // 版本号
};

std::atomic<Stamped<int>> data{{0, 0}};

void modify() {
    auto old = data.load();
    Stamped<int> new_val{old.value + 1, old.stamp + 1};
    data.compare_exchange_strong(old, new_val);
}

// 方案 2：使用 std::atomic_shared_ptr (C++20)
// 引用计数自动管理，避免重复使用同一地址

// 方案 3：使用 hazard pointers
// 延迟回收内存，确保不会重用正在使用的地址
```

---

## 8. 面试常见问题

### 8.1 高频问题汇总

**Q1：互斥锁和自旋锁的区别？**
- 互斥锁：获取失败则睡眠，适合临界区长
- 自旋锁：获取失败则忙等，适合临界区短

**Q2：死锁的四个必要条件？**
- 互斥、占有并等待、不可抢占、循环等待

**Q3：如何避免死锁？**
- 按固定顺序获取锁
- 使用 std::scoped_lock
- 超时机制
- 银行家算法

**Q4：什么是条件变量？为什么需要配合互斥锁？**
- 条件变量用于等待特定条件
- 配合互斥锁保护条件检查和修改

**Q5：什么是虚假唤醒？如何处理？**
- 线程可能在条件不满足时被唤醒
- 使用 wait(lock, predicate) 循环检查条件

**Q6：信号量和互斥锁的区别？**
- 互斥锁：二元，只能获取/释放
- 信号量：可以是任意正整数，支持计数

**Q7：什么是 CAS？有什么优缺点？**
- 比较并交换的原子操作
- 优点：无锁，避免上下文切换
- 缺点：ABA 问题，竞争激烈时效率低

---

## 9. 参考资料

- 《C++ Concurrency in Action》
- 《操作系统概念》第 6-8 章
- 《Linux 内核设计与实现》
- cppreference.com 并发库文档
