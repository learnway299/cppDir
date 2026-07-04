# 性能分析 (Performance Profiling)

性能分析是定位程序瓶颈、优化运行效率的核心技能。

---

## 1. 性能分析基础

### 1.1 性能指标

```
┌─────────────────────────────────────────────────────────────┐
│                     性能指标分类                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  CPU 相关                                                   │
│  ├── CPU 使用率：程序占用 CPU 的百分比                       │
│  ├── 指令数 (Instructions)：执行的指令总数                  │
│  ├── IPC (Instructions Per Cycle)：每周期指令数             │
│  ├── 缓存命中率：L1/L2/L3 Cache Hit Rate                   │
│  └── 分支预测命中率：Branch Prediction Hit Rate             │
│                                                             │
│  内存相关                                                   │
│  ├── 内存带宽：读写吞吐量                                   │
│  ├── 页错误 (Page Faults)：主/次缺页                        │
│  ├── TLB 命中率                                             │
│  └── 内存分配频率                                           │
│                                                             │
│  时间相关                                                   │
│  ├── Wall Clock Time：实际经过时间                          │
│  ├── CPU Time：CPU 执行时间（用户态 + 内核态）              │
│  ├── Latency：延迟（P50/P99/P999）                          │
│  └── Throughput：吞吐量（QPS/TPS）                          │
│                                                             │
│  I/O 相关                                                   │
│  ├── 磁盘 IOPS                                              │
│  ├── 网络带宽利用率                                         │
│  └── 系统调用频率                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 性能分析方法

```
┌─────────────────────────────────────────────────────────────┐
│                    性能分析方法分类                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 采样分析 (Sampling)                                     │
│     定期中断程序，记录当前执行位置                           │
│     优点：开销低（1-5%）                                    │
│     缺点：统计近似，短函数可能遗漏                          │
│     工具：perf, VTune, Instruments                          │
│                                                             │
│  2. 插桩分析 (Instrumentation)                              │
│     在函数入口/出口插入计时代码                              │
│     优点：精确，不遗漏                                      │
│     缺点：开销大（10-100x）                                 │
│     工具：gprof, Callgrind                                  │
│                                                             │
│  3. 事件追踪 (Event Tracing)                                │
│     记录特定事件（系统调用、锁操作等）                       │
│     优点：详细时序信息                                      │
│     缺点：数据量大                                          │
│     工具：ETW, dtrace, strace                               │
│                                                             │
│  4. 硬件计数器 (Hardware Counters)                          │
│     CPU 内置性能计数器                                      │
│     优点：最准确，开销最低                                  │
│     缺点：需要特定硬件支持                                  │
│     工具：perf stat, VTune, PAPI                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Linux perf 工具

### 2.1 perf 基础命令

```bash
# 安装
sudo apt install linux-tools-common linux-tools-$(uname -r)

# 统计程序运行指标
perf stat ./MyApp
# 输出：
#  Performance counter stats for './MyApp':
#
#        1234.56 msec task-clock                #    0.987 CPUs utilized
#             23      context-switches          #   18.630 /sec
#              5      cpu-migrations            #    4.050 /sec
#           1234      page-faults               #  999.190 /sec
#    3456789012      cycles                    #    2.800 GHz
#    1234567890      instructions              #    0.36  insn per cycle
#     234567890      branches                  #  189.999 M/sec
#      12345678      branch-misses             #    5.26% of all branches

# 详细计数器
perf stat -e cycles,instructions,cache-references,cache-misses ./MyApp

# 采样分析
perf record -g ./MyApp          # -g: 记录调用栈
perf report                     # 查看报告
perf report --stdio             # 文本输出

# 实时 Top
perf top                        # 实时显示热点函数
perf top -p <pid>               # 指定进程
```

### 2.2 perf 采样与火焰图

```bash
# 1. 采样
perf record -g -F 99 ./MyApp    # -F 99: 99Hz 采样频率
# 或附加到运行中的进程
perf record -g -p <pid> sleep 30

# 2. 生成火焰图（使用 FlameGraph 工具）
git clone https://github.com/brendangregg/FlameGraph

perf script | ./FlameGraph/stackcollapse-perf.pl | \
    ./FlameGraph/flamegraph.pl > flame.svg

# 3. 查看火焰图
# 用浏览器打开 flame.svg
# - X 轴：函数占用时间比例
# - Y 轴：调用栈深度
# - 点击可缩放
```

### 2.3 火焰图解读

```
┌─────────────────────────────────────────────────────────────┐
│                      火焰图解读                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    main()                            │   │
│  │  ┌─────────────────────────┬───────────────────┐    │   │
│  │  │     process_data()      │    save_result()  │    │   │
│  │  │  ┌─────────┬─────────┐  │                   │    │   │
│  │  │  │  parse()│compute()│  │                   │    │   │
│  │  └──┴─────────┴─────────┴──┴───────────────────┘    │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  解读方法：                                                  │
│  1. 越宽的函数耗时越多（重点优化目标）                       │
│  2. 最顶层的函数是实际执行代码的位置                         │
│  3. 相邻函数无父子关系，只是同一层级                         │
│  4. 颜色无特殊含义（随机分配）                               │
│                                                             │
│  优化策略：                                                  │
│  • 找最宽的"平顶"函数                                       │
│  • 这些函数本身耗时多，是优化重点                           │
│  • 瘦长的调用链可能是递归或深嵌套                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.4 perf 高级用法

```bash
# 缓存分析
perf stat -e L1-dcache-loads,L1-dcache-load-misses,\
LLC-loads,LLC-load-misses ./MyApp

# 分支预测分析
perf stat -e branches,branch-misses ./MyApp

# 记录特定事件
perf record -e cpu-clock,page-faults ./MyApp

# 追踪系统调用
perf trace ./MyApp

# 内核函数分析
sudo perf top -e cycles:k     # 只看内核

# 比较两次运行
perf record -o before.data ./MyApp
# 修改代码后
perf record -o after.data ./MyApp
perf diff before.data after.data
```

---

## 3. Windows 性能工具

### 3.1 Visual Studio Profiler

```
1. 启动性能分析
   Debug → Performance Profiler (Alt+F2)

2. 选择分析类型
   • CPU Usage: CPU 热点分析
   • Memory Usage: 内存分配分析
   • GPU Usage: GPU 分析
   • .NET Async: 异步操作分析

3. 开始分析
   点击 Start → 运行程序 → Stop

4. 查看结果
   • Hot Path: 热点调用路径
   • Functions: 按函数排序
   • Caller/Callee: 调用关系
   • Flame Graph: 火焰图视图

5. 导出报告
   File → Export → .diagsession 或 .csv
```

### 3.2 Intel VTune Profiler

```bash
# 命令行分析
vtune -collect hotspots ./MyApp
vtune -report summary

# 分析类型
vtune -collect hotspots           # CPU 热点
vtune -collect memory-access      # 内存访问
vtune -collect threading          # 线程分析
vtune -collect microarchitecture  # 微架构分析

# GUI 使用
vtune-gui
# 1. New Project
# 2. Configure Analysis
# 3. Start
# 4. View Results

# 火焰图
vtune -collect hotspots -knob sampling-mode=hw ./MyApp
# VTune GUI 中查看 Flame Graph
```

### 3.3 Windows Performance Analyzer (WPA)

```
1. 使用 Windows Performance Recorder (WPR) 录制
   wpr -start CPU
   # 运行测试
   wpr -stop trace.etl

2. 使用 WPA 分析
   wpa trace.etl

3. 常用视图
   • CPU Usage (Sampled): CPU 采样
   • CPU Usage (Precise): 上下文切换
   • Disk I/O: 磁盘 IO
   • Memory: 内存使用

4. 分析热点
   展开 CPU Usage → Stack
   按 Weight 排序找热点
```

---

## 4. macOS Instruments

### 4.1 Time Profiler

```bash
# 命令行
xcrun xctrace record --template 'Time Profiler' \
    --launch -- ./MyApp

# 或 GUI
# Xcode → Open Developer Tool → Instruments
# 选择 Time Profiler

# 分析结果
# 1. 查看 Call Tree
# 2. 展开热点函数
# 3. 双击跳转到源码
# 4. 查看 Heaviest Stack Trace
```

### 4.2 Instruments 模板

```
常用模板：

1. Time Profiler
   CPU 采样分析，找热点函数

2. Allocations
   内存分配追踪

3. Leaks
   内存泄漏检测

4. System Trace
   系统级追踪（系统调用、调度等）

5. Core Animation
   UI 渲染性能

6. Network
   网络活动分析

7. Energy Log
   能耗分析（移动端）
```

### 4.3 sample 命令

```bash
# 简单采样（无需 Instruments）
sample <pid> 5              # 采样 5 秒
sample <pid> 5 -file out.txt  # 输出到文件

# 输出示例：
# Call graph:
#     2107 Thread_1234
#       2107 start
#         2107 main
#           1500 process_data  (71.2%)
#             1200 compute    (57.0%)
#             300 parse       (14.2%)
#           607 save_result   (28.8%)
```

---

## 5. 代码级性能分析

### 5.1 手动计时

```cpp
#include <chrono>

class ScopedTimer {
public:
    ScopedTimer(const char* name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<
            std::chrono::microseconds>(end - start_);
        printf("%s: %lld us\n", name_, duration.count());
    }

private:
    const char* name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

// 使用
void process() {
    ScopedTimer timer("process");
    // 代码...
}

// 或使用宏
#define PROFILE_SCOPE(name) ScopedTimer _timer_##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)

void my_function() {
    PROFILE_FUNCTION();
    // 代码...
}
```

### 5.2 RDTSC 高精度计时

```cpp
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

inline uint64_t rdtsc() {
    return __rdtsc();
}

// 使用
uint64_t start = rdtsc();
// 代码...
uint64_t end = rdtsc();
uint64_t cycles = end - start;

// 转换为时间（需要知道 CPU 频率）
// 时间(秒) = cycles / CPU_FREQ_HZ
```

### 5.3 统计框架

```cpp
#include <map>
#include <string>
#include <mutex>
#include <atomic>

class Profiler {
public:
    static Profiler& instance() {
        static Profiler inst;
        return inst;
    }

    void record(const char* name, uint64_t duration_us) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stat = stats_[name];
        stat.count++;
        stat.total_us += duration_us;
        stat.min_us = std::min(stat.min_us, duration_us);
        stat.max_us = std::max(stat.max_us, duration_us);
    }

    void report() {
        std::lock_guard<std::mutex> lock(mutex_);
        printf("\n=== Performance Report ===\n");
        printf("%-30s %10s %10s %10s %10s %10s\n",
               "Function", "Count", "Total(ms)", "Avg(us)", "Min(us)", "Max(us)");

        for (const auto& [name, stat] : stats_) {
            printf("%-30s %10llu %10.2f %10.2f %10llu %10llu\n",
                   name,
                   stat.count,
                   stat.total_us / 1000.0,
                   stat.count ? (double)stat.total_us / stat.count : 0,
                   stat.min_us,
                   stat.max_us);
        }
    }

private:
    struct Stat {
        uint64_t count = 0;
        uint64_t total_us = 0;
        uint64_t min_us = UINT64_MAX;
        uint64_t max_us = 0;
    };

    std::mutex mutex_;
    std::map<std::string, Stat> stats_;
};

// 自动记录
class AutoProfiler {
public:
    AutoProfiler(const char* name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }

    ~AutoProfiler() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<
            std::chrono::microseconds>(end - start_).count();
        Profiler::instance().record(name_, duration);
    }

private:
    const char* name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

#define PROFILE(name) AutoProfiler _profiler(name)

// 程序结束时
// Profiler::instance().report();
```

---

## 6. 性能优化策略

### 6.1 CPU 优化

```cpp
// 1. 减少分支预测失败
// 差：随机分支
if (rand() % 2) { ... }

// 好：排序后分支更可预测
std::sort(data.begin(), data.end());
for (auto& x : data) {
    if (x > threshold) { ... }  // 分支更可预测
}

// 2. 循环展开
// 差
for (int i = 0; i < n; i++) {
    sum += arr[i];
}

// 好（编译器通常自动做）
for (int i = 0; i < n; i += 4) {
    sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
}

// 3. SIMD 向量化
#include <immintrin.h>

void add_vectors_simd(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }
}

// 4. 避免虚函数开销
// 热点路径避免虚函数调用
// 使用 CRTP 替代运行时多态
template<typename Derived>
class Base {
    void process() {
        static_cast<Derived*>(this)->process_impl();
    }
};
```

### 6.2 缓存优化

```cpp
// 1. 数据局部性
// 差：列优先访问（缓存不友好）
for (int j = 0; j < cols; j++) {
    for (int i = 0; i < rows; i++) {
        sum += matrix[i][j];
    }
}

// 好：行优先访问（缓存友好）
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        sum += matrix[i][j];
    }
}

// 2. 结构体布局
// 差：填充浪费，缓存行利用率低
struct Bad {
    char a;      // 1 byte + 7 padding
    double b;    // 8 bytes
    char c;      // 1 byte + 7 padding
};  // 24 bytes

// 好：紧凑布局
struct Good {
    double b;    // 8 bytes
    char a;      // 1 byte
    char c;      // 1 byte + 6 padding
};  // 16 bytes

// 3. 缓存行对齐
struct alignas(64) CacheLineAligned {
    std::atomic<int> counter;
    char padding[60];  // 避免 false sharing
};

// 4. 预取
for (int i = 0; i < n; i++) {
    __builtin_prefetch(&data[i + 16], 0, 3);  // 预取
    process(data[i]);
}
```

### 6.3 内存优化

```cpp
// 1. 减少分配
// 差：频繁分配
for (int i = 0; i < n; i++) {
    std::vector<int> temp(size);
    process(temp);
}

// 好：复用内存
std::vector<int> temp(size);
for (int i = 0; i < n; i++) {
    temp.clear();  // 不释放内存
    process(temp);
}

// 2. 内存池
class MemoryPool {
public:
    void* allocate(size_t size) {
        if (free_list_) {
            void* ptr = free_list_;
            free_list_ = *static_cast<void**>(free_list_);
            return ptr;
        }
        return malloc(size);
    }

    void deallocate(void* ptr) {
        *static_cast<void**>(ptr) = free_list_;
        free_list_ = ptr;
    }

private:
    void* free_list_ = nullptr;
};

// 3. 小对象优化 (SBO)
// std::string, std::function 等已实现
// 小数据存储在对象内部，避免堆分配

// 4. 移动语义
std::vector<Data> process() {
    std::vector<Data> result;
    // ...
    return result;  // 移动，非拷贝
}
```

---

## 7. 分析实战案例

### 7.1 CPU 热点分析

```bash
# 1. 采样
perf record -g ./MyApp

# 2. 查看报告
perf report

# 报告示例：
# Overhead  Command   Shared Object      Symbol
# 45.23%    MyApp     MyApp              [.] compute_hash
# 22.15%    MyApp     MyApp              [.] parse_data
# 15.67%    MyApp     libc.so.6          [.] malloc
#  8.34%    MyApp     MyApp              [.] sort_results

# 3. 分析结论
# - compute_hash 占 45%，是主要优化目标
# - malloc 占 15%，考虑减少分配或使用内存池
```

### 7.2 缓存分析

```bash
# 缓存命中率分析
perf stat -e L1-dcache-loads,L1-dcache-load-misses,\
L1-icache-loads,L1-icache-load-misses,\
LLC-loads,LLC-load-misses ./MyApp

# 输出示例：
#    1,234,567,890      L1-dcache-loads
#       12,345,678      L1-dcache-load-misses  # 1% 未命中
#      234,567,890      L1-icache-loads
#        2,345,678      L1-icache-load-misses  # 1% 未命中
#       23,456,789      LLC-loads
#        5,678,901      LLC-load-misses        # 24% 未命中！

# 结论：LLC 未命中率高，数据局部性差
```

### 7.3 锁竞争分析

```bash
# 使用 perf lock
perf lock record ./MyApp
perf lock report

# 输出示例：
# Name             acquired contended total wait (us) avg wait (us)
# mutex_a           12345      234        45678          195
# mutex_b            5678       12         1234          103

# 结论：mutex_a 竞争严重，考虑减小锁粒度或无锁设计
```

---

## 8. 面试深度问题

### Q1: 如何定位 CPU 使用率 100% 的问题？

**答案**：

```
步骤：

1. 确认是用户态还是内核态
   top / htop 查看 %us vs %sy

2. 找到热点进程
   top -p <pid> 或 ps aux

3. 采样分析
   perf record -g -p <pid>
   perf report

4. 分析热点函数
   - 是否死循环？
   - 是否 busy-wait？
   - 是否计算密集？

5. 检查调用栈
   - 正常业务逻辑？
   - 锁自旋？
   - 递归？

6. 具体问题具体分析
   - 死循环：修复逻辑
   - 锁自旋：减少竞争
   - 计算密集：优化算法
```

### Q2: 火焰图怎么看？如何找到优化点？

**答案**：

```
火焰图解读：

1. 宽度 = 时间占比
   越宽的函数耗时越多

2. 高度 = 调用深度
   顶部是实际执行的函数
   底部是 main/start

3. 颜色无特殊含义
   只是为了区分不同函数

找优化点：

1. 找"平顶山"
   宽且平的顶部函数
   这些函数本身耗时多（不是调用其他函数）

2. 找"悬崖"
   从宽突然变窄
   说明子函数耗时少，本层耗时多

3. 关注系统函数
   malloc/free 占比大 → 优化分配
   lock/mutex 占比大 → 减少锁竞争

4. 多次对比
   优化前后的火焰图对比
   验证优化效果
```

### Q3: perf 和 VTune 有什么区别？

**答案**：

```
┌─────────────────────────────────────────────────────────────┐
│                   perf vs VTune                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  特性          perf              VTune                      │
│  ─────────────────────────────────────────────────────────  │
│  平台          Linux             跨平台                     │
│  价格          免费              商业（有免费版）            │
│  易用性        命令行            GUI + 命令行               │
│  分析深度      基础              深入（微架构级）            │
│  Intel 优化    一般              专门优化                   │
│  报告          文本/火焰图       丰富的可视化               │
│                                                             │
│  选择：                                                     │
│  • 快速分析/脚本化 → perf                                  │
│  • 深入分析/Intel CPU → VTune                              │
│  • 跨平台/Windows → VTune                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Q4: 如何分析 I/O 性能问题？

**答案**：

```
分析步骤：

1. 确认 I/O 等待
   top 查看 %wa（I/O wait）
   iostat -x 1 查看磁盘利用率

2. 追踪 I/O 操作
   strace -e trace=read,write,open ./MyApp
   或
   perf trace -e 'syscalls:sys_*_read' ./MyApp

3. 分析 I/O 模式
   - 读写大小
   - 顺序 vs 随机
   - 同步 vs 异步

4. 常见问题
   - 小 I/O 太多 → 批量化
   - 同步 I/O 阻塞 → 改异步
   - 随机访问 → 改顺序或缓存

5. 优化策略
   - 缓冲区 / 缓存
   - 异步 I/O (io_uring, IOCP)
   - 内存映射 (mmap)
   - 预读取
```

### Q5: 如何优化缓存命中率？

**答案**：

```
优化策略：

1. 数据布局
   - 热数据放一起
   - 缓存行对齐 (alignas(64))
   - 避免 false sharing

2. 访问模式
   - 顺序访问优于随机
   - 行优先遍历二维数组
   - 分块处理大数据

3. 数据结构
   - 数组优于链表（连续内存）
   - SOA 优于 AOS（结构数组 vs 数组结构）

4. 预取
   - 软件预取 __builtin_prefetch
   - 循环展开增加预取距离

5. 减小数据大小
   - 使用更小的数据类型
   - 压缩存储
   - 热点数据单独存放

6. 验证
   perf stat -e cache-references,cache-misses
```

---

## 9. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                   性能分析知识图谱                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  分析工具                                                   │
│  ├── Linux: perf, Valgrind (callgrind)                     │
│  ├── Windows: VTune, VS Profiler, WPA                      │
│  ├── macOS: Instruments, sample                            │
│  └── 跨平台: Intel VTune                                   │
│                                                             │
│  分析方法                                                   │
│  ├── 采样分析（低开销）                                     │
│  ├── 插桩分析（精确）                                       │
│  ├── 硬件计数器（最准确）                                   │
│  └── 事件追踪（详细时序）                                   │
│                                                             │
│  优化方向                                                   │
│  ├── CPU: 减少分支、SIMD、避免虚函数                        │
│  ├── 缓存: 数据局部性、对齐、预取                           │
│  ├── 内存: 减少分配、内存池、移动语义                       │
│  └── I/O: 批量化、异步、缓存                                │
│                                                             │
│  面试重点                                                   │
│  ├── 火焰图解读                                             │
│  ├── perf 使用                                              │
│  ├── 缓存优化策略                                           │
│  └── CPU 100% 问题定位                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
