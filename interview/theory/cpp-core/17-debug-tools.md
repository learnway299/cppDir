# 17 - 调试工具 (Debug Tools)

## 简介

掌握调试工具是 C++ 开发者的必备技能。本章介绍 GDB、Sanitizers、Profilers 等核心调试和性能分析工具。

**核心工具**：
- **GDB**：调试器
- **Sanitizers**：检测内存错误、数据竞争
- **Valgrind**：内存分析
- **Profilers**：性能分析

---

## 1. GDB（GNU Debugger）

### 1.1 基本命令？

```bash
# 编译时加 -g 选项
g++ -g -O0 main.cpp -o program

# 启动 GDB
gdb ./program

# GDB 基本命令
(gdb) run                    # 运行程序
(gdb) run arg1 arg2         # 带参数运行
(gdb) break main            # 在 main 设置断点
(gdb) break file.cpp:42     # 在指定行设置断点
(gdb) break func_name       # 在函数设置断点
(gdb) info breakpoints      # 查看断点
(gdb) delete 1              # 删除断点 1

(gdb) next                  # 单步执行（不进入函数）
(gdb) step                  # 单步执行（进入函数）
(gdb) continue              # 继续执行
(gdb) finish                # 执行完当前函数

(gdb) print variable        # 打印变量
(gdb) print *pointer        # 打印指针内容
(gdb) print array[0]@10     # 打印数组前 10 个元素

(gdb) backtrace            # 查看调用栈
(gdb) frame 2              # 切换到栈帧 2
(gdb) info locals          # 查看局部变量
(gdb) info args            # 查看函数参数

(gdb) watch variable       # 监视变量变化
(gdb) quit                 # 退出
```

### 1.2 调试核心转储？

```bash
# 启用核心转储
ulimit -c unlimited

# 程序崩溃后生成 core 文件
gdb ./program core

# 查看崩溃时的调用栈
(gdb) backtrace
```

### 1.3 多线程调试？

```bash
(gdb) info threads         # 查看所有线程
(gdb) thread 2             # 切换到线程 2
(gdb) thread apply all bt  # 查看所有线程的调用栈
```

---

## 2. AddressSanitizer (ASan)

### 2.1 检测内存错误？

```bash
# 编译时启用 ASan
g++ -fsanitize=address -g main.cpp -o program

# 运行
./program
```

**可检测的错误**：
- 堆缓冲区溢出
- 栈缓冲区溢出
- Use-after-free
- Double-free
- 内存泄漏

**示例**：

```cpp
// Use-after-free
int* ptr = new int(10);
delete ptr;
*ptr = 20;  // ❌ ASan 检测到

// 缓冲区溢出
int arr[10];
arr[10] = 42;  // ❌ ASan 检测到

// 内存泄漏
int* leak = new int(10);
// 忘记 delete  // ❌ ASan 检测到
```

---

## 3. ThreadSanitizer (TSan)

### 3.1 检测数据竞争？

```bash
# 编译时启用 TSan
g++ -fsanitize=thread -g main.cpp -o program -pthread

# 运行
./program
```

**可检测的错误**：
- 数据竞争
- 死锁

**示例**：

```cpp
int global = 0;

void thread1() {
    global = 1;  // ❌ 数据竞争
}

void thread2() {
    int x = global;  // ❌ 数据竞争
}

// TSan 会报告数据竞争的位置
```

---

## 4. UndefinedBehaviorSanitizer (UBSan)

### 4.1 检测未定义行为？

```bash
# 编译时启用 UBSan
g++ -fsanitize=undefined -g main.cpp -o program

# 运行
./program
```

**可检测的错误**：
- 整数溢出
- 空指针解引用
- 除以零
- 数组越界
- 非法类型转换

```cpp
int x = INT_MAX;
x = x + 1;  // ❌ 整数溢出

int* p = nullptr;
*p = 10;  // ❌ 空指针解引用

int y = 10 / 0;  // ❌ 除以零
```

---

## 5. Valgrind

### 5.1 Memcheck 内存检查？

```bash
# 运行 Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./program

# 详细输出
valgrind --leak-check=full --track-origins=yes ./program
```

**可检测的错误**：
- 内存泄漏
- 非法读写
- 使用未初始化的值

**示例输出**：
```
==12345== Invalid write of size 4
==12345==    at 0x40056E: main (test.cpp:10)
==12345==  Address 0x5203040 is 0 bytes after a block of size 40 alloc'd
```

### 5.2 Callgrind 性能分析？

```bash
# 运行 Callgrind
valgrind --tool=callgrind ./program

# 查看结果
kcachegrind callgrind.out.12345
```

---

## 6. 性能分析工具

### 6.1 perf（Linux）？

```bash
# 编译时保留符号
g++ -g -O2 main.cpp -o program

# 记录性能数据
perf record ./program

# 查看报告
perf report

# 查看热点函数
perf top

# CPU 缓存分析
perf stat -e cache-misses,cache-references ./program
```

### 6.2 gprof？

```bash
# 编译时启用 profiling
g++ -pg main.cpp -o program

# 运行程序（生成 gmon.out）
./program

# 查看报告
gprof program gmon.out
```

### 6.3 Flamegraph 火焰图？

```bash
# 使用 perf 生成火焰图
perf record -F 99 -a -g -- ./program
perf script | ./flamegraph.pl > flamegraph.svg
```

---

## 7. 静态分析工具

### 7.1 Clang Static Analyzer？

```bash
# 使用 scan-build
scan-build make

# 或直接分析单个文件
clang++ --analyze main.cpp
```

### 7.2 Cppcheck？

```bash
# 安装 Cppcheck
sudo apt-get install cppcheck

# 运行分析
cppcheck --enable=all main.cpp
```

---

## 8. 调试技巧

### 8.1 条件断点？

```bash
(gdb) break main.cpp:42 if x == 10  # 仅当 x == 10 时断点
```

### 8.2 打印调用栈？

```cpp
#include <execinfo.h>

void print_stacktrace() {
    void* array[10];
    size_t size = backtrace(array, 10);
    char** strings = backtrace_symbols(array, size);
    for (size_t i = 0; i < size; i++) {
        std::cout << strings[i] << "\n";
    }
    free(strings);
}
```

### 8.3 断言？

```cpp
#include <cassert>

void func(int* ptr) {
    assert(ptr != nullptr);  // Debug 模式检查
    *ptr = 10;
}
```

---

## 总结

### 工具选择指南

| 问题类型 | 推荐工具 |
|---------|---------|
| 内存错误 | ASan, Valgrind |
| 数据竞争 | TSan |
| 未定义行为 | UBSan |
| 性能瓶颈 | perf, Callgrind |
| 崩溃调试 | GDB |
| 静态分析 | Clang Analyzer, Cppcheck |

### 关键记忆点

1. **编译时加 -g**：调试符号
2. **ASan**：快速检测内存错误
3. **TSan**：检测数据竞争
4. **Valgrind**：全面但慢
5. **perf**：Linux 性能分析首选
6. **GDB**：交互式调试器
