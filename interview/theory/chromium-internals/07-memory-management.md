# 07 - 内存管理 (Memory Management)

## 简介

Chromium 是一个内存密集型应用（"Chrome 吃内存"），其内存管理涉及多个层次：操作系统层的进程内存、C++ 层的自定义分配器（PartitionAlloc）、Blink 层的垃圾回收器（Oilpan），以及 V8 的 JS 堆管理。

---

## 1. Chromium 内存全景

```
一个典型 Chrome 浏览器的内存分布：

┌──────────────────────────────────────────────────────┐
│                    操作系统进程内存                     │
│                                                      │
│  Browser 进程   (~100-300 MB)                        │
│  ├── UI 渲染、网络栈、存储管理                        │
│  └── 进程管理开销                                     │
│                                                      │
│  Renderer 进程 × N   (每个 ~50-300 MB)               │
│  ├── V8 Heap (JS 堆)      ~10-100 MB               │
│  ├── Blink Heap (Oilpan)  ~5-50 MB                 │
│  ├── PartitionAlloc 堆     ~10-50 MB                │
│  ├── 共享库映射            ~30-50 MB                 │
│  └── 其他（栈、线程、mmap）                          │
│                                                      │
│  GPU 进程   (~50-200 MB)                             │
│  ├── GPU 纹理缓存                                    │
│  └── 命令缓冲区                                      │
│                                                      │
│  Utility 进程 × M  (各 ~20-50 MB)                    │
└──────────────────────────────────────────────────────┘
```

---

## 2. PartitionAlloc

### 2.1 什么是 PartitionAlloc？

```
PartitionAlloc 是 Chromium 自研的内存分配器，替代系统的 malloc/free

设计目标：
├── 安全性：防止 Use-After-Free、堆溢出等漏洞
├── 性能：快速分配/释放，减少碎片
├── 可控性：精确的内存统计和调控
└── 确定性：相同输入产生相同的内存布局（有助调试）
```

### 2.2 分区隔离

```
核心安全特性：按类型分区（Partition）

  Partition A（DOM 节点）
  ┌──────────────────────────┐
  │ Node  Node  Node  Node   │
  └──────────────────────────┘

  Partition B（字符串）
  ┌──────────────────────────┐
  │ Str   Str   Str   Str    │
  └──────────────────────────┘

  Partition C（网络缓冲区）
  ┌──────────────────────────┐
  │ Buf   Buf   Buf   Buf    │
  └──────────────────────────┘

不同类型的对象在不同分区分配，互不干扰：
├── 类型混淆攻击更难利用（攻击者溢出 Buf 不会覆盖 Node）
├── Use-After-Free 更难利用（释放的 Node 空间不会被 Str 重用）
└── 每个分区独立管理碎片
```

### 2.3 分配策略

```
PartitionAlloc 的内存层次：

Super Page (2MB，对齐到 2MB 边界)
  │
  ├── Slot Span（多个连续页面）
  │     │
  │     ├── Slot（固定大小的槽位）
  │     │   每个槽位存放一个对象
  │     │
  │     └── Slot 大小按 bucket 分档：
  │         8, 16, 32, 48, 64, 80, 96, ...
  │         256, 384, 512, ... 最大到直接 mmap
  │
  └── Metadata（元数据，与数据页分离）
        存储空闲列表、分配计数等

分配流程：
  1. 根据请求大小找到对应 bucket
  2. 从 bucket 的空闲列表取一个 slot
  3. 如果空闲列表为空 → 从 Slot Span 分配新 slot
  4. 如果 Slot Span 已满 → 分配新的 Slot Span
  5. 超大对象 → 直接 mmap
```

### 2.4 安全特性

```
1. MiraclePtr (BackupRefPtr)
   引用计数指针，检测 Use-After-Free
   释放时如果仍有引用 → 隔离而非真正释放
   后续访问时崩溃在可控位置（而非被利用）

2. 内存清零
   释放内存时清零（可选）
   防止信息泄漏

3. 元数据分离
   空闲列表等元数据不存储在数据页中
   防止堆溢出覆盖元数据

4. Guard Pages
   Super Page 之间插入保护页
   溢出到保护页 → 立即崩溃

5. 内存标记 (MTE, Memory Tagging Extension)
   ARM v8.5+ 硬件支持
   每个分配带标签，Use-After-Free 立即检测
```

---

## 3. Oilpan (Blink GC)

### 3.1 什么是 Oilpan？

```
Oilpan 是 Blink 渲染引擎的垃圾回收器
管理 DOM 节点、CSS 对象等 Blink 内部 C++ 对象

为什么 C++ 也需要 GC？
├── DOM 对象之间有复杂的引用关系（循环引用很常见）
│   例：Node → ChildNode → ParentNode → Node
├── DOM 对象被 JS 和 C++ 同时引用
│   例：document.getElementById("foo") 返回的 DOM 节点
├── 手动管理这些引用的生命周期极其困难且容易出错
└── GC 自动处理循环引用和跨语言引用
```

### 3.2 Oilpan 的工作方式

```
垃圾回收对象声明：
  class MyNode : public GarbageCollected<MyNode> {
      Member<MyNode> child_;   // GC 追踪的指针（类似智能指针）
      WeakMember<MyNode> peer_; // 弱引用（不阻止回收）

      void Trace(Visitor* visitor) const {
          visitor->Trace(child_);
          visitor->Trace(peer_);
      }
  };

GC 根：
├── 栈上的局部变量
├── 全局/静态变量
├── V8 引用的 DOM 对象
└── 定时器、事件监听器等

回收算法：
  标记（Mark）：从根出发遍历对象图，标记可达对象
  清除（Sweep）：回收不可达对象
  整理（Compact）：可选，移动对象减少碎片
```

### 3.3 跨堆引用 (V8 ↔ Oilpan)

```
JS 对象（V8 堆）和 DOM 对象（Oilpan 堆）互相引用：

  V8 Heap                    Oilpan Heap
┌───────────┐             ┌───────────────┐
│ JS Object │────引用────→│ DOM Element   │
│           │←───引用─────│               │
└───────────┘             └───────────────┘

跨堆引用的 GC 挑战：
  V8 GC 不知道 Oilpan 堆的引用
  Oilpan GC 不知道 V8 堆的引用
  → 可能误回收仍在使用的对象

解决方案：Unified Heap（统一堆）
  V8 和 Oilpan 协调 GC：
  1. 共享标记阶段（Unified Marking）
  2. V8 标记时会追踪到 Oilpan 对象
  3. Oilpan 标记时会追踪到 V8 对象
  4. 两个堆同步决定哪些对象可以回收
```

---

## 4. 内存压力处理

### 4.1 内存压力信号

```
操作系统 / Chromium 内部检测到内存不足：

内存压力级别：
├── Moderate（中等）：系统内存使用较高
│   → 清理不重要的缓存
│   → 减少内存预分配
│
├── Critical（严重）：系统内存即将耗尽
│   → 激进清理缓存
│   → 触发 GC
│   → 丢弃后台标签页
│
└── 自定义阈值
    → Chromium 内部设定进程内存上限
    → 超出时触发内部压力处理
```

### 4.2 Tab Discarding（标签页丢弃）

```
内存不足时，Chrome 会自动丢弃后台标签页：

丢弃优先级（先丢弃优先级低的）：
1. 内部页面（chrome://newtab 等）
2. 长时间未访问的后台标签页
3. 最近较少使用的后台标签页
4. 固定的标签页（优先保留）
5. 当前活动标签页（永远不丢弃）

丢弃过程：
  杀死 Renderer 进程 → 释放内存
  标签页仍在标签栏显示
  用户点击时重新加载
```

### 4.3 内存限制策略

```
V8 堆大小限制：
├── 64 位系统：默认最大 ~4GB
├── 32 位系统：默认最大 ~700MB
├── 移动端：默认最大 ~512MB
└── 通过 --max-old-space-size 调整

Renderer 进程内存限制：
├── Chrome 会监控每个 Renderer 的内存使用
├── 超出阈值时触发 GC 和缓存清理
├── 极端情况下杀死进程（OOM Killer）
└── Android 上更激进（系统可能随时杀后台进程）
```

---

## 5. 内存问题排查

### 5.1 常用工具

```
Chrome 内置：
├── chrome://memory-internals — 各进程内存详情
├── chrome://tracing — 性能追踪（包含内存事件）
├── DevTools → Memory 面板
│   ├── Heap Snapshot（JS 堆快照）
│   ├── Allocation Timeline（分配时间线）
│   └── Allocation Sampling（分配采样）
└── DevTools → Performance Monitor — 实时内存监控

外部工具：
├── Valgrind / ASan — 内存错误检测
├── Heap Profiler — C++ 堆分析
└── about:tracing 的 memory-infra category
```

### 5.2 常见内存问题

```
1. JS 内存泄漏
   ├── 意外的全局变量
   ├── 被遗忘的定时器和回调
   ├── DOM 引用未释放（分离的 DOM 树）
   └── 闭包持有大量数据

2. C++ 内存泄漏
   ├── 忘记释放 PartitionAlloc 分配的内存
   ├── 循环引用（Oilpan 管理的除外）
   └── 事件监听器未注销

3. 内存碎片
   ├── 频繁分配/释放不同大小的对象
   └── PartitionAlloc 的 bucket 机制缓解但不完全消除

4. GPU 内存泄漏
   ├── 纹理未释放
   ├── 合成层过多
   └── Canvas / WebGL 资源未销毁
```

---

## 6. 面试常见问题

**Q1：Chrome 为什么这么吃内存？如何缓解？**

> 多进程架构下每个进程有独立地址空间（V8 堆、Blink 堆、共享库映射等），空白标签页也需 ~30-50MB。站点隔离进一步增加进程数。缓解手段：Tab Discarding（丢弃后台标签页）、进程复用（同站点共享）、PartitionAlloc 优化、V8 Heap 限制、Oilpan 及时回收。

**Q2：PartitionAlloc vs 系统 malloc 的区别？**

> PartitionAlloc 按类型分区隔离（不同类型对象不混放），有元数据分离、Guard Page、MiraclePtr 等安全特性。性能上通过 bucket 分档和空闲列表实现快速分配。系统 malloc（如 glibc ptmalloc）是通用分配器，不做类型隔离，安全性较弱。

**Q3：Oilpan 和 V8 GC 的关系？**

> Oilpan 管理 Blink 的 C++ 对象（DOM 节点等），V8 GC 管理 JS 对象。两者通过 Unified Heap 机制协调：共享标记阶段，跨堆引用会被正确追踪，避免误回收。这解决了 JS 引用 DOM、DOM 引用 JS 的循环引用问题。

**Q4：什么是 Use-After-Free？PartitionAlloc 如何防御？**

> Use-After-Free 是对象释放后仍被访问的漏洞，常被用于安全攻击。PartitionAlloc 通过以下方式防御：类型分区（释放的内存只会被同类型对象重用）、MiraclePtr（引用计数检测悬空指针）、内存清零（防止旧数据泄漏）、Guard Page（溢出立即崩溃）。

**Q5：如何排查 Web 页面的内存泄漏？**

> 1. DevTools Memory 面板拍 Heap Snapshot，对比不同时间点的快照差异
> 2. 查找 Detached DOM Tree（分离的 DOM 树，已从页面移除但 JS 仍引用）
> 3. 检查定时器、事件监听器是否在组件卸载时清理
> 4. 使用 Allocation Timeline 观察内存增长模式
> 5. Performance Monitor 监控 JS heap size 趋势
