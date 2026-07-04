# 06 - 系统调用 (System Calls)

## 简介

系统调用是用户程序请求操作系统内核服务的接口，是用户态与内核态交互的桥梁。

**核心概念**：
- **用户态/内核态**：CPU 的两种运行模式
- **中断**：硬件/软件触发的 CPU 异常处理
- **系统调用**：从用户态切换到内核态执行内核代码

---

## 1. 用户态与内核态

### 1.1 基本概念

**面试问题**：什么是用户态和内核态？为什么要区分？

**答案**：

| 特性 | 用户态 (User Mode) | 内核态 (Kernel Mode) |
|------|-------------------|---------------------|
| **权限** | 受限（Ring 3） | 完全权限（Ring 0） |
| **可访问内存** | 用户空间 | 全部内存 |
| **执行指令** | 非特权指令 | 所有指令 |
| **硬件访问** | 不能直接访问 | 可以直接访问 |
| **代码来源** | 应用程序 | 操作系统内核 |

```
x86 CPU 特权级 (Ring)：

┌─────────────────────────────────────────┐
│                 Ring 0                  │
│               (内核态)                  │
│        操作系统内核、驱动程序           │
│  ┌─────────────────────────────────┐   │
│  │            Ring 1               │   │
│  │   (通常未使用，某些虚拟化使用)   │   │
│  │  ┌─────────────────────────┐   │   │
│  │  │        Ring 2           │   │   │
│  │  │   (通常未使用)          │   │   │
│  │  │  ┌─────────────────┐   │   │   │
│  │  │  │     Ring 3      │   │   │   │
│  │  │  │   (用户态)      │   │   │   │
│  │  │  │  应用程序       │   │   │   │
│  │  │  └─────────────────┘   │   │   │
│  │  └─────────────────────────┘   │   │
│  └─────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

**为什么要区分**：
1. **安全性**：防止用户程序破坏系统
2. **稳定性**：隔离应用程序和内核
3. **资源保护**：控制对硬件的访问

---

### 1.2 态切换时机

**面试问题**：什么时候会发生用户态到内核态的切换？

**答案**：

| 触发原因 | 说明 | 示例 |
|----------|------|------|
| **系统调用** | 用户程序请求内核服务 | read(), write(), fork() |
| **异常** | CPU 执行指令时发生错误 | 除零、缺页、段错误 |
| **中断** | 外部设备请求处理 | 键盘、网卡、定时器 |

```
态切换过程：

用户态                                    内核态
  │                                         │
  │ ──── 系统调用/中断/异常 ──────────────► │
  │                                         │
  │  1. 保存用户态上下文                    │
  │  2. 切换到内核栈                        │
  │  3. 执行内核代码                        │
  │  4. 恢复用户态上下文                    │
  │                                         │
  │ ◄──── 返回用户态 ─────────────────────  │
  │                                         │
```

---

### 1.3 内核栈与用户栈

**面试问题**：为什么进程有两个栈？

**答案**：

```
┌─────────────────────────────────────────────────────┐
│                    进程地址空间                      │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌─────────────────────────────────────────────┐   │
│  │              用户栈                          │   │
│  │  - 用户态函数调用                            │   │
│  │  - 局部变量                                  │   │
│  │  - 函数参数                                  │   │
│  └─────────────────────────────────────────────┘   │
│                        ↓ 向下增长                   │
│                                                     │
│  ┌─────────────────────────────────────────────┐   │
│  │              内核栈                          │   │
│  │  - 内核态函数调用                            │   │
│  │  - 保存的用户态寄存器                        │   │
│  │  - 内核局部变量                              │   │
│  │  - 通常 8KB (Linux)                         │   │
│  └─────────────────────────────────────────────┘   │
│                                                     │
└─────────────────────────────────────────────────────┘

分离原因：
1. 安全：用户程序不能访问内核栈
2. 隔离：内核栈大小固定，防止溢出影响内核
3. 简化：每个进程独立的内核栈
```

---

## 2. 中断与异常

### 2.1 中断类型

**面试问题**：中断和异常有什么区别？

**答案**：

| 类型 | 来源 | 同步/异步 | 示例 |
|------|------|-----------|------|
| **中断** | 外部硬件 | 异步 | 键盘、网卡、定时器 |
| **异常** | CPU 内部 | 同步 | 除零、缺页、断点 |
| **陷阱** | 软件触发 | 同步 | 系统调用 |

```
中断/异常处理流程：

              中断向量表 (IDT)
              ┌────────────┐
           0  │ 除法错误   │──► handler_divide_error()
           1  │ 调试异常   │──► handler_debug()
           ...│    ...     │
          14  │ 缺页异常   │──► handler_page_fault()
          ...│    ...     │
          32  │ 定时器中断 │──► handler_timer()
          33  │ 键盘中断   │──► handler_keyboard()
          ...│    ...     │
         128  │ 系统调用   │──► handler_syscall() (int 0x80)
              └────────────┘

中断发生时：
1. CPU 停止当前指令
2. 根据中断号查找 IDT
3. 切换到内核态
4. 保存上下文到内核栈
5. 执行中断处理程序
6. 恢复上下文
7. 返回用户态
```

---

### 2.2 中断处理

```c
// Linux 中断处理示例（简化）

// 注册中断处理程序
int request_irq(unsigned int irq,
                irq_handler_t handler,
                unsigned long flags,
                const char *name,
                void *dev);

// 中断处理函数
irqreturn_t my_interrupt_handler(int irq, void *dev_id) {
    // 快速处理，不能睡眠
    // 分为上半部（快速）和下半部（可延迟）

    // 上半部：最小化处理
    acknowledge_interrupt();
    schedule_work(&my_work);  // 调度下半部

    return IRQ_HANDLED;
}

// 下半部：workqueue
void my_work_handler(struct work_struct *work) {
    // 可以睡眠，执行耗时操作
}
```

---

## 3. 系统调用机制

### 3.1 系统调用原理

**面试问题**：系统调用是如何实现的？

**答案**：

**系统调用入口**（x86-64 Linux）：

```
用户程序调用 write()：

1. C 库包装函数：
   glibc: write() → syscall(__NR_write, ...)

2. 触发系统调用：
   x86-64: syscall 指令（比 int 0x80 快）
   x86-32: int 0x80 软中断

3. 进入内核：
   - 保存用户态寄存器
   - 切换到内核栈
   - 根据系统调用号调用对应处理函数

4. 执行内核函数：
   sys_write() → vfs_write() → ...

5. 返回用户态：
   - 恢复用户态寄存器
   - sysret/iret 指令返回
```

**x86-64 系统调用寄存器约定**：

| 寄存器 | 用途 |
|--------|------|
| `rax` | 系统调用号 / 返回值 |
| `rdi` | 第 1 个参数 |
| `rsi` | 第 2 个参数 |
| `rdx` | 第 3 个参数 |
| `r10` | 第 4 个参数 |
| `r8` | 第 5 个参数 |
| `r9` | 第 6 个参数 |

---

### 3.2 系统调用表

```c
// Linux 系统调用表（简化）

// 系统调用号定义 (arch/x86/include/generated/asm/syscalls_64.h)
#define __NR_read    0
#define __NR_write   1
#define __NR_open    2
#define __NR_close   3
#define __NR_stat    4
#define __NR_fstat   5
#define __NR_mmap    9
#define __NR_fork    57
#define __NR_execve  59
#define __NR_exit    60
// ... 共约 300+ 个

// 系统调用表
const sys_call_ptr_t sys_call_table[] = {
    [0] = sys_read,
    [1] = sys_write,
    [2] = sys_open,
    [3] = sys_close,
    // ...
};
```

---

### 3.3 系统调用完整流程

```
用户程序                      glibc                       内核

write(fd, buf, n)
     │
     └─────────────────► write()
                            │
                            │  准备寄存器：
                            │  rax = __NR_write (1)
                            │  rdi = fd
                            │  rsi = buf
                            │  rdx = n
                            │
                            │  syscall 指令
                            │
                            └───────────────────► entry_SYSCALL_64
                                                       │
                                                       │ 保存寄存器
                                                       │ 切换到内核栈
                                                       ▼
                                               sys_call_table[rax]
                                                       │
                                                       ▼
                                                  sys_write()
                                                       │
                                                       ▼
                                                  vfs_write()
                                                       │
                                                       ▼
                                              file->f_op->write()
                                                       │
                                                       │ 实际写入
                                                       │
                                               sysret 返回
                                                       │
     ◄────────────── 返回值 ─────────────────────────┘
     │
     │ 检查 errno
     ▼
   结果
```

---

### 3.4 手动调用系统调用

```c
#include <unistd.h>
#include <sys/syscall.h>

// 方法 1：使用 syscall() 函数
ssize_t my_write(int fd, const void *buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
}

// 方法 2：内联汇编（x86-64）
ssize_t my_write_asm(int fd, const void *buf, size_t count) {
    ssize_t ret;
    asm volatile (
        "syscall"
        : "=a" (ret)
        : "a" (__NR_write), "D" (fd), "S" (buf), "d" (count)
        : "rcx", "r11", "memory"
    );
    return ret;
}

int main() {
    const char *msg = "Hello, syscall!\n";
    my_write(1, msg, 16);
    my_write_asm(1, msg, 16);
    return 0;
}
```

---

## 4. 常用系统调用

### 4.1 进程控制

| 系统调用 | 功能 |
|----------|------|
| `fork()` | 创建子进程 |
| `exec*()` | 执行新程序 |
| `exit()` | 终止进程 |
| `wait()` | 等待子进程 |
| `getpid()` | 获取进程 ID |
| `kill()` | 发送信号 |

```c
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程
        execl("/bin/ls", "ls", "-l", NULL);
        _exit(1);  // exec 失败
    } else {
        // 父进程
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("子进程退出码: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
```

---

### 4.2 文件操作

| 系统调用 | 功能 |
|----------|------|
| `open()` | 打开文件 |
| `read()` | 读取数据 |
| `write()` | 写入数据 |
| `close()` | 关闭文件 |
| `lseek()` | 移动文件指针 |
| `stat()` | 获取文件信息 |
| `dup()` | 复制文件描述符 |

```c
#include <fcntl.h>
#include <unistd.h>

int main() {
    // 打开文件
    int fd = open("test.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 写入
    const char *msg = "Hello, World!\n";
    write(fd, msg, strlen(msg));

    // 重置位置
    lseek(fd, 0, SEEK_SET);

    // 读取
    char buf[100];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("读取: %s", buf);

    close(fd);
    return 0;
}
```

---

### 4.3 内存管理

| 系统调用 | 功能 |
|----------|------|
| `brk()` | 调整堆大小 |
| `mmap()` | 内存映射 |
| `munmap()` | 解除映射 |
| `mprotect()` | 修改内存保护 |

```c
#include <sys/mman.h>

int main() {
    // 匿名映射（类似 malloc）
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // 使用内存
    strcpy(ptr, "Hello, mmap!");
    printf("%s\n", (char *)ptr);

    // 解除映射
    munmap(ptr, 4096);

    return 0;
}
```

---

### 4.4 网络操作

| 系统调用 | 功能 |
|----------|------|
| `socket()` | 创建套接字 |
| `bind()` | 绑定地址 |
| `listen()` | 监听连接 |
| `accept()` | 接受连接 |
| `connect()` | 发起连接 |
| `send()/recv()` | 发送/接收数据 |

```c
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    // 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 绑定地址
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = INADDR_ANY
    };
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    // 监听
    listen(sockfd, SOMAXCONN);

    // 接受连接
    int clientfd = accept(sockfd, NULL, NULL);

    // 读取数据
    char buf[1024];
    recv(clientfd, buf, sizeof(buf), 0);

    close(clientfd);
    close(sockfd);
    return 0;
}
```

---

### 4.5 时间与定时器

| 系统调用 | 功能 |
|----------|------|
| `time()` | 获取时间 |
| `gettimeofday()` | 获取精确时间 |
| `clock_gettime()` | 获取时钟时间 |
| `nanosleep()` | 纳秒级睡眠 |
| `alarm()` | 设置定时器 |

```c
#include <time.h>
#include <sys/time.h>

int main() {
    // 获取时间戳
    time_t t = time(NULL);
    printf("time: %ld\n", t);

    // 获取精确时间
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("gettimeofday: %ld.%06ld\n", tv.tv_sec, tv.tv_usec);

    // 高精度时钟
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    printf("clock_gettime: %ld.%09ld\n", ts.tv_sec, ts.tv_nsec);

    // 睡眠 100 毫秒
    struct timespec req = {.tv_sec = 0, .tv_nsec = 100000000};
    nanosleep(&req, NULL);

    return 0;
}
```

---

## 5. 系统调用性能

### 5.1 系统调用开销

**面试问题**：系统调用为什么有开销？

**答案**：

```
系统调用开销来源：

1. 模式切换
   - 保存用户态寄存器
   - 切换到内核栈
   - 改变 CPU 特权级

2. 上下文切换
   - TLB 刷新（某些情况）
   - Cache 污染

3. 安全检查
   - 参数验证
   - 权限检查

4. 数据拷贝
   - 用户空间 ↔ 内核空间

典型开销：
- 简单系统调用：100-300 CPU 周期
- 复杂系统调用：可能更多
```

---

### 5.2 减少系统调用

```c
// 方法 1：批量操作
// 差：多次系统调用
for (int i = 0; i < 1000; i++) {
    write(fd, &data[i], sizeof(data[i]));
}

// 好：一次系统调用
write(fd, data, sizeof(data));


// 方法 2：使用缓冲区
// 差：直接系统调用
write(fd, buf, n);

// 好：使用标准库缓冲
fprintf(fp, "%s", str);  // 内部缓冲


// 方法 3：使用 mmap 减少 read/write
void *mapped = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
// 直接访问内存，减少系统调用


// 方法 4：使用 sendfile 减少用户态拷贝
sendfile(sockfd, filefd, NULL, size);  // 零拷贝
```

---

### 5.3 vDSO (Virtual Dynamic Shared Object)

**面试问题**：什么是 vDSO？

**答案**：

**vDSO**：内核将某些系统调用的代码映射到用户空间，避免模式切换。

```
传统系统调用：
用户态 ──► 内核态 ──► 返回用户态
      syscall      sysret

vDSO：
用户态直接执行（无需切换）

支持的函数：
- gettimeofday()
- time()
- clock_gettime()
- getcpu()
```

```c
// 查看 vDSO
// cat /proc/self/maps | grep vdso
// 7fff12345000-7fff12346000 r-xp ... [vdso]

// vDSO 函数直接在用户态执行
struct timeval tv;
gettimeofday(&tv, NULL);  // 通过 vDSO，无需系统调用
```

---

## 6. 系统调用跟踪

### 6.1 strace 工具

```bash
# 跟踪系统调用
strace ls

# 跟踪指定系统调用
strace -e read,write ls

# 统计系统调用
strace -c ls

# 跟踪正在运行的进程
strace -p PID

# 输出时间戳
strace -t ls

# 跟踪子进程
strace -f ./program
```

```
示例输出：

execve("/bin/ls", ["ls"], 0x7ffd...) = 0
brk(NULL)                           = 0x55a8...
mmap(NULL, 4096, ...)               = 0x7f3c...
access("/etc/ld.so.preload", R_OK)  = -1 ENOENT
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", ...) = 3
read(3, "\177ELF\2\1\1\3\0\0..."..., 832) = 832
...
write(1, "file1.txt  file2.txt\n", 21) = 21
close(1)                            = 0
exit_group(0)                       = ?
```

---

### 6.2 ltrace 工具

```bash
# 跟踪库函数调用
ltrace ls

# 输出：
puts("file1.txt")                          = 10
puts("file2.txt")                          = 10
```

---

## 7. 面试常见问题

### 7.1 高频问题汇总

**Q1：用户态和内核态的区别？**
- 用户态：权限受限，只能执行非特权指令
- 内核态：完全权限，可执行所有指令和访问所有内存

**Q2：什么时候会发生态切换？**
- 系统调用（主动）
- 异常（如缺页、除零）
- 中断（如定时器、I/O）

**Q3：系统调用和普通函数调用的区别？**
- 系统调用需要态切换，开销大
- 普通函数调用仅涉及栈操作
- 系统调用通过特殊指令触发

**Q4：如何减少系统调用开销？**
- 批量操作
- 使用缓冲区
- mmap 替代 read/write
- vDSO 快速路径

**Q5：中断和异常的区别？**
- 中断：外部异步，来自硬件
- 异常：内部同步，来自 CPU 执行

**Q6：为什么进程有两个栈？**
- 用户栈：用户态使用
- 内核栈：内核态使用
- 隔离安全，防止溢出

---

## 8. 深入原理

### 8.1 系统调用入口代码

```c
// Linux x86-64 系统调用入口（简化）

// arch/x86/entry/entry_64.S
SYM_CODE_START(entry_SYSCALL_64)
    // 保存用户态栈指针
    swapgs
    movq    %rsp, PER_CPU_VAR(cpu_tss_rw + TSS_sp2)
    movq    PER_CPU_VAR(cpu_current_top_of_stack), %rsp

    // 保存用户态寄存器
    pushq   $__USER_DS          // ss
    pushq   PER_CPU_VAR(cpu_tss_rw + TSS_sp2)  // rsp
    pushq   %r11                // rflags
    pushq   $__USER_CS          // cs
    pushq   %rcx                // rip

    // 保存其他寄存器
    PUSH_AND_CLEAR_REGS

    // 调用系统调用处理函数
    movq    %rax, %rdi
    call    do_syscall_64

    // 恢复并返回
    POP_REGS
    swapgs
    sysretq
SYM_CODE_END(entry_SYSCALL_64)
```

```c
// 系统调用分发
__visible noinstr bool do_syscall_64(struct pt_regs *regs, int nr) {
    if (likely(nr < NR_syscalls)) {
        nr = array_index_nospec(nr, NR_syscalls);
        regs->ax = sys_call_table[nr](regs);
    }
    return true;
}
```

---

### 8.2 添加自定义系统调用

```c
// 1. 定义系统调用号
// arch/x86/entry/syscalls/syscall_64.tbl
// 添加一行：
// 548  common  my_syscall  sys_my_syscall

// 2. 实现系统调用
// kernel/my_syscall.c
#include <linux/syscalls.h>

SYSCALL_DEFINE1(my_syscall, int, arg) {
    printk(KERN_INFO "my_syscall called with arg=%d\n", arg);
    return arg * 2;
}

// 3. 用户空间调用
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_my_syscall 548

int main() {
    int result = syscall(__NR_my_syscall, 21);
    printf("Result: %d\n", result);  // 输出：42
    return 0;
}
```

---

## 9. 参考资料

- 《深入理解 Linux 内核》第 10 章
- 《Linux 内核设计与实现》
- Linux 内核源码：`arch/x86/entry/`
- man 2（系统调用手册）
