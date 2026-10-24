# 02 - CEF 多进程架构

## 核心概念

CEF3 遵循 Chromium：**一个 Browser 进程 + 多个 Renderer + GPU/Utility 等**。崩溃隔离、安全沙箱、性能并行是多进程的理由。

```
原生宿主应用
     │ 链接 libcef / 启动
     ▼
Browser Process  ←── 你的 CefClient / 大部分原生回调默认在此
     │
     ├── Renderer Process(es)  ← Blink + V8，页面沙箱内
     ├── GPU Process
     └── Utility / 其他
```

同一份可执行文件常通过 **命令行参数** 再进入子进程角色（见下一篇 `CefExecuteProcess`）。

---

## 进程职责

| 进程 | 职责 |
|------|------|
| Browser | 窗口管理、网络、存储、与宿主交互、多数 CEF 回调线程 |
| Renderer | DOM、样式、布局、JS；崩溃不杀宿主 |
| GPU | 合成与加速绘制 |
| Utility | 音频、网络服务等拆分（随 Chromium 演变） |

### 站点隔离（Site Isolation）

不同站点可进不同 Renderer，降低跨站攻击面；结果是 **进程数↑、内存↑**——面试要能权衡。

---

## 线程模型（Browser 进程内）

CEF 强调线程亲和，常见名字：

| 线程 | 典型工作 |
|------|----------|
| UI | 浏览器 UI、大量 Client 回调 |
| IO | IPC、网络相关 |
| FILE / 其它 | 文件与后台 |

**规则**：在错误线程碰 CEF API 会断言或难查 bug；用 `CefPostTask` 投递到目标线程。

---

## 沙箱

Renderer 低权限跑；Windows 上沙箱依赖策略与启动参数。调试时可关沙箱，**生产务必理解风险**。

---

## 与宿主关系

- 宿主负责：原生窗口、业务逻辑、启动关闭、打包 CEF 资源  
- CEF 负责：网页生命周期、渲染、安全子系统  
- 边界：JS 桥与文件/网络权限是安全审查重点  

---

## 常见面试题

**Q1：为什么嵌入浏览器还要多进程？**  
答：崩溃隔离、沙箱安全、并行；对齐 Chrome 架构。

**Q2：页面 JS 崩了宿主会怎样？**  
答：通常仅 Renderer 挂；Browser/宿主可重建浏览器实例。

**Q3：CEF 回调默认在哪个进程？**  
答：多数在 Browser 进程；Renderer 侧有专门 Handler（如部分 V8）。需分清接口注释。

---

## 实战要点

- 任务管理器看多进程是否正常；僵尸子进程常是关闭顺序错误。  
- 内存投诉先分清「多标签多进程」是否预期。  

> 更深： [chromium-internals/01-architecture.md](../chromium-internals/01-architecture.md)

---

## 下一步

- [03-bootstrap-lifecycle.md](03-bootstrap-lifecycle.md)
