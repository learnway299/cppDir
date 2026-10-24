# 09 - 跨平台差异与构建

## 核心概念

同一套源码过三关：**条件编译隔离平台差异**、**CMake/构建选项选对 port**、**安装包与依赖按平台交付**。

---

## 条件编译

常见宏：

| 宏 | 平台 |
|----|------|
| `__WXMSW__` | Windows |
| `__WXGTK__` | Linux GTK |
| `__WXOSX__` / `__WXMAC__` | macOS |

原则：

- 业务逻辑平台无关  
- 少数原生互操作收到独立 `.cpp` 或 `#ifdef` 块  
- 路径、文件对话框、托盘、全局热键等最爱冒出差异  

---

## 构建要点（CMake 向）

面试口述即可：

1. `find_package(wxWidgets)` 或 FetchContent / 子模块  
2. `components`：core、base、adv、aui、html、webview…  
3. Unicode、共享库/静态库选择影响分发体积与许可展示  
4. Windows：子系统、清单（DPI Aware）、运行库匹配  
5. macOS：Bundle、Info.plist、签名公证  
6. Linux：注意 GTK3 依赖与 `.desktop`  

---

## 运行时差异清单（高频踩坑）

| 点 | 说明 |
|----|------|
| 路径分隔与大小写 | 资源路径不要写死 `\` |
| 字体与默认尺寸 | Sizer + DIP |
| 文件对话框通配符 | 语法略有不同 |
| 剪贴板 / DND | API 统一，行为略异 |
| 单实例 | 锁文件或平台 API 封装 |

---

## 常见面试题

**Q1：如何组织跨平台工程？**  
答：公共 UI 代码 + thin 平台层；CMake 按 OS 链不同库；CI 矩阵编 Win/Linux/macOS。

**Q2：静态链接 wx 要注意什么？**  
答：体积、许可声明、与插件 ABI；部分平台动态更常见。

**Q3：DPI 问题归谁？**  
答：清单声明感知 + `FromDIP` + 多分辨率图标 + 真机测；不只靠 wx 默认。

---

## 实战要点

- CI 早亮平台红灯。  
- 资源用相对路径 + 统一查找函数。  

---

## 下一步

- [10-theory-interview-qa.md](10-theory-interview-qa.md)  
- [11-practice-scenarios.md](11-practice-scenarios.md)
