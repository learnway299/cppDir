# 性能优化

## 概述

Duilib 性能问题主要集中在三个方面：绘制（过度重绘）、布局（频繁递归计算）、内存（图片资源占用）。DirectUI 架构虽然避免了大量 HWND 开销，但由于所有控件共享一个窗口的绘制表面，不合理的使用方式容易引发性能瓶颈，尤其在大数据量列表和复杂界面场景下。

---

## 1. 绘制性能优化

### 减少不必要的重绘

```cpp
// ❌ 频繁调用 Invalidate（如动画每帧、鼠标移动中）
void OnProgress(int value) {
    m_pProgress->SetValue(value);      // 内部会 Invalidate
    m_pLabel->SetText(FormatPercent()); // 又 Invalidate
    m_pIcon->SetBkImage(GetIcon());    // 又 Invalidate
    // 一次逻辑更新触发了 3 次 InvalidateRect → 3 次 WM_PAINT
}

// ✅ 合并重绘：用 SetVisible(false) 包裹批量更新
void OnProgress(int value) {
    // 方案A：手动合并脏区域
    RECT rcDirty = { 0 };
    ::UnionRect(&rcDirty, &m_pProgress->GetPos(), &m_pLabel->GetPos());
    ::UnionRect(&rcDirty, &rcDirty, &m_pIcon->GetPos());

    m_pProgress->SetValue(value);
    m_pLabel->SetText(FormatPercent());
    m_pIcon->SetBkImage(GetIcon());

    // 只触发一次
    ::InvalidateRect(m_hWnd, &rcDirty, FALSE);
}

// ✅ 方案B：延迟更新
void OnProgress(int value) {
    m_pendingValue = value;
    if (!m_bUpdatePending) {
        m_bUpdatePending = true;
        ::PostMessage(m_hWnd, WM_USER_UPDATE, 0, 0);
    }
}
// 在 WM_USER_UPDATE 中一次性更新所有控件
```

### 局部重绘 vs 全局重绘

```cpp
// 了解触发全局重绘的操作：
// - NeedUpdate()：布局 + 全窗口重绘
// - 窗口大小变化（WM_SIZE）
// - 添加/移除控件

// 了解触发局部重绘的操作：
// - Invalidate()：只重绘控件自身区域
// - 鼠标进出导致的状态变化

// 优化原则：能 Invalidate 就不要 NeedUpdate
// ❌
void UpdateText(CControlUI* pLabel, LPCTSTR text) {
    pLabel->SetText(text);
    m_pm.NeedUpdate();  // 触发全窗口重新布局和绘制
}

// ✅
void UpdateText(CControlUI* pLabel, LPCTSTR text) {
    pLabel->SetText(text);  // 内部已经 Invalidate 了自己
}
```

### DoPaint 中的优化技巧

```cpp
void CMyControl::DoPaint(HDC hDC, const RECT& rcPaint)
{
    // 1. 脏区域判断（基类通常已做，自定义绘制要注意）
    RECT rcTemp;
    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

    // 2. 避免在 DoPaint 中做耗时操作
    // ❌ 每次绘制都计算
    int value = ExpensiveCalculation();
    DrawProgress(hDC, value);

    // ✅ 缓存计算结果，只在数据变化时更新
    DrawProgress(hDC, m_cachedValue);

    // 3. 利用裁剪区域跳过不可见部分
    // 对于超大控件（如自绘长文本），只绘制可见部分
}
```

---

## 2. 虚拟列表（大数据量优化）

### 问题：万级数据列表

```cpp
// ❌ 直接创建 10000 个列表项
CListUI* pList = ...;
for (int i = 0; i < 10000; i++) {
    CListLabelElementUI* pItem = new CListLabelElementUI;
    pItem->SetText(GetData(i));
    pList->Add(pItem);
}
// 问题：10000 个控件对象占内存，初始化慢，布局计算慢
```

### 虚拟列表原理

```
┌─────────────────────────────┐
│ 可见区域（假设能显示 20 行）   │  ← 只创建约 20-25 个控件
│  ┌───────────────────────┐  │
│  │ Item (数据索引 = 50)   │  │
│  │ Item (数据索引 = 51)   │  │
│  │ Item (数据索引 = 52)   │  │
│  │ ...                    │  │
│  │ Item (数据索引 = 69)   │  │
│  └───────────────────────┘  │
└─────────────────────────────┘
         ↕ 滚动时
  控件复用，更新数据索引和显示内容
  滚动条高度根据总数据量计算
```

### 实现方案

```cpp
class CVirtualListUI : public CContainerUI {
public:
    void SetDataCount(int nCount) {
        m_nDataCount = nCount;
        UpdateScrollRange();
        Invalidate();
    }

    void SetItemProvider(IVirtualListProvider* pProvider) {
        m_pProvider = pProvider;
    }

    void SetPos(RECT rc) override {
        CContainerUI::SetPos(rc);

        int cyItem = m_nItemHeight;
        int nVisibleCount = (rc.bottom - rc.top) / cyItem + 2;  // 多创建几个缓冲

        // 确保足够的控件实例（对象池）
        while (GetCount() < nVisibleCount) {
            CControlUI* pItem = m_pProvider->CreateItem();
            Add(pItem);
        }

        // 根据滚动位置计算第一条可见数据索引
        int nScrollPos = GetScrollPos().cy;
        int nFirstIndex = nScrollPos / cyItem;

        // 填充可见控件的数据
        int yPos = rc.top - (nScrollPos % cyItem);
        for (int i = 0; i < nVisibleCount && nFirstIndex + i < m_nDataCount; i++) {
            CControlUI* pItem = GetItemAt(i);
            pItem->SetVisible(true);

            // 让数据提供者填充内容
            m_pProvider->FillItem(pItem, nFirstIndex + i);

            RECT rcItem = { rc.left, yPos, rc.right, yPos + cyItem };
            pItem->SetPos(rcItem);
            yPos += cyItem;
        }

        // 隐藏多余的控件
        for (int i = nVisibleCount; i < GetCount(); i++) {
            GetItemAt(i)->SetVisible(false);
        }
    }

private:
    int m_nDataCount = 0;
    int m_nItemHeight = 30;
    IVirtualListProvider* m_pProvider = nullptr;

    void UpdateScrollRange() {
        int cyTotal = m_nDataCount * m_nItemHeight;
        SetScrollRange({ 0, cyTotal });
    }
};

// 数据提供者接口
class IVirtualListProvider {
public:
    virtual CControlUI* CreateItem() = 0;
    virtual void FillItem(CControlUI* pItem, int nDataIndex) = 0;
};
```

---

## 3. 布局性能优化

### 避免频繁触发全局布局

```cpp
// ❌ 循环中每次添加控件都触发布局
for (int i = 0; i < 100; i++) {
    CControlUI* pItem = new CListLabelElementUI;
    pItem->SetText(data[i]);
    pList->Add(pItem);  // 每次 Add 内部调用 NeedUpdate
}
// 触发了 100 次布局计算！

// ✅ 批量添加后统一布局
pList->SetDelayedDestroy(false);
for (int i = 0; i < 100; i++) {
    CControlUI* pItem = new CListLabelElementUI;
    pItem->SetText(data[i]);
    pList->AddAt(pItem, pList->GetCount());  // 某些实现可延迟
}
m_pm.NeedUpdate();  // 只触发一次
```

### 延迟布局

```cpp
// 连续多次 NeedUpdate 只执行一次布局
// 原理：NeedUpdate 只设置标记 + InvalidateRect
// 实际布局在 WM_PAINT 时才执行
// Windows 会合并多个 InvalidateRect 为一次 WM_PAINT

void CPaintManagerUI::NeedUpdate() {
    m_bUpdateNeeded = true;  // 只设标记
    ::InvalidateRect(m_hWndPaint, NULL, FALSE);
}

// WM_PAINT 中：
if (m_bUpdateNeeded) {
    m_bUpdateNeeded = false;
    m_pRoot->SetPos(rcClient);  // 此时才真正计算布局
}
```

---

## 4. 内存优化

### 图片资源内存控制

```cpp
// 大图片的内存占用：1920x1080 ARGB = 8MB
// 优化策略：

// 1. 使用合适的图片尺寸（不要用大图缩小显示）
// 一个 48x48 的图标用 512x512 的 PNG，浪费 ~1MB

// 2. 及时释放不可见页面的图片缓存
void OnTabChanged(int newTab) {
    // 清除非当前页的大图缓存
    if (m_nOldTab != newTab) {
        ClearTabImages(m_nOldTab);
    }
    m_nOldTab = newTab;
}

// 3. 使用精灵图（Sprite Sheet）减少图片对象数
// 多个小图标合并为一张大图，通过 source 参数裁剪
```

### 控件对象内存

```cpp
// CControlUI 基础大小约 200-300 字节
// 一个复杂列表项（含多个子控件）可能 1-2KB

// 10000 个列表项 = 10-20MB 纯对象内存
// → 使用虚拟列表，只创建可见数量的控件
```

### GDI 对象泄漏检测

```cpp
// Windows 每进程 GDI 对象限制约 10000 个
// 常见泄漏：自定义绘制中 CreateFont/CreatePen/CreateBrush 后忘记 Delete

// 检测方法：
DWORD nGdiObjects = ::GetGuiResources(::GetCurrentProcess(), GR_GDIOBJECTS);
if (nGdiObjects > 5000) {
    // 警告：可能存在 GDI 泄漏
    OutputDebugString(_T("GDI objects exceeding threshold!\n"));
}

// 使用 Task Manager 的 "GDI Objects" 列监控
```

---

## 5. 消息处理优化

### 减少 HitTest 开销

```cpp
// 每次 WM_MOUSEMOVE 都要遍历控件树做 HitTest
// 优化思路：

// 1. 减少控件树深度（扁平化）
// 嵌套越深，FindControl 递归越多

// 2. 设置不需要鼠标事件的控件为 mouse="false"
// <Label text="纯展示" mouse="false" />
// mouse=false 的控件不参与 HitTest

// 3. 容器的 mousechild="false" 让子控件不响应鼠标
// <VerticalLayout mousechild="false">
//     <!-- 所有子控件不参与 HitTest -->
// </VerticalLayout>
```

### 定时器优化

```cpp
// ❌ 多个控件各自设置高频定时器
class CControl1 : public CControlUI {
    void DoInit() override { m_pManager->SetTimer(this, 1, 16); }
};
// 10 个这样的控件 = 10 个 16ms 定时器 = 每秒 600 次回调

// ✅ 使用统一的帧更新定时器
class CAnimationManager {
    void Start() { ::SetTimer(m_hWnd, TIMER_FRAME, 16, NULL); }

    void OnTimer() {
        for (auto& anim : m_animations) {
            anim->Update();
        }
        // 合并所有动画的脏区域，一次 Invalidate
        ::InvalidateRect(m_hWnd, &m_rcDirtyUnion, FALSE);
    }
};
```

---

## 6. XML 加载优化

### 预编译 XML

```cpp
// 每次 Create 都要解析 XML 文本 → 对象创建
// 优化：对于频繁创建的 XML（如列表项模板），缓存解析结果

// Duilib 的 CDialogBuilder 支持缓存 Markup
CDialogBuilder builder;
// 首次调用 Create 会解析 XML
CControlUI* pItem1 = builder.Create(_T("listitem.xml"), 0, this, &m_pm);

// 后续调用复用已解析的 Markup（内部缓存）
CControlUI* pItem2 = builder.Create(0, this, &m_pm);  // 不传文件名
CControlUI* pItem3 = builder.Create(0, this, &m_pm);
```

### 减少 Include 层级

```
// 每层 Include 都需要：
// 1. 打开文件/从 ZIP 读取
// 2. 解析 XML 文本
// 3. 创建节点树

// 启动时大量 Include 会明显影响窗口显示速度
// 优化：合并小的 Include 文件，或预加载到内存
```

---

## 7. 面试常见问题

### Q1: Duilib 列表控件显示 10 万条数据如何不卡？

核心方案是虚拟列表：
1. 只创建可视区域数量的控件对象（如 30 个）
2. 滚动时复用控件，更新数据内容
3. 滚动条范围根据总数据量计算
4. 额外优化：双缓冲控件池、异步数据加载

### Q2: 如何分析 Duilib 的绘制性能瓶颈？

```cpp
// 方法1：在 DoPaint 中埋点计时
DWORD t1 = ::GetTickCount();
m_pRoot->DoPaint(hDC, rcPaint);
DWORD elapsed = ::GetTickCount() - t1;
// 超过 16ms 说明单帧绘制时间过长（低于 60fps）

// 方法2：统计 WM_PAINT 频率
static int nPaintCount = 0;
static DWORD dwLastReport = 0;
nPaintCount++;
if (::GetTickCount() - dwLastReport > 1000) {
    OutputDebugString(Format(_T("Paint FPS: %d\n"), nPaintCount));
    nPaintCount = 0;
    dwLastReport = ::GetTickCount();
}

// 方法3：使用 ETW / GPUView 分析渲染管线
```

### Q3: NeedUpdate 和 Invalidate 在性能上有何区别？

| | NeedUpdate | Invalidate |
|---|---|---|
| 布局计算 | 触发 SetPos 递归 | 不触发 |
| 重绘范围 | 整个窗口 | 控件区域 |
| 性能开销 | 高（布局 + 全绘制） | 低（局部绘制） |
| 使用场景 | 控件增删、窗口 resize | 状态变化、数据更新 |

### Q4: 如何优化频繁更新的控件（如实时数据面板）？

```cpp
// 1. 降低更新频率（聚合多次数据更新）
void OnDataArrived(const Data& data) {
    m_latestData = data;
    if (!m_bUpdateScheduled) {
        m_bUpdateScheduled = true;
        // 最多 30fps 刷新
        m_pManager->SetTimer(this, TIMER_REFRESH, 33);
    }
}

// 2. 只更新变化的部分
void RefreshUI() {
    if (m_latestData.price != m_displayedData.price) {
        m_pLabelPrice->SetText(FormatPrice(m_latestData.price));
    }
    // 不变的字段不调用 SetText（避免不必要的 Invalidate）
}

// 3. 使用 RedrawWindow 控制重绘时机
::RedrawWindow(m_hWnd, &rcDataPanel, NULL,
               RDW_INVALIDATE | RDW_UPDATENOW);
```

---

## 8. 性能检查清单

| 检查项 | 标准 |
|--------|------|
| 单帧绘制时间 | < 16ms（保证 60fps） |
| GDI 对象数量 | < 3000（远离 10000 限制） |
| 控件树深度 | < 10 层嵌套 |
| 列表数据量 | > 1000 条应使用虚拟列表 |
| 图片资源 | 实际显示尺寸加载，不过度缩放 |
| 定时器数量 | 尽量合并，避免 > 20 个同时活跃 |
| XML Include | 控制在 3 层以内 |
| 窗口启动时间 | < 200ms 显示首帧 |

---

## 9. 总结

| 优化方向 | 核心策略 |
|---------|---------|
| **绘制** | 局部 Invalidate、合并脏区域、跳过不可见控件 |
| **列表** | 虚拟列表：控件复用 + 数据按需加载 |
| **布局** | 避免频繁 NeedUpdate、批量操作后统一布局 |
| **内存** | 合适尺寸图片、精灵图、及时释放不可见资源 |
| **消息** | mouse=false 减少 HitTest、合并定时器 |
| **加载** | 缓存 XML 解析结果、减少 Include 层级 |
