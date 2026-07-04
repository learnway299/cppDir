 # 内存分析 (Memory Profiling)

内存分析用于了解程序内存使用情况，定位内存问题和优化内存效率。

---

## 1. 内存分析基础

### 1.1 内存指标

```
┌─────────────────────────────────────────────────────────────┐
│                     内存指标解析                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  指标名称              含义                                  │
│  ─────────────────────────────────────────────────────────  │
│  VSS (Virtual)        虚拟内存大小                          │
│                       进程地址空间总大小                     │
│                       包含未实际分配的区域                   │
│                                                             │
│  RSS (Resident)       常驻内存                              │
│                       实际占用的物理内存                     │
│                       不包含换出的页面                       │
│                                                             │
│  PSS (Proportional)   按比例共享内存                        │
│                       共享内存按进程数平分                   │
│                       更准确反映进程内存占用                 │
│                                                             │
│  USS (Unique)         独占内存                              │
│                       进程独占的物理内存                     │
│                       不包含共享库等                         │
│                                                             │
│  关系：VSS >= RSS >= PSS >= USS                             │
│                                                             │
│  堆内存细分：                                                │
│  ├── 已分配 (Allocated): 应用使用中                         │
│  ├── 空闲 (Free): 分配器持有但未使用                        │
│  └── 碎片 (Fragmented): 无法利用的小块                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 内存布局

```
┌─────────────────────────────────────────────────────────────┐
│                    进程内存布局                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  高地址                                                     │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              内核空间 (Kernel Space)                 │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              栈 (Stack)              ↓ 向下增长      │   │
│  │              - 局部变量                              │   │
│  │              - 函数参数                              │   │
│  │              - 返回地址                              │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              空闲区域                                │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              内存映射区 (mmap)                       │   │
│  │              - 共享库                                │   │
│  │              - 文件映射                              │   │
│  │              - 匿名映射                              │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              堆 (Heap)               ↑ 向上增长      │   │
│  │              - 动态分配 (new/malloc)                 │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              BSS (未初始化数据)                       │   │
│  │              - 未初始化全局/静态变量                  │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              Data (已初始化数据)                      │   │
│  │              - 已初始化全局/静态变量                  │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │              Text (代码段)                           │   │
│  │              - 程序指令                              │   │
│  └─────────────────────────────────────────────────────┘   │
│  低地址                                                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Linux 内存分析

### 2.1 /proc 文件系统

```bash
# 查看进程内存映射
cat /proc/<pid>/maps
# 输出：地址范围、权限、偏移、设备、inode、路径

# 查看内存状态
cat /proc/<pid>/status | grep -E "^(Vm|Rss|Pss)"
# VmSize:    虚拟内存大小
# VmRSS:     常驻内存
# VmData:    数据段大小
# VmStk:     栈大小
# VmExe:     代码段大小
# VmLib:     共享库大小

# 查看详细内存映射（含 PSS）
cat /proc/<pid>/smaps

# 查看内存统计摘要
cat /proc/<pid>/smaps_rollup

# 查看堆信息
cat /proc/<pid>/heap  # 需要内核支持
```

### 2.2 pmap 工具

```bash
# 查看进程内存映射
pmap <pid>

# 详细信息
pmap -x <pid>
# 输出列：
# Address: 起始地址
# Kbytes: 大小
# RSS: 常驻内存
# Dirty: 脏页（被修改）
# Mode: 权限
# Mapping: 映射文件

# 扩展信息
pmap -X <pid>
# 额外显示 PSS、Referenced 等

# 示例输出：
# Address           Kbytes     RSS   Dirty Mode  Mapping
# 0000555555554000      4       4       0 r-x-- MyApp
# 0000555555754000      4       4       4 r---- MyApp
# 0000555555755000      4       4       4 rw--- MyApp
# 00007f1234567000   1234     512       0 r-x-- libc.so.6
# ...
# total           123456   12345    1234
```

### 2.3 Valgrind Massif

```bash
# 使用 Massif 分析堆内存
valgrind --tool=massif ./MyApp

# 生成 massif.out.<pid> 文件

# 查看结果
ms_print massif.out.12345

# 输出示例：
#     MB
# 12.00^                                   #
#      |                                 ##:
#      |                               ##:::
#      |                             ##:::::
#      |                           ##:::::::
#      |                         ##:::::::::
#      |                       ##:::::::::::
#      |                     ##:::::::::::::
#      |                   ##:::::::::::::::
#      |                 ##:::::::::::::::::
#      |               ##:::::::::::::::::::
#      |             ##:::::::::::::::::::::
#      |           ##:::::::::::::::::::::::
#      |         ##:::::::::::::::::::::::::
#      |       ##:::::::::::::::::::::::::::
#      |     ##:::::::::::::::::::::::::::::
#   0 +----------------------------------------------------------------------->

# 选项
valgrind --tool=massif \
    --heap=yes \              # 分析堆
    --stacks=yes \            # 分析栈
    --pages-as-heap=yes \     # 分析 mmap
    --threshold=1.0 \         # 显示阈值
    --detailed-freq=10 \      # 详细快照频率
    ./MyApp
```

### 2.4 Heaptrack

```bash
# 安装
sudo apt install heaptrack heaptrack-gui

# 录制
heaptrack ./MyApp

# 分析（GUI）
heaptrack_gui heaptrack.MyApp.12345.gz

# 分析（命令行）
heaptrack_print heaptrack.MyApp.12345.gz

# 输出信息：
# - 总分配次数和大小
# - 峰值内存使用
# - 每个调用点的分配统计
# - 内存泄漏

# 特点：
# - 比 Valgrind 快很多
# - 详细的分配调用栈
# - 支持火焰图
```

---

## 3. Windows 内存分析

### 3.1 任务管理器和资源监视器

```
任务管理器 (Task Manager):
- 工作集 (Working Set): RSS
- 内存 (Private): 私有内存
- 共享: 共享内存

资源监视器 (Resource Monitor):
- 更详细的内存分类
- 按进程查看
- 硬故障/秒（页面错误）
```

### 3.2 VMMap

```
VMMap (Sysinternals 工具):

1. 下载并运行 VMMap
   https://docs.microsoft.com/en-us/sysinternals/downloads/vmmap

2. 选择目标进程

3. 查看内存分类：
   - Image: 可执行文件和 DLL
   - Private Data: 私有数据（堆、栈等）
   - Heap: 堆内存
   - Stack: 栈
   - Mapped File: 内存映射文件
   - Shareable: 可共享内存

4. 分析功能：
   - 按类型汇总
   - 按地址查看
   - 时间线对比（快照）
```

### 3.3 WinDbg 内存分析

```
// 查看进程内存统计
!address -summary

// 输出示例：
// --- Usage Summary ---------------- RgnCount ----------- Total Size
// Free                                    123     7ffe`12345678 ( 127.992 TB)
// <unknown>                                45        0`12345678 ( 291.316 MB)
// Image                                   234        0`01234567 (  18.204 MB)
// Heap                                     56        0`00123456 (   1.145 MB)
// Stack                                    12        0`00012345 ( 104.000 KB)
// TEB                                       4        0`00001234 (  16.000 KB)
// PEB                                       1        0`00000123 (   4.000 KB)

// 查看堆信息
!heap -s

// 查看特定堆
!heap -a <heap_address>

// 查看堆分配
!heap -p -a <address>

// 查看内存区域
!address <address>

// 搜索内存模式
s -a 0 L?80000000 "pattern"
```

### 3.4 Visual Studio 诊断工具

```
1. 启动内存分析
   Debug → Performance Profiler
   勾选 "Memory Usage"

2. 拍摄快照
   运行程序，点击 "Take Snapshot"

3. 分析快照
   - 对象类型统计
   - 分配调用栈
   - 引用关系图

4. 对比快照
   选择两个快照，查看差异
   找出增长的对象

5. 本机内存分析
   需要启用 "Native memory profiling"
   Tools → Options → Debugging → General
```

---

## 4. macOS 内存分析

### 4.1 Activity Monitor

```
Activity Monitor:
- 内存 (Memory): 进程使用的内存
- 实际内存 (Real Memory): 物理内存
- 虚拟内存 (Virtual Memory): 虚拟地址空间
- 共享内存 (Shared Memory): 共享部分

系统内存：
- App 内存: 应用使用
- 连线内存 (Wired): 不可换出
- 压缩内存: 已压缩
- 缓存文件: 文件缓存
```

### 4.2 Instruments - Allocations

```bash
# 命令行启动
xcrun xctrace record --template 'Allocations' \
    --launch -- ./MyApp

# 或 GUI
# Instruments → Allocations

# 分析功能：
# 1. All Allocations: 所有分配
# 2. All Heap Allocations: 堆分配
# 3. All Anonymous VM: 匿名内存
# 4. Statistics: 按类型统计

# 视图：
# - List: 分配列表
# - Call Trees: 调用树
# - Heavy: 按大小排序
# - Detail: 详细信息
```

### 4.3 vmmap 和 heap

```bash
# 查看进程内存映射
vmmap <pid>

# 汇总信息
vmmap -summary <pid>

# 输出示例：
# REGION TYPE                      VIRTUAL    RSDNT    DIRTY
# ===========                      =======    =====    =====
# Kernel Alloc Once                     4K       4K       4K
# MALLOC guard page                    16K       0K       0K
# MALLOC metadata                     136K     112K     112K
# MALLOC_LARGE                         4K       4K       4K
# MALLOC_SMALL                       1024K     512K     512K
# MALLOC_TINY                         256K     128K     128K
# Stack                              8192K      32K      32K
# __DATA                              456K     256K     128K
# __LINKEDIT                          234K      64K       0K
# __TEXT                             1234K     512K       0K

# 堆分析
heap <pid>
heap -s <pid>           # 按大小排序
heap -addresses <pid>   # 显示地址

# 输出：
# Process 12345: 3 zones
# Zone DefaultMallocZone: 1234 nodes (567890 bytes)
#   COUNT     BYTES       AVG   CLASS_NAME  TYPE  BINARY
#     123     56789     461.7   std::string       libstdc++.dylib
#      45     12345     274.3   MyClass           MyApp
```

### 4.4 leaks 和 malloc_history

```bash
# 检测泄漏
leaks <pid>
leaks --atExit -- ./MyApp

# 查看分配历史
# 需要启用 MallocStackLogging
export MallocStackLogging=1
./MyApp &
malloc_history <pid> -all

# 查看特定地址的分配历史
malloc_history <pid> <address>

# 输出：
# ALLOC 0x600000000000-0x600000000040 [size=64]:
#   0x7fff2034a123 malloc
#   0x100001234 create_object (main.cpp:10)
#   0x100001567 main (main.cpp:20)
```

---

## 5. 内存碎片分析

### 5.1 碎片类型

```
┌─────────────────────────────────────────────────────────────┐
│                     内存碎片类型                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 外部碎片 (External Fragmentation)                       │
│     ┌───┬─────┬───┬─────────┬───┬───┬─────────┐            │
│     │ A │空闲 │ B │  空闲   │ C │空闲│    D    │            │
│     └───┴─────┴───┴─────────┴───┴───┴─────────┘            │
│     空闲总量足够，但不连续，无法分配大块                     │
│                                                             │
│  2. 内部碎片 (Internal Fragmentation)                       │
│     ┌─────────────────┐                                     │
│     │ 请求 100 字节   │                                     │
│     │ 分配 128 字节   │ ← 28 字节浪费                       │
│     └─────────────────┘                                     │
│     分配器对齐/桶大小导致的浪费                              │
│                                                             │
│  碎片率计算：                                                │
│  碎片率 = (分配器持有内存 - 应用使用内存) / 分配器持有内存  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 检测碎片

```cpp
// Windows: 使用 HeapWalk 分析碎片
#include <windows.h>

void analyze_heap_fragmentation() {
    HANDLE heap = GetProcessHeap();
    PROCESS_HEAP_ENTRY entry;
    entry.lpData = nullptr;

    size_t total_size = 0;
    size_t total_overhead = 0;
    size_t free_blocks = 0;
    size_t busy_blocks = 0;

    HeapLock(heap);
    while (HeapWalk(heap, &entry)) {
        if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
            busy_blocks++;
            total_size += entry.cbData;
            total_overhead += entry.cbOverhead;
        } else {
            free_blocks++;
        }
    }
    HeapUnlock(heap);

    printf("Busy blocks: %zu\n", busy_blocks);
    printf("Free blocks: %zu\n", free_blocks);
    printf("Total size: %zu\n", total_size);
    printf("Overhead: %zu\n", total_overhead);
    printf("Fragmentation ratio: %.2f%%\n",
           100.0 * free_blocks / (busy_blocks + free_blocks));
}
```

```bash
# Linux: 使用 malloc_stats / malloc_info
# 在程序中调用
malloc_stats();  # 打印到 stderr

# 或获取 XML 格式
char* buf;
size_t size;
FILE* f = open_memstream(&buf, &size);
malloc_info(0, f);
fclose(f);
printf("%s", buf);
free(buf);

# jemalloc 统计
export MALLOC_CONF="stats_print:true"
./MyApp
```

### 5.3 减少碎片策略

```cpp
// 1. 使用内存池
template<typename T, size_t BlockSize = 4096>
class MemoryPool {
public:
    T* allocate() {
        if (free_list_) {
            T* ptr = free_list_;
            free_list_ = *reinterpret_cast<T**>(free_list_);
            return ptr;
        }
        if (current_block_ + sizeof(T) > BlockSize) {
            blocks_.push_back(new char[BlockSize]);
            current_block_ = 0;
        }
        T* ptr = reinterpret_cast<T*>(
            blocks_.back() + current_block_);
        current_block_ += sizeof(T);
        return ptr;
    }

    void deallocate(T* ptr) {
        *reinterpret_cast<T**>(ptr) = free_list_;
        free_list_ = ptr;
    }

private:
    std::vector<char*> blocks_;
    size_t current_block_ = BlockSize;
    T* free_list_ = nullptr;
};

// 2. 固定大小分配
// 避免不同大小混合分配

// 3. 对象复用
std::vector<Object*> pool;

Object* acquire() {
    if (!pool.empty()) {
        Object* obj = pool.back();
        pool.pop_back();
        obj->reset();
        return obj;
    }
    return new Object();
}

void release(Object* obj) {
    pool.push_back(obj);
}

// 4. 使用 jemalloc / tcmalloc
// 更好的碎片处理
```

---

## 6. OOM 问题排查

### 6.1 OOM 原因分析

```
┌─────────────────────────────────────────────────────────────┐
│                     OOM 常见原因                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 内存泄漏                                                 │
│     持续分配不释放，最终耗尽                                 │
│                                                             │
│  2. 峰值内存过高                                             │
│     某些操作临时需要大量内存                                 │
│                                                             │
│  3. 内存碎片                                                 │
│     总量足够但无法分配大块                                   │
│                                                             │
│  4. 容器无限增长                                             │
│     缓存、队列未设上限                                       │
│                                                             │
│  5. 大对象分配                                               │
│     一次分配超大数组                                         │
│                                                             │
│  6. 地址空间耗尽（32位）                                     │
│     虚拟地址空间 4GB 限制                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 OOM 预防

```cpp
// 1. 分配前检查
void* safe_malloc(size_t size) {
    if (size > MAX_ALLOWED_SIZE) {
        log_error("Allocation too large: %zu", size);
        return nullptr;
    }

    void* ptr = malloc(size);
    if (!ptr) {
        log_error("Allocation failed: %zu bytes", size);
        // 尝试释放缓存
        release_caches();
        ptr = malloc(size);
    }
    return ptr;
}

// 2. 设置内存限制
#include <sys/resource.h>

void set_memory_limit(size_t max_bytes) {
    struct rlimit limit;
    limit.rlim_cur = max_bytes;
    limit.rlim_max = max_bytes;
    setrlimit(RLIMIT_AS, &limit);
}

// 3. 监控内存使用
class MemoryMonitor {
public:
    void check() {
        size_t current = get_memory_usage();
        if (current > warning_threshold_) {
            log_warning("Memory high: %zu MB", current / 1024 / 1024);
        }
        if (current > critical_threshold_) {
            log_error("Memory critical!");
            trigger_emergency_gc();
        }
    }

private:
    size_t warning_threshold_ = 1024 * 1024 * 1024;  // 1GB
    size_t critical_threshold_ = 2048 * 1024 * 1024; // 2GB
};

// 4. 自定义 new handler
void my_new_handler() {
    log_error("Memory allocation failed!");

    // 尝试释放内存
    release_caches();

    // 如果还是失败，抛异常或终止
    static int retries = 0;
    if (++retries > 3) {
        throw std::bad_alloc();
    }
}

std::set_new_handler(my_new_handler);
```

### 6.3 OOM 日志分析

```bash
# Linux OOM Killer 日志
dmesg | grep -i "oom\|killed"
# 或
journalctl -k | grep -i "oom\|killed"

# 输出示例：
# Out of memory: Killed process 12345 (MyApp) total-vm:12345678kB,
# anon-rss:1234567kB, file-rss:12345kB, shmem-rss:0kB,
# oom_score_adj:0

# 解读：
# - total-vm: 虚拟内存总量
# - anon-rss: 匿名页物理内存
# - file-rss: 文件映射物理内存
# - oom_score_adj: OOM 分数调整值

# 查看 OOM 分数
cat /proc/<pid>/oom_score
cat /proc/<pid>/oom_score_adj

# 保护进程不被 OOM Kill
echo -1000 > /proc/<pid>/oom_score_adj  # 需要 root
```

---

## 7. 内存分配器

### 7.1 分配器对比

```
┌─────────────────────────────────────────────────────────────┐
│                    内存分配器对比                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  分配器        特点                    适用场景             │
│  ─────────────────────────────────────────────────────────  │
│  glibc malloc  默认，通用              大多数场景           │
│  jemalloc      低碎片，多线程好        高并发服务           │
│  tcmalloc      快速，线程缓存          多线程频繁分配       │
│  mimalloc      微软，极快              通用                 │
│  dlmalloc      小巧，可移植            嵌入式               │
│                                                             │
│  性能对比（相对值，越小越好）：                              │
│                                                             │
│  分配器      小对象   大对象   多线程   碎片                │
│  ─────────────────────────────────────────────────────────  │
│  glibc         1.0      1.0      1.0     1.0               │
│  jemalloc      0.8      0.9      0.7     0.6               │
│  tcmalloc      0.7      0.8      0.6     0.8               │
│  mimalloc      0.6      0.7      0.5     0.7               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 使用替代分配器

```bash
# 使用 jemalloc
# 编译时链接
gcc main.c -ljemalloc -o MyApp

# 或运行时替换
LD_PRELOAD=/usr/lib/libjemalloc.so ./MyApp

# 使用 tcmalloc
LD_PRELOAD=/usr/lib/libtcmalloc.so ./MyApp

# jemalloc 配置
export MALLOC_CONF="background_thread:true,metadata_thp:auto"

# tcmalloc 配置
export TCMALLOC_RELEASE_RATE=1.0  # 释放内存给系统的速率

# Windows: 使用 mimalloc
# 链接 mimalloc.lib 或 mimalloc-override.lib
```

### 7.3 分配器统计

```cpp
// jemalloc 统计
#include <jemalloc/jemalloc.h>

void print_jemalloc_stats() {
    // 刷新统计
    uint64_t epoch = 1;
    size_t sz = sizeof(epoch);
    mallctl("epoch", &epoch, &sz, &epoch, sz);

    // 获取统计
    size_t allocated, active, mapped;
    sz = sizeof(size_t);
    mallctl("stats.allocated", &allocated, &sz, nullptr, 0);
    mallctl("stats.active", &active, &sz, nullptr, 0);
    mallctl("stats.mapped", &mapped, &sz, nullptr, 0);

    printf("Allocated: %zu MB\n", allocated / 1024 / 1024);
    printf("Active: %zu MB\n", active / 1024 / 1024);
    printf("Mapped: %zu MB\n", mapped / 1024 / 1024);
    printf("Fragmentation: %.2f%%\n",
           100.0 * (active - allocated) / active);
}

// tcmalloc 统计
#include <gperftools/malloc_extension.h>

void print_tcmalloc_stats() {
    char buf[10000];
    MallocExtension::instance()->GetStats(buf, sizeof(buf));
    printf("%s\n", buf);
}
```

---

## 8. 面试深度问题

### Q1: 如何分析进程内存持续增长的问题？

**答案**：

```
分析步骤：

1. 确认增长模式
   - 线性增长 → 每次操作泄漏固定量
   - 阶梯增长 → 特定操作触发
   - 缓慢增长 → 可能是碎片

2. 使用工具监控
   - Linux: top, htop, smaps
   - 持续记录 RSS 变化

3. 对比内存快照
   - Heaptrack: 两个时间点对比
   - VS: 内存快照对比

4. 分析分配热点
   - 哪些调用栈分配最多
   - 新增的分配来自哪里

5. 检查可能原因
   - 泄漏：分配无释放
   - 缓存：无限增长
   - 碎片：频繁分配释放

6. 验证修复
   - 长时间运行测试
   - 内存应该趋于稳定
```

### Q2: RSS、VSS、PSS 有什么区别？

**答案**：

```
┌─────────────────────────────────────────────────────────────┐
│  指标   │  含义                  │  特点                    │
├─────────┼────────────────────────┼──────────────────────────┤
│  VSS    │  虚拟内存大小           │  包含未映射页            │
│         │  进程地址空间总大小      │  最大，参考价值有限      │
├─────────┼────────────────────────┼──────────────────────────┤
│  RSS    │  常驻内存              │  实际占用物理内存         │
│         │  不含换出页             │  常用指标                │
├─────────┼────────────────────────┼──────────────────────────┤
│  PSS    │  按比例共享内存         │  共享库按进程数平分      │
│         │  更准确反映实际占用      │  适合计算总内存占用      │
├─────────┼────────────────────────┼──────────────────────────┤
│  USS    │  独占内存              │  进程退出后释放的内存     │
│         │  不含共享部分           │  最小                    │
└─────────┴────────────────────────┴──────────────────────────┘

例子：
进程 A 和 B 共享一个 10MB 的库

进程 A:
- VSS: 100MB
- RSS: 50MB (含 10MB 共享库)
- PSS: 45MB (共享库算 5MB)
- USS: 40MB (不含共享库)

监控建议：
- 单进程监控用 RSS
- 多进程总量用 PSS
- 评估进程退出释放用 USS
```

### Q3: 如何减少内存碎片？

**答案**：

```
策略：

1. 使用内存池
   - 相同大小对象用同一个池
   - 避免不同大小混合

2. 对象复用
   - 不频繁 new/delete
   - 使用对象池

3. 预分配
   std::vector<T> vec;
   vec.reserve(expected_size);

4. 避免频繁小分配
   - 批量分配
   - 使用 arena 分配器

5. 选择合适的分配器
   - jemalloc: 低碎片设计
   - tcmalloc: 线程缓存

6. 定期紧缩（如果支持）
   - 某些分配器支持内存紧缩
   - 空闲时执行

7. 对齐分配大小
   - 按 2 的幂分配
   - 减少不同大小的桶
```

### Q4: 32 位程序内存限制是什么？如何突破？

**答案**：

```
32 位限制：
- 虚拟地址空间 4GB
- 用户空间通常 2-3GB（内核占用 1-2GB）
- 单次分配上限约 1.5GB（连续空间）

突破方法：

1. 使用 64 位系统和程序
   - 根本解决方案
   - 理论上 16EB 地址空间

2. Windows /LARGEADDRESSAWARE
   - 32 位程序使用 3GB 用户空间
   - 需要系统启用 3GB 开关

3. 内存映射文件
   - 按需映射大文件
   - 不占用连续地址空间

4. 分块处理
   - 大数据分块加载
   - 处理完释放

5. 使用外部存储
   - 临时文件
   - 数据库

6. AWE (Address Windowing Extensions)
   - Windows 特有
   - 通过窗口访问更多物理内存
```

### Q5: 如何分析共享库的内存使用？

**答案**：

```
分析方法：

1. 查看内存映射
   pmap -x <pid> | grep .so
   cat /proc/<pid>/maps | grep .so

2. 查看 PSS（共享部分按比例）
   cat /proc/<pid>/smaps | grep -A 20 "\.so"
   # 关注 Pss 字段

3. 使用 vmmap (macOS)
   vmmap <pid> | grep dylib

4. 共享库占用分类
   - .text (代码)：共享，只读
   - .rodata：共享，只读
   - .data：私有（写时复制）
   - .bss：私有

5. 减少共享库内存
   - 延迟加载：dlopen
   - 去除不用的符号：strip
   - 使用静态链接（减少共享但增加私有）
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   内存分析知识图谱                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  内存指标                                                   │
│  ├── VSS: 虚拟内存                                         │
│  ├── RSS: 常驻内存                                         │
│  ├── PSS: 按比例共享                                       │
│  └── USS: 独占内存                                         │
│                                                             │
│  分析工具                                                   │
│  ├── Linux: pmap, smaps, Massif, Heaptrack                 │
│  ├── Windows: VMMap, WinDbg, VS Profiler                   │
│  ├── macOS: vmmap, heap, Instruments                       │
│  └── 跨平台: Valgrind                                      │
│                                                             │
│  关键问题                                                   │
│  ├── 内存泄漏                                               │
│  ├── 内存碎片                                               │
│  ├── OOM                                                   │
│  └── 峰值过高                                               │
│                                                             │
│  优化手段                                                   │
│  ├── 内存池                                                 │
│  ├── 对象复用                                               │
│  ├── 替代分配器                                             │
│  └── 预分配                                                 │
│                                                             │
│  面试重点                                                   │
│  ├── RSS/VSS/PSS 区别                                      │
│  ├── 碎片分析与优化                                         │
│  ├── OOM 排查                                              │
│  └── 分配器选择                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
