# 06 - 网络编程 (Socket Programming)

## 简介

Socket 是网络编程的基础接口。

---

## 1. TCP Socket 流程

```
服务器                              客户端
  │                                   │
socket()                           socket()
  │                                   │
bind()                                │
  │                                   │
listen()                              │
  │                                   │
accept() ◄─────── 三次握手 ────────── connect()
  │                                   │
recv()/send() ◄─── 数据传输 ───► recv()/send()
  │                                   │
close() ◄──────── 四次挥手 ────────── close()
```

---

## 2. UDP Socket 流程

```
服务器                              客户端
  │                                   │
socket()                           socket()
  │                                   │
bind()                                │
  │                                   │
recvfrom() ◄─────── 数据 ─────────── sendto()
  │                                   │
sendto() ────────► 数据 ──────────► recvfrom()
```

---

## 3. I/O 多路复用

### 3.1 select/poll/epoll 对比

| 特性 | select | poll | epoll |
|------|--------|------|-------|
| fd 数量 | 1024 | 无限 | 无限 |
| 拷贝 | 每次全量 | 每次全量 | 仅注册时 |
| 时间复杂度 | O(n) | O(n) | O(1) |
| 触发模式 | LT | LT | LT/ET |

### 3.2 epoll 示例

```c
int epfd = epoll_create1(0);

struct epoll_event ev;
ev.events = EPOLLIN | EPOLLET;
ev.data.fd = sockfd;
epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

struct epoll_event events[MAX_EVENTS];
while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
        // 处理事件
    }
}
```

---

## 4. Reactor 模式

```
                ┌─────────────────┐
   事件源 ────► │   Demultiplexer │ (epoll)
                │     事件分发器   │
                └────────┬────────┘
                         │
         ┌───────────────┼───────────────┐
         ▼               ▼               ▼
    ┌────────┐      ┌────────┐      ┌────────┐
    │Handler1│      │Handler2│      │Handler3│
    │ (读)   │      │ (写)   │      │ (定时) │
    └────────┘      └────────┘      └────────┘
```

---

## 5. 高并发模型

| 模型 | 特点 |
|------|------|
| 多进程 | 简单，隔离好，开销大 |
| 多线程 | 共享方便，需同步 |
| 单线程 + 多路复用 | 高效，Redis 使用 |
| 多线程 + 多路复用 | 更高性能，Nginx 使用 |

---

## 6. 面试常见问题

**Q1：epoll 为什么高效？**
- 仅注册时拷贝 fd
- 使用红黑树管理
- 就绪链表直接返回

**Q2：LT 和 ET 的区别？**
- LT：有数据就通知
- ET：状态变化时通知一次

**Q3：Reactor 和 Proactor 区别？**
- Reactor：I/O 就绪通知，应用执行 I/O
- Proactor：I/O 完成通知，内核执行 I/O
