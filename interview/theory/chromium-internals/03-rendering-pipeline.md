# 03 - Blink 渲染流水线 (Rendering Pipeline)

## 简介

Blink 是 Chromium 的渲染引擎（从 WebKit 分叉），负责将 HTML/CSS/JS 转化为用户看到的画面。本文深入解析渲染流水线的每个阶段。

---

## 渲染流水线全景

```
      主线程（Main Thread）                     合成线程（Compositor Thread）
┌────────────────────────────────────┐    ┌────────────────────────────┐
│                                    │    │                            │
│  Parse → Style → Layout → Paint   │───→│  Commit → Tiling → Raster │──→ GPU
│                                    │    │       → Draw → Display    │
└────────────────────────────────────┘    └────────────────────────────┘

详细阶段：
1. Parse HTML      → DOM Tree
2. Parse CSS       → CSSOM / StyleSheets
3. Style           → ComputedStyle（每个节点的最终样式）
4. Layout          → LayoutTree（几何信息：位置 + 大小）
5. Pre-paint       → Property Trees（变换、裁剪、效果、滚动）
6. Paint           → Paint Records / Display List（绘制指令）
7. Commit          → 主线程数据提交给合成线程
8. Tiling          → 图层切分为 tile
9. Raster          → tile 光栅化为位图
10. Activate       → pending tree → active tree
11. Draw           → 生成 CompositorFrame
12. Display        → Viz 进程合成输出到屏幕
```

---

## 1. DOM 树构建 (Parse HTML)

### 1.1 HTML 解析器架构

```
网络数据流
    │
    ▼
字节流解码器（根据 Content-Type charset 或 BOM 或 meta 标签）
    │
    ▼
输入流预处理（归一化换行符）
    │
    ▼
Tokenizer（词法分析器）
    │  生成 Token：StartTag, EndTag, Character, Comment, DOCTYPE, EOF
    ▼
TreeBuilder（树构建器）
    │  根据 Token 构建 DOM 节点，插入到正确位置
    ▼
DOM Tree
```

### 1.2 解析的阻塞与优化

```
遇到 <script>（无 async/defer）：
  1. 暂停 HTML 解析
  2. 如果有未完成的 CSS → 等待 CSS 解析完成（JS 可能查询样式）
  3. 下载 JS（如果是外部脚本）
  4. 执行 JS
  5. 恢复 HTML 解析

为什么 JS 阻塞解析？
  → JS 可以调用 document.write() 修改 HTML 流
  → JS 可以操作已构建的 DOM
  → 解析器必须等 JS 执行完才能确定后续 DOM 结构

预加载扫描器（Preload Scanner）：
  独立于主解析器，在主解析器被阻塞时继续扫描 HTML
  提前发现 <link>, <script>, <img> 等资源并触发下载
  大幅减少网络空闲时间
```

---

## 2. 样式计算 (Style)

### 2.1 CSS 解析

```
CSS 来源（优先级从低到高）：
1. User Agent Stylesheet（浏览器默认样式）
2. 用户样式（极少使用）
3. 作者样式
   ├── 外部样式表 <link>
   ├── <style> 内部样式
   └── style="" 内联样式
4. !important 声明（反转优先级）
```

### 2.2 选择器匹配

```
CSS 选择器从右向左匹配：

规则：div.container > ul li a.link

匹配过程：
1. 找所有 class="link" 的 <a> 元素
2. 向上检查：父元素是否是 <li>？
3. 继续向上：祖先中是否有 <ul>？
4. <ul> 的直接父元素是否是 div.container？

为什么从右到左？
  → 大多数规则很快就能排除不匹配的节点
  → 从左到右可能遍历很多节点后才发现不匹配

选择器性能（从快到慢）：
  ID (#id)          → 哈希查找，O(1)
  Class (.class)    → 哈希查找，O(1)
  Tag (div)         → 哈希查找，O(1)
  相邻兄弟 (A + B)  → 需要遍历
  子选择器 (A > B)  → 需要遍历
  后代选择器 (A B)  → 最慢，需要递归向上
  通配符 (*)        → 匹配所有
```

### 2.3 样式失效与优化

```
Style Invalidation（样式失效）：

当 DOM 变化时，不需要重新计算所有节点的样式
Blink 使用精确的失效策略：

1. 直接失效：修改元素自身的 class/style → 只重算该元素
2. 后代失效：修改的规则可能影响后代 → 标记后代重算
3. 兄弟失效：:nth-child 等选择器 → 可能影响兄弟节点

优化措施：
├── StyleInvalidation（精确追踪哪些节点需要重算）
├── Style 共享（相同样式的兄弟节点共享 ComputedStyle）
└── 增量更新（只重算失效的节点，不全量重算）
```

---

## 3. 布局 (Layout)

### 3.1 布局树 (LayoutTree)

```
DOM 树    →    布局树（不是 1:1 对应）

区别：
├── display:none 的节点 → 不在布局树中
├── ::before/::after 伪元素 → 在布局树中（DOM 树没有）
├── 匿名盒 → 布局树自动创建
│   例：<div><span>A</span> B <span>C</span></div>
│   "B" 文本需要一个匿名 inline box
└── 一个 DOM 节点 → 可能多个布局对象（多列布局、首字母等）
```

### 3.2 布局算法

```
Blink 的布局引擎（LayoutNG，Next Generation）：

特点：
├── 不可变的输入/输出（函数式设计）
├── Fragment Tree（片段树，替代旧的 LayoutObject 树）
├── 更好的增量布局支持
└── 统一的布局约束系统

支持的布局模式：
├── Block Layout（块级布局）
├── Inline Layout（行内布局）
├── Flex Layout（弹性布局）
├── Grid Layout（网格布局）
├── Table Layout（表格布局）
├── Float Layout（浮动布局）
└── Multi-column Layout（多列布局）

布局计算核心：
  输入：节点 + 约束（可用空间、方向等）
  输出：Fragment（位置、大小、基线等）

  // 伪代码
  LayoutResult Layout(Node, Constraints) {
      if (node.isBlock()) return BlockLayout(node, constraints);
      if (node.isFlex())  return FlexLayout(node, constraints);
      if (node.isGrid())  return GridLayout(node, constraints);
      // ...
  }
```

### 3.3 布局的性能问题

```
布局是渲染流水线中最昂贵的阶段之一

影响布局范围的因素：
├── 全局布局：视口大小变化、字体变化 → 整棵树重新布局
├── 局部布局：单个元素变化 → 只影响部分子树
└── 增量布局：标记脏节点，只重算脏子树

⚠️ 布局抖动（Layout Thrashing）：
  连续读写交替导致反复强制布局

  // ❌ 布局抖动
  for (let i = 0; i < items.length; i++) {
      const h = items[i].offsetHeight;     // 读 → 强制布局
      items[i].style.height = h * 2 + 'px'; // 写 → 标记脏
  }
  // 每次循环都触发一次布局！

  // ✅ 读写分离
  const heights = items.map(item => item.offsetHeight);  // 批量读
  items.forEach((item, i) => {
      item.style.height = heights[i] * 2 + 'px';         // 批量写
  });
  // 只触发一次布局
```

---

## 4. Pre-paint 与 Property Trees

### 4.1 Property Trees（属性树）

```
Chromium 使用四棵属性树替代旧的图层树：

1. Transform Tree（变换树）
   记录每个节点的 CSS transform

2. Clip Tree（裁剪树）
   记录 overflow: hidden, clip-path 等裁剪信息

3. Effect Tree（效果树）
   记录 opacity, filter, blend-mode 等视觉效果

4. Scroll Tree（滚动树）
   记录滚动偏移量

为什么用四棵独立的树？
├── 变换变化时不需要重新计算裁剪
├── 透明度变化时不需要重新计算变换
├── 各属性独立更新，提高效率
└── 合成线程可以直接修改属性树（如滚动偏移），不需要主线程参与
```

---

## 5. 绘制 (Paint)

### 5.1 绘制记录

```
Paint 阶段遍历布局树，生成绘制指令列表（Display List）

绘制指令示例：
┌─ DisplayItem List ──────────────────────────┐
│ 1. DrawRect(0, 0, 800, 600, white)         │  // body 背景
│ 2. DrawRect(10, 10, 200, 100, #f0f0f0)     │  // div 背景
│ 3. DrawBorder(10, 10, 200, 100, 1px #ccc)  │  // div 边框
│ 4. DrawText("Hello", 20, 30, 16px, black)  │  // 文字
│ 5. DrawImage(img, 20, 60, 100, 80)         │  // 图片
└─────────────────────────────────────────────┘

绘制顺序严格按照 CSS 层叠上下文（Stacking Context）：
  1. 根元素的背景和边框
  2. 负 z-index 的子层叠上下文
  3. 普通流中的块级后代
  4. 浮动元素
  5. 普通流中的行内后代
  6. z-index: auto / 0 的定位元素
  7. 正 z-index 的子层叠上下文
```

### 5.2 绘制失效

```
当元素样式变化时，判断是否需要重新绘制：

只触发合成（最快）：
  transform, opacity
  → 不需要重新绘制，合成线程直接处理

触发重绘（中等）：
  color, background, box-shadow, border-color, visibility
  → 重新生成绘制指令，但不需要重新布局

触发重排+重绘（最慢）：
  width, height, margin, padding, display, position
  → 重新布局 → 重新绘制 → 重新合成
```

---

## 6. 合成 (Compositing)

### 6.1 图层与合成原因

```
并非每个元素都有独立图层，图层提升条件：

显式提升：
├── will-change: transform / opacity / ...
├── transform: translateZ(0) / translate3d(...)
├── <video>, <canvas>, <iframe>
└── position: fixed / sticky

隐式提升（Squashing / Overlap）：
├── 元素与已提升的图层重叠 → 被迫提升
└── 避免绘制顺序错误
```

### 6.2 合成线程工作流

```
                  主线程                        合成线程
                    │                              │
            Paint Records                          │
                    │                              │
                    │──── Commit ──────────────→    │
                    │                          创建 pending tree
                    │                              │
                    │                          Tiling（分块）
                    │                              │
                    │                     Raster（光栅化 → GPU 线程/进程）
                    │                              │
                    │                          Activate
                    │                     pending tree → active tree
                    │                              │
                    │                          Draw
                    │                     生成 CompositorFrame
                    │                              │
                    │                              │──→ GPU 进程 → 屏幕
```

### 6.3 光栅化策略

```
光栅化将绘制指令转化为像素位图

策略：
1. 软件光栅化（CPU）
   适用于低端设备或简单内容
   由光栅化线程池在 Renderer 进程内完成

2. GPU 光栅化（OOP-R: Out-of-Process Rasterization）
   绘制指令发送到 GPU 进程
   GPU 直接执行绘制指令 → 像素
   减少 CPU-GPU 数据传输

3. 零拷贝光栅化（Zero-copy raster）
   GPU 光栅化结果直接写入共享内存/GPU 纹理
   合成时无需额外拷贝

优先级：
  视口内的 tile → 高优先级
  即将进入视口的 tile → 中优先级
  远离视口的 tile → 低优先级或不光栅化
```

---

## 7. 帧调度 (Frame Scheduling)

```
浏览器的渲染与显示器 VSync 信号同步

                 VSync           VSync           VSync
                  │               │               │
  ┌───── 帧 1 ───┤┌───── 帧 2 ──┤┌───── 帧 3 ──┤
  │               ││              ││              │
  │  JS + Style   ││  JS + Style  ││              │
  │  Layout       ││  Layout      ││              │
  │  Paint        ││  Paint       ││              │
  │  Commit       ││              ││              │
  │  Raster       ││  Commit      ││              │
  │  Draw         ││  Raster      ││              │
  └───────────────┘│  Draw        ││              │
                   └──────────────┘│              │
                                   └──────────────┘

如果某帧工作量超过 16.67ms → 丢帧（jank）

帧调度器（Scheduler）的策略：
├── 输入优先：用户交互事件优先处理
├── 合成优先：合成线程不等主线程
├── 空闲回调：requestIdleCallback 在空闲时间执行
└── 自适应：根据负载调整策略
```

---

## 8. 面试常见问题

**Q1：Blink 渲染流水线有哪些阶段？**

> Parse → Style → Layout → Pre-paint → Paint → Commit → Tiling → Raster → Activate → Draw → Display。前 6 步在主线程，后面在合成线程/GPU。

**Q2：什么是层叠上下文 (Stacking Context)？如何创建？**

> 层叠上下文决定了元素在 z 轴上的堆叠顺序。以下条件创建新的层叠上下文：根元素、position 非 static 且 z-index 非 auto、opacity < 1、transform 非 none、filter 非 none、will-change 等。层叠上下文内部按固定顺序绘制（背景→负 z-index→块级→浮动→行内→z-index:0→正 z-index）。

**Q3：为什么 will-change 不应该滥用？**

> `will-change` 提示浏览器提前创建合成层和分配 GPU 资源。每个合成层消耗 GPU 内存（一个 1000x1000 的层 = ~4MB）。过多合成层反而导致内存爆炸和合成开销增大。应只在动画元素上使用，动画结束后移除。

**Q4：CSS 选择器为什么从右往左匹配？**

> 从右往左可以快速排除不匹配的元素。例如 `.container p`，从右开始先看是否是 `p` 标签（大部分元素不是），不是就立即跳过。从左开始则需要先找到所有 `.container`，再遍历其所有后代查找 `p`，效率低很多。

**Q5：LayoutNG 相比旧布局引擎的优势？**

> LayoutNG 采用不可变输入输出的函数式设计，布局结果以 Fragment 树表示。优势：更好的增量布局（输入不变则输出可缓存）、统一的约束系统（Block/Flex/Grid 共用框架）、更少的布局 bug（旧引擎有大量历史遗留问题）。

**Q6：什么是合成层爆炸 (Layer Explosion)？如何避免？**

> 当大量元素被隐式提升为合成层（如与动画元素重叠），导致几十甚至上百个合成层，每个都占用 GPU 内存。避免方法：用 DevTools Layers 面板检查图层数量、给动画元素设置较高的 z-index 避免隐式提升、减少不必要的 will-change。
