# 04 - V8 引擎 (V8 Engine)

## 简介

V8 是 Google 开发的高性能 JavaScript/WebAssembly 引擎，用于 Chrome 和 Node.js。它将 JS 编译为机器码执行，并通过多种优化技术实现接近原生的性能。

---

## 1. V8 编译管线

### 1.1 整体架构

```
JavaScript 源码
      │
      ▼
┌─────────────┐
│   Parser    │  语法解析 → AST（抽象语法树）
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  Ignition   │  解释器：AST → Bytecode（字节码）
│  (解释器)   │  直接执行字节码（启动快）
└──────┬──────┘
       │
       │ 收集类型反馈（Type Feedback）
       │ 热点函数检测
       ▼
┌─────────────┐
│  TurboFan   │  优化编译器：Bytecode → 优化后的机器码
│ (优化编译器) │  基于类型反馈进行推测性优化
└──────┬──────┘
       │
       │ 如果类型假设失败
       ▼
  Deoptimization（去优化）→ 回退到 Ignition 执行字节码
```

### 1.2 各阶段详解

**Parser（解析器）**：

```
源码 → Token → AST

优化：
├── Lazy Parsing（懒解析）
│   不立即解析所有函数体
│   只在函数被调用时才完整解析
│   减少启动时间
│
├── Pre-parser（预解析器）
│   快速扫描函数体，只检查语法正确性
│   不生成 AST
│   标记变量作用域
│
└── Streaming（流式解析）
    边下载边解析，不等整个文件下载完
```

**Ignition（解释器）**：

```
AST → 字节码（Bytecode）

字节码是平台无关的中间表示：
  LdaSmi [42]        // 加载整数 42
  Star r0            // 存入寄存器 r0
  LdaSmi [58]        // 加载整数 58
  Add r0             // 加法
  Return             // 返回

特点：
├── 启动快：字节码生成比机器码编译快得多
├── 内存省：字节码比 AST 和机器码都小
├── 收集反馈：执行过程中记录变量的实际类型
└── 寄存器架构：Ignition 使用虚拟寄存器机
```

**TurboFan（优化编译器）**：

```
字节码 + 类型反馈 → 优化后的机器码

优化手段：
├── 内联（Inlining）
│   将频繁调用的小函数展开到调用点
│
├── 推测性优化（Speculative Optimization）
│   根据类型反馈假设类型，生成特化代码
│   例：如果 add(a, b) 总是被传入 int，生成整数加法的机器码
│
├── 逃逸分析（Escape Analysis）
│   对象未逃逸出函数 → 栈上分配（避免堆分配和 GC）
│
├── 常量折叠（Constant Folding）
│   编译期计算常量表达式
│
└── 死代码消除（Dead Code Elimination）
    删除不会被执行的代码
```

**Deoptimization（去优化）**：

```
当运行时类型与 TurboFan 的假设不符时：

function add(a, b) { return a + b; }

// 前 10000 次：add(1, 2) → TurboFan 优化为整数加法
// 第 10001 次：add("hello", "world") → 类型假设失败！

触发去优化：
1. 丢弃优化后的机器码
2. 回退到 Ignition 字节码执行
3. 重新收集类型反馈
4. 可能再次被 TurboFan 优化（使用更宽泛的类型假设）

去优化的代价：
├── 性能骤降（从机器码回到字节码）
├── 重新编译的开销
└── 频繁去优化会导致性能不稳定
```

---

## 2. 隐藏类 (Hidden Classes / Maps)

### 2.1 为什么需要隐藏类？

```
JavaScript 是动态类型语言，对象属性可以随时增删：
  const obj = {};
  obj.x = 1;    // 动态添加属性
  obj.y = 2;
  delete obj.x;  // 动态删除

如果每次属性访问都做字典查找 → 很慢

V8 的方案：Hidden Class（隐藏类，内部称为 Map）
  给每个对象关联一个描述其"形状"的隐藏类
  属性访问通过隐藏类的偏移量直接定位 → 接近 C++ struct 的速度
```

### 2.2 隐藏类转换链

```
const obj = {};        // HiddenClass H0: {}
obj.x = 1;             // HiddenClass H1: { x: offset 0 }
obj.y = 2;             // HiddenClass H2: { x: offset 0, y: offset 1 }

隐藏类转换链：
  H0 ──(添加 x)──→ H1 ──(添加 y)──→ H2

如果另一个对象以相同顺序添加相同属性：
  const obj2 = {};     // H0
  obj2.x = 10;         // H1（复用！）
  obj2.y = 20;         // H2（复用！）
  obj 和 obj2 共享 H2

但如果属性添加顺序不同：
  const obj3 = {};     // H0
  obj3.y = 10;         // H3: { y: offset 0 }  ← 不同的隐藏类！
  obj3.x = 20;         // H4: { y: offset 0, x: offset 1 }
  obj3 不能与 obj 共享隐藏类

优化建议：
  ✅ 在构造函数中以相同顺序初始化所有属性
  ✅ 使用类/构造函数创建对象（结构一致）
  ❌ 避免动态增删属性
  ❌ 避免 delete 操作（会使对象退化为字典模式）
```

---

## 3. 内联缓存 (Inline Caches)

```
问题：obj.x 如何快速访问？

第一次访问：
  obj.x → 查找隐藏类 → 找到 x 在 offset 0 → 缓存这个信息

后续访问（同形状对象）：
  obj.x → 检查隐藏类是否一致 → 直接用 offset 0 读取 → 极快

内联缓存状态：
├── Uninitialized：未执行过
├── Monomorphic：只见过一种隐藏类 → 最快（直接偏移访问）
├── Polymorphic：见过 2-4 种隐藏类 → 较快（switch-case 检查）
└── Megamorphic：见过 >4 种隐藏类 → 最慢（退化为字典查找）

// ❌ 导致 Megamorphic 的代码
function getX(obj) { return obj.x; }
getX({ x: 1 });           // 形状 1
getX({ x: 1, y: 2 });     // 形状 2
getX({ x: 1, z: 3 });     // 形状 3
getX({ x: 1, a: 4 });     // 形状 4
getX({ x: 1, b: 5 });     // 形状 5 → Megamorphic！

// ✅ 保持对象形状一致
class Point { constructor(x, y) { this.x = x; this.y = y; } }
getX(new Point(1, 2));     // 所有 Point 共享隐藏类 → Monomorphic
getX(new Point(3, 4));
```

---

## 4. 垃圾回收 (Garbage Collection)

### 4.1 堆内存结构

```
V8 堆分为多个空间：

┌──────────────────────────────────────────────────┐
│                    V8 Heap                        │
├───────────────┬──────────────────┬───────────────┤
│  New Space    │    Old Space     │  Large Object  │
│  (新生代)     │    (老生代)      │    Space       │
│  ~1-8 MB      │    ~数百 MB      │               │
│               │                  │               │
│ ┌───┬────┐   │  ┌────────────┐  │  大对象直接    │
│ │From│ To │   │  │ 长期存活的  │  │  分配在这里   │
│ │   │    │   │  │ 对象        │  │               │
│ └───┴────┘   │  └────────────┘  │               │
├───────────────┼──────────────────┼───────────────┤
│   Code Space  │    Map Space     │  其他...       │
│  (编译后代码) │  (隐藏类对象)    │               │
└───────────────┴──────────────────┴───────────────┘
```

### 4.2 新生代 GC — Scavenge (Minor GC)

```
算法：Cheney's 半空间复制算法

新生代分为两个半空间：From 和 To

分配：在 From 空间分配
回收：
  1. 从根对象开始，标记所有可达对象
  2. 将可达对象从 From 复制到 To
  3. From 和 To 互换角色
  4. 旧的 From 空间整体清空

    From Space              To Space
  ┌──────────────┐      ┌──────────────┐
  │ A  B  C  D   │      │              │
  │ ↑     ↑      │      │              │
  │ 可达  可达    │      │              │
  └──────────────┘      └──────────────┘
         │                     │
         ▼ 复制可达对象         ▼
  ┌──────────────┐      ┌──────────────┐
  │  （清空）     │      │ A  C         │
  │              │      │              │
  └──────────────┘      └──────────────┘
  现在变成 To            现在变成 From

晋升（Promotion）：
  对象在新生代存活超过两次 GC → 晋升到老生代

特点：
├── 速度快（~1-5ms）
├── 频率高（可能每秒多次）
├── 只处理新生代，范围小
└── 停顿短，对用户几乎无感
```

### 4.3 老生代 GC — Mark-Sweep-Compact (Major GC)

```
三个阶段：

1. 标记（Mark）
   从根对象出发，遍历对象图，标记所有可达对象
   使用三色标记法：
   ├── 白色：未访问（默认）
   ├── 灰色：已发现但子节点未处理
   └── 黑色：已处理完毕

2. 清除（Sweep）
   遍历堆，回收所有白色（不可达）对象
   产生内存碎片

3. 整理（Compact）（可选）
   移动存活对象，消除碎片
   更新所有指针
   代价大，仅在碎片严重时执行
```

### 4.4 GC 优化技术

```
V8 使用多种技术减少 GC 停顿：

1. 增量标记（Incremental Marking）
   将标记工作分成小块，穿插在 JS 执行之间
   每次只做 ~5ms 的标记工作
   避免一次性长停顿

   JS执行 │ 标记 │ JS执行 │ 标记 │ JS执行 │ 标记 │ 清除
   ────────┼──────┼────────┼──────┼────────┼──────┼──────

2. 并发标记（Concurrent Marking）
   标记工作在后台线程执行
   主线程继续执行 JS
   最终需要短暂停顿同步状态

3. 并行清除（Parallel Sweeping）
   多个线程同时清除不同区域

4. 并行整理（Parallel Compaction）
   多个线程同时移动和整理对象

5. 懒清除（Lazy Sweeping）
   不立即清除所有空间
   在分配新对象时按需清除

实际 GC 停顿时间：
├── Minor GC (Scavenge): ~1-5ms
├── Major GC (增量): ~5-50ms（分散到多个小停顿）
└── 最坏情况: ~100ms（大堆、大量存活对象）
```

---

## 5. Isolate 与 Context

```
Isolate（隔离区）：
├── 一个独立的 V8 实例
├── 有自己的堆、GC、编译管线
├── 同一时刻只有一个线程可以执行
├── 不同 Isolate 之间完全隔离
└── 通常每个 Renderer 进程一个 Isolate

Context（上下文）：
├── 一个独立的 JS 执行环境
├── 有自己的全局对象（window/global）
├── 同一个 Isolate 中可以有多个 Context
├── 不同 Context 之间变量不可见
└── 用于隔离同一页面中不同 frame 的 JS 环境

关系：
  Isolate 1
  ├── Context A（主页面）
  │   └── window.x = 1
  ├── Context B（iframe 1）
  │   └── window.x = 2（独立）
  └── Context C（iframe 2）
```

---

## 6. 面试常见问题

**Q1：V8 的编译流程？**

> JS 源码 → Parser（AST）→ Ignition（字节码，直接执行）→ 热点函数被 TurboFan 编译为优化机器码。如果类型假设失败则 Deopt 回退到 Ignition。早期有 Full-codegen 和 Crankshaft，现已移除。

**Q2：什么是隐藏类？为什么重要？**

> V8 给每个对象关联一个描述其"形状"的隐藏类（Map），记录属性名→偏移量的映射。相同形状的对象共享隐藏类。属性访问通过偏移量直接寻址，接近 C++ struct 的性能。以不同顺序添加属性会产生不同的隐藏类，降低性能。

**Q3：什么是内联缓存？Monomorphic vs Megamorphic？**

> 内联缓存在属性访问点缓存隐藏类和偏移量。Monomorphic：只见过一种形状，最快。Polymorphic：2-4 种形状，较快。Megamorphic：>4 种形状，退化为字典查找，最慢。保持对象形状一致是 JS 性能优化的关键。

**Q4：V8 的垃圾回收机制？**

> 分代式 GC。新生代（New Space）用 Scavenge 算法（半空间复制），速度快但空间小。老生代（Old Space）用 Mark-Sweep-Compact（标记-清除-整理）。通过增量标记、并发标记、并行清除等技术减少 GC 停顿。

**Q5：如何避免 V8 去优化？**

> 1. 保持函数参数类型一致（避免类型变化）
> 2. 避免使用 `arguments` 对象（改用 rest 参数）
> 3. 避免 `try-catch` 包含大量代码（会阻止优化）
> 4. 避免 `delete` 操作（破坏隐藏类）
> 5. 使用类和构造函数（保证对象形状一致）

**Q6：V8 和 JavaScriptCore (JSC) 的主要区别？**

> V8（Chrome）用 Ignition + TurboFan（两级）；JSC（Safari）用 LLInt + Baseline + DFG + FTL（四级）。V8 倾向更激进的优化和去优化；JSC 倾向更多层级的渐进式优化。V8 的 GC 是增量并发的；JSC 使用 Riptide GC。

**Q7：为什么 JS 是单线程的？V8 内部有多线程吗？**

> JS 执行是单线程的（同一 Context 同一时刻只有一个线程执行 JS），这是语言设计决定的（避免 DOM 操作竞争）。但 V8 内部使用多线程：后台编译线程（TurboFan）、并发 GC 标记线程、并行 GC 清除线程、字节码编译线程等。
