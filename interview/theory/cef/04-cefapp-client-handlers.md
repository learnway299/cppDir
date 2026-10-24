# 04 - CefApp / CefClient / Handler 体系

## 核心概念

CEF 用 **接口回调** 扩展行为：全局用 **`CefApp`**，每个浏览器实例用 **`CefClient`**，再按领域拆成多个 **Handler**。

```
CefApp (进程级)
  ├─ OnBeforeCommandLineProcessing
  ├─ CefBrowserProcessHandler / CefRenderProcessHandler …
  └─ …

CefClient (每个 CefBrowser)
  ├─ CefLifeSpanHandler      创建/关闭
  ├─ CefLoadHandler          加载状态
  ├─ CefDisplayHandler       标题、地址、全屏
  ├─ CefRequestHandler       导航、资源请求
  ├─ CefContextMenuHandler
  ├─ CefKeyboardHandler
  ├─ CefJSDialogHandler
  ├─ CefDownloadHandler
  ├─ CefRenderHandler        OSR
  └─ GetFocusHandler / DragHandler …
```

---

## 职责划分

| 接口 | 管什么 | 典型面试点 |
|------|--------|------------|
| `CefApp` | 启动参数、进程级钩子 | 改 command line、渲染进程初始化 |
| `CefRenderProcessHandler` | Renderer 内 | V8 扩展注册时机 |
| `CefClient` | 聚合 Get*Handler | 返回 this 或子对象 |
| `LifeSpanHandler` | `OnAfterCreated` / `DoClose` / `OnBeforeClose` | 关闭流程 |
| `LoadHandler` | 开始/结束/错误 | 加载失败页 |
| `RequestHandler` | `OnBeforeBrowse`、证书错误 | 拦截导航、权限 |
| `DisplayHandler` | 标题、tooltip、fullscreen | 同步到原生标题栏 |

引用计数：`CefRefPtr<>` 管理；Handler 生命周期必须覆盖浏览器存活期。

---

## 设计模式

- **单一 Client 实现多 Handler 接口**（demo 常见）  
- **组合**：Client 持有各 Handler 小对象（工程更清晰）  

---

## 常见面试题

**Q1：App 和 Client 区别？**  
答：App 进程级全局；Client 绑定具体 Browser 实例的 UI/加载/请求行为。

**Q2：JS 扩展应在哪注册？**  
答：多在 Render 进程 `OnContextCreated`（RenderProcessHandler）；与 Browser 通信仍走 IPC。

**Q3：如何拦截跳转？**  
答：`RequestHandler::OnBeforeBrowse` / 资源处理器；注意别死锁 UI 线程。

---

## 实战要点

- 先画「事件从用户点击到哪支 Handler」再写代码。  
- 所有 `CefRefPtr` 成员在 `OnBeforeClose` 路径上放空。  

---

## 下一步

- [05-browser-window-integration.md](05-browser-window-integration.md)
