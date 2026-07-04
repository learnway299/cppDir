# 控件体系

## 概述

Duilib 的控件体系以 CControlUI 为基类，通过继承构建出完整的控件层次。所有控件共享统一的属性系统、事件处理接口和绘制流程。理解控件继承关系和核心虚函数，是自定义控件开发的前提。

---

## 1. 控件继承层次

```
CControlUI（所有控件基类）
├── CLabelUI（标签）
│   ├── CTextUI（多行文本）
│   └── CButtonUI（按钮）
│       ├── COptionUI（单选/复选）
│       └── CCheckBoxUI
├── CProgressUI（进度条）
│   └── CSliderUI（滑块）
├── CEditUI（单行编辑框）
├── CRichEditUI（富文本编辑框）
├── CContainerUI（容器基类）
│   ├── CVerticalLayoutUI（垂直布局）
│   ├── CHorizontalLayoutUI（水平布局）
│   ├── CTileLayoutUI（瓦片布局）
│   ├── CTabLayoutUI（标签页布局）
│   ├── CListUI（列表）
│   │   ├── CListHeaderUI
│   │   └── CListElementUI
│   └── CComboUI（下拉框）
├── CScrollBarUI（滚动条）
└── CDialogLayoutUI（对话框布局）
```

---

## 2. CControlUI 基类核心成员

### 关键数据成员

```cpp
class CControlUI {
protected:
    CPaintManagerUI* m_pManager;    // 所属管理器
    CControlUI*      m_pParent;    // 父控件
    CDuiString       m_sName;      // 控件名称（唯一标识）
    CDuiString       m_sText;      // 文本内容

    RECT    m_rcItem;       // 控件在窗口中的绝对坐标
    RECT    m_rcPadding;    // 外边距
    SIZE    m_cxyFixed;     // 固定尺寸（0 表示自适应）
    SIZE    m_cxyMin;       // 最小尺寸
    SIZE    m_cxyMax;       // 最大尺寸

    bool    m_bVisible;     // 是否可见
    bool    m_bEnabled;     // 是否启用
    bool    m_bFocused;     // 是否获得焦点
    bool    m_bFloat;       // 是否浮动定位（脱离布局流）

    DWORD   m_dwBkColor;       // 背景色
    CDuiString m_sBkImage;     // 背景图
    CDuiString m_sBorderColor; // 边框色
    int     m_nBorderSize;     // 边框粗细

    CDuiString m_sUserData;    // 用户自定义数据
    UINT_PTR   m_pTag;         // 用户标签（指针或整数）
};
```

### 核心虚函数

```cpp
class CControlUI {
public:
    // --- 生命周期 ---
    virtual void Init();           // 控件被加入管理器后调用
    virtual void DoInit();         // 子类初始化入口
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit);

    // --- 尺寸与位置 ---
    virtual SIZE EstimateSize(SIZE szAvailable);  // 返回期望尺寸
    virtual void SetPos(RECT rc);                 // 设置最终位置

    // --- 绘制 ---
    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    // --- 事件处理 ---
    virtual void DoEvent(TEventUI& event);

    // --- 属性 ---
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    // --- 查找 ---
    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    // --- 接口查询 ---
    virtual LPVOID GetInterface(LPCTSTR pstrName);
};
```

---

## 3. 关键虚函数详解

### EstimateSize - 尺寸估算

```cpp
// 返回控件的期望尺寸
// szAvailable: 父容器提供的可用空间
SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    // 如果设置了固定宽高，直接返回
    return m_cxyFixed;
    // cx=0 表示宽度自适应，cy=0 表示高度自适应
}

// CButtonUI 可能根据文本长度计算
SIZE CButtonUI::EstimateSize(SIZE szAvailable)
{
    if (m_cxyFixed.cx > 0 && m_cxyFixed.cy > 0)
        return m_cxyFixed;

    // 根据文本内容计算需要的宽度
    SIZE szText = CalcTextSize();
    return { szText.cx + m_rcPadding.left + m_rcPadding.right,
             m_cxyFixed.cy > 0 ? m_cxyFixed.cy : szText.cy + 8 };
}
```

### SetPos - 位置设置

```cpp
void CControlUI::SetPos(RECT rc)
{
    m_rcItem = rc;
    // 基础控件到此为止
    // 容器控件会在此基础上递归设置子控件位置
}
```

### DoPaint - 绘制流程

```cpp
void CControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    // rcPaint 是脏区域，判断是否需要绘制
    RECT rcTemp;
    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

    // 分层绘制，子类可单独重写某一层
    PaintBkColor(hDC);      // 1. 背景色
    PaintBkImage(hDC);      // 2. 背景图
    PaintStatusImage(hDC);  // 3. 状态图（normal/hot/pushed/disabled）
    PaintText(hDC);         // 4. 文本
    PaintBorder(hDC);       // 5. 边框
}
```

### DoEvent - 事件处理

```cpp
void CControlUI::DoEvent(TEventUI& event)
{
    switch (event.Type) {
    case UIEVENT_MOUSEENTER:
        m_bHot = true;
        Invalidate();  // 重绘（切换到 hot 状态）
        break;
    case UIEVENT_MOUSELEAVE:
        m_bHot = false;
        Invalidate();
        break;
    case UIEVENT_SETFOCUS:
        m_bFocused = true;
        Invalidate();
        break;
    case UIEVENT_KILLFOCUS:
        m_bFocused = false;
        Invalidate();
        break;
    }
    // 未处理的事件交给父控件
    if (m_pParent) m_pParent->DoEvent(event);
}
```

---

## 4. 典型控件实现分析

### CButtonUI

```cpp
class CButtonUI : public CLabelUI {
public:
    // 按钮有多种状态图
    void PaintStatusImage(HDC hDC) override {
        if (!IsEnabled()) {
            DrawImage(hDC, m_sDisabledImage);
        } else if (m_bPushed) {
            DrawImage(hDC, m_sPushedImage);
        } else if (m_bHot) {
            DrawImage(hDC, m_sHotImage);
        } else if (m_bFocused) {
            DrawImage(hDC, m_sFocusedImage);
        } else {
            DrawImage(hDC, m_sNormalImage);
        }
    }

    void DoEvent(TEventUI& event) override {
        if (event.Type == UIEVENT_BUTTONDOWN) {
            m_bPushed = true;
            Invalidate();
        }
        else if (event.Type == UIEVENT_BUTTONUP) {
            if (m_bPushed) {
                m_bPushed = false;
                Invalidate();
                // 触发点击通知
                m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
            }
        }
        CLabelUI::DoEvent(event);
    }

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override {
        if (_tcscmp(pstrName, _T("normalimage")) == 0)
            m_sNormalImage = pstrValue;
        else if (_tcscmp(pstrName, _T("hotimage")) == 0)
            m_sHotImage = pstrValue;
        else if (_tcscmp(pstrName, _T("pushedimage")) == 0)
            m_sPushedImage = pstrValue;
        else if (_tcscmp(pstrName, _T("disabledimage")) == 0)
            m_sDisabledImage = pstrValue;
        else
            CLabelUI::SetAttribute(pstrName, pstrValue);
    }

protected:
    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sPushedImage;
    CDuiString m_sDisabledImage;
    bool m_bPushed = false;
    bool m_bHot = false;
};
```

### CContainerUI

```cpp
class CContainerUI : public CControlUI, public IContainerUI {
public:
    // 容器重写 DoPaint，递归绘制子控件
    void DoPaint(HDC hDC, const RECT& rcPaint) override {
        // 先绘制自身背景
        CControlUI::DoPaint(hDC, rcPaint);

        // 设置裁剪区域（子控件不能画出容器范围）
        RECT rcClip = m_rcItem;
        HRGN hRgn = ::CreateRectRgnIndirect(&rcClip);
        ::SelectClipRgn(hDC, hRgn);

        // 递归绘制子控件
        for (int i = 0; i < m_items.GetSize(); i++) {
            CControlUI* pChild = (CControlUI*)m_items[i];
            if (!pChild->IsVisible()) continue;

            // 只绘制与脏区域相交的子控件
            RECT rcTemp;
            if (::IntersectRect(&rcTemp, &rcPaint, &pChild->GetPos())) {
                pChild->DoPaint(hDC, rcPaint);
            }
        }

        ::SelectClipRgn(hDC, NULL);
        ::DeleteObject(hRgn);
    }

    // IContainerUI 接口
    bool Add(CControlUI* pControl) override {
        pControl->SetManager(m_pManager, this, true);
        m_items.Add(pControl);
        m_pManager->NeedUpdate();
        return true;
    }

    bool Remove(CControlUI* pControl) override {
        for (int i = 0; i < m_items.GetSize(); i++) {
            if ((CControlUI*)m_items[i] == pControl) {
                m_items.Remove(i);
                m_pManager->NeedUpdate();
                delete pControl;
                return true;
            }
        }
        return false;
    }

protected:
    CStdPtrArray m_items;  // 子控件数组
};
```

---

## 5. 控件状态机

### 按钮状态转换

```
                     MouseEnter
    ┌──────────┐  ──────────────►  ┌──────────┐
    │  Normal  │                    │   Hot    │
    └──────────┘  ◄──────────────  └──────────┘
                     MouseLeave          │
                                   ButtonDown
                                         │
                                         ▼
                                    ┌──────────┐
                                    │  Pushed  │
                                    └──────────┘
                                         │
                                    ButtonUp
                                         │
                                         ▼
                                   SendNotify(click)
                                   → 回到 Hot 状态

    全局状态：Disabled（SetEnabled(false) 时，忽略所有交互）
```

### 状态对应的图片

```xml
<Button name="btn"
    normalimage="btn_normal.png"
    hotimage="btn_hover.png"
    pushedimage="btn_pressed.png"
    focusedimage="btn_focused.png"
    disabledimage="btn_disabled.png"
/>
```

---

## 6. GetInterface 接口查询

```cpp
// 类似 COM 的 QueryInterface，用于安全类型转换
LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, _T("Control")) == 0) return this;
    return NULL;
}

LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, _T("IContainer")) == 0)
        return static_cast<IContainerUI*>(this);
    if (_tcscmp(pstrName, _T("Container")) == 0)
        return this;
    return CControlUI::GetInterface(pstrName);
}

// 使用：安全地判断控件是否是容器
IContainerUI* pContainer = (IContainerUI*)pControl->GetInterface(_T("IContainer"));
if (pContainer) {
    pContainer->Add(pNewChild);
}
```

**面试题：为什么不用 dynamic_cast 而用 GetInterface？**

Duilib 早期设计时考虑到：
1. 有些编译器/配置关闭了 RTTI
2. dynamic_cast 有运行时开销
3. GetInterface 可以暴露任意接口（不限于继承层次），更灵活

---

## 7. 面试常见问题

### Q1: CControlUI 的 Invalidate 做了什么？

```cpp
void CControlUI::Invalidate()
{
    // 不是整个窗口重绘，只标记控件所在区域为脏
    if (m_pManager) {
        RECT rc = m_rcItem;
        m_pManager->Invalidate(rc);
        // 内部调用 ::InvalidateRect(m_hWnd, &rc, FALSE)
    }
}
```

只使控件自身矩形区域失效，触发局部重绘，避免全窗口重绘的性能开销。

### Q2: Float 控件和普通控件的区别？

```xml
<!-- 普通控件：参与布局流，由父容器的 SetPos 计算位置 -->
<Button name="btn1" width="80" height="30" />

<!-- 浮动控件：脱离布局流，使用绝对坐标定位 -->
<Button name="btnFloat" float="true" pos="100,200,180,230" />
```

Float 控件不参与父容器的自动布局计算，位置由 `pos` 属性直接指定。适用于需要固定位置的叠加层控件（如关闭按钮、水印）。

### Q3: 控件的 m_pTag 和 m_sUserData 有什么区别？

| | m_pTag | m_sUserData |
|---|---|---|
| 类型 | UINT_PTR（指针或整数） | CDuiString（字符串） |
| 用途 | 关联自定义对象指针 | 存储字符串形式的附加数据 |
| 来源 | 代码中 SetTag() | XML 中 `userdata="..."` 或代码 |
| 典型场景 | 列表项关联数据模型指针 | 传递简单标识字符串 |

### Q4: 为什么按钮继承自 Label 而不是直接继承 Control？

因为按钮本质上就是带文本 + 带交互的 Label。CLabelUI 已经实现了文本绘制（字体、颜色、对齐）和图标显示，CButtonUI 只需在此基础上增加状态切换和点击事件，避免重复代码。

---

## 8. 常见陷阱

```cpp
// ❌ static_cast 不安全，可能类型不匹配
CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("myLabel")));
pBtn->SetPushedImage(...);  // 如果实际是 Label，行为未定义

// ✅ 先用 GetInterface 或 GetClass 检查类型
CControlUI* pCtrl = m_pm.FindControl(_T("myControl"));
if (_tcscmp(pCtrl->GetClass(), _T("ButtonUI")) == 0) {
    CButtonUI* pBtn = static_cast<CButtonUI*>(pCtrl);
    pBtn->SetPushedImage(...);
}
```

```cpp
// ❌ 忘记调用父类的 DoEvent，导致基础行为丢失
void CMyControl::DoEvent(TEventUI& event) {
    if (event.Type == UIEVENT_BUTTONDOWN) {
        // 自定义处理...
        return;  // 没有调用父类
    }
}

// ✅ 未处理的事件传递给父类
void CMyControl::DoEvent(TEventUI& event) {
    if (event.Type == UIEVENT_BUTTONDOWN) {
        // 自定义处理...
    }
    CControlUI::DoEvent(event);  // 父类处理默认行为
}
```

---

## 9. 总结

| 特性 | 说明 |
|------|------|
| **基类** | CControlUI，所有控件的公共基类 |
| **继承体系** | 单继承为主，接口通过 GetInterface 查询 |
| **核心虚函数** | EstimateSize / SetPos / DoPaint / DoEvent / SetAttribute |
| **绘制分层** | BkColor → BkImage → StatusImage → Text → Border |
| **状态管理** | Normal / Hot / Pushed / Focused / Disabled |
| **容器** | CContainerUI 管理子控件，递归绘制和布局 |
| **局部重绘** | Invalidate 只标记控件区域为脏 |
