# Development Tools

开发工具知识 - 面试常考的工程化技能。

---

## 知识体系

### 版本控制 - 1 个主题

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 01 | [Git 深入](01-git.md) | 工作原理、merge vs rebase、冲突解决、高级操作 |

### 调试工具 - 2 个主题

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 02 | [WinDbg 命令体系](02-windbg.md) | 基础命令、进阶命令、符号、断点、堆分析、TTD |
| 03 | [WinDbg 实战与特殊场景](03-windbg-practice.md) | Crash Dump 分析、堆损坏、无符号定位、死锁、延迟崩溃 |

### 构建工具 - 2 个主题

| 编号 | 主题 | 核心内容 |
|------|------|----------|
| 04 | [CMake 基础与现代用法](04-cmake-basics.md) | CMake 工作模型、target-based、PRIVATE/PUBLIC/INTERFACE、库类型、生成器 |
| 05 | [CMake 工程实战与常见问题](05-cmake-practice.md) | 多模块工程、第三方依赖、Qt/MSVC、安装导出、构建排错 |

---

## 面试重点

### 必问话题

1. **Git 基础**
   - 工作区/暂存区/仓库的区别
   - 常用命令（add、commit、push、pull）

2. **分支与合并**
   - merge vs rebase 的区别与选择
   - pull vs pull --rebase
   - 冲突解决策略

3. **撤销操作**
   - reset vs revert vs checkout
   - 如何恢复误删的提交

4. **WinDbg 基础**
   - `!analyze -v`、`.ecxr`、`kP` 的使用顺序
   - 符号路径配置与 PDB 加载排查
   - 查看线程、调用栈、寄存器、内存的常用命令

5. **WinDbg 实战**
   - Access Violation 如何分析
   - 堆内存损坏如何定位真正破坏者
   - 无符号文件时如何通过模块基址和 RVA 定位崩溃
   - 死锁、句柄泄漏、栈损坏如何排查

6. **CMake 基础**
   - CMake 与 Makefile/Ninja/Visual Studio 的关系
   - out-of-source build 的意义
   - 现代 CMake target-based 写法
   - `PRIVATE` / `PUBLIC` / `INTERFACE` 的区别

7. **CMake 工程实践**
   - 如何组织大型 C++ 多模块项目
   - `find_package`、`FetchContent`、`ExternalProject` 的区别
   - Qt 项目中的 `AUTOMOC` / `AUTOUIC` / `AUTORCC`
   - MSVC 下 DLL 导出、运行时库和 Debug/Release 匹配

### 加分项

1. Git 对象模型（blob、tree、commit）
2. reflog 救命技巧
3. cherry-pick、bisect 使用场景
4. Git Hooks 自动化
5. 工作流对比（Git Flow vs GitHub Flow）
6. Page Heap / Application Verifier 配合 WinDbg 定位堆损坏
7. 数据断点 `ba w` 定位内存写入者
8. Time Travel Debugging (TTD) 反向调试
9. 无符号场景下通过反汇编、MAP、栈扫描定位问题
10. CMake 安装导出与 `find_package(MyLib CONFIG REQUIRED)` 支持
11. Generator Expression 的工程应用
12. vcpkg/conan 与 CMake toolchain 集成

---

## 文件列表

```
dev-tools/
├── README.md                 # 本文件
├── 01-git.md                 # Git 深入
├── 02-windbg.md              # WinDbg 命令体系
├── 03-windbg-practice.md     # WinDbg 实战与特殊场景
├── 04-cmake-basics.md        # CMake 基础与现代用法
└── 05-cmake-practice.md      # CMake 工程实战与常见问题
```
