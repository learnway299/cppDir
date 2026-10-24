# 03 - 应用入口与消息循环

## 核心概念

每个 wx 程序通常有一个 **`wxApp` 派生类**，宏 **`wxIMPLEMENT_APP(MyApp)`**（或等价）生成跨平台入口；真正业务从 **`OnInit()`** 开始，然后进入框架主循环，直到顶层窗口关闭或 `ExitMainLoop`。

```
启动 → wxApp::OnInit()
         │  创建 Frame、Show
         ▼
     主事件循环 (Yield / Dispatch)
         │  窗口消息 → wxEvent → Handler
         ▼
     OnExit() → 清理
```

---

## 原理深入

### wxApp 关键虚函数

| 方法 | 时机 | 典型工作 |
|------|------|----------|
| `OnInit` | 启动早期 | 解析命令行、创建主窗口、初始化语言/配置；返回 false 则退出 |
| `OnRun` | 默认进主循环 | 一般不重写；特殊嵌入场景可能定制 |
| `OnExit` | 循环结束后 | 释放全局资源（非窗口所有权内的） |

### 主循环在做什么

1. 从系统队列取消息（Win：GetMessage/Dispatch；GTK/macOS 各有泵）  
2. 翻译为 `wxEvent`  
3. 交给目标窗口处理函数 / Bind 的回调  
4. 处理空闲（`wxIdleEvent`）、定时器、投递的线程事件  

### 空闲事件与延后工作

- **`wxIdleEvent`**：队列空闲时触发，适合「稍后刷新 UI」但需 `RequestMore` 控制，避免空转吃 CPU。  
- **`CallAfter`**：在下一轮 UI 循环安全执行函数对象——**线程回 UI** 的首选之一。

### 模态与嵌套循环

`wxDialog::ShowModal` 会进入**嵌套消息循环**：外层循环未结束，内层先转。面试常问：模态期间定时器/外窗刷新是否仍可能发生？——通常仍派发，但需防重入。

---

## 与「嵌入」场景

把 wx 嵌进已有消息循环（少见面试加分）：可能用 `wxApp::SetExitOnFrameDelete`、手动 `Yield`，或与其它框架共存——高风险，需明确谁拥有泵。

---

## 常见面试题

**Q1：`OnInit` 返回 false 会怎样？**  
答：应用初始化失败，不会正常进主循环，进程退出。

**Q2：为什么不能在工作线程直接改控件？**  
答：多数 UI 工具包非线程安全；wx 要求 UI 操作在主线程，用 `CallAfter` / `wxQueueEvent` 投递。

**Q3：`wxIMPLEMENT_APP` 做什么？**  
答：生成 `main`/`WinMain` 等入口，创建 `wxApp` 实例并跑起来，屏蔽平台差异。

---

## 实战要点

- `OnInit` 保持短：重 I/O 放到窗口显示后异步。  
- 全局单例生命周期与 `OnExit`、智能指针清理对齐，避免 Frame 销毁后仍回调。  

---

## 下一步

- 窗口控件：[04-windows-controls.md](04-windows-controls.md)  
- 事件：[05-event-system.md](05-event-system.md)
