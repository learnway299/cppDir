# 02 - wxWidgets 框架架构

## 核心概念

wxWidgets 的核心设计是 **「一套 API + 多平台后端」**：你写 `wxFrame` / `wxButton`，在 Windows 上落到 Win32 控件，在 Linux 上落到 GTK，在 macOS 上落到 Cocoa——**不是**整窗自绘 DirectUI。

```
应用代码 (wxFrame / wxPanel / wxSizer / Bind)
              │
              ▼
     wxWidgets 公共 API 层
              │
    ┌─────────┼─────────┐
    ▼         ▼         ▼
  wxMSW     wxGTK     wxOSX/...
 (Win32)    (GTK)     (Cocoa)
```

---

## 框架分层

| 层级 | 职责 | 面试抓手 |
|------|------|----------|
| 应用层 | `wxApp`、业务 Frame/Dialog | 入口与生命周期 |
| 窗口/控件层 | `wxWindow` 树、标准控件 | 父子关系与所有权 |
| 布局层 | `wxSizer` | 几何而非绝对坐标 |
| 事件层 | 事件表 / Bind、命令事件 | 路由与传播 |
| 图形/资源 | DC、Bitmap、XRC | 自绘与皮肤化能力边界 |
| 移植层 (Port) | MSW/GTK/OSX | `#ifdef __WXMSW__` 等 |

### 与常见 GUI 方案对比

| | wxWidgets | Qt | Duilib | MFC |
|---|-----------|-----|--------|-----|
| 控件实现 | 多为原生 | 自绘/自有为主 | 全逻辑控件 | Win 原生/封装 |
| 跨平台 | 是 | 是 | 基本仅 Windows | 否 |
| 元对象 | 无 MOC | 强（信号槽） | 无 | 消息映射 |
| 外观 | 跟系统 | 可高度统一皮肤 | 完全自定义 | 系统风 |

---

## 关键抽象

### wxObject / RTTI 辅助

多数类继承 `wxObject`，提供运行时类信息、引用计数辅助等；强度不如 Qt meta-object，但足以支撑动态转换与 XRC。

### wxWindow 是中枢

几乎所有可见元素最终是 `wxWindow`（或子类）：

- 处理原生消息翻译成 `wxEvent`
- 持有子控件列表
- 参与 Sizer 布局测量

### 「包装原生」的含义与代价

**收益**：辅助技术、字体、IME、系统主题更自然。  
**代价**：各平台控件能力不一致；极端自定义外观弱于 DirectUI；某些行为要写 port 条件代码。

---

## 源码/模块心智模型（面试够用）

不必背目录，能分清：

- **公共头文件**：你 include 的 API  
- **backend 实现**：同头不同 `.cpp`（MSW/GTK）  
- **可选组件**：AUI、WebView、RichText、QA 等可裁剪  

---

## 常见面试题

**Q1：wxWidgets 是自绘框架吗？**  
答：默认不是。主流路径是原生控件包装；可用 DC 自绘扩展，但定位不同于 Duilib 单 HWND 全自绘。

**Q2：如何做到跨平台？**  
答：稳定公共 API + 各平台 port 实现；应用层少碰原生句柄，必要时用条件编译或抽象接口隔离。

**Q3：为什么同一套布局在 Win/GTK 上间距不同？**  
答：原生控件默认尺寸、边框、字体度量平台差异；Sizer 用比例与最小尺寸缓解，仍需真机调。

---

## 实战要点

- 业务代码依赖 **wx API**，避免直接 `HWND`/`GtkWidget` 散落各处。  
- 深度定制 UI 前先评估：是否应选 Qt/自绘方案。  

---

## 下一步

- 应用循环：[03-app-event-loop.md](03-app-event-loop.md)  
- 控件树：[04-windows-controls.md](04-windows-controls.md)
