# 12 - 多线程高级 (Thread Advanced)

## 简介

多线程高级主题涉及原子操作、内存模型、无锁编程等底层机制。理解这些概念对于编写高性能、正确的并发代码至关重要。

**核心特点**：
- **原子操作**：无需锁的线程安全操作
- **内存序**：控制内存访问的可见性和顺序
- **无锁编程**：避免锁的性能开销
- **内存模型**：理解多线程的底层行为

---

## 1. 原子操作 (Atomic Operations)

### 1.1 `std::atomic` 的基本使用？

**问题描述**：什么是原子操作？如何使用？

**原子操作的特点**：
- **不可分割**：操作要么全部完成，要么完全不执行
- **线程安全**：无需额外同步
- **无锁**（通常）：避免互斥锁的开销

**基本用法**：

```cpp
#include <atomic>
#include <thread>
#include <iostream>

// ❌ 非原子操作：需要锁
int non_atomic_counter = 0;
std::mutex mtx;

void increment_with_lock() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++non_atomic_counter;
    }
}

// ✅ 原子操作：无需锁
std::atomic<int> atomic_counter{0};

void increment_atomic() {
    for (int i = 0; i < 100000; ++i) {
        ++atomic_counter;  // 原子自增
        // 或：atomic_counter.fetch_add(1);
    }
}

void atomic_demo() {
    std::thread t1(increment_atomic);
    std::thread t2(increment_atomic);
    t1.join();
    t2.join();

    std::cout << "Atomic counter: " << atomic_counter << "\n";  // 200000
}
```

**原子操作的方法**：

```cpp
void atomic_operations() {
    std::atomic<int> x{10};

    // 读取
    int val = x.load();  // 或：int val = x;

    // 写入
    x.store(20);  // 或：x = 20;

    // 交换
    int old = x.exchange(30);  // 返回旧值，设置新值

    // 比较并交换 (Compare-And-Swap, CAS)
    int expected = 30;
    bool success = x.compare_exchange_weak(expected, 40);
    // 如果 x == expected，设置 x = 40，返回 true
    // 否则，设置 expected = x，返回 false

    // 算术操作（仅整数类型）
    x.fetch_add(5);   // x += 5，返回旧值
    x.fetch_sub(3);   // x -= 3，返回旧值
    x.fetch_and(7);   // x &= 7，返回旧值
    x.fetch_or(8);    // x |= 8，返回旧值
    x.fetch_xor(9);   // x ^= 9，返回旧值

    // 复合操作（返回新值）
    int new_val = ++x;  // 等价于 x.fetch_add(1) + 1
}
```

**支持的类型**：

```cpp
// 整数类型
std::atomic<int> ai;
std::atomic<long> al;
std::atomic<unsigned> au;

// 指针类型
std::atomic<int*> ap;

// bool 类型
std::atomic<bool> ab;

// 自定义类型（需满足平凡可拷贝 trivially copyable）
struct Point {
    int x, y;
};
std::atomic<Point> apt;  // 可能使用锁实现（is_lock_free() 检查）

// 检查是否无锁
std::cout << "int is lock-free: " << std::atomic<int>{}.is_lock_free() << "\n";
std::cout << "Point is lock-free: " << std::atomic<Point>{}.is_lock_free() << "\n";
```

**关键要点**：
- 原子操作保证操作的原子性和内存可见性
- 整数类型通常无锁，自定义类型可能使用锁
- `is_lock_free()` 检查是否真正无锁

---

### 1.2 Compare-And-Swap (CAS) 的使用？

**问题描述**：CAS 是无锁编程的核心操作。

**基本 CAS**：

```cpp
std::atomic<int> value{100};

void cas_demo() {
    int expected = 100;
    int desired = 200;

    // 弱版本：可能虚假失败（性能更好）
    bool success = value.compare_exchange_weak(expected, desired);

    if (success) {
        std::cout << "CAS succeeded, value is now 200\n";
    } else {
        std::cout << "CAS failed, expected was: " << expected << "\n";
    }
}
```

**`compare_exchange_weak` vs `compare_exchange_strong`**：

```cpp
void cas_weak_vs_strong() {
    std::atomic<int> x{10};

    // weak：可能虚假失败（在某些架构上），需要循环
    int expected = 10;
    while (!x.compare_exchange_weak(expected, 20)) {
        // 重试
        expected = 10;  // 重置 expected
    }

    // strong：不会虚假失败，但性能可能稍差
    expected = 20;
    bool success = x.compare_exchange_strong(expected, 30);
}
```

**CAS 循环模式**：

```cpp
// 原子地将值乘以 2
void atomic_multiply(std::atomic<int>& x) {
    int old_val = x.load();
    int new_val;

    do {
        new_val = old_val * 2;
    } while (!x.compare_exchange_weak(old_val, new_val));
    // 循环直到成功：如果 x == old_val，设置 x = new_val
}
```

**实现自旋锁**：

```cpp
class SpinLock {
    std::atomic<bool> flag{false};

public:
    void lock() {
        // 自旋等待，直到成功将 false 改为 true
        while (flag.exchange(true, std::memory_order_acquire)) {
            // 如果已经是 true，继续自旋
        }
    }

    void unlock() {
        flag.store(false, std::memory_order_release);
    }
};

void spinlock_demo() {
    SpinLock lock;
    int counter = 0;

    auto worker = [&]() {
        for (int i = 0; i < 100000; ++i) {
            lock.lock();
            ++counter;
            lock.unlock();
        }
    };

    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();

    std::cout << "Counter: " << counter << "\n";  // 200000
}
```

**关键要点**：
- **CAS** 是无锁算法的基础
- **weak** 版本性能更好，但需要循环处理虚假失败
- **strong** 版本保证不虚假失败
- CAS 常用于实现无锁数据结构

---

## 2. 内存序 (Memory Order)

### 2.1 什么是内存序？

**问题描述**：C++ 内存模型和内存序的作用。

**内存序的六种类型**：

```cpp
namespace std {
    enum memory_order {
        memory_order_relaxed,   // 宽松序：无同步，仅保证原子性
        memory_order_consume,   // 消费序：依赖序（很少使用）
        memory_order_acquire,   // 获取序：读操作
        memory_order_release,   // 释放序：写操作
        memory_order_acq_rel,   // 获取-释放序：读-修改-写操作
        memory_order_seq_cst    // 顺序一致序：默认，最强
    };
}
```

**内存序层次**：

```
memory_order_seq_cst（最强，性能最差）
    ↓
memory_order_acq_rel
    ↓
memory_order_acquire / memory_order_release
    ↓
memory_order_relaxed（最弱，性能最好）
```

**关键要点**：
- **内存序控制内存访问的可见性和顺序**
- **默认是 `seq_cst`**（顺序一致），最安全但性能较差
- **放宽内存序可以提升性能**，但需要小心

---

### 2.2 `memory_order_relaxed` 的使用？

**问题描述**：宽松内存序只保证原子性，不保证顺序。

```cpp
std::atomic<int> x{0}, y{0};
int r1, r2;

// 线程 1
void thread1_relaxed() {
    x.store(1, std::memory_order_relaxed);
    r1 = y.load(std::memory_order_relaxed);
}

// 线程 2
void thread2_relaxed() {
    y.store(1, std::memory_order_relaxed);
    r2 = x.load(std::memory_order_relaxed);
}

// 可能的结果：r1 = 0, r2 = 0（两个线程都没看到对方的写入）
```

**适用场景：计数器**：

```cpp
class Counter {
    std::atomic<int> count{0};

public:
    void increment() {
        // 只需要原子性，不需要顺序保证
        count.fetch_add(1, std::memory_order_relaxed);
    }

    int get() const {
        return count.load(std::memory_order_relaxed);
    }
};
```

**关键要点**：
- **只保证原子性**，不保证其他线程的可见性顺序
- **适用于纯计数器**（不依赖其他变量）
- **性能最好**

---

### 2.3 `memory_order_acquire` 和 `memory_order_release`？

**问题描述**：获取-释放语义建立同步关系。

**同步对（Synchronizes-with）**：

```cpp
std::atomic<bool> ready{false};
int data = 0;

// 生产者线程
void producer() {
    data = 42;  // 1. 写数据
    ready.store(true, std::memory_order_release);  // 2. 释放
    // release 确保：1 happens-before 2
}

// 消费者线程
void consumer() {
    while (!ready.load(std::memory_order_acquire));  // 3. 获取
    assert(data == 42);  // 4. 读数据
    // acquire 确保：3 happens-before 4
    // release-acquire 对确保：2 synchronizes-with 3
    // 因此：1 happens-before 4（data 的写入对消费者可见）
}
```

**内存序的 happens-before 保证**：

- **Release**：之前的所有写入对后续的 acquire 可见
- **Acquire**：之后的所有读取能看到之前的 release 写入

**实现自旋锁（正确版本）**：

```cpp
class SpinLock {
    std::atomic<bool> flag{false};

public:
    void lock() {
        while (flag.exchange(true, std::memory_order_acquire)) {
            // 自旋
        }
        // acquire 确保：锁内的操作不会被重排到 lock() 之前
    }

    void unlock() {
        flag.store(false, std::memory_order_release);
        // release 确保：锁内的操作不会被重排到 unlock() 之后
    }
};
```

**关键要点**：
- **Release-Acquire** 建立线程间的同步
- **Release** 用于写操作，**Acquire** 用于读操作
- 常用于生产者-消费者模式

---

### 2.4 `memory_order_seq_cst` 的使用？

**问题描述**：顺序一致性内存序提供全局一致的顺序。

```cpp
std::atomic<int> x{0}, y{0};
int r1, r2;

// 线程 1
void thread1_seq_cst() {
    x.store(1, std::memory_order_seq_cst);
    r1 = y.load(std::memory_order_seq_cst);
}

// 线程 2
void thread2_seq_cst() {
    y.store(1, std::memory_order_seq_cst);
    r2 = x.load(std::memory_order_seq_cst);
}

// 使用 seq_cst，不可能出现 r1 = 0, r2 = 0
// 必然有一个线程看到另一个的写入
```

**关键要点**：
- **默认内存序**，最容易理解
- **提供全局一致的操作顺序**
- **性能开销最大**，能用更弱的内存序时应避免

---

### 2.5 内存序选择指南

**选择决策树**：

```
需要同步吗？
├─ 否 → relaxed（纯计数器）
└─ 是 → 需要全局顺序吗？
    ├─ 是 → seq_cst（默认，最安全）
    └─ 否 →
        ├─ 写操作 → release
        ├─ 读操作 → acquire
        └─ 读-修改-写 → acq_rel
```

**典型场景**：

```cpp
// 场景 1：纯计数器
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);  // ✅

// 场景 2：标志位 + 数据
std::atomic<bool> ready{false};
int data;

// 生产者
data = 42;
ready.store(true, std::memory_order_release);  // ✅

// 消费者
while (!ready.load(std::memory_order_acquire));  // ✅
use(data);

// 场景 3：复杂同步（不确定时使用）
std::atomic<int> x{0};
x.store(1, std::memory_order_seq_cst);  // ✅ 安全但慢
```

---

## 3. 无锁编程 (Lock-Free Programming)

### 3.1 什么是无锁编程？

**问题描述**：无锁数据结构的特点和优势。

**无锁的三个级别**：

1. **Obstruction-Free**（无阻塞）：没有其他线程时能保证进展
2. **Lock-Free**（无锁）：至少一个线程能保证进展
3. **Wait-Free**（无等待）：所有线程都能保证进展（最难实现）

**无锁 vs 有锁**：

| 特性 | 有锁 | 无锁 |
|------|------|------|
| 死锁风险 | 有 | 无 |
| 优先级反转 | 有 | 无 |
| 性能（低竞争） | 较好 | 很好 |
| 性能（高竞争） | 较差 | 较好 |
| 实现复杂度 | 简单 | 复杂 |
| 调试难度 | 中等 | 困难 |

**关键要点**：
- 无锁不意味着更快，取决于竞争程度
- 无锁避免了死锁和优先级反转
- 实现和调试困难

---

### 3.2 无锁栈 (Lock-Free Stack)？

**问题描述**：如何实现无锁栈？

```cpp
template<typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& data_) : data(data_), next(nullptr) {}
    };

    std::atomic<Node*> head{nullptr};

public:
    void push(const T& data) {
        Node* new_node = new Node(data);
        new_node->next = head.load(std::memory_order_relaxed);

        // CAS 循环：只有当 head 仍是 old_head 时才设置为 new_node
        while (!head.compare_exchange_weak(new_node->next, new_node,
                                           std::memory_order_release,
                                           std::memory_order_relaxed));
    }

    bool pop(T& result) {
        Node* old_head = head.load(std::memory_order_acquire);

        while (old_head && !head.compare_exchange_weak(old_head, old_head->next,
                                                       std::memory_order_acquire,
                                                       std::memory_order_relaxed));

        if (old_head) {
            result = old_head->data;
            delete old_head;  // ⚠️ 注意：这里有 ABA 问题和内存回收问题
            return true;
        }

        return false;
    }
};

void lockfree_stack_demo() {
    LockFreeStack<int> stack;

    // 生产者
    std::thread producer([&]() {
        for (int i = 0; i < 1000; ++i) {
            stack.push(i);
        }
    });

    // 消费者
    std::thread consumer([&]() {
        int value;
        for (int i = 0; i < 1000; ++i) {
            while (!stack.pop(value));
        }
    });

    producer.join();
    consumer.join();
}
```

**关键要点**：
- 使用 CAS 循环确保原子更新
- **内存序选择**：push 使用 release，pop 使用 acquire
- **存在问题**：ABA 问题、内存回收问题

---

### 3.3 ABA 问题？

**问题描述**：CAS 的一个经典陷阱。

**ABA 问题示例**：

```
时间 | 线程1                 | 线程2
-----|----------------------|------------------
t1   | 读取 head = A        |
t2   |                      | pop A
t3   |                      | pop B
t4   |                      | push A（复用地址）
t5   | CAS(A, B) 成功！     |
     | 但 A 已经不是原来的 A！
```

**后果**：
- 线程1 以为栈没变，实际上已经变了
- 可能导致数据丢失或崩溃

**解决方案 1：版本号（Tagged Pointer）**：

```cpp
template<typename T>
class LockFreeStackWithTag {
private:
    struct Node {
        T data;
        Node* next;
    };

    struct TaggedPointer {
        Node* ptr;
        uintptr_t tag;  // 版本号
    };

    std::atomic<TaggedPointer> head{{nullptr, 0}};

public:
    void push(const T& data) {
        Node* new_node = new Node{data, nullptr};
        TaggedPointer old_head = head.load();

        do {
            new_node->next = old_head.ptr;
        } while (!head.compare_exchange_weak(
            old_head, {new_node, old_head.tag + 1}));
        // 每次更新都增加版本号
    }

    bool pop(T& result) {
        TaggedPointer old_head = head.load();

        do {
            if (old_head.ptr == nullptr) {
                return false;
            }
        } while (!head.compare_exchange_weak(
            old_head, {old_head.ptr->next, old_head.tag + 1}));

        result = old_head.ptr->data;
        delete old_head.ptr;
        return true;
    }
};
```

**解决方案 2：危险指针（Hazard Pointers）**：

```cpp
// 复杂，需要跟踪哪些指针正在被使用
// 延迟删除，直到确认没有线程持有该指针
```

**解决方案 3：引用计数**：

```cpp
// 使用 shared_ptr 的原子操作
template<typename T>
class LockFreeStackShared {
private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
    };

    std::atomic<std::shared_ptr<Node>> head;

public:
    void push(const T& data) {
        auto new_node = std::make_shared<Node>();
        new_node->data = data;
        new_node->next = std::atomic_load(&head);

        while (!std::atomic_compare_exchange_weak(&head, &new_node->next, new_node));
    }

    bool pop(T& result) {
        auto old_head = std::atomic_load(&head);

        while (old_head && !std::atomic_compare_exchange_weak(&head, &old_head, old_head->next));

        if (old_head) {
            result = old_head->data;
            return true;
        }

        return false;
    }
};
```

**关键要点**：
- **ABA 问题**：指针地址相同，但内容已变
- **版本号**：最常用的解决方案
- **危险指针/引用计数**：解决内存回收问题

---

### 3.4 无锁队列 (Lock-Free Queue)？

**问题描述**：实现 FIFO 无锁队列。

```cpp
template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node() : next(nullptr) {}
        Node(const T& data_) : data(data_), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();
        head.store(dummy);
        tail.store(dummy);
    }

    void enqueue(const T& data) {
        Node* new_node = new Node(data);
        Node* old_tail;

        while (true) {
            old_tail = tail.load();
            Node* next = old_tail->next.load();

            if (old_tail == tail.load()) {  // 检查是否被其他线程修改
                if (next == nullptr) {
                    // tail 指向最后一个节点
                    if (old_tail->next.compare_exchange_weak(next, new_node)) {
                        break;  // 成功插入
                    }
                } else {
                    // tail 落后，帮助推进
                    tail.compare_exchange_weak(old_tail, next);
                }
            }
        }

        // 尝试推进 tail
        tail.compare_exchange_weak(old_tail, new_node);
    }

    bool dequeue(T& result) {
        while (true) {
            Node* old_head = head.load();
            Node* old_tail = tail.load();
            Node* next = old_head->next.load();

            if (old_head == head.load()) {
                if (old_head == old_tail) {
                    if (next == nullptr) {
                        return false;  // 队列为空
                    }
                    // tail 落后，帮助推进
                    tail.compare_exchange_weak(old_tail, next);
                } else {
                    result = next->data;
                    if (head.compare_exchange_weak(old_head, next)) {
                        delete old_head;  // ⚠️ 仍有内存回收问题
                        return true;
                    }
                }
            }
        }
    }
};
```

**关键要点**：
- 使用 dummy node 简化边界条件
- **双 CAS**：需要同时更新 head 和 tail
- **帮助机制**：线程发现 tail 落后时主动推进
- 比无锁栈更复杂

---

## 4. 内存屏障 (Memory Barrier)

### 4.1 什么是内存屏障？

**问题描述**：内存屏障阻止指令重排。

**四种内存屏障**：

1. **LoadLoad**：Load1; LoadLoad; Load2 → Load2 不会在 Load1 前执行
2. **StoreStore**：Store1; StoreStore; Store2 → Store2 不会在 Store1 前执行
3. **LoadStore**：Load1; LoadStore; Store2 → Store2 不会在 Load1 前执行
4. **StoreLoad**：Store1; StoreLoad; Load2 → Load2 不会在 Store1 前执行（最昂贵）

**C++ 中的内存屏障**：

```cpp
#include <atomic>

void memory_fence_demo() {
    int data = 0;
    std::atomic<bool> ready{false};

    // 生产者
    auto producer = [&]() {
        data = 42;
        std::atomic_thread_fence(std::memory_order_release);  // 释放屏障
        ready.store(true, std::memory_order_relaxed);
    };

    // 消费者
    auto consumer = [&]() {
        while (!ready.load(std::memory_order_relaxed));
        std::atomic_thread_fence(std::memory_order_acquire);  // 获取屏障
        assert(data == 42);
    };
}
```

**内存序 vs 内存屏障**：

```cpp
// 使用内存序（推荐）
ready.store(true, std::memory_order_release);

// 等价于（使用屏障）
std::atomic_thread_fence(std::memory_order_release);
ready.store(true, std::memory_order_relaxed);
```

**关键要点**：
- 内存屏障防止编译器和 CPU 重排指令
- 通常使用内存序而不是显式屏障
- 屏障用于与非原子变量的同步

---

## 5. 性能考虑

### 5.1 原子操作的性能？

**性能对比**（相对时间）：

| 操作 | 相对时间 |
|------|---------|
| 普通变量访问 | 1x |
| `relaxed` 原子操作 | 1-2x |
| `acquire`/`release` | 2-3x |
| `seq_cst` | 3-5x |
| 互斥锁（无竞争） | 10-20x |
| 互斥锁（有竞争） | 100-1000x |

**优化建议**：

```cpp
// ❌ 避免：频繁的 seq_cst
for (int i = 0; i < 1000000; ++i) {
    counter.fetch_add(1, std::memory_order_seq_cst);  // 慢
}

// ✅ 优化：使用 relaxed
for (int i = 0; i < 1000000; ++i) {
    counter.fetch_add(1, std::memory_order_relaxed);  // 快
}

// ✅ 进一步优化：批量累加
int local = 0;
for (int i = 0; i < 1000000; ++i) {
    ++local;  // 无原子操作
}
counter.fetch_add(local, std::memory_order_relaxed);  // 只一次原子操作
```

---

### 5.2 伪共享 (False Sharing)？

**问题描述**：多个线程访问同一缓存行的不同变量。

**伪共享示例**：

```cpp
// ❌ 伪共享：两个变量在同一缓存行
struct BadCounter {
    std::atomic<int> counter1;  // 缓存行起始
    std::atomic<int> counter2;  // 同一缓存行
};

// 线程1 频繁修改 counter1
// 线程2 频繁修改 counter2
// → 缓存行不断失效，性能下降

// ✅ 避免伪共享：填充到不同缓存行
struct alignas(64) GoodCounter {  // 缓存行通常 64 字节
    std::atomic<int> counter1;
    char padding[64 - sizeof(std::atomic<int>)];  // 填充
};

struct GoodCounters {
    GoodCounter c1;  // 独立缓存行
    GoodCounter c2;  // 独立缓存行
};
```

**C++17 解决方案**：

```cpp
#include <new>

struct Counters {
    alignas(std::hardware_destructive_interference_size) std::atomic<int> counter1;
    alignas(std::hardware_destructive_interference_size) std::atomic<int> counter2;
};
```

**关键要点**：
- 伪共享导致缓存行频繁失效
- 使用对齐和填充避免
- C++17 提供 `hardware_destructive_interference_size`

---

## 6. 无锁编程的陷阱

### 6.1 常见陷阱

**陷阱 1：过度使用无锁**

```cpp
// ❌ 错误：简单场景用无锁（复杂且容易出错）
LockFreeQueue<int> queue;

// ✅ 正确：简单场景用锁
std::mutex mtx;
std::queue<int> queue;
```

**陷阱 2：忘记内存序**

```cpp
// ❌ 错误：默认 seq_cst，性能差
counter.fetch_add(1);

// ✅ 正确：明确指定内存序
counter.fetch_add(1, std::memory_order_relaxed);
```

**陷阱 3：ABA 问题**

```cpp
// ❌ 错误：简单 CAS，有 ABA 问题
Node* old = head.load();
// ... 其他线程 pop 再 push 相同地址
head.compare_exchange_weak(old, new_node);  // ❌ 可能出错

// ✅ 正确：使用版本号
```

**陷阱 4：内存泄漏**

```cpp
// ❌ 错误：直接 delete，可能其他线程还在用
if (head.compare_exchange_weak(old, old->next)) {
    delete old;  // ❌ 其他线程可能还持有 old
}

// ✅ 正确：使用引用计数或危险指针
```

---

### 6.2 无锁编程的适用场景

**适合无锁**：
- 高竞争、低延迟场景（如金融交易）
- 需要避免死锁（实时系统）
- 性能瓶颈在锁竞争

**不适合无锁**：
- 低竞争场景（锁足够快）
- 复杂数据结构（实现困难）
- 维护成本高于性能收益

**性能测试建议**：
```cpp
// 必须实际测试，不要臆测
auto start = std::chrono::high_resolution_clock::now();
// ... 测试代码
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
```

---

## 7. 调试技巧

### 7.1 如何调试无锁代码？

**工具**：

1. **ThreadSanitizer (TSan)**：检测数据竞争
```bash
g++ -fsanitize=thread -g program.cpp
```

2. **Helgrind (Valgrind)**：检测同步错误
```bash
valgrind --tool=helgrind ./program
```

3. **压力测试**：
```cpp
// 大量线程、长时间运行
std::vector<std::thread> threads;
for (int i = 0; i < 100; ++i) {
    threads.emplace_back(worker);
}
```

**日志技巧**：

```cpp
// 原子日志，避免竞争
std::atomic<int> log_counter{0};

void log(const std::string& msg) {
    int id = log_counter.fetch_add(1);
    std::cout << "[" << id << "] " << msg << "\n";
}
```

---

## 总结

### 关键概念速查

| 概念 | 说明 |
|------|------|
| **`std::atomic`** | 原子类型，无锁线程安全 |
| **CAS** | Compare-And-Swap，无锁算法核心 |
| **内存序** | 控制内存访问的可见性和顺序 |
| **relaxed** | 只保证原子性，性能最好 |
| **acquire/release** | 建立同步关系 |
| **seq_cst** | 顺序一致，默认且最安全 |
| **ABA 问题** | CAS 的经典陷阱 |
| **无锁编程** | 避免锁，但实现复杂 |
| **伪共享** | 缓存行失效导致性能下降 |

### 关键记忆点

1. **原子操作**：无锁线程安全，但不是总是更快
2. **CAS** 是无锁算法的基础，weak 版本需要循环
3. **内存序**：relaxed < acquire/release < seq_cst
4. **默认是 seq_cst**，性能敏感时考虑放宽
5. **ABA 问题**：使用版本号或引用计数解决
6. **无锁不意味着高性能**，必须实际测试
7. **伪共享**：用对齐避免缓存行竞争
8. **无锁编程困难**：优先考虑锁，确有需要再用无锁

### 进阶方向

- **无锁数据结构**：队列、哈希表、跳表
- **内存回收**：Hazard Pointers、RCU
- **硬件知识**：CPU 缓存、MESI 协议
- **形式化验证**：证明无锁算法正确性
- **高级同步原语**：读写锁、信号量

### 最佳实践

1. **能用锁就用锁**，无锁是最后手段
2. **明确指定内存序**，避免默认 seq_cst
3. **大量测试**，无锁代码容易出错
4. **使用工具**：TSan、Helgrind
5. **考虑维护成本**，无锁代码难以理解和维护
