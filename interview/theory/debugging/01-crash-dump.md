# 崩溃分析 (Crash Dump Analysis)

程序崩溃分析是资深工程师的核心技能，涉及原理理解和工具实操。

---

## 1. 崩溃基础概念

### 1.1 什么是崩溃

```
┌─────────────────────────────────────────────────────────────┐
│                      程序崩溃本质                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  崩溃 = 程序执行了非法操作，被操作系统强制终止                │
│                                                             │
│  ┌─────────────┐     ┌─────────────┐     ┌─────────────┐   │
│  │  非法操作    │ ──→ │  CPU 异常   │ ──→ │  OS 信号    │   │
│  │             │     │  (硬件中断) │     │  (SIGSEGV)  │   │
│  └─────────────┘     └─────────────┘     └─────────────┘   │
│                                                    │        │
│                                                    ↓        │
│                                          ┌─────────────┐   │
│                                          │  进程终止    │   │
│                                          │  生成 Dump  │   │
│                                          └─────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 常见崩溃类型

| 崩溃类型 | 信号/异常 | 典型原因 | 示例代码 |
|----------|-----------|----------|----------|
| 空指针解引用 | SIGSEGV / Access Violation | 访问地址 0 | `int* p = nullptr; *p = 1;` |
| 野指针 | SIGSEGV | use-after-free | `delete p; *p = 1;` |
| 栈溢出 | SIGSEGV / Stack Overflow | 无限递归 | `void f() { f(); }` |
| 堆损坏 | SIGABRT | 双重释放、越界写 | `delete p; delete p;` |
| 除零错误 | SIGFPE | 整数除以零 | `int x = 1/0;` |
| 非法指令 | SIGILL | 执行数据区 | 函数指针指向数据 |
| 断言失败 | SIGABRT | assert 触发 | `assert(false);` |
| 纯虚函数调用 | SIGABRT | 构造/析构中调虚函数 | 见下方示例 |

### 1.3 崩溃代码示例

```cpp
// 1. 空指针
void null_pointer_crash() {
    int* p = nullptr;
    *p = 42;  // SIGSEGV
}

// 2. 栈溢出
void stack_overflow() {
    char buf[1024 * 1024];  // 1MB 局部变量
    stack_overflow();        // 无限递归
}

// 3. 堆损坏 - 双重释放
void double_free() {
    int* p = new int(42);
    delete p;
    delete p;  // SIGABRT: double free
}

// 4. 堆损坏 - 越界写
void heap_overflow() {
    int* arr = new int[10];
    arr[100] = 42;  // 堆元数据损坏，可能延迟崩溃
    delete[] arr;   // 这里才崩溃
}

// 5. 纯虚函数调用
class Base {
public:
    Base() { init(); }  // 危险！
    virtual void init() = 0;
};
class Derived : public Base {
    void init() override {}
};
// Derived d;  // 构造 Base 时调用纯虚函数 → SIGABRT

// 6. 返回局部变量引用
int& dangling_reference() {
    int local = 42;
    return local;  // 悬垂引用
}
```

---

## 2. 信号机制 (Unix/Linux/macOS)

### 2.1 崩溃相关信号

```
┌─────────────────────────────────────────────────────────────┐
│                     崩溃信号详解                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  信号        编号    触发原因                                │
│  ─────────────────────────────────────────────────────────  │
│  SIGSEGV     11     段错误（非法内存访问）                   │
│                     - 空指针解引用                           │
│                     - 访问未映射内存                         │
│                     - 写只读内存                             │
│                                                             │
│  SIGBUS      7      总线错误                                │
│                     - 内存对齐错误                           │
│                     - 访问不存在的物理地址                   │
│                                                             │
│  SIGABRT     6      异常终止                                │
│                     - abort() 调用                          │
│                     - assert 失败                           │
│                     - 堆损坏检测                             │
│                                                             │
│  SIGFPE      8      浮点异常                                │
│                     - 整数除零                               │
│                     - 浮点溢出                               │
│                                                             │
│  SIGILL      4      非法指令                                │
│                     - 执行非指令数据                         │
│                     - 特权指令                               │
│                                                             │
│  SIGTRAP     5      调试陷阱                                │
│                     - 断点                                   │
│                     - __builtin_trap()                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 信号处理与崩溃捕获

```cpp
#include <signal.h>
#include <execinfo.h>  // backtrace
#include <unistd.h>
#include <cstring>
#include <cstdlib>

// 信号处理函数
void crash_handler(int sig, siginfo_t* info, void* context) {
    const char* sig_name = "UNKNOWN";
    switch (sig) {
        case SIGSEGV: sig_name = "SIGSEGV"; break;
        case SIGABRT: sig_name = "SIGABRT"; break;
        case SIGFPE:  sig_name = "SIGFPE"; break;
        case SIGBUS:  sig_name = "SIGBUS"; break;
        case SIGILL:  sig_name = "SIGILL"; break;
    }

    // 输出基本信息
    fprintf(stderr, "\n=== CRASH: %s ===\n", sig_name);
    fprintf(stderr, "Signal: %d\n", sig);
    fprintf(stderr, "Fault address: %p\n", info->si_addr);

    // 打印调用栈
    void* bt[64];
    int bt_size = backtrace(bt, 64);
    fprintf(stderr, "\nBacktrace:\n");
    backtrace_symbols_fd(bt, bt_size, STDERR_FILENO);

    // 重置信号处理器，让默认处理生成 core dump
    signal(sig, SIG_DFL);
    raise(sig);
}

// 安装信号处理器
void install_crash_handler() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = crash_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND;

    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGFPE, &sa, nullptr);
    sigaction(SIGBUS, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
}

int main() {
    install_crash_handler();

    // 触发崩溃
    int* p = nullptr;
    *p = 42;

    return 0;
}
```

---

## 3. Core Dump 原理

### 3.1 Core Dump 是什么

```
┌─────────────────────────────────────────────────────────────┐
│                    Core Dump 结构                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Core Dump = 进程崩溃时的内存快照                            │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    Core 文件内容                      │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │  ELF Header          文件格式头                       │   │
│  │  Program Headers     段描述                          │   │
│  │  ─────────────────────────────────────────────────  │   │
│  │  寄存器状态          PC、SP、通用寄存器等             │   │
│  │  ─────────────────────────────────────────────────  │   │
│  │  内存段                                              │   │
│  │  ├── .text (代码段)   可选，通常从可执行文件读        │   │
│  │  ├── .data (数据段)   全局变量                       │   │
│  │  ├── .bss            未初始化数据                    │   │
│  │  ├── heap            堆内存                          │   │
│  │  ├── stack           栈内存（每个线程）               │   │
│  │  └── mmap regions    内存映射区域                    │   │
│  │  ─────────────────────────────────────────────────  │   │
│  │  线程信息            每个线程的状态                   │   │
│  │  信号信息            触发 dump 的信号                 │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Linux Core Dump 配置

```bash
# 查看当前 core dump 配置
ulimit -c           # core 文件大小限制（0 表示禁用）
cat /proc/sys/kernel/core_pattern  # core 文件路径模式

# 启用 core dump
ulimit -c unlimited

# 配置 core 文件路径和命名
# %p: PID, %e: 程序名, %t: 时间戳
sudo sysctl -w kernel.core_pattern=/tmp/core.%e.%p.%t

# 永久配置 /etc/sysctl.conf
kernel.core_pattern = /var/crash/core.%e.%p.%t
kernel.core_uses_pid = 1

# 检查进程是否会生成 core dump
cat /proc/<pid>/limits | grep "Max core file size"
```

### 3.3 macOS Core Dump 配置

```bash
# macOS 默认 core dump 位置
# /cores/core.<pid>

# 启用 core dump
ulimit -c unlimited

# 检查 core dump 是否启用
sudo sysctl kern.coredump
# 1 = 启用, 0 = 禁用

# 启用 core dump（需要关闭 SIP）
sudo sysctl -w kern.coredump=1

# 查看 crash reports
# ~/Library/Logs/DiagnosticReports/
# /Library/Logs/DiagnosticReports/
```

---

## 4. 调用栈回溯原理

### 4.1 栈帧结构

```
┌─────────────────────────────────────────────────────────────┐
│                      x86-64 栈帧布局                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  高地址                                                     │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    调用者栈帧                         │   │
│  │  ...                                                │   │
│  │  参数 N (如果参数 > 6 个)                            │   │
│  │  参数 7                                             │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │  返回地址 (call 指令压入)          ← RBP + 8        │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │  保存的 RBP (上一帧基址)           ← RBP            │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │  局部变量 1                        ← RBP - 8        │   │
│  │  局部变量 2                        ← RBP - 16       │   │
│  │  ...                                                │   │
│  │  保存的寄存器 (callee-saved)                        │   │
│  │  红区 (128 字节，叶函数可用)        ← RSP           │   │
│  └─────────────────────────────────────────────────────┘   │
│  低地址                                                     │
│                                                             │
│  栈回溯过程：                                               │
│  1. 从当前 RBP 读取上一帧 RBP                               │
│  2. 从 RBP + 8 读取返回地址                                 │
│  3. 重复直到 RBP = 0 或无效                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 DWARF 调试信息

```
┌─────────────────────────────────────────────────────────────┐
│                    DWARF 调试信息                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  DWARF = Debugging With Attributed Record Formats           │
│                                                             │
│  主要段：                                                   │
│  ├── .debug_info      类型、变量、函数信息                  │
│  ├── .debug_abbrev    调试信息缩写表                        │
│  ├── .debug_line      源码行号映射                          │
│  ├── .debug_frame     栈帧展开信息（CFI）                   │
│  ├── .debug_str       字符串表                              │
│  └── .debug_loc       变量位置信息                          │
│                                                             │
│  编译选项：                                                  │
│  ├── -g               包含调试信息                          │
│  ├── -g3              包含宏定义信息                        │
│  ├── -gdwarf-4        指定 DWARF 版本                       │
│  └── -gsplit-dwarf    分离调试信息到 .dwo 文件             │
│                                                             │
│  查看调试信息：                                              │
│  $ readelf --debug-dump=info ./a.out                       │
│  $ dwarfdump ./a.out                                       │
│  $ llvm-dwarfdump ./a.out                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 符号表与地址解析

```bash
# 查看符号表
nm -C ./a.out           # -C: demangle C++ 符号
objdump -t ./a.out      # 符号表
objdump -T ./a.out      # 动态符号表

# 地址转源码位置
addr2line -e ./a.out -f -C 0x401234
# -e: 可执行文件
# -f: 显示函数名
# -C: demangle

# macOS 地址解析
atos -o ./a.out -l 0x100000000 0x100001234
# -l: 加载地址（ASLR 基址）

# 批量解析（从 crash log）
cat crash.log | atos -o ./a.out -l 0x100000000
```

---

## 5. Windows 崩溃分析

### 5.1 Windows 异常机制

```
┌─────────────────────────────────────────────────────────────┐
│                   Windows 异常类型                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  异常代码                        含义                        │
│  ─────────────────────────────────────────────────────────  │
│  0xC0000005  ACCESS_VIOLATION    访问冲突（空指针/野指针）   │
│  0xC0000094  INTEGER_DIVIDE_BY_ZERO  整数除零               │
│  0xC00000FD  STACK_OVERFLOW      栈溢出                     │
│  0xC0000409  STACK_BUFFER_OVERRUN  栈缓冲区溢出（安全检查） │
│  0xC0000374  HEAP_CORRUPTION     堆损坏                     │
│  0x80000003  BREAKPOINT          断点                       │
│  0xC000001D  ILLEGAL_INSTRUCTION 非法指令                   │
│  0xE06D7363  C++ Exception       C++ 异常                   │
│                                                             │
│  ACCESS_VIOLATION 细分：                                     │
│  - 读取地址 0x00000000 → 空指针读                           │
│  - 写入地址 0x00000000 → 空指针写                           │
│  - 读取地址 0xCDCDCDCD → 未初始化堆内存                     │
│  - 读取地址 0xDDDDDDDD → 已释放堆内存                       │
│  - 读取地址 0xFDFDFDFD → 堆边界（越界）                     │
│  - 读取地址 0xCCCCCCCC → 未初始化栈内存                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 Windows Dump 类型

```
┌─────────────────────────────────────────────────────────────┐
│                   Windows Dump 类型                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  类型              大小      内容                            │
│  ─────────────────────────────────────────────────────────  │
│  Minidump         小        线程栈、寄存器、模块列表         │
│  (MiniDumpNormal)           最基础，可能缺少关键数据         │
│                                                             │
│  Minidump +       中等      + 部分堆内存                     │
│  WithDataSegs               适合大多数场景                   │
│                                                             │
│  Full Dump        大        完整进程内存                     │
│  (MiniDumpWithFullMemory)   最完整，文件可能数 GB            │
│                                                             │
│  Heap Dump        可变      堆信息详细                       │
│  (MiniDumpWithFullMemoryInfo)  用于内存问题分析             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.3 生成 Dump 文件

```cpp
// Windows 程序内生成 dump
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

LONG WINAPI CrashHandler(EXCEPTION_POINTERS* pException) {
    // 创建 dump 文件
    HANDLE hFile = CreateFileW(
        L"crash.dmp",
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mei;
        mei.ThreadId = GetCurrentThreadId();
        mei.ExceptionPointers = pException;
        mei.ClientPointers = FALSE;

        // 写入 dump（包含堆信息）
        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            (MINIDUMP_TYPE)(MiniDumpWithDataSegs |
                           MiniDumpWithHandleData |
                           MiniDumpWithThreadInfo),
            &mei,
            nullptr,
            nullptr
        );

        CloseHandle(hFile);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

int main() {
    // 安装异常处理器
    SetUnhandledExceptionFilter(CrashHandler);

    // 触发崩溃
    int* p = nullptr;
    *p = 42;

    return 0;
}
```

### 5.4 WinDbg 分析 Dump

```
┌─────────────────────────────────────────────────────────────┐
│                   WinDbg 常用命令                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  === 基础命令 ===                                           │
│  !analyze -v          自动分析崩溃原因（最常用）            │
│  .ecxr               切换到异常上下文                       │
│  r                   显示寄存器                             │
│  k                   显示调用栈                             │
│  kv                  显示调用栈 + 参数                      │
│  kP                  显示调用栈 + 完整参数                  │
│  kb                  显示调用栈 + 前 3 个参数               │
│                                                             │
│  === 内存查看 ===                                           │
│  dd <addr>           显示 DWORD                             │
│  dq <addr>           显示 QWORD                             │
│  da <addr>           显示 ASCII 字符串                      │
│  du <addr>           显示 Unicode 字符串                    │
│  dps <addr>          显示指针 + 符号                        │
│                                                             │
│  === 符号相关 ===                                           │
│  .sympath            查看/设置符号路径                      │
│  .symfix+ <path>     添加微软符号服务器                     │
│  .reload /f          强制重新加载符号                       │
│  ln <addr>           查找最近的符号                         │
│  x module!pattern    搜索符号                               │
│                                                             │
│  === 堆分析 ===                                             │
│  !heap -s            堆摘要                                 │
│  !heap -a <addr>     分析特定堆                             │
│  !heap -p -a <addr>  页堆分析                               │
│                                                             │
│  === 线程分析 ===                                           │
│  ~                   列出所有线程                           │
│  ~*k                 显示所有线程调用栈                     │
│  ~0s                 切换到线程 0                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.5 WinDbg 实战示例

```
// 1. 打开 dump 文件
File → Open Crash Dump → 选择 .dmp 文件

// 2. 设置符号路径
.sympath srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
.sympath+ C:\MyProject\build\Release  // 添加自己的 PDB 路径
.reload

// 3. 自动分析
!analyze -v

// 输出示例：
// EXCEPTION_RECORD:  (.exr -1)
// ExceptionAddress: 00007ff6abc01234 (MyApp!CrashFunction+0x24)
//    ExceptionCode: c0000005 (Access violation)
//   ExceptionFlags: 00000000
// NumberParameters: 2
//    Parameter[0]: 0000000000000000  ← 读操作
//    Parameter[1]: 0000000000000000  ← 访问地址（空指针）
//
// STACK_TEXT:
// 00007ff6abc01234 MyApp!CrashFunction+0x24
// 00007ff6abc01567 MyApp!main+0x47
// 00007ff6abc01890 MyApp!__mainCRTStartup+0x10c

// 4. 查看崩溃位置源码
.ecxr
u @rip L5    // 反汇编当前位置前后 5 条指令

// 5. 查看局部变量
dv            // 显示局部变量（需要完整 PDB）

// 6. 查看特定内存
dq @rsp L10  // 查看栈顶 10 个 QWORD
dd @rcx      // 查看 rcx 指向的内存
```

### 5.6 Visual Studio 分析 Dump

```
1. 打开 Dump：
   File → Open → File → 选择 .dmp 文件

2. 设置符号路径：
   Debug → Options → Debugging → Symbols
   添加 PDB 文件路径或符号服务器

3. 开始调试：
   点击 "Debug with Native Only"

4. 查看信息：
   - Call Stack 窗口：查看调用栈
   - Locals 窗口：查看局部变量
   - Watch 窗口：监视表达式
   - Memory 窗口：查看原始内存
   - Disassembly 窗口：查看汇编

5. 源码映射：
   - 确保 PDB 与 EXE 匹配（时间戳/GUID）
   - 源码路径可能需要重新映射
```

---

## 6. macOS 崩溃分析

### 6.1 Crash Report 结构

```
┌─────────────────────────────────────────────────────────────┐
│                   macOS Crash Report                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Process:         MyApp [12345]                             │
│  Path:            /Applications/MyApp.app/.../MyApp         │
│  Identifier:      com.example.MyApp                         │
│  Version:         1.0.0 (1)                                 │
│  Code Type:       ARM-64 / X86-64                           │
│  Parent Process:  launchd [1]                               │
│                                                             │
│  Date/Time:       2024-01-15 10:30:45.123 +0800             │
│  OS Version:      macOS 14.2 (23C64)                        │
│                                                             │
│  Exception Type:  EXC_BAD_ACCESS (SIGSEGV)                  │
│  Exception Codes: KERN_INVALID_ADDRESS at 0x0000000000000000│
│  Termination Reason: SIGNAL 11 Segmentation fault          │
│                                                             │
│  VM Region Info: 0 is not in any region.                    │
│                                                             │
│  Thread 0 Crashed:                                          │
│  0   MyApp        0x0000000100001234 crash_func + 20        │
│  1   MyApp        0x0000000100001567 main + 47              │
│  2   dyld         0x00000001800123ab start + 520            │
│                                                             │
│  Binary Images:                                             │
│  0x100000000 - 0x100ffffff MyApp <UUID> /path/to/MyApp     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 LLDB 分析 Core Dump

```bash
# 1. 加载 core dump
lldb --core /cores/core.12345 ./MyApp

# 2. 或者附加到进程后生成
lldb -p <pid>
(lldb) process save-core /tmp/my.core

# 3. 常用命令
(lldb) bt                    # 显示调用栈
(lldb) bt all                # 显示所有线程调用栈
(lldb) frame select 0        # 选择栈帧
(lldb) frame variable        # 显示局部变量
(lldb) register read         # 显示寄存器
(lldb) memory read 0x1234    # 读取内存
(lldb) disassemble -p        # 反汇编当前位置
(lldb) image list            # 显示加载的模块
(lldb) image lookup -a 0x1234  # 地址查符号
```

### 6.3 dSYM 符号文件

```bash
# 生成 dSYM（编译时）
clang++ -g -o MyApp main.cpp
dsymutil MyApp  # 生成 MyApp.dSYM

# 或使用 Xcode 构建
# Archive 时自动生成 dSYM

# 查看 dSYM 内容
dwarfdump MyApp.dSYM

# 符号化崩溃日志
# 1. 使用 atos
atos -o MyApp.dSYM/Contents/Resources/DWARF/MyApp \
     -l 0x100000000 \
     -arch arm64 \
     0x100001234

# 2. 使用 symbolicatecrash（Xcode 工具）
export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer
/Applications/Xcode.app/Contents/SharedFrameworks/DVTFoundation.framework/\
Versions/A/Resources/symbolicatecrash crash.log MyApp.dSYM > symbolicated.log
```

### 6.4 LLDB 实战示例

```
$ lldb --core /cores/core.12345 ./MyApp

(lldb) target create "./MyApp" --core "/cores/core.12345"
Core file '/cores/core.12345' (x86_64) was loaded.

(lldb) bt
* thread #1, stop reason = signal SIGSEGV
  * frame #0: 0x0000000100001234 MyApp`crash_func() + 20 at main.cpp:10
    frame #1: 0x0000000100001567 MyApp`main + 47 at main.cpp:20
    frame #2: 0x00007fff204f5f3d dyld`start + 1

(lldb) frame select 0
frame #0: 0x0000000100001234 MyApp`crash_func() + 20 at main.cpp:10

(lldb) frame variable
(int *) p = 0x0000000000000000  ← 空指针！

(lldb) disassemble -p
->  0x100001234 <+20>: movl   $0x2a, (%rax)  ; 写入 42 到 rax 指向的地址
    0x100001237 <+23>: addq   $0x10, %rsp

(lldb) register read rax
     rax = 0x0000000000000000  ← rax 为空

(lldb) image lookup -a 0x0000000100001234
      Address: MyApp[0x0000000100001234] (MyApp.__TEXT.__text + 20)
      Summary: MyApp`crash_func() + 20 at main.cpp:10
```

---

## 7. 符号管理

### 7.1 符号文件对照表

| 平台 | 调试信息 | 符号文件 | 生成方式 |
|------|----------|----------|----------|
| Windows | PDB | .pdb | 编译时自动生成 |
| Linux | DWARF | 内嵌在 ELF 或 .debug | -g 编译 |
| macOS | DWARF | .dSYM | dsymutil 生成 |

### 7.2 符号服务器

```
┌─────────────────────────────────────────────────────────────┐
│                     符号服务器架构                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                 ┌─────────────────────┐                     │
│                 │     调试器           │                     │
│                 │  (WinDbg/LLDB)      │                     │
│                 └──────────┬──────────┘                     │
│                            │ 请求符号                        │
│                            ↓                                │
│  ┌──────────────────────────────────────────────────────┐  │
│  │                   符号服务器                           │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐      │  │
│  │  │ 本地缓存   │→ │ 内部服务器  │→ │ 公共服务器  │      │  │
│  │  │            │  │ (自建)     │  │ (微软等)   │      │  │
│  │  └────────────┘  └────────────┘  └────────────┘      │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
│  符号索引：GUID + Age（PDB）/ Build ID（ELF）               │
│  确保符号与二进制完全匹配                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```bash
# Windows 符号服务器设置
# WinDbg 中：
.sympath srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
.sympath+ srv*C:\Symbols*\\myserver\symbols  # 内部符号服务器

# 上传符号到服务器（symstore）
symstore add /r /f *.pdb /s \\myserver\symbols /t "MyApp" /v "1.0.0"

# Linux/macOS debuginfod 服务
export DEBUGINFOD_URLS="https://debuginfod.elfutils.org/"
# GDB/LLDB 会自动下载调试信息
```

---

## 8. 面试深度问题

### Q1: 如何分析无符号的 dump 文件？

**答案**：

```
无符号分析策略：

1. 反汇编分析
   - 通过 disassemble 查看崩溃位置汇编
   - 分析指令和寄存器状态推断逻辑
   - 识别函数调用约定（参数传递）

2. 模块偏移定位
   - 记录崩溃地址相对模块基址的偏移
   - 用相同版本的带符号版本分析偏移对应的函数

3. 内存模式识别
   - 0xCDCDCDCD: 未初始化堆内存 (MSVC Debug)
   - 0xDDDDDDDD: 已释放堆内存
   - 0xFDFDFDFD: 堆边界（越界访问）
   - 0xCCCCCCCC: 未初始化栈内存
   - 0xDEADBEEF: 常见魔数

4. 字符串搜索
   - 搜索内存中的字符串常量
   - 可能找到函数名、错误信息

5. 结合日志
   - 崩溃前的日志帮助缩小范围
   - 分析代码路径

示例：
(lldb) memory find -s "Error" 0x100000000 0x100100000
(lldb) memory read --format s 0x100012340
```

### Q2: Release 版本崩溃定位有什么技巧？

**答案**：

```
Release 调试难点：
- 编译优化导致代码重排
- 内联展开使栈回溯不完整
- 局部变量被优化掉

技巧：

1. 保留必要调试信息
   MSVC:  /Zi /DEBUG /OPT:REF
   GCC:   -O2 -g -fno-omit-frame-pointer
   Clang: -O2 -g -fno-omit-frame-pointer

2. 帧指针保留
   -fno-omit-frame-pointer  确保栈回溯完整

3. 使用 AddressSanitizer
   即使 Release 也能检测内存问题

4. 映射表
   - 保存地址到函数的映射
   - IDA/Ghidra 辅助分析

5. 日志埋点
   关键路径打印函数进入/退出日志

6. 二分定位
   - 二分注释代码找到问题模块
   - 逐步缩小范围
```

### Q3: 堆损坏导致的延迟崩溃如何定位？

**答案**：

```
延迟崩溃原因：
堆元数据被破坏后，可能在下一次 malloc/free 时才崩溃，
崩溃点不是问题点。

定位方法：

1. 使用页堆（Page Heap）
   # Windows
   gflags /p /enable MyApp.exe /full

   # 原理：每次分配使用独立页，访问越界立即崩溃

2. AddressSanitizer
   clang++ -fsanitize=address -g main.cpp
   # 检测 heap-buffer-overflow, use-after-free

3. Valgrind
   valgrind --tool=memcheck ./MyApp
   # 报告所有非法内存访问

4. Debug CRT (MSVC)
   _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
   # 每次堆操作都检查完整性

5. 自定义分配器
   - 前后加 guard bytes
   - 记录分配调用栈
   - 检测越界写入

6. rr (Record and Replay)
   rr record ./MyApp
   rr replay
   # 反向调试，从崩溃点倒推
```

### Q4: ASLR 对崩溃分析有什么影响？如何处理？

**答案**：

```
ASLR = Address Space Layout Randomization
每次运行，模块加载地址随机

影响：
- 崩溃地址每次不同
- 不能直接用地址定位

处理方法：

1. 计算偏移
   偏移 = 崩溃地址 - 模块基址
   然后用偏移在符号表中查找

2. 从 dump/crash report 获取基址
   Windows: lm 命令查看模块基址
   macOS: Binary Images 部分

3. 禁用 ASLR（调试时）
   # macOS
   $ sudo nvram boot-args="amfi_get_out_of_my_way=1"

   # Linux
   $ echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

   # Windows (Visual Studio)
   链接选项: /DYNAMICBASE:NO

4. 使用 Build ID 匹配
   确保 dump 和符号文件 Build ID 一致
```

### Q5: 多线程程序崩溃如何确定哪个线程有问题？

**答案**：

```
分析步骤：

1. 查看所有线程栈
   WinDbg: ~*k
   LLDB:   bt all
   GDB:    thread apply all bt

2. 找到异常线程
   - 异常上下文指向的线程
   - 崩溃信号发送的目标线程

3. 分析异常线程
   WinDbg: ~<n>s 切换线程，.ecxr 切换到异常上下文
   LLDB:   thread select <n>

4. 检查共享数据
   - 崩溃时访问的地址是否被其他线程修改
   - 查看锁状态

5. 时序问题
   - 使用 TSan 检测数据竞争
   - 日志时间戳分析

6. 死锁检测
   WinDbg: !locks
   LLDB:   thread list（查看等待状态）
```

### Q6: 如何在生产环境捕获崩溃？

**答案**：

```cpp
// 生产环境崩溃捕获方案

// 1. Windows - SetUnhandledExceptionFilter
LONG WINAPI ProductionCrashHandler(EXCEPTION_POINTERS* ep) {
    // 生成 minidump
    WriteMiniDump(ep);

    // 收集上下文信息
    CollectContext();

    // 异步上报（不阻塞）
    AsyncUploadCrashReport();

    return EXCEPTION_EXECUTE_HANDLER;
}

// 2. Unix - 信号处理
void production_signal_handler(int sig, siginfo_t* info, void* ctx) {
    // 在信号处理函数中只做最少的事
    // 避免调用非 async-signal-safe 函数

    // 写入 crash 标记文件
    int fd = open("/tmp/crash_flag", O_CREAT | O_WRONLY, 0644);
    write(fd, &sig, sizeof(sig));
    close(fd);

    // 让默认处理生成 core
    signal(sig, SIG_DFL);
    raise(sig);
}

// 3. 使用成熟框架
// - Google Breakpad / Crashpad
// - Sentry Native SDK
// - Bugly (腾讯)

// 关键点：
// - 最小化崩溃处理代码
// - 异步上报
// - 保留符号文件用于后续分析
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   崩溃分析知识图谱                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  崩溃类型                                                   │
│  ├── 空指针 / 野指针 → SIGSEGV / Access Violation          │
│  ├── 栈溢出 → Stack Overflow                               │
│  ├── 堆损坏 → SIGABRT / Heap Corruption                    │
│  └── 除零 / 非法指令 → SIGFPE / SIGILL                     │
│                                                             │
│  分析工具                                                   │
│  ├── Windows: WinDbg, Visual Studio                        │
│  ├── macOS: LLDB, atos, Instruments                        │
│  └── Linux: GDB, addr2line, Valgrind                       │
│                                                             │
│  核心命令                                                   │
│  ├── WinDbg: !analyze -v, k, .ecxr, dd                     │
│  ├── LLDB: bt, frame variable, memory read                 │
│  └── 通用: 查栈、查寄存器、查内存                           │
│                                                             │
│  符号管理                                                   │
│  ├── Windows: PDB + 符号服务器                             │
│  ├── macOS: dSYM + atos                                    │
│  └── Linux: DWARF + debuginfod                             │
│                                                             │
│  面试重点                                                   │
│  ├── 无符号分析技巧                                        │
│  ├── Release 版本调试                                      │
│  ├── 堆损坏延迟崩溃定位                                    │
│  ├── ASLR 影响与处理                                       │
│  └── 生产环境崩溃捕获                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
