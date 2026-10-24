# 03 - 启动、消息循环与关闭

## 核心概念

CEF 嵌入的生死线：**子进程入口 → 初始化 → 跑消息循环 → 有序关闭**。任何一步做错都会白屏、多开进程或退出卡死。

```
main()
  │
  ├─ CefExecuteProcess() ──有子进程角色──► 进入子进程逻辑并返回退出码
  │
  ├─ CefSettings + CefInitialize(app)
  ├─ 创建 Browser / 原生窗托管
  ├─ 运行消息循环 (CefRunMessageLoop 或集成宿主泵)
  ├─ 关闭所有 Browser、释放引用
  └─ CefShutdown()
```

---

## CefExecuteProcess

同一 EXE 被 Chromium 拉起作为 Renderer/GPU 等时，**必须先** `CefExecuteProcess`：

- 返回 `>=0`：当前就是子进程，干完应退出，**不要再 Initialize 宿主逻辑**  
- 返回 `-1`：当前是 Browser/宿主，继续初始化  

这是最高频的「启动必问题」。

---

## 初始化

`CefSettings` 常见项（口述级）：

| 项 | 含义 |
|----|------|
| `browser_subprocess_path` | 子进程可执行路径（可与主程序分离） |
| `cache_path` | 磁盘缓存/登录态；空则内存或默认策略 |
| `log_severity` / `log_file` | 排障 |
| `no_sandbox` | 调试用 |
| `windowless_rendering_enabled` | OSR |
| 语言、远程调试端口 | 开发期 |

`CefInitialize` 传入 `CefApp` 实现以挂全局钩子。

---

## 消息循环集成

| 模式 | 说明 |
|------|------|
| `CefRunMessageLoop` | CEF 自管泵；简单 demo |
| `CefDoMessageLoopWork` | 嵌入已有 Win32/Qt/wx 循环，定时泵 |
| 平台集成 API | 按官方示例与宿主框架选型 |

泵得不够：页面卡、定时器不灵；泵得太凶：占 CPU。

---

## 关闭顺序（考点）

1. 对 Browser 调关闭（`TryCloseBrowser` / 关闭事件）  
2. 等待 `OnBeforeClose` 等，释放 `CefRefPtr`  
3. 确认无残留浏览器对象  
4. `CefShutdown`  
5. 再退出宿主  

反模式：直接杀进程、Shutdown 时仍持有 Browser 引用 → 断言、挂起、孤儿进程。

---

## 常见面试题

**Q1：为何必须 CefExecuteProcess？**  
答：多进程共用 EXE 入口；子进程走 Chromium 子进程主函数，宿主不可重复初始化。

**Q2：如何把 CEF 接到已有 UI 循环？**  
答：在宿主 Idle/Timer 里 `CefDoMessageLoopWork`，或按平台用官方集成方式。

**Q3：退出时进程杀不干净？**  
答：查关闭顺序、引用环、是否还有隐藏 Browser、子进程 path 是否一致。

---

## 实战要点

- 单实例与多开：注意缓存目录锁。  
- 调试：`--remote-debugging-port` 用 Chrome DevTools 连。  

---

## 下一步

- [04-cefapp-client-handlers.md](04-cefapp-client-handlers.md)
