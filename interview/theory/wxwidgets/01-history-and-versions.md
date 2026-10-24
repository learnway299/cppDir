# 01 - wxWidgets 历史与主流版本

## 核心概念

**wxWidgets**（早期名 **wxWindows**）是开源的 **C++ 跨平台 GUI 工具包**：在各平台上尽量使用**原生控件**绘制界面，而不是像 Duilib 那样全自绘，也不同于 Qt 那套高度自有的控件与元对象体系。

一句话定位：**用一套 C++ API，在 Windows / macOS / Linux 上得到相对「原汁原味」的桌面观感。**

---

## 发展脉络（简述）

| 阶段 | 要点 |
|------|------|
| 1992 前后 | 爱丁堡大学项目起源，服务跨 Unix / Windows 的学术与商业需求 |
| 1990s–2000s | 以 **wxWindows** 之名扩散；后因商标原因更名为 **wxWidgets** |
| 2.x 时代 | 长期主流；奠定事件表、Sizer、多后端（MSW / GTK / macOS）格局 |
| 3.0（约 2013） | 现代化里程碑：Unicode 默认、API 清理、`Bind` 推广、C++ 更友好 |
| 3.1 / 3.2 | 持续改进 HiDPI、暗色主题、Wayland/GTK3+、CMake 支持与缺陷修复 |
| 今日 | 仍活跃于 IDE、科学软件、工具类桌面、部分遗留商业客户端；与 Qt 并存于「原生感 / LGPL 友好」选型中 |

> 面试不要求背年份，要能说清：**从 wxWindows → wxWidgets，以及 2.x → 3.x 的能力跃迁。**

---

## 主流版本特性对比

### wxWidgets 2.8 / 2.9（历史主流）

| 特性 | 说明 |
|------|------|
| 事件表宏 | `BEGIN_EVENT_TABLE` / `EVT_*` 仍是主流写法 |
| ANSI/Unicode | 构建时可切；字符串类型易踩坑 |
| 后端 | MSW、GTK2、Carbon/Cocoa 过渡期差异大 |
| 构建 | 常配合自带 bakefile / 手工工程 |

### wxWidgets 3.0（分水岭）

| 特性 | 说明 |
|------|------|
| Unicode 默认 | `wxString` 以 Unicode 为核心，减少双构建复杂度 |
| 事件连接 | 推荐 `Bind` / `Unbind`，类型更安全，支持绑定到任意仿函数 |
| API 现代化 | 废弃/整理一批 2.x 接口；更清晰的所有权与智能指针辅助 |
| XRC / AUI 等 | XML 资源、高级 UI 布局组件继续强化 |

### wxWidgets 3.1 → 3.2（近年主流）

| 特性 | 说明 |
|------|------|
| HiDPI / Per-Monitor DPI | Windows 高分屏、位图与字体缩放更可用 |
| 暗色模式 | 跟随系统主题的能力增强（平台相关） |
| GTK3 / Wayland | Linux 侧跟上桌面环境演进 |
| CMake | 官方/社区 CMake 集成成为默认工程实践 |
| WebView / Media | 可选后端嵌入浏览器或媒体能力（平台依赖） |

---

## 许可与生态定位

| 维度 | wxWidgets |
|------|-----------|
| 许可证 | 修改版 LGPL 风格（**wxWindows Library Licence**），静态链接相对友好（仍需核对条款） |
| 语言 | 以 C++ 为主；有绑定但面试主考 C++ |
| 典型竞品 | Qt（功能全、商业许可考量）、MFC（仅 Windows）、Duilib（Win 自绘）、Flutter/Electron（另一路线） |

**选型一句话**：要跨平台 + 原生观感 + C++ + 许可相对宽松 → 常提到 wxWidgets；要生态/QML/工具链一体化 → 常选 Qt。

---

## 常见面试题

**Q1：wxWidgets 和 Qt 怎么区分？**  
答：wx 偏**原生控件包装**，界面更「像系统」；Qt 控件与元对象/信号槽是自有体系，跨平台一致性与生态更强。许可与商业捆绑策略也不同。

**Q2：为什么要从 2.x 升到 3.x？**  
答：Unicode 默认、`Bind`、HiDPI、现代平台后端与构建；减少 ANSI/Unicode 双轨与陈旧 API。

**Q3：wxWindows 这个名字还用吗？**  
答：历史名称；现正式为 wxWidgets。文档/老书可能混用。

---

## 实战要点

- 新项目优先 **3.2 + CMake + Bind**，避免从 2.8 抄宏式样板。  
- 读老代码时先确认 Unicode 构建与事件绑定方式。  

---

## 下一步

- 架构：[02-architecture.md](02-architecture.md)
