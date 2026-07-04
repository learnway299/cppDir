# 消息循环与窗口机制

## 概述

Duilib 的窗口机制建立在 Win32 API 之上。CWindowWnd 封装了窗口注册、创建和消息回调，而 CPaintManagerUI 负责将 Windows 消息转化为控件可处理的事件。理解消息从系统到控件的完整流转路径，是掌握 Duilib 的关键。

---

## 1. Windows 消息循环基础

### Win32 消息泵

```cpp
// 标准 Win32 消息循环
MSG msg = { 0 };
while (::GetMessage(&msg, NULL, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);  // 分发到 WndProc
}
```

### Duilib 的消息循环封装

```cpp
// CPaintManagerUI::MessageLoop 实现（简化）
void CPaintManagerUI::MessageLoop()
{
    MSG msg = { 0 };
    while (::GetMessage(&msg, NULL, 0, 0)) {
        if (!CPaintManagerUI::TranslateMessage(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

// TranslateMessage - 给所有 PaintManager 实例预处理消息的机会
bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
{
    HWND hwndParent = ::GetParent(pMsg->hwnd);
    for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
        CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
        if (pMsg->hwnd == pT->GetPaintWindow()
            || hwndParent == pT->GetPaintWindow()) {
            if (pT->PreMessageHandler(pMsg->message, pMsg->wParam,
                                       pMsg->lParam, lRes)) {
                return true;
            }
        }
    }
    return false;
}
```

**面试题：为什么 Duilib 需要 TranslateMessage 预处理？**

因为 DirectUI 控件没有自己的 HWND，无法通过系统的 IsDialogMessage 处理 Tab 切换、快捷键等。Duilib 在消息循环层拦截这些消息，手动实现焦点导航和加速键功能。

---

## 2. CWindowWnd 窗口基类

### 窗口注册与创建

```cpp
class CWindowWnd {
public:
    virtual LPCTSTR GetWindowClassName() const = 0;

    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle,
                DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void OnFinalMessage(HWND hWnd);

protected:
    HWND m_hWnd;
};
```

### 静态 WndProc 与实例绑定

```cpp
LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg,
                                        WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA,
                           reinterpret_cast<LPARAM>(pThis));
    } else {
        pThis = reinterpret_cast<CWindowWnd*>(
            ::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
```

**面试题：Duilib 如何将静态 WndProc 关联到具体的窗口实例？**

通过 `WM_NCCREATE` 消息。创建窗口时将 `this` 指针作为 `CreateWindowEx` 的最后一个参数传入，系统在 `WM_NCCREATE` 时通过 `CREATESTRUCT::lpCreateParams` 传回，框架将其存入 `GWLP_USERDATA`。后续消息通过 `GetWindowLongPtr` 取回实例指针，调用虚函数 `HandleMessage`。

---

## 3. CPaintManagerUI 的消息处理

### MessageHandler 核心流程

```cpp
bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam,
                                      LPARAM lParam, LRESULT& lRes)
{
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps = { 0 };
        HDC hDC = ::BeginPaint(m_hWndPaint, &ps);
        if (m_bUpdateNeeded) {
            m_bUpdateNeeded = false;
            RECT rcClient;
            ::GetClientRect(m_hWndPaint, &rcClient);
            m_pRoot->SetPos(rcClient);
        }
        // 绘制到离屏 DC，再 BitBlt 到窗口
        CRenderEngine::DrawImage(m_hDcOffscreen, ...);
        ::BitBlt(hDC, ...);
        ::EndPaint(m_hWndPaint, &ps);
        return true;
    }

    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        CControlUI* pNewHover = FindControl(pt);
        if (pNewHover != m_pEventHover) {
            if (m_pEventHover) {
                TEventUI event = { UIEVENT_MOUSELEAVE };
                m_pEventHover->DoEvent(event);
            }
            m_pEventHover = pNewHover;
            if (m_pEventHover) {
                TEventUI event = { UIEVENT_MOUSEENTER };
                m_pEventHover->DoEvent(event);
            }
        }
        if (m_pEventHover) {
            TEventUI event = { UIEVENT_MOUSEMOVE, wParam, lParam, pt };
            m_pEventHover->DoEvent(event);
        }
        return true;
    }

    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        CControlUI* pControl = FindControl(pt);
        if (pControl) {
            m_pEventClick = pControl;
            pControl->SetFocus();
            TEventUI event = { UIEVENT_BUTTONDOWN, wParam, lParam, pt };
            pControl->DoEvent(event);
        }
        return true;
    }

    case WM_LBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        TEventUI event = { UIEVENT_BUTTONUP, wParam, lParam, pt };
        if (m_pEventClick) {
            m_pEventClick->DoEvent(event);
        }
        m_pEventClick = NULL;
        return true;
    }

    case WM_KEYDOWN: {
        if (m_pFocus) {
            TEventUI event = { UIEVENT_KEYDOWN, wParam, lParam };
            m_pFocus->DoEvent(event);
        }
        return true;
    }

    case WM_SIZE: {
        m_bUpdateNeeded = true;
        ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        return true;
    }

    case WM_TIMER: {
        for (int i = 0; i < m_aTimers.GetSize(); i++) {
            const TIMERINFO* pTimer = (TIMERINFO*)m_aTimers[i];
            if (pTimer->uWinTimer == LOWORD(wParam)) {
                TEventUI event = { UIEVENT_TIMER, pTimer->nLocalID };
                pTimer->pSender->DoEvent(event);
                break;
            }
        }
        return true;
    }
    }
    return false;
}
```

---

## 4. 消息流转全景图

```
Windows 消息泵
    │
    ▼
CPaintManagerUI::TranslateMessage()  ← 预处理（Tab/快捷键）
    │ (未处理)
    ▼
DispatchMessage() → WndProc
    │
    ▼
CWindowWnd::HandleMessage()
    │
    ▼
CPaintManagerUI::MessageHandler()
    │
    ├── WM_PAINT      → 布局计算 + 双缓冲绘制
    ├── WM_MOUSEMOVE  → FindControl(HitTest) → DoEvent(MOUSEMOVE)
    ├── WM_LBUTTONDOWN→ FindControl → SetFocus + DoEvent(BUTTONDOWN)
    ├── WM_LBUTTONUP  → DoEvent(BUTTONUP) → 触发 Notify(click)
    ├── WM_KEYDOWN    → m_pFocus → DoEvent(KEYDOWN)
    ├── WM_SIZE       → 标记 m_bUpdateNeeded → InvalidateRect
    ├── WM_TIMER      → 查表 → DoEvent(TIMER)
    └── 其他          → DefWindowProc
```

---

## 5. FindControl 与 HitTest

### 坐标查找控件

```cpp
CControlUI* CPaintManagerUI::FindControl(POINT pt) const
{
    return m_pRoot->FindControl(__FindControlFromPoint, &pt,
                                UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

static CControlUI* __FindControlFromPoint(CControlUI* pThis, LPVOID pData)
{
    POINT* pPoint = static_cast<POINT*>(pData);
    RECT rc = pThis->GetPos();
    if (::PtInRect(&rc, *pPoint)) {
        return pThis;
    }
    return NULL;
}
```

### 容器的 FindControl 递归

```cpp
CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    // 逆序遍历：后添加的控件在上层，优先命中
    for (int i = m_items.GetSize() - 1; i >= 0; i--) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[i]);
        CControlUI* pResult = pControl->FindControl(Proc, pData, uFlags);
        if (pResult) return pResult;
    }
    return NULL;
}
```

**面试题：为什么 FindControl 是逆序遍历？**

因为 XML 中后声明的控件在视觉上位于上层（Z-Order 更高）。逆序遍历确保鼠标点击时先命中上层控件，符合用户视觉预期。

---

## 6. 焦点管理

### 焦点切换机制

```cpp
void CPaintManagerUI::SetFocus(CControlUI* pControl)
{
    if (m_pFocus == pControl) return;

    if (m_pFocus) {
        TEventUI event = { UIEVENT_KILLFOCUS };
        m_pFocus->DoEvent(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
    }

    m_pFocus = pControl;

    if (m_pFocus) {
        TEventUI event = { UIEVENT_SETFOCUS };
        m_pFocus->DoEvent(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_SETFOCUS);
    }
}
```

### Tab 键焦点导航

```cpp
bool CPaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam,
                                         LPARAM lParam, LRESULT& lRes)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_TAB) {
        bool bForward = !(::GetKeyState(VK_SHIFT) & 0x8000);
        CControlUI* pNext = GetNextTabControl(bForward);
        if (pNext) {
            SetFocus(pNext);
            return true;
        }
    }
    return false;
}
```

---

## 7. 定时器机制

```cpp
bool CPaintManagerUI::SetTimer(CControlUI* pControl, UINT nTimerID,
                                UINT uElapse)
{
    UINT uWinTimer = (UINT)::SetTimer(m_hWndPaint, m_uTimerID++, uElapse, NULL);

    TIMERINFO info;
    info.pSender = pControl;
    info.nLocalID = nTimerID;
    info.uWinTimer = uWinTimer;
    m_aTimers.Add(&info);
    return true;
}
```

**面试题：Duilib 定时器和 Win32 SetTimer 的关系？**

Duilib 在 Win32 SetTimer 之上做了一层封装。每个控件可以设置多个逻辑定时器（用 nLocalID 区分），内部映射到一个 Windows 定时器，通过映射表将 WM_TIMER 路由到正确的控件。控件销毁时框架自动 KillTimer 清理。

---

## 8. 面试常见问题

### Q1: HandleMessage 和 MessageHandler 的区别？

| | HandleMessage | MessageHandler |
|---|---|---|
| 所属类 | CWindowWnd | CPaintManagerUI |
| 职责 | 窗口级消息（创建/销毁/非客户区） | UI 消息（绘制/鼠标/键盘） |
| 调用关系 | 内部调用 MessageHandler | 被 HandleMessage 调用 |
| 典型消息 | WM_CREATE, WM_DESTROY, WM_NCHITTEST | WM_PAINT, WM_MOUSEMOVE |

### Q2: Duilib 如何实现无标题栏窗口拖动？

```cpp
// 响应 WM_NCHITTEST，让客户区返回 HTCAPTION
LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_NCHITTEST) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ScreenToClient(m_hWnd, &pt);
        RECT rcCaption = { 0, 0, clientWidth, 30 };
        if (::PtInRect(&rcCaption, pt)) {
            return HTCAPTION;
        }
    }
    // ...
}
```

### Q3: 为什么键盘消息发给焦点控件，而鼠标消息需要 HitTest？

键盘输入在逻辑上只能有一个接收者（当前焦点），和系统行为一致。鼠标消息的目标取决于光标位置，由于 DirectUI 控件没有 HWND，系统无法自动路由，必须由框架根据坐标做 HitTest 确定目标。

### Q4: Duilib 如何处理鼠标捕获（Capture）？

```cpp
void CPaintManagerUI::SetCapture()
{
    ::SetCapture(m_hWndPaint);
    m_bMouseCapture = true;
}

void CPaintManagerUI::ReleaseCapture()
{
    ::ReleaseCapture();
    m_bMouseCapture = false;
}
// 捕获期间所有鼠标消息都发给 m_pEventClick，不再做 HitTest
```

---

## 9. 总结

| 特性 | 说明 |
|------|------|
| **消息循环** | 标准 Win32 消息泵 + TranslateMessage 预处理 |
| **窗口绑定** | WM_NCCREATE + GWLP_USERDATA 关联实例 |
| **消息分层** | CWindowWnd 处理窗口级，CPaintManagerUI 处理 UI 级 |
| **鼠标路由** | FindControl HitTest，逆序遍历（Z-Order） |
| **键盘路由** | 直接发给焦点控件 m_pFocus |
| **焦点管理** | 框架维护，支持 Tab 导航 |
| **定时器** | Win32 SetTimer 封装，映射表路由到控件 |
