# XML 布局与控件树

## 概述

Duilib 使用 XML 文件声明界面布局，运行时由 CDialogBuilder 解析 XML 并创建对应的控件树。这种声明式 UI 将界面描述与业务逻辑分离，也是实现换肤的基础。

---

## 1. XML 布局文件结构

### 基本格式

```xml
<?xml version="1.0" encoding="utf-8"?>
<Window size="800,600" caption="0,0,0,36" roundcorner="4,4">
    <Default name="Button" value="normalimage=&quot;btn_normal.png&quot;" />

    <VerticalLayout bkcolor="#FFFFFFFF">
        <HorizontalLayout height="36" bkcolor="#FF2D2D30">
            <Label text="My App" textcolor="#FFFFFFFF" />
            <Control />
            <Button name="btnMin" width="36" height="36" />
            <Button name="btnClose" width="36" height="36" />
        </HorizontalLayout>

        <VerticalLayout padding="10,10,10,10">
            <Edit name="editInput" height="30" text="请输入..." />
            <Button name="btnSubmit" height="36" text="提交" />
            <List name="listResult" vscrollbar="true" />
        </VerticalLayout>
    </VerticalLayout>
</Window>
```

### Window 节点属性

```xml
<Window
    size="800,600"           <!-- 窗口初始大小 -->
    sizebox="4,4,4,4"       <!-- 可拖拽调整大小的边距 -->
    caption="0,0,0,36"      <!-- 标题栏区域（用于拖动） -->
    roundcorner="4,4"       <!-- 圆角半径 -->
    mininfo="400,300"       <!-- 最小尺寸 -->
    maxinfo="1920,1080"     <!-- 最大尺寸 -->
>
```

---

## 2. CDialogBuilder 解析流程

### 整体流程

```
XML 文件 → CMarkup 解析为节点树 → CDialogBuilder 遍历
    ├── Window 节点 → 设置窗口属性
    ├── Default 节点 → 注册默认属性
    ├── Include 节点 → 递归加载外部 XML
    └── 控件节点 → 控件工厂创建实例 → SetAttribute → Add 到父容器
```

### Create 源码简化

```cpp
CControlUI* CDialogBuilder::Create(LPCTSTR pstrXML, UINT nID,
                                    IDialogBuilderCallback* pCallback,
                                    CPaintManagerUI* pManager)
{
    CMarkup xml;
    if (!xml.Load(pstrXML)) return NULL;

    CMarkupNode root = xml.GetRoot();

    // 处理 Window 节点属性
    if (_tcscmp(root.GetName(), _T("Window")) == 0) {
        for (int i = 0; i < root.GetAttributeCount(); i++) {
            pManager->SetWindowAttribute(
                root.GetAttributeName(i), root.GetAttributeValue(i));
        }
    }

    return _Parse(&root, NULL, pManager, pCallback);
}
```

### _Parse 递归构建

```cpp
CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot,
                                    CControlUI* pParent,
                                    CPaintManagerUI* pManager,
                                    IDialogBuilderCallback* pCallback)
{
    CControlUI* pReturn = NULL;

    for (CMarkupNode node = pRoot->GetChild(); node.IsValid();
         node = node.GetSibling()) {

        LPCTSTR pstrName = node.GetName();

        // 特殊节点处理
        if (_tcscmp(pstrName, _T("Default")) == 0) {
            pManager->AddDefaultAttributeList(
                node.GetAttributeValue(_T("name")),
                node.GetAttributeValue(_T("value")));
            continue;
        }

        if (_tcscmp(pstrName, _T("Include")) == 0) {
            CDialogBuilder subBuilder;
            CControlUI* pInclude = subBuilder.Create(
                node.GetAttributeValue(_T("source")), 0, pCallback, pManager);
            if (pInclude && pParent) {
                IContainerUI* pContainer = (IContainerUI*)pParent->GetInterface(_T("IContainer"));
                pContainer->Add(pInclude);
            }
            continue;
        }

        // 创建控件：先尝试自定义工厂，再尝试内置
        CControlUI* pControl = NULL;
        if (pCallback) pControl = pCallback->CreateControl(pstrName);
        if (!pControl) pControl = CreateControlFromName(pstrName);
        if (!pControl) continue;

        // 应用默认属性 + XML 显式属性
        LPCTSTR pDefault = pManager->GetDefaultAttributeList(pstrName);
        if (pDefault) pControl->ApplyAttributeList(pDefault);

        for (int i = 0; i < node.GetAttributeCount(); i++) {
            pControl->SetAttribute(node.GetAttributeName(i),
                                    node.GetAttributeValue(i));
        }

        // 加入父容器
        if (pParent) {
            IContainerUI* pContainer = (IContainerUI*)pParent->GetInterface(_T("IContainer"));
            if (pContainer) pContainer->Add(pControl);
        }

        // 递归处理子节点
        if (node.HasChildren()) {
            _Parse(&node, pControl, pManager, pCallback);
        }

        if (!pReturn) pReturn = pControl;
    }
    return pReturn;
}
```

---

## 3. 控件工厂机制

### 内置控件注册

```cpp
CControlUI* CreateControlFromName(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, _T("Label")) == 0)            return new CLabelUI;
    if (_tcscmp(pstrName, _T("Button")) == 0)           return new CButtonUI;
    if (_tcscmp(pstrName, _T("Edit")) == 0)             return new CEditUI;
    if (_tcscmp(pstrName, _T("List")) == 0)             return new CListUI;
    if (_tcscmp(pstrName, _T("Combo")) == 0)            return new CComboUI;
    if (_tcscmp(pstrName, _T("Option")) == 0)           return new COptionUI;
    if (_tcscmp(pstrName, _T("Progress")) == 0)         return new CProgressUI;
    if (_tcscmp(pstrName, _T("Slider")) == 0)           return new CSliderUI;
    if (_tcscmp(pstrName, _T("HorizontalLayout")) == 0) return new CHorizontalLayoutUI;
    if (_tcscmp(pstrName, _T("VerticalLayout")) == 0)   return new CVerticalLayoutUI;
    if (_tcscmp(pstrName, _T("TileLayout")) == 0)       return new CTileLayoutUI;
    if (_tcscmp(pstrName, _T("TabLayout")) == 0)        return new CTabLayoutUI;
    if (_tcscmp(pstrName, _T("Control")) == 0)          return new CControlUI;
    return NULL;
}
```

### 自定义控件工厂回调

```cpp
class CMainWnd : public CWindowWnd, public IDialogBuilderCallback {
public:
    CControlUI* CreateControl(LPCTSTR pstrClass) override {
        if (_tcscmp(pstrClass, _T("ChatBubble")) == 0)
            return new CChatBubbleUI;
        if (_tcscmp(pstrClass, _T("CircleAvatar")) == 0)
            return new CCircleAvatarUI;
        return NULL;
    }
};

// XML 中直接使用自定义标签
// <ChatBubble name="bubble1" text="Hello!" direction="left" />
```

---

## 4. 属性系统

### SetAttribute 统一入口

```cpp
void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("width")) == 0)
        SetFixedWidth(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("height")) == 0)
        SetFixedHeight(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("name")) == 0)
        SetName(pstrValue);
    else if (_tcscmp(pstrName, _T("text")) == 0)
        SetText(pstrValue);
    else if (_tcscmp(pstrName, _T("visible")) == 0)
        SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("enabled")) == 0)
        SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("bkcolor")) == 0)
        SetBkColor(ParseColor(pstrValue));
    else if (_tcscmp(pstrName, _T("padding")) == 0) {
        RECT rc = { 0 };
        _stscanf(pstrValue, _T("%d,%d,%d,%d"), &rc.left, &rc.top, &rc.right, &rc.bottom);
        SetPadding(rc);
    }
}
```

### 属性值格式

| 类型 | 格式 | 示例 |
|------|------|------|
| 整数 | 直接数字 | `width="100"` |
| 布尔 | true/false | `visible="true"` |
| 颜色 | #AARRGGBB | `bkcolor="#FF2D2D30"` |
| 矩形 | l,t,r,b | `padding="10,5,10,5"` |
| 图片 | 路径+可选参数 | `bkimage="bg.png corner='5,5,5,5'"` |

### Default 默认属性

```xml
<!-- 定义全局默认值，所有同类控件生效 -->
<Default name="Button" value="height=&quot;36&quot; normalimage=&quot;btn.png&quot;" />
```

显式属性优先级高于默认属性，类似 CSS 内联样式覆盖全局样式。

---

## 5. Include 与布局复用

```xml
<!-- main.xml -->
<VerticalLayout>
    <Include source="titlebar.xml" />
    <Include source="sidebar.xml" />
    <VerticalLayout name="content" />
</VerticalLayout>

<!-- titlebar.xml -->
<HorizontalLayout height="36" bkcolor="#FF2D2D30">
    <Label text="Title" textcolor="#FFFFFFFF" />
    <Control />
    <Button name="btnClose" width="36" />
</HorizontalLayout>
```

---

## 6. 控件树运行时操作

### 按名称查找

```cpp
CControlUI* pBtn = m_pm.FindControl(_T("btnSubmit"));
CButtonUI* pButton = static_cast<CButtonUI*>(pBtn);
pButton->SetText(_T("已提交"));
pButton->SetEnabled(false);
```

### 动态添加控件

```cpp
CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listResult")));

CListLabelElementUI* pItem = new CListLabelElementUI;
pItem->SetText(_T("新数据"));
pItem->SetFixedHeight(24);
pList->Add(pItem);
```

### 动态移除

```cpp
pList->RemoveAt(0);    // 移除第一项
pList->RemoveAll();    // 清空
```

---

## 7. 布局计算流程（SetPos 递归）

```cpp
void CVerticalLayoutUI::SetPos(RECT rc)
{
    CContainerUI::SetPos(rc);

    int cyAvailable = rc.bottom - rc.top;
    int cyFixed = 0;
    int nExpandCount = 0;

    // 第一轮：统计固定高度和自适应控件数
    for (int i = 0; i < GetCount(); i++) {
        SIZE sz = GetItemAt(i)->EstimateSize({rc.right - rc.left, 0});
        if (sz.cy > 0) cyFixed += sz.cy;
        else nExpandCount++;
    }

    // 第二轮：分配剩余空间，递归设置子控件位置
    int cyExpand = nExpandCount > 0 ? (cyAvailable - cyFixed) / nExpandCount : 0;
    int yPos = rc.top;

    for (int i = 0; i < GetCount(); i++) {
        CControlUI* pChild = GetItemAt(i);
        SIZE sz = pChild->EstimateSize({rc.right - rc.left, 0});
        int cyItem = (sz.cy > 0) ? sz.cy : cyExpand;

        RECT rcChild = { rc.left, yPos, rc.right, yPos + cyItem };
        pChild->SetPos(rcChild);
        yPos += cyItem;
    }
}
```

---

## 8. 面试常见问题

### Q1: Duilib XML 解析用的什么库？

自带轻量级解析器 CMarkup，不依赖第三方（TinyXML/RapidXML）。只支持基本的节点遍历和属性读取，不支持 XPath。

### Q2: 如何实现 XML 热重载？

```cpp
void ReloadUI() {
    m_pm.RevertStatic();
    CDialogBuilder builder;
    CControlUI* pRoot = builder.Create(_T("main.xml"), 0, this, &m_pm);
    m_pm.AttachDialog(pRoot);
    m_pm.AddNotifier(this);
    m_pm.NeedUpdate();
}
```

### Q3: FindControl 按名称查找的时间复杂度？

默认递归遍历整棵树，O(n)。优化方案是内部维护 name → control 的 HashMap，部分分支已实现。

### Q4: Include 嵌套过深有什么问题？

解析耗时增加、控件名可能冲突、调试时难以定位。建议 2-3 层以内。

---

## 9. 常见陷阱

```xml
<!-- ❌ 控件名重复，FindControl 只能找到其中一个 -->
<Button name="btn" text="按钮1" />
<Button name="btn" text="按钮2" />

<!-- ✅ 使用唯一名称 -->
<Button name="btnSave" text="保存" />
<Button name="btnCancel" text="取消" />
```

```xml
<!-- ❌ Default value 中引号未转义 -->
<Default name="Button" value="text="hello"" />

<!-- ✅ 用 &quot; 转义 -->
<Default name="Button" value="text=&quot;hello&quot;" />
```

```cpp
// ❌ 动态创建控件后未加入容器，不会显示也收不到事件
CButtonUI* pBtn = new CButtonUI;
pBtn->SetText(_T("test"));

// ✅ 必须 Add 到已关联 PaintManager 的容器
pContainer->Add(pBtn);
```

---

## 10. 总结

| 特性 | 说明 |
|------|------|
| **布局描述** | XML 声明式，支持嵌套和 Include 复用 |
| **解析器** | 自带 CMarkup 轻量解析 |
| **控件创建** | 工厂模式：内置 + 自定义回调 |
| **属性系统** | SetAttribute 统一入口，字符串解析 |
| **默认属性** | Default 节点定义全局默认值 |
| **布局计算** | SetPos 递归，两轮遍历（统计 + 分配） |
| **运行时** | FindControl 查找 + 动态增删控件 |
