# 崩溃捕获机制 (Crash Capture Mechanism)

深入理解操作系统级别的异常捕获机制，是区分资深工程师与普通开发者的重要标准。

---

## 1. 概述与平台对比

### 1.1 异常处理层次

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        异常处理层次模型                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                         应用层                                       │   │
│  │   C++ try/catch  │  Objective-C @try  │  语言级异常处理              │   │
│  └──────────────────────────────┬──────────────────────────────────────┘   │
│                                 │                                           │
│  ┌──────────────────────────────▼──────────────────────────────────────┐   │
│  │                       运行时/框架层                                   │   │
│  │   Windows: SEH    │   macOS: NSException  │  平台异常机制            │   │
│  └──────────────────────────────┬──────────────────────────────────────┘   │
│                                 │                                           │
│  ┌──────────────────────────────▼──────────────────────────────────────┐   │
│  │                        操作系统层                                     │   │
│  │   Windows: VEH/SEH  │  macOS: Mach Exception / POSIX Signal         │   │
│  └──────────────────────────────┬──────────────────────────────────────┘   │
│                                 │                                           │
│  ┌──────────────────────────────▼──────────────────────────────────────┐   │
│  │                         内核层                                        │   │
│  │   CPU 异常 → 中断向量表 → 内核异常处理 → 用户态分发                  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Windows vs macOS 机制对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Windows vs macOS 异常机制对比                            │
├─────────────────────┬─────────────────────┬─────────────────────────────────┤
│        特性         │      Windows        │           macOS                 │
├─────────────────────┼─────────────────────┼─────────────────────────────────┤
│ 底层机制            │ SEH (结构化异常)    │ Mach Exception                  │
│ 用户态信号          │ 无 (SEH 统一)       │ POSIX Signal                    │
│ 向量化处理          │ VEH                 │ 无直接等价物                    │
│ 异常链              │ FS:[0] / GS:[0]     │ exception_handler 线程          │
│ 语法支持            │ __try/__except      │ signal() / sigaction()          │
│ C++ 集成            │ /EHa 混合模式       │ 独立体系                        │
│ 调试器接口          │ DEBUG_EVENT         │ task_set_exception_ports        │
│ 异常代码            │ NTSTATUS (32-bit)   │ exception_type + code           │
│ 典型框架            │ Crashpad            │ Breakpad / Crashpad             │
└─────────────────────┴─────────────────────┴─────────────────────────────────┘
```

### 1.3 异常处理流程对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        异常处理流程                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Windows:                                                                   │
│  ┌────────┐   ┌────────┐   ┌────────┐   ┌────────┐   ┌────────┐           │
│  │CPU异常 │ → │ 内核   │ → │  VEH   │ → │  SEH   │ → │ 未处理 │           │
│  │        │   │KiDispatch│  │(向量化)│   │(帧链)  │   │异常过滤│           │
│  └────────┘   └────────┘   └────────┘   └────────┘   └────────┘           │
│                                                                             │
│  macOS:                                                                     │
│  ┌────────┐   ┌────────┐   ┌────────────┐   ┌────────┐   ┌────────┐       │
│  │CPU异常 │ → │Mach内核│ → │Mach Exception│ → │ 转换为 │ → │ POSIX  │       │
│  │        │   │        │   │Handler线程 │   │ Signal │   │ Handler│       │
│  └────────┘   └────────┘   └────────────┘   └────────┘   └────────┘       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Windows 异常处理体系

### 2.1 基础：SEH 语法

#### __try / __except / __finally

```cpp
#include <Windows.h>
#include <stdio.h>

// 基础 SEH 用法
void basic_seh_example() {
    int* p = nullptr;

    __try {
        // 保护代码块
        *p = 42;  // 触发 ACCESS_VIOLATION
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // 异常处理块
        printf("Caught exception!\n");
    }

    printf("Continuing after exception...\n");
}

// __finally 保证执行
void finally_example() {
    HANDLE hFile = INVALID_HANDLE_VALUE;

    __try {
        hFile = CreateFileW(L"test.txt", GENERIC_READ, 0,
                            nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            return;  // 即使 return，__finally 也会执行
        }
        // 使用文件...
    }
    __finally {
        // 无论如何都会执行（正常退出、return、异常）
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            printf("File handle closed in __finally\n");
        }
    }
}

// 获取异常信息
void exception_info_example() {
    __try {
        int* p = nullptr;
        *p = 42;
    }
    __except (
        printf("Exception Code: 0x%08X\n", GetExceptionCode()),
        EXCEPTION_EXECUTE_HANDLER
    ) {
        printf("In exception handler\n");
    }
}
```

#### 异常代码详解

```cpp
// 常见异常代码及其含义
void print_exception_info(DWORD code, EXCEPTION_POINTERS* ep) {
    printf("Exception Code: 0x%08X\n", code);

    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:  // 0xC0000005
            printf("Access Violation\n");
            // ExceptionInformation[0]: 0=读, 1=写, 8=DEP
            // ExceptionInformation[1]: 目标地址
            if (ep) {
                ULONG_PTR operation = ep->ExceptionRecord->ExceptionInformation[0];
                ULONG_PTR address = ep->ExceptionRecord->ExceptionInformation[1];
                printf("  %s address 0x%p\n",
                       operation == 0 ? "Reading" :
                       operation == 1 ? "Writing" : "DEP violation at",
                       (void*)address);
            }
            break;

        case EXCEPTION_STACK_OVERFLOW:    // 0xC00000FD
            printf("Stack Overflow\n");
            break;

        case EXCEPTION_INT_DIVIDE_BY_ZERO:  // 0xC0000094
            printf("Integer Divide by Zero\n");
            break;

        case EXCEPTION_ILLEGAL_INSTRUCTION:  // 0xC000001D
            printf("Illegal Instruction\n");
            break;

        case EXCEPTION_BREAKPOINT:  // 0x80000003
            printf("Breakpoint\n");
            break;

        case 0xE06D7363:  // C++ Exception ("msc" in ASCII)
            printf("C++ Exception\n");
            break;

        default:
            printf("Unknown exception\n");
    }
}
```

### 2.2 进阶：异常过滤器

#### 过滤器返回值

```cpp
/*
 * 异常过滤器返回值：
 *
 * EXCEPTION_EXECUTE_HANDLER (1)
 *   - 执行 __except 块
 *   - 栈展开，销毁局部对象
 *
 * EXCEPTION_CONTINUE_SEARCH (0)
 *   - 不处理，继续搜索上层处理器
 *   - 传递给外层 __try 或 VEH
 *
 * EXCEPTION_CONTINUE_EXECUTION (-1)
 *   - 修复问题后重新执行导致异常的指令
 *   - 危险！可能导致无限循环
 */

// 复杂过滤器示例
DWORD FilterExpression(EXCEPTION_POINTERS* ep) {
    DWORD code = ep->ExceptionRecord->ExceptionCode;

    // 只处理访问违规
    if (code == EXCEPTION_ACCESS_VIOLATION) {
        // 检查是否是可修复的情况
        ULONG_PTR address = ep->ExceptionRecord->ExceptionInformation[1];

        // 如果是空指针，记录并处理
        if (address == 0) {
            LogCrash(ep);
            return EXCEPTION_EXECUTE_HANDLER;
        }

        // 其他访问违规，尝试修复（如提交虚拟内存页）
        if (TryFixMemory(address)) {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    // 其他异常，传递给上层
    return EXCEPTION_CONTINUE_SEARCH;
}

void advanced_filter_example() {
    __try {
        // 危险代码
        int* p = nullptr;
        *p = 42;
    }
    __except (FilterExpression(GetExceptionInformation())) {
        printf("Exception handled\n");
    }
}
```

#### 嵌套 SEH

```cpp
void nested_seh_example() {
    __try {
        __try {
            __try {
                int* p = nullptr;
                *p = 42;
            }
            __except (
                printf("Inner filter\n"),
                EXCEPTION_CONTINUE_SEARCH  // 不处理，传递给上层
            ) {
                printf("Inner handler (won't reach)\n");
            }
        }
        __except (
            printf("Middle filter\n"),
            EXCEPTION_EXECUTE_HANDLER  // 处理异常
        ) {
            printf("Middle handler\n");
        }
    }
    __except (
        printf("Outer filter (won't reach)\n"),
        EXCEPTION_EXECUTE_HANDLER
    ) {
        printf("Outer handler (won't reach)\n");
    }
}

// 输出：
// Inner filter
// Middle filter
// Middle handler
```

### 2.3 深入：SEH 链与栈展开

#### SEH 链结构

```cpp
/*
 * x86 SEH 链 (32位)
 *
 * FS:[0] → EXCEPTION_REGISTRATION_RECORD
 *            │
 *            ├── Next → 下一个处理器
 *            └── Handler → 异常处理函数
 *
 * x64 SEH (64位)
 *
 * 不再使用链表，而是基于 .pdata 段的表驱动
 * 编译器生成 RUNTIME_FUNCTION 表
 */

// x86 手动注册 SEH 处理器 (仅演示原理)
#ifdef _M_IX86

typedef struct _EXCEPTION_REGISTRATION {
    struct _EXCEPTION_REGISTRATION* Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION;

EXCEPTION_DISPOSITION __cdecl
my_seh_handler(
    EXCEPTION_RECORD* ExceptionRecord,
    void* EstablisherFrame,
    CONTEXT* ContextRecord,
    void* DispatcherContext
) {
    printf("My SEH handler called\n");
    printf("Exception: 0x%08X\n", ExceptionRecord->ExceptionCode);

    // ExceptionContinueExecution = 0  继续执行
    // ExceptionContinueSearch = 1     搜索下一个
    // ExceptionNestedException = 2    嵌套异常
    // ExceptionCollidedUnwind = 3     展开冲突

    return ExceptionContinueSearch;
}

void manual_seh_registration() {
    EXCEPTION_REGISTRATION registration;

    // 读取当前 SEH 链头
    __asm {
        mov eax, fs:[0]
        mov registration.Next, eax
    }

    registration.Handler = my_seh_handler;

    // 插入到链头
    __asm {
        lea eax, registration
        mov fs:[0], eax
    }

    // 触发异常
    __try {
        int* p = nullptr;
        *p = 42;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("Caught!\n");
    }

    // 恢复 SEH 链
    __asm {
        mov eax, registration.Next
        mov fs:[0], eax
    }
}

#endif // _M_IX86
```

#### 栈展开 (Stack Unwinding)

```cpp
/*
 * 栈展开过程：
 *
 * 1. 异常发生
 * 2. 遍历 SEH 链，寻找处理器
 * 3. 找到处理器后，开始展开
 * 4. 从异常点到处理器之间的所有帧：
 *    - 调用 __finally 块
 *    - 调用 C++ 析构函数
 * 5. 执行 __except 块
 */

class Resource {
public:
    Resource(const char* name) : name_(name) {
        printf("Resource '%s' acquired\n", name_);
    }
    ~Resource() {
        printf("Resource '%s' released\n", name_);
    }
private:
    const char* name_;
};

void inner_function() {
    Resource r("inner");
    int* p = nullptr;
    *p = 42;  // 异常
}

void middle_function() {
    Resource r("middle");
    inner_function();
}

void outer_function() {
    Resource r("outer");

    __try {
        middle_function();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("Exception handled\n");
    }
}

// 输出（启用 /EHa）：
// Resource 'outer' acquired
// Resource 'middle' acquired
// Resource 'inner' acquired
// Resource 'inner' released   ← 栈展开
// Resource 'middle' released  ← 栈展开
// Exception handled
// Resource 'outer' released   ← 正常退出
```

### 2.4 高级：VEH (Vectored Exception Handler)

#### VEH vs SEH

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         VEH vs SEH 对比                                     │
├─────────────────────┬─────────────────────┬─────────────────────────────────┤
│        特性         │        VEH          │            SEH                  │
├─────────────────────┼─────────────────────┼─────────────────────────────────┤
│ 作用域              │ 进程全局            │ 线程/函数局部                   │
│ 注册方式            │ API 函数            │ __try/__except 语法             │
│ 执行顺序            │ 先于 SEH            │ 后于 VEH                        │
│ 调试器通知          │ 在调试器之后        │ 在调试器之后                    │
│ 可以修改上下文      │ 是                  │ 是                              │
│ 适用场景            │ 全局监控、调试器    │ 局部错误处理                    │
│ 性能影响            │ 每个异常都触发      │ 只在作用域内触发                │
└─────────────────────┴─────────────────────┴─────────────────────────────────┘
```

#### VEH 实现

```cpp
#include <Windows.h>
#include <stdio.h>

// VEH 处理器
LONG CALLBACK VectoredHandler(EXCEPTION_POINTERS* ep) {
    DWORD code = ep->ExceptionRecord->ExceptionCode;

    printf("[VEH] Exception: 0x%08X at 0x%p\n",
           code, ep->ExceptionRecord->ExceptionAddress);

    // 打印调用栈
    PrintStackTrace(ep->ContextRecord);

    // 返回值决定后续行为
    // EXCEPTION_CONTINUE_SEARCH: 继续搜索其他处理器
    // EXCEPTION_CONTINUE_EXECUTION: 修复后继续执行

    return EXCEPTION_CONTINUE_SEARCH;
}

// 第一个 VEH（最先执行）
LONG CALLBACK FirstChanceHandler(EXCEPTION_POINTERS* ep) {
    printf("[First Chance] Code: 0x%08X\n",
           ep->ExceptionRecord->ExceptionCode);

    // 如果是断点，可以处理
    if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
        // 跳过 int 3 指令
        #ifdef _M_X64
        ep->ContextRecord->Rip++;
        #else
        ep->ContextRecord->Eip++;
        #endif
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void setup_veh() {
    // 注册 VEH
    // 参数 1: First=1 表示插入到链头（最先执行）
    PVOID handle1 = AddVectoredExceptionHandler(1, FirstChanceHandler);
    PVOID handle2 = AddVectoredExceptionHandler(0, VectoredHandler);

    // 执行可能触发异常的代码...

    // 移除 VEH
    RemoveVectoredExceptionHandler(handle1);
    RemoveVectoredExceptionHandler(handle2);
}
```

#### 使用 VEH 实现简单调试器

```cpp
// 使用 VEH 实现软件断点

#include <Windows.h>
#include <map>

class SimpleDebugger {
public:
    static SimpleDebugger& Instance() {
        static SimpleDebugger instance;
        return instance;
    }

    // 设置断点
    void SetBreakpoint(void* address) {
        DWORD oldProtect;
        VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);

        // 保存原始字节
        BYTE original = *(BYTE*)address;
        breakpoints_[address] = original;

        // 写入 int 3 (0xCC)
        *(BYTE*)address = 0xCC;

        VirtualProtect(address, 1, oldProtect, &oldProtect);
        FlushInstructionCache(GetCurrentProcess(), address, 1);
    }

    // 移除断点
    void RemoveBreakpoint(void* address) {
        auto it = breakpoints_.find(address);
        if (it != breakpoints_.end()) {
            DWORD oldProtect;
            VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)address = it->second;
            VirtualProtect(address, 1, oldProtect, &oldProtect);
            FlushInstructionCache(GetCurrentProcess(), address, 1);
            breakpoints_.erase(it);
        }
    }

private:
    SimpleDebugger() {
        veh_handle_ = AddVectoredExceptionHandler(1, VEHHandler);
    }

    ~SimpleDebugger() {
        if (veh_handle_) {
            RemoveVectoredExceptionHandler(veh_handle_);
        }
    }

    static LONG CALLBACK VEHHandler(EXCEPTION_POINTERS* ep) {
        if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
            void* address = ep->ExceptionRecord->ExceptionAddress;
            auto& self = Instance();

            auto it = self.breakpoints_.find(address);
            if (it != self.breakpoints_.end()) {
                printf("[Breakpoint Hit] Address: %p\n", address);

                // 恢复原始指令
                DWORD oldProtect;
                VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(BYTE*)address = it->second;
                VirtualProtect(address, 1, oldProtect, &oldProtect);

                // 不调整 IP，让原始指令重新执行
                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }

    PVOID veh_handle_ = nullptr;
    std::map<void*, BYTE> breakpoints_;
};
```

### 2.5 实战：完整崩溃捕获框架

```cpp
#include <Windows.h>
#include <DbgHelp.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "DbgHelp.lib")

class CrashHandler {
public:
    static void Install() {
        // 1. 安装 VEH（第一道防线）
        AddVectoredExceptionHandler(1, VectoredHandler);

        // 2. 安装未处理异常过滤器（最后一道防线）
        SetUnhandledExceptionFilter(UnhandledExceptionHandler);

        // 3. 捕获 CRT 错误
        _set_invalid_parameter_handler(InvalidParameterHandler);
        _set_purecall_handler(PureCallHandler);

        // 4. 捕获 terminate
        set_terminate(TerminateHandler);

        // 5. 捕获 SIGABRT (abort)
        signal(SIGABRT, SignalHandler);
    }

private:
    // VEH 处理器 - 记录所有异常
    static LONG CALLBACK VectoredHandler(EXCEPTION_POINTERS* ep) {
        // 记录异常信息（不处理，只记录）
        LogException("VEH", ep);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // 未处理异常过滤器
    static LONG CALLBACK UnhandledExceptionHandler(EXCEPTION_POINTERS* ep) {
        LogException("Unhandled", ep);
        WriteMiniDump(ep);
        NotifyCrashServer(ep);

        // 返回值选择：
        // EXCEPTION_EXECUTE_HANDLER: 终止进程
        // EXCEPTION_CONTINUE_SEARCH: 让 Windows 显示错误对话框
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // CRT 无效参数处理
    static void InvalidParameterHandler(
        const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line,
        uintptr_t reserved
    ) {
        wprintf(L"Invalid parameter: %s\nFunction: %s\nFile: %s:%u\n",
                expression, function, file, line);

        // 生成 dump
        RaiseException(STATUS_INVALID_PARAMETER, 0, 0, nullptr);
    }

    // 纯虚函数调用处理
    static void PureCallHandler() {
        printf("Pure virtual function call!\n");
        RaiseException(STATUS_NONCONTINUABLE_EXCEPTION, 0, 0, nullptr);
    }

    // terminate 处理
    static void TerminateHandler() {
        printf("std::terminate called!\n");

        // 尝试获取当前异常
        try {
            throw;
        } catch (const std::exception& e) {
            printf("Exception: %s\n", e.what());
        } catch (...) {
            printf("Unknown exception\n");
        }

        RaiseException(STATUS_NONCONTINUABLE_EXCEPTION, 0, 0, nullptr);
    }

    // 信号处理
    static void SignalHandler(int signal) {
        printf("Signal %d received\n", signal);
        RaiseException(STATUS_NONCONTINUABLE_EXCEPTION, 0, 0, nullptr);
    }

    // 生成 Minidump
    static void WriteMiniDump(EXCEPTION_POINTERS* ep) {
        // 生成文件名
        wchar_t filename[MAX_PATH];
        time_t now = time(nullptr);
        struct tm tm_now;
        localtime_s(&tm_now, &now);
        swprintf_s(filename, L"crash_%04d%02d%02d_%02d%02d%02d.dmp",
                   tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
                   tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

        HANDLE hFile = CreateFileW(filename, GENERIC_WRITE, 0, nullptr,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hFile != INVALID_HANDLE_VALUE) {
            MINIDUMP_EXCEPTION_INFORMATION mei;
            mei.ThreadId = GetCurrentThreadId();
            mei.ExceptionPointers = ep;
            mei.ClientPointers = FALSE;

            MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                (MINIDUMP_TYPE)(
                    MiniDumpWithDataSegs |
                    MiniDumpWithHandleData |
                    MiniDumpWithThreadInfo |
                    MiniDumpWithUnloadedModules
                ),
                &mei,
                nullptr,
                nullptr
            );

            CloseHandle(hFile);
            wprintf(L"Dump saved: %s\n", filename);
        }
    }

    static void LogException(const char* source, EXCEPTION_POINTERS* ep) {
        printf("[%s] Exception 0x%08X at %p\n",
               source,
               ep->ExceptionRecord->ExceptionCode,
               ep->ExceptionRecord->ExceptionAddress);
    }

    static void NotifyCrashServer(EXCEPTION_POINTERS* ep) {
        // 异步上报崩溃信息到服务器
        // ...
    }
};

// 使用
int main() {
    CrashHandler::Install();

    // 应用程序代码...

    return 0;
}
```

---

## 3. macOS 异常处理体系

### 3.1 基础：POSIX 信号

#### signal 与 sigaction

```cpp
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>

// 简单信号处理（不推荐生产使用）
void simple_signal_handler(int sig) {
    printf("Caught signal %d\n", sig);
    exit(1);
}

// 推荐：使用 sigaction
void sigaction_handler(int sig, siginfo_t* info, void* context) {
    // siginfo_t 包含更多信息
    printf("Signal: %d\n", sig);
    printf("Fault address: %p\n", info->si_addr);
    printf("Error code: %d\n", info->si_code);

    // 获取寄存器上下文
    ucontext_t* uctx = (ucontext_t*)context;
    #if defined(__x86_64__)
    printf("RIP: 0x%llx\n", uctx->uc_mcontext->__ss.__rip);
    printf("RSP: 0x%llx\n", uctx->uc_mcontext->__ss.__rsp);
    #elif defined(__aarch64__)
    printf("PC: 0x%llx\n", uctx->uc_mcontext->__ss.__pc);
    printf("SP: 0x%llx\n", uctx->uc_mcontext->__ss.__sp);
    #endif

    // 打印调用栈
    void* bt[64];
    int bt_size = backtrace(bt, 64);
    backtrace_symbols_fd(bt, bt_size, STDERR_FILENO);

    // 重置处理器，让默认行为生成 core dump
    signal(sig, SIG_DFL);
    raise(sig);
}

void install_signal_handlers() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigaction_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND;  // 获取详细信息 + 只触发一次

    // 安装处理器
    sigaction(SIGSEGV, &sa, nullptr);  // 段错误
    sigaction(SIGBUS, &sa, nullptr);   // 总线错误
    sigaction(SIGABRT, &sa, nullptr);  // abort
    sigaction(SIGFPE, &sa, nullptr);   // 浮点异常
    sigaction(SIGILL, &sa, nullptr);   // 非法指令
    sigaction(SIGTRAP, &sa, nullptr);  // 调试陷阱
}
```

#### 信号处理限制 (async-signal-safe)

```cpp
/*
 * 信号处理函数只能调用 async-signal-safe 函数
 *
 * 安全函数（部分）：
 * - write()
 * - _exit()
 * - signal()
 * - raise()
 *
 * 不安全函数（常见）：
 * - printf()  (使用了锁)
 * - malloc()  (使用了锁)
 * - new/delete
 * - std::string 操作
 * - 大多数 C++ 标准库
 *
 * 原因：信号可能在持有锁时到达，如果处理函数也要获取同一把锁，就会死锁
 */

// 安全的信号处理方式
void safe_signal_handler(int sig, siginfo_t* info, void* context) {
    // 只使用 async-signal-safe 函数

    // 写入文件描述符
    const char msg[] = "Crash detected\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);

    // 设置全局标志（volatile sig_atomic_t 是安全的）
    static volatile sig_atomic_t crashed = 0;
    crashed = 1;

    // 恢复默认处理
    signal(sig, SIG_DFL);
    raise(sig);
}

// 使用管道延迟处理（安全模式）
int g_crash_pipe[2];

void async_safe_handler(int sig, siginfo_t* info, void* context) {
    // 只做最少的事：写入信号到管道
    int saved_errno = errno;
    write(g_crash_pipe[1], &sig, sizeof(sig));
    errno = saved_errno;
}

void setup_deferred_handler() {
    pipe(g_crash_pipe);

    // 在另一个线程中读取管道
    std::thread([]{
        int sig;
        while (read(g_crash_pipe[0], &sig, sizeof(sig)) > 0) {
            // 这里可以安全地调用任何函数
            printf("Deferred handling of signal %d\n", sig);
            GenerateCrashReport();
        }
    }).detach();
}
```

### 3.2 进阶：Mach Exception

#### Mach 与 BSD 的关系

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    macOS 内核架构 (XNU)                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        用户空间                                      │   │
│  │  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐     │   │
│  │  │   应用程序       │  │   POSIX API     │  │   Mach API      │     │   │
│  │  │                 │  │  signal/pthread │  │  mach_msg/task  │     │   │
│  │  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘     │   │
│  └───────────┼────────────────────┼────────────────────┼───────────────┘   │
│              │                    │                    │                    │
│  ┌───────────▼────────────────────▼────────────────────▼───────────────┐   │
│  │                         XNU 内核                                     │   │
│  │  ┌──────────────────────────────────────────────────────────────┐  │   │
│  │  │                      BSD 层                                   │  │   │
│  │  │   进程、文件系统、网络、POSIX 信号                            │  │   │
│  │  │                                                              │  │   │
│  │  │   ┌──────────────────────────────────────────────────────┐  │  │   │
│  │  │   │                  Mach 层                              │  │  │   │
│  │  │   │   任务、线程、IPC、虚拟内存、异常处理                 │  │  │   │
│  │  │   │                                                      │  │  │   │
│  │  │   │   异常发生 → Mach Exception → (可选) 转换为 Signal    │  │  │   │
│  │  │   └──────────────────────────────────────────────────────┘  │  │   │
│  │  └──────────────────────────────────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  异常处理顺序：                                                             │
│  1. CPU 异常                                                               │
│  2. Mach 内核捕获                                                          │
│  3. 发送到 Mach exception port（如果设置了）                               │
│  4. 如果没有 Mach handler 或 handler 不处理                                │
│  5. 转换为 POSIX signal                                                    │
│  6. 调用 signal handler                                                    │
│  7. 如果没有 handler，默认行为（终止/core dump）                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

#### Mach Exception 类型

```cpp
/*
 * Mach Exception 类型
 *
 * EXC_BAD_ACCESS      - 内存访问错误 (对应 SIGSEGV/SIGBUS)
 *   code: KERN_INVALID_ADDRESS, KERN_PROTECTION_FAILURE
 *
 * EXC_BAD_INSTRUCTION - 非法指令 (对应 SIGILL)
 *
 * EXC_ARITHMETIC      - 算术异常 (对应 SIGFPE)
 *
 * EXC_EMULATION       - 模拟异常
 *
 * EXC_SOFTWARE        - 软件异常
 *   code: EXC_UNIX_ABORT (对应 SIGABRT)
 *
 * EXC_BREAKPOINT      - 断点 (对应 SIGTRAP)
 *
 * EXC_CRASH           - 进程即将崩溃
 *   只发送到 task exception port，不发送到 thread port
 */

#include <mach/mach.h>

void print_exception_type(exception_type_t type) {
    switch (type) {
        case EXC_BAD_ACCESS:
            printf("EXC_BAD_ACCESS (memory access error)\n");
            break;
        case EXC_BAD_INSTRUCTION:
            printf("EXC_BAD_INSTRUCTION (illegal instruction)\n");
            break;
        case EXC_ARITHMETIC:
            printf("EXC_ARITHMETIC (arithmetic error)\n");
            break;
        case EXC_BREAKPOINT:
            printf("EXC_BREAKPOINT (breakpoint/trap)\n");
            break;
        case EXC_SOFTWARE:
            printf("EXC_SOFTWARE (software exception)\n");
            break;
        case EXC_CRASH:
            printf("EXC_CRASH (process crashing)\n");
            break;
        default:
            printf("Unknown exception type: %d\n", type);
    }
}
```

### 3.3 深入：Mach Exception Handler

#### 设置 Exception Port

```cpp
#include <mach/mach.h>
#include <pthread.h>

// 异常处理线程
static mach_port_t g_exception_port;
static pthread_t g_exception_thread;

// 异常处理线程函数
void* exception_handler_thread(void* arg) {
    kern_return_t kr;

    while (1) {
        // 接收异常消息
        struct {
            mach_msg_header_t header;
            mach_msg_body_t body;
            mach_msg_port_descriptor_t thread;
            mach_msg_port_descriptor_t task;
            NDR_record_t NDR;
            exception_type_t exception;
            mach_msg_type_number_t code_count;
            mach_exception_data_t code;
            mach_msg_trailer_t trailer;
        } request;

        struct {
            mach_msg_header_t header;
            NDR_record_t NDR;
            kern_return_t return_code;
        } reply;

        // 等待异常消息
        kr = mach_msg(&request.header,
                      MACH_RCV_MSG,
                      0,
                      sizeof(request),
                      g_exception_port,
                      MACH_MSG_TIMEOUT_NONE,
                      MACH_PORT_NULL);

        if (kr != KERN_SUCCESS) {
            printf("mach_msg receive failed: %d\n", kr);
            continue;
        }

        // 处理异常
        printf("Exception received:\n");
        printf("  Type: %d\n", request.exception);
        printf("  Code count: %d\n", request.code_count);
        for (int i = 0; i < request.code_count; i++) {
            printf("  Code[%d]: 0x%llx\n", i, request.code[i]);
        }

        // 获取线程状态
        thread_state_flavor_t flavor;
        mach_msg_type_number_t state_count;

        #if defined(__x86_64__)
        x86_thread_state64_t state;
        flavor = x86_THREAD_STATE64;
        state_count = x86_THREAD_STATE64_COUNT;
        #elif defined(__aarch64__)
        arm_thread_state64_t state;
        flavor = ARM_THREAD_STATE64;
        state_count = ARM_THREAD_STATE64_COUNT;
        #endif

        kr = thread_get_state(request.thread.name, flavor,
                              (thread_state_t)&state, &state_count);

        if (kr == KERN_SUCCESS) {
            #if defined(__x86_64__)
            printf("  RIP: 0x%llx\n", state.__rip);
            printf("  RSP: 0x%llx\n", state.__rsp);
            #elif defined(__aarch64__)
            printf("  PC: 0x%llx\n", state.__pc);
            printf("  SP: 0x%llx\n", state.__sp);
            #endif
        }

        // 生成崩溃报告
        GenerateCrashReport(request.thread.name, request.exception, request.code);

        // 发送回复（不处理异常，让系统继续默认行为）
        reply.header.msgh_bits =
            MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(request.header.msgh_bits), 0);
        reply.header.msgh_remote_port = request.header.msgh_remote_port;
        reply.header.msgh_size = sizeof(reply);
        reply.header.msgh_local_port = MACH_PORT_NULL;
        reply.header.msgh_id = request.header.msgh_id + 100;
        reply.NDR = NDR_record;
        reply.return_code = KERN_FAILURE;  // 不处理，让系统继续

        mach_msg(&reply.header,
                 MACH_SEND_MSG,
                 sizeof(reply),
                 0,
                 MACH_PORT_NULL,
                 MACH_MSG_TIMEOUT_NONE,
                 MACH_PORT_NULL);
    }

    return nullptr;
}

// 安装 Mach exception handler
bool install_mach_exception_handler() {
    kern_return_t kr;

    // 创建异常端口
    kr = mach_port_allocate(mach_task_self(),
                            MACH_PORT_RIGHT_RECEIVE,
                            &g_exception_port);
    if (kr != KERN_SUCCESS) {
        printf("mach_port_allocate failed: %d\n", kr);
        return false;
    }

    // 添加发送权限
    kr = mach_port_insert_right(mach_task_self(),
                                g_exception_port,
                                g_exception_port,
                                MACH_MSG_TYPE_MAKE_SEND);
    if (kr != KERN_SUCCESS) {
        printf("mach_port_insert_right failed: %d\n", kr);
        return false;
    }

    // 设置为任务的异常端口
    kr = task_set_exception_ports(
        mach_task_self(),
        EXC_MASK_BAD_ACCESS |
        EXC_MASK_BAD_INSTRUCTION |
        EXC_MASK_ARITHMETIC |
        EXC_MASK_BREAKPOINT |
        EXC_MASK_SOFTWARE |
        EXC_MASK_CRASH,          // 重要：捕获 EXC_CRASH
        g_exception_port,
        EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,
        THREAD_STATE_NONE
    );

    if (kr != KERN_SUCCESS) {
        printf("task_set_exception_ports failed: %d\n", kr);
        return false;
    }

    // 启动异常处理线程
    pthread_create(&g_exception_thread, nullptr,
                   exception_handler_thread, nullptr);

    return true;
}
```

### 3.4 为什么 Breakpad/Crashpad 使用 Mach Exception

```
┌─────────────────────────────────────────────────────────────────────────────┐
│              Mach Exception vs POSIX Signal 对比                            │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Signal 的问题：                                                            │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ 1. async-signal-safe 限制                                           │   │
│  │    - 不能调用 malloc/printf/大多数库函数                            │   │
│  │    - 崩溃时收集信息困难                                             │   │
│  │                                                                     │   │
│  │ 2. 在原线程栈上执行                                                 │   │
│  │    - 如果是栈溢出，处理器本身可能也会栈溢出                         │   │
│  │    - 处理器可能破坏现场                                             │   │
│  │                                                                     │   │
│  │ 3. 信号可能被屏蔽                                                   │   │
│  │    - 某些信号可以被进程屏蔽                                         │   │
│  │    - 可能收不到崩溃通知                                             │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Mach Exception 的优势：                                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ 1. 独立线程处理                                                     │   │
│  │    - 异常处理在专门的线程中                                         │   │
│  │    - 有自己独立的栈                                                 │   │
│  │    - 可以安全调用任何函数                                           │   │
│  │                                                                     │   │
│  │ 2. 不破坏现场                                                       │   │
│  │    - 崩溃线程被挂起                                                 │   │
│  │    - 可以完整获取寄存器和栈状态                                     │   │
│  │                                                                     │   │
│  │ 3. 先于 signal 处理                                                 │   │
│  │    - 在转换为 signal 之前就能捕获                                   │   │
│  │    - 更完整的异常信息                                               │   │
│  │                                                                     │   │
│  │ 4. 可以捕获 EXC_CRASH                                               │   │
│  │    - 进程即将终止时的最后通知                                       │   │
│  │    - Signal 没有等价物                                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.5 实战：完整 macOS 崩溃捕获框架

```cpp
#include <mach/mach.h>
#include <signal.h>
#include <pthread.h>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>

class MacCrashHandler {
public:
    static bool Install() {
        // 1. 安装 Mach exception handler（主要方式）
        if (!InstallMachHandler()) {
            return false;
        }

        // 2. 安装 Signal handler（备用）
        InstallSignalHandler();

        // 3. 安装 Objective-C 异常处理（如果使用 ObjC）
        #ifdef __OBJC__
        NSSetUncaughtExceptionHandler(ObjCExceptionHandler);
        #endif

        return true;
    }

private:
    static mach_port_t s_exception_port;
    static pthread_t s_exception_thread;

    static bool InstallMachHandler() {
        kern_return_t kr;

        // 创建端口
        kr = mach_port_allocate(mach_task_self(),
                                MACH_PORT_RIGHT_RECEIVE,
                                &s_exception_port);
        if (kr != KERN_SUCCESS) return false;

        kr = mach_port_insert_right(mach_task_self(),
                                    s_exception_port,
                                    s_exception_port,
                                    MACH_MSG_TYPE_MAKE_SEND);
        if (kr != KERN_SUCCESS) return false;

        // 保存旧的端口
        exception_mask_t masks[EXC_TYPES_COUNT];
        mach_msg_type_number_t count = EXC_TYPES_COUNT;
        mach_port_t ports[EXC_TYPES_COUNT];
        exception_behavior_t behaviors[EXC_TYPES_COUNT];
        thread_state_flavor_t flavors[EXC_TYPES_COUNT];

        task_get_exception_ports(mach_task_self(),
            EXC_MASK_ALL,
            masks, &count, ports, behaviors, flavors);

        // 设置新端口
        kr = task_set_exception_ports(
            mach_task_self(),
            EXC_MASK_BAD_ACCESS |
            EXC_MASK_BAD_INSTRUCTION |
            EXC_MASK_ARITHMETIC |
            EXC_MASK_SOFTWARE |
            EXC_MASK_CRASH,
            s_exception_port,
            EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,
            THREAD_STATE_NONE
        );

        if (kr != KERN_SUCCESS) return false;

        // 启动处理线程
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&s_exception_thread, &attr, ExceptionThread, nullptr);
        pthread_attr_destroy(&attr);

        return true;
    }

    static void* ExceptionThread(void* arg) {
        while (true) {
            // 简化的消息接收
            union {
                mach_msg_header_t header;
                char data[4096];
            } message;

            kern_return_t kr = mach_msg(
                &message.header,
                MACH_RCV_MSG | MACH_RCV_LARGE,
                0,
                sizeof(message),
                s_exception_port,
                MACH_MSG_TIMEOUT_NONE,
                MACH_PORT_NULL
            );

            if (kr == KERN_SUCCESS) {
                HandleMachException(&message.header);
            }
        }
        return nullptr;
    }

    static void HandleMachException(mach_msg_header_t* message) {
        // 解析异常消息并生成报告
        printf("[Mach] Exception received\n");

        // 获取崩溃线程的调用栈
        // 生成 minidump 或崩溃报告
        WriteCrashReport();

        // 不处理异常，让系统继续（转换为 signal）
    }

    static void InstallSignalHandler() {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = SignalHandler;
        sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

        // 使用备用栈（防止栈溢出时无法处理）
        static char alt_stack[SIGSTKSZ];
        stack_t ss;
        ss.ss_sp = alt_stack;
        ss.ss_size = SIGSTKSZ;
        ss.ss_flags = 0;
        sigaltstack(&ss, nullptr);

        sigaction(SIGSEGV, &sa, nullptr);
        sigaction(SIGBUS, &sa, nullptr);
        sigaction(SIGABRT, &sa, nullptr);
        sigaction(SIGFPE, &sa, nullptr);
        sigaction(SIGILL, &sa, nullptr);
    }

    static void SignalHandler(int sig, siginfo_t* info, void* context) {
        // async-signal-safe 方式记录
        const char* msg = "Signal handler called\n";
        write(STDERR_FILENO, msg, strlen(msg));

        // 打印调用栈（backtrace 不是严格 async-signal-safe，但通常可用）
        void* bt[64];
        int bt_size = backtrace(bt, 64);
        backtrace_symbols_fd(bt, bt_size, STDERR_FILENO);

        // 恢复默认处理
        signal(sig, SIG_DFL);
        raise(sig);
    }

    #ifdef __OBJC__
    static void ObjCExceptionHandler(NSException* exception) {
        NSLog(@"Uncaught Objective-C exception: %@", exception);
        NSLog(@"Stack trace: %@", [exception callStackSymbols]);
    }
    #endif

    static void WriteCrashReport() {
        // 写入崩溃报告文件
        // ...
    }
};

mach_port_t MacCrashHandler::s_exception_port = MACH_PORT_NULL;
pthread_t MacCrashHandler::s_exception_thread = nullptr;
```

---

## 4. 跨平台统一方案

### 4.1 抽象层设计

```cpp
// 跨平台崩溃处理抽象

class ICrashHandler {
public:
    virtual ~ICrashHandler() = default;
    virtual bool Install() = 0;
    virtual void Uninstall() = 0;

    // 设置崩溃回调
    using CrashCallback = std::function<void(const CrashInfo&)>;
    virtual void SetCallback(CrashCallback callback) = 0;
};

struct CrashInfo {
    uint64_t crash_address;
    uint64_t instruction_pointer;
    uint64_t stack_pointer;
    int signal_or_exception;
    std::string exception_message;
    std::vector<StackFrame> stack_trace;
    std::map<std::string, std::string> registers;
};

// 工厂函数
std::unique_ptr<ICrashHandler> CreateCrashHandler() {
    #ifdef _WIN32
    return std::make_unique<WindowsCrashHandler>();
    #elif defined(__APPLE__)
    return std::make_unique<MacCrashHandler>();
    #elif defined(__linux__)
    return std::make_unique<LinuxCrashHandler>();
    #else
    return nullptr;
    #endif
}
```

### 4.2 Breakpad/Crashpad 架构简介

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Crashpad 架构                                            │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        客户端应用                                    │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                   Crashpad Client                            │   │   │
│  │  │   - 注册异常处理器                                           │   │   │
│  │  │   - 与 Handler 进程通信                                      │   │   │
│  │  │   - 设置崩溃时的额外信息                                     │   │   │
│  │  └──────────────────────────┬──────────────────────────────────┘   │   │
│  └─────────────────────────────┼───────────────────────────────────────┘   │
│                                │ IPC (Mach/Named Pipe)                      │
│  ┌─────────────────────────────▼───────────────────────────────────────┐   │
│  │                    Crashpad Handler (独立进程)                       │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │  - 接收崩溃通知                                              │   │   │
│  │  │  - 生成 Minidump                                             │   │   │
│  │  │  - 上传到服务器                                              │   │   │
│  │  │  - 不会被客户端崩溃影响                                      │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  优势：                                                                     │
│  - Handler 在独立进程，不受客户端崩溃影响                                  │
│  - 可以在客户端进程完全不可用时仍然工作                                    │
│  - 统一的跨平台 API                                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 5. 面试深度问题

### Q1: SEH 和 C++ 异常有什么关系？

**答案：**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    SEH 与 C++ 异常的关系                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  编译选项影响行为：                                                         │
│                                                                             │
│  /EHsc (默认)                                                              │
│  ├── 只捕获 C++ 异常 (throw)                                               │
│  ├── SEH 异常不会被 catch(...) 捕获                                        │
│  └── 访问违规等不会触发 C++ 析构                                           │
│                                                                             │
│  /EHa (混合模式)                                                           │
│  ├── catch(...) 可以捕获 SEH 异常                                          │
│  ├── SEH 异常也会触发栈展开和析构                                          │
│  └── 性能略有影响                                                          │
│                                                                             │
│  实现原理：                                                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  C++ 异常在 Windows 上是用 SEH 实现的                               │   │
│  │                                                                     │   │
│  │  throw MyException()                                                │   │
│  │       ↓                                                             │   │
│  │  RaiseException(0xE06D7363, ...)  // "msc" 魔数                     │   │
│  │       ↓                                                             │   │
│  │  SEH 机制分发                                                       │   │
│  │       ↓                                                             │   │
│  │  C++ 运行时找到匹配的 catch                                         │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

示例代码：

// /EHsc 模式
void test_ehsc() {
    try {
        int* p = nullptr;
        *p = 42;  // ACCESS_VIOLATION
    }
    catch (...) {
        // 不会进入这里！
        printf("Caught\n");
    }
    // 程序崩溃
}

// /EHa 模式
void test_eha() {
    try {
        int* p = nullptr;
        *p = 42;  // ACCESS_VIOLATION
    }
    catch (...) {
        // 会进入这里
        printf("Caught SEH as C++ exception\n");
    }
}
```

---

### Q2: VEH 能捕获哪些 SEH 捕获不到的异常？

**答案：**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    VEH 的独特能力                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  1. 第一时间捕获                                                           │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ VEH 在所有 SEH handler 之前执行                                  │    │
│     │ 可以在异常被任何人处理之前观察和记录                             │    │
│     │                                                                 │    │
│     │ 异常 → 调试器 → VEH → SEH 帧链 → 未处理异常过滤器               │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  2. 跨线程监控                                                             │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ VEH 是进程级别的，对所有线程有效                                 │    │
│     │ SEH 是函数级别的，只对当前调用栈有效                             │    │
│     │                                                                 │    │
│     │ 新创建的线程自动受 VEH 保护                                     │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  3. 捕获已被处理的异常                                                     │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ 即使某个 SEH 处理了异常，VEH 仍然会先看到                        │    │
│     │ 可以统计所有异常，包括被处理的                                   │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  4. 无法被绕过                                                             │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ SEH 链可能被恶意代码破坏                                         │    │
│     │ VEH 链由系统管理，更难被篡改                                     │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  典型场景：                                                                 │
│  - 安全软件监控所有异常                                                    │
│  - 崩溃报告系统收集完整信息                                                │
│  - 自定义调试器实现                                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

### Q3: Mach Exception 相比 Signal 有什么优势？为什么 Crashpad 使用它？

**答案：**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Mach Exception 优势详解                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  1. 独立线程处理                                                           │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ Signal:                                                          │    │
│     │ - 在崩溃线程的栈上执行处理器                                     │    │
│     │ - 如果栈溢出，处理器本身也会崩溃                                 │    │
│     │ - 可能破坏崩溃现场                                               │    │
│     │                                                                 │    │
│     │ Mach Exception:                                                  │    │
│     │ - 处理器在独立线程执行                                           │    │
│     │ - 有自己的栈空间                                                 │    │
│     │ - 崩溃线程被挂起，现场完整保留                                   │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  2. 无 async-signal-safe 限制                                              │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ Signal Handler:                                                  │    │
│     │ - 只能调用少量 async-signal-safe 函数                            │    │
│     │ - 不能 malloc、不能 printf                                       │    │
│     │ - 收集崩溃信息非常困难                                           │    │
│     │                                                                 │    │
│     │ Mach Exception Handler:                                          │    │
│     │ - 可以调用任何函数                                               │    │
│     │ - 可以分配内存、写文件、网络通信                                 │    │
│     │ - 可以完整收集崩溃信息                                           │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  3. 更早介入                                                               │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ 异常流程：                                                       │    │
│     │ CPU 异常 → Mach 内核 → Mach Exception → (转换) → Signal          │    │
│     │                          ↑                                       │    │
│     │                    这里介入最干净                                 │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  4. 可以捕获 EXC_CRASH                                                     │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ EXC_CRASH 是进程即将终止时的最后通知                             │    │
│     │ Signal 没有等价物                                                │    │
│     │ 可以在进程真正死亡前做最后的清理和上报                           │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

### Q4: 崩溃捕获中如何避免二次崩溃？

**答案：**

```cpp
/*
 * 避免二次崩溃的策略
 */

// 1. 使用备用栈（避免栈溢出时无法执行处理器）
void setup_alternate_stack() {
    static char alt_stack[SIGSTKSZ];
    stack_t ss;
    ss.ss_sp = alt_stack;
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    sigaltstack(&ss, nullptr);

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;  // 关键：SA_ONSTACK
    // ...
}

// 2. 预分配资源（避免崩溃时分配失败）
class CrashHandler {
    // 预分配的缓冲区
    static char s_preallocated_buffer[64 * 1024];
    static bool s_buffer_used;

    static void* SafeAlloc(size_t size) {
        if (!s_buffer_used && size <= sizeof(s_preallocated_buffer)) {
            s_buffer_used = true;
            return s_preallocated_buffer;
        }
        return nullptr;  // 不要在崩溃处理中 malloc
    }
};

// 3. 最小化处理器代码（减少出错机会）
void minimal_signal_handler(int sig, siginfo_t* info, void* ctx) {
    // 只做最少的事情
    // 1. 设置标志
    static volatile sig_atomic_t g_crashed = 1;

    // 2. 写入预分配的文件描述符
    write(g_crash_fd, &sig, sizeof(sig));

    // 3. 立即恢复默认处理
    signal(sig, SIG_DFL);
    raise(sig);
}

// 4. 使用独立进程处理（Crashpad 方式）
void use_out_of_process_handler() {
    // 预先启动一个 handler 进程
    // 崩溃时只需通知 handler 进程
    // handler 进程可以安全地收集所有信息
}

// 5. 防止递归崩溃
static volatile sig_atomic_t g_handling_crash = 0;

void safe_signal_handler(int sig, siginfo_t* info, void* ctx) {
    // 检查是否已经在处理崩溃
    if (g_handling_crash) {
        // 递归崩溃，立即终止
        _exit(128 + sig);
    }
    g_handling_crash = 1;

    // 处理崩溃...

    // 恢复默认处理
    signal(sig, SIG_DFL);
    raise(sig);
}

// 6. 设置处理器超时
void signal_handler_with_timeout(int sig, siginfo_t* info, void* ctx) {
    // 设置 alarm，防止处理器卡死
    alarm(5);  // 5 秒超时

    // 处理崩溃...

    alarm(0);  // 取消 alarm
}
```

---

### Q5: 如何验证一个工程师是否真正理解崩溃捕获机制？

**面试追问策略：**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    崩溃捕获机制面试追问                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  第一层：基础概念                                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Q: Windows 上捕获崩溃用什么？                                        │   │
│  │ A: SetUnhandledExceptionFilter / __try __except / VEH               │   │
│  │                                                                     │   │
│  │ Q: macOS 上捕获崩溃用什么？                                         │   │
│  │ A: signal/sigaction / Mach exception ports                          │   │
│  │                                                                     │   │
│  │ 🔍 如果只知道一种，说明实践经验有限                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  第二层：原理理解                                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Q: VEH 和 SEH 的执行顺序是什么？                                    │   │
│  │ A: VEH 先于 SEH                                                     │   │
│  │                                                                     │   │
│  │ Q: 为什么 Crashpad 在 macOS 上用 Mach exception 而不是 signal？     │   │
│  │ A: 独立线程、无 async-signal-safe 限制、不破坏现场                  │   │
│  │                                                                     │   │
│  │ 🔍 如果说不清楚，说明只是"会用"而不"理解"                           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  第三层：实战经验                                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Q: 你的崩溃处理器中如何避免二次崩溃？                               │   │
│  │ A: 备用栈、预分配内存、最小化代码、独立进程...                      │   │
│  │                                                                     │   │
│  │ Q: 栈溢出崩溃时，你的 signal handler 能正常执行吗？                 │   │
│  │ A: 需要 sigaltstack + SA_ONSTACK                                    │   │
│  │                                                                     │   │
│  │ 🔍 踩过坑的人才会知道这些细节                                       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  第四层：深度理解                                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Q: SEH 在 x86 和 x64 上的实现有什么区别？                           │   │
│  │ A: x86 用链表 (FS:[0])，x64 用表驱动 (.pdata)                       │   │
│  │                                                                     │   │
│  │ Q: /EHa 和 /EHsc 对崩溃处理有什么影响？                             │   │
│  │ A: /EHa 允许 catch(...) 捕获 SEH 异常，触发析构                     │   │
│  │                                                                     │   │
│  │ Q: Mach exception 和 POSIX signal 是如何关联的？                    │   │
│  │ A: Mach exception 可以转换为 signal，反之不行                       │   │
│  │                                                                     │   │
│  │ 🔍 这些问题能区分"资深"和"高级"                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  红旗警告（可能只是背过文档）：                                             │
│  - 无法解释为什么选择某种方案                                              │
│  - 不知道常见的坑（栈溢出、二次崩溃、async-signal-safe）                   │
│  - 只知道一个平台的实现                                                    │
│  - 无法解释底层原理（只会调 API）                                          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

### Q6: 生产环境中遇到只有 Release 版本崩溃、Debug 正常的情况，如何排查？

**答案：**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Release-Only 崩溃排查策略                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  可能原因：                                                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ 1. 未初始化变量                                                     │   │
│  │    - Debug 模式内存被填充为 0xCC/0xCD                               │   │
│  │    - Release 模式是随机值                                           │   │
│  │                                                                     │   │
│  │ 2. 时序问题                                                         │   │
│  │    - Debug 模式较慢，掩盖了竞态条件                                 │   │
│  │    - Release 模式优化后暴露问题                                     │   │
│  │                                                                     │   │
│  │ 3. 优化导致的行为变化                                               │   │
│  │    - 函数内联改变栈布局                                             │   │
│  │    - 指令重排导致非预期行为                                         │   │
│  │    - 未定义行为在不同优化级别表现不同                               │   │
│  │                                                                     │   │
│  │ 4. 缓冲区溢出                                                       │   │
│  │    - Debug 模式有 guard bytes                                       │   │
│  │    - Release 模式直接破坏相邻数据                                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  排查步骤：                                                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ 1. 开启 Release + 调试信息                                          │   │
│  │    MSVC: /Zi /DEBUG /OPT:REF                                        │   │
│  │    GCC:  -O2 -g -fno-omit-frame-pointer                             │   │
│  │                                                                     │   │
│  │ 2. 使用 Sanitizer                                                   │   │
│  │    -fsanitize=address,undefined                                     │   │
│  │    即使是 Release 也能检测内存问题                                  │   │
│  │                                                                     │   │
│  │ 3. 使用 Page Heap (Windows)                                         │   │
│  │    gflags /p /enable MyApp.exe /full                                │   │
│  │                                                                     │   │
│  │ 4. 二分法缩小范围                                                   │   │
│  │    逐步禁用优化，定位到具体模块                                     │   │
│  │    #pragma optimize("", off)                                        │   │
│  │                                                                     │   │
│  │ 5. 分析崩溃 dump                                                    │   │
│  │    检查崩溃地址附近的内存模式                                       │   │
│  │    0xCCCCCCCC 未初始化栈                                            │   │
│  │    0xCDCDCDCD 未初始化堆                                            │   │
│  │    0xDDDDDDDD 已释放内存                                            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 6. 总结

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      崩溃捕获机制知识图谱                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Windows                           macOS                                    │
│  ├── SEH (__try/__except)         ├── POSIX Signal (sigaction)            │
│  │   └── 帧级别异常处理           │   └── 有 async-signal-safe 限制       │
│  │                                │                                        │
│  ├── VEH (AddVectoredExceptionHandler)                                     │
│  │   └── 进程级别，先于 SEH       ├── Mach Exception                      │
│  │                                │   ├── task_set_exception_ports        │
│  ├── SetUnhandledExceptionFilter  │   ├── 独立线程处理                    │
│  │   └── 最后一道防线             │   └── 先于 Signal                     │
│  │                                │                                        │
│  └── 特殊处理                     └── NSSetUncaughtExceptionHandler       │
│      ├── _set_purecall_handler        (Objective-C)                        │
│      ├── _set_invalid_parameter_handler                                    │
│      └── set_terminate                                                     │
│                                                                             │
│  面试重点                                                                   │
│  ├── 原理：VEH/SEH 顺序、Mach→Signal 转换                                  │
│  ├── 实战：避免二次崩溃、备用栈、预分配内存                                │
│  ├── 深度：x86/x64 SEH 差异、/EHa vs /EHsc                                │
│  └── 经验：Release-Only 崩溃、栈溢出处理                                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```
