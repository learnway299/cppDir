# 01 - CEF 历史与主流版本

## 核心概念

**CEF（Chromium Embedded Framework）** 用 C/C++ API 把 **Chromium** 嵌进原生应用，让桌面程序内嵌现代 Web 渲染、JS 与网络栈，而不必自己维护整套浏览器。

一句话：**给 C++ 客户端用的「可嵌入 Chromium」框架。**

---

## 发展脉络（简述）

| 阶段 | 要点 |
|------|------|
| 2008–2009 前后 | Marshall Greenblatt 发起，目标是稳定、易用的 Chromium 嵌入层 |
| CEF1 | 早期单进程倾向方案；已历史淘汰，面试提及即可 |
| **CEF3** | 现行主流：对齐 Chromium **多进程** 模型，长期演进 |
| 持续至今 | 几乎跟随 Chromium 发行列车；广泛用于游戏启动器、IM、管理端、行业客户端 |
| 生态关系 | Electron 等也用 Chromium，但是 **Node+JS 应用壳**；CEF 是 **原生宿主嵌入** |

---

## 版本如何理解

CEF **没有**像「只有 3.0/3.2」那种慢节奏语义化大版本给业务用；日常说的是：

| 概念 | 含义 |
|------|------|
| CEF 主线 | CEF3 API 形态 |
| Chromium 版本号 | 如 120、128… 决定 Blink/V8/网络特性与安全补丁 |
| Branch / Build | 官方构建分支；下载对应平台二进制 |
| API 稳定性 | 相对 Chromium 内部，CEF 尽量提供更稳的嵌入 API，但仍随大版本有破坏性变更 |

**面试口径**：谈特性时同时说清 **CEF API 层** 与 **所捆绑的 Chromium 大版本**。

### 「历史主流」特性演进（按能力，不背日期）

| 能力方向 | 说明 |
|----------|------|
| 多进程 + 沙箱 | CEF3 与 Chrome 安全模型对齐 |
| 窗口渲染 / OSR | 子窗口托管 vs 离屏像素回调 |
| JS 绑定进化 | V8 扩展、IPC 消息；注意废弃的老绑定方式 |
| 网络与请求拦截 | Request/Resource Handler 增强 |
| 外观与运行参数 | `CefSettings`、command line 开关跟随 Chromium |
| 宽字符/高 DPI | Windows 高分屏、多监视器随 Chromium 改善 |

安全补丁节奏：嵌入 Chromium 意味着 **必须规划升级**；陈旧 CEF = 陈旧浏览器引擎。

---

## 与相近技术定位

| 方案 | 定位 |
|------|------|
| CEF | C/C++ 嵌入，宿主是原生程序 |
| Electron | 用 Web 技术写整应用，主进程 Node |
| WebView2 | Windows 上用系统 Edge；包体小，平台绑定 |
| Qt WebEngine | Qt 生态内的 Chromium 封装 |
| 系统 WebView | 轻，但版本碎片化 |

---

## 常见面试题

**Q1：CEF1 和 CEF3？**  
答：CEF3 才是现行多进程架构；CEF1 已过时。

**Q2：CEF 版本号怎么说？**  
答：以捆绑的 Chromium 版本 + CEF 构建号标识；升级为跟火车。

**Q3：和 Electron 区别？**  
答：CEF 嵌进已有 C++；Electron 以 JS/Node 为中心做应用壳。

---

## 实战要点

- 选型即接受 **体积（百 MB 级）与升级债**。  
- 安全产品必须有 Chromium 跟进策略。  

---

## 下一步

- [02-architecture-multiprocess.md](02-architecture-multiprocess.md)
