# 框架概述与架构

## 概述

Duilib 是基于 DirectUI 思想的 Windows 轻量级界面库。DirectUI 的核心理念是"无句柄控件"——整个窗口只创建一个真实的 HWND，所有可见的按钮、列表、文本框等控件都是通过 GDI/GDI+ 绘制的逻辑对象，不占用系统窗口句柄资源。

---

## 1. DirectUI 理念

### 什么是 DirectUI？

DirectUI 并非某个具体的库，而是一种 UI 架构思想：

- **传统方式**：每个控件是一个 HWND（Windows 句柄），系统负责绘制和消息路由
- **DirectUI 方式**：只有顶层窗口有 HWND，控件是内存中的 C++ 对象，由框架统一绘制和分发消息

```
传统 Win32/MFC 架构：
┌─────────────────────────────┐
│ 主窗口 HWND                  │
│  ┌──────┐ ┌──────┐ ┌─────┐ │
│  │按钮   │ │编辑框│ │列表  │ │  ← 每个控件都是独立 HWND
│  │HWND   │ │HWND  │ │HWND │ │
│  └──────┘ └──────┘ └─────┘ │
└─────────────────────────────┘

DirectUI / Duilib 架构：
┌─────────────────────────────┐
│ 主窗口 HWND（唯一真实句柄）    │
│  ┌──────┐ ┌──────┐ ┌─────┐ │
│  │按钮   │ │编辑框│ │列表  │ │  ← 逻辑对象，绘制在同一 DC 上
│  │对象   │ │对象  │ │对象 │ │
│  └──────┘ └──────┘ └─────┘ │
└─────────────────────────────┘
```

### 为什么选择 DirectUI？

| 优势 | 说明 |
|------|------|
| 资源占用低 | 不消耗大量 HWND 句柄（系统限制约 10000 个/进程） |
| 完全自绘 | 控件外观不受系统主题限制，可实现任意视觉效果 |
| 高性能 | 减少系统消息开销，双缓冲统一绘制 |
| 换肤容易 | 整体重绘即可，不需要逐个子窗口处理 |

| 劣势 | 说明 |
|------|------|
| 无障碍支持弱 | 无原生 HWND，屏幕阅读器难以识别 |
| 输入法兼容性 | 编辑框需要特殊处理 IME 定位 |
| 开发门槛 | 需要理解 Windows 消息机制和 GDI 绘制 |

---

## 2. Duilib 整体架构

### 核心类关系

```
CPaintManagerUI（核心管理器）
├── 窗口管理：持有 HWND，转发 Windows 消息
├── 控件树管理：维护根控件及整棵控件树
├── 渲染管理：双缓冲 DC，统一绘制
├── 资源管理：图片、字体、样式缓存
├── 事件分发：Notify 通知机制
└── 布局计算：触发控件树递归 SetPos

CWindowWnd（窗口基类）
├── 封装 Win32 窗口创建/注册
├── WndProc 静态回调 → 转发到虚函数 HandleMessage
└── 消息循环管理

CControlUI（控件基类）
├── 所有可视控件的基类
├── 核心虚函数：DoPaint / DoEvent / SetPos / EstimateSize
├── 属性系统：SetAttribute 统一处理 XML 属性
└── 树形关系：m_pParent / m_pManager
```

### 一次 UI 显示的完整流程

```cpp
// 1. 创建窗口
class CMainWnd : public CWindowWnd, public INotifyUI {
public:
    LPCTSTR GetWindowClassName() const { return _T("MainWnd"); }

    // 2. 窗口创建后，初始化 PaintManager 并加载 XML
    void OnFinalMessage(HWND hWnd) { delete this; }

    void Init() {
        m_pm.Init(m_hWnd);
        CDialogBuilder builder;
        // 3. XML 解析 → 创建控件树
        CControlUI* pRoot = builder.Create(_T("main.xml"), (UINT)0, NULL, &m_pm);
        m_pm.AttachDialog(pRoot);
        // 4. 注册通知接收者
        m_pm.AddNotifier(this);
    }

    // 5. 消息处理
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        LRESULT lRes = 0;
        if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
            return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    // 6. 接收控件通知
    void Notify(TNotifyUI& msg) {
        if (msg.sType == DUI_MSGTYPE_CLICK) {
            if (msg.pSender->GetName() == _T("btnClose")) {
                Close();
            }
        }
    }

private:
    CPaintManagerUI m_pm;
};

// 7. 入口
int WINAPI WinMain(...) {
    CPaintManagerUI::SetInstance(hInstance);
    CMainWnd* pWnd = new CMainWnd();
    pWnd->Create(NULL, _T("Demo"), UI_WNDSTYLE_FRAME, 0);
    pWnd->Init();
    pWnd->ShowWindow();

    CPaintManagerUI::MessageLoop();  // 消息循环
    return 0;
}
```

---

## 3. 源码目录结构

```
duilib/
├── Core/
│   ├── UIManager.h/cpp        # CPaintManagerUI - 核心管理器
│   ├── UIBase.h/cpp           # CWindowWnd 窗口基类
│   ├── UIControl.h/cpp        # CControlUI 控件基类
│   ├── UIContainer.h/cpp      # CContainerUI 容器基类
│   ├── UIRender.h/cpp         # 渲染相关
│   └── UIMarkup.h/cpp         # XML 解析器
├── Control/
│   ├── UIButton.h/cpp         # 按钮
│   ├── UILabel.h/cpp          # 标签
│   ├── UIList.h/cpp           # 列表
│   ├── UIEdit.h/cpp           # 编辑框
│   ├── UICombo.h/cpp          # 下拉框
│   └── ...                    # 其他控件
├── Layout/
│   ├── UIHorizontalLayout.h/cpp  # 水平布局
│   ├── UIVerticalLayout.h/cpp    # 垂直布局
│   ├── UITileLayout.h/cpp        # 瓦片布局
│   └── UITabLayout.h/cpp         # 标签页布局
└── Utils/
    ├── Utils.h/cpp            # 工具函数
    ├── UIDelegate.h/cpp       # 委托（早期事件机制）
    └── UIShadow.h/cpp         # 窗口阴影
```

---

## 4. 与 Win32 / MFC 的本质区别

| 维度 | Win32/MFC | Duilib |
|------|-----------|--------|
| 控件本质 | 每个控件是 HWND | 控件是 C++ 对象 |
| 消息路由 | 系统根据 HWND 分发 | 框架根据坐标 HitTest 分发 |
| 绘制方式 | 每个控件处理自己的 WM_PAINT | 统一在一个 DC 上绘制整棵树 |
| 控件外观 | 受系统主题影响 | 完全自定义 |
| 资源消耗 | HWND 数量有系统限制 | 仅受内存限制 |
| 布局方式 | 手动坐标或简单 Anchor | XML 声明式 + 布局管理器 |
| 换肤 | 困难，需逐个控件处理 | 替换 XML/图片资源即可 |

---

## 5. 面试常见问题

### Q1: Duilib 为什么只用一个 HWND？

**答：** 减少系统资源消耗，避免 HWND 句柄数量限制；统一绘制提高渲染效率；便于实现换肤和异形窗口。所有控件消息由框架内部根据鼠标坐标进行 HitTest 分发，不依赖系统的消息路由。

### Q2: Duilib 的编辑框（CEditUI）为什么是特殊的？

**答：** 编辑框需要接收键盘输入和 IME（输入法）消息，这些必须依赖真实 HWND。因此 CEditUI 内部实际创建了一个原生 Win32 Edit 控件（CEditWnd），在获得焦点时显示，失去焦点时隐藏并将文本同步回 CEditUI 的绘制状态。这是 DirectUI 框架中"破例使用子窗口"的典型场景。

### Q3: CPaintManagerUI 的核心职责是什么？

**答：** CPaintManagerUI 是 Duilib 的中枢，负责：
1. 持有窗口 HWND，接收所有 Windows 消息
2. 管理控件树（根控件、焦点控件、捕获控件）
3. 处理消息分发（HitTest → DoEvent）
4. 管理双缓冲 DC 和绘制流程
5. 维护资源缓存（图片、字体、颜色）
6. 分发 Notify 通知给外部监听者

### Q4: Duilib 如何处理鼠标消息分发？

**答：** 当收到 WM_MOUSEMOVE / WM_LBUTTONDOWN 等消息时，CPaintManagerUI 根据鼠标坐标对控件树进行逆序遍历（从上层到下层）的 HitTest，找到坐标落在哪个控件的矩形区域内，然后将消息封装为 TEventUI 传递给该控件的 DoEvent 方法。

---

## 6. 总结

| 特性 | 说明 |
|------|------|
| **架构思想** | DirectUI，无句柄控件，单 HWND |
| **核心管理器** | CPaintManagerUI，统管消息、绘制、控件树 |
| **控件本质** | C++ 对象，非系统窗口 |
| **绘制方式** | GDI/GDI+ 双缓冲统一绘制 |
| **布局定义** | XML 声明式 |
| **适用场景** | Windows 桌面客户端，追求自定义 UI 和轻量级 |
