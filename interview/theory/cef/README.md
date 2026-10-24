# CEF 框架考察

**Chromium Embedded Framework** 面试专项 — 历史版本、多进程架构、Handler 体系、JS 桥、实战集成与排障。

> 对照阅读：  
> - [frontend-container/](../frontend-container/)（多容器横向对比，含 CEF 专篇）  
> - [chromium-internals/](../chromium-internals/)（Blink/V8/合成器更深）  
> 本目录为 **CEF 嵌入开发** 的完整考察路径，与上两者并行、可交叉引用。

---

## 主题列表 (12 个主题)

### 基础层

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 01 | [01-history-and-versions.md](01-history-and-versions.md) | 历史与版本 | CEF 起源、分支模型、与 Chromium 版本关系 |
| 02 | [02-architecture-multiprocess.md](02-architecture-multiprocess.md) | 多进程架构 | Browser/Renderer/GPU/Utility、沙箱 |
| 03 | [03-bootstrap-lifecycle.md](03-bootstrap-lifecycle.md) | 启动与生命周期 | CefExecuteProcess、初始化、消息循环、关闭 |
| 04 | [04-cefapp-client-handlers.md](04-cefapp-client-handlers.md) | App/Client/Handler | 扩展点职责划分 |

### 进阶层

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 05 | [05-browser-window-integration.md](05-browser-window-integration.md) | 窗口集成 | 子窗口托管、焦点、DPI、原生父窗 |
| 06 | [06-js-native-bridge.md](06-js-native-bridge.md) | JS ↔ C++ | V8 Handler、IPC、异步回调 |
| 07 | [07-ipc-messaging.md](07-ipc-messaging.md) | 进程通信 | ProcessMessage、CefFrame、线程模型 |
| 08 | [08-offscreen-rendering.md](08-offscreen-rendering.md) | 离屏渲染 | OSR、脏区、输入注入 |

### 深度层

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 09 | [09-performance-memory.md](09-performance-memory.md) | 性能与内存 | 多进程占用、缓存、页面生命周期 |
| 10 | [10-security-sandbox.md](10-security-sandbox.md) | 安全与沙箱 | 权限、CSP、同源、危险 API |
| 11 | [11-theory-interview-qa.md](11-theory-interview-qa.md) | 理论考察题 | 高频概念与追问 |
| 12 | [12-practice-scenarios.md](12-practice-scenarios.md) | 实战场景考察 | 嵌入、白屏、崩溃、升级 Chromium |

---

## 学习路径

1. **01 → 04**：版本定位 + 多进程 + 启动 + Handler 地图  
2. **05 → 08**：嵌窗、JS 桥、IPC、OSR  
3. **09 → 12**：性能安全 + 理论/实战刷题  

---

## 面试高频考点

- CEF 与 Electron / WebView2 / Qt WebEngine 对比
- 为何需要 `CefExecuteProcess` 与多进程
- CefApp / CefClient / 各 Handler 职责
- Browser 线程模型（UI / IO / FILE 等）
- JS 调用原生与原生调 JS 的安全边界
- 正常关闭顺序（防僵尸进程）
- OSR 适用场景与代价
- 版本跟随 Chromium 的升级策略

---

## 文件列表

```
cef/
├── README.md
├── 01-history-and-versions.md
├── 02-architecture-multiprocess.md
├── 03-bootstrap-lifecycle.md
├── 04-cefapp-client-handlers.md
├── 05-browser-window-integration.md
├── 06-js-native-bridge.md
├── 07-ipc-messaging.md
├── 08-offscreen-rendering.md
├── 09-performance-memory.md
├── 10-security-sandbox.md
├── 11-theory-interview-qa.md
└── 12-practice-scenarios.md
```
