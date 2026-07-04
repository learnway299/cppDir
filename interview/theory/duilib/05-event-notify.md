# 事件与通知机制

## 概述

Duilib 有两套事件机制：底层的 DoEvent（控件内部处理鼠标/键盘等原始事件）和上层的 Notify（业务层通知，如按钮点击、列表选中）。DoEvent 面向控件开发者，Notify 面向应用开发者。理解两者的关系和使用场景是 Duilib 开发的关键。

---

## 1. 两套机制的定位

```
Windows 消息 (WM_xxx)
    │
    ▼
CPaintManagerUI::MessageHandler
    │
    ▼
┌─────────────────────────────────────────┐
│  DoEvent（底层事件）                      │
│  - 控件内部使用                           │
│  - 处理原始输入：鼠标进出、按下抬起、键盘  │
│  - 用于实现控件行为（状态切换、绘制更新）   │
└─────────────────────────────────────────┘
    │ 控件处理完后，对外发出通知
    ▼
┌─────────────────────────────────────────┐
│  Notify（上层通知）                       │
│  - 业务层使用                            │
│  - 语义化事件：click、textchanged、select │
│  - 通过 INotifyUI 接口分发给窗口类        │
└─────────────────────────────────────────┘
```

---

## 2. DoEvent 底层事件

### TEventUI 结构

```cpp
struct TEventUI {
    int Type;              // 事件类型（UIEVENT_xxx）
    CControlUI* pSender;  // 发送者
    DWORD dwTimestamp;     // 时间戳
    POINT ptMouse;         // 鼠标位置
    TCHAR chKey;           // 按键字符
    WORD wKeyState;        // 键盘修饰键状态
    WPARAM wParam;
    LPARAM lParam;
};
```

### 事件类型枚举

```cpp
enum EVENTTYPE_UI {
    UIEVENT__FIRST = 0,
    UIEVENT_MOUSEENTER,     // 鼠标进入
    UIEVENT_MOUSELEAVE,     // 鼠标离开
    UIEVENT_MOUSEMOVE,      // 鼠标移动
    UIEVENT_MOUSEHOVER,     // 鼠标悬停
    UIEVENT_BUTTONDOWN,     // 鼠标左键按下
    UIEVENT_BUTTONUP,       // 鼠标左键抬起
    UIEVENT_RBUTTONDOWN,    // 鼠标右键按下
    UIEVENT_DBLCLICK,       // 双击
    UIEVENT_SCROLLWHEEL,    // 滚轮
    UIEVENT_KEYDOWN,        // 键盘按下
    UIEVENT_KEYUP,          // 键盘抬起
    UIEVENT_CHAR,           // 字符输入
    UIEVENT_SETFOCUS,       // 获得焦点
    UIEVENT_KILLFOCUS,      // 失去焦点
    UIEVENT_TIMER,          // 定时器
    UIEVENT_CONTEXTMENU,    // 右键菜单
    UIEVENT__LAST,
};
```

### DoEvent 处理示例

```cpp
// CButtonUI 的事件处理
void CButtonUI::DoEvent(TEventUI& event)
{
    if (!IsEnabled()) {
        // 禁用状态下事件传给父控件
        CControlUI::DoEvent(event);
        return;
    }

    switch (event.Type) {
    case UIEVENT_MOUSEENTER:
        if (::PtInRect(&m_rcItem, event.ptMouse)) {
            m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        break;

    case UIEVENT_MOUSELEAVE:
        m_uButtonState &= ~UISTATE_HOT;
        Invalidate();
        break;

    case UIEVENT_BUTTONDOWN:
        m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
        Invalidate();
        break;

    case UIEVENT_BUTTONUP:
        if (m_uButtonState & UISTATE_CAPTURED) {
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
            // 鼠标在控件范围内抬起 → 触发点击通知
            if (::PtInRect(&m_rcItem, event.ptMouse)) {
                m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
            }
        }
        break;

    case UIEVENT_KEYDOWN:
        if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
            m_uButtonState |= UISTATE_PUSHED;
            Invalidate();
        }
        break;

    case UIEVENT_KEYUP:
        if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
            m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
            m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
        }
        break;

    default:
        CLabelUI::DoEvent(event);
        break;
    }
}
```

---

## 3. Notify 通知机制

### TNotifyUI 结构

```cpp
struct TNotifyUI {
    CDuiString sType;       // 通知类型字符串 "click", "textchanged" 等
    CDuiString sVirtualWnd; // 虚拟窗口名
    CControlUI* pSender;   // 发送控件
    DWORD dwTimestamp;      // 时间戳
    POINT ptMouse;          // 鼠标位置
    WPARAM wParam;
    LPARAM lParam;
};
```

### 常见通知类型

```cpp
// DUI_MSGTYPE 宏定义
#define DUI_MSGTYPE_CLICK          _T("click")
#define DUI_MSGTYPE_DBCLICK        _T("dbclick")
#define DUI_MSGTYPE_TEXTCHANGED    _T("textchanged")
#define DUI_MSGTYPE_RETURN         _T("return")
#define DUI_MSGTYPE_SCROLL         _T("scroll")
#define DUI_MSGTYPE_DROPDOWN       _T("dropdown")
#define DUI_MSGTYPE_SETFOCUS       _T("setfocus")
#define DUI_MSGTYPE_KILLFOCUS      _T("killfocus")
#define DUI_MSGTYPE_ITEMSELECT     _T("itemselect")
#define DUI_MSGTYPE_ITEMACTIVATE   _T("itemactivate")
#define DUI_MSGTYPE_ITEMCLICK      _T("itemclick")
#define DUI_MSGTYPE_TABSELECT      _T("tabselect")
#define DUI_MSGTYPE_VALUECHANGED   _T("valuechanged")
#define DUI_MSGTYPE_SELECTCHANGED  _T("selectchanged")
#define DUI_MSGTYPE_TIMER          _T("timer")
#define DUI_MSGTYPE_MENU           _T("menu")
#define DUI_MSGTYPE_WINDOWINIT     _T("windowinit")
```

### INotifyUI 接口

```cpp
// 接收通知的接口
class INotifyUI {
public:
    virtual void Notify(TNotifyUI& msg) = 0;
};

// 注册通知接收者
m_pm.AddNotifier(this);  // this 实现了 INotifyUI
```

### 通知分发流程

```cpp
// CPaintManagerUI::SendNotify
void CPaintManagerUI::SendNotify(CControlUI* pControl, LPCTSTR pstrMessage,
                                  WPARAM wParam, LPARAM lParam)
{
    TNotifyUI msg;
    msg.pSender = pControl;
    msg.sType = pstrMessage;
    msg.wParam = wParam;
    msg.lParam = lParam;
    msg.dwTimestamp = ::GetTickCount();
    msg.ptMouse = m_ptLastMousePos;

    // 1. 先调用控件自身的消息处理代理（CDelegate，可选）
    // （部分版本支持）

    // 2. 通知所有注册的 INotifyUI 监听者
    for (int i = 0; i < m_aNotifiers.GetSize(); i++) {
        INotifyUI* pNotify = static_cast<INotifyUI*>(m_aNotifiers[i]);
        pNotify->Notify(msg);
    }
}
```

---

## 4. 业务层通知处理

### 基本用法

```cpp
class CMainWnd : public CWindowWnd, public INotifyUI {
public:
    void Notify(TNotifyUI& msg) override {
        if (msg.sType == DUI_MSGTYPE_CLICK) {
            CDuiString name = msg.pSender->GetName();
            if (name == _T("btnLogin")) {
                OnLogin();
            } else if (name == _T("btnClose")) {
                Close();
            } else if (name == _T("btnMin")) {
                SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
            }
        }
        else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED) {
            if (msg.pSender->GetName() == _T("editSearch")) {
                OnSearchTextChanged();
            }
        }
        else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
            if (msg.pSender->GetName() == _T("listUsers")) {
                int index = (int)msg.wParam;
                OnUserSelected(index);
            }
        }
    }
};
```

### 使用宏简化分发（部分版本）

```cpp
// 某些 Duilib 分支提供消息映射宏
DUI_BEGIN_MESSAGE_MAP(CMainWnd, CWindowWnd)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_TEXTCHANGED, OnTextChanged)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_END_MESSAGE_MAP()

void CMainWnd::OnClick(TNotifyUI& msg) {
    if (msg.pSender->GetName() == _T("btnLogin")) {
        OnLogin();
    }
}
```

---

## 5. 虚拟窗口（VirtualWnd）

### 解决大窗口中的通知路由问题

```cpp
// 问题：一个复杂窗口有多个功能模块，所有通知都涌入一个 Notify
// 解决：使用 VirtualWnd 将控件分组

// XML 中指定虚拟窗口
// <Button name="btnSend" virtualwnd="ChatPanel" />
// <Button name="btnAdd" virtualwnd="ContactPanel" />

// 注册虚拟窗口处理器
class CChatPanel : public INotifyUI {
public:
    void Notify(TNotifyUI& msg) override {
        if (msg.sType == DUI_MSGTYPE_CLICK) {
            if (msg.pSender->GetName() == _T("btnSend")) {
                SendMessage();
            }
        }
    }
};

// 主窗口转发
void CMainWnd::Notify(TNotifyUI& msg) {
    if (!msg.sVirtualWnd.IsEmpty()) {
        // 根据 VirtualWnd 名称路由到子模块
        if (msg.sVirtualWnd == _T("ChatPanel")) {
            m_chatPanel.Notify(msg);
            return;
        }
    }
    // 主窗口自己的处理...
}
```

---

## 6. CDelegate 委托机制（早期版本）

```cpp
// 某些版本支持将事件直接绑定到成员函数
class CDelegate {
public:
    typedef void (CDelegateBase::*FnType)(TNotifyUI&);
    // ...
};

// 使用示例
CButtonUI* pBtn = FindControl(_T("btn"));
pBtn->OnNotify += MakeDelegate(this, &CMainWnd::OnBtnClick);

void CMainWnd::OnBtnClick(TNotifyUI& msg) {
    // 只有 pBtn 的通知会到这里
}
```

这种方式类似 C# 的事件委托，但在实际项目中使用较少，多数项目仍使用集中式 Notify。

---

## 7. DoEvent 与 Notify 的对比

| 维度 | DoEvent | Notify |
|------|---------|--------|
| 层级 | 底层，控件内部 | 上层，业务逻辑 |
| 参数 | TEventUI（原始输入） | TNotifyUI（语义化） |
| 处理者 | 控件自身 | 窗口类（INotifyUI） |
| 典型事件 | MOUSEENTER, BUTTONDOWN | click, textchanged |
| 触发方式 | PaintManager 调用控件的 DoEvent | 控件调用 PaintManager 的 SendNotify |
| 方向 | 从管理器到控件（自顶向下） | 从控件到管理器再到监听者（自底向上） |
| 用途 | 实现控件交互行为 | 响应业务操作 |

---

## 8. 事件传播与冒泡

### DoEvent 的向上传播

```cpp
// CControlUI::DoEvent 默认行为：未处理的事件传给父控件
void CControlUI::DoEvent(TEventUI& event)
{
    if (m_pParent) m_pParent->DoEvent(event);
}

// 子类决定是否消费事件
void CMyControl::DoEvent(TEventUI& event)
{
    if (event.Type == UIEVENT_BUTTONDOWN) {
        // 处理了，不再向上传播
        DoSomething();
        return;  // 不调用父类 DoEvent
    }
    // 其他事件继续向上冒泡
    CControlUI::DoEvent(event);
}
```

### Notify 的广播特性

```cpp
// Notify 不是冒泡，而是广播：所有注册的 INotifyUI 都会收到
// 注册多个监听者
m_pm.AddNotifier(&m_mainHandler);
m_pm.AddNotifier(&m_logHandler);    // 日志记录
m_pm.AddNotifier(&m_statsHandler);  // 统计埋点

// SendNotify 时，所有监听者都会被调用
// 顺序是注册顺序
```

---

## 9. 面试常见问题

### Q1: 为什么 Duilib 用字符串比较来分发通知？性能如何？

字符串比较确实比枚举/整数慢，但 Notify 触发频率远低于 DoEvent（只在用户操作时触发）。实际性能影响可忽略。好处是扩展性强——自定义控件可以定义任意通知类型字符串而无需修改框架枚举。

### Q2: 一个控件的 click 事件经历了哪些步骤？

```
1. 用户按下鼠标 → Windows 发送 WM_LBUTTONDOWN
2. CPaintManagerUI::MessageHandler 接收
3. FindControl(HitTest) 找到目标控件
4. 调用控件的 DoEvent(UIEVENT_BUTTONDOWN)
5. 控件记录 PUSHED 状态，Invalidate 重绘

6. 用户抬起鼠标 → Windows 发送 WM_LBUTTONUP
7. CPaintManagerUI 调用控件 DoEvent(UIEVENT_BUTTONUP)
8. 控件判断鼠标仍在范围内 → 调用 SendNotify(DUI_MSGTYPE_CLICK)
9. CPaintManagerUI 遍历所有 INotifyUI 监听者
10. 窗口类的 Notify 方法被调用，处理业务逻辑
```

### Q3: 如何阻止事件继续传播？

```cpp
// DoEvent 层面：不调用父类 DoEvent 即可阻止冒泡
void CMyControl::DoEvent(TEventUI& event) {
    if (event.Type == UIEVENT_BUTTONDOWN) {
        HandleClick();
        return;  // 事件被消费，不再冒泡
    }
    CControlUI::DoEvent(event);
}

// Notify 层面：无法阻止，所有监听者都会收到
// 如果需要阻止，可以在控件的 DoEvent 中不调用 SendNotify
```

### Q4: 多个窗口（多个 PaintManager）如何通信？

```cpp
// 方案1：通过全局单例中转
class MessageCenter {
public:
    static MessageCenter& Instance();
    void Subscribe(const CDuiString& type, std::function<void(WPARAM, LPARAM)> handler);
    void Publish(const CDuiString& type, WPARAM wParam, LPARAM lParam);
};

// 方案2：通过 Windows 消息 PostMessage/SendMessage
::PostMessage(hOtherWnd, WM_USER + 100, wParam, lParam);

// 方案3：直接持有对方 PaintManager 的指针（强耦合，不推荐）
```

---

## 10. 常见陷阱

```cpp
// ❌ 在 Notify 中用 == 比较字符串（CDuiString 未重载 ==）
void Notify(TNotifyUI& msg) {
    if (msg.sType == _T("click")) { ... }  // 取决于版本是否支持
}

// ✅ 安全写法
void Notify(TNotifyUI& msg) {
    if (_tcscmp(msg.sType, DUI_MSGTYPE_CLICK) == 0) { ... }
    // 或者使用 CDuiString 的比较方法
    if (msg.sType == DUI_MSGTYPE_CLICK) { ... }  // 多数版本 CDuiString 支持
}
```

```cpp
// ❌ 忘记 AddNotifier，永远收不到通知
void CMainWnd::Init() {
    CDialogBuilder builder;
    CControlUI* pRoot = builder.Create(_T("main.xml"), 0, this, &m_pm);
    m_pm.AttachDialog(pRoot);
    // 忘记了 m_pm.AddNotifier(this);
}

// ✅
void CMainWnd::Init() {
    // ...
    m_pm.AttachDialog(pRoot);
    m_pm.AddNotifier(this);  // 必须注册
}
```

```cpp
// ❌ 在 Notify 中删除触发通知的控件，导致悬空指针
void Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        m_pContainer->Remove(msg.pSender);  // 危险！
        // msg.pSender 已被 delete，后续访问 UB
    }
}

// ✅ 延迟删除：PostMessage 到自己，在下一轮消息中处理
void Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        ::PostMessage(m_hWnd, WM_USER + 1, (WPARAM)msg.pSender, 0);
    }
}
// 在 HandleMessage 中处理 WM_USER+1 时再安全删除
```

---

## 11. 总结

| 特性 | 说明 |
|------|------|
| **双层设计** | DoEvent 底层输入 + Notify 上层通知 |
| **DoEvent** | 控件内部消费，支持向上冒泡 |
| **Notify** | 语义化通知，广播给所有 INotifyUI 监听者 |
| **通知类型** | 字符串标识，可扩展 |
| **VirtualWnd** | 按模块分组路由通知 |
| **触发关系** | DoEvent 处理完后调用 SendNotify 发出通知 |
