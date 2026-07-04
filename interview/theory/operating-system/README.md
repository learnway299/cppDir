# Operating System Knowledge

操作系统知识 - 面试必备的 10 个主题。

---

## 知识体系

### 基础层 (Basic) - 3 个主题

操作系统核心概念，必须掌握。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [进程管理](01-process.md) | 进程/线程/协程、PCB、状态转换、调度算法 |
| 02 | [内存管理](02-memory.md) | 虚拟内存、分页/分段、页面置换、TLB |
| 03 | [文件系统](03-filesystem.md) | inode、文件描述符、VFS、缓存机制 |

### 进阶层 (Intermediate) - 2 个主题

深入操作系统机制。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 04 | [I/O 系统](04-io.md) | 同步/异步、阻塞/非阻塞、I/O 模型、epoll |
| 05 | [进程同步](05-synchronization.md) | 互斥锁、信号量、死锁、经典同步问题 |

### 深度层 (Advanced) - 5 个主题

操作系统内核与底层原理。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 06 | [系统调用](06-syscall.md) | 用户态/内核态、中断、系统调用过程 |
| 07 | [Linux 内核](07-linux-kernel.md) | CFS 调度器、slab 分配器、网络栈 |
| 08 | [CEF 容器](08-cef-container.md) | 多进程架构、IPC、OSR、JS/C++ 交互 |
| 09 | [IPC 与 RPC](09-ipc-rpc.md) | 进程间通信、远程调用、Thrift/gRPC 框架 |
| 10 | [沙箱安全模型](10-sandbox.md) | 隔离原理、Namespace、seccomp、容器安全 |

---

## 面试重点

### 必问话题

1. **进程与线程**
   - 进程和线程的区别
   - 进程间通信方式（IPC）
   - 线程同步机制

5. **IPC 与 RPC**
   - 管道、消息队列、共享内存对比
   - Unix Domain Socket 与文件描述符传递
   - RPC 框架设计（Thrift/gRPC）

2. **内存管理**
   - 虚拟内存原理
   - 页面置换算法（LRU、FIFO、Clock）
   - 内存泄漏与检测

3. **I/O 模型**
   - 5 种 I/O 模型
   - select/poll/epoll 区别
   - 同步异步、阻塞非阻塞

4. **死锁**
   - 死锁的四个必要条件
   - 死锁预防、避免、检测
   - 银行家算法

6. **沙箱安全**
   - 沙箱隔离原理
   - Linux Namespace 与 cgroups
   - seccomp 系统调用过滤
   - 浏览器沙箱架构

### 加分项

1. Linux 内核调度器（CFS）
2. 内存分配器（slab/buddy）
3. 文件系统原理（ext4、日志）
4. 零拷贝技术
5. 用户态与内核态切换
6. CEF/Chromium 多进程架构
7. 浏览器渲染原理与 OSR
8. RPC 超时/重试/熔断机制
9. 服务发现与负载均衡
10. 容器隔离与 Docker 安全
11. Windows AppContainer 机制

---

## 文件列表

```
operating-system/
├── README.md              # 本文件
├── 01-process.md          # 进程管理
├── 02-memory.md           # 内存管理
├── 03-filesystem.md       # 文件系统
├── 04-io.md               # I/O 系统
├── 05-synchronization.md  # 进程同步
├── 06-syscall.md          # 系统调用
├── 07-linux-kernel.md     # Linux 内核
├── 08-cef-container.md    # CEF 容器
├── 09-ipc-rpc.md          # IPC 与 RPC
└── 10-sandbox.md          # 沙箱安全模型
```
