# CMake 工程实战与常见问题

## 概述

CMake 面试的进阶部分通常不再停留在命令语法，而是考察能否维护真实 C++ 工程：如何组织多模块项目、管理第三方依赖、集成 Qt/MSVC、处理安装导出、定位构建失败与运行时 DLL 问题。

---

## 1. 推荐工程结构

```text
project/
├── CMakeLists.txt
├── cmake/                 # 自定义 CMake 模块、工具函数
├── include/               # 对外公开头文件
│   └── mylib/
├── src/                   # 源文件与私有头文件
├── tests/                 # 单元测试
├── examples/              # 示例程序
├── third_party/           # 第三方源码或适配层
└── build/                 # 构建目录，不提交
```

顶层 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

option(MYPROJECT_BUILD_TESTS "Build tests" ON)
option(MYPROJECT_BUILD_EXAMPLES "Build examples" ON)

add_subdirectory(src)

if(MYPROJECT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MYPROJECT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
```

`src/CMakeLists.txt`：

```cmake
add_library(mylib
    core.cpp
    network.cpp
)

target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(mylib PUBLIC cxx_std_17)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE mylib)
```

---

## 2. 多模块依赖组织

### 推荐依赖方向

```text
app -> ui -> network -> core
            -> storage -> core
```

原则：
- 底层模块不依赖上层模块
- 公共能力下沉到 core
- 避免循环依赖
- 每个模块独立声明自己的 include、宏、链接库

```cmake
add_library(core src/core.cpp)
target_include_directories(core PUBLIC include)

add_library(network src/network.cpp)
target_link_libraries(network PUBLIC core)

add_library(ui src/ui.cpp)
target_link_libraries(ui PUBLIC network PRIVATE resources)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE ui)
```

### 避免循环依赖

```text
错误：
core -> ui
ui   -> core

改法：
1. 抽出 interface/common 模块
2. 用回调/接口反转依赖
3. 拆分数据结构和业务实现
```

---

## 3. 第三方依赖管理

### 常见方式对比

| 方式 | 适用场景 | 特点 |
|------|----------|------|
| `find_package` | 系统已安装或包管理器安装 | 最推荐，使用 imported target |
| `FetchContent` | 小型源码依赖 | 配置阶段拉源码并纳入主工程 |
| `ExternalProject_Add` | 独立构建的大型依赖 | 外部构建，不自然成为 target |
| Git submodule | 固定源码版本 | 简单直接，但需要手动维护 |
| vcpkg/conan | 跨平台包管理 | 适合团队统一依赖版本 |

### find_package

```cmake
find_package(fmt CONFIG REQUIRED)
target_link_libraries(app PRIVATE fmt::fmt)
```

`find_package` 有两种模式：

| 模式 | 文件 | 说明 |
|------|------|------|
| Module mode | `FindXXX.cmake` | CMake 或项目提供查找脚本 |
| Config mode | `XXXConfig.cmake` | 依赖库安装时导出的配置文件 |

### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

target_link_libraries(app PRIVATE fmt::fmt)
```

### find_package 失败排查

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=D:/libs/fmt/install
cmake -S . -B build --debug-find
```

重点检查：
- `CMAKE_PREFIX_PATH`
- `XXX_DIR`
- 是否有 `XXXConfig.cmake`
- Debug/Release 架构是否匹配
- x86/x64 是否匹配

---

## 4. Qt + CMake 集成

### Qt6 基本写法

```cmake
cmake_minimum_required(VERSION 3.20)
project(QtApp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(QtApp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
)

target_link_libraries(QtApp PRIVATE Qt6::Widgets)
```

### AUTOMOC / AUTOUIC / AUTORCC

| 选项 | 作用 |
|------|------|
| `CMAKE_AUTOMOC` | 自动对含 `Q_OBJECT` 的头/源文件运行 moc |
| `CMAKE_AUTOUIC` | 自动处理 `.ui` 文件 |
| `CMAKE_AUTORCC` | 自动处理 `.qrc` 资源文件 |

**面试题：Qt 的 MOC 在 CMake 中怎么处理？**

开启 `CMAKE_AUTOMOC` 后，CMake 会扫描 target 源文件中包含 `Q_OBJECT` 等宏的类，自动调用 moc 生成元对象代码，并把生成文件加入构建。

---

## 5. Windows / MSVC 实战

### DLL 导出

```cpp
// export.h
#pragma once

#ifdef MYLIB_BUILDING
#  define MYLIB_API __declspec(dllexport)
#else
#  define MYLIB_API __declspec(dllimport)
#endif

class MYLIB_API MyClass {
public:
    void run();
};
```

```cmake
add_library(mylib SHARED src/mylib.cpp)
target_compile_definitions(mylib PRIVATE MYLIB_BUILDING)
```

也可以快速导出所有符号：

```cmake
set_target_properties(mylib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
```

但正式项目更推荐显式导出宏。

### MSVC 运行时库

```cmake
# CMake 3.15+
set_property(TARGET app PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)
```

| 选项 | 含义 |
|------|------|
| `/MD` | 动态链接 CRT，Release |
| `/MDd` | 动态链接 Debug CRT |
| `/MT` | 静态链接 CRT，Release |
| `/MTd` | 静态链接 Debug CRT |

### PDB 输出目录

```cmake
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb
)
```

### Post-build 拷贝 DLL

```cmake
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:mylib>
        $<TARGET_FILE_DIR:app>
)
```

---

## 6. 安装与导出

### 基本 install

```cmake
install(TARGETS mylib
    EXPORT MyLibTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)
```

### 导出 targets

```cmake
install(EXPORT MyLibTargets
    FILE MyLibTargets.cmake
    NAMESPACE MyLib::
    DESTINATION lib/cmake/MyLib
)
```

### 生成 Config 文件

```cmake
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MyLibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibConfig.cmake
    INSTALL_DESTINATION lib/cmake/MyLib
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibConfigVersion.cmake
    DESTINATION lib/cmake/MyLib
)
```

使用者：

```cmake
find_package(MyLib CONFIG REQUIRED)
target_link_libraries(app PRIVATE MyLib::mylib)
```

---

## 7. 实战命令组合

### Ninja 单配置构建

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
cmake --install build --prefix install
```

### Visual Studio 多配置构建

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --prefix install
```

### 清理缓存重新配置

```bash
cmake --fresh -S . -B build
# 或删除 build/CMakeCache.txt 和 build/CMakeFiles
```

---

## 8. 常见问题与排查

### Q1: Windows 下运行时找不到 DLL

表现：

```text
The code execution cannot proceed because xxx.dll was not found.
```

解决：
- 将 DLL 复制到 exe 目录
- 将 DLL 目录加入 PATH
- 使用 post-build copy
- 安装阶段将 DLL 放入 `bin`

```cmake
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE_DIR:app>
)
```

Qt 项目更常用 `windeployqt`：

```cmake
add_custom_command(TARGET app POST_BUILD
    COMMAND Qt6::windeployqt $<TARGET_FILE:app>
)
```

### Q2: Debug 链接了 Release 第三方库

表现：
- 运行时崩溃
- STL/CRT 内存跨模块释放异常
- `_ITERATOR_DEBUG_LEVEL` 不匹配

排查：
- 检查 imported target 是否区分 Debug/Release
- 检查库文件名是否带 `d`
- 检查 MSVC Runtime `/MDd` vs `/MD`

### Q3: `find_package` 找不到库

```bash
cmake -S . -B build --debug-find
cmake -S . -B build -Dfmt_DIR=D:/fmt/lib/cmake/fmt
cmake -S . -B build -DCMAKE_PREFIX_PATH=D:/fmt
```

优先使用依赖库安装后提供的 `XXXConfig.cmake`，而不是手写 include/lib 路径。

### Q4: `CMAKE_BUILD_TYPE` 无效

如果使用 Visual Studio 或 Xcode，这是正常的。它们是多配置生成器，应使用：

```bash
cmake --build build --config Release
```

### Q5: 新增 `.cpp` 后没有参与编译

如果显式列源文件，需要手动添加到 `CMakeLists.txt`。如果使用 `file(GLOB)`，旧版 CMake 可能不会自动重新 configure；可加 `CONFIGURE_DEPENDS`，但大型项目仍推荐显式列出。

---

## 9. 常见面试问题

### Q1: 如何组织一个大型 C++ CMake 项目？

按模块拆 target，每个模块独立 `add_library`，使用 `target_include_directories` 和 `target_link_libraries` 声明依赖，顶层只负责选项、全局策略和 `add_subdirectory`。依赖方向保持单向，公共能力下沉，避免循环依赖。

### Q2: `find_package` 的 Config mode 和 Module mode 区别？

Module mode 查找 `FindXXX.cmake`；Config mode 查找库安装时提供的 `XXXConfig.cmake`。现代 CMake 更推荐 Config mode，因为它能导出 imported target，携带完整 usage requirements。

### Q3: FetchContent 和 ExternalProject 的区别？

`FetchContent` 在 configure 阶段拉取源码，并通过 `add_subdirectory` 纳入当前构建，依赖 target 可直接使用。`ExternalProject_Add` 是外部独立构建，默认不成为当前工程 target，适合大型依赖或需要完全隔离构建的项目。

### Q4: 如何让自己的库支持别人 `find_package`？

需要：
1. target 使用正确的 `BUILD_INTERFACE` / `INSTALL_INTERFACE`
2. `install(TARGETS ... EXPORT ...)`
3. `install(EXPORT ... NAMESPACE ...)`
4. 生成并安装 `MyLibConfig.cmake` 和 `MyLibConfigVersion.cmake`
5. 安装公开头文件

### Q5: 如何处理 Qt 项目中的 MOC/UIC/RCC？

开启 `CMAKE_AUTOMOC`、`CMAKE_AUTOUIC`、`CMAKE_AUTORCC`，并将 `.h/.ui/.qrc` 加入 target 源文件。链接 `Qt6::Widgets` 等 imported target。

---

## 10. 常见陷阱

```cmake
# ❌ 安装导出时使用源码绝对路径，安装后不可移植
target_include_directories(mylib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# ✅ 区分构建树与安装树
target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

```cmake
# ❌ 直接写库文件路径，丢失依赖传播信息
target_link_libraries(app PRIVATE D:/libs/foo.lib)

# ✅ 优先使用 imported target
target_link_libraries(app PRIVATE Foo::Foo)
```

```cmake
# ❌ PUBLIC/PRIVATE 写错，消费者编译失败
# mylib 的 public header 包含 <fmt/core.h>，但 fmt 是 PRIVATE
target_link_libraries(mylib PRIVATE fmt::fmt)

# ✅ 应该传播给消费者
target_link_libraries(mylib PUBLIC fmt::fmt)
```

---

## 11. 总结

| 场景 | 核心做法 |
|------|---------|
| **大型项目** | 多 target 拆分，依赖单向，顶层统一配置 |
| **第三方库** | 优先 `find_package` + imported target |
| **源码依赖** | 小型用 FetchContent，大型隔离用 ExternalProject |
| **Qt 项目** | AUTOMOC/AUTOUIC/AUTORCC + Qt imported target |
| **MSVC** | 关注 DLL 导出、运行时库、PDB、Debug/Release 匹配 |
| **安装导出** | `install(EXPORT)` + Config.cmake + INSTALL_INTERFACE |
| **排查失败** | `--debug-find`、清 CMakeCache、检查生成器与配置 |
