# IPC 与 RPC 深入解析

进程间通信（IPC）和远程过程调用（RPC）是分布式系统的基础。

---

## 1. IPC 进 程间通信

### 1.1 为什么需要 IPC

```
┌─────────────────────────────────────────────────────────────┐
│                      进程隔离性                               │
├─────────────────────────────────────────────────────────────┤
│  进程 A                          进程 B                      │
│  ┌─────────────┐                ┌─────────────┐             │
│  │ 虚拟地址空间 │                │ 虚拟地址空间 │             │
│  │ 0x00000000  │                │ 0x00000000  │             │
│  │     ↓       │                │     ↓       │             │
│  │ 0xFFFFFFFF  │                │ 0xFFFFFFFF  │             │
│  └─────────────┘                └─────────────┘             │
│        ↓                              ↓                     │
│  ┌─────────────┐                ┌─────────────┐             │
│  │  物理内存A   │                │  物理内存B   │             │
│  └─────────────┘                └─────────────┘             │
│                                                             │
│  进程间内存完全隔离，无法直接访问对方数据                      │
│  → 需要 IPC 机制来实现数据交换                               │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 IPC 机制全景

```
┌─────────────────────────────────────────────────────────────┐
│                     IPC 机制分类                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                 基于数据传输                          │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐             │   │
│  │  │   管道   │  │消息队列  │  │  Socket │             │   │
│  │  │  Pipe   │  │  MsgQ   │  │         │             │   │
│  │  └─────────┘  └─────────┘  └─────────┘             │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                 基于共享内存                          │   │
│  │  ┌─────────────────┐  ┌─────────────────┐           │   │
│  │  │    共享内存       │  │   内存映射文件    │           │   │
│  │  │  Shared Memory  │  │      mmap       │           │   │
│  │  └─────────────────┘  └─────────────────┘           │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                 基于信号通知                          │   │
│  │  ┌─────────┐  ┌─────────┐                           │   │
│  │  │  信号   │  │ 信号量   │                           │   │
│  │  │ Signal │  │Semaphore│                           │   │
│  │  └─────────┘  └─────────┘                           │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 管道 (Pipe)

### 2.1 匿名管道

```
┌─────────────────────────────────────────────────────────────┐
│                      匿名管道原理                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   父进程                          子进程                     │
│  ┌─────────┐                    ┌─────────┐                │
│  │ fd[1]   │─── 写入 ───→       │         │                │
│  │ (写端)  │     ┌─────────────┐│ fd[0]   │                │
│  │         │     │  内核缓冲区  ││ (读端)  │                │
│  │ fd[0]   │     │  (64KB)     ││         │                │
│  │ (关闭)  │     └─────────────┘│         │←── 读取        │
│  └─────────┘                    └─────────┘                │
│                                                             │
│  特点：                                                     │
│  • 半双工（单向通信）                                        │
│  • 仅限父子/兄弟进程                                         │
│  • 基于文件描述符                                            │
│  • 生命周期随进程                                            │
└─────────────────────────────────────────────────────────────┘
```

**代码示例：**

```cpp
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
    int pipefd[2];  // pipefd[0]: 读端, pipefd[1]: 写端

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程：读取
        close(pipefd[1]);  // 关闭写端

        char buffer[256];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << "Child received: " << buffer << std::endl;
        }

        close(pipefd[0]);
    } else {
        // 父进程：写入
        close(pipefd[0]);  // 关闭读端

        const char* msg = "Hello from parent!";
        write(pipefd[1], msg, strlen(msg));

        close(pipefd[1]);
        wait(nullptr);
    }

    return 0;
}
```

### 2.2 命名管道 (FIFO)

```
┌─────────────────────────────────────────────────────────────┐
│                      命名管道原理                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   进程 A                          进程 B                     │
│  ┌─────────┐                    ┌─────────┐                │
│  │  写入   │                    │  读取   │                │
│  └────┬────┘                    └────┬────┘                │
│       │                              │                      │
│       ↓                              ↓                      │
│  ┌─────────────────────────────────────────┐               │
│  │        /tmp/myfifo (文件系统路径)        │               │
│  │           ┌──────────────┐              │               │
│  │           │   内核缓冲区   │              │               │
│  │           └──────────────┘              │               │
│  └─────────────────────────────────────────┘               │
│                                                             │
│  特点：                                                     │
│  • 有文件系统路径（可见文件）                                 │
│  • 无亲缘关系进程也可通信                                    │
│  • 仍然是半双工                                              │
│  • 阻塞式读写                                                │
└─────────────────────────────────────────────────────────────┘
```

**代码示例：**

```cpp
// 创建 FIFO
#include <sys/stat.h>
#include <fcntl.h>

// 写进程
int main() {
    const char* fifo_path = "/tmp/myfifo";

    // 创建命名管道
    mkfifo(fifo_path, 0666);

    // 打开并写入
    int fd = open(fifo_path, O_WRONLY);
    const char* msg = "Hello via FIFO!";
    write(fd, msg, strlen(msg));
    close(fd);

    return 0;
}

// 读进程
int main() {
    const char* fifo_path = "/tmp/myfifo";

    int fd = open(fifo_path, O_RDONLY);
    char buffer[256];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    std::cout << "Received: " << buffer << std::endl;
    close(fd);

    return 0;
}
```

---

## 3. 消息队列 (Message Queue)

### 3.1 消息队列原理

```
┌─────────────────────────────────────────────────────────────┐
│                     消息队列架构                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   发送者                                      接收者         │
│  ┌──────┐                                   ┌──────┐       │
│  │进程 A│                                   │进程 B│       │
│  └──┬───┘                                   └──┬───┘       │
│     │ msgsnd()                                 │ msgrcv()  │
│     ↓                                          ↑           │
│  ┌─────────────────────────────────────────────────┐       │
│  │                  消息队列（内核）                 │       │
│  │  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐  │       │
│  │  │msg1 │→ │msg2 │→ │msg3 │→ │msg4 │→ │msg5 │  │       │
│  │  │type1│  │type2│  │type1│  │type3│  │type2│  │       │
│  │  └─────┘  └─────┘  └─────┘  └─────┘  └─────┘  │       │
│  └─────────────────────────────────────────────────┘       │
│                                                             │
│  特点：                                                     │
│  • 消息有类型，可选择性接收                                  │
│  • 消息有边界（不像管道是字节流）                             │
│  • 生命周期独立于进程                                        │
│  • 可一对多、多对一                                          │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 System V 消息队列

```cpp
#include <sys/msg.h>
#include <cstring>

// 消息结构
struct Message {
    long mtype;        // 消息类型（必须 > 0）
    char mtext[256];   // 消息内容
};

// 发送端
int main() {
    // 创建/获取消息队列
    key_t key = ftok("/tmp", 'A');
    int msgid = msgget(key, IPC_CREAT | 0666);

    Message msg;
    msg.mtype = 1;  // 类型 1
    strcpy(msg.mtext, "Hello Message Queue!");

    // 发送消息
    msgsnd(msgid, &msg, sizeof(msg.mtext), 0);

    return 0;
}

// 接收端
int main() {
    key_t key = ftok("/tmp", 'A');
    int msgid = msgget(key, 0666);

    Message msg;

    // 接收类型为 1 的消息
    msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
    std::cout << "Received: " << msg.mtext << std::endl;

    // 删除消息队列
    msgctl(msgid, IPC_RMID, nullptr);

    return 0;
}
```

### 3.3 POSIX 消息队列

```cpp
#include <mqueue.h>
#include <fcntl.h>

int main() {
    // POSIX 消息队列属性
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;     // 最大消息数
    attr.mq_msgsize = 256;   // 最大消息大小
    attr.mq_curmsgs = 0;

    // 创建消息队列
    mqd_t mq = mq_open("/myqueue", O_CREAT | O_RDWR, 0666, &attr);

    // 发送
    const char* msg = "Hello POSIX MQ!";
    mq_send(mq, msg, strlen(msg) + 1, 0);

    // 接收
    char buffer[256];
    unsigned int prio;
    mq_receive(mq, buffer, sizeof(buffer), &prio);

    mq_close(mq);
    mq_unlink("/myqueue");

    return 0;
}
```

---

## 4. 共享内存 (Shared Memory)

### 4.1 共享内存原理

```
┌─────────────────────────────────────────────────────────────┐
│                     共享内存原理                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   进程 A                          进程 B                     │
│  ┌─────────────────┐            ┌─────────────────┐        │
│  │   虚拟地址空间    │            │   虚拟地址空间    │        │
│  │                 │            │                 │        │
│  │  ┌───────────┐  │            │  ┌───────────┐  │        │
│  │  │共享内存映射│  │            │  │共享内存映射│  │        │
│  │  │0x7fff0000 │  │            │  │0x7f000000 │  │        │
│  │  └─────┬─────┘  │            │  └─────┬─────┘  │        │
│  └────────┼────────┘            └────────┼────────┘        │
│           │                              │                  │
│           └──────────┬───────────────────┘                  │
│                      ↓                                      │
│           ┌──────────────────┐                              │
│           │     物理内存      │                              │
│           │   共享内存区域    │                              │
│           │  (同一块物理页)   │                              │
│           └──────────────────┘                              │
│                                                             │
│  特点：                                                     │
│  • 最快的 IPC 方式（无数据复制）                              │
│  • 需要同步机制（互斥锁/信号量）                              │
│  • 进程各自映射到不同虚拟地址                                 │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 POSIX 共享内存

```cpp
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

// 共享数据结构
struct SharedData {
    int counter;
    char message[256];
    pthread_mutex_t mutex;  // 进程间互斥锁
};

// 写进程
int main() {
    // 创建共享内存对象
    int fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);

    // 设置大小
    ftruncate(fd, sizeof(SharedData));

    // 映射到进程地址空间
    SharedData* data = (SharedData*)mmap(
        nullptr, sizeof(SharedData),
        PROT_READ | PROT_WRITE,
        MAP_SHARED, fd, 0
    );

    // 初始化互斥锁（进程共享）
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);

    // 写入数据
    pthread_mutex_lock(&data->mutex);
    data->counter = 42;
    strcpy(data->message, "Hello Shared Memory!");
    pthread_mutex_unlock(&data->mutex);

    // 解除映射
    munmap(data, sizeof(SharedData));
    close(fd);

    return 0;
}

// 读进程
int main() {
    int fd = shm_open("/myshm", O_RDWR, 0666);

    SharedData* data = (SharedData*)mmap(
        nullptr, sizeof(SharedData),
        PROT_READ | PROT_WRITE,
        MAP_SHARED, fd, 0
    );

    // 读取数据
    pthread_mutex_lock(&data->mutex);
    std::cout << "Counter: " << data->counter << std::endl;
    std::cout << "Message: " << data->message << std::endl;
    pthread_mutex_unlock(&data->mutex);

    munmap(data, sizeof(SharedData));
    close(fd);

    // 删除共享内存
    shm_unlink("/myshm");

    return 0;
}
```

### 4.3 内存映射文件 (mmap)

```cpp
#include <sys/mman.h>
#include <fcntl.h>

int main() {
    // 打开文件
    int fd = open("data.bin", O_RDWR | O_CREAT, 0666);
    ftruncate(fd, 4096);

    // 映射文件到内存
    void* addr = mmap(
        nullptr,        // 系统选择地址
        4096,           // 映射大小
        PROT_READ | PROT_WRITE,  // 读写权限
        MAP_SHARED,     // 共享映射（写回文件）
        fd,             // 文件描述符
        0               // 偏移量
    );

    // 直接操作内存 = 操作文件
    char* data = (char*)addr;
    strcpy(data, "Hello mmap!");

    // 同步到磁盘
    msync(addr, 4096, MS_SYNC);

    // 解除映射
    munmap(addr, 4096);
    close(fd);

    return 0;
}
```

---

## 5. 信号 (Signal)

### 5.1 信号机制

```
┌─────────────────────────────────────────────────────────────┐
│                       信号处理流程                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────┐     ┌──────────┐     ┌──────────────────┐    │
│  │  信号源   │ ──→ │   内核   │ ──→ │     目标进程      │    │
│  └──────────┘     └──────────┘     └──────────────────┘    │
│                                                             │
│  信号源：                                                   │
│  • 键盘（Ctrl+C → SIGINT）                                  │
│  • 硬件异常（除零 → SIGFPE）                                 │
│  • kill 命令/系统调用                                       │
│  • 软件条件（管道破裂 → SIGPIPE）                            │
│                                                             │
│  信号处理方式：                                              │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  1. 默认处理（终止/忽略/停止/继续）                     │   │
│  │  2. 忽略信号（SIG_IGN）                               │   │
│  │  3. 捕获信号（自定义处理函数）                          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  不可捕获的信号：SIGKILL(9), SIGSTOP(19)                    │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 常用信号

| 信号 | 编号 | 默认行为 | 说明 |
|------|------|----------|------|
| SIGINT | 2 | 终止 | Ctrl+C |
| SIGQUIT | 3 | 终止+core | Ctrl+\ |
| SIGKILL | 9 | 终止 | 强制杀死（不可捕获） |
| SIGSEGV | 11 | 终止+core | 段错误 |
| SIGPIPE | 13 | 终止 | 管道破裂 |
| SIGTERM | 15 | 终止 | 正常终止请求 |
| SIGCHLD | 17 | 忽略 | 子进程状态改变 |
| SIGSTOP | 19 | 停止 | 暂停进程（不可捕获） |
| SIGUSR1/2 | 10/12 | 终止 | 用户自定义 |

### 5.3 信号处理代码

```cpp
#include <signal.h>
#include <unistd.h>

// 信号处理函数
void signal_handler(int signum) {
    switch (signum) {
        case SIGINT:
            std::cout << "\nReceived SIGINT (Ctrl+C)" << std::endl;
            // 清理资源...
            exit(0);
            break;
        case SIGUSR1:
            std::cout << "Received SIGUSR1" << std::endl;
            break;
    }
}

int main() {
    // 方式 1: signal() - 简单但不可靠
    signal(SIGINT, signal_handler);

    // 方式 2: sigaction() - 推荐使用
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // 被中断的系统调用自动重启

    sigaction(SIGUSR1, &sa, nullptr);

    // 发送信号给自己
    kill(getpid(), SIGUSR1);

    // 等待信号
    while (true) {
        pause();  // 挂起等待信号
    }

    return 0;
}
```

---

## 6. Socket IPC

### 6.1 Unix Domain Socket

```
┌─────────────────────────────────────────────────────────────┐
│                   Unix Domain Socket                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   进程 A (客户端)                 进程 B (服务端)            │
│  ┌─────────────┐               ┌─────────────┐             │
│  │   socket()  │               │   socket()  │             │
│  │   connect() │               │   bind()    │             │
│  └──────┬──────┘               │   listen()  │             │
│         │                      │   accept()  │             │
│         │                      └──────┬──────┘             │
│         │                             │                     │
│         └─────────────────────────────┘                     │
│                       ↓                                     │
│         ┌───────────────────────────┐                       │
│         │  /tmp/app.sock (文件路径) │                       │
│         │     内核缓冲区直接通信     │                       │
│         └───────────────────────────┘                       │
│                                                             │
│  优势（对比 TCP）：                                          │
│  • 无网络协议栈开销                                          │
│  • 无 TCP/IP 头部                                           │
│  • 支持传递文件描述符                                        │
│  • 支持凭证传递（UID/GID）                                   │
└─────────────────────────────────────────────────────────────┘
```

**代码示例：**

```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// 服务端
int main() {
    const char* socket_path = "/tmp/app.sock";

    // 创建 Unix Domain Socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    unlink(socket_path);  // 删除旧文件
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    // 接受连接
    int client_fd = accept(server_fd, nullptr, nullptr);

    char buffer[256];
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "Received: " << buffer << std::endl;

    close(client_fd);
    close(server_fd);
    unlink(socket_path);

    return 0;
}

// 客户端
int main() {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/app.sock", sizeof(addr.sun_path) - 1);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    const char* msg = "Hello Unix Socket!";
    write(sock, msg, strlen(msg));

    close(sock);
    return 0;
}
```

### 6.2 传递文件描述符

```cpp
// Unix Domain Socket 独有能力：进程间传递文件描述符

// 发送文件描述符
void send_fd(int socket, int fd_to_send) {
    struct msghdr msg = {0};
    struct cmsghdr* cmsg;
    char buf[CMSG_SPACE(sizeof(int))];

    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *((int*)CMSG_DATA(cmsg)) = fd_to_send;

    char dummy = 'F';
    struct iovec iov = { &dummy, 1 };
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sendmsg(socket, &msg, 0);
}

// 接收文件描述符
int recv_fd(int socket) {
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(int))];

    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    char dummy;
    struct iovec iov = { &dummy, 1 };
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    recvmsg(socket, &msg, 0);

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    return *((int*)CMSG_DATA(cmsg));
}
```

---

## 7. IPC 性能对比

```
┌─────────────────────────────────────────────────────────────┐
│                     IPC 性能对比                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  机制          数据复制次数    性能      适用场景            │
│  ─────────────────────────────────────────────────────────  │
│  共享内存        0 次        ★★★★★   大数据、高频通信      │
│  mmap           0 次        ★★★★★   文件共享、持久化      │
│  Unix Socket    2 次        ★★★★☆   通用 IPC、fd 传递    │
│  管道           2 次        ★★★☆☆   父子进程、简单通信    │
│  消息队列       2 次        ★★★☆☆   消息分类、解耦        │
│  信号           0 次        ★★☆☆☆   事件通知（无数据）    │
│  TCP Socket    4+ 次        ★★☆☆☆   跨机器通信           │
│                                                             │
│  数据复制流程（以管道为例）：                                 │
│  ┌─────────┐  复制1  ┌─────────┐  复制2  ┌─────────┐       │
│  │用户空间A│ ──────→ │ 内核缓冲 │ ──────→ │用户空间B│       │
│  └─────────┘         └─────────┘         └─────────┘       │
│                                                             │
│  共享内存无复制：                                            │
│  ┌─────────┐                             ┌─────────┐       │
│  │用户空间A│ ─────→ 同一物理页 ←───────── │用户空间B│       │
│  └─────────┘                             └─────────┘       │
└─────────────────────────────────────────────────────────────┘
```

---

## 8. RPC 远程过程调用

### 8.1 RPC 基本原理

```
┌─────────────────────────────────────────────────────────────┐
│                      RPC 调用流程                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   客户端                                     服务端          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  1. 调用本地存根                                     │   │
│  │     result = rpc.add(1, 2)                          │   │
│  │           ↓                                         │   │
│  │  2. Client Stub                                     │   │
│  │     - 序列化参数                                    │   │
│  │     - 打包请求                                      │   │
│  │           ↓                                         │   │
│  │  3. 网络传输 ─────────────────────────────────────→ │   │
│  │                                                     │   │
│  │  ←───────────────────────────────────────────────── │   │
│  │           ↓                     4. Server Stub      │   │
│  │  7. 反序列化结果                   - 反序列化参数    │   │
│  │           ↓                       - 调用真实方法    │   │
│  │  8. 返回结果                            ↓           │   │
│  │     return 3                    5. 执行业务逻辑     │   │
│  │                                   add(1, 2) = 3     │   │
│  │                                       ↓             │   │
│  │                                 6. 序列化结果       │   │
│  │                                    网络返回         │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  RPC = 远程调用 + 序列化 + 网络传输 + 服务发现               │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 RPC vs REST

```
┌─────────────────────────────────────────────────────────────┐
│                    RPC vs REST 对比                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  维度          RPC                    REST                  │
│  ──────────────────────────────────────────────────────     │
│  抽象层次      方法调用               资源操作              │
│  协议格式      二进制（高效）          JSON/XML（可读）      │
│  传输协议      自定义/HTTP/2          HTTP/1.1              │
│  强类型        是（IDL 定义）          否                    │
│  性能          高                     中等                  │
│  浏览器支持    需要 gRPC-Web          原生支持              │
│  调试          需要专用工具            curl/浏览器          │
│                                                             │
│  RPC 调用风格：                                              │
│  client.createUser(name: "张三", age: 25)                   │
│  client.getUser(id: 123)                                    │
│  client.updateUser(id: 123, name: "李四")                   │
│                                                             │
│  REST 调用风格：                                             │
│  POST   /users         { name: "张三", age: 25 }            │
│  GET    /users/123                                          │
│  PUT    /users/123     { name: "李四" }                     │
│                                                             │
│  选择建议：                                                  │
│  • 内部微服务通信 → RPC（高性能、强类型）                     │
│  • 公开 API → REST（通用、易用）                             │
│  • 实时双向通信 → gRPC streaming                            │
└─────────────────────────────────────────────────────────────┘
```

### 8.3 序列化协议对比

```
┌─────────────────────────────────────────────────────────────┐
│                    序列化协议对比                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  协议          格式      性能    可读性   跨语言   大小      │
│  ──────────────────────────────────────────────────────     │
│  JSON          文本      中      高       是       大        │
│  XML           文本      低      高       是       很大      │
│  Protocol Buf  二进制    高      低       是       小        │
│  Thrift        二进制    高      低       是       小        │
│  MessagePack   二进制    高      低       是       小        │
│  FlatBuffers   二进制    极高    低       是       小        │
│                                                             │
│  序列化性能基准（相对 JSON）：                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Protocol Buffers  ████████████████████  3-10x     │   │
│  │  Thrift Binary     ██████████████████    2-8x      │   │
│  │  MessagePack       ████████████████      2-5x      │   │
│  │  JSON              ████████              1x        │   │
│  │  XML               ████                  0.5x      │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 9. Apache Thrift

### 9.1 Thrift 概述

```
┌─────────────────────────────────────────────────────────────┐
│                     Apache Thrift                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  定义：Facebook 开源的跨语言 RPC 框架                        │
│                                                             │
│  核心组件：                                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  IDL（接口定义语言）                                  │   │
│  │      ↓                                              │   │
│  │  Thrift 编译器 ─────→ 生成各语言代码                  │   │
│  │      ↓                  (C++/Java/Python/Go...)      │   │
│  │  ┌─────────────────────────────────────────────┐    │   │
│  │  │            Thrift 运行时栈                    │    │   │
│  │  │  ┌─────────┐  ┌─────────┐  ┌─────────┐      │    │   │
│  │  │  │Transport│  │Protocol │  │Processor│      │    │   │
│  │  │  │ 传输层  │  │ 协议层  │  │ 处理器  │      │    │   │
│  │  │  └─────────┘  └─────────┘  └─────────┘      │    │   │
│  │  └─────────────────────────────────────────────┘    │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  支持语言：C++, Java, Python, Go, PHP, Ruby, Node.js...    │
│  支持传输：TCP, HTTP, Unix Socket, Memory                  │
│  支持协议：Binary, Compact, JSON                           │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 Thrift IDL 语法

```thrift
// user.thrift - 接口定义文件

namespace cpp demo.user
namespace java com.demo.user
namespace py demo.user

// 枚举类型
enum UserStatus {
    ACTIVE = 1,
    INACTIVE = 2,
    BANNED = 3
}

// 结构体
struct User {
    1: required i64 id,              // 必填字段
    2: required string name,
    3: optional i32 age,             // 可选字段
    4: optional string email,
    5: UserStatus status = UserStatus.ACTIVE,  // 默认值
    6: optional map<string, string> metadata,  // 复杂类型
}

// 异常
exception UserNotFoundException {
    1: i64 userId,
    2: string message
}

exception InvalidParameterException {
    1: string paramName,
    2: string message
}

// 服务接口
service UserService {
    // 创建用户
    User createUser(1: string name, 2: i32 age)
        throws (1: InvalidParameterException e),

    // 获取用户
    User getUser(1: i64 userId)
        throws (1: UserNotFoundException e),

    // 获取用户列表
    list<User> listUsers(1: i32 offset, 2: i32 limit),

    // 更新用户
    void updateUser(1: User user),

    // 删除用户
    bool deleteUser(1: i64 userId),

    // 单向调用（无返回值，不等待响应）
    oneway void logUserAction(1: i64 userId, 2: string action)
}
```

### 9.3 Thrift 数据类型

```
┌─────────────────────────────────────────────────────────────┐
│                   Thrift 数据类型                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  基本类型：                                                  │
│  ├── bool        布尔值                                     │
│  ├── byte        8 位有符号整数                              │
│  ├── i16         16 位有符号整数                             │
│  ├── i32         32 位有符号整数                             │
│  ├── i64         64 位有符号整数                             │
│  ├── double      64 位浮点数                                 │
│  ├── string      字符串（UTF-8）                             │
│  └── binary      二进制数据                                  │
│                                                             │
│  容器类型：                                                  │
│  ├── list<T>     有序列表                                   │
│  ├── set<T>      无序集合                                   │
│  └── map<K,V>    键值映射                                   │
│                                                             │
│  复合类型：                                                  │
│  ├── struct      结构体                                     │
│  ├── enum        枚举                                       │
│  ├── exception   异常                                       │
│  ├── service     服务接口                                   │
│  └── typedef     类型别名                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.4 Thrift C++ 实现

```cpp
// === 编译 IDL 生成代码 ===
// thrift --gen cpp user.thrift
// 生成: gen-cpp/user_types.h, gen-cpp/UserService.h, gen-cpp/UserService.cpp

// === 服务端实现 ===
#include "gen-cpp/UserService.h"
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

class UserServiceHandler : public UserServiceIf {
private:
    std::map<int64_t, User> users_;
    int64_t nextId_ = 1;

public:
    // 实现 createUser
    void createUser(User& result, const std::string& name, int32_t age) override {
        result.id = nextId_++;
        result.name = name;
        result.age = age;
        result.status = UserStatus::ACTIVE;
        users_[result.id] = result;

        std::cout << "Created user: " << result.id << " - " << name << std::endl;
    }

    // 实现 getUser
    void getUser(User& result, int64_t userId) override {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            UserNotFoundException e;
            e.userId = userId;
            e.message = "User not found";
            throw e;
        }
        result = it->second;
    }

    // 实现 listUsers
    void listUsers(std::vector<User>& result, int32_t offset, int32_t limit) override {
        int count = 0;
        for (auto& [id, user] : users_) {
            if (count >= offset && count < offset + limit) {
                result.push_back(user);
            }
            count++;
        }
    }

    // 实现 updateUser
    void updateUser(const User& user) override {
        users_[user.id] = user;
    }

    // 实现 deleteUser
    bool deleteUser(int64_t userId) override {
        return users_.erase(userId) > 0;
    }

    // 实现 oneway 方法
    void logUserAction(int64_t userId, const std::string& action) override {
        std::cout << "User " << userId << " action: " << action << std::endl;
    }
};

int main() {
    int port = 9090;

    // 创建处理器
    auto handler = std::make_shared<UserServiceHandler>();
    auto processor = std::make_shared<UserServiceProcessor>(handler);

    // 创建传输层
    auto serverTransport = std::make_shared<TServerSocket>(port);
    auto transportFactory = std::make_shared<TBufferedTransportFactory>();
    auto protocolFactory = std::make_shared<TBinaryProtocolFactory>();

    // 创建服务器
    TSimpleServer server(processor, serverTransport,
                         transportFactory, protocolFactory);

    std::cout << "Starting server on port " << port << std::endl;
    server.serve();

    return 0;
}
```

```cpp
// === 客户端实现 ===
#include "gen-cpp/UserService.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

int main() {
    // 创建连接
    auto socket = std::make_shared<TSocket>("localhost", 9090);
    auto transport = std::make_shared<TBufferedTransport>(socket);
    auto protocol = std::make_shared<TBinaryProtocol>(transport);

    UserServiceClient client(protocol);

    try {
        transport->open();

        // 创建用户
        User newUser;
        client.createUser(newUser, "张三", 25);
        std::cout << "Created user ID: " << newUser.id << std::endl;

        // 获取用户
        User user;
        client.getUser(user, newUser.id);
        std::cout << "Got user: " << user.name << ", age: " << user.age << std::endl;

        // 获取列表
        std::vector<User> users;
        client.listUsers(users, 0, 10);
        std::cout << "Total users: " << users.size() << std::endl;

        // oneway 调用
        client.logUserAction(newUser.id, "login");

        transport->close();
    } catch (UserNotFoundException& e) {
        std::cerr << "User not found: " << e.userId << std::endl;
    } catch (TException& e) {
        std::cerr << "Thrift error: " << e.what() << std::endl;
    }

    return 0;
}
```

### 9.5 Thrift 协议层

```
┌─────────────────────────────────────────────────────────────┐
│                   Thrift 协议对比                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  协议                  特点                    适用场景      │
│  ─────────────────────────────────────────────────────────  │
│  TBinaryProtocol      二进制，高效              生产环境    │
│  TCompactProtocol     压缩二进制，更小          带宽受限    │
│  TJSONProtocol        JSON 格式，可读           调试、Web   │
│  TDebugProtocol       人类可读格式              开发调试    │
│                                                             │
│  Binary vs Compact 大小对比：                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  数据类型     Binary     Compact     压缩率         │   │
│  │  ─────────────────────────────────────────────────  │   │
│  │  i32          4 bytes    1-5 bytes   ~60%          │   │
│  │  i64          8 bytes    1-10 bytes  ~50%          │   │
│  │  string       4+len      1-3+len     ~20%          │   │
│  │  struct       header+    delta+      ~40%          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.6 Thrift 传输层

```cpp
// === 各种传输层配置 ===

// 1. TSocket - 基础 TCP Socket
auto socket = std::make_shared<TSocket>("localhost", 9090);
socket->setConnTimeout(5000);   // 连接超时 5s
socket->setSendTimeout(3000);   // 发送超时 3s
socket->setRecvTimeout(3000);   // 接收超时 3s

// 2. TBufferedTransport - 带缓冲
auto buffered = std::make_shared<TBufferedTransport>(socket);

// 3. TFramedTransport - 帧传输（用于非阻塞服务器）
auto framed = std::make_shared<TFramedTransport>(socket);

// 4. TZlibTransport - 压缩传输
auto zlib = std::make_shared<TZlibTransport>(buffered);

// 5. TSSLSocket - SSL/TLS 加密
auto sslFactory = std::make_shared<TSSLSocketFactory>();
sslFactory->loadCertificate("client.crt");
sslFactory->loadPrivateKey("client.key");
sslFactory->loadTrustedCertificates("ca.crt");
auto sslSocket = sslFactory->createSocket("localhost", 9090);
```

### 9.7 Thrift 服务器模型

```
┌─────────────────────────────────────────────────────────────┐
│                   Thrift 服务器模型                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  服务器类型              特点                  适用场景      │
│  ─────────────────────────────────────────────────────────  │
│  TSimpleServer          单线程，阻塞           调试         │
│  TThreadedServer        每连接一线程           中等负载     │
│  TThreadPoolServer      线程池                 生产环境     │
│  TNonblockingServer     非阻塞 I/O            高并发       │
│  THsHaServer           半同步半异步            高并发       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```cpp
// === 高性能服务器配置 ===

// 1. 线程池服务器
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/ThreadFactory.h>

auto threadManager = ThreadManager::newSimpleThreadManager(16);  // 16 线程
auto threadFactory = std::make_shared<ThreadFactory>();
threadManager->threadFactory(threadFactory);
threadManager->start();

TThreadPoolServer server(
    processor,
    serverTransport,
    transportFactory,
    protocolFactory,
    threadManager
);

// 2. 非阻塞服务器
#include <thrift/server/TNonblockingServer.h>

TNonblockingServer server(processor, protocolFactory, port);
server.setNumIOThreads(4);      // I/O 线程数
server.setNumWorkThreads(16);   // 工作线程数
server.serve();
```

---

## 10. gRPC

### 10.1 gRPC 概述

```
┌─────────────────────────────────────────────────────────────┐
│                         gRPC                                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  定义：Google 开源的高性能 RPC 框架                          │
│                                                             │
│  核心特性：                                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  • 基于 HTTP/2（多路复用、头部压缩、双向流）           │   │
│  │  • 使用 Protocol Buffers（高效序列化）                │   │
│  │  • 强类型 IDL（.proto 文件）                          │   │
│  │  • 支持 4 种通信模式                                   │   │
│  │  • 支持多语言                                          │   │
│  │  • 内置认证、负载均衡、健康检查                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  gRPC vs Thrift：                                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  特性          gRPC              Thrift              │   │
│  │  ─────────────────────────────────────────────────  │   │
│  │  传输协议      HTTP/2            TCP/HTTP            │   │
│  │  序列化        ProtoBuf          Thrift Binary       │   │
│  │  流式支持      原生              有限                │   │
│  │  浏览器        gRPC-Web          不支持              │   │
│  │  生态          云原生标准        老牌稳定            │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 10.2 gRPC 四种通信模式

```
┌─────────────────────────────────────────────────────────────┐
│                   gRPC 通信模式                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 一元调用（Unary）                                        │
│     Client ──── Request ────→ Server                        │
│     Client ←─── Response ──── Server                        │
│                                                             │
│  2. 服务端流（Server Streaming）                             │
│     Client ──── Request ────→ Server                        │
│     Client ←─── Stream[N] ─── Server                        │
│                                                             │
│  3. 客户端流（Client Streaming）                             │
│     Client ──── Stream[N] ──→ Server                        │
│     Client ←─── Response ──── Server                        │
│                                                             │
│  4. 双向流（Bidirectional Streaming）                        │
│     Client ←──→ Stream[N] ←──→ Server                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 10.3 Protocol Buffers 定义

```protobuf
// user.proto

syntax = "proto3";

package demo.user;

option cc_namespace = "demo::user";

// 用户消息
message User {
    int64 id = 1;
    string name = 2;
    int32 age = 3;
    string email = 4;
    UserStatus status = 5;
    map<string, string> metadata = 6;
}

enum UserStatus {
    UNKNOWN = 0;
    ACTIVE = 1;
    INACTIVE = 2;
    BANNED = 3;
}

// 请求/响应消息
message CreateUserRequest {
    string name = 1;
    int32 age = 2;
}

message GetUserRequest {
    int64 user_id = 1;
}

message ListUsersRequest {
    int32 offset = 1;
    int32 limit = 2;
}

message ListUsersResponse {
    repeated User users = 1;
    int32 total = 2;
}

// 服务定义
service UserService {
    // 一元调用
    rpc CreateUser(CreateUserRequest) returns (User);
    rpc GetUser(GetUserRequest) returns (User);

    // 服务端流
    rpc ListUsers(ListUsersRequest) returns (stream User);

    // 客户端流
    rpc BatchCreateUsers(stream CreateUserRequest) returns (ListUsersResponse);

    // 双向流
    rpc Chat(stream ChatMessage) returns (stream ChatMessage);
}

message ChatMessage {
    int64 user_id = 1;
    string content = 2;
    int64 timestamp = 3;
}
```

### 10.4 gRPC C++ 实现

```cpp
// === 编译 proto ===
// protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` user.proto

// === 服务端实现 ===
#include <grpcpp/grpcpp.h>
#include "user.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::ServerReaderWriter;

class UserServiceImpl final : public demo::user::UserService::Service {
private:
    std::map<int64_t, demo::user::User> users_;
    std::atomic<int64_t> nextId_{1};

public:
    // 一元调用
    Status CreateUser(ServerContext* context,
                      const demo::user::CreateUserRequest* request,
                      demo::user::User* response) override {
        response->set_id(nextId_++);
        response->set_name(request->name());
        response->set_age(request->age());
        response->set_status(demo::user::ACTIVE);

        users_[response->id()] = *response;
        return Status::OK;
    }

    Status GetUser(ServerContext* context,
                   const demo::user::GetUserRequest* request,
                   demo::user::User* response) override {
        auto it = users_.find(request->user_id());
        if (it == users_.end()) {
            return Status(grpc::NOT_FOUND, "User not found");
        }
        *response = it->second;
        return Status::OK;
    }

    // 服务端流
    Status ListUsers(ServerContext* context,
                     const demo::user::ListUsersRequest* request,
                     ServerWriter<demo::user::User>* writer) override {
        int count = 0;
        for (auto& [id, user] : users_) {
            if (count >= request->offset() &&
                count < request->offset() + request->limit()) {
                writer->Write(user);
            }
            count++;
        }
        return Status::OK;
    }

    // 客户端流
    Status BatchCreateUsers(ServerContext* context,
                            ServerReader<demo::user::CreateUserRequest>* reader,
                            demo::user::ListUsersResponse* response) override {
        demo::user::CreateUserRequest request;
        while (reader->Read(&request)) {
            demo::user::User user;
            user.set_id(nextId_++);
            user.set_name(request.name());
            user.set_age(request.age());
            users_[user.id()] = user;
            *response->add_users() = user;
        }
        response->set_total(response->users_size());
        return Status::OK;
    }

    // 双向流
    Status Chat(ServerContext* context,
                ServerReaderWriter<demo::user::ChatMessage,
                                   demo::user::ChatMessage>* stream) override {
        demo::user::ChatMessage msg;
        while (stream->Read(&msg)) {
            // Echo back
            demo::user::ChatMessage response;
            response.set_user_id(0);  // Server
            response.set_content("Echo: " + msg.content());
            response.set_timestamp(time(nullptr));
            stream->Write(response);
        }
        return Status::OK;
    }
};

int main() {
    std::string server_address("0.0.0.0:50051");
    UserServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
    return 0;
}
```

```cpp
// === 客户端实现 ===
#include <grpcpp/grpcpp.h>
#include "user.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;

int main() {
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());
    auto stub = demo::user::UserService::NewStub(channel);

    // 一元调用
    {
        ClientContext context;
        demo::user::CreateUserRequest request;
        request.set_name("张三");
        request.set_age(25);

        demo::user::User user;
        Status status = stub->CreateUser(&context, request, &user);

        if (status.ok()) {
            std::cout << "Created user: " << user.id() << std::endl;
        }
    }

    // 服务端流
    {
        ClientContext context;
        demo::user::ListUsersRequest request;
        request.set_offset(0);
        request.set_limit(10);

        std::unique_ptr<ClientReader<demo::user::User>> reader(
            stub->ListUsers(&context, request)
        );

        demo::user::User user;
        while (reader->Read(&user)) {
            std::cout << "User: " << user.name() << std::endl;
        }
    }

    return 0;
}
```

---

## 11. 其他 RPC 框架

### 11.1 框架对比

```
┌─────────────────────────────────────────────────────────────┐
│                   RPC 框架对比                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  框架          开发者    协议       序列化     主要优势      │
│  ─────────────────────────────────────────────────────────  │
│  gRPC         Google    HTTP/2     ProtoBuf   云原生标准    │
│  Thrift       Facebook  TCP/HTTP   Thrift     跨语言全面    │
│  Dubbo        Alibaba   TCP        Hessian    Java 生态    │
│  brpc         Baidu     多协议     ProtoBuf   高性能 C++   │
│  Tars         Tencent   TCP        Tars       运维友好     │
│  JSON-RPC     -         HTTP       JSON       简单通用     │
│  XML-RPC      -         HTTP       XML        遗留系统     │
│                                                             │
│  性能基准（QPS，参考值）：                                   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  brpc         ████████████████████████████  100K+   │   │
│  │  gRPC         ████████████████████          80K     │   │
│  │  Thrift       ██████████████████            70K     │   │
│  │  Dubbo        ████████████████              60K     │   │
│  │  JSON-RPC     ████████                      30K     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 11.2 brpc (Baidu RPC)

```cpp
// brpc 服务端示例
#include <brpc/server.h>

class UserServiceImpl : public UserService {
public:
    void GetUser(google::protobuf::RpcController* cntl_base,
                 const GetUserRequest* request,
                 User* response,
                 google::protobuf::Closure* done) override {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

        // 业务逻辑
        response->set_id(request->user_id());
        response->set_name("TestUser");
    }
};

int main() {
    brpc::Server server;
    UserServiceImpl impl;

    server.AddService(&impl, brpc::SERVER_DOESNT_OWN_SERVICE);

    brpc::ServerOptions options;
    options.idle_timeout_sec = 60;
    options.max_concurrency = 1000;

    server.Start(8080, &options);
    server.RunUntilAskedToQuit();

    return 0;
}
```

---

## 12. 同机跨进程 RPC 通信原理

### 12.1 核心问题：本地 RPC 是否经过网卡？

```
┌─────────────────────────────────────────────────────────────┐
│              同机 RPC 通信路径分析                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  问题：两个进程在同一台机器上通过 127.0.0.1 进行 RPC 通信，  │
│        数据是否会经过物理网卡？                              │
│                                                             │
│  答案：不会！数据在内核协议栈内完成回环，不经过网卡硬件。      │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    用户空间                          │   │
│  │   ┌──────────┐                  ┌──────────┐        │   │
│  │   │ 进程 A   │                  │ 进程 B   │        │   │
│  │   │ (Client) │                  │ (Server) │        │   │
│  │   └────┬─────┘                  └────┬─────┘        │   │
│  └────────┼─────────────────────────────┼──────────────┘   │
│           │ send()                      │ recv()           │
│           ↓                             ↑                  │
│  ┌────────────────────────────────────────────────────┐    │
│  │                    内核空间                         │    │
│  │  ┌─────────────────────────────────────────────┐   │    │
│  │  │              TCP/IP 协议栈                   │   │    │
│  │  │                                             │   │    │
│  │  │   ┌─────────┐    loopback    ┌─────────┐   │   │    │
│  │  │   │ Socket  │ ←───────────→  │ Socket  │   │   │    │
│  │  │   │  缓冲区  │    (lo 接口)   │  缓冲区  │   │   │    │
│  │  │   └─────────┘                └─────────┘   │   │    │
│  │  │              ↑                             │   │    │
│  │  │              │ 内核直接拷贝                 │   │    │
│  │  │              ↓                             │   │    │
│  │  └─────────────────────────────────────────────┘   │    │
│  │                      ✗                              │    │
│  │            不经过物理网卡驱动                        │    │
│  └────────────────────────────────────────────────────┘    │
│                         ✗                                  │
│  ┌────────────────────────────────────────────────────┐    │
│  │              物理网卡 (eth0/ens33)                  │    │
│  │                   完全不参与                        │    │
│  └────────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.2 Loopback 回环接口原理

```
┌─────────────────────────────────────────────────────────────┐
│                    Loopback 接口详解                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  什么是 Loopback（lo）接口？                                 │
│  ───────────────────────────────────────────────────────    │
│  • 虚拟网络接口，没有对应的物理硬件                          │
│  • IP 地址：127.0.0.1（IPv4）/ ::1（IPv6）                  │
│  • 所有发往 127.0.0.0/8 的数据都走 lo 接口                  │
│  • 数据不会离开内核，直接在协议栈内"回环"                    │
│                                                             │
│  数据流向对比：                                              │
│                                                             │
│  【本地回环 127.0.0.1】                                      │
│  ┌────────┐    ┌──────────────────────────────┐            │
│  │ 进程 A │───→│  TCP/IP 栈 (lo 虚拟接口)      │───→ 进程 B  │
│  └────────┘    │  • 完整协议处理               │            │
│                │  • 无网卡驱动调用              │            │
│                │  • 无 DMA / 中断              │            │
│                └──────────────────────────────┘            │
│                                                             │
│  【远程通信 192.168.1.x】                                    │
│  ┌────────┐    ┌────────────┐    ┌────────┐    ┌────────┐  │
│  │ 进程 A │───→│ TCP/IP 栈  │───→│ 网卡驱动│───→│物理网卡│  │
│  └────────┘    └────────────┘    └────────┘    └───┬────┘  │
│                                                     │ 网线  │
│  ┌────────┐    ┌────────────┐    ┌────────┐    ┌───↓────┐  │
│  │ 进程 B │←───│ TCP/IP 栈  │←───│ 网卡驱动│←───│物理网卡│  │
│  └────────┘    └────────────┘    └────────┘    └────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.3 同机通信的几种方式及性能对比

```
┌─────────────────────────────────────────────────────────────┐
│              同机跨进程通信方式对比                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  方式              协议栈开销   数据拷贝   性能    适用场景   │
│  ─────────────────────────────────────────────────────────  │
│  共享内存           无          0 次     ★★★★★  最高性能    │
│  Unix Domain Socket 无          2 次     ★★★★☆  通用 IPC   │
│  TCP 127.0.0.1     部分         2 次     ★★★☆☆  兼容性好   │
│  命名管道 FIFO      无          2 次     ★★★☆☆  简单通信   │
│                                                             │
│  详细分析：                                                  │
│                                                             │
│  1. 共享内存 + 同步原语                                      │
│     ┌─────────────────────────────────────────────────┐    │
│     │  进程 A ←────→ 物理内存页 ←────→ 进程 B          │    │
│     │           （零拷贝，直接访问）                    │    │
│     │  优点：最快，无系统调用开销                       │    │
│     │  缺点：需要自己实现同步，复杂度高                 │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
│  2. Unix Domain Socket (UDS)                                │
│     ┌─────────────────────────────────────────────────┐    │
│     │  进程 A → 内核缓冲 → 进程 B                       │    │
│     │  优点：                                          │    │
│     │  • 无 TCP/IP 协议栈开销                          │    │
│     │  • 支持传递文件描述符                             │    │
│     │  • API 与 TCP Socket 兼容                        │    │
│     │  缺点：仅限本机                                   │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
│  3. TCP Loopback (127.0.0.1)                                │
│     ┌─────────────────────────────────────────────────┐    │
│     │  进程 A → TCP 栈 → lo 接口 → TCP 栈 → 进程 B     │    │
│     │  优点：                                          │    │
│     │  • 代码无需修改，换 IP 即可跨机器                  │    │
│     │  • 协议栈提供可靠传输                             │    │
│     │  缺点：                                          │    │
│     │  • 有 TCP 头开销（40 字节/包）                    │    │
│     │  • 协议栈处理消耗 CPU                             │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.4 gRPC/Thrift 同机通信优化

```
┌─────────────────────────────────────────────────────────────┐
│              RPC 框架的同机优化策略                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. gRPC 同机优化                                           │
│     ─────────────────────────────────────────────────────   │
│                                                             │
│     方式 A: Unix Domain Socket                              │
│     ┌─────────────────────────────────────────────────┐    │
│     │  // 服务端                                       │    │
│     │  server.AddListeningPort(                        │    │
│     │      "unix:///tmp/grpc.sock",                    │    │
│     │      grpc::InsecureServerCredentials()           │    │
│     │  );                                              │    │
│     │                                                  │    │
│     │  // 客户端                                       │    │
│     │  auto channel = grpc::CreateChannel(             │    │
│     │      "unix:///tmp/grpc.sock",                    │    │
│     │      grpc::InsecureChannelCredentials()          │    │
│     │  );                                              │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
│     方式 B: In-Process Channel（进程内）                    │
│     ┌─────────────────────────────────────────────────┐    │
│     │  // 同进程内的不同组件通信，完全绕过网络          │    │
│     │  grpc::experimental::LocalChannel(               │    │
│     │      server,                                     │    │
│     │      grpc::experimental::LOCAL_TCP              │    │
│     │  );                                              │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
│  2. Thrift 同机优化                                         │
│     ─────────────────────────────────────────────────────   │
│                                                             │
│     方式 A: Unix Domain Socket                              │
│     ┌─────────────────────────────────────────────────┐    │
│     │  // 使用 TServerSocket 的 Unix 路径              │    │
│     │  auto transport = std::make_shared<TServerSocket│    │
│     │      >("/tmp/thrift.sock");                      │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
│     方式 B: Shared Memory Transport                         │
│     ┌─────────────────────────────────────────────────┐    │
│     │  // 自定义共享内存传输层                          │    │
│     │  class SharedMemoryTransport : public TTransport │    │
│     │  {                                               │    │
│     │      // 使用 mmap + 环形缓冲区                    │    │
│     │  };                                              │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.5 同机 RPC 的协议栈路径详解

```
┌─────────────────────────────────────────────────────────────┐
│           TCP 127.0.0.1 的内核处理路径                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  发送端（进程 A）                                            │
│  ─────────────────────────────────────────────────────────  │
│  1. write()/send() 系统调用                                 │
│  2. 进入内核 → 找到 socket 结构                             │
│  3. TCP 层：                                                │
│     • 分段（如果数据大于 MSS）                               │
│     • 添加 TCP 头部（20+ 字节）                              │
│     • 计算校验和                                            │
│     • 放入发送队列                                          │
│  4. IP 层：                                                 │
│     • 路由查找 → 目标 127.x.x.x → 选择 lo 接口              │
│     • 添加 IP 头部（20 字节）                                │
│  5. lo 接口：                                               │
│     • 不调用网卡驱动                                        │
│     • 直接将数据包放入接收队列                               │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │    ★ 关键：在第 5 步直接"回环"，不经过物理层 ★         │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  接收端（进程 B）                                            │
│  ─────────────────────────────────────────────────────────  │
│  6. 内核软中断处理 lo 接口的"接收"数据                       │
│  7. IP 层：验证目标地址，去掉 IP 头                          │
│  8. TCP 层：                                                │
│     • 校验和验证                                            │
│     • 匹配 socket（四元组）                                 │
│     • 放入接收缓冲区                                        │
│     • 唤醒阻塞的 recv()                                     │
│  9. 用户进程从接收缓冲区读取数据                             │
│                                                             │
│  开销分析：                                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  省掉的开销：                                        │   │
│  │  ✗ 网卡驱动调用                                      │   │
│  │  ✗ DMA 数据传输                                      │   │
│  │  ✗ 硬件中断                                          │   │
│  │  ✗ 物理层编解码                                      │   │
│  │  ✗ 网线传输延迟                                      │   │
│  │                                                      │   │
│  │  仍有的开销：                                        │   │
│  │  ✓ TCP/IP 头部封装与解析                             │   │
│  │  ✓ 校验和计算                                        │   │
│  │  ✓ 2 次内存拷贝（用户态 ↔ 内核态）                    │   │
│  │  ✓ 系统调用开销                                      │   │
│  │  ✓ 软中断处理                                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.6 Unix Domain Socket vs TCP Loopback

```
┌─────────────────────────────────────────────────────────────┐
│          Unix Domain Socket vs TCP 127.0.0.1               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  维度              Unix Domain Socket    TCP 127.0.0.1      │
│  ─────────────────────────────────────────────────────────  │
│  协议栈处理         无                    完整 TCP/IP       │
│  头部开销           无                    40+ 字节/包       │
│  校验和计算         无                    有                │
│  拥塞控制           无                    有（但无意义）     │
│  性能               更快 10-30%           基准              │
│  可移植性           POSIX 系统            全平台            │
│  切换到网络         需改代码              改 IP 即可        │
│  传递 fd            支持                  不支持            │
│  凭证传递           支持 (SO_PEERCRED)    不支持            │
│                                                             │
│  性能测试参考（1KB 消息，单线程）：                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  机制                  延迟         吞吐量            │   │
│  │  ─────────────────────────────────────────────────  │   │
│  │  共享内存 + futex       ~0.5 μs     5M+ msg/s        │   │
│  │  Unix Domain Socket    ~3 μs       800K msg/s       │   │
│  │  TCP 127.0.0.1         ~5 μs       500K msg/s       │   │
│  │  TCP 跨机器（千兆）     ~100 μs     50K msg/s        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  代码对比：                                                  │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  // Unix Domain Socket                                      │
│  struct sockaddr_un addr;                                   │
│  addr.sun_family = AF_UNIX;                                 │
│  strcpy(addr.sun_path, "/tmp/app.sock");                    │
│  connect(sock, (sockaddr*)&addr, sizeof(addr));             │
│                                                             │
│  // TCP Loopback                                            │
│  struct sockaddr_in addr;                                   │
│  addr.sin_family = AF_INET;                                 │
│  addr.sin_port = htons(8080);                               │
│  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);           │
│  connect(sock, (sockaddr*)&addr, sizeof(addr));             │
│                                                             │
│  // 切换：Unix → TCP 只需改地址结构                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.7 高性能同机 RPC 实现方案

```cpp
// === 方案 1: 共享内存 + 环形缓冲区 ===
// 最高性能，适合超低延迟场景

#include <sys/mman.h>
#include <atomic>

struct RingBuffer {
    static constexpr size_t CAPACITY = 1024 * 1024;  // 1MB

    std::atomic<size_t> write_pos{0};
    std::atomic<size_t> read_pos{0};
    char data[CAPACITY];

    bool write(const char* msg, size_t len) {
        size_t w = write_pos.load(std::memory_order_relaxed);
        size_t r = read_pos.load(std::memory_order_acquire);

        if (CAPACITY - (w - r) < len + sizeof(size_t)) {
            return false;  // 缓冲区满
        }

        // 写入长度 + 数据
        size_t pos = w % CAPACITY;
        memcpy(data + pos, &len, sizeof(size_t));
        memcpy(data + pos + sizeof(size_t), msg, len);

        write_pos.store(w + len + sizeof(size_t),
                        std::memory_order_release);
        return true;
    }

    bool read(char* buf, size_t* len) {
        size_t r = read_pos.load(std::memory_order_relaxed);
        size_t w = write_pos.load(std::memory_order_acquire);

        if (r == w) return false;  // 缓冲区空

        size_t pos = r % CAPACITY;
        memcpy(len, data + pos, sizeof(size_t));
        memcpy(buf, data + pos + sizeof(size_t), *len);

        read_pos.store(r + *len + sizeof(size_t),
                       std::memory_order_release);
        return true;
    }
};

// 创建共享内存
int fd = shm_open("/rpc_shm", O_CREAT | O_RDWR, 0666);
ftruncate(fd, sizeof(RingBuffer));
RingBuffer* ring = (RingBuffer*)mmap(nullptr, sizeof(RingBuffer),
    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
```

```cpp
// === 方案 2: Unix Domain Socket + 零拷贝 ===
// 平衡性能与易用性

#include <sys/socket.h>
#include <sys/un.h>

class LocalRpcTransport {
public:
    // 使用 splice() 实现管道间零拷贝
    ssize_t sendfile_zerocopy(int out_fd, int in_fd, size_t count) {
        int pipefd[2];
        pipe(pipefd);

        // 从输入 fd splice 到管道
        ssize_t n = splice(in_fd, nullptr, pipefd[1], nullptr,
                           count, SPLICE_F_MOVE);

        // 从管道 splice 到输出 fd
        splice(pipefd[0], nullptr, out_fd, nullptr,
               n, SPLICE_F_MOVE);

        close(pipefd[0]);
        close(pipefd[1]);
        return n;
    }

    // 使用 MSG_ZEROCOPY 标志（Linux 4.14+）
    ssize_t send_zerocopy(int sock, const void* buf, size_t len) {
        return send(sock, buf, len, MSG_ZEROCOPY);
    }
};
```

```cpp
// === 方案 3: 智能选择传输层 ===
// 根据目标自动选择最优传输

class SmartRpcChannel {
public:
    enum TransportType {
        SHARED_MEMORY,    // 同进程（不同组件）
        UNIX_SOCKET,      // 同机器
        TCP_LOOPBACK,     // 同机器（需兼容）
        TCP_NETWORK       // 跨机器
    };

    TransportType selectTransport(const std::string& target) {
        if (target == "inproc://") {
            return SHARED_MEMORY;
        }
        if (target.starts_with("unix://")) {
            return UNIX_SOCKET;
        }
        if (target.starts_with("127.") || target == "localhost") {
            // 同机：优先 Unix Socket
            return UNIX_SOCKET;
        }
        return TCP_NETWORK;
    }

    std::unique_ptr<Transport> createTransport(const std::string& target) {
        switch (selectTransport(target)) {
            case SHARED_MEMORY:
                return std::make_unique<SharedMemoryTransport>();
            case UNIX_SOCKET:
                return std::make_unique<UnixSocketTransport>();
            case TCP_LOOPBACK:
            case TCP_NETWORK:
                return std::make_unique<TcpTransport>();
        }
    }
};
```

### 12.8 面试问答：同机 RPC 深度问题

```
┌─────────────────────────────────────────────────────────────┐
│              同机 RPC 面试问答                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Q1: 127.0.0.1 的数据会经过网卡吗？                          │
│  ─────────────────────────────────────────────────────────  │
│  答：不会。127.0.0.1 属于 loopback 地址段（127.0.0.0/8），   │
│  数据在内核 IP 层路由时会被定向到虚拟 lo 接口，直接回环      │
│  到接收队列，完全不涉及物理网卡驱动和硬件。                  │
│                                                             │
│  Q2: 为什么同机还要用 TCP 而不是共享内存？                   │
│  ─────────────────────────────────────────────────────────  │
│  答：主要原因：                                              │
│  1. 代码兼容性：改 IP 即可跨机器，无需重构                   │
│  2. 开发效率：利用成熟的 RPC 框架（gRPC、Thrift）            │
│  3. 调试便利：可用 tcpdump、Wireshark 抓包分析               │
│  4. 隔离性：进程崩溃不会污染共享内存                         │
│  5. 安全性：TCP 有端口隔离，共享内存需额外权限控制            │
│                                                             │
│  Q3: Unix Domain Socket 为什么比 TCP 127.0.0.1 快？         │
│  ─────────────────────────────────────────────────────────  │
│  答：UDS 跳过了完整的 TCP/IP 协议栈：                        │
│  • 无 TCP/IP 头部封装（节省 40+ 字节/包）                    │
│  • 无校验和计算                                             │
│  • 无拥塞控制、流量控制逻辑                                  │
│  • 无路由查找                                               │
│  内核直接在两个 socket 缓冲区之间拷贝数据。                  │
│                                                             │
│  Q4: 如何优化同机 gRPC 性能？                                │
│  ─────────────────────────────────────────────────────────  │
│  答：                                                        │
│  1. 使用 Unix Domain Socket：unix:///path/to/sock           │
│  2. 禁用不必要的功能（如 TLS）                               │
│  3. 调整缓冲区大小                                          │
│  4. 使用 streaming API 减少调用次数                          │
│  5. 考虑 In-Process Channel（同进程内）                      │
│  6. 对于极致性能，使用共享内存自定义传输层                   │
│                                                             │
│  Q5: Docker 容器间同机通信走什么路径？                       │
│  ─────────────────────────────────────────────────────────  │
│  答：取决于网络模式：                                        │
│  • bridge 模式：经过 docker0 虚拟网桥，仍在内核中路由        │
│  • host 模式：与宿主机相同，127.0.0.1 走 lo 接口            │
│  • 共享网络命名空间：可使用 Unix Domain Socket               │
│                                                             │
│  无论哪种模式，只要不跨物理机，都不经过物理网卡。            │
│                                                             │
│  Q6: 同机 RPC 的瓶颈通常在哪里？                             │
│  ─────────────────────────────────────────────────────────  │
│  答：主要瓶颈：                                              │
│  1. 序列化/反序列化 CPU 开销                                 │
│  2. 用户态/内核态切换（每次 send/recv）                      │
│  3. 内存拷贝（用户缓冲区 ↔ 内核缓冲区）                      │
│  4. 锁竞争（多线程共享连接）                                 │
│                                                             │
│  优化思路：                                                  │
│  • 批量发送减少系统调用                                     │
│  • 使用零拷贝技术（splice/MSG_ZEROCOPY）                     │
│  • 每线程独立连接减少锁竞争                                  │
│  • 考虑 io_uring 异步 IO（Linux 5.1+）                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 13. 面试深度问题

### Q1: IPC 各机制的性能差异根本原因是什么？

**答案**：

```
性能差异的核心在于数据复制次数和同步开销：

1. 共享内存最快
   - 零复制：进程直接访问同一物理内存
   - 无系统调用开销（映射后）
   - 但需要额外的同步机制

2. 管道/消息队列
   - 两次复制：用户态 → 内核 → 用户态
   - 每次读写都是系统调用

3. Socket（本地）
   - 经过协议栈（即使是 Unix Domain Socket）
   - 但优于 TCP：无网络层处理

4. 同步开销
   - 共享内存需要用户空间锁
   - 管道/消息队列由内核同步
   - 信号量开销最小（仅同步无数据）
```

### Q2: RPC 框架如何实现服务发现？

**答案**：

```
服务发现模式：

1. 客户端发现模式
   Client → 注册中心（ZooKeeper/etcd/Consul）→ 获取服务列表 → 负载均衡 → 调用

2. 服务端发现模式
   Client → 负载均衡器（LB）→ 注册中心 → 服务实例

3. 服务注册流程：
   ┌─────────┐        ┌─────────┐        ┌─────────┐
   │ Service │──注册─→│Registry │←─订阅──│ Client  │
   │         │←─心跳──│(etcd)   │──推送─→│         │
   └─────────┘        └─────────┘        └─────────┘

4. 健康检查：
   - 主动心跳：服务定期上报
   - 被动检查：注册中心主动探测
   - TTL 租约：超时自动下线
```

### Q3: Thrift 和 gRPC 如何选择？

**答案**：

```
选择 gRPC 的场景：
• 云原生/Kubernetes 环境（官方支持）
• 需要流式通信（双向流）
• 浏览器客户端（gRPC-Web）
• 团队已使用 Protocol Buffers
• 需要 HTTP/2 特性

选择 Thrift 的场景：
• 遗留系统集成
• 需要更多协议选择（Compact 压缩）
• Facebook 生态系统
• 需要更灵活的传输层

性能几乎相当，选择更多取决于生态和团队熟悉度。
```

### Q4: 如何设计一个高性能 RPC 框架？

**答案**：

```
核心设计点：

1. 网络模型
   - Reactor/Proactor 模式
   - I/O 多路复用（epoll）
   - 工作线程池

2. 序列化
   - 选择高效协议（ProtoBuf/FlatBuffers）
   - 支持零拷贝（FlatBuffers）
   - 考虑压缩（LZ4）

3. 连接管理
   - 连接池复用
   - 长连接 + 心跳
   - 优雅关闭

4. 负载均衡
   - Round Robin / Weighted
   - 一致性哈希
   - 自适应负载

5. 容错机制
   - 超时控制
   - 重试策略（指数退避）
   - 熔断降级（Circuit Breaker）
   - 限流（令牌桶/漏桶）

6. 可观测性
   - 链路追踪（OpenTelemetry）
   - 指标采集（Prometheus）
   - 日志关联
```

### Q5: 共享内存进程间同步的几种方式？

**答案**：

```cpp
// 1. POSIX 进程间互斥锁
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
pthread_mutex_init(&shm->mutex, &attr);

// 2. POSIX 进程间条件变量
pthread_condattr_t cond_attr;
pthread_condattr_init(&cond_attr);
pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
pthread_cond_init(&shm->cond, &cond_attr);

// 3. POSIX 命名信号量
sem_t* sem = sem_open("/mysem", O_CREAT, 0666, 1);
sem_wait(sem);
// 临界区
sem_post(sem);

// 4. 文件锁
int fd = open("lockfile", O_RDWR);
flock(fd, LOCK_EX);  // 排他锁
// 临界区
flock(fd, LOCK_UN);

// 5. 原子操作 + 自旋（无锁）
std::atomic<int> shm_flag;
while (shm_flag.exchange(1) == 1) {
    // 自旋等待
}
// 临界区
shm_flag.store(0);
```

### Q6: Unix Domain Socket 为什么可以传递文件描述符？

**答案**：

```
文件描述符是进程私有的整数索引，指向内核中的 file 结构。

传递机制：
1. 发送方通过 SCM_RIGHTS 控制消息发送 fd
2. 内核在接收方进程分配新的 fd 编号
3. 新 fd 指向相同的内核 file 结构

                发送方                接收方
              ┌─────┐              ┌─────┐
              │fd=3 │              │fd=7 │
              └──┬──┘              └──┬──┘
                 │                    │
                 ↓                    ↓
              ┌──────────────────────────┐
              │   内核 file 结构（共享）   │
              │   引用计数 + 1            │
              └──────────────────────────┘

用途：
• 父进程打开文件，传给受限子进程
• 服务进程传递监听 socket 给 worker
• 热升级时传递连接给新进程
```

### Q7: RPC 超时设计有哪些考量？

**答案**：

```
超时层次：

1. 连接超时（ConnectTimeout）
   - TCP 三次握手时间
   - 通常 1-5 秒

2. 读超时（ReadTimeout）
   - 等待响应的时间
   - 根据业务 RT 设置

3. 写超时（WriteTimeout）
   - 发送请求的时间
   - 通常较短

4. 请求超时（RequestTimeout）
   - 端到端总时间
   - 包含重试时间

超时传递：
┌────────────────────────────────────────┐
│  服务 A (timeout=5s)                    │
│     ↓ 已用 1s                           │
│  服务 B (timeout=4s)   ← 传递剩余时间    │
│     ↓ 已用 2s                           │
│  服务 C (timeout=2s)                    │
└────────────────────────────────────────┘

超时 = min(本地配置, 上游传递)

重试与超时：
总超时 = 单次超时 × 重试次数
需预留重试时间：5s 总超时，3 次重试 → 单次 1.5s
```

### Q8: 如何实现 RPC 的优雅关闭？

**答案**：

```cpp
// 优雅关闭流程

class GracefulShutdown {
public:
    void shutdown() {
        // 1. 停止接收新请求
        server_.stopAccepting();

        // 2. 从注册中心下线
        registry_.deregister(serviceId_);

        // 3. 等待注册中心同步（让客户端感知）
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 4. 等待正在处理的请求完成
        while (activeRequests_.load() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // 超时强制关闭
            if (elapsed > maxShutdownTime_) {
                break;
            }
        }

        // 5. 关闭连接
        server_.closeConnections();

        // 6. 释放资源
        cleanup();
    }

private:
    std::atomic<int> activeRequests_;
};

// 信号处理
void signalHandler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        gracefulShutdown.shutdown();
    }
}
```

---

## 14. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                     IPC/RPC 知识图谱                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  IPC（本机进程间通信）                                       │
│  ├── 管道：父子进程、简单通信                                │
│  ├── 消息队列：消息分类、解耦                                │
│  ├── 共享内存：高性能、需同步                                │
│  ├── 信号：事件通知                                         │
│  └── Unix Socket：通用、可传 fd                             │
│                                                             │
│  RPC（远程过程调用）                                         │
│  ├── 序列化：ProtoBuf、Thrift、JSON                         │
│  ├── 传输：HTTP/2、TCP                                      │
│  ├── 服务发现：ZooKeeper、etcd、Consul                      │
│  └── 负载均衡：客户端/服务端                                 │
│                                                             │
│  主流框架                                                   │
│  ├── gRPC：云原生标准、HTTP/2、流式                         │
│  ├── Thrift：跨语言全面、协议灵活                           │
│  ├── brpc：C++ 高性能                                       │
│  └── Dubbo：Java 生态                                       │
│                                                             │
│  同机 RPC 优化                                              │
│  ├── Loopback 不经网卡（内核直接回环）                       │
│  ├── Unix Domain Socket 比 TCP 更快                         │
│  ├── 共享内存实现极致性能                                   │
│  └── gRPC/Thrift 本地优化策略                               │
│                                                             │
│  面试重点                                                   │
│  ├── IPC 各机制对比和选择                                   │
│  ├── 共享内存同步方式                                       │
│  ├── RPC 框架设计要点                                       │
│  ├── 同机 RPC 是否经过网卡                                  │
│  ├── 超时/重试/熔断机制                                     │
│  └── 服务发现与负载均衡                                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
