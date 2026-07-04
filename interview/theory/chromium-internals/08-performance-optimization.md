# 08 - 性能分析与优化 (Performance Optimization)

## 简介

Chromium 性能优化涵盖渲染、JS 执行、内存、网络等多个维度。本文从问题定位到优化手段，系统梳理 C++ 客户端/浏览器内核开发中常见的性能分析与优化方向。

---

## 1. 性能指标体系

### 1.1 Core Web Vitals（核心 Web 指标）

```
Google 定义的三大核心用户体验指标：

┌─────────────────────────────────────────────────────────┐
│  LCP (Largest Contentful Paint)                         │
│  最大内容绘制：最大可见元素渲染完成的时间                  │
│  好: < 2.5s  |  中: 2.5-4s  |  差: > 4s                │
├─────────────────────────────────────────────────────────┤
│  INP (Interaction to Next Paint)                        │
│  交互到下一次绘制：用户交互到视觉反馈的延迟               │
│  好: < 200ms  |  中: 200-500ms  |  差: > 500ms         │
├─────────────────────────────────────────────────────────┤
│  CLS (Cumulative Layout Shift)                          │
│  累积布局偏移：页面内容意外移动的程度                      │
│  好: < 0.1  |  中: 0.1-0.25  |  差: > 0.25             │
└─────────────────────────────────────────────────────────┘
```

### 1.2 其他关键指标

```
加载相关：
├── TTFB (Time to First Byte)     — 首字节时间
├── FCP (First Contentful Paint)  — 首次内容绘制
├── TTI (Time to Interactive)     — 可交互时间
└── Total Blocking Time (TBT)     — 总阻塞时间

渲染相关：
├── FPS (Frames Per Second)       — 帧率（目标 60fps）
├── Frame Drop Rate               — 丢帧率
├── Main Thread Busy Time         — 主线程繁忙时间
└── Long Task Count               — 长任务数量（>50ms）

内存相关：
├── JS Heap Size                  — V8 堆大小
├── DOM Node Count                — DOM 节点数量
├── Total Process Memory          — 进程总内存
└── GPU Memory Usage              — GPU 内存使用
```

---

## 2. 渲染性能优化

### 2.1 避免不必要的重排 (Layout)

```
重排是渲染流水线中最昂贵的操作

触发重排的操作：
├── 修改几何属性：width, height, margin, padding, border
├── 修改布局属性：display, position, float, overflow
├── 修改文字属性：font-size, font-family, line-height
├── 窗口 resize
└── 读取布局属性（触发强制同步布局）：
    offsetWidth, offsetHeight, offsetTop, offsetLeft
    clientWidth, clientHeight, scrollWidth, scrollHeight
    getComputedStyle(), getBoundingClientRect()

优化策略：

// ❌ 反复触发重排
for (let i = 0; i < 100; i++) {
    el.style.left = el.offsetLeft + 1 + 'px';  // 读+写交替
}

// ✅ 批量读，批量写
const left = el.offsetLeft;
for (let i = 0; i < 100; i++) {
    el.style.left = left + i + 1 + 'px';
}

// ✅ 使用 transform 代替 left/top
el.style.transform = `translateX(${offset}px)`;  // 只触发合成

// ✅ 使用 requestAnimationFrame 合并修改
requestAnimationFrame(() => {
    el.style.width = '200px';
    el.style.height = '100px';
    // 同一个 rAF 内的多次修改只触发一次重排
});

// ✅ 使用 DocumentFragment 批量 DOM 操作
const fragment = document.createDocumentFragment();
for (let i = 0; i < 1000; i++) {
    const div = document.createElement('div');
    fragment.appendChild(div);
}
container.appendChild(fragment);  // 一次性插入
```

### 2.2 合成层优化

```
目标：让动画只触发合成（最便宜的渲染步骤）

只触发合成的属性（Compositor-only properties）：
├── transform
├── opacity
├── filter（部分）
└── clip-path（部分）

// ✅ 合成器动画
.animate {
    will-change: transform;
    transition: transform 0.3s ease;
}
.animate:hover {
    transform: scale(1.1);  // 只触发合成，不经过主线程
}

// ❌ 主线程动画
.animate:hover {
    width: 110%;  // 触发 Layout → Paint → Composite
}

合成层管理：
├── ✅ 只对动画元素使用 will-change
├── ❌ 不要 * { will-change: transform }（每个元素都变合成层）
├── ✅ 动画结束后移除 will-change
├── ✅ 给动画元素设高 z-index（避免相邻元素被迫提升）
└── 用 DevTools Layers 面板监控合成层数量和大小
```

### 2.3 减少绘制区域

```
// ✅ 使用 content-visibility: auto
// 不可见区域跳过 Layout/Paint，巨大性能提升
.card {
    content-visibility: auto;
    contain-intrinsic-size: 200px 300px;  // 预估大小
}

// ✅ 使用 CSS contain 限制影响范围
.widget {
    contain: layout style paint;
    // 该元素内部的变化不会影响外部布局/绘制
}

// ✅ 减少绘制复杂度
// box-shadow, border-radius, gradient 等有绘制开销
// 性能敏感区域使用简单样式
```

---

## 3. JavaScript 性能优化

### 3.1 减少主线程阻塞

```
长任务（>50ms）会阻塞用户交互和渲染

// ❌ 长任务阻塞主线程
function processLargeArray(data) {
    for (let i = 0; i < 1000000; i++) {
        heavyComputation(data[i]);  // 可能耗时数百毫秒
    }
}

// ✅ 分片处理（Time Slicing）
async function processLargeArray(data) {
    const CHUNK_SIZE = 1000;
    for (let i = 0; i < data.length; i += CHUNK_SIZE) {
        const chunk = data.slice(i, i + CHUNK_SIZE);
        chunk.forEach(item => heavyComputation(item));

        // 让出主线程，允许渲染和事件处理
        await new Promise(resolve => setTimeout(resolve, 0));
        // 或 await scheduler.yield();  (Scheduler API)
    }
}

// ✅ 使用 Web Worker 卸载计算
const worker = new Worker('heavy-task.js');
worker.postMessage(largeData);
worker.onmessage = (e) => {
    updateUI(e.data);  // 只在主线程做 UI 更新
};

// ✅ 使用 requestIdleCallback 处理低优先级任务
requestIdleCallback((deadline) => {
    while (deadline.timeRemaining() > 0 && tasks.length > 0) {
        doNextTask(tasks.shift());
    }
});
```

### 3.2 V8 优化友好的代码

```
// ✅ 保持对象形状一致（优化隐藏类和内联缓存）
class Point {
    constructor(x, y) {
        this.x = x;  // 所有 Point 实例形状一致
        this.y = y;
    }
}

// ❌ 动态添加属性
function createPoint(x, y) {
    const p = {};
    p.x = x;
    if (y !== undefined) p.y = y;  // 有时有 y，有时没有 → 不同形状
    return p;
}

// ✅ 保持函数参数类型一致
function add(a, b) { return a + b; }
add(1, 2);       // int + int → TurboFan 优化为整数加法
add(3, 4);       // 仍然 int → monomorphic
// ❌ add("a", "b");  // 类型变了 → 去优化

// ✅ 避免 delete（破坏隐藏类）
// ❌ delete obj.x;
// ✅ obj.x = undefined;

// ✅ 使用 TypedArray 处理数值数据
const arr = new Float64Array(10000);  // 连续内存，无装箱
// ❌ const arr = new Array(10000);   // 可能是稀疏数组
```

---

## 4. 内存优化

### 4.1 减少内存占用

```
DOM 节点管理：
├── ✅ 虚拟列表（只渲染可见区域的 DOM）
│   大列表从 10000 个 DOM 节点 → 50 个可见节点
├── ✅ 及时移除不需要的 DOM 节点
├── ❌ 避免分离的 DOM 树（JS 引用已移除的 DOM）
└── ✅ 使用 IntersectionObserver 懒加载

图片优化：
├── ✅ 使用合适尺寸的图片（不要 4K 图片缩小显示）
├── ✅ 使用现代格式（WebP / AVIF）
├── ✅ 懒加载（loading="lazy"）
├── ✅ 使用 srcset 响应式图片
└── ✅ 及时回收 Canvas/WebGL 资源

缓存管理：
├── ✅ 设置合理的缓存上限
├── ✅ LRU 策略淘汰不常用缓存
└── ✅ 监听 memory-pressure 事件清理缓存
```

### 4.2 内存泄漏防范

```
常见泄漏模式：

1. 事件监听器未清理
   // ❌
   window.addEventListener('resize', this.onResize);
   // 组件销毁后未 removeEventListener → 闭包持有 this → 整个组件泄漏

   // ✅
   destroy() {
       window.removeEventListener('resize', this.onResize);
   }

2. 定时器未清理
   // ❌
   setInterval(() => this.update(), 1000);
   // 组件销毁后 setInterval 仍运行

   // ✅
   this.timerId = setInterval(() => this.update(), 1000);
   destroy() { clearInterval(this.timerId); }

3. 闭包引用
   // ❌
   function setup() {
       const hugeData = loadMB();
       button.onclick = () => {
           console.log('clicked');
           // 闭包捕获了 hugeData（即使没用到，某些引擎也不会优化）
       };
   }

4. 分离的 DOM 树
   // ❌
   const ref = document.getElementById('panel');
   document.body.removeChild(panel);
   // panel 已从 DOM 移除，但 ref 仍持有引用 → 无法 GC
```

---

## 5. 网络优化

```
资源加载优化：
├── ✅ HTTP/2 多路复用（一个连接并发请求）
├── ✅ Brotli 压缩（比 gzip 小 15-20%）
├── ✅ 资源提示
│   <link rel="preconnect" href="https://cdn.example.com">
│   <link rel="dns-prefetch" href="https://api.example.com">
│   <link rel="preload" href="critical.css" as="style">
│   <link rel="prefetch" href="next-page.js">
├── ✅ 代码分割 + 按需加载
├── ✅ Service Worker 缓存策略
└── ✅ CDN + 边缘缓存

关键渲染路径优化：
├── ✅ 内联关键 CSS（首屏渲染所需的最小 CSS）
├── ✅ 异步加载非关键 CSS
├── ✅ defer/async 加载 JS
├── ✅ 减少重定向次数
└── ✅ 使用 103 Early Hints 提前推送资源
```

---

## 6. 性能分析工具

### 6.1 Chrome DevTools

```
Performance 面板：
├── 录制页面加载或交互过程
├── 查看火焰图（Flame Chart）定位耗时函数
├── 识别长任务（红色三角标记）
├── 查看各阶段时间（Loading/Scripting/Rendering/Painting）
└── 分析帧率和丢帧原因

Memory 面板：
├── Heap Snapshot — 对比快照找泄漏
├── Allocation Timeline — 观察分配模式
├── Allocation Sampling — 低开销的分配采样
└── Detached DOM Tree 检测

Layers 面板：
├── 查看合成层数量和大小
├── 了解图层提升原因
└── 检测合成层爆炸

Lighthouse：
├── 自动化性能审计
├── 给出 Core Web Vitals 分数
└── 提供具体优化建议
```

### 6.2 chrome://tracing (Trace Event)

```
Chromium 内部的性能追踪系统，C++ 客户端开发必备

使用方式：
1. 打开 chrome://tracing
2. 点击 Record
3. 选择需要追踪的类别：
   ├── blink           — Blink 渲染引擎事件
   ├── cc              — 合成器事件
   ├── gpu             — GPU 事件
   ├── v8              — V8 引擎事件
   ├── memory-infra    — 内存分析
   ├── input           — 输入事件
   └── navigation      — 导航事件
4. 执行操作
5. 停止录制，分析 trace

C++ 代码中的埋点：
  #include "base/trace_event/trace_event.h"

  void RenderFrame() {
      TRACE_EVENT0("renderer", "RenderFrame");
      // ... 渲染逻辑 ...

      {
          TRACE_EVENT1("renderer", "Layout", "node_count", count);
          // ... 布局逻辑 ...
      }
  }

分析要点：
├── 主线程时间线：哪些任务耗时最长？
├── 合成线程时间线：提交和光栅化是否及时？
├── GPU 线程：是否有 GPU 瓶颈？
├── IPC 消息：进程间通信延迟？
└── 帧的 begin-to-display 延迟
```

### 6.3 Perfetto

```
Chromium 正在迁移到 Perfetto 追踪系统

优势：
├── 更高效的追踪格式（protobuf）
├── 支持系统级追踪（CPU 调度、内存、IO）
├── 基于 Web 的分析界面（ui.perfetto.dev）
├── 支持 SQL 查询追踪数据
└── 统一了 Chrome tracing 和 Android systrace

使用：
  chrome://tracing → 选择 Perfetto UI
  或直接访问 ui.perfetto.dev
```

---

## 7. C++ 层面的性能优化

```
针对 Chromium C++ 代码的优化方向：

1. 避免不必要的拷贝
   ✅ 使用 std::move / 移动语义
   ✅ 返回值优化（RVO/NRVO）
   ✅ 传引用 / string_view

2. 减少内存分配
   ✅ 对象池 / 内存池
   ✅ SmallVector（栈上小缓冲区）
   ✅ 预分配（reserve）
   ✅ 避免频繁 new/delete

3. 数据结构选择
   ✅ flat_map / flat_set（小数据量，缓存友好）
   ✅ absl::InlinedVector（小数组栈上分配）
   ✅ 根据访问模式选择（随机访问 vs 遍历 vs 查找）

4. 锁优化
   ✅ 减小临界区
   ✅ 使用 base::Lock 而非 std::mutex（Chromium 封装）
   ✅ 读多写少用 base::ReadWriteLock
   ✅ 考虑无锁数据结构（base::AtomicFlag 等）

5. 任务调度
   ✅ 使用 base::ThreadPool 而非手动创建线程
   ✅ 区分 USER_BLOCKING / USER_VISIBLE / BEST_EFFORT 优先级
   ✅ 将非紧急工作延迟到空闲时间
```

---

## 8. 面试常见问题

**Q1：页面卡顿怎么排查？**

> 1. DevTools Performance 面板录制交互过程
> 2. 查看火焰图，定位主线程长任务（红色三角标记的 >50ms 任务）
> 3. 分析长任务内容：JS 执行过长？强制同步布局？大量 DOM 操作？
> 4. 检查帧率：是否有丢帧？丢帧时主线程在做什么？
> 5. 检查合成：Layers 面板看合成层数量是否爆炸
> 6. 检查内存：Memory 面板看是否有频繁 GC

**Q2：如何优化首屏加载时间？**

> 1. 减少关键资源：内联关键 CSS、defer 非关键 JS
> 2. 压缩资源：Brotli 压缩、Tree Shaking、代码分割
> 3. 网络优化：CDN、preconnect、HTTP/2
> 4. 优化 TTFB：服务端优化、缓存策略
> 5. 渲染优化：减少 DOM 深度、避免阻塞渲染的资源
> 6. 监控指标：LCP < 2.5s、FCP < 1.8s

**Q3：什么是长任务 (Long Task)？如何避免？**

> 连续执行超过 50ms 的任务。长任务期间用户输入无法被处理，导致页面无响应。避免方法：任务分片（setTimeout/requestIdleCallback/scheduler.yield）、将计算移到 Web Worker、减少不必要的 JS 执行、避免强制同步布局。

**Q4：为什么 CSS 动画比 JS 动画性能好？**

> CSS 的 transform/opacity 动画由合成线程驱动，不经过主线程，即使 JS 繁忙也不丢帧。JS 动画（修改 style.left 等）在主线程执行，与 JS 逻辑竞争资源。CSS 动画还可以被浏览器优化（如自动 GPU 加速、智能帧调度）。

**Q5：chrome://tracing 和 DevTools Performance 有什么区别？**

> DevTools Performance 面向 Web 开发者，展示 JS/渲染/网络层面的信息，UI 友好。chrome://tracing 面向浏览器内核开发者，展示 C++ 层面的追踪事件（Blink、cc、GPU、V8 内部），粒度更细，适合排查引擎层面的性能问题。Perfetto 是下一代追踪工具，功能更强。

**Q6：content-visibility: auto 的工作原理？**

> 浏览器跳过不可见区域（视口外）的 Layout 和 Paint，只在元素进入视口时才渲染。需要配合 `contain-intrinsic-size` 提供预估大小，否则滚动条可能跳动。对于长列表/长页面，可以将渲染工作量减少 90% 以上。

**Q7：如何优化 Chromium 的进程间通信性能？**

> 1. 减少 IPC 消息数量（合并多个小消息为一个批量消息）
> 2. 减少消息大小（只传必要数据，大数据用共享内存）
> 3. 使用 Mojo Data Pipe 流式传输大块数据
> 4. 使用 Associated Interface 保证相关消息顺序（避免不必要的等待）
> 5. 避免同步 IPC（阻塞调用方线程等待响应）

**Q8：渲染性能优化的优先级排序？**

> ```
> 优先级从高到低：
> 1. 消除强制同步布局和布局抖动（最大收益）
> 2. 减少主线程工作量（长任务分片/Worker）
> 3. 使用合成器友好的属性做动画（transform/opacity）
> 4. 减少绘制区域和复杂度（contain/content-visibility）
> 5. 控制合成层数量（避免爆炸）
> 6. 减少 DOM 数量和深度
> 7. 优化 JS 执行（V8 友好代码）
> ```
