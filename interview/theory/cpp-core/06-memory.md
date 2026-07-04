 # C++ 内存管理 (Memory Management)

## 概述

本文档涵盖 C++ 内存管理的核心知识，包括堆栈内存、动态内存分配（new/delete）、智能指针、RAII 原则、内存泄漏、内存对齐等重要概念。

---

## 1. 内存布局

### 1.1 程序的内存分区

**问题：C++ 程序的内存是如何组织的？**

```cpp
#include <iostream>

int global_var = 100;           // 全局变量：数据段（.data）
static int static_var = 200;    // 静态变量：数据段（.data）
const int const_var = 300;      // 常量：只读数据段（.rodata）

int uninitialized_global;       // 未初始化全局变量：BSS 段

void func() {
    static int static_local = 400;  // 静态局部变量：数据段
}

int main() {
    int local_var = 10;             // 局部变量：栈
    int* heap_var = new int(20);    // 动态分配：堆

    std::cout << "Stack address (local_var): " << &local_var << std::endl;
    std::cout << "Heap address (heap_var): " << heap_var << std::endl;
    std::cout << "Data segment (global_var): " << &global_var << std::endl;
    std::cout << "BSS segment (uninitialized): " << &uninitialized_global << std::endl;
    std::cout << "Text segment (function): " << (void*)&func << std::endl;

    delete heap_var;
    return 0;
}

/*
内存布局（从低地址到高地址）：
+------------------+
| Text Segment     | 代码段：存放程序指令
+------------------+
| .rodata          | 只读数据段：const 常量、字符串字面量
+------------------+
| .data            | 数据段：已初始化的全局/静态变量
+------------------+
| .bss             | BSS 段：未初始化的全局/静态变量（自动初始化为 0）
+------------------+
| Heap             | 堆：动态分配的内存（向上增长）
|     ↑            |
+------------------+
|     ↓            |
| Stack            | 栈：局部变量、函数调用（向下增长）
+------------------+
| Kernel Space     | 内核空间
+------------------+
*/
```

**关键要点：**
- **栈 (Stack)**：自动管理，速度快，空间有限（通常几 MB）
- **堆 (Heap)**：手动管理，灵活，空间大，速度较慢
- **数据段 (.data)**：已初始化的全局/静态变量
- **BSS 段**：未初始化的全局/静态变量（自动初始化为 0）
- **代码段 (Text)**：程序指令，只读

**常见陷阱：**
- 混淆栈和堆的特性
- 栈空间有限，大对象或递归过深会导致栈溢出

---

### 1.2 各内存区域可用大小

**问题：进程内存布局中各区域的可用大小是多少？**

各内存区域的大小因操作系统、编译器和系统配置而异：

| 区域 | Windows (默认) | Linux (默认) | 特点 |
|------|---------------|--------------|------|
| **栈 (Stack)** | 1 MB | 8 MB | 可调整，存放局部变量、函数调用 |
| **堆 (Heap)** | ~2 GB (32位) / ~128 TB (64位) | 受物理内存+swap限制 | 动态分配，最大可用区域 |
| **全局/静态区** | 受可执行文件大小限制 | 同左 | .data + .bss 段 |
| **代码区 (Text)** | 受可执行文件大小限制 | 同左 | 只读，存放指令 |
| **常量区** | 包含在代码区或只读数据段 | 同左 | 字符串字面量等 |

**查看和修改栈大小：**

```cpp
// Windows: 编译时设置栈大小
// cl /F 4194304 main.cpp          // 设置为 4MB
// 或链接时: /STACK:4194304

// Linux: 运行时查看和设置
// ulimit -s                        // 查看栈大小 (KB)
// ulimit -s 16384                  // 设置为 16MB

// Linux: 编译时设置
// g++ -Wl,--stack,4194304 main.cpp

// 代码中获取栈大小（Linux）
#include <sys/resource.h>

void printStackLimit() {
    struct rlimit limit;
    getrlimit(RLIMIT_STACK, &limit);
    std::cout << "Stack soft limit: " << limit.rlim_cur / 1024 << " KB\n";
    std::cout << "Stack hard limit: " << limit.rlim_max / 1024 << " KB\n";
}
```

**测试各区域大小的代码：**

```cpp
#include <iostream>
#include <vector>
#include <cstdlib>

// 测试堆大小
void testHeapSize() {
    size_t total = 0;
    std::vector<void*> blocks;

    while (true) {
        void* p = malloc(1024 * 1024);  // 每次 1MB
        if (!p) break;
        blocks.push_back(p);
        total += 1024 * 1024;
    }

    std::cout << "Max heap allocated: " << total / (1024 * 1024) << " MB\n";

    for (void* p : blocks) free(p);
}

// 测试栈大小（通过递归深度估算）
void testStackDepth(int depth, int& maxDepth) {
    char buffer[1024];  // 每次递归占用约 1KB+
    maxDepth = depth;
    testStackDepth(depth + 1, maxDepth);  // 会栈溢出
}

int main() {
    // 测试堆
    testHeapSize();

    // 测试栈（会崩溃，仅演示）
    // int depth = 0;
    // testStackDepth(0, depth);
    // std::cout << "Stack depth: ~" << depth << " KB\n";

    return 0;
}
```

**关键要点：**

| 要点 | 说明 |
|------|------|
| 栈空间有限 | 大数组应放堆上，避免栈溢出 |
| 堆几乎无限 | 受物理内存 + 虚拟内存限制 |
| 64位更大 | 地址空间从 4GB 扩展到 TB 级别 |
| 可配置 | 栈大小可通过编译器/系统参数调整 |

**常见问题：**

```cpp
// 1. 栈溢出：大数组放栈上
void bad() {
    int arr[1000000];  // ~4MB，可能栈溢出
}

void good() {
    std::vector<int> arr(1000000);  // 堆上分配，安全
    // 或
    static int arr2[1000000];  // 静态区，不占用栈
}

// 2. 深递归导致栈溢出
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);  // 深递归风险
}

// 解决方案：
// - 改用迭代
// - 使用尾递归优化
// - 增大栈空间
// - 使用显式栈（std::stack）模拟递归
```

**32位 vs 64位进程内存空间：**

```
32位进程（4GB 虚拟地址空间）：
┌─────────────────┐ 0xFFFFFFFF (4GB)
│   Kernel (1GB)  │
├─────────────────┤ 0xC0000000 (3GB)
│                 │
│   User Space    │  ← 最多 3GB 可用
│   (Stack/Heap)  │
│                 │
└─────────────────┘ 0x00000000

64位进程（128TB+ 虚拟地址空间）：
┌─────────────────┐ 高地址
│   Kernel Space  │
├─────────────────┤
│                 │
│   User Space    │  ← 理论上 128TB+ 可用
│   (巨大空间)     │     实际受物理内存限制
│                 │
└─────────────────┘ 低地址
```

**常见陷阱：**
- 在 32 位系统上分配超过 2GB 内存
- 不了解栈大小限制导致栈溢出
- 递归深度过大未做保护

---

### 1.3 内核空间与用户空间

**问题：什么是内核空间和用户空间？每个进程是否都占用这两部分内存？**

操作系统将进程的虚拟地址空间划分为两部分：

| 区域 | 说明 | 访问权限 |
|------|------|----------|
| **内核空间 (Kernel Space)** | 操作系统内核代码和数据 | 仅内核态可访问 |
| **用户空间 (User Space)** | 应用程序代码和数据 | 用户态可访问 |

**各系统的划分：**

| 系统 | 用户空间 | 内核空间 |
|------|----------|----------|
| **Linux 32位 (默认)** | 3 GB (0x00000000 - 0xBFFFFFFF) | 1 GB (0xC0000000 - 0xFFFFFFFF) |
| **Windows 32位 (默认)** | 2 GB (0x00000000 - 0x7FFFFFFF) | 2 GB (0x80000000 - 0xFFFFFFFF) |
| **Linux 64位** | 128 TB | 128 TB |
| **Windows 64位** | 8-128 TB | 8-128 TB |

**每个进程是否都占用两部分？**

**是的**，每个运行的 C++ 程序（进程）都同时占用两部分内存：

| 空间 | 占用方式 | 说明 |
|------|----------|------|
| **用户空间** | **独占** | 每个进程有自己独立的用户空间 |
| **内核空间** | **共享** | 所有进程共享同一个内核空间 |

```
进程 A                    进程 B                    进程 C
┌─────────────┐          ┌─────────────┐          ┌─────────────┐
│ 用户空间 A   │          │ 用户空间 B   │          │ 用户空间 C   │
│ (独立)      │          │ (独立)      │          │ (独立)      │
│ 代码/堆/栈   │          │ 代码/堆/栈   │          │ 代码/堆/栈   │
├─────────────┤          ├─────────────┤          ├─────────────┤
│             │          │             │          │             │
│  内核空间    │◄─────────┴─────────────┴──────────►│             │
│  (共享)     │         所有进程共享同一份           │             │
└─────────────┘                                    └─────────────┘
```

**进程在内核空间占用的资源：**

虽然内核代码共享，但每个进程会占用内核空间中的一些专属资源：

```cpp
// 每个进程在内核空间的开销（约 10-100 KB）：
struct 进程内核开销 {
    task_struct  pcb;           // 进程控制块，约 1-10 KB
    char         kernel_stack[8192];  // 内核栈，8-16 KB
    pgd_t*       page_table;    // 页表，大小取决于内存使用量
    files_struct file_table;    // 打开的文件描述符
    signal_struct signals;      // 信号处理结构
};
```

**用户态与内核态切换：**

```cpp
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int global_var = 42;  // 用户空间：数据段

int main() {
    int local_var = 10;           // 用户空间：栈
    int* heap_var = new int(20);  // 用户空间：堆

    // 以上都在用户空间执行

    // 系统调用 → 切换到内核态
    pid_t pid = getpid();  // 陷入内核获取 PID

    int fd = open("file.txt", O_RDONLY);  // 陷入内核打开文件
    if (fd >= 0) {
        char buf[64];
        read(fd, buf, sizeof(buf));  // 陷入内核读取数据
        close(fd);  // 陷入内核关闭文件
    }

    delete heap_var;
    return 0;
}

/*
执行流程：
┌──────────────┐
│  用户态代码   │ ← 普通计算、内存访问
└──────┬───────┘
       │ open() 系统调用
       ▼
┌──────────────┐
│  内核态代码   │ ← 检查权限、分配 fd、访问磁盘
└──────┬───────┘
       │ 返回
       ▼
┌──────────────┐
│  用户态代码   │ ← 继续执行
└──────────────┘
*/
```

**为什么需要划分？**

| 原因 | 说明 |
|------|------|
| **安全隔离** | 防止用户程序破坏内核 |
| **权限控制** | 敏感操作需通过系统调用 |
| **稳定性** | 用户程序崩溃不影响内核 |
| **内存保护** | 硬件 MMU 强制执行边界 |

**关键要点：**

| 问题 | 答案 |
|------|------|
| 用户空间是否独立？ | **是**，每个进程有独立的用户空间 |
| 内核空间是否独立？ | **否**，所有进程共享同一内核空间 |
| 进程能访问其他进程用户空间吗？ | **不能**（除非共享内存） |
| 进程能直接访问内核空间吗？ | **不能**，必须通过系统调用 |
| 每个进程占用多少内核资源？ | 约 **10-100 KB** |

**常见面试题：**
- 为什么 32 位系统最大只能用约 3GB 内存？（4GB 中 1GB 给内核）
- 为什么内核空间所有进程共享？（节省内存，方便通信）
- 用户态如何访问内核功能？（通过系统调用 syscall）

**常见陷阱：**
- 试图在用户态直接访问内核地址（导致段错误）
- 不理解系统调用的开销（频繁调用影响性能）
- 混淆虚拟地址空间和物理内存

---

### 1.4 标准库 I/O 缓冲区

**问题：标准库输入输出的缓冲区大小是多少？每个进程独占还是共享？**

| 问题 | 答案 |
|------|------|
| 缓冲区大小 | 通常 **4KB - 8KB**（平台相关） |
| 是否独占 | **每个进程独占自己的缓冲区** |
| 存放位置 | 用户空间（堆或静态区） |

**查看缓冲区大小：**

```cpp
#include <iostream>
#include <cstdio>

int main() {
    // 查看 C 标准库缓冲区大小
    std::cout << "BUFSIZ: " << BUFSIZ << " bytes\n";
    // Linux 通常: 8192 (8KB)
    // Windows 通常: 512 或 4096

    return 0;
}
```

**各平台默认大小：**

| 平台 | BUFSIZ | 说明 |
|------|--------|------|
| Linux (glibc) | 8192 (8KB) | 可配置 |
| Windows (MSVC) | 512 或 4096 | 取决于版本 |
| macOS | 1024 或 8192 | 取决于版本 |

**每个进程独占各自的缓冲区：**

```
进程 A                              进程 B
┌─────────────────────────┐        ┌─────────────────────────┐
│ 用户空间                 │        │ 用户空间                 │
│                         │        │                         │
│  stdin  缓冲区 [8KB]     │        │  stdin  缓冲区 [8KB]     │
│  stdout 缓冲区 [8KB]     │        │  stdout 缓冲区 [8KB]     │
│  stderr 缓冲区 [无]      │        │  stderr 缓冲区 [无]      │
│                         │        │                         │
│  FILE* f1 缓冲区 [8KB]   │        │  FILE* f2 缓冲区 [8KB]   │
└─────────────────────────┘        └─────────────────────────┘
        ↓                                  ↓
        └──────────┬───────────────────────┘
                   ▼
          ┌─────────────────┐
          │   内核空间       │
          │  (文件系统缓存)   │  ← 内核缓冲区可能共享
          └─────────────────┘
```

**三种缓冲模式：**

```cpp
#include <cstdio>

int main() {
    FILE* f = fopen("test.txt", "w");

    // 1. 全缓冲 (_IOFBF)：缓冲区满或手动刷新时写入
    char buf1[4096];
    setvbuf(f, buf1, _IOFBF, sizeof(buf1));

    // 2. 行缓冲 (_IOLBF)：遇到换行符或缓冲区满时写入
    char buf2[1024];
    setvbuf(f, buf2, _IOLBF, sizeof(buf2));

    // 3. 无缓冲 (_IONBF)：立即写入
    setvbuf(f, nullptr, _IONBF, 0);

    fclose(f);
    return 0;
}
```

**默认缓冲模式：**

| 流 | 默认模式 | 原因 |
|----|----------|------|
| `stdin` | 行缓冲（终端）/ 全缓冲（文件） | 交互需要及时响应 |
| `stdout` | 行缓冲（终端）/ 全缓冲（文件） | 同上 |
| `stderr` | **无缓冲** | 错误信息需立即显示 |
| 普通文件 | 全缓冲 | 提高 I/O 效率 |

**C++ iostream 缓冲区：**

```cpp
#include <iostream>

int main() {
    // cout 有自己的缓冲区（streambuf）
    std::streambuf* buf = std::cout.rdbuf();

    // 手动刷新缓冲区
    std::cout << "Hello" << std::flush;  // 方式1
    std::cout << "World" << std::endl;   // 方式2：endl = '\n' + flush

    // 关闭与 C stdio 的同步可提高性能
    std::ios_base::sync_with_stdio(false);

    return 0;
}
```

**fork 时的缓冲区问题：**

```cpp
#include <cstdio>
#include <unistd.h>

int main() {
    printf("Before fork");  // 注意：没有 \n，数据在缓冲区中

    pid_t pid = fork();
    if (pid == 0) {
        printf(" - Child\n");
    } else {
        printf(" - Parent\n");
    }
    return 0;
}

/*
可能输出：
Before fork - Parent
Before fork - Child    ← "Before fork" 被输出两次！

原因：fork 时缓冲区中的 "Before fork" 被复制到子进程
解决：fork 前调用 fflush(stdout)
*/
```

**关键要点：**

| 要点 | 说明 |
|------|------|
| 缓冲区大小 | 通常 4-8 KB，由 BUFSIZ 宏定义 |
| 进程隔离 | 每个进程独占自己的 I/O 缓冲区 |
| 存放位置 | 用户空间（堆或静态区） |
| fork 问题 | 子进程会复制父进程的缓冲区内容 |
| stderr | 默认无缓冲，错误立即输出 |

**常见陷阱：**
- fork 前忘记刷新缓冲区（导致输出重复）
- cout 和 printf 混用顺序错乱（需关闭同步或只用一种）
- 程序崩溃时缓冲区数据丢失（关键日志用 cerr 或手动刷新）

---

## 2. 栈内存 (Stack)

### 2.1 栈的特性

**问题：栈内存有什么特点？如何使用？**

```cpp
#include <iostream>

void func() {
    int a = 10;       // 栈上分配
    int arr[1000];    // 栈上分配数组

    std::cout << "Inside func, a = " << a << std::endl;
}  // a 和 arr 在这里自动销毁

class MyClass {
public:
    MyClass() { std::cout << "Constructor\n"; }
    ~MyClass() { std::cout << "Destructor\n"; }
};

int main() {
    {
        MyClass obj;  // 栈上创建对象
        // ...
    }  // obj 离开作用域，自动调用析构函数

    func();
    // func 返回后，a 和 arr 的内存自动回收

    return 0;
}
```

**关键要点：**
- 栈内存自动管理，遵循 LIFO (Last In First Out) 原则
- 变量离开作用域时自动销毁
- 速度快（指针移动即可分配/释放）
- 空间有限（通常 1-8 MB，取决于系统）
- 适合小对象和临时变量

**常见陷阱：**
- 返回局部变量的指针/引用（悬垂指针）
- 递归过深导致栈溢出
- 在栈上分配大数组导致栈溢出

---

### 2.2 栈溢出

**问题：什么是栈溢出？如何避免？**

```cpp
// 错误示例1：递归过深
void infiniteRecursion() {
    infiniteRecursion();  // 无限递归，栈溢出
}

// 错误示例2：栈上分配大对象
void largeArray() {
    int huge_array[10000000];  // 可能导致栈溢出
}

// 正确做法1：使用尾递归优化或迭代
int factorial(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial(n - 1, n * acc);  // 尾递归
}

// 正确做法2：大对象放在堆上
void largeArrayHeap() {
    int* huge_array = new int[10000000];  // 堆上分配
    // ...
    delete[] huge_array;
}

// 正确做法3：使用容器
void largeVector() {
    std::vector<int> vec(10000000);  // 数据在堆上
}
```

**关键要点：**
- 栈溢出原因：递归过深、大对象、大数组
- 解决方法：使用堆、减少递归深度、使用容器

**常见陷阱：**
- 不检查递归终止条件
- 低估栈空间限制

---

## 3. 堆内存 (Heap)

### 3.1 new 和 delete

**问题：如何使用 new 和 delete 进行动态内存分配？**

```cpp
#include <iostream>

int main() {
    // 1. 分配单个对象
    int* p1 = new int;           // 未初始化
    int* p2 = new int(42);       // 初始化为 42
    int* p3 = new int{100};      // C++11 统一初始化

    delete p1;
    delete p2;
    delete p3;

    // 2. 分配数组
    int* arr1 = new int[10];           // 未初始化
    int* arr2 = new int[10]{};         // 值初始化（全为 0）
    int* arr3 = new int[5]{1, 2, 3, 4, 5};  // 列表初始化

    delete[] arr1;  // 注意：必须使用 delete[]
    delete[] arr2;
    delete[] arr3;

    // 3. 分配对象
    class MyClass {
    public:
        MyClass(int x) : x_(x) {
            std::cout << "Constructor: " << x_ << std::endl;
        }
        ~MyClass() {
            std::cout << "Destructor: " << x_ << std::endl;
        }
    private:
        int x_;
    };

    MyClass* obj = new MyClass(10);  // 调用构造函数
    delete obj;  // 调用析构函数

    // 4. 分配对象数组
    MyClass* obj_arr = new MyClass[3]{MyClass(1), MyClass(2), MyClass(3)};
    delete[] obj_arr;  // 依次调用每个对象的析构函数

    return 0;
}
```

**关键要点：**
- `new` 分配内存并调用构造函数
- `delete` 调用析构函数并释放内存
- `new[]` 和 `delete[]` 用于数组
- `new` 失败时抛出 `std::bad_alloc` 异常（除非使用 `new(std::nothrow)`）

**常见陷阱：**
- `new[]` 配 `delete` 或 `new` 配 `delete[]`（未定义行为）
- `new` 后忘记 `delete`（内存泄漏）
- 重复 `delete`（double free）

---

### 3.2 new 和 malloc 的区别

**问题：new/delete 和 malloc/free 有什么区别？**

```cpp
#include <cstdlib>
#include <iostream>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
private:
    int x_;
};

int main() {
    // 1. malloc/free：只分配/释放内存，不调用构造/析构函数
    MyClass* p1 = (MyClass*)malloc(sizeof(MyClass));
    // 没有调用构造函数！对象未初始化
    free(p1);
    // 没有调用析构函数！

    // 2. new/delete：分配内存 + 调用构造/析构函数
    MyClass* p2 = new MyClass(10);  // 调用构造函数
    delete p2;  // 调用析构函数

    // 3. malloc 需要手动计算大小，new 自动
    int* arr1 = (int*)malloc(10 * sizeof(int));
    int* arr2 = new int[10];

    free(arr1);
    delete[] arr2;

    // 4. new 失败抛出异常，malloc 返回 nullptr
    try {
        int* huge = new int[1000000000000];  // 抛出 std::bad_alloc
    } catch (const std::bad_alloc& e) {
        std::cout << "Allocation failed: " << e.what() << std::endl;
    }

    int* huge2 = (int*)malloc(1000000000000 * sizeof(int));
    if (huge2 == nullptr) {
        std::cout << "malloc failed" << std::endl;
    }

    return 0;
}
```

**关键要点：**

| 特性 | new/delete | malloc/free |
|------|------------|-------------|
| 类型 | 操作符 | 函数 |
| 构造/析构 | 调用 | 不调用 |
| 类型安全 | 有 | 无（需要转型） |
| 大小计算 | 自动 | 手动 |
| 失败处理 | 抛异常 | 返回 nullptr |
| 重载 | 可以 | 不可以 |
| 使用场景 | C++ 对象 | C 兼容、POD 类型 |

**常见陷阱：**
- 混用 `new`/`free` 或 `malloc`/`delete`（未定义行为）
- 对 C++ 对象使用 `malloc`/`free`

---

### 3.3 placement new

**问题：什么是 placement new？何时使用？**

```cpp
#include <iostream>
#include <new>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
    void print() {
        std::cout << "x = " << x_ << std::endl;
    }
private:
    int x_;
};

int main() {
    // 1. 在预分配的内存上构造对象
    char buffer[sizeof(MyClass)];  // 预分配内存

    // placement new：在指定位置构造对象
    MyClass* obj = new (buffer) MyClass(42);
    obj->print();

    // 必须显式调用析构函数
    obj->~MyClass();
    // 不需要 delete（内存不是通过 new 分配的）

    // 2. 用途：内存池
    const size_t pool_size = 1024;
    char* memory_pool = new char[pool_size];

    MyClass* obj1 = new (memory_pool) MyClass(1);
    MyClass* obj2 = new (memory_pool + sizeof(MyClass)) MyClass(2);

    obj1->~MyClass();
    obj2->~MyClass();

    delete[] memory_pool;

    return 0;
}
```

**关键要点：**
- Placement new 在指定内存位置构造对象，不分配新内存
- 用途：内存池、对象池、共享内存
- 必须显式调用析构函数
- 不能对 placement new 的对象使用 `delete`

**常见陷阱：**
- 忘记显式调用析构函数
- 对 placement new 对象使用 `delete`
- 内存对齐问题

---

## 4. 智能指针 (Smart Pointers)

### 4.1 为什么需要智能指针

**问题：为什么要使用智能指针？**

```cpp
#include <iostream>

// 问题1：忘记 delete 导致内存泄漏
void memoryLeak() {
    int* p = new int(42);
    // 忘记 delete p;
}  // 内存泄漏

// 问题2：异常导致资源未释放
void exceptionLeak() {
    int* p = new int(42);

    // 如果这里抛出异常，delete 不会执行
    if (some_condition) {
        throw std::runtime_error("Error");
    }

    delete p;  // 可能不会执行
}

// 问题3：多次 delete
void doubleFree() {
    int* p = new int(42);
    delete p;
    // delete p;  // 未定义行为
}

// 解决方案：智能指针（RAII）
void smartPointerSolution() {
    std::unique_ptr<int> p(new int(42));

    if (some_condition) {
        throw std::runtime_error("Error");
    }

    // 不需要 delete，智能指针自动释放
}  // p 离开作用域，自动释放内存
```

**关键要点：**
- 智能指针自动管理内存，避免内存泄漏
- 异常安全：无论如何退出作用域，都会释放资源
- 符合 RAII 原则

---

### 4.2 unique_ptr

**问题：unique_ptr 有什么特点？如何使用？**

```cpp
#include <memory>
#include <iostream>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
    void print() {
        std::cout << "x = " << x_ << std::endl;
    }
private:
    int x_;
};

int main() {
    // 1. 创建 unique_ptr
    std::unique_ptr<int> p1(new int(42));
    std::unique_ptr<int> p2 = std::make_unique<int>(100);  // C++14，推荐

    // 2. 访问对象
    std::cout << *p1 << std::endl;
    *p2 = 200;

    // 3. unique_ptr 独占所有权，不能拷贝
    // std::unique_ptr<int> p3 = p1;  // Error: 拷贝被禁用

    // 4. 可以移动
    std::unique_ptr<int> p3 = std::move(p1);  // p1 变为 nullptr
    if (p1 == nullptr) {
        std::cout << "p1 is now nullptr" << std::endl;
    }

    // 5. 管理数组
    std::unique_ptr<int[]> arr = std::make_unique<int[]>(10);
    arr[0] = 100;
    arr[1] = 200;

    // 6. 自定义删除器
    auto deleter = [](MyClass* p) {
        std::cout << "Custom deleter\n";
        delete p;
    };
    std::unique_ptr<MyClass, decltype(deleter)> p4(new MyClass(10), deleter);

    // 7. 手动释放
    std::unique_ptr<MyClass> p5 = std::make_unique<MyClass>(20);
    p5.reset();  // 立即释放
    p5.reset(new MyClass(30));  // 释放旧对象，管理新对象

    // 8. 获取原始指针
    std::unique_ptr<int> p6 = std::make_unique<int>(42);
    int* raw_ptr = p6.get();  // 不转移所有权
    std::cout << *raw_ptr << std::endl;

    // 9. 释放所有权
    int* raw_ptr2 = p6.release();  // p6 不再管理内存
    delete raw_ptr2;  // 必须手动 delete

    return 0;
}  // 所有 unique_ptr 离开作用域，自动释放资源
```

**关键要点：**
- `unique_ptr` 独占所有权，不能拷贝，只能移动
- 使用 `std::make_unique`（C++14）创建，更安全、更高效
- 零开销：和原始指针性能相同
- 适用场景：独占资源、函数返回值

**常见陷阱：**
- 尝试拷贝 `unique_ptr`
- 从 `get()` 获取的指针创建新的 `unique_ptr`（导致 double free）

---

### 4.2.1 new vs make_unique

**问题：`new` + 构造 和 `make_unique` 有什么区别？哪个效率高？哪个有风险？**

```cpp
#include <memory>
#include <iostream>

// 两种创建方式
std::unique_ptr<int> p1(new int(42));              // 方式1: new
std::unique_ptr<int> p2 = std::make_unique<int>(100);  // 方式2: make_unique (C++14)
```

**核心区别：**

| 对比项 | `new` + 构造 | `make_unique` |
|--------|-------------|---------------|
| 内存分配次数 | 1次 | 1次 |
| 异常安全 | **有风险** | 安全 |
| 代码简洁性 | 重复写类型 | 更简洁 |
| C++ 版本 | C++11 | C++14 |
| 自定义删除器 | 支持 | 不支持 |

**效率对比：基本相同**

对于 `unique_ptr`，两者效率几乎一样，都是一次内存分配：

```cpp
// 两者都只分配一次内存
std::unique_ptr<int> p1(new int(42));              // 分配 int
std::unique_ptr<int> p2 = std::make_unique<int>(100);  // 分配 int

// 注意：make_shared 比 new + shared_ptr 效率更高（1次 vs 2次分配）
// 但 make_unique 没有这个优势
```

**风险：`new` 有异常安全问题**

```cpp
void process(std::unique_ptr<int> a, std::unique_ptr<int> b);

// 危险！可能内存泄漏（C++17 前）
process(std::unique_ptr<int>(new int(1)),
        std::unique_ptr<int>(new int(2)));

// 安全
process(std::make_unique<int>(1),
        std::make_unique<int>(2));
```

**问题原因**（C++17 前）：

编译器可能按以下顺序执行：
1. `new int(1)` ✓ 成功，分配内存
2. `new int(2)` → 抛异常 ❌
3. 构造 `unique_ptr`（第一个 new 的结果）← 未执行

此时第一个 `new` 的内存泄漏！

```
执行顺序可能是：
┌─────────────────────────────────────┐
│ new int(1)                          │ ← 成功，分配内存
│ new int(2)                          │ ← 抛异常！
│ unique_ptr(第一个new的结果)          │ ← 未执行
│ unique_ptr(第二个new的结果)          │ ← 未执行
└─────────────────────────────────────┘
结果：第一个 new 的内存泄漏
```

> C++17 修复了函数参数求值顺序问题，但仍推荐用 `make_unique`。

**何时必须用 `new`：**

```cpp
// 1. 自定义删除器
auto deleter = [](int* p) {
    std::cout << "deleting\n";
    delete p;
};
std::unique_ptr<int, decltype(deleter)> p(new int(42), deleter);

// 2. 接管已有裸指针
int* raw = legacyFunction();
std::unique_ptr<int> p(raw);

// 3. 数组（C++14 前）
std::unique_ptr<int[]> arr(new int[10]);  // C++14 前
auto arr2 = std::make_unique<int[]>(10);  // C++14 起
```

**总结：**

| 结论 | 说明 |
|------|------|
| **效率** | 基本相同 |
| **安全性** | `make_unique` 更安全 |
| **推荐** | **优先使用 `make_unique`** |
| **例外** | 需要自定义删除器时用 `new` |

```cpp
// ✓ 推荐
auto p = std::make_unique<MyClass>(args...);

// ✗ 避免（除非必要）
std::unique_ptr<MyClass> p(new MyClass(args...));
```

---

### 4.3 shared_ptr

**问题：shared_ptr 如何实现共享所有权？**

```cpp
#include <memory>
#include <iostream>

class MyClass {
public:
    MyClass(int x) : x_(x) {
        std::cout << "Constructor: " << x_ << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor: " << x_ << std::endl;
    }
    int x_;
};

int main() {
    // 1. 创建 shared_ptr
    std::shared_ptr<MyClass> p1 = std::make_shared<MyClass>(42);
    std::cout << "use_count: " << p1.use_count() << std::endl;  // 1

    // 2. 拷贝：共享所有权，引用计数 +1
    std::shared_ptr<MyClass> p2 = p1;
    std::cout << "use_count: " << p1.use_count() << std::endl;  // 2
    std::cout << "use_count: " << p2.use_count() << std::endl;  // 2

    {
        std::shared_ptr<MyClass> p3 = p1;
        std::cout << "use_count: " << p1.use_count() << std::endl;  // 3
    }  // p3 离开作用域，引用计数 -1

    std::cout << "use_count: " << p1.use_count() << std::endl;  // 2

    // 3. reset：释放当前对象，引用计数 -1
    p2.reset();
    std::cout << "use_count: " << p1.use_count() << std::endl;  // 1

    // 4. 手动减少引用计数
    p1 = nullptr;
    std::cout << "Object destroyed" << std::endl;

    // 5. 循环引用问题（见下节）

    return 0;
}
```

**关键要点：**
- `shared_ptr` 使用引用计数实现共享所有权
- 拷贝时引用计数 +1，销毁时引用计数 -1
- 引用计数为 0 时自动释放资源
- 使用 `std::make_shared` 更高效（一次分配）
- 有开销：引用计数的空间和时间开销

**常见陷阱：**
- 循环引用导致内存泄漏（需要 `weak_ptr`）
- 不要从原始指针创建多个 `shared_ptr`

---

### 4.4 weak_ptr

**问题：weak_ptr 解决什么问题？如何使用？**

```cpp
#include <memory>
#include <iostream>

// 循环引用问题
class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() { std::cout << "A destroyed\n"; }
};

class B {
public:
    std::shared_ptr<A> a_ptr;  // 循环引用！
    ~B() { std::cout << "B destroyed\n"; }
};

void circularReference() {
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();

    a->b_ptr = b;  // a 持有 b
    b->a_ptr = a;  // b 持有 a

    // 循环引用：a 和 b 互相持有，引用计数不会为 0，内存泄漏！
}

// 解决方案：使用 weak_ptr
class B_fixed;

class A_fixed {
public:
    std::shared_ptr<B_fixed> b_ptr;
    ~A_fixed() { std::cout << "A_fixed destroyed\n"; }
};

class B_fixed {
public:
    std::weak_ptr<A_fixed> a_ptr;  // 使用 weak_ptr 打破循环
    ~B_fixed() { std::cout << "B_fixed destroyed\n"; }
};

void fixedReference() {
    std::shared_ptr<A_fixed> a = std::make_shared<A_fixed>();
    std::shared_ptr<B_fixed> b = std::make_shared<B_fixed>();

    a->b_ptr = b;
    b->a_ptr = a;  // weak_ptr 不增加引用计数

    // a 和 b 都会被正确释放
}

// weak_ptr 使用
int main() {
    std::shared_ptr<int> sp = std::make_shared<int>(42);
    std::weak_ptr<int> wp = sp;  // weak_ptr 不增加引用计数

    std::cout << "use_count: " << sp.use_count() << std::endl;  // 1
    std::cout << "weak use_count: " << wp.use_count() << std::endl;  // 1

    // 使用 weak_ptr：先转换为 shared_ptr
    if (std::shared_ptr<int> sp2 = wp.lock()) {  // lock() 返回 shared_ptr
        std::cout << "Value: " << *sp2 << std::endl;
        std::cout << "use_count: " << sp.use_count() << std::endl;  // 2
    }

    sp.reset();  // 释放资源

    // 对象已销毁，lock() 返回空指针
    if (std::shared_ptr<int> sp3 = wp.lock()) {
        std::cout << "Object exists" << std::endl;
    } else {
        std::cout << "Object destroyed" << std::endl;
    }

    std::cout << "expired: " << wp.expired() << std::endl;  // true

    return 0;
}
```

**关键要点：**
- `weak_ptr` 不增加引用计数，用于观察 `shared_ptr` 管理的对象
- 用途：打破循环引用、缓存、观察者模式
- 使用前必须通过 `lock()` 转换为 `shared_ptr`
- `expired()` 检查对象是否已销毁

**常见陷阱：**
- 不检查 `lock()` 返回值就使用
- 混淆 `weak_ptr` 和 `shared_ptr` 的用途

---

### 4.5 智能指针的选择

**问题：何时使用哪种智能指针？**

```cpp
// 1. unique_ptr：独占所有权（默认选择）
std::unique_ptr<MyClass> createObject() {
    return std::make_unique<MyClass>(42);
}

// 2. shared_ptr：共享所有权
class Cache {
    std::unordered_map<std::string, std::shared_ptr<Data>> cache_;

    std::shared_ptr<Data> get(const std::string& key) {
        return cache_[key];  // 共享所有权
    }
};

// 3. weak_ptr：观察者模式
class Subject;

class Observer {
    std::weak_ptr<Subject> subject_;  // 不持有 Subject

    void notify() {
        if (auto sub = subject_.lock()) {  // 检查 Subject 是否存活
            // 使用 sub
        }
    }
};
```

**选择指南：**

| 场景 | 智能指针 | 原因 |
|------|----------|------|
| 默认选择 | `unique_ptr` | 性能最好，语义明确 |
| 多个所有者 | `shared_ptr` | 需要共享所有权 |
| 打破循环引用 | `weak_ptr` | 不增加引用计数 |
| 观察对象 | `weak_ptr` | 不影响对象生命周期 |
| 数组 | `unique_ptr<T[]>` 或 `vector` | 优先使用 `vector` |

---

## 5. RAII (Resource Acquisition Is Initialization)

### 5.1 RAII 原则

**问题：什么是 RAII？为什么重要？**

```cpp
#include <iostream>
#include <fstream>
#include <mutex>

// 反例：手动管理资源（容易出错）
void badResourceManagement() {
    std::FILE* file = std::fopen("data.txt", "r");
    if (!file) {
        return;  // 忘记关闭？不，这里还没打开
    }

    // 处理文件
    if (error_occurred) {
        // 忘记 fclose(file)！内存泄漏
        return;
    }

    std::fclose(file);  // 可能不会执行
}

// 正确：RAII 封装资源
class FileGuard {
public:
    FileGuard(const char* filename, const char* mode) {
        file_ = std::fopen(filename, mode);
        if (!file_) {
            throw std::runtime_error("Failed to open file");
        }
    }

    ~FileGuard() {
        if (file_) {
            std::fclose(file_);
            std::cout << "File closed\n";
        }
    }

    std::FILE* get() { return file_; }

    // 禁用拷贝
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;

private:
    std::FILE* file_;
};

void goodResourceManagement() {
    FileGuard file("data.txt", "r");

    // 处理文件
    if (error_occurred) {
        return;  // FileGuard 析构函数自动关闭文件
    }

    // ...
}  // 无论如何退出，文件都会被正确关闭

// 更好：使用标准库的 RAII 类
void bestPractice() {
    std::ifstream file("data.txt");  // RAII

    if (error_occurred) {
        return;  // 自动关闭
    }

    // ...
}  // 自动关闭

// 互斥锁的 RAII
std::mutex mtx;

void threadSafeFunction() {
    std::lock_guard<std::mutex> lock(mtx);  // RAII 锁

    // 临界区
    if (error_occurred) {
        return;  // 自动解锁
    }

    // ...
}  // 自动解锁
```

**关键要点：**
- **RAII**：资源获取即初始化，资源释放即销毁
- 在构造函数中获取资源，在析构函数中释放资源
- 利用 C++ 的自动析构机制，确保资源总是被释放
- 异常安全：无论如何退出，资源都会被正确释放

**RAII 管理的资源：**
- 内存：智能指针
- 文件：`std::fstream`
- 锁：`std::lock_guard`、`std::unique_lock`
- 数据库连接、网络套接字等

**常见陷阱：**
- 忘记遵循 RAII 原则，手动管理资源
- 在构造函数中获取资源但忘记在析构函数中释放

---

## 6. 内存泄漏检测

### 6.1 常见内存泄漏场景

**问题：有哪些常见的内存泄漏场景？**

```cpp
#include <iostream>
#include <memory>

// 1. 忘记 delete
void leak1() {
    int* p = new int(42);
    // 忘记 delete p;
}

// 2. 异常导致未 delete
void leak2() {
    int* p = new int(42);
    throw std::runtime_error("Error");  // p 未被 delete
    delete p;  // 不会执行
}

// 3. 循环引用
class Node {
public:
    std::shared_ptr<Node> next;
    std::shared_ptr<Node> prev;  // 循环引用
    ~Node() { std::cout << "Node destroyed\n"; }
};

void leak3() {
    auto node1 = std::make_shared<Node>();
    auto node2 = std::make_shared<Node>();
    node1->next = node2;
    node2->prev = node1;  // 循环引用，内存泄漏
}

// 4. 容器中的原始指针
void leak4() {
    std::vector<int*> vec;
    vec.push_back(new int(1));
    vec.push_back(new int(2));
    // vector 销毁时不会 delete 指针指向的对象
}

// 5. 虚析构函数缺失
class Base {
public:
    ~Base() { std::cout << "Base destroyed\n"; }  // 非虚析构
};

class Derived : public Base {
public:
    Derived() { data_ = new int[100]; }
    ~Derived() {
        delete[] data_;
        std::cout << "Derived destroyed\n";
    }
private:
    int* data_;
};

void leak5() {
    Base* p = new Derived();
    delete p;  // 只调用 Base 析构，Derived 析构不执行，泄漏！
}

// 正确做法
void fixed() {
    // 1. 使用智能指针
    auto p = std::make_unique<int>(42);

    // 2. 异常安全
    try {
        auto p2 = std::make_unique<int>(42);
        throw std::runtime_error("Error");
    } catch (...) {
        // p2 自动释放
    }

    // 3. 使用 weak_ptr 打破循环
    class NodeFixed {
    public:
        std::shared_ptr<NodeFixed> next;
        std::weak_ptr<NodeFixed> prev;  // weak_ptr
    };

    // 4. 容器使用智能指针
    std::vector<std::unique_ptr<int>> vec;
    vec.push_back(std::make_unique<int>(1));

    // 5. 虚析构函数
    class BaseFixed {
    public:
        virtual ~BaseFixed() = default;
    };
}
```

**内存泄漏检测工具：**
- **Valgrind**（Linux）：`valgrind --leak-check=full ./program`
- **AddressSanitizer**（GCC/Clang）：编译时加 `-fsanitize=address`
- **Visual Studio**（Windows）：内置内存泄漏检测
- **Dr. Memory**（跨平台）

---

## 7. 内存对齐

### 7.1 内存对齐基础

**问题：什么是内存对齐？为什么需要？**

```cpp
#include <iostream>

struct A {
    char c;    // 1 字节
    int i;     // 4 字节
    short s;   // 2 字节
};

struct B {
    char c;    // 1 字节
    char pad1[3];  // 填充 3 字节
    int i;     // 4 字节
    short s;   // 2 字节
    char pad2[2];  // 填充 2 字节
};

struct C {
    int i;     // 4 字节
    char c;    // 1 字节
    short s;   // 2 字节
    char pad[1];  // 填充 1 字节
};

int main() {
    std::cout << "sizeof(char): " << sizeof(char) << std::endl;    // 1
    std::cout << "sizeof(short): " << sizeof(short) << std::endl;  // 2
    std::cout << "sizeof(int): " << sizeof(int) << std::endl;      // 4

    std::cout << "sizeof(A): " << sizeof(A) << std::endl;  // 12 (不是 7)
    std::cout << "sizeof(B): " << sizeof(B) << std::endl;  // 12
    std::cout << "sizeof(C): " << sizeof(C) << std::endl;  // 8

    std::cout << "alignof(A): " << alignof(A) << std::endl;  // 4

    return 0;
}

/*
A 的内存布局（按声明顺序）：
+---+---+---+---+---+---+---+---+---+---+---+---+
| c |pad|pad|pad| i | i | i | i | s | s |pad|pad|
+---+---+---+---+---+---+---+---+---+---+---+---+
  1   1   1   1   4   4   4   4   2   2   1   1  = 12 字节

C 的内存布局（优化后）：
+---+---+---+---+---+---+---+---+
| i | i | i | i | c | s | s |pad|
+---+---+---+---+---+---+---+---+
  4   4   4   4   1   2   2   1  = 8 字节
*/
```

**关键要点：**
- **内存对齐**：数据地址必须是其大小的整数倍
- 对齐原因：CPU 访问对齐的数据更高效，某些架构要求对齐
- 结构体大小是最大成员对齐值的整数倍
- 优化：将大字段放在前面，减少填充

**常见陷阱：**
- 忽略内存对齐，导致结构体大小超出预期
- 跨平台时对齐规则不同

---

### 7.2 控制内存对齐

**问题：如何控制内存对齐？**

```cpp
#include <iostream>

// 1. 默认对齐
struct Default {
    char c;
    int i;
};

// 2. 指定对齐（C++11）
struct alignas(16) Aligned16 {
    char c;
    int i;
};

// 3. 取消对齐（编译器特定）
#pragma pack(push, 1)
struct Packed {
    char c;
    int i;
    short s;
};
#pragma pack(pop)

// 4. 指定成员对齐
struct MemberAligned {
    char c;
    alignas(8) int i;  // i 对齐到 8 字节
};

int main() {
    std::cout << "sizeof(Default): " << sizeof(Default) << std::endl;       // 8
    std::cout << "sizeof(Aligned16): " << sizeof(Aligned16) << std::endl;   // 16
    std::cout << "sizeof(Packed): " << sizeof(Packed) << std::endl;         // 7
    std::cout << "sizeof(MemberAligned): " << sizeof(MemberAligned) << std::endl;  // 16

    return 0;
}
```

**关键要点：**
- `alignas` 指定对齐值（C++11）
- `#pragma pack` 取消对齐（编译器特定）
- 取消对齐会降低性能，但减少内存占用

**常见陷阱：**
- 过度使用 `#pragma pack` 导致性能下降
- `alignas` 只能增加对齐，不能减少

---

## 8. 内存优化技巧

### 8.1 对象池 (Object Pool)

**问题：如何减少频繁 new/delete 的开销？**

```cpp
#include <iostream>
#include <vector>
#include <memory>

template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t size) {
        pool_.reserve(size);
    }

    template<typename... Args>
    T* allocate(Args&&... args) {
        if (pool_.empty()) {
            return new T(std::forward<Args>(args)...);
        } else {
            T* obj = pool_.back();
            pool_.pop_back();
            new (obj) T(std::forward<Args>(args)...);  // placement new
            return obj;
        }
    }

    void deallocate(T* obj) {
        obj->~T();  // 显式调用析构函数
        pool_.push_back(obj);
    }

    ~ObjectPool() {
        for (T* obj : pool_) {
            delete obj;
        }
    }

private:
    std::vector<T*> pool_;
};

class MyClass {
public:
    MyClass(int x) : x_(x) {}
    int x_;
};

int main() {
    ObjectPool<MyClass> pool(10);

    MyClass* obj1 = pool.allocate(42);
    MyClass* obj2 = pool.allocate(100);

    pool.deallocate(obj1);  // 回收到池中
    pool.deallocate(obj2);

    MyClass* obj3 = pool.allocate(200);  // 从池中获取

    pool.deallocate(obj3);

    return 0;
}
```

**关键要点：**
- 对象池预分配对象，减少 new/delete 开销
- 适用于频繁创建/销毁的小对象
- 使用 placement new 在池中重用内存

---

## 9. 总结

### 核心要点
1. **内存分区**：栈（自动）、堆（手动）、数据段、代码段
2. **栈 vs 堆**：栈快但有限，堆灵活但慢，需手动管理
3. **new/delete**：调用构造/析构函数，优于 malloc/free
4. **智能指针**：`unique_ptr`（独占）、`shared_ptr`（共享）、`weak_ptr`（观察）
5. **RAII**：资源获取即初始化，确保资源总是被释放
6. **内存泄漏**：使用智能指针、虚析构函数、打破循环引用
7. **内存对齐**：提高性能，注意结构体大小

### 最佳实践
- 优先使用栈内存和智能指针
- 遵循 RAII 原则
- 避免裸 new/delete
- 使用 `make_unique` 和 `make_shared`
- 基类必须有虚析构函数
- 使用工具检测内存泄漏

### 常见面试题
- 栈和堆的区别？
- new 和 malloc 的区别？
- 智能指针的实现原理？
- 如何避免内存泄漏？
- 什么是 RAII？
- 循环引用如何解决？
- 内存对齐的作用？

---

## 参考资料
- *Effective C++* by Scott Meyers (Item 13-17)
- *Effective Modern C++* by Scott Meyers (Item 18-22)
- *C++ Primer* (5th Edition), Chapter 12
- ISO C++ Core Guidelines - Resource Management

---

## 10. 高频补充：动态对象生命周期与智能指针计数

### Q1: 函数内部 `new` 一个对象，函数退出后对象会消失吗？

**答案：**
- 不会自动消失。
- `new` 分配在堆上，函数退出只会销毁“局部指针变量”，不会自动释放堆对象。
- 若无人管理会导致内存泄漏。

### Q2: 如果要把对象传到函数外继续用，推荐怎么实现？

**答案：**
- 优先返回值对象（让编译器做 RVO/移动）。
- 需要动态分配时，优先返回智能指针。

```cpp
Foo makeFoo();                              // 优先
std::unique_ptr<Foo> makeUniqueFoo();       // 单一所有权
std::shared_ptr<Foo> makeSharedFoo();       // 共享所有权
```

### Q3: `return std::make_shared<Foo>();` 时引用计数如何变化？

```cpp
std::shared_ptr<Foo> makeFoo() {
    return std::make_shared<Foo>();
}
```

**答案：**
- 创建时强引用计数为 1。
- 返回到调用方通常走移动/返回值优化，计数通常保持 1。
- 发生拷贝时计数 +1，析构/reset 时计数 -1，降为 0 时销毁对象。

### Q4: 先 `new` 再构造 `shared_ptr` 时计数如何变化？

```cpp
std::shared_ptr<Foo> makeFoo2() {
    Foo* raw = new Foo();
    std::shared_ptr<Foo> sp(raw);  // 注意显式构造
    return sp;
}
```

**答案：**
- `new Foo()` 阶段还没有控制块，无引用计数。
- 构造 `shared_ptr` 后创建控制块，强引用计数变为 1。
- 返回后通常仍为 1（移动/NRVO）。

**注意：**
- 不要从同一裸指针构造多个 `shared_ptr`，否则可能 double free。
- 更推荐 `make_shared`：代码更简洁，通常分配更高效。
