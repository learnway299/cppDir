# Debugging & Diagnostics

调试与诊断技能 - 资深工程师必备的 9 个主题。

---

## 知识体系

### 崩溃与内存 - 4 个主题

程序崩溃和内存问题的分析排查。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [崩溃分析](01-crash-dump.md) | Core Dump、WinDbg/LLDB、符号解析、信号机制 |
| 02 | [内存泄漏](02-memory-leak.md) | 泄漏检测、VLD/Valgrind/Instruments |
| 03 | [野指针排查](03-wild-pointer.md) | use-after-free、悬垂指针、页堆调试 |
| 04 | [多线程调试](04-thread-debug.md) | 数据竞争、死锁分析、TSan |

### 性能与分析 - 2 个主题

性能瓶颈定位和内存使用分析。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 05 | [性能分析](05-performance-profiling.md) | CPU Profiling、火焰图、perf/VTune |
| 06 | [内存分析](06-memory-profiling.md) | 内存占用、堆分析、碎片、OOM |

### 高级技能 - 3 个主题

资深工程师进阶技能。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 07 | [逆向调试](07-reverse-debug.md) | 反汇编、无符号分析、Release 调试 |
| 08 | [崩溃收集系统](08-coredump-collection.md) | Breakpad/Crashpad、符号服务器 |
| 09 | [Sanitizer 工具集](09-sanitizers.md) | ASan/TSan/MSan/UBSan 原理与实战 |

---

## 面试重点

### 必问话题

1. **崩溃分析**
   - Core Dump 分析流程
   - WinDbg/LLDB 常用命令
   - 符号文件管理

2. **内存问题**
   - 内存泄漏检测方法
   - 野指针和 use-after-free
   - 堆损坏排查

3. **多线程调试**
   - 死锁检测与分析
   - 数据竞争定位

4. **性能分析**
   - CPU 热点定位
   - 火焰图解读

### 加分项

1. 无符号 dump 分析技巧
2. Release 版本调试
3. AddressSanitizer 原理
4. Breakpad 崩溃收集系统
5. 生产环境问题定位

---

## 文件列表

```
debugging/
├── README.md                      # 本文件
├── 01-crash-dump.md               # 崩溃分析
├── 02-memory-leak.md              # 内存泄漏
├── 03-wild-pointer.md             # 野指针排查
├── 04-thread-debug.md             # 多线程调试
├── 05-performance-profiling.md    # 性能分析
├── 06-memory-profiling.md         # 内存分析
├── 07-reverse-debug.md            # 逆向调试
├── 08-coredump-collection.md      # 崩溃收集系统
└── 09-sanitizers.md               # Sanitizer 工具集
```
