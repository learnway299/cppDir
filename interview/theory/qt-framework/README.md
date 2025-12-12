# Qt Framework Knowledge

Qt 框架知识 - 面试必备的 18 个主题。

---

## 知识体系

### 基础层 (Basic) - 6 个主题

Qt 核心机制，必须掌握。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [信号与槽](01-signal-slot.md) | 连接方式、线程安全、性能 |
| 02 | [对象模型](02-object-model.md) | QObject、对象树、内存管理 |
| 03 | [元对象系统](03-meta-object.md) | MOC、属性系统、动态调用 |
| 04 | [事件系统](04-event-system.md) | 事件类型、传递机制、过滤器 |
| 05 | [事件循环](05-event-loop.md) | QEventLoop、exec()、processEvents() |
| 06 | [消息机制对比](06-message-mechanism.md) | Qt vs Win32 vs Cocoa |

### 进阶层 (Intermediate) - 6 个主题

深入 Qt 应用开发。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 07 | [多线程](07-thread.md) | QThread、线程池、跨线程通信 |
| 08 | [Model/View](08-model-view.md) | 模型、视图、代理、自定义 |
| 09 | [图形系统](09-graphics.md) | QPainter、Graphics View、OpenGL |
| 10 | [网络与 IO](10-network-io.md) | QNetworkAccessManager、异步 IO |
| 11 | [高级主题](11-advanced.md) | 插件、资源系统、部署 |
| 12 | [常用控件](12-widgets.md) | 布局、对话框、自定义控件 |

### 深度层 (Advanced) - 6 个主题

Qt 内部机制与高级技巧。

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 13 | [字符串与容器](13-string-container.md) | QString、QList、隐式共享 |
| 14 | [样式与主题](14-style-theme.md) | QSS、自定义样式、主题切换 |
| 15 | [国际化](15-i18n.md) | tr()、翻译工具、RTL 支持 |
| 16 | [Qt 内部机制](16-qt-internals.md) | 源码分析、信号槽实现 |
| 17 | [Qt 与现代 C++](17-qt-modern-cpp.md) | C++17/20 集成、最佳实践 |
| 18 | [调试与性能](18-debug-profiling.md) | Qt Creator、Gammaray、性能分析 |

---

## 面试重点

### 必问话题

1. **信号与槽机制**
   - 五种连接方式及区别
   - 跨线程信号槽
   - 与回调函数的对比

2. **对象模型**
   - QObject 生命周期管理
   - 对象树与内存管理
   - 父子关系与 deleteLater

3. **事件系统**
   - 事件循环原理
   - 事件传递与过滤
   - 自定义事件

4. **多线程**
   - QThread 正确用法
   - moveToThread
   - 线程安全的信号槽

### 加分项

1. MOC 工作原理
2. 隐式共享 (Copy-on-Write)
3. Qt 与 STL 的选择
4. QML/Quick 基础

---

## 文件列表

```
qt-framework/
├── README.md              # 本文件
├── 01-signal-slot.md
├── 02-object-model.md
├── 03-meta-object.md
├── 04-event-system.md
├── 05-event-loop.md
├── 06-message-mechanism.md
├── 07-thread.md
├── 08-model-view.md
├── 09-graphics.md
├── 10-network-io.md
├── 11-advanced.md
├── 12-widgets.md
├── 13-string-container.md
├── 14-style-theme.md
├── 15-i18n.md
├── 16-qt-internals.md
├── 17-qt-modern-cpp.md
└── 18-debug-profiling.md
```
