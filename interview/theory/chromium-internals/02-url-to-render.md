# 02 - 从 URL 输入到页面展示 (URL to Render)

## 简介

"从输入 URL 到页面显示经历了什么？" 是浏览器内核方向最经典的面试题。本文以 Chromium 为例，完整梳理每个阶段发生的事情，从用户按下回车到像素上屏。

---

## 全流程概览

```
用户输入 URL 并回车
    │
    ▼
┌─ 阶段 1：导航准备 ──────────────────────────────────────────┐
│  ① 输入处理 → ② 查找缓存 → ③ DNS 解析 → ④ 建立连接       │
└──────────────────────────────────────────────────────────────┘
    │
    ▼
┌─ 阶段 2：请求与响应 ────────────────────────────────────────┐
│  ⑤ 发送 HTTP 请求 → ⑥ 接收响应 → ⑦ 导航提交                │
└──────────────────────────────────────────────────────────────┘
    │
    ▼
┌─ 阶段 3：解析与构建 ────────────────────────────────────────┐
│  ⑧ HTML 解析 → ⑨ 子资源加载 → ⑩ DOM 树 + CSSOM 构建        │
└──────────────────────────────────────────────────────────────┘
    │
    ▼
┌─ 阶段 4：渲染流水线 ────────────────────────────────────────┐
│  ⑪ 样式计算 → ⑫ 布局 → ⑬ 分层 → ⑭ 绘制 → ⑮ 合成 → ⑯ 显示 │
└──────────────────────────────────────────────────────────────┘
```

---

## 阶段 1：导航准备

### ① 输入处理（Browser 进程 - UI 线程）

```
用户在地址栏输入内容，按下回车

Browser 进程判断输入内容：
├── 是合法 URL（如 https://example.com）→ 直接导航
├── 不是 URL（如 "chromium architecture"）→ 组合成搜索引擎 URL
└── 是域名但无协议（如 example.com）→ 补全为 https://example.com

同时触发：
├── beforeunload 事件（如果当前页面有监听）
├── 标签页 loading 动画开始
└── 通知 Network Thread 开始处理
```

### ② 查找缓存（Browser 进程 - Network Service）

```
请求发出前，先查找缓存：

Service Worker 缓存（如果已注册）
    │ 未命中
    ▼
HTTP 缓存（磁盘/内存缓存）
    │
    ├── 强缓存命中（Cache-Control / Expires 未过期）
    │   → 直接使用缓存，不发请求，返回 200 (from cache)
    │
    ├── 协商缓存（Cache-Control 过期，但有 ETag / Last-Modified）
    │   → 发送条件请求（If-None-Match / If-Modified-Since）
    │   → 服务端返回 304 Not Modified → 使用缓存
    │   → 服务端返回 200 → 使用新数据
    │
    └── 无缓存
        → 发起完整网络请求
```

### ③ DNS 解析

```
URL: https://www.example.com/path

需要将域名 www.example.com 解析为 IP 地址

查找顺序：
1. 浏览器 DNS 缓存（约 1 分钟有效）
2. 操作系统 DNS 缓存
3. hosts 文件
4. 本地 DNS 服务器（路由器/ISP）
5. 递归查询
   根域名服务器 → .com 服务器 → example.com 权威服务器
   → 返回 93.184.216.34

优化：
├── DNS 预解析（<link rel="dns-prefetch">）
├── Happy Eyeballs 算法（IPv4/IPv6 同时尝试）
└── DNS over HTTPS (DoH)
```

### ④ 建立连接

```
TCP 三次握手：
  Client → SYN → Server
  Client ← SYN+ACK ← Server
  Client → ACK → Server

如果是 HTTPS（绝大多数情况）：
  TLS 握手（在 TCP 之上）：
  ├── Client Hello（支持的加密套件、随机数）
  ├── Server Hello（选择的套件、证书、随机数）
  ├── 客户端验证证书（CA 链验证、OCSP 检查）
  ├── 密钥交换（ECDHE）
  └── 对称加密通信开始

  TLS 1.3 优化：1-RTT 握手（vs TLS 1.2 的 2-RTT）
  已有会话：0-RTT 恢复（Session Resumption）

时间开销：
  DNS:  ~20-120ms
  TCP:  ~1 RTT（~20-100ms）
  TLS:  ~1-2 RTT（~20-200ms）
  总计: ~60-400ms（首次连接）
```

---

## 阶段 2：请求与响应

### ⑤ 发送 HTTP 请求（Network Service）

```
GET /path HTTP/2
Host: www.example.com
Accept: text/html,application/xhtml+xml,...
Accept-Encoding: gzip, br
Cookie: session=abc123
User-Agent: Chrome/120.0 ...

关键请求头：
├── Accept-Encoding: gzip, br  → 服务端压缩响应
├── If-None-Match / If-Modified-Since → 协商缓存
├── Connection: keep-alive → 连接复用
└── HTTP/2 多路复用 → 一个连接并发多个请求
```

### ⑥ 接收响应（Network Service）

```
HTTP/2 200 OK
Content-Type: text/html; charset=utf-8
Content-Encoding: br
Cache-Control: max-age=3600
Content-Security-Policy: default-src 'self'
...

<html>...

Browser 进程收到响应后的处理：
1. 检查 Content-Type
   ├── text/html → 继续渲染流程
   ├── application/pdf → 调用 PDF 插件
   └── application/octet-stream → 下载
2. 安全检查
   ├── Safe Browsing（是否恶意网站）
   ├── CORB（Cross-Origin Read Blocking）
   └── CSP（Content Security Policy）
3. 响应数据开始流式传输给 Renderer 进程
```

### ⑦ 导航提交（Browser → Renderer）

```
Browser 进程确认安全后：
1. 选择/创建目标 Renderer 进程
   ├── 同站点 → 复用已有进程
   └── 跨站点 → 创建新进程
2. 通过 Mojo IPC 发送 CommitNavigation 消息
3. Renderer 确认接收，发送 DidCommitNavigation 回执
4. Browser 更新 UI（地址栏 URL、安全锁图标、前进/后退记录）
5. 文档的 loading 状态开始
6. 旧页面的 Renderer 进程可以被卸载（unload 事件）
```

---

## 阶段 3：解析与构建

### ⑧ HTML 解析（Renderer 进程 - 主线程）

```
HTML 字节流 → 字符编码解码 → Token 化 → DOM 树构建

字节：3C 68 74 6D 6C 3E ...
  │ 解码（UTF-8）
  ▼
字符：<html><head>...</head><body>...</body></html>
  │ 词法分析（Tokenizer）
  ▼
Token：[StartTag:html] [StartTag:head] ... [EndTag:html]
  │ 语法分析（TreeBuilder）
  ▼
DOM 树：
  Document
  └── html
      ├── head
      │   ├── meta
      │   ├── title
      │   └── link (CSS)
      └── body
          ├── div
          │   └── p
          │       └── "Hello"
          └── script

关键特性：
├── 流式解析：不需要等整个 HTML 下载完成就开始解析
├── 容错：HTML 解析器非常宽容，能处理各种不规范的 HTML
└── 增量构建：边接收边解析边构建 DOM
```

### ⑨ 子资源加载

```
解析过程中遇到外部资源：

<link rel="stylesheet" href="style.css">
  → 异步加载，不阻塞 HTML 解析
  → 但阻塞渲染（CSSOM 未完成不能渲染）

<script src="app.js"></script>
  → ⚠️ 阻塞 HTML 解析！
  → 浏览器必须等 JS 下载+执行完才能继续解析
  → 因为 JS 可能修改 DOM（document.write）

<script src="app.js" async></script>
  → 异步下载，下载完立即执行
  → 不阻塞 HTML 解析
  → 执行时可能 DOM 尚未完成

<script src="app.js" defer></script>
  → 异步下载，HTML 解析完后、DOMContentLoaded 前执行
  → 不阻塞 HTML 解析
  → 保证执行顺序

<img src="photo.jpg">
  → 异步加载，不阻塞解析和渲染

资源加载优化：
├── 预加载扫描器（Preload Scanner）
│   主线程被 JS 阻塞时，预扫描后续 HTML 发现资源并提前加载
├── <link rel="preload"> → 优先加载关键资源
├── <link rel="prefetch"> → 空闲时加载下一页资源
└── HTTP/2 Server Push → 服务端主动推送资源
```

### ⑩ DOM 树 + CSSOM 构建

```
DOM 树（来自 HTML 解析）：

  Document
  └── html
      ├── head
      └── body
          └── div.container
              └── p#text → "Hello World"

CSSOM 树（来自 CSS 解析）：

  StyleSheetList
  ├── 浏览器默认样式（User Agent Stylesheet）
  ├── 外部样式表（style.css）
  └── 内联样式（<style> 标签）

CSS 解析特点：
├── 规则匹配：选择器从右到左匹配（.container p → 先找所有 p，再看父元素是否有 .container）
├── 层叠计算：权重、特指度、顺序
└── 继承：某些属性（color、font 等）自动继承给子元素
```

---

## 阶段 4：渲染流水线

### ⑪ 样式计算 (Style Calculation)

```
输入：DOM 树 + CSSOM
输出：每个 DOM 节点的 ComputedStyle（最终样式值）

过程：
1. 收集所有匹配的 CSS 规则
2. 按优先级排序（!important > 内联 > ID > class > 标签）
3. 解析相对值（如 em → px, % → px, red → rgb(255,0,0)）
4. 处理继承和默认值

结果：每个可见节点都有确定的样式值
  div.container → { display: flex, width: 800px, color: #333, ... }
  p#text → { font-size: 16px, line-height: 1.5, color: #333（继承）, ... }
```

### ⑫ 布局 (Layout)

```
输入：带 ComputedStyle 的 DOM 树
输出：布局树（LayoutTree）— 每个节点的几何信息（位置和大小）

过程：
1. 构建布局树（不可见元素如 display:none 不参与）
2. 计算每个节点的盒模型
   ├── content → padding → border → margin
   └── width / height 计算
3. 处理不同布局模式
   ├── 普通流（Block / Inline）
   ├── Flex 布局
   ├── Grid 布局
   └── 绝对定位 / 固定定位
4. 输出每个节点的坐标和尺寸

布局树 vs DOM 树：
├── display:none 的节点不在布局树中
├── ::before / ::after 伪元素在布局树中
├── 匿名盒（Anonymous Box）可能被创建
└── 一个 DOM 节点可能对应多个布局对象
```

### ⑬ 分层 (Layer)

```
输入：布局树
输出：图层树（LayerTree）

为什么需要分层？
  → 某些元素需要独立绘制和合成（如固定定位、动画、滚动区域）
  → 分层后只需重绘/重排变化的图层，不影响其他图层

触发独立图层的条件：
├── position: fixed / sticky
├── transform / opacity 动画
├── will-change: transform
├── overflow: scroll（滚动区域）
├── <video> / <canvas> / <iframe>
├── z-index 不为 auto 的定位元素
└── CSS filter / backdrop-filter

注意：图层不是越多越好！
├── 每个图层占用 GPU 内存
├── 过多图层反而降低性能
└── 浏览器会自动合并不必要的图层
```

### ⑭ 绘制 (Paint)

```
输入：图层树
输出：每个图层的绘制指令列表（Paint Record / Display List）

过程：
  遍历图层中的节点，生成绘制指令
  指令类似：
  ├── DrawRect(x:0, y:0, w:800, h:600, color:#fff)  // 背景
  ├── DrawText("Hello", x:10, y:20, font:16px Arial)  // 文字
  ├── DrawBorder(x:0, y:0, w:800, h:600, 1px solid #ccc)  // 边框
  └── DrawImage(url, x:100, y:50, w:200, h:150)  // 图片

绘制顺序（严格按照层叠上下文）：
  1. 背景和边框
  2. 负 z-index 子元素
  3. 普通流块级元素
  4. 浮动元素
  5. 普通流行内元素
  6. z-index: 0 / auto
  7. 正 z-index 子元素

注意：绘制不是真的画像素！
  这一步只生成"指令"，真正的像素化在光栅化阶段
```

### ⑮ 合成 (Compositing)

```
合成在 合成线程（Compositor Thread）中执行，不阻塞主线程！

步骤：
1. 分块（Tiling）
   将每个图层分成固定大小的 tile（通常 256x256 或 512x512）
   只光栅化视口附近的 tile，节省计算

2. 光栅化（Rasterization）
   在光栅化线程池中执行
   ├── 软件光栅化：CPU 绘制 → 位图
   └── GPU 光栅化：绘制指令发给 GPU → 位图（更快）

3. 合成帧（Draw Quad）
   合成线程将所有图层的 tile 组合成最终画面
   ├── 处理图层的位置、变换、透明度
   ├── 生成 CompositorFrame
   └── 提交给 GPU 进程

4. 显示（Display）
   GPU 进程接收 CompositorFrame
   ├── 执行最终合成（Display Compositor）
   ├── 通过 OpenGL/Vulkan/Metal 绘制到帧缓冲区
   └── 调用 SwapBuffers 显示到屏幕
```

### ⑯ 显示

```
帧缓冲区的内容在下一次 VSync 信号到来时被显示到屏幕

VSync（垂直同步）：
├── 60Hz 显示器 → 每 16.67ms 一次 VSync
├── 120Hz 显示器 → 每 8.33ms 一次 VSync
└── 浏览器的帧调度与 VSync 对齐，避免画面撕裂

至此，用户看到了页面！
```

---

## 关键时间节点与事件

```
时间线：
──────────────────────────────────────────────────────────────→

导航开始
  │
  ├── DNS + TCP + TLS          ~100-500ms
  │
  ├── First Byte (TTFB)        服务端响应第一个字节
  │
  ├── HTML 开始解析
  │
  ├── DOMContentLoaded          DOM 构建完成（不含图片/CSS）
  │   └── document.addEventListener('DOMContentLoaded', ...)
  │
  ├── First Paint (FP)          首次绘制（可能只是背景色）
  │
  ├── First Contentful Paint    首次有内容的绘制（文字/图片）
  │   (FCP)
  │
  ├── Largest Contentful Paint  最大内容元素绘制完成
  │   (LCP)                     （Core Web Vitals 核心指标）
  │
  ├── load 事件                 所有资源（图片等）加载完成
  │   └── window.addEventListener('load', ...)
  │
  └── Time to Interactive       页面可交互
      (TTI)
```

---

## 重排、重绘、合成的触发条件

### 三者的代价比较

```
重排 (Reflow/Layout) > 重绘 (Repaint) > 合成 (Composite)

重排（最昂贵）：
  改变几何属性 → 重新计算布局 → 重新绘制 → 重新合成
  触发属性：width, height, margin, padding, display, position,
           font-size, border, top/left/right/bottom, ...

重绘（中等）：
  改变外观属性 → 跳过布局 → 重新绘制 → 重新合成
  触发属性：color, background, visibility, box-shadow,
           border-color, outline, ...

合成（最便宜）：
  只改变合成属性 → 跳过布局和绘制 → 只重新合成
  触发属性：transform, opacity, will-change
  在合成线程完成，不阻塞主线程！
```

### 优化原则

```
性能优先级：
  ✅ 优先使用只触发合成的属性（transform / opacity）
  ⚠️ 尽量减少触发重绘的操作
  ❌ 避免频繁触发重排

// ❌ 差：每次修改都触发重排
element.style.width = '100px';   // 重排
element.style.height = '200px';  // 重排
element.style.margin = '10px';   // 重排
// 实际上浏览器会批量处理，但读取布局属性会强制刷新

// ❌ 强制同步布局（Forced Synchronous Layout）
element.style.width = '100px';           // 标记需要重排
const height = element.offsetHeight;     // ⚠️ 强制立即重排！
element.style.height = height + 'px';    // 又标记需要重排

// ✅ 好：先读后写，避免反复触发
const height = element.offsetHeight;     // 读
element.style.width = '100px';           // 写（批量）
element.style.height = height + 'px';    // 写（批量）

// ✅ 最佳：使用 transform 代替几何属性
element.style.transform = 'translateX(100px)';  // 只触发合成
```

---

## 完整流程时序图

```
Browser 进程          Network Service        Renderer 进程           GPU 进程
     │                      │                      │                    │
     │──── 导航请求 ────→   │                      │                    │
     │                      │                      │                    │
     │                  DNS 解析                    │                    │
     │                  TCP + TLS                   │                    │
     │                  HTTP 请求                   │                    │
     │                      │                      │                    │
     │  ←── 响应头 ────────│                      │                    │
     │                      │                      │                    │
     │  选择/创建 Renderer  │                      │                    │
     │──── CommitNavigation ──────────────────→    │                    │
     │                      │                      │                    │
     │                      │──── 响应体流式传输 ──→│                    │
     │                      │                      │                    │
     │                      │              HTML 解析 + DOM 构建          │
     │                      │              CSS 解析 + CSSOM              │
     │                      │              JS 执行                       │
     │                      │              样式计算                      │
     │                      │              布局                          │
     │                      │              分层                          │
     │                      │              绘制（生成指令）              │
     │                      │                      │                    │
     │                      │              合成线程：分块 + 光栅化命令    │
     │                      │                      │──── 光栅化指令 ──→│
     │                      │                      │                    │
     │                      │                      │←── tile 位图 ─────│
     │                      │                      │                    │
     │                      │              合成线程：提交 CompositorFrame│
     │                      │                      │──── 合成帧 ──────→│
     │                      │                      │                    │
     │                      │                      │              最终合成
     │                      │                      │              SwapBuffers
     │                      │                      │              屏幕显示 ✨
```

---

## 面试常见问题

**Q1：从输入 URL 到页面展示，完整流程？（经典题）**

> 精简版回答（面试时按深度展开）：
> 1. **URL 解析**：判断输入是 URL 还是搜索词
> 2. **缓存检查**：Service Worker → HTTP 强缓存 → 协商缓存
> 3. **DNS 解析**：域名 → IP 地址（浏览器缓存 → OS 缓存 → DNS 服务器）
> 4. **建立连接**：TCP 三次握手 + TLS 握手（HTTPS）
> 5. **发送请求**：HTTP 请求，等待服务端响应
> 6. **接收响应**：解析响应头，安全检查，确定内容类型
> 7. **HTML 解析**：字节流 → Token → DOM 树（流式、增量）
> 8. **CSS 解析**：构建 CSSOM
> 9. **JavaScript 执行**：遇到 `<script>` 暂停解析（除非 async/defer）
> 10. **样式计算**：DOM + CSSOM → 每个节点的 ComputedStyle
> 11. **布局**：计算每个节点的位置和大小
> 12. **分层**：生成图层树
> 13. **绘制**：生成每个图层的绘制指令
> 14. **光栅化**：将绘制指令转为像素位图（GPU 加速）
> 15. **合成**：将所有图层合成最终画面
> 16. **显示**：GPU 输出到帧缓冲区，VSync 时显示

**Q2：为什么 CSS 放 head、JS 放 body 底部？**

> **CSS 放 head**：CSS 不阻塞 HTML 解析，但阻塞渲染。放 head 可以尽早开始下载和解析 CSS，避免出现无样式内容闪烁（FOUC）。
> **JS 放 body 底部**：`<script>` 默认阻塞 HTML 解析。放底部让 DOM 先构建完成，用户更早看到内容。现代实践：使用 `defer`/`async` 属性替代位置调整。

**Q3：async 和 defer 的区别？**

> - **async**：异步下载，下载完**立即执行**（可能在 DOM 解析完成之前），不保证执行顺序。适合独立脚本（如统计代码）。
> - **defer**：异步下载，在 **DOM 解析完成后、DOMContentLoaded 之前**执行，保证执行顺序。适合依赖 DOM 的脚本。
> - 无属性：同步下载+执行，阻塞 HTML 解析。

**Q4：什么是关键渲染路径 (Critical Rendering Path)？**

> 从收到 HTML 到首次渲染（First Paint）所必须完成的最小工作量：
> HTML → DOM → CSSOM → Render Tree → Layout → Paint
> 优化目标：减少关键资源数量、减小关键资源大小、缩短关键路径长度。

**Q5：DOMContentLoaded 和 load 事件的区别？**

> - **DOMContentLoaded**：HTML 解析完成、DOM 树构建完毕时触发（不等待图片、CSS、子框架）。
> - **load**：所有资源（图片、CSS、iframe 等）全部加载完成后触发。
> - DOMContentLoaded 一定先于 load。

**Q6：什么是强制同步布局 (Forced Synchronous Layout)？如何避免？**

> 当 JS 先修改了影响布局的样式，然后立即读取布局属性（如 offsetHeight），浏览器被迫立即执行布局计算（而不是等到批量处理时），这就是强制同步布局。
> 避免方法：先批量读取布局属性，再批量修改样式。或使用 `requestAnimationFrame` 将修改推迟到下一帧。

**Q7：为什么 transform 动画比 top/left 动画流畅？**

> `top/left` 触发重排（Layout）+ 重绘（Paint）+ 合成，都在主线程执行。
> `transform` 只触发合成（Composite），在合成线程执行，完全不阻塞主线程。
> 即使主线程在执行耗时 JS，transform 动画依然流畅。

**Q8：浏览器一帧内做了什么？（16.67ms 内）**

> ```
> ┌─ 一帧（~16.67ms @ 60fps）─────────────────────┐
> │ 1. 处理输入事件（click, scroll, ...）          │
> │ 2. 执行 requestAnimationFrame 回调             │
> │ 3. 样式计算（Style）                           │
> │ 4. 布局（Layout）                              │
> │ 5. 绘制（Paint）                               │
> │ 6. 合成（Composite）                           │
> │ 7. 空闲时间 → requestIdleCallback              │
> └────────────────────────────────────────────────┘
> ```
> 如果以上工作超过 16.67ms，就会丢帧（jank）。

**Q9：预加载扫描器 (Preload Scanner) 是什么？**

> 当主线程的 HTML 解析器被 `<script>` 阻塞时，预加载扫描器会在后台继续扫描 HTML，提前发现后续的 CSS、JS、图片等资源并开始下载。这避免了"串行下载"的性能损失，是浏览器的关键优化之一。

**Q10：首屏优化的核心策略？**

> 1. **减少关键资源**：内联关键 CSS，defer/async 非关键 JS
> 2. **减小资源体积**：压缩（gzip/br）、Tree Shaking、代码分割
> 3. **加速网络**：CDN、HTTP/2、预连接（preconnect）、预解析（dns-prefetch）
> 4. **优化渲染**：避免强制同步布局、减少 DOM 层级、使用 content-visibility
> 5. **缓存策略**：Service Worker、HTTP 缓存、资源版本化
