# 渲染机制与自绘

## 概述

Duilib 的渲染核心是双缓冲绘制：所有控件绘制到一个离屏位图（内存 DC），再一次性 BitBlt 到窗口，避免闪烁。绘制流程从 WM_PAINT 触发，经过布局计算，最终递归调用每个控件的 DoPaint 完成整棵控件树的渲染。

---

## 1. 双缓冲原理

### 为什么需要双缓冲？

```
单缓冲（直接画到窗口 DC）：
1. 擦除背景 → 屏幕瞬间变白
2. 绘制控件A → 屏幕显示部分内容
3. 绘制控件B → 屏幕显示更多内容
结果：用户看到闪烁

双缓冲（先画到内存 DC）：
1. 在内存 DC 上擦除背景
2. 在内存 DC 上绘制控件A
3. 在内存 DC 上绘制控件B
4. 一次性 BitBlt 到窗口 DC → 屏幕瞬间更新
结果：用户无感知，无闪烁
```

### Duilib 双缓冲实现

```cpp
// CPaintManagerUI 中的离屏位图管理
class CPaintManagerUI {
    HDC     m_hDcOffscreen;   // 离屏 DC
    HBITMAP m_hbmpOffscreen;  // 离屏位图
    HDC     m_hDcPaint;       // 窗口 DC（BeginPaint 返回）
    int     m_nOffscreenWidth;
    int     m_nOffscreenHeight;
};

// 创建/调整离屏位图
void CPaintManagerUI::EnsureOffscreenDC(int cx, int cy)
{
    if (m_hDcOffscreen && m_nOffscreenWidth >= cx && m_nOffscreenHeight >= cy)
        return;

    if (m_hDcOffscreen) {
        ::DeleteDC(m_hDcOffscreen);
        ::DeleteObject(m_hbmpOffscreen);
    }

    m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
    m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, cx, cy);
    ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
    m_nOffscreenWidth = cx;
    m_nOffscreenHeight = cy;
}
```

---

## 2. WM_PAINT 完整处理流程

```cpp
// CPaintManagerUI::MessageHandler 中 WM_PAINT 处理（简化）
case WM_PAINT: {
    PAINTSTRUCT ps = { 0 };
    HDC hDC = ::BeginPaint(m_hWndPaint, &ps);
    m_hDcPaint = hDC;

    RECT rcClient;
    ::GetClientRect(m_hWndPaint, &rcClient);
    int cx = rcClient.right - rcClient.left;
    int cy = rcClient.bottom - rcClient.top;

    // 1. 确保离屏 DC 足够大
    EnsureOffscreenDC(cx, cy);

    // 2. 如果需要重新布局
    if (m_bUpdateNeeded) {
        m_bUpdateNeeded = false;
        if (m_pRoot) m_pRoot->SetPos(rcClient);
    }

    // 3. 在离屏 DC 上绘制整棵控件树
    RECT rcPaint = ps.rcPaint;  // 脏区域（系统计算的需要重绘的区域）
    if (m_pRoot) {
        m_pRoot->DoPaint(m_hDcOffscreen, rcPaint);
    }

    // 4. 绘制插入符（光标）
    for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
        // 后绘制控件（如浮动层）
    }

    // 5. 一次性 BitBlt 到窗口
    ::BitBlt(hDC,
             rcPaint.left, rcPaint.top,
             rcPaint.right - rcPaint.left,
             rcPaint.bottom - rcPaint.top,
             m_hDcOffscreen,
             rcPaint.left, rcPaint.top,
             SRCCOPY);

    ::EndPaint(m_hWndPaint, &ps);
    return true;
}
```

### 绘制流程图

```
WM_PAINT
    │
    ├── BeginPaint → 获取脏区域 rcPaint
    │
    ├── 布局阶段（如果 m_bUpdateNeeded）
    │   └── m_pRoot->SetPos(rcClient)  ← 递归计算所有控件位置
    │
    ├── 绘制阶段
    │   └── m_pRoot->DoPaint(hDcOffscreen, rcPaint)
    │       ├── PaintBkColor    ← 背景色
    │       ├── PaintBkImage    ← 背景图
    │       ├── PaintStatusImage← 状态图
    │       ├── PaintText       ← 文本
    │       ├── PaintBorder     ← 边框
    │       └── 容器：递归子控件 DoPaint
    │
    ├── BitBlt(hDC ← hDcOffscreen)
    │
    └── EndPaint
```

---

## 3. DoPaint 分层绘制

### CControlUI 的绘制层次

```cpp
void CControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    // 脏区域判断：控件不在脏区域内则跳过
    RECT rcTemp;
    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

    // 分层绘制（顺序固定，子类可重写任一层）
    PaintBkColor(hDC);       // 第1层：背景色填充
    PaintBkImage(hDC);       // 第2层：背景图片
    PaintStatusImage(hDC);   // 第3层：状态相关图片
    PaintText(hDC);          // 第4层：文本内容
    PaintBorder(hDC);        // 第5层：边框
}
```

### 各层具体实现

```cpp
// 背景色
void CControlUI::PaintBkColor(HDC hDC)
{
    if (m_dwBkColor != 0) {
        CRenderEngine::DrawColor(hDC, m_rcItem, m_dwBkColor);
    }
}

// 背景图
void CControlUI::PaintBkImage(HDC hDC)
{
    if (!m_sBkImage.IsEmpty()) {
        CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_sBkImage);
    }
}

// 状态图（按钮等有多种状态）
void CButtonUI::PaintStatusImage(HDC hDC)
{
    if (!IsEnabled()) {
        if (!m_sDisabledImage.IsEmpty()) {
            CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_sDisabledImage);
            return;
        }
    }
    if (m_uButtonState & UISTATE_PUSHED) {
        if (!m_sPushedImage.IsEmpty()) {
            CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_sPushedImage);
            return;
        }
    }
    if (m_uButtonState & UISTATE_HOT) {
        if (!m_sHotImage.IsEmpty()) {
            CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_sHotImage);
            return;
        }
    }
    if (!m_sNormalImage.IsEmpty()) {
        CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_sNormalImage);
    }
}

// 文本
void CLabelUI::PaintText(HDC hDC)
{
    if (m_sText.IsEmpty()) return;

    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.top += m_rcTextPadding.top;
    rcText.right -= m_rcTextPadding.right;
    rcText.bottom -= m_rcTextPadding.bottom;

    CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText,
                            m_dwTextColor, m_iFont, m_uTextStyle);
}

// 边框
void CControlUI::PaintBorder(HDC hDC)
{
    if (m_nBorderSize > 0) {
        CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, m_dwBorderColor);
    }
}
```

---

## 4. CRenderEngine 渲染引擎

### GDI 绘制基础

```cpp
class CRenderEngine {
public:
    // 填充颜色（支持 Alpha）
    static void DrawColor(HDC hDC, const RECT& rc, DWORD dwColor);

    // 绘制矩形边框
    static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwColor);

    // 绘制圆角矩形
    static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height,
                               int nSize, DWORD dwColor);

    // 绘制文本
    static void DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc,
                          LPCTSTR pstrText, DWORD dwTextColor, int iFont,
                          UINT uStyle);

    // 绘制图片（核心方法）
    static void DrawImage(HDC hDC, CPaintManagerUI* pManager,
                           const RECT& rc, const CDuiString& sImageName);

    // 绘制渐变
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst,
                              DWORD dwSecond, bool bVertical);
};
```

### 图片绘制与九宫格

```cpp
// 图片属性字符串格式
// "file='btn.png' corner='5,5,5,5' source='0,0,80,30' dest='0,0,80,30'"

// corner 属性实现九宫格拉伸
//
//  ┌──────┬────────────────┬──────┐
//  │ 左上 │     上边       │ 右上 │  ← 角部不拉伸
//  ├──────┼────────────────┼──────┤
//  │ 左边 │     中心       │ 右边 │  ← 边缘单向拉伸
//  ├──────┼────────────────┼──────┤     中心双向拉伸
//  │ 左下 │     下边       │ 右下 │
//  └──────┴────────────────┴──────┘

// corner="5,5,5,5" 表示四个角各保留 5px 不变形
// 适用于圆角按钮、面板边框等需要保持角部形状的图片

static void DrawImage(HDC hDC, const RECT& rcDest,
                      const RECT& rcSource, const RECT& rcCorner,
                      HBITMAP hBitmap, bool bAlpha)
{
    // 九宫格绘制：分 9 次 StretchBlt/AlphaBlend
    // 四个角：直接 BitBlt（不拉伸）
    // 四条边：单方向 StretchBlt
    // 中心：双方向 StretchBlt
}
```

---

## 5. Alpha 混合与半透明

### 带 Alpha 通道的图片绘制

```cpp
// Duilib 使用 AlphaBlend 实现半透明效果
static void DrawImageWithAlpha(HDC hDC, HBITMAP hBitmap,
                                const RECT& rcDest, const RECT& rcSource,
                                BYTE uFade = 255)
{
    HDC hMemDC = ::CreateCompatibleDC(hDC);
    ::SelectObject(hMemDC, hBitmap);

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
    ::AlphaBlend(hDC,
                 rcDest.left, rcDest.top,
                 rcDest.right - rcDest.left, rcDest.bottom - rcDest.top,
                 hMemDC,
                 rcSource.left, rcSource.top,
                 rcSource.right - rcSource.left, rcSource.bottom - rcSource.top,
                 bf);

    ::DeleteDC(hMemDC);
}
```

### 窗口整体半透明（Layered Window）

```cpp
// 使用 UpdateLayeredWindow 实现异形/半透明窗口
void CPaintManagerUI::PaintLayeredWindow()
{
    RECT rcClient;
    ::GetClientRect(m_hWndPaint, &rcClient);
    int cx = rcClient.right - rcClient.left;
    int cy = rcClient.bottom - rcClient.top;

    // 创建 32 位带 Alpha 位图
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy;  // 自顶向下
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;

    LPBYTE pBits = NULL;
    HBITMAP hBmp = ::CreateDIBSection(m_hDcPaint, &bmi,
                                       DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
    HDC hMemDC = ::CreateCompatibleDC(m_hDcPaint);
    ::SelectObject(hMemDC, hBmp);

    // 在 hMemDC 上绘制（带 Alpha 信息）
    m_pRoot->DoPaint(hMemDC, rcClient);

    // 使用 UpdateLayeredWindow 更新窗口
    POINT ptSrc = { 0, 0 };
    SIZE szWnd = { cx, cy };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    ::UpdateLayeredWindow(m_hWndPaint, m_hDcPaint,
                          NULL, &szWnd,
                          hMemDC, &ptSrc,
                          0, &bf, ULW_ALPHA);

    ::DeleteDC(hMemDC);
    ::DeleteObject(hBmp);
}
```

---

## 6. 脏区域管理

### 局部重绘

```cpp
// 控件调用 Invalidate() 时，只标记自身区域为脏
void CControlUI::Invalidate()
{
    if (m_pManager) {
        RECT rc = m_rcItem;
        m_pManager->Invalidate(rc);
    }
}

void CPaintManagerUI::Invalidate(RECT& rc)
{
    // 调用 Windows API 标记脏区域
    ::InvalidateRect(m_hWndPaint, &rc, FALSE);
    // FALSE = 不擦除背景（我们自己双缓冲绘制，不需要系统擦）
}

// WM_PAINT 时，ps.rcPaint 就是系统合并后的脏区域
// DoPaint 中通过 IntersectRect 判断控件是否在脏区域内
// 不在脏区域内的控件直接跳过，减少绘制量
```

### NeedUpdate vs Invalidate

```cpp
// NeedUpdate：标记需要重新布局 + 重绘（整个窗口）
void CPaintManagerUI::NeedUpdate()
{
    m_bUpdateNeeded = true;
    ::InvalidateRect(m_hWndPaint, NULL, FALSE);  // 整个窗口
}

// Invalidate：只重绘特定区域（不重新布局）
void CControlUI::Invalidate()
{
    ::InvalidateRect(m_hWndPaint, &m_rcItem, FALSE);  // 局部
}
```

| 方法 | 触发布局 | 重绘范围 | 使用场景 |
|------|---------|---------|---------|
| NeedUpdate | 是 | 整个窗口 | 添加/移除控件、窗口大小变化 |
| Invalidate | 否 | 控件区域 | 状态变化（hover、点击） |

---

## 7. GDI+ 扩展渲染

```cpp
// 某些 Duilib 版本支持 GDI+ 高级渲染
#include <GdiPlus.h>

void CMyControl::PaintBkImage(HDC hDC)
{
    Gdiplus::Graphics graphics(hDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

    // 绘制圆形头像
    Gdiplus::GraphicsPath path;
    path.AddEllipse(m_rcItem.left, m_rcItem.top,
                    m_rcItem.right - m_rcItem.left,
                    m_rcItem.bottom - m_rcItem.top);
    graphics.SetClip(&path);

    Gdiplus::Image image(L"avatar.png");
    graphics.DrawImage(&image, m_rcItem.left, m_rcItem.top,
                       m_rcItem.right - m_rcItem.left,
                       m_rcItem.bottom - m_rcItem.top);
}
```

---

## 8. 面试常见问题

### Q1: Duilib 为什么使用 GDI 而不是 Direct2D/OpenGL？

历史原因和兼容性考虑：
- Duilib 诞生时（2010 年前后）GDI 是 Windows 最通用的 2D 渲染 API
- GDI 在所有 Windows 版本上都可用，无需额外运行时
- 对于 UI 控件级别的 2D 渲染，GDI 性能足够
- 后续部分分支（如 nim_duilib）已增加 Direct2D/Skia 渲染后端

### Q2: 双缓冲会额外占用多少内存？

一个 32 位色 1920x1080 的离屏位图占用约 `1920 × 1080 × 4 = 8MB`。对于普通窗口（800x600）约 `1.8MB`。这是空间换时间的典型权衡。

### Q3: 如何实现圆角窗口？

```cpp
// 方案1：窗口区域裁剪
HRGN hRgn = ::CreateRoundRectRgn(0, 0, cx, cy, rx, ry);
::SetWindowRgn(m_hWnd, hRgn, TRUE);

// 方案2：Layered Window + 带 Alpha 的圆角绘制
// 使用 UpdateLayeredWindow，圆角外部像素 Alpha=0（透明）

// 方案3：XML 属性 roundcorner
// <Window roundcorner="8,8" />
// 内部使用方案1实现
```

### Q4: WM_ERASEBKGND 如何处理？

```cpp
case WM_ERASEBKGND:
    return 1;  // 返回非零值，告诉系统不要擦除背景
    // 因为我们双缓冲绘制时会完整覆盖整个区域
    // 如果让系统擦除，反而会导致闪烁
```

### Q5: 容器的裁剪（Clip）为什么重要？

```cpp
// 容器设置裁剪区域，防止子控件画到容器外
void CContainerUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    // 设置裁剪
    HRGN hRgn = ::CreateRectRgnIndirect(&m_rcItem);
    ::SelectClipRgn(hDC, hRgn);

    // 绘制子控件
    for (int i = 0; i < m_items.GetSize(); i++) {
        ((CControlUI*)m_items[i])->DoPaint(hDC, rcPaint);
    }

    // 恢复裁剪
    ::SelectClipRgn(hDC, NULL);
    ::DeleteObject(hRgn);
}
// 没有裁剪的话，滚动列表的子项会画到列表框外面
```

---

## 9. 常见陷阱

```cpp
// ❌ 在 DoPaint 中创建 GDI 对象不释放 → GDI 泄漏
void CMyControl::PaintText(HDC hDC) {
    HFONT hFont = ::CreateFont(...);
    ::SelectObject(hDC, hFont);
    ::DrawText(hDC, ...);
    // 忘记 DeleteObject(hFont)！
}

// ✅ 正确管理 GDI 对象
void CMyControl::PaintText(HDC hDC) {
    HFONT hFont = ::CreateFont(...);
    HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
    ::DrawText(hDC, ...);
    ::SelectObject(hDC, hOldFont);  // 恢复旧对象
    ::DeleteObject(hFont);           // 删除创建的对象
}
```

```cpp
// ❌ Invalidate 过于频繁导致性能问题
void OnMouseMove(POINT pt) {
    m_ptCursor = pt;
    Invalidate();  // 每次鼠标移动都重绘整个控件
}

// ✅ 只在状态真正变化时重绘
void OnMouseMove(POINT pt) {
    bool bOldHot = m_bHot;
    m_bHot = ::PtInRect(&m_rcHotZone, pt);
    if (bOldHot != m_bHot) Invalidate();  // 仅状态变化时重绘
}
```

---

## 10. 总结

| 特性 | 说明 |
|------|------|
| **核心机制** | 双缓冲：离屏位图绘制 + BitBlt 到窗口 |
| **绘制触发** | WM_PAINT，由 InvalidateRect 触发 |
| **绘制分层** | BkColor → BkImage → StatusImage → Text → Border |
| **脏区域** | 局部 Invalidate 减少绘制量 |
| **图片渲染** | 支持九宫格拉伸、Alpha 混合 |
| **半透明窗口** | UpdateLayeredWindow + 32 位位图 |
| **渲染引擎** | 以 GDI 为主，部分版本支持 GDI+/Direct2D |
