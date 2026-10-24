# wxWidgets 框架考察

跨平台原生 GUI 框架面试专项 — 由浅入深覆盖历史版本、架构分层、事件与控件、实战开发与高频题。

> 对照阅读：[qt-framework/](../qt-framework/)、[duilib/](../duilib/)（同为桌面 UI，理念不同）

---

## 主题列表 (11 个主题)

### 基础层

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 01 | [01-history-and-versions.md](01-history-and-versions.md) | 历史与版本 | 发展脉络、2.x/3.x 主流特性、与 MFC/Qt 定位 |
| 02 | [02-architecture.md](02-architecture.md) | 框架架构 | 原生控件包装、分层、移植层、与 DirectUI 对比 |
| 03 | [03-app-event-loop.md](03-app-event-loop.md) | 应用与消息循环 | wxApp、OnInit、主循环、空闲事件 |
| 04 | [04-windows-controls.md](04-windows-controls.md) | 窗口与控件体系 | wxWindow 树、常用控件、所有权 |

### 进阶层

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 05 | [05-event-system.md](05-event-system.md) | 事件系统 | 事件表、Bind/Unbind、传播、自定义事件 |
| 06 | [06-sizer-layout.md](06-sizer-layout.md) | 布局管理 | wxSizer 家族、比例、嵌套、DPI |
| 07 | [07-drawing-resources.md](07-drawing-resources.md) | 绘图与资源 | DC、BufferedPaint、位图、XRC |
| 08 | [08-threading-dialogs.md](08-threading-dialogs.md) | 线程与对话框 | CallAfter、线程安全、标准对话框 |

### 深度层

| # | 文件 | 主题 | 核心内容 |
|---|------|------|----------|
| 09 | [09-cross-platform-build.md](09-cross-platform-build.md) | 跨平台与构建 | 条件编译、CMake、打包差异 |
| 10 | [10-theory-interview-qa.md](10-theory-interview-qa.md) | 理论考察题 | 高频概念与追问 |
| 11 | [11-practice-scenarios.md](11-practice-scenarios.md) | 实战场景考察 | 自定义控件、卡顿、DPI、国际化等 |

---

## 学习路径

1. **01 → 04**：历史定位 + 架构 + App/控件基础  
2. **05 → 08**：事件、布局、绘图、线程对话框  
3. **09 → 11**：跨平台工程 + 理论/实战刷题  

---

## 面试高频考点

- wxWidgets 与 Qt / MFC / Duilib 的本质区别（原生控件 vs 自绘）
- wxApp 生命周期与消息循环
- 事件表 vs `Bind`，事件向上传播
- Sizer 布局与比例、嵌套
- UI 线程约束与 `CallAfter` / `wxQueueEvent`
- 所有权：父子销毁规则
- 跨平台 `#ifdef` 与构建选项

---

## 文件列表

```
wxwidgets/
├── README.md
├── 01-history-and-versions.md
├── 02-architecture.md
├── 03-app-event-loop.md
├── 04-windows-controls.md
├── 05-event-system.md
├── 06-sizer-layout.md
├── 07-drawing-resources.md
├── 08-threading-dialogs.md
├── 09-cross-platform-build.md
├── 10-theory-interview-qa.md
└── 11-practice-scenarios.md
```
