# 布局管理器

## 概述

Duilib 提供多种布局管理器来自动排列子控件，避免手动计算坐标。布局的核心流程是两轮遍历：第一轮统计固定尺寸和自适应控件数，第二轮分配剩余空间并递归设置子控件位置。所有布局管理器都继承自 CContainerUI，通过重写 SetPos 实现不同的排列策略。

---

## 1. 布局类型一览

| 布局 | 类名 | 排列方式 |
|------|------|---------|
| 垂直布局 | CVerticalLayoutUI | 子控件从上到下排列 |
| 水平布局 | CHorizontalLayoutUI | 子控件从左到右排列 |
| 瓦片布局 | CTileLayoutUI | 网格排列，自动换行 |
| 标签页布局 | CTabLayoutUI | 同一时间只显示一个子控件 |
| 容器 | CContainerUI | 不自动布局，子控件使用绝对定位 |

---

## 2. 布局计算核心流程

### 通用两轮算法

```
第一轮：统计
├── 遍历所有可见子控件
├── 调用 EstimateSize 获取期望尺寸
├── 有固定尺寸的 → 累加到 fixedTotal
└── 无固定尺寸的 → expandCount++

第二轮：分配
├── expandSize = (availableSpace - fixedTotal) / expandCount
├── 遍历子控件
├── 固定尺寸控件 → 使用其 EstimateSize
├── 自适应控件 → 分配 expandSize
└── 调用子控件 SetPos(计算出的矩形) → 递归
```

---

## 3. CVerticalLayoutUI 垂直布局

### SetPos 实现

```cpp
void CVerticalLayoutUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);

    // 减去容器内边距
    RECT rcInset = GetInset();
    rc.left += rcInset.left;
    rc.top += rcInset.top;
    rc.right -= rcInset.right;
    rc.bottom -= rcInset.bottom;

    // 第一轮：统计
    int cyAvailable = rc.bottom - rc.top;
    int cyFixed = 0;
    int nExpandCount = 0;
    int nVisibleCount = 0;

    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];
        if (!pChild->IsVisible()) continue;
        nVisibleCount++;

        SIZE sz = pChild->EstimateSize({ rc.right - rc.left, cyAvailable });
        if (sz.cy == 0) {
            nExpandCount++;  // 高度自适应
        } else {
            cyFixed += sz.cy;
        }
        cyFixed += pChild->GetPadding().top + pChild->GetPadding().bottom;
    }

    // 子控件间距
    cyFixed += (nVisibleCount - 1) * m_iChildPadding;

    // 第二轮：分配
    int cyExpand = 0;
    if (nExpandCount > 0) {
        cyExpand = max(0, (cyAvailable - cyFixed) / nExpandCount);
    }

    int yPos = rc.top;

    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];
        if (!pChild->IsVisible()) continue;

        RECT rcPadding = pChild->GetPadding();
        yPos += rcPadding.top;

        SIZE sz = pChild->EstimateSize({ rc.right - rc.left, cyAvailable });
        int cyItem = (sz.cy == 0) ? cyExpand : sz.cy;

        // 水平方向处理对齐
        int cxItem = rc.right - rc.left - rcPadding.left - rcPadding.right;
        if (sz.cx > 0 && sz.cx < cxItem) {
            // 控件有固定宽度，居中或靠左
            cxItem = sz.cx;
        }

        RECT rcChild = {
            rc.left + rcPadding.left,
            yPos,
            rc.left + rcPadding.left + cxItem,
            yPos + cyItem
        };

        pChild->SetPos(rcChild);
        yPos += cyItem + rcPadding.bottom + m_iChildPadding;
    }
}
```

### XML 示例

```xml
<VerticalLayout inset="10,10,10,10" childpadding="8">
    <Label text="标题" height="24" />
    <Edit name="input" height="30" />
    <!-- height 未设置，自适应分配剩余空间 -->
    <List name="list" vscrollbar="true" />
    <Button name="btnOk" height="36" text="确定" />
</VerticalLayout>
```

---

## 4. CHorizontalLayoutUI 水平布局

### 与垂直布局的对称关系

```cpp
void CHorizontalLayoutUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);
    RECT rcInset = GetInset();
    rc.left += rcInset.left;
    rc.top += rcInset.top;
    rc.right -= rcInset.right;
    rc.bottom -= rcInset.bottom;

    int cxAvailable = rc.right - rc.left;
    int cxFixed = 0;
    int nExpandCount = 0;

    // 第一轮：统计（方向变为水平）
    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];
        if (!pChild->IsVisible()) continue;

        SIZE sz = pChild->EstimateSize({ cxAvailable, rc.bottom - rc.top });
        if (sz.cx == 0)
            nExpandCount++;
        else
            cxFixed += sz.cx;
        cxFixed += pChild->GetPadding().left + pChild->GetPadding().right;
    }

    // 第二轮：水平分配
    int cxExpand = nExpandCount > 0 ? max(0, (cxAvailable - cxFixed) / nExpandCount) : 0;
    int xPos = rc.left;

    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];
        if (!pChild->IsVisible()) continue;

        RECT rcPadding = pChild->GetPadding();
        xPos += rcPadding.left;

        SIZE sz = pChild->EstimateSize({ cxAvailable, rc.bottom - rc.top });
        int cxItem = (sz.cx == 0) ? cxExpand : sz.cx;
        int cyItem = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;

        RECT rcChild = {
            xPos,
            rc.top + rcPadding.top,
            xPos + cxItem,
            rc.top + rcPadding.top + cyItem
        };

        pChild->SetPos(rcChild);
        xPos += cxItem + rcPadding.right + m_iChildPadding;
    }
}
```

### 常见模式：弹性占位

```xml
<!-- 标题栏：左侧标题，右侧按钮组 -->
<HorizontalLayout height="36">
    <Label text="应用名称" width="120" />
    <Control />  <!-- 无宽高，自适应占满剩余空间 -->
    <Button name="btnMin" width="36" />
    <Button name="btnMax" width="36" />
    <Button name="btnClose" width="36" />
</HorizontalLayout>
```

`<Control />` 是一个空控件，宽度为 0（自适应），会自动占据中间的所有剩余空间，将按钮推到右侧。

---

## 5. CTileLayoutUI 瓦片布局

### 网格排列（自动换行）

```cpp
void CTileLayoutUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);
    RECT rcInset = GetInset();
    rc.left += rcInset.left;
    rc.top += rcInset.top;
    rc.right -= rcInset.right;
    rc.bottom -= rcInset.bottom;

    int cxAvailable = rc.right - rc.left;

    // 每个格子的尺寸（固定）
    int cxItem = m_szItem.cx;  // 格子宽度
    int cyItem = m_szItem.cy;  // 格子高度

    // 计算每行能放多少个
    int nColumns = (cxAvailable + m_iChildPadding) / (cxItem + m_iChildPadding);
    if (nColumns < 1) nColumns = 1;

    int xPos = rc.left;
    int yPos = rc.top;
    int nCol = 0;

    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];
        if (!pChild->IsVisible()) continue;

        RECT rcChild = { xPos, yPos, xPos + cxItem, yPos + cyItem };
        pChild->SetPos(rcChild);

        nCol++;
        if (nCol >= nColumns) {
            // 换行
            nCol = 0;
            xPos = rc.left;
            yPos += cyItem + m_iChildPadding;
        } else {
            xPos += cxItem + m_iChildPadding;
        }
    }
}
```

### XML 示例

```xml
<!-- 图片网格，每格 120x120 -->
<TileLayout itemsize="120,120" childpadding="8" vscrollbar="true">
    <Control bkimage="photo1.jpg" />
    <Control bkimage="photo2.jpg" />
    <Control bkimage="photo3.jpg" />
    <!-- 自动换行排列 -->
</TileLayout>
```

---

## 6. CTabLayoutUI 标签页布局

### 同时只显示一个子控件

```cpp
void CTabLayoutUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);
    RECT rcInset = GetInset();
    rc.left += rcInset.left;
    rc.top += rcInset.top;
    rc.right -= rcInset.right;
    rc.bottom -= rcInset.bottom;

    // 只有当前选中页可见，占满整个容器
    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];
        if (i == m_iCurSel) {
            pChild->SetVisible(true);
            pChild->SetPos(rc);  // 占满整个可用区域
        } else {
            pChild->SetVisible(false);
        }
    }
}

// 切换页面
bool CTabLayoutUI::SelectItem(int iIndex)
{
    if (iIndex < 0 || iIndex >= m_items.GetSize()) return false;
    if (iIndex == m_iCurSel) return true;

    m_iCurSel = iIndex;
    NeedParentUpdate();  // 触发重新布局

    // 发送通知
    if (m_pManager) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel);
    }
    return true;
}
```

### XML 示例

```xml
<!-- 配合 Option 按钮组实现 Tab 页切换 -->
<HorizontalLayout height="36">
    <Option name="tab1" text="首页" group="tabs" selected="true" />
    <Option name="tab2" text="消息" group="tabs" />
    <Option name="tab3" text="设置" group="tabs" />
</HorizontalLayout>

<TabLayout name="tabPages">
    <!-- 第0页 -->
    <VerticalLayout>
        <Label text="首页内容" />
    </VerticalLayout>
    <!-- 第1页 -->
    <VerticalLayout>
        <Label text="消息内容" />
    </VerticalLayout>
    <!-- 第2页 -->
    <VerticalLayout>
        <Label text="设置内容" />
    </VerticalLayout>
</TabLayout>
```

```cpp
// 代码中切换
void Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) {
        CTabLayoutUI* pTab = (CTabLayoutUI*)m_pm.FindControl(_T("tabPages"));
        if (msg.pSender->GetName() == _T("tab1")) pTab->SelectItem(0);
        else if (msg.pSender->GetName() == _T("tab2")) pTab->SelectItem(1);
        else if (msg.pSender->GetName() == _T("tab3")) pTab->SelectItem(2);
    }
}
```

---

## 7. Float 浮动布局

### 脱离布局流

```xml
<!-- float="true" 的控件不参与父容器的自动布局 -->
<VerticalLayout>
    <Label text="正常流控件" height="30" />
    <Button text="正常流按钮" height="36" />

    <!-- 浮动控件：绝对定位，覆盖在其他控件上方 -->
    <Button name="btnFloat" float="true" pos="300,10,380,40"
            normalimage="icon_notify.png" />
</VerticalLayout>
```

### Float 控件在 SetPos 中的处理

```cpp
void CVerticalLayoutUI::SetPos(RECT rc)
{
    // ... 布局计算 ...

    for (int i = 0; i < m_items.GetSize(); i++) {
        CControlUI* pChild = (CControlUI*)m_items[i];

        if (pChild->IsFloat()) {
            // 浮动控件：使用自身的 pos 属性设置位置
            RECT rcFloat = pChild->GetFloatPos();
            // 相对于容器左上角偏移
            rcFloat.left += rc.left;
            rcFloat.top += rc.top;
            rcFloat.right += rc.left;
            rcFloat.bottom += rc.top;
            pChild->SetPos(rcFloat);
            continue;  // 不参与正常布局流
        }

        // 正常布局...
    }
}
```

---

## 8. 布局相关属性

### 容器属性

| 属性 | 说明 | 示例 |
|------|------|------|
| inset | 容器内边距 | `inset="10,10,10,10"` |
| childpadding | 子控件间距 | `childpadding="8"` |
| vscrollbar | 垂直滚动条 | `vscrollbar="true"` |
| hscrollbar | 水平滚动条 | `hscrollbar="true"` |

### 子控件属性

| 属性 | 说明 | 示例 |
|------|------|------|
| width | 固定宽度（0=自适应） | `width="100"` |
| height | 固定高度（0=自适应） | `height="36"` |
| minwidth/minheight | 最小尺寸 | `minheight="20"` |
| maxwidth/maxheight | 最大尺寸 | `maxwidth="500"` |
| padding | 控件外边距 | `padding="5,5,5,5"` |
| float | 浮动定位 | `float="true"` |
| pos | 浮动位置 | `pos="10,10,100,40"` |

---

## 9. 面试常见问题

### Q1: EstimateSize 返回 {0,0} 意味着什么？

表示控件希望在两个方向上都自适应。布局管理器会将剩余空间平均分配给所有 EstimateSize 返回 0 的控件。如果一个方向返回固定值，另一方向返回 0，则只在一个方向自适应。

### Q2: childpadding 和 padding 的区别？

- `childpadding` 是容器属性，定义相邻子控件之间的间距
- `padding` 是控件自身属性，定义控件外侧的边距（类似 CSS margin）
- `inset` 是容器属性，定义容器内壁与子控件的间距（类似 CSS padding）

```
┌─ Container ─────────────────────────┐
│  inset                               │
│  ┌─ Child1 ──────────────────────┐  │
│  │  padding                       │  │
│  └────────────────────────────────┘  │
│  childpadding                        │
│  ┌─ Child2 ──────────────────────┐  │
│  │  padding                       │  │
│  └────────────────────────────────┘  │
└──────────────────────────────────────┘
```

### Q3: 如何实现等比例分配？

```xml
<!-- 三列等宽布局：三个控件都不设 width，自动三等分 -->
<HorizontalLayout>
    <VerticalLayout />  <!-- 1/3 -->
    <VerticalLayout />  <!-- 1/3 -->
    <VerticalLayout />  <!-- 1/3 -->
</HorizontalLayout>

<!-- 2:1 比例：左侧 width=0（自适应），右侧固定 -->
<!-- 无法直接设比例，需要通过代码动态计算 -->
```

Duilib 原版不支持权重/比例布局（不像 Android 的 layout_weight）。如需比例分配，通常在 WM_SIZE 时手动计算并 SetFixedWidth。

### Q4: 嵌套布局的性能如何？

每次窗口大小变化都会从根节点递归 SetPos。嵌套越深，递归调用越多。但由于 SetPos 本身只是数学计算（不涉及 I/O 或绘制），实际项目中即使嵌套 5-6 层也不会有明显性能问题。性能瓶颈通常在绘制（DoPaint）而非布局。

---

## 10. 常见陷阱

```xml
<!-- ❌ 所有子控件都设固定高度，没有自适应控件，底部可能留空 -->
<VerticalLayout>
    <Label height="30" text="A" />
    <Label height="30" text="B" />
    <Label height="30" text="C" />
    <!-- 容器高度 300px，但子控件只占 90px，底部 210px 空白 -->
</VerticalLayout>

<!-- ✅ 至少一个子控件自适应填充剩余空间 -->
<VerticalLayout>
    <Label height="30" text="A" />
    <Label height="30" text="B" />
    <List vscrollbar="true" />  <!-- 自适应占满剩余空间 -->
</VerticalLayout>
```

```xml
<!-- ❌ TileLayout 没有设置 itemsize，子控件大小为 0 -->
<TileLayout>
    <Control bkimage="img1.png" />
</TileLayout>

<!-- ✅ 必须指定每格尺寸 -->
<TileLayout itemsize="100,100">
    <Control bkimage="img1.png" />
</TileLayout>
```

```xml
<!-- ❌ float 控件没有设置 pos，位置为 (0,0,0,0) 不可见 -->
<Button float="true" text="浮动" />

<!-- ✅ float 必须配合 pos 使用 -->
<Button float="true" pos="10,10,90,40" text="浮动" />
```

---

## 11. 总结

| 布局 | 排列方向 | 自适应方式 | 适用场景 |
|------|---------|-----------|---------|
| VerticalLayout | 从上到下 | 高度自适应平分 | 表单、列表页 |
| HorizontalLayout | 从左到右 | 宽度自适应平分 | 工具栏、标题栏 |
| TileLayout | 网格+自动换行 | 固定格子尺寸 | 图片墙、图标网格 |
| TabLayout | 切换显示 | 选中页占满 | 多页面切换 |
| Container | 无自动布局 | 绝对定位 | 特殊定位需求 |

核心记忆：**两轮遍历（统计 + 分配）+ EstimateSize 返回 0 表示自适应**。
