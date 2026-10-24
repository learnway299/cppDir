# 06 - Sizer 布局管理

## 核心概念

**wxSizer** 负责子窗口的位置与尺寸，替代绝对坐标。窗口 `SetSizer` / `SetSizerAndFit` 后，尺寸变化时递归重新计算布局。

```
wxBoxSizer(wxVERTICAL)
 ├── header (proportion=0)
 ├── content (proportion=1)  ← 吃掉剩余空间
 └── buttons (wxHORIZONTAL, proportion=0)
```

---

## 常用 Sizer

| 类 | 用途 |
|----|------|
| `wxBoxSizer` | 水平/垂直条带，最常用 |
| `wxGridSizer` | 等分网格 |
| `wxFlexGridSizer` | 网格 + 可伸展行列 |
| `wxGridBagSizer` | 单元格跨行跨列 |
| `wxStaticBoxSizer` | 带分组框 |
| `wxWrapSizer` | 自动换行流式 |

### 添加子项关键参数

`Add(window, proportion, flag, border)`

| 参数 | 含义 |
|------|------|
| proportion | 同向空间分配权重；0 表示按最小尺寸 |
| flag | `wxEXPAND`、对齐、`wxALL` 等边框方向 |
| border | 边距像素 |

---

## 布局原则

1. **嵌套 Sizer** 表达复杂结构，而不是一层塞所有控件。  
2. **最小尺寸**：控件 `GetMinSize` + `SetMinSize` 影响收缩下限。  
3. **`Fit` / `SetSizeHints`**：让顶层窗口包住内容。  
4. **`Layout()`**：手动改子项后强制重新排。  
5. **隐藏控件**：`Show(false)` 后需 `Layout`，Sizer 可忽略隐藏项（视 flag）。  

---

## DPI 与跨平台

高分屏下 border 写死「5px」可能偏小/偏大；优先：

- 用 `FromDIP` / `ToDIP`（3.x）转换  
- 依赖字体度量而非魔法数  
- 真机在 100%/150%/200% 缩放验收  

---

## 常见面试题

**Q1：proportion=0 和 1 的区别？**  
答：0 不参与多余空间分配；1 及以上按权重分剩余空间。

**Q2：为什么窗口拉大控件不动？**  
答：缺 `wxEXPAND` 或 proportion 全 0；或未 `SetSizer`。

**Q3：Sizer 与绝对定位混用可以吗？**  
答：不推荐；Sizer 管理的子项不要再手写冲突的 `SetPosition`。

---

## 实战要点

- 先画线框层级再写代码。  
- 动态增删控件后调用 `Layout` + 必要时 `Fit`。  
- 列表类控件通常 `proportion=1` + `wxEXPAND`。  

---

## 下一步

- 绘图资源：[07-drawing-resources.md](07-drawing-resources.md)  
- 线程对话框：[08-threading-dialogs.md](08-threading-dialogs.md)
