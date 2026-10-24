# 06 - JS ↔ Native 桥接

## 核心概念

业务要「网页按钮调 C++」「C++ 推事件到页面」。CEF 典型路径：

1. **Renderer**：在 V8 Context 注入 JS 函数（`CefV8Handler`）  
2. **IPC**：Renderer ↔ Browser 发 `CefProcessMessage`  
3. **Browser**：原生执行业务，必要时再 `ExecuteJavaScript` 或 IPC 回传  

```
JS 调用
  → V8Handler (Renderer)
  → SendProcessMessage → Browser
  → C++ 业务
  → (可选) ExecuteJavaScript / IPC → JS 回调
```

---

## 为何不在 Renderer 直接干重活？

Renderer 有沙箱与线程限制；文件、窗口、业务数据库通常在 **Browser/宿主**。桥接 = **能力上移 + 消息边界**。

---

## 原生调用 JS

- `CefFrame::ExecuteJavaScript`  
- 注意：字符串拼脚本易注入；优先固定桥与 JSON 参数  
- 必须在正确线程 / 正确 Frame（主 frame vs iframe）  

---

## 安全要点

| 风险 | 缓解 |
|------|------|
| 任意 JS 调特权原生 | 白名单方法、参数校验 |
| 网页内容不可信 | 少暴露 API；本地页面用自定义 scheme |
| 同步堵 UI | 异步消息 + 回调 |
| 字符串拼接 XSS | 结构化消息 |

---

## 与 frontend-container 联系

绑定思想类似「JS-Native」；CEF 细节是 V8Handler + 多进程 IPC。见 [02-js-native-binding.md](../frontend-container/02-js-native-binding.md)。

---

## 常见面试题

**Q1：JS 调 C++ 全链路？**  
答：注入函数 → Renderer Handler → ProcessMessage → Browser 处理 → 可选回传。

**Q2：能在 Browser 进程直接碰 V8 吗？**  
答：V8 在 Renderer；Browser 侧不要幻想直接拿 JS 对象。

**Q3：如何设计稳定桥？**  
答：版本化消息名、JSON schema、超时、错误码、禁止同步长调用。

---

## 实战要点

- 本地 UI 用自定义协议（`client://`）加载，减少 file 权限问题。  
- 日志打 message name 与方向，排障极快。  

---

## 下一步

- [07-ipc-messaging.md](07-ipc-messaging.md)
