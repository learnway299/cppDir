# 11 - wxWidgets 实战场景考察

按场景组织口述题：目标 → 做法 → 验收 → 翻车点。

---

## 场景 1：从零搭主窗口

1. `wxApp::OnInit` 创建 `wxFrame`  
2. 中央 `wxPanel` + 根 `wxBoxSizer`  
3. 菜单/状态栏挂 Frame  
4. `Show` + `SetSizerAndFit` 或显式尺寸  
5. 验收：拉扯窗口布局正确、关闭进程干净  

**翻车**：Frame 上直接堆控件导致焦点/背景异常；忘记 SetSizer。

---

## 场景 2：自定义自绘控件

1. 派生 `wxPanel`/`wxWindow`  
2. Paint + 缓冲 DC  
3. 鼠标事件命中测试  
4. `RefreshRect` 局部刷新  
5. 提供 `GetBestSize` 融进 Sizer  

**翻车**：ClientDC 乱画被擦掉；全窗 Refresh 卡顿。

---

## 场景 3：后台任务 + 进度 UI

1. 工作线程跑任务  
2. 进度用 `CallAfter` 更新 `wxGauge`  
3. 取消标志 `atomic`  
4. 关闭窗口时停线程/弃用回调  

**翻车**：线程直接 SetValue；对话框关闭后回调崩溃。

---

## 场景 4：DPI / 高分屏糊、布局挤

1. 清单开启 DPI Aware（Windows）  
2. `FromDIP` 边距与最小尺寸  
3. 多套图标分辨率  
4. 100%/150%/200% 实测  

---

## 场景 5：国际化

1. `_()` / `wxGetTranslation`  
2. 目录与语言切换  
3. XRC 字符串可译  
4. 布局留出文字伸长空间（德文等）  

---

## 场景 6：与原生句柄互操作

1. `GetHandle()` 拿到 HWND/NSView  
2. 隔离在 platform 文件  
3. 消息钩子注意与 wx 泵冲突  

**翻车**：到处 `#ifdef` 泄漏进业务。

---

## 场景 7：卡顿排查

| 可疑 | 动作 |
|------|------|
| UI 线程 IO | 搬到工作线程 |
| 过度刷新 | 脏矩形、节流 |
| 巨量控件 | 虚拟化列表、复用 |
| 同步远程 | 异步 + 超时 |

---

## 场景 8：老 2.8 工程升级 3.2

1. 开 Unicode 构建，修 `wxString`/字符字面量  
2. 事件表逐步改 Bind  
3. 替换废弃 API  
4. HiDPI 回归  
5. 全平台 CI  

---

## 场景 9：选型题（口述）

「要做跨平台工具箱 UI，设计师要强皮肤？」  
→ 强皮肤偏 Qt/自绘；要系统原生感、许可宽松可 wx。  
「仅 Windows、完全自定义 IM 皮肤？」  
→ Duilib/类 DirectUI 更常见。

---

## 场景速查

| 场景 | 关键词 |
|------|--------|
| 主窗 | App、Panel、Sizer |
| 自绘 | PaintDC、缓冲、脏区 |
| 线程 | CallAfter |
| DPI | FromDIP、清单 |
| 升级 | Unicode、Bind |
| 选型 | 原生 vs 自绘 vs Qt |
