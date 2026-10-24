# C++ Interview Guide
```
狗尾续貂 博君一笑

C++ 面试指南 - 系统性准备 C++ 与 Qt 面试的完整资料库。
```

## 目录结构

```
interview/
├── README.md                 # 本文件 - 面试指南总览
├── theory/                   # 理论知识部分
│   ├── cpp-core/            # C++ 核心知识 (26 个主题)
│   │   ├── README.md
│   │   ├── 01-basic-syntax.md
│   │   └── ...
│   ├── qt-framework/        # Qt 框架知识 (24 个主题)
│   │   ├── README.md
│   │   ├── 01-signal-slot.md
│   │   └── ...
│   ├── debugging/           # 调试与问题排查 (10 个主题)
│   │   ├── README.md
│   │   ├── 01-crash-dump.md
│   │   └── ...
│   ├── operating-system/    # 操作系统 (9 个主题)
│   │   ├── README.md
│   │   ├── 01-process.md
│   │   └── ...
│   ├── network/             # 计算机网络 (10 + 8 主题)
│   │   ├── README.md
│   │   ├── fundamentals/    # 协议栈基础 (10 个主题)
│   │   │   ├── 01-network-model.md
│   │   │   └── ...
│   │   └── web-protocols-interview/  # HTTP/HTTPS/WebSocket 面试专项 (8 个主题)
│   │       ├── 01-http-fundamentals.md
│   │       └── ...
│   ├── graphics/            # 图形与视觉 (8 个主题)
│   │   ├── README.md
│   │   ├── 01-graphics-fundamentals.md
│   │   └── ...
│   ├── duilib/              # Duilib 框架知识 (12 个主题)
│   │   ├── README.md
│   │   ├── 01-architecture.md
│   │   └── ...
│   ├── frontend-container/  # 前端容器技术 (9 个主题)
│   │   ├── README.md
│   │   ├── 01-container-lifecycle.md
│   │   └── ...
│   ├── dev-tools/          # 开发工具 (5 个主题)
│   │   ├── README.md
│   │   ├── 01-git.md
│   │   └── ...
│   ├── chromium-internals/  # Chromium 内核深入 (8 个主题)
│   │   ├── README.md
│   │   ├── 01-architecture.md
│   │   └── ...
│   ├── ai-coding/           # AI 编程考察 (9 个主题)
│   │   ├── README.md
│   │   ├── 01-ai-fundamentals.md
│   │   └── ...
│   ├── wxwidgets/           # wxWidgets 框架考察 (11 个主题)
│   │   ├── README.md
│   │   ├── 01-history-and-versions.md
│   │   └── ...
│   └── cef/                 # CEF 框架考察 (12 个主题)
│       ├── README.md
│       ├── 01-history-and-versions.md
│       └── ...
└── coding/                   # 编程实战部分
    ├── README.md
    ├── 01-basic/            # 基础题
    ├── 02-linked-list/      # 链表
    ├── 03-tree/             # 树
    ├── 04-dp/               # 动态规划
    ├── 05-string/           # 字符串
    ├── 06-stack-queue/      # 栈与队列
    ├── 07-hash-design/      # 哈希与设计
    ├── 08-sorting/          # 排序
    ├── 09-graph/            # 图
    ├── 10-smart-pointer/    # 智能指针实现
    ├── 11-memory/           # 内存管理
    ├── 12-oop-design/       # OOP 设计模式
    ├── 13-thread/           # 多线程编程
    ├── 14-lockfree/         # 无锁编程
    ├── 15-template/         # 模板元编程
    ├── 16-coroutine/        # 协程
    ├── 17-backtracking/     # 回溯
    ├── 18-two-pointers/     # 双指针
    ├── 19-bit-manipulation/ # 位运算
    ├── 20-heap/             # 堆
    └── 21-system-design/    # 系统设计
```

---

## 内容概览

### 理论部分 (Theory)

#### C++ 核心知识 (26 个主题)

| 层级 | 主题数 | 内容 |
|------|--------|------|
| 基础层 | 11 | 语法、类型、函数、OOP、内存、STL、模板、多线程基础 |
| 进阶层 | 7 | 高级多线程、编译链接、异常、现代C++、性能优化、调试 |
| 深度层 | 8 | ABI、UB、位运算、字符串编码、IO、时间、元编程、跨语言 |

#### Qt 框架知识 (24 个主题)

| 层级 | 主题数 | 内容 |
|------|--------|------|
| 基础层 | 7 | 信号槽、对象模型、元对象、事件系统、事件循环、消息机制、定时器 |
| 进阶层 | 7 | 多线程、Model/View、图形、网络IO、高级主题、控件、QML/Qt Quick |
| 深度层 | 10 | 容器、样式主题、国际化、内部机制、现代C++集成、调试、绘图系统、Windows消息、动画系统、智能指针 |


#### Duilib 框架知识 (12 个主题)

| 层级 | 主题数 | 内容 |
|------|--------|------|
| 基础层 | 5 | 架构概述、消息循环、XML布局、控件体系、事件通知 |
| 进阶层 | 4 | 渲染机制、自定义控件、布局管理器、资源换肤 |
| 深度层 | 3 | 性能优化、多线程UI更新、常见问题调试 |

#### 调试与问题排查 (10 个主题)

| 主题 | 内容 |
|------|------|
| 崩溃分析 | Crash Dump 分析、崩溃捕获机制 (SEH/VEH/Mach Exception) |
| 内存问题 | 内存泄漏检测、野指针排查、内存剖析 |
| 多线程调试 | 死锁检测、竞态条件、线程调试技巧 |
| 性能分析 | 性能剖析、逆向调试、Sanitizers |
| 生产环境 | Coredump 收集与分析 |

#### 操作系统 (9 个主题)

| 主题 | 内容 |
|------|------|
| 进程与内存 | 进程管理、内存管理、虚拟内存 |
| 存储与IO | 文件系统、IO 模型、系统调用 |
| 同步与通信 | 同步原语、IPC/RPC 机制 |
| 内核 | Linux 内核基础、CEF 容器进程模型 |

#### 计算机网络 (10 + 8 个主题)

| 子模块 | 主题 | 内容 |
|--------|------|------|
| fundamentals/ | 协议栈 | OSI/TCP-IP 模型、链路层、网络层、传输层、应用层 |
| fundamentals/ | 编程实践 | Socket 编程、网络安全、性能优化 |
| fundamentals/ | 应用场景 | 浏览器导航流程、心跳保活机制 |
| web-protocols-interview/ | Web 协议专项 | HTTP 版本演进、HTTPS/TLS、WebSocket、对比选型、面试 Q&A、用户环境排查 |

#### 图形与视觉 (8 个主题)

| 主题 | 内容 |
|------|------|
| 图形学基础 | 渲染管线、坐标变换、光照模型 |
| OpenGL | 基础与高级渲染技术 |
| OpenCV | 图像处理、特征匹配、目标检测、3D 视觉 |

#### 前端容器技术 (9 个主题)

| 主题 | 内容 |
|------|------|
| 核心机制 | 容器生命周期、JS-Native 绑定、IPC 协议 |
| 框架深度 | CEF、Electron、WebView2、Qt WebEngine、Tauri |
| 工程实践 | 性能优化与安全加固 |

#### 开发工具 (5 个主题)

| 主题 | 内容 |
|------|------|
| Git | 工作原理、分支合并、冲突解决、撤销操作、高级技巧 |
| WinDbg 命令 | 符号、线程、调用栈、内存、断点、异常、堆分析、TTD |
| WinDbg 实战 | Crash Dump、堆损坏、无符号定位、死锁、延迟崩溃 |
| CMake 基础 | 工作模型、target-based、PRIVATE/PUBLIC/INTERFACE、生成器 |
| CMake 实战 | 多模块工程、依赖管理、Qt/MSVC、安装导出、构建排错 |

#### Chromium 内核深入 (8 个主题)

| 主题 | 内容 |
|------|------|
| 架构与进程 | 多进程架构、沙箱机制、站点隔离、线程模型 |
| 渲染全链路 | URL 输入到页面展示完整流程、关键渲染路径 |
| 渲染引擎 | Blink 渲染流水线、样式计算、布局、绘制 |
| JS 引擎 | V8 编译管线、隐藏类、内联缓存、垃圾回收 |
| 合成器 | 图层系统、合成线程、光栅化、帧调度 |
| IPC 通信 | Mojo 机制、消息管道、共享内存、数据管道 |
| 内存管理 | PartitionAlloc、Oilpan (Blink GC)、内存压力处理 |
| 性能优化 | 渲染/JS/内存/网络优化、DevTools、Trace Event |

#### AI 编程考察 (9 个主题)

| 层级 | 主题 | 内容 |
|------|------|------|
| 基础 | 01-02 | LLM/RAG/Agent 概念、编程 Agent 客户端与服务端架构 |
| 进阶 | 03-06 | 幻觉等术语与缓解、Context 工程、开源工具链、主流产品对比 |
| 深度 | 07-09 | 质量安全与评测、理论面试题、实战场景考察 |

#### wxWidgets 框架考察 (11 个主题)

| 层级 | 主题 | 内容 |
|------|------|------|
| 基础 | 01-04 | 历史版本、原生控件架构、wxApp 循环、窗口控件树 |
| 进阶 | 05-08 | 事件 Bind、Sizer 布局、DC/XRC、线程与对话框 |
| 深度 | 09-11 | 跨平台构建、理论题、实战场景 |

#### CEF 框架考察 (12 个主题)

| 层级 | 主题 | 内容 |
|------|------|------|
| 基础 | 01-04 | 历史与 Chromium 版本、多进程、启动关闭、App/Client/Handler |
| 进阶 | 05-08 | 窗口集成、JS 桥、IPC、离屏渲染 |
| 深度 | 09-12 | 性能内存、安全沙箱、理论题、实战场景 |

### 编程部分 (Coding)

共 21 个分类，约 90+ 道经典面试题，每题包含：
- 问题描述与分析
- 解题思路
- 完整代码实现
- 复杂度分析
- 常见变体与扩展

---

## 使用建议

### 学习路径

1. **基础巩固** (1-2 周)
   - 理论：C++ 基础层 11 个主题
   - Coding：01-basic, 02-linked-list, 03-tree

2. **核心强化** (2-3 周)
   - 理论：C++ 进阶层 + Qt 基础层 + Duilib 基础层
   - Coding：04-dp, 05-string, 06-stack-queue, 07-hash-design

3. **深度提升** (2-3 周)
   - 理论：C++ 深度层 + Qt 进阶/深度层
   - Coding：10-smart-pointer, 11-memory, 12-oop-design, 13-thread

4. **综合冲刺** (1-2 周)
   - 回顾所有高频考点
   - Coding：14-lockfree, 15-template, 21-system-design

### 面试前复习清单

#### C++ 核心
- [ ] 内存模型与智能指针
- [ ] 虚函数表与多态实现
- [ ] 模板与 SFINAE
- [ ] 多线程同步原语
- [ ] STL 容器底层实现
- [ ] 常见设计模式

#### Qt 框架
- [ ] 信号槽机制与 MOC
- [ ] 事件循环与事件过滤
- [ ] Model/View 架构
- [ ] 动画系统原理与性能优化
- [ ] Qt 智能指针 (QPointer/QSharedPointer)
- [ ] QML/Qt Quick 与 C++ 交互
- [ ] 定时器类型与精度 (QTimer/QBasicTimer)

#### Duilib 框架
- [ ] DirectUI 架构与单 HWND 原理
- [ ] 消息循环与 HitTest 分发机制
- [ ] XML 布局解析与控件工厂
- [ ] 双缓冲渲染与自定义控件开发
- [ ] 虚拟列表性能优化

#### 调试与系统
- [ ] 崩溃捕获机制 (SEH/VEH/Mach Exception)
- [ ] 内存泄漏与野指针排查
- [ ] 操作系统进程/内存/IO 模型
- [ ] WinDbg 基础命令与符号加载
- [ ] WinDbg Dump 分析、堆损坏与无符号定位

#### 网络与图形
- [ ] TCP/UDP 协议与 Socket 编程
- [ ] HTTP/HTTPS 与网络安全
- [ ] OpenGL 渲染管线 / OpenCV 图像处理

#### 前端容器
- [ ] CEF/Electron 进程模型
- [ ] JS-Native 绑定机制

#### Chromium 内核
- [ ] 多进程架构与沙箱机制
- [ ] URL 输入到页面展示全流程
- [ ] Blink 渲染流水线（Style → Layout → Paint → Composite）
- [ ] V8 编译管线与 GC 机制
- [ ] 合成器与图层优化

#### 开发工具
- [ ] Git 分支合并、rebase、reset/revert、reflog
- [ ] WinDbg 基础命令、Dump 分析、堆损坏定位
- [ ] CMake target-based 写法与依赖传播
- [ ] CMake 第三方依赖、Qt/MSVC 集成与安装导出

#### AI 编程
- [ ] LLM / RAG / Agent 基础与选型（Prompt vs RAG vs Fine-tune）
- [ ] Coding Agent 架构（客户端、Runtime、工具/MCP、索引）
- [ ] 幻觉、注入、目标漂移等术语与缓解
- [ ] Context/Rules、开源工具链、Copilot/Cursor/Claude/Codex 对比
- [ ] AI PR 验收、安全合规、理论题与实战场景

#### wxWidgets
- [ ] 与 Qt/MFC/Duilib 定位差异（原生控件包装）
- [ ] wxApp 生命周期、事件表 vs Bind、Sizer 布局
- [ ] UI 线程与 CallAfter、父子所有权
- [ ] 2.x→3.x / HiDPI / 跨平台构建要点

#### CEF
- [ ] 与 Electron/WebView2 差异、Chromium 版本跟随
- [ ] CefExecuteProcess、多进程、有序关闭
- [ ] CefApp/Client/Handler、JS↔C++ 桥与 IPC
- [ ] 嵌窗排障、OSR、沙箱与升级策略

#### 手写代码
- [ ] LRU Cache、智能指针、线程池
- [ ] 生产者消费者、读写锁

---

## 参考资源

- [C++ Reference](https://en.cppreference.com/)
- [Qt Documentation](https://doc.qt.io/)
- [LeetCode](https://leetcode.com/)
- Effective C++ / Effective Modern C++
- C++ Concurrency in Action

---

## 更新日志

- 2026-10: 新增 wxWidgets (11) 与 CEF (12) 框架考察模块
- 2026-09: 新增 AI 编程考察模块 (9 个主题)
- 2026-03: 新增 Duilib 框架模块 (12 个主题)
- 2026-02: 新增 Chromium 内核深入模块 (8 个主题)
- 2026-01: 新增调试、操作系统、网络、图形、前端容器 5 大模块
- 2025-12: 初始版本创建
