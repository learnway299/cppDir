# 自定义控件开发

## 概述

Duilib 内置控件无法覆盖所有业务需求，自定义控件开发是实际项目中的常见任务。开发流程是：继承合适的基类 → 重写关键虚函数 → 注册到控件工厂 → 在 XML 中使用。本节通过完整示例讲解整个过程。

---

## 1. 开发步骤总览

```
1. 选择基类（CControlUI / CLabelUI / CContainerUI 等）
2. 重写虚函数
   ├── GetClass / GetInterface  ← 类型标识
   ├── EstimateSize             ← 尺寸计算
   ├── SetPos                   ← 位置（容器类）
   ├── DoEvent                  ← 交互行为
   ├── DoPaint / PaintXxx       ← 自定义绘制
   └── SetAttribute             ← 自定义属性解析
3. 注册控件工厂（IDialogBuilderCallback）
4. 在 XML 中使用自定义标签
```

---

## 2. 完整示例：圆形进度条

### 头文件

```cpp
// CCircleProgressUI.h
#pragma once
#include "UIlib.h"

class CCircleProgressUI : public CControlUI {
public:
    CCircleProgressUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;

    void SetValue(int nValue);
    int GetValue() const;
    void SetMaxValue(int nMax);
    void SetLineWidth(int nWidth);
    void SetForeColor(DWORD dwColor);

    SIZE EstimateSize(SIZE szAvailable) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    void PaintBkImage(HDC hDC) override;
    void DoEvent(TEventUI& event) override;

protected:
    int   m_nValue;       // 当前值
    int   m_nMaxValue;    // 最大值
    int   m_nLineWidth;   // 圆弧线宽
    DWORD m_dwForeColor;  // 前景色（进度条颜色）
    DWORD m_dwBgRingColor;// 背景圆环色
};
```

### 实现文件

```cpp
// CCircleProgressUI.cpp
#include "CCircleProgressUI.h"
#include <GdiPlus.h>

CCircleProgressUI::CCircleProgressUI()
    : m_nValue(0)
    , m_nMaxValue(100)
    , m_nLineWidth(4)
    , m_dwForeColor(0xFF1890FF)
    , m_dwBgRingColor(0xFFE8E8E8)
{
}

LPCTSTR CCircleProgressUI::GetClass() const
{
    return _T("CircleProgressUI");
}

LPVOID CCircleProgressUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, _T("CircleProgress")) == 0)
        return this;
    return CControlUI::GetInterface(pstrName);
}

void CCircleProgressUI::SetValue(int nValue)
{
    if (nValue < 0) nValue = 0;
    if (nValue > m_nMaxValue) nValue = m_nMaxValue;
    if (m_nValue != nValue) {
        m_nValue = nValue;
        Invalidate();
    }
}

int CCircleProgressUI::GetValue() const { return m_nValue; }
void CCircleProgressUI::SetMaxValue(int nMax) { m_nMaxValue = nMax; }
void CCircleProgressUI::SetLineWidth(int nWidth) { m_nLineWidth = nWidth; }
void CCircleProgressUI::SetForeColor(DWORD dwColor) { m_dwForeColor = dwColor; }

SIZE CCircleProgressUI::EstimateSize(SIZE szAvailable)
{
    // 如果设置了固定尺寸就用固定尺寸，否则默认 64x64
    return { m_cxyFixed.cx > 0 ? m_cxyFixed.cx : 64,
             m_cxyFixed.cy > 0 ? m_cxyFixed.cy : 64 };
}

void CCircleProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("value")) == 0)
        SetValue(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("maxvalue")) == 0)
        SetMaxValue(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("linewidth")) == 0)
        SetLineWidth(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("forecolor")) == 0)
        m_dwForeColor = ParseColor(pstrValue);
    else if (_tcscmp(pstrName, _T("bgringcolor")) == 0)
        m_dwBgRingColor = ParseColor(pstrValue);
    else
        CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCircleProgressUI::PaintBkImage(HDC hDC)
{
    // 使用 GDI+ 绘制圆形进度
    Gdiplus::Graphics graphics(hDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

    int cx = m_rcItem.right - m_rcItem.left;
    int cy = m_rcItem.bottom - m_rcItem.top;
    int diameter = min(cx, cy) - m_nLineWidth;
    int x = m_rcItem.left + (cx - diameter) / 2;
    int y = m_rcItem.top + (cy - diameter) / 2;

    Gdiplus::RectF rcCircle((float)x, (float)y, (float)diameter, (float)diameter);

    // 背景圆环
    Gdiplus::Pen penBg(Gdiplus::Color(m_dwBgRingColor), (float)m_nLineWidth);
    graphics.DrawEllipse(&penBg, rcCircle);

    // 前景弧线（进度）
    if (m_nValue > 0) {
        float sweepAngle = 360.0f * m_nValue / m_nMaxValue;
        Gdiplus::Pen penFore(Gdiplus::Color(m_dwForeColor), (float)m_nLineWidth);
        penFore.SetStartCap(Gdiplus::LineCapRound);
        penFore.SetEndCap(Gdiplus::LineCapRound);
        graphics.DrawArc(&penFore, rcCircle, -90.0f, sweepAngle);
    }

    // 中心文字（百分比）
    CDuiString sText;
    sText.Format(_T("%d%%"), m_nValue * 100 / m_nMaxValue);

    Gdiplus::Font font(L"Microsoft YaHei", 14.0f);
    Gdiplus::SolidBrush brush(Gdiplus::Color(m_dwForeColor));
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    graphics.DrawString(sText, -1, &font, rcCircle, &fmt, &brush);
}

void CCircleProgressUI::DoEvent(TEventUI& event)
{
    // 可选：响应点击事件
    if (event.Type == UIEVENT_BUTTONUP) {
        if (::PtInRect(&m_rcItem, event.ptMouse)) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
        }
        return;
    }
    CControlUI::DoEvent(event);
}
```

### 注册与使用

```cpp
// 窗口类中注册
CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
    if (_tcscmp(pstrClass, _T("CircleProgress")) == 0)
        return new CCircleProgressUI;
    return NULL;
}

// XML 中使用
// <CircleProgress name="progress1" width="80" height="80"
//     value="75" maxvalue="100" linewidth="6"
//     forecolor="#FF1890FF" bgringcolor="#FFE8E8E8" />
```

---

## 3. 选择基类的原则

| 需求 | 推荐基类 | 原因 |
|------|---------|------|
| 纯展示/简单交互 | CControlUI | 最轻量，无额外开销 |
| 需要显示文本 | CLabelUI | 已有文本绘制逻辑 |
| 按钮类交互 | CButtonUI | 已有状态机和状态图 |
| 需要包含子控件 | CContainerUI | 已有子控件管理和递归绘制 |
| 带滚动的容器 | CContainerUI + IScrollBarHost | 支持滚动条 |
| 列表项 | CListElementUI | 已有选中/高亮/数据绑定 |

---

## 4. 必须重写的虚函数

### GetClass / GetInterface

```cpp
// GetClass 返回类名字符串，用于调试和类型判断
LPCTSTR CMyControl::GetClass() const
{
    return _T("MyControlUI");
}

// GetInterface 用于安全的类型查询
LPVOID CMyControl::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, _T("MyControl")) == 0)
        return this;
    return CControlUI::GetInterface(pstrName);
}
```

### SetAttribute

```cpp
// 解析 XML 中的自定义属性
void CMyControl::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("myprop")) == 0) {
        m_myProp = _ttoi(pstrValue);
    }
    else {
        // 未识别的属性交给父类处理
        CControlUI::SetAttribute(pstrName, pstrValue);
    }
}
```

---

## 5. 复合控件开发

### 示例：搜索框（输入框 + 清除按钮 + 搜索图标）

```cpp
class CSearchBoxUI : public CHorizontalLayoutUI {
public:
    CSearchBoxUI() : m_pEdit(nullptr), m_pBtnClear(nullptr) {}

    LPCTSTR GetClass() const override { return _T("SearchBoxUI"); }

    void DoInit() override {
        // 控件加入管理器后，动态创建子控件
        // 搜索图标
        CControlUI* pIcon = new CControlUI;
        pIcon->SetFixedWidth(24);
        pIcon->SetFixedHeight(24);
        pIcon->SetBkImage(_T("icon_search.png"));
        Add(pIcon);

        // 编辑框
        m_pEdit = new CEditUI;
        m_pEdit->SetName(_T("searchEdit"));
        Add(m_pEdit);  // 宽度自适应

        // 清除按钮
        m_pBtnClear = new CButtonUI;
        m_pBtnClear->SetName(_T("btnClear"));
        m_pBtnClear->SetFixedWidth(24);
        m_pBtnClear->SetFixedHeight(24);
        m_pBtnClear->SetNormalImage(_T("icon_clear.png"));
        m_pBtnClear->SetVisible(false);
        Add(m_pBtnClear);
    }

    // 监听子控件通知
    void DoEvent(TEventUI& event) override {
        CHorizontalLayoutUI::DoEvent(event);
    }

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override {
        if (_tcscmp(pstrName, _T("placeholder")) == 0) {
            m_sPlaceholder = pstrValue;
        } else {
            CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
        }
    }

    CEditUI* GetEdit() const { return m_pEdit; }

protected:
    CEditUI*   m_pEdit;
    CButtonUI* m_pBtnClear;
    CDuiString m_sPlaceholder;
};
```

---

## 6. 动画支持

### 基于定时器的简单动画

```cpp
class CAnimButtonUI : public CButtonUI {
public:
    void DoEvent(TEventUI& event) override {
        if (event.Type == UIEVENT_MOUSEENTER) {
            m_nAlpha = 0;
            m_pManager->SetTimer(this, TIMER_FADE_IN, 16);  // ~60fps
        }
        else if (event.Type == UIEVENT_MOUSELEAVE) {
            m_pManager->KillTimer(this, TIMER_FADE_IN);
            m_nAlpha = 255;
            Invalidate();
        }
        else if (event.Type == UIEVENT_TIMER) {
            if (event.wParam == TIMER_FADE_IN) {
                m_nAlpha += 25;
                if (m_nAlpha >= 255) {
                    m_nAlpha = 255;
                    m_pManager->KillTimer(this, TIMER_FADE_IN);
                }
                Invalidate();
            }
        }
        CButtonUI::DoEvent(event);
    }

    void PaintStatusImage(HDC hDC) override {
        // 使用 m_nAlpha 控制 hot 图片的透明度
        if (m_nAlpha > 0 && m_nAlpha < 255) {
            // 先画 normal，再叠加半透明 hot
            DrawImage(hDC, m_sNormalImage);
            DrawImageWithAlpha(hDC, m_sHotImage, m_nAlpha);
        } else {
            CButtonUI::PaintStatusImage(hDC);
        }
    }

private:
    enum { TIMER_FADE_IN = 1 };
    int m_nAlpha = 255;
};
```

---

## 7. 面试常见问题

### Q1: 自定义控件如何接收来自其他控件的通知？

自定义控件本身不直接接收 Notify（Notify 发给 INotifyUI 监听者）。如果需要：
- 方案 A：在窗口的 Notify 中转发给控件的方法
- 方案 B：让自定义控件也实现 INotifyUI 并注册为监听者
- 方案 C：复合控件内部直接持有子控件指针，在 DoEvent 中处理

### Q2: DoInit 和构造函数的区别？

| | 构造函数 | DoInit |
|---|---|---|
| 调用时机 | new 时 | 被 Add 到容器并关联 Manager 后 |
| m_pManager | NULL | 已设置 |
| 适合做什么 | 初始化成员变量默认值 | 创建子控件、设置定时器、访问 Manager |

### Q3: 为什么 PaintBkImage 里绘制而不是 DoPaint？

重写整个 DoPaint 需要处理脏区域判断和所有绘制层。重写 PaintBkImage 只替换某一层的绘制逻辑，其他层（背景色、边框、文本）仍由基类处理，更安全。

### Q4: 自定义控件的生命周期管理？

```
new → SetAttribute (XML 属性) → Add 到容器 → SetManager → DoInit
                                                              ↓
运行中：DoEvent / DoPaint ← 反复调用
                                                              ↓
容器 Remove 或窗口关闭 → delete（由容器负责 delete 子控件）
```

控件的内存由其父容器管理。RemoveAll/Remove 时会 delete 子控件。不要手动 delete 已加入容器的控件。

---

## 8. 常见陷阱

```cpp
// ❌ 在构造函数中访问 m_pManager（此时为 NULL）
CMyControl::CMyControl() {
    m_pManager->SetTimer(this, 1, 1000);  // 崩溃！
}

// ✅ 在 DoInit 中初始化
void CMyControl::DoInit() {
    m_pManager->SetTimer(this, 1, 1000);  // 安全
}
```

```cpp
// ❌ 重写 SetAttribute 但不调用父类处理未知属性
void CMyControl::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
    if (_tcscmp(pstrName, _T("myprop")) == 0) {
        m_myProp = _ttoi(pstrValue);
    }
    // 缺少 else CControlUI::SetAttribute(pstrName, pstrValue);
    // 导致 width/height/name 等基础属性全部失效
}
```

```cpp
// ❌ 自定义绘制后不考虑脏区域
void CMyControl::DoPaint(HDC hDC, const RECT& rcPaint) {
    // 直接绘制，不检查是否在脏区域内
    ExpensiveDrawing(hDC);
}

// ✅ 先判断是否需要绘制
void CMyControl::DoPaint(HDC hDC, const RECT& rcPaint) {
    RECT rcTemp;
    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;
    ExpensiveDrawing(hDC);
}
```

---

## 9. 总结

| 特性 | 说明 |
|------|------|
| **继承模式** | 选择合适基类，重写关键虚函数 |
| **注册方式** | IDialogBuilderCallback::CreateControl |
| **属性扩展** | 重写 SetAttribute，未知属性交给父类 |
| **绘制** | 重写 PaintXxx 子方法优于重写整个 DoPaint |
| **交互** | 重写 DoEvent 实现自定义行为 |
| **初始化** | DoInit 中访问 Manager，不在构造函数中 |
| **生命周期** | 容器负责 delete，不要手动释放 |
| **动画** | 基于 SetTimer + Invalidate 的逐帧更新 |
