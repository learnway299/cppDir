# 内存泄漏排查 (Memory Leak Detection)

内存泄漏是 C/C++ 程序最常见的问题之一，排查能力是资深工程师的核心技能。

---

## 1. 内存泄漏基础

### 1.1 什么是内存泄漏

```
┌─────────────────────────────────────────────────────────────┐
│                      内存泄漏本质                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  内存泄漏 = 程序分配了内存，但失去了对它的引用，无法释放      │
│                                                             │
│  正常流程：                                                  │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐                 │
│  │ malloc  │ →  │  使用   │ →  │  free   │                 │
│  └─────────┘    └─────────┘    └─────────┘                 │
│                                                             │
│  泄漏流程：                                                  │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐                 │
│  │ malloc  │ →  │  使用   │ →  │ 丢失指针 │ → 永久占用      │
│  └─────────┘    └─────────┘    └─────────┘                 │
│                                                             │
│  后果：                                                      │
│  • 内存持续增长                                              │
│  • 最终 OOM (Out of Memory)                                 │
│  • 系统变慢、程序崩溃                                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 泄漏类型分类

```
┌─────────────────────────────────────────────────────────────┐
│                     内存泄漏分类                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 堆内存泄漏 (Heap Leak)                                  │
│     最常见，new/malloc 后未 delete/free                     │
│                                                             │
│  2. 资源泄漏 (Resource Leak)                                │
│     文件句柄、Socket、GDI 对象等未关闭                       │
│                                                             │
│  3. 系统内存泄漏                                             │
│     共享内存、内存映射文件未释放                             │
│                                                             │
│  4. 逻辑泄漏 (Logical Leak)                                 │
│     内存仍有引用，但永远不会被使用                           │
│     如：缓存无限增长、容器只增不减                           │
│                                                             │
│  泄漏严重程度：                                              │
│  ├── 一次性泄漏：程序启动时泄漏，影响小                      │
│  ├── 偶发性泄漏：特定条件触发，难以发现                      │
│  └── 频繁泄漏：每次操作都泄漏，影响严重                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 常见泄漏代码模式

```cpp
// 1. 基本泄漏 - 忘记释放
void basic_leak() {
    int* p = new int[100];
    // 忘记 delete[] p;
}  // 泄漏 400 字节

// 2. 异常导致泄漏
void exception_leak() {
    int* p = new int[100];
    do_something();  // 如果抛异常
    delete[] p;      // 这行不会执行
}

// 3. 提前返回泄漏
bool early_return_leak() {
    int* p = new int[100];
    if (some_condition()) {
        return false;  // 泄漏！
    }
    delete[] p;
    return true;
}

// 4. 覆盖指针泄漏
void overwrite_leak() {
    int* p = new int[100];
    p = new int[200];  // 第一块内存泄漏
    delete[] p;
}

// 5. 循环引用泄漏 (智能指针)
class Node {
    std::shared_ptr<Node> next;
    // A->next = B, B->next = A
    // 引用计数永远不为 0
};

// 6. 容器元素泄漏
void container_leak() {
    std::vector<int*> vec;
    vec.push_back(new int(1));
    vec.push_back(new int(2));
    vec.clear();  // 只清除指针，不释放内存
}

// 7. 工厂函数泄漏
Widget* CreateWidget() {
    return new Widget();  // 调用者可能忘记释放
}

// 8. 资源泄漏
void resource_leak() {
    FILE* f = fopen("test.txt", "r");
    // 忘记 fclose(f);

    HANDLE h = CreateFile(...);
    // 忘记 CloseHandle(h);
}
```

---

## 2. 泄漏预防 - RAII 和智能指针

### 2.1 RAII 原则

```cpp
// RAII = Resource Acquisition Is Initialization
// 资源获取即初始化，析构时自动释放

// 自定义 RAII 包装器
template<typename T>
class ScopedPtr {
public:
    explicit ScopedPtr(T* p = nullptr) : ptr_(p) {}
    ~ScopedPtr() { delete ptr_; }

    // 禁止拷贝
    ScopedPtr(const ScopedPtr&) = delete;
    ScopedPtr& operator=(const ScopedPtr&) = delete;

    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }

private:
    T* ptr_;
};

// 文件句柄 RAII
class FileHandle {
public:
    explicit FileHandle(const char* path, const char* mode)
        : file_(fopen(path, mode)) {}

    ~FileHandle() {
        if (file_) fclose(file_);
    }

    FILE* get() const { return file_; }
    operator bool() const { return file_ != nullptr; }

private:
    FILE* file_;
};

// 使用示例
void safe_function() {
    ScopedPtr<int> p(new int(42));
    FileHandle f("test.txt", "r");

    do_something();  // 即使抛异常，资源也会释放
}  // 自动释放
```

### 2.2 智能指针使用

```cpp
#include <memory>

// 1. unique_ptr - 独占所有权
void unique_ptr_example() {
    auto p = std::make_unique<int>(42);
    // 不需要手动 delete

    // 转移所有权
    auto p2 = std::move(p);  // p 变为 nullptr

    // 数组版本
    auto arr = std::make_unique<int[]>(100);
}

// 2. shared_ptr - 共享所有权
void shared_ptr_example() {
    auto p1 = std::make_shared<int>(42);
    auto p2 = p1;  // 引用计数 = 2

    p1.reset();    // 引用计数 = 1
}  // p2 析构，引用计数 = 0，释放内存

// 3. weak_ptr - 打破循环引用
class Node {
public:
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;  // 使用 weak_ptr 避免循环引用
};

// 4. 工厂函数返回智能指针
std::unique_ptr<Widget> CreateWidget() {
    return std::make_unique<Widget>();
}

// 5. 容器使用智能指针
void container_safe() {
    std::vector<std::unique_ptr<int>> vec;
    vec.push_back(std::make_unique<int>(1));
    vec.push_back(std::make_unique<int>(2));
    vec.clear();  // 自动释放所有元素
}
```

---

## 3. Windows 平台泄漏检测

### 3.1 Visual Studio 诊断工具

```cpp
// CRT Debug Heap
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

// 重定义 new 以记录文件行号
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

int main() {
    // 启用内存泄漏检测
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // 可选：在特定分配序号处中断
    // _CrtSetBreakAlloc(62);  // 第 62 次分配时中断

    // 泄漏代码
    int* p = new int(42);
    // 忘记 delete

    return 0;
}

// 程序结束时输出：
// Detected memory leaks!
// Dumping objects ->
// {62} normal block at 0x00ABC123, 4 bytes long.
//  Data: <*   > 2A 00 00 00
// Object dump complete.
```

### 3.2 Visual Leak Detector (VLD)

```cpp
// 1. 安装 VLD
// https://github.com/KindDragon/vld

// 2. 包含头文件
#include <vld.h>

int main() {
    // VLD 自动检测泄漏
    int* p = new int[100];

    return 0;
}

// 输出详细调用栈：
// WARNING: Visual Leak Detector detected memory leaks!
// ---------- Block 1 at 0x00ABC123: 400 bytes ----------
//   Leak Hash: 0x12345678, Count: 1, Total 400 bytes
//   Call Stack:
//     main (d:\project\main.cpp:10)
//     __mainCRTStartup (crtexe.c:323)
```

**VLD 配置文件 (vld.ini)：**

```ini
; vld.ini
VLD = on
AggregateDuplicates = yes
MaxDataDump = 256
MaxTraceFrames = 64
ReportEncoding = unicode
ReportFile = memory_leak_report.txt
ReportTo = both  ; debugger + file
```

### 3.3 UMDH (User-Mode Dump Heap)

```batch
:: 1. 启用用户态栈跟踪
gflags /i MyApp.exe +ust

:: 2. 运行程序并创建快照
umdh -pn:MyApp.exe -f:snap1.log

:: 3. 执行可能泄漏的操作...

:: 4. 创建第二个快照
umdh -pn:MyApp.exe -f:snap2.log

:: 5. 比较两个快照
umdh snap1.log snap2.log > diff.log

:: 6. 分析 diff.log 查看增长的分配
:: + 表示新增，- 表示释放
```

### 3.4 Windows Performance Analyzer

```
1. 使用 Windows Performance Recorder 录制
   - 选择 "Heap Usage" 配置
   - 开始录制 → 执行操作 → 停止录制

2. 使用 Windows Performance Analyzer 分析
   - 打开 .etl 文件
   - 查看 "Memory" → "Heap Allocations"
   - 按进程、调用栈分组查看分配

3. 识别泄漏
   - 查找持续增长的分配
   - 分析对应的调用栈
```

---

## 4. macOS 平台泄漏检测

### 4.1 Instruments - Leaks

```bash
# 命令行启动 Leaks 工具
leaks --atExit -- ./MyApp

# 或使用 Instruments GUI
# Xcode → Open Developer Tool → Instruments → Leaks

# 输出示例：
# Process 12345: 2 leaks for 128 total leaked bytes.
#
#     2 (128 bytes) ROOT LEAK: 0x600000008000 [64]
#        Call stack: main | leak_function | malloc
```

### 4.2 Instruments 使用步骤

```
1. 打开 Instruments
   Xcode → Product → Profile (Cmd+I)
   或：/Applications/Xcode.app/Contents/Applications/Instruments.app

2. 选择 "Leaks" 模板

3. 选择目标应用

4. 点击红色录制按钮

5. 执行应用操作

6. 查看 Leaks 轨道
   - 红色 X 标记表示检测到泄漏
   - 点击查看泄漏详情和调用栈

7. 分析调用栈定位泄漏源
```

### 4.3 MallocStackLogging

```bash
# 启用 malloc 栈日志
export MallocStackLogging=1
./MyApp

# 使用 leaks 分析
leaks MyApp

# 使用 malloc_history 查看特定地址的分配历史
malloc_history MyApp 0x600000008000

# 输出：
# ALLOC 0x600000008000-0x600000008040 [size=64]:
#   0x7fff2034a123 malloc
#   0x100001234 leak_function (main.cpp:10)
#   0x100001567 main (main.cpp:20)
```

### 4.4 Guard Malloc

```bash
# 启用 Guard Malloc（检测越界和 use-after-free）
export DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib
./MyApp

# 或通过 Xcode
# Product → Scheme → Edit Scheme → Diagnostics
# 勾选 "Guard Malloc"

# Guard Malloc 特性：
# - 每次分配使用独立的内存页
# - 分配后的页面设为不可访问
# - 立即检测越界访问和 use-after-free
```

---

## 5. Linux 平台泄漏检测

### 5.1 Valgrind Memcheck

```bash
# 基本使用
valgrind --leak-check=full ./MyApp

# 详细选项
valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind.log \
    ./MyApp

# 选项说明：
# --leak-check=full      完整泄漏检查
# --show-leak-kinds=all  显示所有类型泄漏
# --track-origins=yes    跟踪未初始化值来源
# --num-callers=50       调用栈深度
```

### 5.2 Valgrind 输出解读

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 400 bytes in 1 blocks
==12345==   total heap usage: 10 allocs, 9 frees, 4,000 bytes allocated
==12345==
==12345== 400 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4C2FB55: malloc (vg_replace_malloc.c:299)
==12345==    by 0x400567: leak_function (main.cpp:10)
==12345==    by 0x400589: main (main.cpp:20)
==12345==
==12345== LEAK SUMMARY:
==12345==    definitely lost: 400 bytes in 1 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
==12345==    still reachable: 0 bytes in 0 blocks
==12345==         suppressed: 0 bytes in 0 blocks

泄漏类型：
• definitely lost: 确定泄漏，无任何指针指向
• indirectly lost: 间接泄漏，被 definitely lost 的块引用
• possibly lost:   可能泄漏，有内部指针指向
• still reachable: 程序结束时仍有指针指向（通常无害）
```

### 5.3 Valgrind Suppressions

```
# 创建 suppressions 文件忽略已知问题
# myapp.supp

{
   ignore_known_leak
   Memcheck:Leak
   match-leak-kinds: reachable
   fun:malloc
   fun:_dl_init
}

{
   ignore_third_party
   Memcheck:Leak
   ...
   obj:/usr/lib/libthirdparty.so
}

# 使用 suppressions
valgrind --suppressions=myapp.supp ./MyApp
```

### 5.4 mtrace (glibc)

```cpp
#include <mcheck.h>

int main() {
    mtrace();  // 开始追踪

    int* p = (int*)malloc(100);
    // 忘记 free

    muntrace();  // 停止追踪
    return 0;
}
```

```bash
# 设置输出文件
export MALLOC_TRACE=mtrace.log
./MyApp

# 分析结果
mtrace ./MyApp mtrace.log

# 输出：
# Memory not freed:
# -----------------
#    Address     Size     Caller
# 0x08049a30     64   at main.cpp:10
```

---

## 6. 跨平台工具

### 6.1 AddressSanitizer (ASan)

```bash
# 编译时启用
clang++ -fsanitize=address -g main.cpp -o MyApp
# 或
g++ -fsanitize=address -g main.cpp -o MyApp

# 运行
./MyApp

# ASan 会在程序结束时报告泄漏：
# ==12345==ERROR: LeakSanitizer: detected memory leaks
#
# Direct leak of 400 byte(s) in 1 object(s) allocated from:
#     #0 0x4c2fb55 in malloc
#     #1 0x400567 in leak_function main.cpp:10
#     #2 0x400589 in main main.cpp:20
#
# SUMMARY: AddressSanitizer: 400 byte(s) leaked in 1 allocation(s).
```

### 6.2 ASan 环境变量

```bash
# 控制 ASan 行为
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:log_path=asan.log"

# 常用选项：
# detect_leaks=1          启用泄漏检测
# halt_on_error=0         出错不中断
# log_path=asan.log       输出到文件
# malloc_context_size=30  调用栈深度
# fast_unwind_on_malloc=0 更准确的栈回溯
# suppressions=asan.supp  忽略已知问题
```

### 6.3 Heaptrack (Linux)

```bash
# 安装
sudo apt install heaptrack heaptrack-gui

# 录制
heaptrack ./MyApp

# 分析 (GUI)
heaptrack_gui heaptrack.MyApp.12345.gz

# 分析 (命令行)
heaptrack_print heaptrack.MyApp.12345.gz

# 特点：
# - 比 Valgrind 快很多
# - 提供图形化分析
# - 显示内存随时间变化
```

---

## 7. 泄漏检测原理

### 7.1 检测原理

```
┌─────────────────────────────────────────────────────────────┐
│                   泄漏检测原理                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  方法一：分配追踪                                            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Hook malloc/free，记录所有分配                       │   │
│  │                                                     │   │
│  │  AllocationTable:                                   │   │
│  │  ┌─────────┬────────┬─────────────────┐            │   │
│  │  │ Address │  Size  │   Call Stack    │            │   │
│  │  ├─────────┼────────┼─────────────────┤            │   │
│  │  │0x1000   │  64    │ main→foo→malloc │            │   │
│  │  │0x2000   │  128   │ main→bar→malloc │            │   │
│  │  └─────────┴────────┴─────────────────┘            │   │
│  │                                                     │   │
│  │  程序结束时，表中剩余项 = 泄漏                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  方法二：内存扫描                                            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  1. 记录所有堆分配块                                  │   │
│  │  2. 扫描栈、全局区、寄存器中的指针                    │   │
│  │  3. 递归扫描已标记块中的指针                          │   │
│  │  4. 未被任何指针引用的块 = 泄漏                       │   │
│  │                                                     │   │
│  │  类似 GC 的 Mark-Sweep 算法                          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 Hook 实现

```cpp
// 简单的 malloc hook 实现
#include <map>
#include <mutex>
#include <execinfo.h>

struct AllocInfo {
    size_t size;
    void* callstack[32];
    int callstack_depth;
};

std::map<void*, AllocInfo> g_allocations;
std::mutex g_mutex;

// Hook malloc
extern "C" void* __real_malloc(size_t size);
extern "C" void* __wrap_malloc(size_t size) {
    void* ptr = __real_malloc(size);

    std::lock_guard<std::mutex> lock(g_mutex);
    AllocInfo info;
    info.size = size;
    info.callstack_depth = backtrace(info.callstack, 32);
    g_allocations[ptr] = info;

    return ptr;
}

// Hook free
extern "C" void __real_free(void* ptr);
extern "C" void __wrap_free(void* ptr) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_allocations.erase(ptr);
    }
    __real_free(ptr);
}

// 报告泄漏
void report_leaks() {
    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& [ptr, info] : g_allocations) {
        fprintf(stderr, "Leak: %p, size: %zu\n", ptr, info.size);
        backtrace_symbols_fd(info.callstack, info.callstack_depth, 2);
    }
}

// 编译时链接
// g++ -Wl,--wrap=malloc -Wl,--wrap=free main.cpp
```

---

## 8. 生产环境泄漏定位

### 8.1 监控内存使用

```cpp
// 定期监控进程内存使用

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>

size_t get_memory_usage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;  // 物理内存使用
    }
    return 0;
}
#else
#include <sys/resource.h>

size_t get_memory_usage() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024;  // macOS 返回 KB，Linux 返回字节
    }
    return 0;
}
#endif

// 定期记录
void memory_monitor() {
    static size_t last_usage = 0;
    size_t current = get_memory_usage();

    if (current > last_usage * 1.1) {  // 增长超过 10%
        log_warning("Memory increased: %zu -> %zu", last_usage, current);
    }
    last_usage = current;
}
```

### 8.2 自定义内存分配器

```cpp
// 带统计的分配器
class TrackedAllocator {
public:
    static void* allocate(size_t size, const char* file, int line) {
        void* ptr = malloc(size + sizeof(Header));
        if (!ptr) return nullptr;

        Header* header = static_cast<Header*>(ptr);
        header->size = size;
        header->file = file;
        header->line = line;

        std::lock_guard<std::mutex> lock(mutex_);
        total_allocated_ += size;
        allocation_count_++;
        allocations_[header + 1] = header;

        return header + 1;
    }

    static void deallocate(void* ptr) {
        if (!ptr) return;

        Header* header = static_cast<Header*>(ptr) - 1;

        std::lock_guard<std::mutex> lock(mutex_);
        total_allocated_ -= header->size;
        allocation_count_--;
        allocations_.erase(ptr);

        free(header);
    }

    static void report() {
        std::lock_guard<std::mutex> lock(mutex_);
        printf("Total allocated: %zu bytes in %zu blocks\n",
               total_allocated_, allocation_count_);

        for (const auto& [ptr, header] : allocations_) {
            printf("  %p: %zu bytes at %s:%d\n",
                   ptr, header->size, header->file, header->line);
        }
    }

private:
    struct Header {
        size_t size;
        const char* file;
        int line;
    };

    static std::mutex mutex_;
    static size_t total_allocated_;
    static size_t allocation_count_;
    static std::map<void*, Header*> allocations_;
};

// 宏替换
#define TRACKED_NEW(T, ...) \
    new (TrackedAllocator::allocate(sizeof(T), __FILE__, __LINE__)) T(__VA_ARGS__)
```

### 8.3 生产环境采样

```cpp
// 采样式泄漏检测（低开销）
class SamplingLeakDetector {
public:
    static void* malloc_hook(size_t size) {
        void* ptr = original_malloc(size);

        // 1% 采样率
        if (rand() % 100 == 0) {
            record_allocation(ptr, size);
        }

        return ptr;
    }

    static void free_hook(void* ptr) {
        remove_allocation(ptr);
        original_free(ptr);
    }

    // 定期报告长期存活的分配
    static void report_long_lived(int age_seconds) {
        auto now = std::chrono::steady_clock::now();

        for (const auto& [ptr, info] : sampled_allocations_) {
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                now - info.timestamp
            ).count();

            if (age > age_seconds) {
                log_warning("Long-lived allocation: %p, size=%zu, age=%lds",
                           ptr, info.size, age);
            }
        }
    }
};
```

---

## 9. 面试深度问题

### Q1: 如何区分真正的泄漏和"一次性分配"？

**答案**：

```
一次性分配：
- 程序启动时分配，直到退出才释放
- 通常是全局配置、单例对象
- 技术上是泄漏，但无害

判断方法：
1. 观察内存增长模式
   - 真正泄漏：持续增长
   - 一次性：启动后稳定

2. 工具辅助
   - Valgrind: still reachable vs definitely lost
   - ASan: 可配置忽略 reachable

3. 代码审查
   - 全局/静态对象
   - 单例模式
   - 缓存（有界 vs 无界）

处理：
- 使用 suppressions 文件忽略已知的一次性分配
- 代码中标记有意的"泄漏"
```

### Q2: 智能指针能完全避免内存泄漏吗？

**答案**：

```
不能完全避免，以下情况仍会泄漏：

1. 循环引用
   shared_ptr<A> a(new A);
   shared_ptr<B> b(new B);
   a->b = b;
   b->a = a;  // 循环引用，永不释放

   解决：使用 weak_ptr 打破循环

2. this 指针泄漏
   class Widget {
       void register_callback() {
           // 如果回调持有 shared_ptr<Widget>
           // 而 Widget 也持有回调的引用
           callback_ = [self = shared_from_this()] { ... };
       }
   };

3. 容器持有过期对象
   std::vector<std::shared_ptr<Obj>> cache;
   // 缓存无限增长

4. 异常安全问题
   void bad(std::shared_ptr<A>& a, std::shared_ptr<B>& b) {
       a.reset(new A(throw_func()));  // 可能泄漏
       b.reset(new B);
   }

   解决：使用 make_shared/make_unique
```

### Q3: 如何定位生产环境的内存泄漏？

**答案**：

```
策略：

1. 监控告警
   - 进程 RSS 持续增长
   - 设置内存阈值告警

2. 重现问题
   - 收集泄漏前的操作序列
   - 在测试环境重现

3. 增量分析
   - 对比两个时间点的内存状态
   - UMDH (Windows)、heaptrack (Linux)

4. 核心转储分析
   - gcore 生成 dump
   - 分析堆内存分布

5. 采样检测
   - 低开销的运行时检测
   - 记录长期存活的分配

6. 代码审计
   - 最近变更的代码
   - 容器/缓存相关逻辑

7. 二分定位
   - 功能模块逐个禁用
   - 定位泄漏模块

实战流程：
监控发现 → 收集信息 → 测试重现 → 工具分析 → 代码修复 → 验证上线
```

### Q4: Valgrind 和 ASan 有什么区别？如何选择？

**答案**：

```
┌─────────────────────────────────────────────────────────────┐
│               Valgrind vs AddressSanitizer                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  特性          Valgrind          ASan                       │
│  ─────────────────────────────────────────────────────────  │
│  实现方式      二进制翻译        编译时插桩                  │
│  性能开销      20-50x            2-3x                       │
│  内存开销      2-3x              2-3x                       │
│  准确性        高                高                         │
│  使用难度      无需重编译        需要重编译                  │
│  平台支持      Linux主要         跨平台                     │
│  检测能力      更全面            更快                       │
│                                                             │
│  选择建议：                                                  │
│  • 开发阶段：ASan（快速迭代）                               │
│  • 深度测试：Valgrind（更全面）                             │
│  • 第三方库：Valgrind（无需源码）                           │
│  • CI 集成：ASan（性能可接受）                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Q5: 如何处理第三方库的内存泄漏？

**答案**：

```
方法：

1. 确认是否真的泄漏
   - 可能是有意的一次性分配
   - 使用 suppressions 忽略

2. 隔离第三方库
   - 独立进程运行
   - 定期重启

3. 包装层管理
   class ThirdPartyWrapper {
       // RAII 确保资源释放
       ~ThirdPartyWrapper() {
           third_party_cleanup();
       }
   };

4. 报告给库维护者
   - 提供复现步骤
   - 贡献修复补丁

5. 替代方案
   - 寻找无泄漏的替代库
   - 自己实现相关功能

6. 内存池
   - 使用独立内存池给第三方库
   - 周期性重置内存池
```

---

## 10. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   内存泄漏排查知识图谱                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  预防手段                                                   │
│  ├── RAII 原则                                             │
│  ├── 智能指针 (unique_ptr/shared_ptr/weak_ptr)             │
│  └── 代码规范与 Review                                      │
│                                                             │
│  检测工具                                                   │
│  ├── Windows: VLD, CRT Debug, UMDH                         │
│  ├── macOS: Instruments, leaks, MallocStackLogging         │
│  ├── Linux: Valgrind, mtrace, heaptrack                    │
│  └── 跨平台: AddressSanitizer                              │
│                                                             │
│  分析方法                                                   │
│  ├── 增量对比（两个快照）                                   │
│  ├── 调用栈追踪                                            │
│  └── 内存增长曲线                                          │
│                                                             │
│  面试重点                                                   │
│  ├── 泄漏类型与检测原理                                    │
│  ├── 工具使用与输出解读                                    │
│  ├── 智能指针的局限性                                      │
│  ├── 生产环境定位策略                                      │
│  └── Valgrind vs ASan 选择                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
