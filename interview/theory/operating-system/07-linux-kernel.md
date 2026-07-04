# 07 - Linux 内核 (Linux Kernel)

## 简介

Linux 内核是 Linux 操作系统的核心，理解其关键子系统对于系统编程和性能优化非常重要。

**核心子系统**：
- **进程调度**：CFS 完全公平调度器
- **内存管理**：Buddy 系统、Slab 分配器
- **网络栈**：协议栈、Socket 接口
- **内核模块**：动态加载扩展功能

---

## 1. 进程调度器

### 1.1 CFS 调度器

**面试问题**：Linux CFS 调度器的原理是什么？

**答案**：

**CFS (Completely Fair Scheduler)**：完全公平调度器，基于虚拟运行时间实现公平调度。

**核心思想**：
- 每个进程维护虚拟运行时间 (vruntime)
- 优先调度 vruntime 最小的进程
- 使用红黑树管理就绪队列

```
vruntime 计算：

vruntime = 实际运行时间 × (NICE_0_LOAD / 权重)

权重与 nice 值关系：
nice = -20 → 权重 = 88761 (优先级最高)
nice = 0   → 权重 = 1024  (默认)
nice = 19  → 权重 = 15    (优先级最低)

示例：
进程 A: nice = 0,  权重 = 1024, 运行 10ms → vruntime = 10 × 1024/1024 = 10
进程 B: nice = -5, 权重 = 3121, 运行 10ms → vruntime = 10 × 1024/3121 ≈ 3.3
进程 B 的 vruntime 增长慢，获得更多 CPU 时间
```

**红黑树结构**：

```
                    ┌─────────┐
                    │vruntime │
                    │  = 50   │
                    └────┬────┘
              ┌──────────┴──────────┐
        ┌─────┴─────┐         ┌─────┴─────┐
        │vruntime=30│         │vruntime=70│
        └─────┬─────┘         └─────┬─────┘
        ┌─────┴─────┐         ┌─────┴─────┐
   ┌────┴────┐ ┌────┴────┐ ┌────┴────┐ ┌────┴────┐
   │  v=20   │ │  v=40   │ │  v=60   │ │  v=80   │
   └─────────┘ └─────────┘ └─────────┘ └─────────┘
        ↑
   最左节点（vruntime 最小）= 下一个被调度的进程
```

**CFS 特点**：
- O(log n) 插入/删除
- O(1) 选择下一进程（最左节点缓存）
- 自动平衡负载

---

### 1.2 调度类

**面试问题**：Linux 有哪些调度策略？

**答案**：

```c
// 调度策略
#define SCHED_NORMAL    0   // 普通进程（CFS）
#define SCHED_FIFO      1   // 实时，先进先出
#define SCHED_RR        2   // 实时，时间片轮转
#define SCHED_BATCH     3   // 批处理（CFS 变体）
#define SCHED_IDLE      5   // 空闲时才调度
#define SCHED_DEADLINE  6   // 期限调度（EDF）
```

**调度类优先级**：

```
┌─────────────────────────────────────────────────────────┐
│                    调度类层次                           │
├─────────────────────────────────────────────────────────┤
│  stop_sched_class        │ 最高优先级，用于停止 CPU    │
├─────────────────────────────────────────────────────────┤
│  dl_sched_class          │ SCHED_DEADLINE              │
├─────────────────────────────────────────────────────────┤
│  rt_sched_class          │ SCHED_FIFO, SCHED_RR        │
├─────────────────────────────────────────────────────────┤
│  fair_sched_class        │ SCHED_NORMAL, SCHED_BATCH   │
├─────────────────────────────────────────────────────────┤
│  idle_sched_class        │ SCHED_IDLE                  │
└─────────────────────────────────────────────────────────┘

调度器选择：从高到低检查每个类，选择第一个有可运行任务的
```

---

### 1.3 调度相关系统调用

```c
#include <sched.h>

// 设置调度策略和优先级
struct sched_param param;
param.sched_priority = 50;  // 实时优先级 1-99
sched_setscheduler(pid, SCHED_FIFO, &param);

// 设置 nice 值
nice(-5);  // 提高优先级
setpriority(PRIO_PROCESS, pid, -10);

// 绑定 CPU
cpu_set_t mask;
CPU_ZERO(&mask);
CPU_SET(0, &mask);  // 绑定到 CPU 0
sched_setaffinity(pid, sizeof(mask), &mask);

// 主动让出 CPU
sched_yield();
```

---

## 2. 内存分配器

### 2.1 Buddy System

**面试问题**：Buddy 系统是如何工作的？

**答案**：

**Buddy System（伙伴系统）**：按 2^n 页分配物理内存。

```
内存布局（假设总共 16 页）：

Order 4 (16页): [████████████████]
                ↓ 分裂
Order 3 (8页):  [████████] [████████]
                     ↓ 分裂
Order 2 (4页):  [████] [████] [████████]
                  ↓ 分裂
Order 1 (2页):  [██] [██] [████] [████████]
                 ↓ 分裂
Order 0 (1页):  [█] [█] [██] [████] [████████]

分配 3 页：需要 Order 2（4 页）块
释放时：检查伙伴是否空闲，合并成更大的块
```

**伙伴关系**：

```
地址          0    1    2    3    4    5    6    7
Order 0:     [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]
             └─┬─┘    └─┬─┘    └─┬─┘    └─┬─┘
伙伴对:        0-1      2-3      4-5      6-7

Order 1:     [0-1]    [2-3]    [4-5]    [6-7]
             └──┬──┘          └──┬──┘
伙伴对:          0-3              4-7

Order 2:     [0-3]            [4-7]
             └────────┬────────┘
伙伴对:                0-7
```

**API**：

```c
// 分配 2^order 个连续页
struct page *alloc_pages(gfp_t gfp_mask, unsigned int order);

// 释放
void __free_pages(struct page *page, unsigned int order);

// 分配单页
struct page *alloc_page(gfp_t gfp_mask);

// 获取页的虚拟地址
void *page_address(struct page *page);
```

---

### 2.2 Slab 分配器

**面试问题**：什么是 Slab 分配器？解决什么问题？

**答案**：

**问题**：Buddy 系统按页分配，小对象浪费内存。

**Slab 分配器**：为固定大小对象提供高效缓存。

```
┌─────────────────────────────────────────────────────────────────┐
│                         Slab 架构                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   kmem_cache ("task_struct")                                    │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  对象大小: 4KB                                          │  │
│   │  每 Slab 对象数: 8                                      │  │
│   └─────────────────────────────────────────────────────────┘  │
│               │                │                │               │
│               ▼                ▼                ▼               │
│          Slab (Full)      Slab (Partial)    Slab (Empty)        │
│   ┌───────────────┐  ┌───────────────┐  ┌───────────────┐      │
│   │[X][X][X][X]   │  │[X][X][ ][ ]   │  │[ ][ ][ ][ ]   │      │
│   │[X][X][X][X]   │  │[X][ ][ ][ ]   │  │[ ][ ][ ][ ]   │      │
│   └───────────────┘  └───────────────┘  └───────────────┘      │
│                                                                 │
│   X = 已分配对象                                                │
│   [ ] = 空闲对象                                                │
│                                                                 │
│   分配优先级: Partial > Empty > 新分配 Slab                     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Slab 优点**：
1. 对象重用：释放后不立即回收，下次分配直接使用
2. 减少碎片：按对象大小分配
3. 缓存着色：优化 CPU 缓存利用

**API**：

```c
// 创建 kmem_cache
struct kmem_cache *cache = kmem_cache_create(
    "my_cache",           // 名称
    sizeof(struct my_obj), // 对象大小
    0,                     // 对齐
    SLAB_HWCACHE_ALIGN,   // 标志
    NULL                   // 构造函数
);

// 分配对象
struct my_obj *obj = kmem_cache_alloc(cache, GFP_KERNEL);

// 释放对象
kmem_cache_free(cache, obj);

// 销毁缓存
kmem_cache_destroy(cache);
```

---

### 2.3 SLUB 分配器

**面试问题**：SLUB 和 SLAB 有什么区别？

**答案**：

| 特性 | SLAB | SLUB |
|------|------|------|
| **复杂度** | 复杂（三链表） | 简单（单链表） |
| **元数据** | 多 | 少 |
| **调试支持** | 较弱 | 更好 |
| **内存效率** | 较低 | 较高 |
| **默认状态** | 旧内核 | 2.6.23+ 默认 |

```
SLUB 简化结构：

┌─────────────────────────────────────────┐
│            Per-CPU Slab                 │
│  ┌───────────────────────────────────┐  │
│  │  freelist → [obj] → [obj] → NULL  │  │
│  └───────────────────────────────────┘  │
│                                         │
│            Partial Slabs                │
│  ┌───────────────────────────────────┐  │
│  │  page → page → page → NULL        │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘

分配流程：
1. 从 Per-CPU slab 的 freelist 获取
2. 若空，从 partial 列表获取 slab
3. 若无 partial，从 buddy 系统分配新 slab
```

---

### 2.4 kmalloc 与 vmalloc

**面试问题**：kmalloc 和 vmalloc 的区别？

**答案**：

| 特性 | kmalloc | vmalloc |
|------|---------|---------|
| **物理内存** | 连续 | 不需要连续 |
| **虚拟内存** | 连续 | 连续 |
| **最大分配** | 有限（通常 4MB） | 较大 |
| **性能** | 快 | 慢（需要页表映射） |
| **适用场景** | DMA、小块分配 | 大块内存 |

```c
// kmalloc：物理连续，快速
void *ptr = kmalloc(size, GFP_KERNEL);
kfree(ptr);

// kzalloc：分配并清零
void *ptr = kzalloc(size, GFP_KERNEL);

// vmalloc：虚拟连续，可分配大块
void *ptr = vmalloc(large_size);
vfree(ptr);

// kvmalloc：自动选择 kmalloc 或 vmalloc
void *ptr = kvmalloc(size, GFP_KERNEL);
kvfree(ptr);
```

---

## 3. 网络栈

### 3.1 协议栈架构

**面试问题**：Linux 网络栈的结构是怎样的？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                         用户空间                                │
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  应用程序: send(), recv(), socket()                     │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
├─────────────────────────────┼───────────────────────────────────┤
│                             │ 系统调用                          │
│                         内核空间                                │
│                             ▼                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  Socket 层                                              │  │
│   │  struct socket, struct sock                             │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  传输层 (TCP/UDP)                                       │  │
│   │  tcp_sendmsg(), udp_sendmsg()                          │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  网络层 (IP)                                            │  │
│   │  ip_queue_xmit(), ip_local_deliver()                   │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  邻居子系统 (ARP)                                       │  │
│   │  neigh_resolve_output()                                │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  设备无关层                                             │  │
│   │  dev_queue_xmit(), netif_receive_skb()                 │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  设备驱动层                                             │  │
│   │  ndo_start_xmit(), NAPI poll()                         │  │
│   └─────────────────────────────────────────────────────────┘  │
│                             │                                   │
├─────────────────────────────┼───────────────────────────────────┤
│                             │                                   │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │  网卡 (NIC)                                             │  │
│   │  DMA, 中断                                              │  │
│   └─────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

### 3.2 sk_buff 结构

**面试问题**：什么是 sk_buff？

**答案**：

**sk_buff (Socket Buffer)**：网络数据包在内核中的表示。

```c
struct sk_buff {
    // 链表指针
    struct sk_buff *next, *prev;

    // 所属 socket
    struct sock *sk;

    // 网络设备
    struct net_device *dev;

    // 数据指针
    unsigned char *head;   // 缓冲区开始
    unsigned char *data;   // 数据开始
    unsigned char *tail;   // 数据结束
    unsigned char *end;    // 缓冲区结束

    // 协议头指针
    union {
        struct tcphdr *th;
        struct udphdr *uh;
        struct icmphdr *icmph;
    } h;

    union {
        struct iphdr *iph;
        struct ipv6hdr *ipv6h;
    } nh;

    // 长度
    unsigned int len;      // 数据长度
    unsigned int data_len; // 分片数据长度
};
```

```
sk_buff 内存布局：

    head                                             end
      │                                               │
      ▼                                               ▼
      ┌───────────────────────────────────────────────┐
      │ headroom │  数据区域  │ tailroom              │
      └───────────────────────────────────────────────┘
                 ▲           ▲
                 │           │
               data        tail

发送时（添加协议头）：
      ┌───────────────────────────────────────────────┐
      │ IP头 │ TCP头 │  应用数据  │ tailroom          │
      └───────────────────────────────────────────────┘
      ▲                         ▲
    data                      tail

接收时（剥离协议头）：
data 指针向后移动，指向下一层数据
```

---

### 3.3 NAPI (New API)

**面试问题**：什么是 NAPI？为什么需要它？

**答案**：

**问题**：高速网络下，每个数据包触发一次中断，开销巨大。

**NAPI**：结合中断和轮询的收包机制。

```
传统中断模式：
数据包1 → 中断 → 处理
数据包2 → 中断 → 处理
数据包3 → 中断 → 处理
... 大量中断，CPU 被频繁打断

NAPI 模式：
数据包1 → 中断 → 禁用中断，启动轮询
数据包2 → 轮询处理
数据包3 → 轮询处理
... 批量处理
无数据 → 重新启用中断
```

```c
// NAPI 使用示例

// 注册 NAPI
struct napi_struct napi;
netif_napi_add(netdev, &napi, my_poll, NAPI_POLL_WEIGHT);
napi_enable(&napi);

// 中断处理
irqreturn_t my_interrupt(int irq, void *dev_id) {
    // 禁用中断
    disable_irq_nosync(irq);
    // 调度 NAPI 轮询
    napi_schedule(&napi);
    return IRQ_HANDLED;
}

// 轮询函数
int my_poll(struct napi_struct *napi, int budget) {
    int received = 0;

    while (received < budget && has_more_packets()) {
        struct sk_buff *skb = receive_packet();
        netif_receive_skb(skb);
        received++;
    }

    if (received < budget) {
        // 处理完毕，退出轮询模式
        napi_complete(napi);
        enable_irq(irq);
    }

    return received;
}
```

---

### 3.4 零拷贝技术

**面试问题**：Linux 网络栈有哪些零拷贝技术？

**答案**：

| 技术 | 说明 |
|------|------|
| **sendfile** | 文件直接发送到 socket |
| **splice** | 管道间数据移动 |
| **MSG_ZEROCOPY** | 发送时避免拷贝 |
| **io_uring** | 异步 I/O，减少拷贝 |

```c
// sendfile：文件 → socket
ssize_t n = sendfile(sockfd, filefd, NULL, size);

// splice：文件 → 管道 → socket
int pipefd[2];
pipe(pipefd);
splice(filefd, NULL, pipefd[1], NULL, size, SPLICE_F_MOVE);
splice(pipefd[0], NULL, sockfd, NULL, size, SPLICE_F_MOVE);

// MSG_ZEROCOPY（内核 4.14+）
int one = 1;
setsockopt(sockfd, SOL_SOCKET, SO_ZEROCOPY, &one, sizeof(one));
send(sockfd, buf, len, MSG_ZEROCOPY);
```

---

## 4. 内核模块

### 4.1 模块基础

**面试问题**：如何编写和加载内核模块？

**答案**：

```c
// hello.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Hello World Module");

// 模块加载时调用
static int __init hello_init(void) {
    printk(KERN_INFO "Hello, Kernel!\n");
    return 0;  // 0 表示成功
}

// 模块卸载时调用
static void __exit hello_exit(void) {
    printk(KERN_INFO "Goodbye, Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
```

**Makefile**：

```makefile
obj-m += hello.o

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

**模块操作**：

```bash
# 编译
make

# 加载模块
sudo insmod hello.ko

# 查看模块
lsmod | grep hello

# 查看日志
dmesg | tail

# 卸载模块
sudo rmmod hello

# 查看模块信息
modinfo hello.ko
```

---

### 4.2 模块参数

```c
#include <linux/moduleparam.h>

// 模块参数
static int count = 1;
static char *name = "default";

// 声明参数
module_param(count, int, 0644);    // 权限 0644
module_param(name, charp, 0644);

MODULE_PARM_DESC(count, "Repeat count");
MODULE_PARM_DESC(name, "Name string");

static int __init param_init(void) {
    int i;
    for (i = 0; i < count; i++) {
        printk(KERN_INFO "Hello, %s!\n", name);
    }
    return 0;
}
```

```bash
# 加载时传递参数
sudo insmod hello.ko count=5 name="World"
```

---

### 4.3 字符设备驱动

```c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define BUF_LEN 80

static int major;
static char msg[BUF_LEN];
static int msg_len;

// 打开设备
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

// 关闭设备
static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

// 读取设备
static ssize_t device_read(struct file *file, char __user *buffer,
                           size_t len, loff_t *offset) {
    int bytes = min(len, (size_t)(msg_len - *offset));

    if (bytes <= 0) return 0;

    if (copy_to_user(buffer, msg + *offset, bytes))
        return -EFAULT;

    *offset += bytes;
    return bytes;
}

// 写入设备
static ssize_t device_write(struct file *file, const char __user *buffer,
                            size_t len, loff_t *offset) {
    int bytes = min(len, (size_t)BUF_LEN);

    if (copy_from_user(msg, buffer, bytes))
        return -EFAULT;

    msg_len = bytes;
    return bytes;
}

// 文件操作结构
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init chardev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major;
    }
    printk(KERN_INFO "Device registered with major %d\n", major);
    return 0;
}

static void __exit chardev_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");
```

```bash
# 使用设备
sudo insmod chardev.ko
# 创建设备节点
sudo mknod /dev/mychardev c <major> 0
# 写入
echo "Hello" > /dev/mychardev
# 读取
cat /dev/mychardev
```

---

## 5. 内核同步机制

### 5.1 自旋锁

```c
#include <linux/spinlock.h>

spinlock_t my_lock;

// 初始化
spin_lock_init(&my_lock);

// 使用
spin_lock(&my_lock);
// 临界区（不能睡眠！）
spin_unlock(&my_lock);

// 中断上下文版本
spin_lock_irqsave(&my_lock, flags);
// 临界区
spin_unlock_irqrestore(&my_lock, flags);
```

---

### 5.2 互斥锁

```c
#include <linux/mutex.h>

DEFINE_MUTEX(my_mutex);

// 或动态初始化
struct mutex my_mutex;
mutex_init(&my_mutex);

// 使用
mutex_lock(&my_mutex);
// 临界区（可以睡眠）
mutex_unlock(&my_mutex);

// 尝试获取
if (mutex_trylock(&my_mutex)) {
    // 成功
    mutex_unlock(&my_mutex);
}
```

---

### 5.3 RCU (Read-Copy-Update)

**面试问题**：什么是 RCU？

**答案**：

**RCU**：读写分离的同步机制，读者无锁，写者复制后更新。

```c
#include <linux/rcupdate.h>

struct my_data {
    int value;
    struct rcu_head rcu;
};

struct my_data __rcu *global_ptr;

// 读取（无锁）
void reader(void) {
    struct my_data *p;

    rcu_read_lock();
    p = rcu_dereference(global_ptr);
    if (p)
        printk("value = %d\n", p->value);
    rcu_read_unlock();
}

// 更新
void updater(int new_value) {
    struct my_data *old, *new;

    new = kmalloc(sizeof(*new), GFP_KERNEL);
    new->value = new_value;

    old = rcu_dereference_protected(global_ptr,
                                    lockdep_is_held(&update_lock));
    rcu_assign_pointer(global_ptr, new);

    // 等待所有读者完成
    synchronize_rcu();

    // 现在可以安全释放旧数据
    kfree(old);
}
```

**RCU 优点**：
- 读者极快（接近无锁）
- 适合读多写少场景
- 无读者饥饿问题

---

## 6. 面试常见问题

### 6.1 高频问题汇总

**Q1：CFS 调度器的原理？**
- 基于虚拟运行时间 (vruntime)
- 红黑树管理就绪队列
- vruntime 小的进程优先调度

**Q2：Buddy 系统和 Slab 的区别？**
- Buddy：按页分配，用于大块内存
- Slab：按对象分配，用于小对象缓存

**Q3：kmalloc 和 vmalloc 的区别？**
- kmalloc：物理连续，快速
- vmalloc：虚拟连续，可分配大块

**Q4：什么是 NAPI？**
- 结合中断和轮询的收包机制
- 减少高速网络下的中断开销

**Q5：如何编写内核模块？**
- 实现 init 和 exit 函数
- 使用 module_init/module_exit 宏
- 编写 Makefile 编译

**Q6：内核中的锁有哪些？**
- 自旋锁：不能睡眠，适合短临界区
- 互斥锁：可以睡眠，适合长临界区
- 读写锁：读多写少场景
- RCU：读者无锁，写者复制更新

**Q7：什么是 sk_buff？**
- 网络数据包在内核中的表示
- 包含数据指针、协议头、长度等信息

---

## 7. 内核调试

### 7.1 常用调试方法

```bash
# printk 调试
printk(KERN_DEBUG "debug: value = %d\n", value);
printk(KERN_INFO "info: something happened\n");
printk(KERN_WARNING "warning: potential issue\n");
printk(KERN_ERR "error: something wrong\n");

# 查看日志
dmesg
dmesg | tail -n 20
journalctl -k

# 动态调试
echo 'file myfile.c +p' > /sys/kernel/debug/dynamic_debug/control

# 内核追踪
trace-cmd record -e sched_switch
trace-cmd report

# perf 分析
perf top
perf record -g ./program
perf report
```

---

### 7.2 proc 和 sysfs

```bash
# /proc 虚拟文件系统
cat /proc/cpuinfo        # CPU 信息
cat /proc/meminfo        # 内存信息
cat /proc/interrupts     # 中断统计
cat /proc/<pid>/status   # 进程状态
cat /proc/<pid>/maps     # 内存映射

# /sys 设备和驱动信息
ls /sys/class/net/       # 网络设备
cat /sys/block/sda/size  # 磁盘大小
```

---

## 8. 参考资料

- 《深入理解 Linux 内核》
- 《Linux 内核设计与实现》
- 《Linux 设备驱动程序》
- Linux 内核源码：https://github.com/torvalds/linux
- 内核文档：Documentation/
