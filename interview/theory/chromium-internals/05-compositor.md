# 05 - 合成器与图层 (Compositor & Layers)

## 简介

合成器（Compositor）是 Chromium 渲染架构中实现高性能动画和滚动的关键。它将页面分解为独立图层，在合成线程中组合这些图层生成最终画面，从而避免阻塞主线程。

---

## 1. 为什么需要合成器？

```
没有合成器的渲染模型（旧架构）：
  DOM 变化 → 重排 → 重绘整个页面 → 上屏
  所有工作在主线程，JS 执行阻塞渲染

有合成器的渲染模型：
  DOM 变化 → 只更新受影响的图层 → 合成线程合成 → 上屏
  合成线程独立于主线程，滚动和 CSS 动画不受 JS 阻塞

核心价值：
├── 滚动不卡：合成线程直接处理滚动，主线程被 JS 阻塞也不影响
├── 动画流畅：transform/opacity 动画在合成线程执行
├── 增量更新：只重绘变化的图层，不重绘整个页面
└── GPU 加速：图层以纹理形式存储在 GPU，合成只是纹理拼接
```

---

## 2. 图层系统

### 2.1 图层的概念

```
页面被分解为多个图层（Layer），每个图层独立绘制和合成：

┌─────────────────────────────────────┐
│              页面                     │
│  ┌──────────────────────────────┐   │
│  │  背景图层 (root layer)       │   │
│  │  ┌───────────────────────┐   │   │
│  │  │  固定定位导航栏        │   │   │  ← 独立图层
│  │  └───────────────────────┘   │   │
│  │  ┌───────────────────────┐   │   │
│  │  │  滚动内容区            │   │   │  ← 独立图层（可滚动）
│  │  │  ┌─────────────────┐  │   │   │
│  │  │  │  CSS 动画元素    │  │   │   │  ← 独立图层
│  │  │  └─────────────────┘  │   │   │
│  │  └───────────────────────┘   │   │
│  └──────────────────────────────┘   │
└─────────────────────────────────────┘
```

### 2.2 图层提升条件

```
显式提升（开发者控制）：
├── will-change: transform | opacity | ...
├── transform: translateZ(0) 或 translate3d(...)
├── backface-visibility: hidden
├── position: fixed | sticky
└── <video> / <canvas> / WebGL

隐式提升（浏览器自动）：
├── 元素与已提升图层重叠（Overlap Reason）
│   → 必须提升，否则绘制顺序错误
├── 有 3D CSS 属性的子元素
└── 使用 CSS filter / backdrop-filter

不提升的情况（即使设置了相关属性）：
├── 元素不可见（display:none / visibility:hidden 且无子元素可见）
├── 浏览器判断提升无收益
└── 图层数超过限制，浏览器合并图层（Squashing）
```

### 2.3 图层树

```
Chromium 实际使用两种图层表示：

1. PaintLayer（绘制层）— 主线程
   由布局阶段根据层叠上下文和绘制需求创建
   不一定是合成层

2. cc::Layer（合成层）— 合成线程
   真正提交给合成器的图层
   有独立的 GPU 纹理/位图

关系：
  多个 PaintLayer → 可能合并为一个 cc::Layer
  一个需要独立合成的 PaintLayer → 一个 cc::Layer

合成层的代价：
├── 每个合成层需要 GPU 内存（width × height × 4 bytes）
├── 一个 1920×1080 的图层 = ~8MB GPU 内存
├── 上传纹理到 GPU 有带宽开销
└── 过多图层增加合成复杂度
```

---

## 3. 合成线程工作流

### 3.1 双缓冲树 (Pending / Active Tree)

```
合成器维护两棵树：

Pending Tree（待定树）：
  接收主线程 Commit 提交的新数据
  光栅化在这棵树上进行
  光栅化完成后整棵树被激活

Active Tree（活跃树）：
  当前用于绘制输出的树
  Draw 操作从这棵树取数据

流程：
  主线程 Commit → Pending Tree
                       │
              光栅化（异步）
                       │
                   Activate
                       │
                  Active Tree → Draw → GPU → 屏幕

双缓冲的好处：
├── 光栅化不阻塞绘制（Pending 在光栅化时，Active 继续绘制）
├── 平滑过渡（激活是整棵树切换，不会出现半完成的帧）
└── 主线程 Commit 不阻塞合成线程绘制
```

### 3.2 Tiling（分块）

```
大图层被分成固定大小的 tile：

图层（2000 × 3000px）被分成 tile grid：
┌─────┬─────┬─────┬─────┐
│ T00 │ T01 │ T02 │ T03 │  每个 tile 256×256
├─────┼─────┼─────┼─────┤
│ T10 │ T11 │ T12 │ T13 │
├─────┼─────┼─────┼─────┤
│ T20 │ T21 │ T22 │ T23 │
├─────┼─────┼─────┼─────┤
│ T30 │ T31 │ T32 │ T33 │
└─────┴─────┴─────┴─────┘

分块的好处：
├── 只光栅化视口内和附近的 tile（节省 GPU/CPU）
├── 滚动时增量光栅化新的 tile
├── 不同 tile 可以用不同分辨率（远处低分辨率）
└── 光栅化可以并行（多个 tile 同时光栅化）

tile 优先级：
  NOW:          视口内的 tile（必须立即光栅化）
  SOON:         视口边缘即将进入的 tile（预光栅化）
  EVENTUALLY:   远离视口的 tile（空闲时光栅化）
  NEVER:        不需要的 tile（不光栅化）
```

### 3.3 光栅化 (Rasterization)

```
将绘制指令（Display List）转化为像素位图

两种模式：

1. 软件光栅化（Software Raster）
   在 CPU 上执行（Renderer 进程的光栅化线程池）
   生成位图后上传到 GPU

2. GPU 光栅化（GPU Raster / OOP-R）
   绘制指令发送到 GPU 进程
   GPU 直接执行绘制命令 → 像素
   避免 CPU 绘制 + 上传开销
   Chromium 默认使用（桌面端）

3. 零拷贝光栅化（Zero-copy）
   使用共享内存或 GpuMemoryBuffer
   GPU 光栅化结果直接可用于合成
   避免额外的内存拷贝

光栅化线程：
  Renderer 进程有一个光栅化线程池（通常 4 个线程）
  多个 tile 可以并行光栅化
```

---

## 4. 帧的生命周期

```
一帧从开始到显示的完整流程：

时间 ──────────────────────────────────────────────────→

VSync 信号
  │
  ├── BeginFrame（帧开始）
  │     合成器发起新帧请求
  │
  ├── 主线程响应
  │     ├── 处理输入事件
  │     ├── requestAnimationFrame
  │     ├── Style + Layout + Paint
  │     └── Commit（提交绘制数据给合成线程）
  │
  ├── 合成线程处理
  │     ├── Tiling（分块）
  │     ├── Raster（光栅化）
  │     ├── Activate（激活）
  │     └── Draw（生成 CompositorFrame）
  │
  ├── 提交到 GPU 进程
  │     └── Display Compositor 最终合成
  │
  └── SwapBuffers → 屏幕显示

总预算：~16.67ms（60fps）或 ~8.33ms（120fps）
```

---

## 5. 合成器动画

```
合成器可以独立驱动某些动画，不需要主线程参与：

合成器可处理的属性（Compositor-accelerated）：
├── transform（translate / rotate / scale）
├── opacity
├── filter（部分）
└── clip-path（部分）

这些动画：
├── 在合成线程执行
├── 不触发 Style / Layout / Paint
├── 不阻塞主线程
├── 即使 JS 执行很久也保持 60fps
└── 通过更新 Property Tree 中对应的属性实现

// ✅ 合成器动画（流畅）
.animated {
    transition: transform 0.3s;
    transform: translateX(100px);
}

// ❌ 主线程动画（可能卡顿）
.animated {
    transition: left 0.3s;
    left: 100px;  // 触发 Layout → Paint → 全在主线程
}
```

---

## 6. 滚动

### 6.1 合成器滚动 vs 主线程滚动

```
合成器滚动（Compositor Scroll）：
  ✅ 默认情况下，滚动由合成线程处理
  ✅ 直接修改 Scroll Tree 的偏移量
  ✅ 不需要主线程参与
  ✅ 极其流畅

主线程滚动（被迫回退）：
  ❌ 当页面注册了 scroll/wheel/touch 事件监听器
  ❌ 且未标记为 passive
  ❌ 合成线程必须先询问主线程：要不要 preventDefault？
  ❌ 等待主线程回应 → 引入延迟 → 滚动卡顿

// ❌ 阻塞合成器滚动
document.addEventListener('wheel', handler);
// 合成线程不知道 handler 会不会 preventDefault
// 必须等主线程执行 handler 后才能滚动

// ✅ passive 事件监听器
document.addEventListener('wheel', handler, { passive: true });
// 告诉浏览器：handler 不会调用 preventDefault
// 合成线程可以立即开始滚动，同时通知主线程执行 handler
```

### 6.2 滚动性能优化

```
1. 使用 passive 事件监听器（Chrome 默认对 touchstart/touchmove 使用 passive）
2. 避免在滚动事件中做重活（使用 requestAnimationFrame 节流）
3. 使用 CSS scroll-snap 替代 JS 滚动控制
4. 使用 content-visibility: auto 跳过不可见区域的渲染
5. 避免对滚动容器使用 will-change（已经有独立滚动层）
```

---

## 7. Viz (Display Compositor)

```
Viz 是 Chromium 的显示合成器，运行在 GPU 进程（或独立 Viz 进程）中

职责：
├── 接收所有进程（多个 Renderer + Browser UI）的 CompositorFrame
├── 将多个 CompositorFrame 合成为最终画面
├── 处理帧同步（VSync 对齐）
├── 输出到窗口系统（Swap Buffers）
└── 处理 Surface 管理

Surface 机制：
  每个提交 CompositorFrame 的源都有一个 Surface
  Viz 将所有 Surface 聚合（Aggregation）为最终帧

  Browser UI Surface ─┐
  Renderer A Surface  ─┤→ Viz Aggregation → 最终帧 → 屏幕
  Renderer B Surface  ─┘
```

---

## 8. 面试常见问题

**Q1：什么是合成 (Compositing)？它为什么重要？**

> 合成是将页面的多个图层按正确顺序组合成最终画面的过程。它在独立于主线程的合成线程中执行，这意味着即使 JS 执行耗时很长，滚动和 CSS transform/opacity 动画依然流畅。这是现代浏览器保证 60fps 的核心机制。

**Q2：如何查看页面有多少个合成层？**

> Chrome DevTools → More tools → Layers 面板。或者 Rendering tab → Layer borders。可以看到每个合成层的大小、提升原因和内存占用。

**Q3：will-change 的工作原理？什么时候用？什么时候不用？**

> `will-change` 提示浏览器该元素即将变化，浏览器提前创建合成层并分配 GPU 纹理。**用**：元素即将执行 transform/opacity 动画时。**不用**：静态元素、大量元素、整个页面。用完后应移除（在动画结束的 transitionend 事件中）。

**Q4：为什么合成层过多反而降低性能？**

> 每个合成层需要独立的 GPU 纹理（宽×高×4字节）。100 个 500×500 的合成层 = 100MB GPU 内存。此外，纹理上传有带宽开销，Viz 合成多个 Surface 也有 CPU 开销。通过浏览器的 Layer Squashing 机制可以合并不必要的图层。

**Q5：合成线程和主线程如何配合？**

> 主线程完成 Style → Layout → Paint 后，通过 Commit 操作将绘制数据提交给合成线程。提交后主线程可以继续执行 JS。合成线程独立进行 Tiling → Raster → Draw → Display。双缓冲树（Pending/Active Tree）确保两个线程不互相阻塞。

**Q6：什么是 jank？什么原因导致 jank？**

> Jank 是用户可感知的卡顿（丢帧）。原因：主线程工作时间超过一帧预算（16.67ms），常见原因包括：长时间 JS 执行、强制同步布局、大量 DOM 操作、GC 停顿、合成层爆炸。排查工具：DevTools Performance 面板、chrome://tracing。
