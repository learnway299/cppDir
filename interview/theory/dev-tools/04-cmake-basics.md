# CMake 基础与现代用法

## 概述

CMake 是 C/C++ 项目中最常见的跨平台构建系统生成器。它本身不是编译器，也不是最终的构建工具，而是根据 `CMakeLists.txt` 生成 Ninja、Makefile、Visual Studio Solution、Xcode Project 等构建文件，再由对应工具调用 MSVC/GCC/Clang 完成编译链接。

---

## 1. CMake 工作模型

### CMake 不是编译器

```text
CMakeLists.txt
    │
    ▼ configure / generate
build system（Ninja / Makefile / Visual Studio / Xcode）
    │
    ▼ build
compiler + linker（MSVC / GCC / Clang）
    │
    ▼
可执行文件 / 静态库 / 动态库
```

### 常用命令

```bash
# 推荐的 out-of-source build
cmake -S . -B build
cmake --build build

# 指定生成器
cmake -S . -B build -G Ninja

# 指定构建类型（单配置生成器）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j

# Visual Studio 多配置生成器
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

---

## 2. 最小 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)

project(HelloCMake
    VERSION 1.0.0
    LANGUAGES CXX
)

add_executable(hello
    main.cpp
)

target_compile_features(hello PRIVATE cxx_std_17)
```

### 基本命令解释

| 命令 | 作用 |
|------|------|
| `cmake_minimum_required` | 指定最低 CMake 版本，同时确定策略行为 |
| `project` | 定义项目名称、版本、语言 |
| `add_executable` | 创建可执行文件 target |
| `add_library` | 创建库 target |
| `target_link_libraries` | 为 target 链接依赖 |
| `target_compile_features` | 指定需要的 C++ 特性 |

---

## 3. Source Tree 与 Build Tree

### 推荐 out-of-source build

```text
project/                  # Source Tree
├── CMakeLists.txt
├── include/
├── src/
└── build/                # Build Tree，不提交到 Git
    ├── CMakeCache.txt
    ├── CMakeFiles/
    └── build.ninja / .sln
```

### 为什么不推荐源码目录内构建？

| 问题 | 说明 |
|------|------|
| 污染源码树 | 生成大量中间文件 |
| 难以清理 | 不知道哪些是生成物 |
| 多配置困难 | Debug/Release 产物混在一起 |
| 不利于版本控制 | 容易误提交构建文件 |

---

## 4. 变量、缓存与 option

### 普通变量

```cmake
set(MY_SRC main.cpp app.cpp)
message(STATUS "sources: ${MY_SRC}")

add_executable(app ${MY_SRC})
```

### Cache 变量

```cmake
set(MY_FEATURE ON CACHE BOOL "Enable my feature")
```

命令行覆盖：

```bash
cmake -S . -B build -DMY_FEATURE=OFF
```

### option

```cmake
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

### 作用域

```cmake
set(VAR "root")
add_subdirectory(src)
# 子目录能读取父目录变量，但默认修改不会反向影响父目录
```

如果要向父作用域传值：

```cmake
set(VAR "new value" PARENT_SCOPE)
```

---

## 5. Target-based CMake

### 现代 CMake 的核心思想

现代 CMake 推荐以 target 为中心组织构建信息：

```cmake
add_library(core src/core.cpp)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(core PUBLIC cxx_std_17)

target_compile_definitions(core
    PRIVATE CORE_BUILDING
    PUBLIC CORE_USE_UTF8
)
```

每个 target 自己携带：
- include 路径
- 编译宏
- 编译选项
- 链接库
- C++ 标准要求
- 安装导出信息

### 不推荐全局命令

```cmake
# ❌ 旧式写法：影响当前目录及子目录所有 target
include_directories(include)
add_definitions(-DUNICODE)
link_libraries(ws2_32)

# ✅ 现代写法：只作用于指定 target
add_executable(app main.cpp)
target_include_directories(app PRIVATE include)
target_compile_definitions(app PRIVATE UNICODE)
target_link_libraries(app PRIVATE ws2_32)
```

**面试题：为什么不推荐全局 `include_directories`？**

因为它会污染目录作用域，所有 target 都会继承该 include 路径，容易导致依赖边界不清、头文件误包含、不同模块之间耦合增强。现代 CMake 应该让每个 target 精确声明自己的依赖。

---

## 6. PRIVATE / PUBLIC / INTERFACE

这是 CMake 面试最高频考点之一。

| 关键字 | 当前 target 使用 | 依赖当前 target 的消费者继承 |
|--------|------------------|------------------------------|
| `PRIVATE` | 是 | 否 |
| `PUBLIC` | 是 | 是 |
| `INTERFACE` | 否 | 是 |

### 示例：库依赖传播

```text
app  →  network  →  core
```

```cmake
add_library(core src/core.cpp)
target_include_directories(core PUBLIC include)

target_compile_definitions(core PUBLIC CORE_ENABLE_LOG)

add_library(network src/network.cpp)
target_link_libraries(network
    PUBLIC core
)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE network)
```

效果：
- `network` 可以使用 `core` 的头文件和宏
- `app` 链接 `network` 后，也自动继承 `core` 的 include 路径和必要链接信息

### 如何选择？

```cmake
# 只在库内部使用，不暴露给使用者
target_link_libraries(my_lib PRIVATE zlib)

# 库的 public header 中包含了依赖库头文件，需要暴露
target_link_libraries(my_lib PUBLIC fmt::fmt)

# header-only 库，只给使用者提供 include/宏，本身不编译
target_include_directories(my_header_only INTERFACE include)
```

---

## 7. add_library 的几种类型

### 静态库与动态库

```cmake
add_library(math STATIC src/math.cpp)
add_library/plugin SHARED src/plugin.cpp)
```

也可以不写类型，由 `BUILD_SHARED_LIBS` 控制：

```cmake
add_library(core src/core.cpp)
# cmake -S . -B build -DBUILD_SHARED_LIBS=ON
```

### 接口库（Header-only）

```cmake
add_library(my_headers INTERFACE)

target_include_directories(my_headers
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(my_headers INTERFACE cxx_std_17)
```

### Object Library

```cmake
add_library(common_obj OBJECT
    src/a.cpp
    src/b.cpp
)

add_library(common_static STATIC $<TARGET_OBJECTS:common_obj>)
add_library(common_shared SHARED $<TARGET_OBJECTS:common_obj>)
```

适用于同一组源文件同时参与静态库和动态库构建。

---

## 8. 编译标准与编译选项

### 推荐写法：target_compile_features

```cmake
target_compile_features(app PRIVATE cxx_std_20)
```

优点：
- target 级别声明
- 可传播（PUBLIC/INTERFACE）
- 比全局 `CMAKE_CXX_STANDARD` 更精确

### 全局写法

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

适合小项目或统一标准的大项目顶层配置。

### 编译选项

```cmake
target_compile_options(app PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
```

---

## 9. Generator Expression

Generator Expression 在 generate/build 阶段求值，不是 configure 阶段普通变量。

### 常见表达式

```cmake
$<CONFIG:Debug>                    # 当前配置是否是 Debug
$<TARGET_FILE:app>                 # app 最终文件路径
$<TARGET_FILE_DIR:app>             # app 输出目录
$<CXX_COMPILER_ID:MSVC>            # 编译器判断
$<BUILD_INTERFACE:...>             # 构建树使用
$<INSTALL_INTERFACE:...>           # 安装树使用
```

### 示例

```cmake
target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:APP_DEBUG>
    $<$<CONFIG:Release>:APP_RELEASE>
)
```

---

## 10. Debug / Release 与生成器

### 单配置生成器

Ninja、Unix Makefiles 通常是单配置：

```bash
cmake -S . -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug

cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

### 多配置生成器

Visual Studio、Xcode 是多配置：

```bash
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
cmake --build build --config Release
```

**面试题：为什么 `CMAKE_BUILD_TYPE` 在 Visual Studio 下没效果？**

因为 Visual Studio 是多配置生成器，Debug/Release 是在 build 阶段通过 `--config` 选择的，而不是 configure 阶段用 `CMAKE_BUILD_TYPE` 固定。

---

## 11. add_subdirectory 与项目组织

### 多目录结构

```text
project/
├── CMakeLists.txt
├── src/
│   └── CMakeLists.txt
├── tests/
│   └── CMakeLists.txt
└── examples/
    └── CMakeLists.txt
```

顶层：

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)
```

`add_subdirectory` 会：
- 进入子目录处理其 `CMakeLists.txt`
- 创建新的目录作用域
- 将子目录 target 加入整体构建图

---

## 12. file(GLOB) 的争议

```cmake
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(app ${SOURCES})
```

### 为什么有争议？

| 问题 | 说明 |
|------|------|
| 新文件可能不触发重新 configure | 旧版本 CMake 尤其明显 |
| 构建输入不显式 | 阅读 CMakeLists 无法直接知道源码列表 |
| 误匹配临时文件 | 可能把不该编译的文件加入 |

### 推荐方式

```cmake
add_executable(app
    src/main.cpp
    src/app.cpp
    src/config.cpp
)
```

大型项目中显式列出源文件更清晰。

---

## 13. 常见面试问题

### Q1: CMake 和 Makefile 的区别？

CMake 是构建系统生成器，Makefile 是一种具体构建系统输入文件。CMake 可以生成 Makefile，也可以生成 Ninja、Visual Studio Solution、Xcode 工程。CMake 解决的是跨平台工程描述问题，Makefile 更接近具体平台的构建规则。

### Q2: 什么是现代 CMake？

现代 CMake 是 target-based 的 CMake：每个库/可执行文件作为 target，所有 include、宏、编译选项、链接依赖都绑定到 target 上，并通过 PRIVATE/PUBLIC/INTERFACE 精确表达依赖传播关系，避免全局变量和目录级命令污染。

### Q3: `PRIVATE`、`PUBLIC`、`INTERFACE` 怎么选？

- 只当前 target 内部使用：`PRIVATE`
- 当前 target 使用，且消费者也必须知道：`PUBLIC`
- 当前 target 自己不编译或不使用，只给消费者：`INTERFACE`

### Q4: `target_link_libraries` 只是链接库吗？

不是。现代 CMake 中它还会传播 usage requirements，包括 include 路径、编译宏、编译特性、链接选项等。链接一个 imported target 时，通常同时获得了正确的头文件路径和依赖库路径。

### Q5: `CMAKE_SOURCE_DIR` 和 `CMAKE_CURRENT_SOURCE_DIR` 区别？

| 变量 | 含义 |
|------|------|
| `CMAKE_SOURCE_DIR` | 顶层源码目录 |
| `CMAKE_CURRENT_SOURCE_DIR` | 当前正在处理的 CMakeLists 所在目录 |
| `PROJECT_SOURCE_DIR` | 当前 project 的源码目录 |
| `CMAKE_BINARY_DIR` | 顶层构建目录 |
| `CMAKE_CURRENT_BINARY_DIR` | 当前目录对应的构建目录 |

在可复用子项目中应尽量使用 `CMAKE_CURRENT_SOURCE_DIR` 或 `PROJECT_SOURCE_DIR`，避免误指向顶层项目。

---

## 14. 常见陷阱

```cmake
# ❌ 全局 include 污染所有 target
include_directories(${CMAKE_SOURCE_DIR}/third_party/foo/include)

# ✅ 只暴露给需要的 target
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/third_party/foo/include)
```

```cmake
# ❌ 错误：public header 里包含 fmt，但链接写成 PRIVATE
target_link_libraries(my_lib PRIVATE fmt::fmt)

# ✅ 如果 my_lib 的公开头文件包含 <fmt/core.h>，应使用 PUBLIC
target_link_libraries(my_lib PUBLIC fmt::fmt)
```

```bash
# ❌ Visual Studio 下这样指定 Release 通常没用
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# ✅ 多配置生成器在 build 阶段选择配置
cmake --build build --config Release
```

---

## 15. 总结

| 主题 | 核心记忆 |
|------|---------|
| **CMake 定位** | 构建系统生成器，不是编译器 |
| **构建方式** | 推荐 `cmake -S . -B build` 的 out-of-source build |
| **现代 CMake** | target-based，避免全局命令 |
| **依赖传播** | `PRIVATE/PUBLIC/INTERFACE` 表达 usage requirements |
| **库类型** | STATIC / SHARED / INTERFACE / OBJECT |
| **配置类型** | Ninja/Makefile 单配置，Visual Studio/Xcode 多配置 |
| **Generator Expression** | generate/build 阶段求值，常用于配置差异 |
