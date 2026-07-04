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

## 8. 锁机制深入考察 (Lock Mechanisms In-Depth)

### 8.1 锁的分类全景图

**问题描述**：C++ 中有哪些锁？各自的定位是什么？

```
锁的分类
├── 按阻塞策略
│   ├── 互斥锁 (std::mutex)        — 获取失败则阻塞（睡眠）
│   ├── 自旋锁 (SpinLock)          — 获取失败则忙等（循环）
│   └── 混合锁 (Adaptive Mutex)     — 先自旋，超时后阻塞
│
├── 按功能特性
│   ├── 基本互斥锁 (std::mutex)
│   ├── 递归锁 (std::recursive_mutex)      — 允许同一线程多次加锁
│   ├── 定时锁 (std::timed_mutex)          — 支持超时
│   ├── 读写锁 (std::shared_mutex)         — 读共享，写独占
│   └── 递归定时锁 (std::recursive_timed_mutex)
│
├── 按思想策略
│   ├── 悲观锁 — 先加锁再操作（mutex）
│   └── 乐观锁 — 先操作再检测冲突（CAS）
│
└── 按粒度
    ├── 粗粒度锁 — 一把大锁保护所有数据
    ├── 细粒度锁 — 多把锁保护不同数据段
    └── 分段锁   — 按 hash 分桶加锁
```

---

### 8.2 自旋锁 vs 互斥锁：原理与选择

**问题描述**：自旋锁和互斥锁的底层区别？什么时候用哪个？

**互斥锁原理**：

```
线程 A 加锁成功 → 进入临界区
线程 B 加锁失败 → 系统调用 → 进入等待队列（睡眠）
线程 A 解锁 → 唤醒线程 B → 线程 B 被调度 → 加锁成功

开销：用户态 ↔ 内核态切换（~微秒级）
```

**自旋锁原理**：

```
线程 A 加锁成功 → 进入临界区
线程 B 加锁失败 → 循环检查（忙等），不让出 CPU
线程 A 解锁 → 线程 B 立即获得锁

开销：CPU 空转（无上下文切换）
```

**自旋锁实现**：

```cpp
// 基础版
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // 忙等
        }
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// 优化版：减少缓存行争用（TTAS - Test and Test-And-Set）
class SpinLockTTAS {
    std::atomic<bool> flag{false};

public:
    void lock() {
        for (;;) {
            // 第一层：只读检查（不产生缓存失效）
            if (!flag.load(std::memory_order_relaxed)) {
                // 第二层：尝试获取
                if (!flag.exchange(true, std::memory_order_acquire)) {
                    return;  // 获取成功
                }
            }
            // 可选：让出 CPU 时间片
            // std::this_thread::yield();
        }
    }

    void unlock() {
        flag.store(false, std::memory_order_release);
    }
};
```

**为什么 TTAS 更快？**
- 简单自旋锁：每次 `test_and_set` 都要独占缓存行（写操作），导致其他核的缓存失效
- TTAS：先用 `load`（读操作）检查，读操作不会独占缓存行，减少总线流量

**核心对比**：

| 维度 | 互斥锁 (mutex) | 自旋锁 (spinlock) |
|------|----------------|-------------------|
| 等待方式 | 阻塞睡眠 | 忙等循环 |
| 上下文切换 | 有（开销大） | 无 |
| CPU 占用 | 不占用（睡眠） | 持续占用 |
| 适合场景 | 临界区较大、持锁时间长 | 临界区极小、持锁时间 < 上下文切换 |
| 公平性 | 通常公平（FIFO 唤醒） | 不公平（谁先抢到谁得） |
| 多核要求 | 无 | **必须多核**（单核自旋无意义） |

**选择决策**：

```
临界区执行时间 < 上下文切换时间（~1-2μs）？
├─ 是 → 自旋锁（避免切换开销）
└─ 否 → 互斥锁（避免浪费 CPU）

是否在中断上下文 / 内核态？
├─ 是 → 自旋锁（不能睡眠）
└─ 否 → 互斥锁（通常更好）

竞争激烈程度？
├─ 低竞争 → 两者差异不大
└─ 高竞争 → 互斥锁（自旋浪费严重）
```

**面试高频问**：自旋锁在单核 CPU 上能工作吗？
> 能编译运行，但**毫无意义**。单核上，持锁线程被抢占后，自旋线程占满 CPU 时间片，持锁线程反而无法运行解锁，白白浪费 CPU。

---

### 8.3 读写锁 (`std::shared_mutex`)

**问题描述**：读多写少的场景如何优化？

**原理**：

```
读锁（共享锁）：多个读线程可以同时持有
写锁（独占锁）：写线程必须独占，排斥所有读和写

状态转换：
  无锁 → 读锁：✅（可多个读者）
  无锁 → 写锁：✅（独占）
  读锁 → 读锁：✅（读者并发）
  读锁 → 写锁：❌（等待所有读者释放）
  写锁 → 读锁：❌（等待写者释放）
  写锁 → 写锁：❌（等待写者释放）
```

**用法**：

```cpp
#include <shared_mutex>
#include <unordered_map>

class ThreadSafeCache {
    mutable std::shared_mutex mtx_;
    std::unordered_map<std::string, std::string> cache_;

public:
    // 读操作：shared_lock（多线程可并发读）
    std::string get(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(mtx_);
        auto it = cache_.find(key);
        return (it != cache_.end()) ? it->second : "";
    }

    // 写操作：unique_lock（独占）
    void put(const std::string& key, const std::string& value) {
        std::unique_lock<std::shared_mutex> lock(mtx_);
        cache_[key] = value;
    }

    // 读 + 条件写：先读锁，需要时升级为写锁
    void put_if_absent(const std::string& key, const std::string& value) {
        // 先用读锁检查
        {
            std::shared_lock<std::shared_mutex> lock(mtx_);
            if (cache_.count(key)) return;
        }
        // 释放读锁后再加写锁（注意：中间可能有其他线程写入）
        {
            std::unique_lock<std::shared_mutex> lock(mtx_);
            cache_.emplace(key, value);  // emplace 不覆盖已有 key
        }
    }
};
```

**读写锁的陷阱**：

```cpp
// ❌ 陷阱 1：写饥饿
// 如果读请求不断，写线程可能永远拿不到锁
// 解决：某些实现支持"写优先"策略

// ❌ 陷阱 2：锁升级导致死锁
void dangerous() {
    std::shared_lock<std::shared_mutex> read_lock(mtx_);
    // ... 读数据 ...
    // 想升级为写锁？std::shared_mutex 不支持！
    // std::unique_lock<std::shared_mutex> write_lock(mtx_);  // 💀 死锁！
    // 正确做法：释放读锁，再获取写锁
}

// ❌ 陷阱 3：读写比太低时反而更慢
// 读写锁本身比普通 mutex 更重（维护读者计数）
// 只有读操作占比 > 90% 时才值得使用
```

**面试高频问**：读写锁内部如何实现？

> 典型实现：一个 `mutex` + 一个 `condition_variable` + 一个 `int reader_count` + 一个 `bool writer_active`。
> 读加锁时：`reader_count++`；读解锁时：`reader_count--`，如果降到 0 则唤醒等待的写者。
> 写加锁时：等待 `reader_count == 0 && !writer_active`，然后设 `writer_active = true`。

---

### 8.4 递归锁 (`std::recursive_mutex`)

**问题描述**：为什么需要递归锁？什么时候用？为什么要尽量避免？

**问题场景**：

```cpp
std::mutex mtx;

void funcB() {
    std::lock_guard<std::mutex> lock(mtx);  // 💀 死锁！mtx 已被 funcA 持有
    // ...
}

void funcA() {
    std::lock_guard<std::mutex> lock(mtx);
    funcB();  // 同一线程再次加锁
}
```

**递归锁解决**：

```cpp
std::recursive_mutex rmtx;

void funcB() {
    std::lock_guard<std::recursive_mutex> lock(rmtx);  // ✅ 同一线程可重入
    // ...
}

void funcA() {
    std::lock_guard<std::recursive_mutex> lock(rmtx);
    funcB();  // 不会死锁
}
```

**原理**：
- 内部维护一个 `owner_thread_id` 和 `lock_count`
- 同一线程再次加锁时：`lock_count++`
- 解锁时：`lock_count--`，降到 0 时真正释放

**为什么要避免使用递归锁？**

```cpp
// 原因 1：掩盖设计问题
// 需要递归锁通常说明调用关系混乱，应该重构

// 原因 2：性能更差
// 每次加锁要检查线程 ID、维护计数器，比普通 mutex 多约 10-20% 开销

// 原因 3：容易造成逻辑错误
void transfer(Account& from, Account& to, int amount) {
    std::lock_guard<std::recursive_mutex> lock(rmtx);
    from.balance -= amount;
    // 这里如果异常，to 没加上，但 from 已经扣了
    // 递归锁让你以为"安全"，实际上事务完整性没保证
    to.balance += amount;
}

// ✅ 更好的做法：拆分公共接口和内部实现
class SafeClass {
    std::mutex mtx_;

    void do_work_impl() {  // 内部方法：不加锁
        // ... 实际逻辑 ...
    }

public:
    void do_work() {  // 公共方法：加锁
        std::lock_guard<std::mutex> lock(mtx_);
        do_work_impl();
    }

    void do_combined_work() {
        std::lock_guard<std::mutex> lock(mtx_);
        do_work_impl();  // 调用内部无锁版本，不会死锁
    }
};
```

---

### 8.5 定时锁 (`std::timed_mutex`)

**问题描述**：如何避免永远等不到锁？

```cpp
#include <mutex>
#include <chrono>

std::timed_mutex tmtx;

void try_with_timeout() {
    // 尝试加锁，最多等 100ms
    if (tmtx.try_lock_for(std::chrono::milliseconds(100))) {
        // 获取成功
        // ... 临界区 ...
        tmtx.unlock();
    } else {
        // 超时，执行降级逻辑
        std::cout << "Lock timeout, fallback\n";
    }

    // 也可以指定绝对时间
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    if (tmtx.try_lock_until(deadline)) {
        // ...
        tmtx.unlock();
    }
}
```

**适用场景**：
- 避免死锁：超时后放弃，记录日志
- 实时系统：不能无限等待
- 优雅降级：获取不到锁时走备用逻辑

---

### 8.6 锁的底层实现：Futex 机制

**问题描述**：`std::mutex` 在操作系统层面是怎么实现的？

**传统方式的问题**：

```
方案 1：纯用户态自旋 → 浪费 CPU
方案 2：纯内核态信号量 → 每次加锁都要系统调用，即使没有竞争
```

**Futex（Fast Userspace Mutex）—— 两全其美**：

```
核心思想：无竞争时在用户态完成，有竞争时才陷入内核

        ┌─────────────┐
        │  atomic var  │   用户态：一个原子变量
        └──────┬──────┘
               │
    ┌──────────┼──────────┐
    │ 无竞争   │          │ 有竞争
    ▼          │          ▼
  CAS 成功     │      CAS 失败
  直接进入     │      调用 futex(WAIT)
  临界区       │      进入内核等待队列
  （零系统     │      （阻塞睡眠）
   调用）      │
               │
             解锁时：
          ┌────┴────┐
          │ 无等待者 │ 有等待者
          ▼         ▼
        直接返回  futex(WAKE)
        （零系统   唤醒一个线程
         调用）
```

**伪代码**：

```cpp
// 简化的 Futex-based mutex
class FutexMutex {
    // 0: 未锁, 1: 已锁无等待者, 2: 已锁有等待者
    std::atomic<int> state{0};

public:
    void lock() {
        // Fast path：无竞争，CAS 0→1
        int expected = 0;
        if (state.compare_exchange_strong(expected, 1)) {
            return;  // 拿到锁，零系统调用
        }

        // Slow path：有竞争，进入内核等待
        while (state.exchange(2) != 0) {
            // futex_wait(&state, 2);  // 内核：如果 state==2 则睡眠
        }
    }

    void unlock() {
        if (state.exchange(0) == 2) {
            // 有线程在等待，唤醒一个
            // futex_wake(&state, 1);
        }
        // 如果之前是 1（无等待者），直接返回，零系统调用
    }
};
```

**关键要点**：
- **无竞争时**（最常见）：仅一次原子操作，性能接近自旋锁
- **有竞争时**：陷入内核睡眠等待，不浪费 CPU
- Linux 的 `pthread_mutex`、glibc 的 `std::mutex` 底层都基于 Futex
- Windows 上对应机制是 `SRWLock`（Slim Reader/Writer Lock）和 `WaitOnAddress`

**面试高频问**：`std::mutex` 的 `lock()` 是系统调用吗？

> **不一定**。无竞争时只是一个用户态的原子操作（CAS），不涉及系统调用。只有当锁已被其他线程持有时，才会进入内核态（Futex WAIT）。这就是为什么无竞争的 mutex 性能非常好。

---

### 8.7 乐观锁 vs 悲观锁

**问题描述**：两种完全不同的并发控制思想。

**悲观锁（Pessimistic Locking）**：

```
假设冲突一定会发生 → 先加锁再操作

特点：安全但保守，有锁等待开销
```

```cpp
std::mutex mtx;
int balance = 1000;

void withdraw_pessimistic(int amount) {
    std::lock_guard<std::mutex> lock(mtx);  // 先锁
    if (balance >= amount) {
        balance -= amount;  // 再操作
    }
}
```

**乐观锁（Optimistic Locking）**：

```
假设冲突很少发生 → 先操作，提交时检测冲突

特点：无等待，但冲突时需要重试
```

```cpp
std::atomic<int> balance{1000};

void withdraw_optimistic(int amount) {
    int old_val = balance.load();
    int new_val;
    do {
        if (old_val < amount) return;  // 余额不足
        new_val = old_val - amount;
    } while (!balance.compare_exchange_weak(old_val, new_val));
    // CAS 失败说明有其他线程修改了 balance，重试
}
```

**版本号乐观锁（适用于复合数据）**：

```cpp
struct Data {
    int value;
    int version;  // 版本号
};

std::mutex mtx;
Data shared_data{0, 0};

void optimistic_update(int new_value) {
    while (true) {
        // 1. 读取当前版本（无锁）
        int old_version = shared_data.version;
        int old_value = shared_data.value;

        // 2. 计算新值（无锁，可以很耗时）
        int computed = old_value + new_value;

        // 3. 提交时加锁检查版本
        std::lock_guard<std::mutex> lock(mtx);
        if (shared_data.version == old_version) {
            shared_data.value = computed;
            shared_data.version++;
            return;  // 成功
        }
        // 版本变了，重试
    }
}
```

**对比**：

| 维度 | 悲观锁 | 乐观锁 |
|------|--------|--------|
| 冲突假设 | 总会冲突 | 很少冲突 |
| 实现方式 | mutex / lock | CAS / 版本号 |
| 读性能 | 加锁开销 | 无开销 |
| 写冲突时 | 排队等待 | 重试 |
| 适合场景 | 写多、冲突频繁 | 读多写少、冲突罕见 |
| 风险 | 死锁 | 活锁（不断重试） |

---

### 8.8 锁的粒度与分段锁

**问题描述**：如何选择锁的粒度？分段锁怎么实现？

**粒度对比**：

```cpp
// ❌ 粗粒度：一把锁保护整个 map
class CoarseMap {
    std::mutex mtx_;
    std::unordered_map<int, int> map_;

public:
    void put(int k, int v) {
        std::lock_guard<std::mutex> lock(mtx_);  // 所有操作串行
        map_[k] = v;
    }
    int get(int k) {
        std::lock_guard<std::mutex> lock(mtx_);
        return map_[k];
    }
};

// ✅ 分段锁：按 key 的 hash 分桶，不同桶可并发
class StripedMap {
    static constexpr int NUM_STRIPES = 16;

    struct Stripe {
        std::mutex mtx;
        std::unordered_map<int, int> map;
    };

    std::array<Stripe, NUM_STRIPES> stripes_;

    Stripe& get_stripe(int key) {
        return stripes_[std::hash<int>{}(key) % NUM_STRIPES];
    }

public:
    void put(int k, int v) {
        auto& stripe = get_stripe(k);
        std::lock_guard<std::mutex> lock(stripe.mtx);
        stripe.map[k] = v;
    }

    int get(int k) {
        auto& stripe = get_stripe(k);
        std::lock_guard<std::mutex> lock(stripe.mtx);
        return stripe.map[k];
    }
};
// 理论上并发度提升 NUM_STRIPES 倍
```

**粒度选择原则**：

```
锁粒度太粗 → 并发度低（串行化严重）
锁粒度太细 → 锁管理开销大、死锁风险增加

经验法则：
1. 先用粗粒度锁保证正确性
2. 性能测试发现锁竞争是瓶颈
3. 再针对性地细化锁粒度
4. 不要过早优化
```

---

### 8.9 Double-Checked Locking Pattern (DCLP)

**问题描述**：经典的单例模式中的锁优化。

```cpp
// ❌ 版本 1：每次都加锁（太慢）
class Singleton_V1 {
    static Singleton_V1* instance;
    static std::mutex mtx;

public:
    static Singleton_V1* getInstance() {
        std::lock_guard<std::mutex> lock(mtx);  // 每次调用都加锁
        if (!instance) {
            instance = new Singleton_V1();
        }
        return instance;
    }
};

// ⚠️ 版本 2：经典 DCLP（C++11 之前有问题）
class Singleton_V2 {
    static Singleton_V2* instance;
    static std::mutex mtx;

public:
    static Singleton_V2* getInstance() {
        if (!instance) {               // 第一次检查（无锁）
            std::lock_guard<std::mutex> lock(mtx);
            if (!instance) {           // 第二次检查（有锁）
                instance = new Singleton_V2();
                // ⚠️ C++11 之前：编译器可能重排序
                // new 的三步：分配内存 → 构造对象 → 赋值指针
                // 可能变成：分配内存 → 赋值指针 → 构造对象
                // 其他线程可能拿到未构造完成的对象！
            }
        }
        return instance;
    }
};

// ✅ 版本 3：C++11 正确的 DCLP
class Singleton_V3 {
    static std::atomic<Singleton_V3*> instance;
    static std::mutex mtx;

public:
    static Singleton_V3* getInstance() {
        auto* p = instance.load(std::memory_order_acquire);  // acquire 读
        if (!p) {
            std::lock_guard<std::mutex> lock(mtx);
            p = instance.load(std::memory_order_relaxed);
            if (!p) {
                p = new Singleton_V3();
                instance.store(p, std::memory_order_release);  // release 写
            }
        }
        return p;
    }
};

// ✅ 版本 4：最佳方案（Meyers' Singleton）
class Singleton_V4 {
public:
    static Singleton_V4& getInstance() {
        static Singleton_V4 instance;  // C++11 保证线程安全
        return instance;
    }
};
// 编译器自动用类似 DCLP + atomic 的方式实现
// 最简洁、最安全、零额外代码
```

**面试高频问**：为什么 Meyers' Singleton 是线程安全的？

> C++11 标准规定：**局部 static 变量的初始化是线程安全的**（§6.7）。编译器会自动插入同步机制（通常是 DCLP + atomic flag），确保只有一个线程执行初始化，其他线程等待。

---

### 8.10 死锁深入分析

**问题描述**：死锁的成因、检测、预防。

**死锁的四个必要条件**（同时满足才会死锁）：

```
1. 互斥条件：资源一次只能被一个线程持有
2. 持有并等待：线程持有一个锁的同时等待另一个锁
3. 不可剥夺：锁只能由持有者主动释放
4. 循环等待：线程之间形成环形等待链
```

**经典死锁场景**：

```cpp
std::mutex mtxA, mtxB;

void thread1() {
    std::lock_guard<std::mutex> lockA(mtxA);   // 先锁 A
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lockB(mtxB);   // 再锁 B → 等待 thread2 释放 B
}

void thread2() {
    std::lock_guard<std::mutex> lockB(mtxB);   // 先锁 B
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lockA(mtxA);   // 再锁 A → 等待 thread1 释放 A
}
// 💀 死锁！thread1 持有 A 等 B，thread2 持有 B 等 A
```

**预防策略**：

```cpp
// 策略 1：固定加锁顺序（破坏循环等待）
void safe_thread1() {
    std::lock_guard<std::mutex> lockA(mtxA);  // 总是先锁 A
    std::lock_guard<std::mutex> lockB(mtxB);  // 再锁 B
}

void safe_thread2() {
    std::lock_guard<std::mutex> lockA(mtxA);  // 也先锁 A
    std::lock_guard<std::mutex> lockB(mtxB);  // 再锁 B
}

// 策略 2：std::lock 同时锁多个（原子地获取所有锁）
void safe_with_std_lock() {
    std::unique_lock<std::mutex> lockA(mtxA, std::defer_lock);
    std::unique_lock<std::mutex> lockB(mtxB, std::defer_lock);
    std::lock(lockA, lockB);  // 同时锁，内部用 try-and-back-off 避免死锁
}

// 策略 3：C++17 std::scoped_lock（最推荐）
void safe_with_scoped_lock() {
    std::scoped_lock lock(mtxA, mtxB);  // 同时锁，RAII 自动释放
    // ... 临界区 ...
}

// 策略 4：try_lock 超时（破坏持有并等待）
void safe_with_trylock() {
    while (true) {
        std::unique_lock<std::mutex> lockA(mtxA);
        if (mtxB.try_lock()) {
            // 两个锁都拿到了
            // ...
            mtxB.unlock();
            return;
        }
        // 拿不到 B，释放 A，重试（避免死等）
    }
}
```

**层级锁（Hierarchical Lock）——编译期防死锁**：

```cpp
class HierarchicalMutex {
    std::mutex mtx_;
    const int level_;                          // 锁的层级
    int previous_level_ = 0;
    static thread_local int current_level_;    // 当前线程持有的最高层级

public:
    explicit HierarchicalMutex(int level) : level_(level) {}

    void lock() {
        if (level_ >= current_level_) {
            throw std::logic_error("Lock hierarchy violated!");
            // 只能从高层级往低层级锁，强制固定顺序
        }
        mtx_.lock();
        previous_level_ = current_level_;
        current_level_ = level_;
    }

    void unlock() {
        current_level_ = previous_level_;
        mtx_.unlock();
    }
};

thread_local int HierarchicalMutex::current_level_ = INT_MAX;

// 使用：
HierarchicalMutex db_lock(1000);      // 数据库层级最高
HierarchicalMutex table_lock(500);    // 表层级其次
HierarchicalMutex row_lock(100);      // 行层级最低

void safe_operation() {
    db_lock.lock();      // ✅ 1000 < INT_MAX
    table_lock.lock();   // ✅ 500 < 1000
    row_lock.lock();     // ✅ 100 < 500
    // ...
    row_lock.unlock();
    table_lock.unlock();
    db_lock.unlock();
}

void unsafe_operation() {
    row_lock.lock();     // ✅ 100 < INT_MAX
    table_lock.lock();   // ❌ 500 >= 100, 抛异常！
}
```

---

### 8.11 锁的 RAII 包装器对比

**问题描述**：`lock_guard`、`unique_lock`、`scoped_lock` 怎么选？

| 特性 | `lock_guard` | `unique_lock` | `scoped_lock` (C++17) |
|------|-------------|---------------|----------------------|
| 自动加/解锁 | ✅ | ✅ | ✅ |
| 延迟加锁 | ❌ | ✅ `defer_lock` | ❌ |
| 提前解锁 | ❌ | ✅ `unlock()` | ❌ |
| 配合条件变量 | ❌ | ✅ | ❌ |
| 多锁同时锁 | ❌ | 配合 `std::lock` | ✅ 直接支持 |
| 可移动 | ❌ | ✅ | ❌ |
| 开销 | 最小 | 稍大（状态标记） | 最小 |

**选择指南**：

```cpp
// 简单加锁解锁 → lock_guard
{
    std::lock_guard<std::mutex> lock(mtx);
    // ...
}

// 需要灵活控制（延迟/提前/配合条件变量）→ unique_lock
{
    std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
    // ... 一些准备工作 ...
    lock.lock();
    // ...
    lock.unlock();  // 提前释放
    // ... 后续不需要锁的工作 ...
}

// 同时锁多个互斥量 → scoped_lock
{
    std::scoped_lock lock(mtxA, mtxB, mtxC);
    // ...
}
```

---

### 8.12 锁机制常见面试题汇总

**Q1：`std::mutex` 能跨进程使用吗？**
> 不能。`std::mutex` 是进程内的同步原语。跨进程需要用操作系统提供的命名互斥量（Windows: `CreateMutex`，POSIX: `pthread_mutex` + 共享内存 + `PTHREAD_PROCESS_SHARED`）。

**Q2：一个线程能对 `std::mutex` 加锁两次吗？**
> 不能，**未定义行为**（通常死锁）。需要用 `std::recursive_mutex`。

**Q3：`std::mutex` 的 `lock()` 性能开销有多大？**
> 无竞争时：约 10-25ns（仅一个原子 CAS 操作）。有竞争时：微秒到毫秒级（涉及内核态切换和线程调度）。

**Q4：自旋锁适合保护 I/O 操作吗？**
> **绝对不适合**。I/O 操作耗时不可预测（毫秒级），自旋锁会让等待线程白白空转消耗 CPU。应该用互斥锁。

**Q5：`volatile` 能代替锁吗？**
> **不能**。`volatile` 只防止编译器优化（不从寄存器读），不提供原子性和内存序保证。多线程同步必须用 `std::atomic` 或锁。C++ 中 `volatile` 和并发无关。

**Q6：如何检测程序中的死锁？**
> - **运行时**：`gdb` 附加到卡住的进程，`thread apply all bt` 看所有线程调用栈
> - **工具**：Helgrind（Valgrind）、ThreadSanitizer
> - **代码层面**：使用层级锁、固定加锁顺序、`try_lock` 超时

**Q7：条件变量为什么必须配合 `unique_lock` 而不能用 `lock_guard`？**
> 因为 `condition_variable::wait()` 需要**原子地释放锁并进入等待**，被唤醒后再重新加锁。这要求锁可以手动 `unlock/lock`，而 `lock_guard` 不支持。

**Q8：`std::lock` 内部如何避免死锁？**
> 使用 **try-and-back-off** 策略：尝试依次锁所有 mutex，如果某个 `try_lock` 失败，释放已获取的所有锁，换一个顺序重试。不是简单的固定顺序，而是动态调整避免活锁。

**Q9：读写锁一定比互斥锁快吗？**
> 不一定。读写锁内部维护读者计数（原子操作），本身比 mutex 更重。只有**读操作占比很高（>90%）且临界区有一定大小**时，读写锁才有优势。否则普通 mutex 更快。

**Q10：什么是锁护送（Lock Convoy）？**
> 多个线程竞争同一个锁，当持锁线程被抢占（时间片用完），其他线程全部阻塞等待。持锁线程恢复后解锁，所有等待线程被唤醒但只有一个能获得锁，其余再次阻塞。这种"集体唤醒→集体阻塞"的模式严重浪费 CPU。解决方案：减小临界区、使用自旋锁（短临界区）、或使用无锁算法。

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
| **自旋锁** | 忙等循环，适合极短临界区 |
| **读写锁** | 读共享写独占，适合读多写少 |
| **递归锁** | 允许同线程重入，但应尽量避免 |
| **Futex** | 用户态快速互斥，无竞争时零系统调用 |
| **乐观锁/悲观锁** | 两种并发控制思想 |
| **分段锁** | 按 hash 分桶，提升并发度 |
| **DCLP** | 双重检查锁定，单例优化模式 |
| **锁护送** | 集体唤醒→集体阻塞的性能陷阱 |

### 关键记忆点

1. **原子操作**：无锁线程安全，但不是总是更快
2. **CAS** 是无锁算法的基础，weak 版本需要循环
3. **内存序**：relaxed < acquire/release < seq_cst
4. **默认是 seq_cst**，性能敏感时考虑放宽
5. **ABA 问题**：使用版本号或引用计数解决
6. **无锁不意味着高性能**，必须实际测试
7. **伪共享**：用对齐避免缓存行竞争
8. **无锁编程困难**：优先考虑锁，确有需要再用无锁
9. **自旋锁 vs 互斥锁**：临界区 < 上下文切换时间用自旋，否则用互斥
10. **读写锁**：读占比 > 90% 时才值得，否则普通 mutex 更快
11. **Futex**：无竞争时仅一次 CAS，有竞争才陷入内核
12. **锁粒度**：先粗后细，用测试验证瓶颈后再优化
13. **DCLP**：C++11 用 `atomic` 或直接用 Meyers' Singleton
14. **避免递归锁**：拆分公共接口和内部实现才是正解

### 进阶方向

- **无锁数据结构**：队列、哈希表、跳表
- **内存回收**：Hazard Pointers、RCU
- **硬件知识**：CPU 缓存、MESI 协议
- **形式化验证**：证明无锁算法正确性
- **高级同步原语**：读写锁、信号量、屏障

### 最佳实践

1. **能用锁就用锁**，无锁是最后手段
2. **明确指定内存序**，避免默认 seq_cst
3. **大量测试**，无锁代码容易出错
4. **使用工具**：TSan、Helgrind
5. **考虑维护成本**，无锁代码难以理解和维护
6. **优先 `scoped_lock`**（C++17）锁多个互斥量
7. **避免递归锁**，重构代码拆分加锁层和逻辑层
8. **锁粒度不要过早优化**，先正确再快速
