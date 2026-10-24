# 07 - 进程间消息

## 核心概念

CEF 暴露的实用 IPC 原语是 **`CefProcessMessage`**（基于 Chromium IPC，业务层不必直接摸 Mojo）。

方向：

- Renderer → Browser：`CefBrowser::GetMainFrame()` / `SendProcessMessage`  
- Browser → Renderer：对 Frame `SendProcessMessage`  
- 接收：`CefClient::OnProcessMessageReceived` 或 RenderProcessHandler 侧对应回调  

---

## 消息设计

| 建议 | 原因 |
|------|------|
| 明确 name 空间 | `app.login` 优于 `msg1` |
| 参数用 `CefListValue`/`CefDictionaryValue` | 类型清晰 |
| 大数据走共享内存/文件 | 别塞巨型字符串进每条消息 |
| 请求-响应带 correlation id | 并发回调不乱 |

---

## 线程

`OnProcessMessageReceived` 有线程约束（看文档当前声明）；立刻 `CefPostTask` 到 UI 做窗口操作是稳妥模式。

---

## 常见面试题

**Q1：CEF 业务 IPC 用什么？**  
答：`CefProcessMessage`；底层是 Chromium IPC/Mojo，业务用 CEF 封装即可。

**Q2：和 ExecuteJavaScript 怎么选？**  
答：控制页面行为可用 JS；结构化高频 RPC 用 ProcessMessage 更清晰安全。

---

## 实战要点

- 先定义消息表再写两端。  
- 版本字段便于灰度升级。  

---

## 下一步

- [08-offscreen-rendering.md](08-offscreen-rendering.md)
