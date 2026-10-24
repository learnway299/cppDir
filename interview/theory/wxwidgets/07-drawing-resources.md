# 07 - 绘图与资源

## 核心概念

需要自绘或自定义外观时，使用 **Device Context（DC）** 系列 API；资源方面常见 **位图 / 图标 / XRC XML 界面**。

wx 仍是原生控件框架——自绘是「增强」，不是默认路径。

---

## DC 家族

| 类 | 场景 |
|----|------|
| `wxPaintDC` | 在 `EVT_PAINT` 内绘制客户区（正确做法） |
| `wxClientDC` | 临时画在客户区（慎用，可能被绘制擦除） |
| `wxBufferedPaintDC` | 双缓冲，防闪烁 |
| `wxMemoryDC` | 画到位图再 blt |
| `wxWindowDC` / `wxScreenDC` | 含非客户区 / 屏幕（少用） |

**规则**：窗口刷新应触发 Paint 事件，在 `OnPaint` 里用 `wxPaintDC`；不要在任意时机乱建 PaintDC。

### 自绘控件要点

1. `Bind(wxEVT_PAINT, ...)`  
2. 需要时 `Bind(wxEVT_ERASE_BACKGROUND, ...)` 并 Skip/清空防闪烁  
3. 数据变了 `Refresh` / `RefreshRect`  
4. 尺寸变了参与 Sizer 的 `GetBestSize`  

---

## 位图与图像

- `wxBitmap` / `wxImage`：平台位图 vs 平台无关像素  
- 资源加载：文件、`wxImage::LoadFile`、嵌入 XPM、Windows 资源  
- HiDPI：提供 `@2x` 或按 DIP 缩放，避免糊图标  

---

## XRC（XML Resources）

用 XML 描述界面，运行时 `wxXmlResource` 加载：

- 好处：UI 与逻辑分离、翻译友好  
- 代价：调试链路更长；复杂动态 UI 仍要代码创建  

`XRCCTRL(parent, "name", Type)` 取控件指针是常考用法。

---

## 常见面试题

**Q1：为什么 Paint 里必须用 wxPaintDC？**  
答：与系统绘制协议匹配，确保绘制区域与 BeginPaint/EndPaint 语义正确；乱用 DC 会导致闪烁或画不出来。

**Q2：如何防闪烁？**  
答：`wxBufferedPaintDC`、减少全窗 Refresh、处理 EraseBackground、避免 CleintDC 频繁重画。

**Q3：XRC 和纯代码创建怎么选？**  
答：相对静态表单用 XRC；强动态/自绘优先代码 + Sizer。

---

## 实战要点

- 重绘制热点：脏矩形，不要动辄全窗刷。  
- 主题色用系统色（`wxSYS_COLOUR_*`）更易跟暗色模式。  

---

## 下一步

- [08-threading-dialogs.md](08-threading-dialogs.md)
