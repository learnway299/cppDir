# 前端容器技术 (Frontend Container)

本模块系统性考察桌面应用中嵌入 Web 前端容器的技术栈，涵盖容器生命周期、JS 与原生代码通信、多进程架构等核心知识点。

---

## 目录

### 基础原理篇

| 文件 | 主题 | 关键内容 |
|------|------|----------|
| [01-container-lifecycle.md](01-container-lifecycle.md) | 容器生命周期 | 初始化流程、渲染管线、页面加载过程 |
| [02-js-native-binding.md](02-js-native-binding.md) | JS-C++ 绑定机制 | V8 扩展、绑定原理、同步/异步调用 |
| [03-ipc-protocol.md](03-ipc-protocol.md) | 进程间通信协议 | IPC 机制、消息序列化、Mojo/MessagePort |

### 框架深入篇

| 文件 | 主题 | 关键内容 |
|------|------|----------|
| [04-cef-deep-dive.md](04-cef-deep-dive.md) | CEF 深入剖析 | 多进程模型、CefClient、Handler 体系；完整专项见 [../cef/](../cef/) |
| [05-electron.md](05-electron.md) | Electron | Main/Renderer 进程、contextBridge、预加载脚本 |
| [06-webview2.md](06-webview2.md) | WebView2 | Edge 内核、COM 接口、Windows 集成 |
| [07-qt-webengine.md](07-qt-webengine.md) | Qt WebEngine | QWebChannel、信号槽桥接、Qt 生态整合 |
| [08-tauri.md](08-tauri.md) | Tauri | Rust 后端、系统 WebView、轻量化方案 |

### 进阶篇

| 文件 | 主题 | 关键内容 |
|------|------|----------|
| [09-performance-security.md](09-performance-security.md) | 性能与安全 | 内存优化、沙箱机制、CSP、XSS 防护 |

---

## 技术栈对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        前端容器技术对比                                      │
├─────────────┬─────────────┬─────────────┬─────────────┬─────────────────────┤
│   框架       │   内核       │   语言      │  包体积     │   适用场景          │
├─────────────┼─────────────┼─────────────┼─────────────┼─────────────────────┤
│ CEF         │ Chromium    │ C/C++       │ ~100MB+     │ 原生应用嵌入浏览器  │
│ Electron    │ Chromium    │ Node.js     │ ~150MB+     │ 跨平台桌面应用      │
│ WebView2    │ Edge        │ C++/C#      │ 系统自带    │ Windows 原生应用    │
│ Qt WebEngine│ Chromium    │ C++/QML     │ ~100MB+     │ Qt 生态应用         │
│ Tauri       │ 系统 WebView │ Rust        │ ~5-10MB     │ 轻量跨平台应用      │
└─────────────┴─────────────┴─────────────┴─────────────┴─────────────────────┘
```

---

## 核心考察维度

1. **容器初始化**：从进程创建到首屏渲染的完整链路
2. **通信机制**：JS 调用 C++、C++ 调用 JS 的实现原理
3. **多进程架构**：Browser/Renderer/GPU 进程分工与 IPC
4. **内存管理**：多进程内存占用、共享内存优化
5. **安全模型**：沙箱隔离、CSP 策略、权限控制

---

## 学习路径建议

```
入门 → 01 容器生命周期 → 02 JS-Native 绑定 → 03 IPC 协议
  ↓
深入 → 选择一个框架精读 (CEF/Electron/WebView2)
  ↓
进阶 → 09 性能与安全 → 横向对比各框架优劣
```
