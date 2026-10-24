# 08 - 多线程与对话框

## 核心概念

**一切 UI 操作在主（UI）线程**；工作线程做计算/IO，通过队列把结果投递回 UI。

对话框分 **模态**（`ShowModal`）与 **非模态**（`Show`），模态带嵌套消息循环。

---

## 线程安全模式

| 做法 | 说明 |
|------|------|
| `CallAfter(fn)` | 简单；捕获需注意生命周期 |
| `wxQueueEvent` / `wxPostEvent` | 跨线程投递事件到窗口 |
| `wxThread` + `OnExit` | 框架线程类；仍禁止直接碰 UI |
| 原子/互斥保护共享数据 | UI 只读快照，避免边遍历边改 |

反模式：工作线程 `text->SetValue(...)` —— 偶现崩溃、难复现。

```
Worker Thread                 UI Thread
     │                            │
     │  完成结果                   │
     │ ── CallAfter / QueueEvent ►│
     │                            │ 更新控件
```

---

## 对话框

| API | 行为 |
|-----|------|
| `ShowModal` | 阻塞调用栈直至结束，返回码 `wxID_OK` 等 |
| `Show(true)` | 非模态，立即返回 |
| 标准盒 | `wxMessageBox`、`wxFileDialog`、`wxDirDialog`、`wxColourDialog`… |

自定义 `wxDialog`：内容同样用 Panel + Sizer；默认按钮绑定 `wxID_OK`/`CANCEL` 可走标准关闭路径。

### 模态重入注意

模态期间定时器、外窗事件仍可能进来——处理器需防重入（禁用按钮、标志位）。

---

## 常见面试题

**Q1：子线程更新进度条？**  
答：子线程算进度，`CallAfter`/`QueueEvent` 到 UI 调 `SetValue`；或用 `wxUpdateUI`/`wxTimer` 抽查询（次优）。

**Q2：ShowModal 和 Show 区别？**  
答：模态阻塞并嵌套循环，适合确认流；非模态不阻塞，适合工具窗。

**Q3：wxThread 删除自己？**  
答：关注 `TestDestroy`、分离/汇合模式；避免在 UI 回调里死等线程而不处理队列（死锁）。

---

## 实战要点

- 长任务：进度对话框 + 可取消标志。  
- 窗口关闭时：停止线程或使回调失效（`Weak` 语义 / 标志）。  

---

## 下一步

- [09-cross-platform-build.md](09-cross-platform-build.md)
