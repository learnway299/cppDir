# 05 - 浏览器窗口与原生集成

## 核心概念

常见两种呈现：

1. **窗口模式（有窗）**：CEF 创建子 HWND/NSView 挂到原生父窗口  
2. **窗口less / OSR**：无子窗，像素画到宿主（见 08）

多数业务客户端用 **父窗口托管子浏览器窗口**。

```
原生 Frame/Dialog
 └── 子区域 (parent handle)
      └── CEF Browser 子窗口
```

---

## 创建浏览器

`CefBrowserHost::CreateBrowser` / `CreateBrowserSync`：

- 传入 `CefWindowInfo`（父窗口、位置、样式）  
- `CefClient`  
- URL、`CefBrowserSettings`  

异步创建更常见；`OnAfterCreated` 里拿 `CefRefPtr<CefBrowser>`。

---

## 焦点、输入、DPI

| 问题 | 处理思路 |
|------|----------|
| 焦点抢夺 | 按官方建议处理 `SetFocus`、原生与浏览器互抢 |
| IME 输入法 | 窗口模式通常较好；OSR 要自己喂组成事件 |
| DPI 缩放 | 父窗 DPI 变化时调整子窗大小与 screen 因子 |
| 最小化/恢复 | 通知或 resize，避免恢复后黑屏 |

移动父窗口客户区：`Moved`/`WasResized` 一类 Host API（名称随版本以文档为准）通知 CEF。

---

## 多标签

一父窗多 `CefBrowser`，或原生 Tab 切换 Show/Hide 子窗；注意内存与进程数。关闭 Tab 必须走浏览器关闭协议。

---

## 常见面试题

**Q1：CEF 如何嵌进 Win32/Qt/wx 窗口？**  
答：以原生父句柄填 `CefWindowInfo`，创建 Browser；宿主负责 resize 同步。

**Q2：黑屏/白屏常见原因？**  
答：尺寸为 0、未泵消息、GPU 问题、错误的子进程路径、初始化失败、关了沙箱仍缺资源文件。

**Q3：有窗 vs OSR？**  
答：有窗集成简单、输入法友好；OSR 要自绘与注入，适合叠加 3D/特殊合成。

---

## 实战要点

- 保证 `icudtl.dat`、`.pak`、snapshot 等资源路径正确。  
- 先用 cefsimple 验证环境再嵌入复杂壳。  

---

## 下一步

- [06-js-native-bridge.md](06-js-native-bridge.md)
