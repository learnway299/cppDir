# 04 - 窗口与控件体系

## 核心概念

界面是一棵 **`wxWindow` 树**：`wxFrame` / `wxDialog` 为顶层，中间常用 `wxPanel`，叶子为 `wxButton`、`wxTextCtrl`、`wxListCtrl` 等。

**所有权规则（高频考点）**：子窗口由父窗口拥有，父销毁时级联销毁子对象；用 `new` 创建的子控件通常**不要**再 `delete`（交给父）。

```
wxFrame
 └── wxPanel
      ├── wxStaticText
      ├── wxTextCtrl
      └── wxButton
```

---

## 常用顶层与容器

| 类 | 用途 |
|----|------|
| `wxFrame` | 主窗口：菜单、状态栏、工具栏 |
| `wxDialog` | 对话框：模态/非模态 |
| `wxPanel` | 逻辑分组、Tab 页内容、接收焦点的容器 |
| `wxScrolledWindow` | 滚动区域 |
| `wxNotebook` / `wxAuiNotebook` | 分页 |

## 常用控件（点名即可）

| 类别 | 例子 |
|------|------|
| 输入 | `wxTextCtrl`、`wxComboBox`、`wxCheckBox`、`wxRadioButton`、`wxSlider` |
| 列表 | `wxListBox`、`wxListCtrl`、`wxTreeCtrl`、`wxDataViewCtrl` |
| 显示 | `wxStaticText`、`wxStaticBitmap`、`wxGauge` |
| 命令 | `wxButton`、`wxBitmapButton`、`wxMenu` / `wxMenuBar` |

平台上同一控件类可能映射到不同原生实现，行为细节（如多行编辑快捷键）会有差异。

---

## ID、标签与查找

- 控件 ID：`wxID_ANY` 或自定义正整数；标准 ID（`wxID_OK`）利于对话按钮约定。  
- `FindWindow` / `FindWindowByName` / `XRCCTRL`：运行时查找；大型 UI 更推荐成员指针直持。  

---

## 最佳实践心智

1. **Frame 只做壳**：内容放 Panel + Sizer。  
2. **少绝对坐标**：位置交给 Sizer。  
3. **命名与 Name 属性**：自动化测试/XRC 友好。  
4. **禁用而非销毁**：频繁切换可用 `Show`/`Enable`。  

---

## 常见面试题

**Q1：子控件要手动 delete 吗？**  
答：一般不要。父窗口销毁会销毁子窗口；双重释放是常见坑。例外：你显式 `Destroy` 或脱离父链的对象需理清所有权。

**Q2：`wxFrame` 和 `wxDialog` 怎么选？**  
答：主功能窗口用 Frame；临时交互、拿返回值用 Dialog（常 `ShowModal`）。

**Q3：为什么推荐中间加 `wxPanel`？**  
答：统一 Tab 遍历、背景、Sizer 根节点；部分平台 Frame 直接塞控件焦点行为怪异。

---

## 实战要点

- 析构顺序：先解绑事件再销毁，避免回调到半销毁对象。  
- 原生句柄 `GetHandle()` 仅在必须互操作时使用，并隔离平台代码。  

---

## 下一步

- 事件系统：[05-event-system.md](05-event-system.md)  
- 布局：[06-sizer-layout.md](06-sizer-layout.md)
