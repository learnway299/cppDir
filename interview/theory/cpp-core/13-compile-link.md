# 13 - 编译与链接 (Compile & Link)

## 简介

理解 C++ 的编译和链接过程对于解决编译错误、优化构建时间、理解程序行为至关重要。本章深入探讨预处理、编译、汇编、链接的各个阶段。

**核心特点**：
- **预处理**：宏展开、文件包含、条件编译
- **编译**：源码 → 汇编 → 目标文件
- **链接**：合并目标文件，解析符号
- **库**：静态库和动态库

---

## 1. 编译过程概览

### 1.1 编译的四个阶段？

**问题描述**：从源文件到可执行文件的完整过程。

**四个阶段**：

```
源文件 (.cpp, .h)
    ↓ 预处理 (Preprocessing)
预处理后的文件 (.i)
    ↓ 编译 (Compilation)
汇编代码 (.s)
    ↓ 汇编 (Assembly)
目标文件 (.o, .obj)
    ↓ 链接 (Linking)
可执行文件 (.exe, .out)
```

**查看各阶段输出**：

```bash
# 1. 预处理
g++ -E main.cpp -o main.i

# 2. 编译到汇编
g++ -S main.cpp -o main.s

# 3. 汇编到目标文件
g++ -c main.cpp -o main.o

# 4. 链接
g++ main.o -o main
```

**示例代码**：

```cpp
// main.cpp
#include <iostream>
#define PI 3.14

int main() {
    std::cout << "PI = " << PI << "\n";
    return 0;
}
```

**预处理后（main.i）**：

```cpp
// ... 大量 iostream 头文件内容

int main() {
    std::cout << "PI = " << 3.14 << "\n";  // PI 被替换
    return 0;
}
```

**关键要点**：
- **预处理**：处理 `#` 开头的指令
- **编译**：语法分析、语义分析、生成汇编
- **汇编**：汇编码 → 机器码
- **链接**：合并目标文件，解析符号引用

---

## 2. 预处理器 (Preprocessor)

### 2.1 `#include` 的工作原理？

**问题描述**：头文件如何被包含？

**`#include` 的两种形式**：

```cpp
// 形式 1：系统头文件（标准库路径）
#include <iostream>
#include <vector>

// 形式 2：用户头文件（当前目录优先）
#include "my_header.h"
#include "utils/helper.h"
```

**工作原理**：

```cpp
// main.cpp
#include "header.h"

int main() {
    func();
}
```

```cpp
// header.h
void func() {
    // ...
}
```

**预处理后**：

```cpp
// main.cpp（展开后）
void func() {
    // ...
}

int main() {
    func();
}
```

**头文件保护（Include Guard）**：

```cpp
// ❌ 问题：重复包含导致重定义错误
// a.h
struct Point { int x, y; };

// b.h
#include "a.h"

// main.cpp
#include "a.h"
#include "b.h"
// 结果：Point 被定义两次 → 编译错误

// ✅ 解决方案 1：传统头文件保护
// a.h
#ifndef A_H
#define A_H

struct Point { int x, y; };

#endif  // A_H

// ✅ 解决方案 2：#pragma once（现代推荐）
// a.h
#pragma once

struct Point { int x, y; };
```

**`#pragma once` vs 传统保护**：

| 特性 | `#pragma once` | 传统保护 |
|------|---------------|---------|
| 简洁性 | ✅ 一行 | ❌ 三行 |
| 标准化 | ❌ 非标准（但广泛支持） | ✅ 标准 |
| 性能 | ✅ 更快 | ❌ 稍慢 |
| 符号冲突 | ✅ 无 | ❌ 可能（宏名冲突） |

**关键要点**：
- `<>` 搜索系统路径，`""` 先搜索当前目录
- `#include` 就是简单的文本替换
- **必须使用头文件保护**，推荐 `#pragma once`

---

### 2.2 宏 (Macros) 的使用？

**问题描述**：预处理器宏的定义和陷阱。

**基本宏**：

```cpp
// 对象宏
#define PI 3.14159
#define MAX_SIZE 1024

// 函数宏
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void macro_demo() {
    double area = PI * 5 * 5;
    int sq = SQUARE(3);       // ((3) * (3))
    int max = MAX(10, 20);    // ((10) > (20) ? (10) : (20))
}
```

**宏的陷阱**：

```cpp
// ❌ 陷阱 1：缺少括号
#define BAD_SQUARE(x) x * x

int result = BAD_SQUARE(2 + 3);  // 展开：2 + 3 * 2 + 3 = 11（期望 25）

// ✅ 正确：加括号
#define GOOD_SQUARE(x) ((x) * (x))
int result2 = GOOD_SQUARE(2 + 3);  // ((2 + 3) * (2 + 3)) = 25

// ❌ 陷阱 2：副作用
#define BAD_MAX(a, b) ((a) > (b) ? (a) : (b))

int x = 5;
int max = BAD_MAX(++x, 10);  // ++x 可能被执行两次！

// ✅ 正确：使用内联函数
inline int good_max(int a, int b) {
    return a > b ? a : b;
}

// ❌ 陷阱 3：类型不安全
#define ADD(a, b) ((a) + (b))

ADD(3.14, "hello");  // ❌ 编译错误，但宏阶段不检查

// ✅ 正确：使用模板
template<typename T>
T add(T a, T b) {
    return a + b;
}
```

**有用的预定义宏**：

```cpp
void predefined_macros() {
    std::cout << "File: " << __FILE__ << "\n";      // 当前文件名
    std::cout << "Line: " << __LINE__ << "\n";      // 当前行号
    std::cout << "Date: " << __DATE__ << "\n";      // 编译日期
    std::cout << "Time: " << __TIME__ << "\n";      // 编译时间
    std::cout << "Function: " << __func__ << "\n";  // 当前函数名（C++11）

#ifdef _WIN32
    std::cout << "Windows\n";
#elif __linux__
    std::cout << "Linux\n";
#elif __APPLE__
    std::cout << "macOS\n";
#endif
}
```

**字符串化和连接**：

```cpp
// # 字符串化
#define STRINGIFY(x) #x

std::cout << STRINGIFY(hello) << "\n";  // "hello"

// ## 连接
#define CONCAT(a, b) a##b

int xy = 10;
int value = CONCAT(x, y);  // xy

// 实用宏
#define LOG(msg) std::cout << __FILE__ << ":" << __LINE__ << ": " << msg << "\n"

LOG("Debug message");  // main.cpp:42: Debug message
```

**关键要点**：
- 宏是文本替换，不进行类型检查
- **优先使用 `const`、`inline`、模板** 替代宏
- 宏的合理用途：条件编译、调试、平台检测

---

### 2.3 条件编译？

**问题描述**：根据条件包含/排除代码。

```cpp
// 调试版本 vs 发布版本
#ifdef DEBUG
    #define LOG(msg) std::cout << msg << "\n"
#else
    #define LOG(msg)  // 空操作
#endif

void conditional_demo() {
    LOG("This is a debug message");  // 仅在 DEBUG 模式输出
}

// 编译命令
// g++ -DDEBUG main.cpp  // 定义 DEBUG
```

**常见条件编译模式**：

```cpp
// 头文件保护（传统方式）
#ifndef MY_HEADER_H
#define MY_HEADER_H
// ...
#endif

// 平台检测
#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
#endif

// 编译器检测
#ifdef __GNUC__
    // GCC 特定代码
#elif _MSC_VER
    // MSVC 特定代码
#endif

// C++ 标准版本检测
#if __cplusplus >= 201703L
    // C++17 或更高
    #include <optional>
#else
    // 使用替代方案
#endif

// 功能检测
#ifdef __has_include
    #if __has_include(<filesystem>)
        #include <filesystem>
        namespace fs = std::filesystem;
    #elif __has_include(<experimental/filesystem>)
        #include <experimental/filesystem>
        namespace fs = std::experimental::filesystem;
    #endif
#endif
```

**`#if` 表达式**：

```cpp
#define VERSION 2

#if VERSION == 1
    // Version 1 code
#elif VERSION == 2
    // Version 2 code
#else
    #error "Unknown version"
#endif

// 复杂条件
#if defined(_WIN32) && !defined(_WIN64)
    // 32-bit Windows
#endif
```

---

## 3. 编译单元与链接

### 3.1 什么是编译单元（Translation Unit）？

**问题描述**：C++ 如何组织源文件？

**编译单元的定义**：
- 一个 `.cpp` 文件及其包含的所有头文件
- 编译器**独立编译**每个编译单元
- 生成一个目标文件（`.o` 或 `.obj`）

**示例**：

```cpp
// math.h
int add(int a, int b);

// math.cpp（编译单元 1）
#include "math.h"

int add(int a, int b) {
    return a + b;
}

// main.cpp（编译单元 2）
#include "math.h"
#include <iostream>

int main() {
    std::cout << add(3, 4) << "\n";
    return 0;
}
```

**编译过程**：

```bash
# 分别编译两个编译单元
g++ -c math.cpp -o math.o
g++ -c main.cpp -o main.o

# 链接
g++ math.o main.o -o program
```

**关键要点**：
- 每个 `.cpp` 是一个独立的编译单元
- 编译单元之间通过**声明**（头文件）和**链接**（符号表）通信
- 修改一个 `.cpp` 只需重新编译该文件

---

### 3.2 声明 vs 定义？

**问题描述**：declaration 和 definition 的区别。

**声明 (Declaration)**：
- 告诉编译器**名称的存在和类型**
- 不分配内存
- 可以多次声明

**定义 (Definition)**：
- 告诉编译器**实际的实现**
- 分配内存
- 只能定义一次（ODR）

**示例**：

```cpp
// 声明
extern int x;               // 变量声明
int add(int a, int b);      // 函数声明
class MyClass;              // 类声明（前向声明）

// 定义
int x = 10;                 // 变量定义
int add(int a, int b) {     // 函数定义
    return a + b;
}
class MyClass {             // 类定义
    int data;
};
```

**函数**：

```cpp
// 声明（可多次）
int func(int x);
int func(int x);  // ✅ 可以

// 定义（只能一次）
int func(int x) {
    return x * 2;
}
// int func(int x) { return x * 3; }  // ❌ 重定义错误
```

**变量**：

```cpp
// file1.cpp
int global_var = 100;  // 定义

// file2.cpp
extern int global_var;  // 声明
std::cout << global_var << "\n";  // 使用

// file3.cpp
extern int global_var;  // 声明（可多次）
```

**类**：

```cpp
// 声明（前向声明）
class MyClass;

// 此时只能使用指针或引用
MyClass* ptr;      // ✅
MyClass& ref;      // ✅
// MyClass obj;    // ❌ 错误：类型不完整

// 定义
class MyClass {
    int data;
};

// 现在可以创建对象
MyClass obj;  // ✅
```

**关键要点**：
- **声明**：告诉编译器存在
- **定义**：告诉编译器实现
- **头文件放声明，源文件放定义**

---

### 3.3 One Definition Rule (ODR)？

**问题描述**：ODR 的含义和违反后果。

**ODR 规则**：
1. **变量/函数**：在整个程序中只能定义一次
2. **类/模板**：在每个编译单元中可以定义一次，但定义必须**完全相同**
3. **内联函数/模板**：可以在多个编译单元中定义，但定义必须**完全相同**

**违反 ODR 的情况**：

```cpp
// ❌ 错误 1：变量重复定义
// file1.cpp
int global = 10;

// file2.cpp
int global = 20;  // ❌ ODR 违规，链接错误

// ✅ 正确：使用 extern
// header.h
extern int global;  // 声明

// file1.cpp
int global = 10;  // 定义

// file2.cpp
extern int global;  // 声明
```

```cpp
// ❌ 错误 2：函数重复定义
// file1.cpp
int func() { return 1; }

// file2.cpp
int func() { return 2; }  // ❌ ODR 违规，链接错误

// ✅ 正确：内联函数可以重复定义
// header.h
inline int func() { return 1; }

// file1.cpp 和 file2.cpp 都包含 header.h
// ✅ 合法，定义相同
```

**类定义必须相同**：

```cpp
// ❌ 错误：类定义不一致
// file1.cpp
class MyClass {
    int x;
};

// file2.cpp
class MyClass {
    int x, y;  // ❌ 定义不同，未定义行为
};
```

**inline 和 ODR**：

```cpp
// header.h
inline int get_value() {
    return 42;
}

// 多个 .cpp 文件包含 header.h
// ✅ 合法：inline 函数可以有多个定义
```

**关键要点**：
- **非内联函数/变量**：全局只能定义一次
- **内联函数/模板**：可以多次定义，但定义必须相同
- **违反 ODR**：链接错误或未定义行为

---

## 4. 符号 (Symbols)

### 4.1 什么是符号和符号表？

**问题描述**：链接器如何找到函数和变量？

**符号的定义**：
- **符号**：函数名、变量名、类名等标识符
- **符号表**：记录符号的名称、地址、类型

**查看符号表**：

```bash
# Linux/macOS
nm program.o
objdump -t program.o

# Windows
dumpbin /symbols program.obj
```

**符号类型**：

```bash
# nm 输出示例
0000000000000000 T main          # T = Text（代码段）
0000000000000010 D global_var    # D = Data（数据段）
                 U printf        # U = Undefined（需要链接）
0000000000000020 t local_func    # t = local（静态函数）
```

**符号可见性**：

```cpp
// 全局符号（默认）
int global_func() { return 1; }

// 局部符号（仅当前编译单元可见）
static int local_func() { return 2; }

// 匿名命名空间（C++ 推荐，替代 static）
namespace {
    int local_func2() { return 3; }
}
```

**关键要点**：
- 符号是链接的基础
- 链接器通过符号表匹配引用和定义

---

### 4.2 名称修饰 (Name Mangling)？

**问题描述**：C++ 如何支持函数重载和命名空间？

**名称修饰的原因**：
- C++ 支持函数重载、命名空间、类等
- 需要将这些信息编码到符号名中

**示例**：

```cpp
// C++ 代码
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }

namespace math {
    int multiply(int a, int b) { return a * b; }
}

class Calculator {
public:
    int subtract(int a, int b) { return a - b; }
};
```

**编译后的符号名（GCC）**：

```bash
_Z3addii        # int add(int, int)
_Z3adddd        # double add(double, double)
_ZN4math8multiplyEii  # math::multiply(int, int)
_ZN10Calculator8subtractEii  # Calculator::subtract(int, int)
```

**查看修饰后的名称**：

```bash
# 编译
g++ -c math.cpp -o math.o

# 查看符号
nm math.o

# 反修饰（demangling）
nm math.o | c++filt
# 或
c++filt _Z3addii  # 输出：add(int, int)
```

**`extern "C"` 阻止名称修饰**：

```cpp
// C++ 代码
extern "C" {
    int c_style_func(int x);  // 不修饰，符号名就是 c_style_func
}

// 用于 C++ 调用 C 库或被 C 代码调用
extern "C" void exported_func() {
    // ...
}
```

**关键要点**：
- **名称修饰**实现函数重载和命名空间
- **不同编译器**的修饰规则不同（不兼容）
- **`extern "C"`** 阻止修饰，用于 C/C++ 互操作

---

### 4.3 链接属性：`extern` / `static` / `inline`？

**问题描述**：如何控制符号的可见性和链接行为？

**`extern`**：

```cpp
// 声明外部符号
extern int global_var;  // 声明，定义在其他文件
extern void func();     // 声明，定义在其他文件

// 定义全局符号（默认就是 extern）
int global_var = 10;  // 等价于 extern int global_var = 10;
```

**`static`**：

```cpp
// 文件级别：限制在当前编译单元
static int file_local_var = 10;  // 仅当前 .cpp 可见
static void file_local_func() {  // 仅当前 .cpp 可见
    // ...
}

// 函数级别：静态局部变量
void func() {
    static int counter = 0;  // 只初始化一次，保留值
    ++counter;
}

// 类成员：静态成员
class MyClass {
    static int shared_var;  // 所有对象共享
};
int MyClass::shared_var = 0;  // 定义
```

**`inline`**：

```cpp
// 建议编译器内联展开（减少函数调用开销）
inline int add(int a, int b) {
    return a + b;
}

// C++17：inline 变量
inline int global_config = 100;  // 可以在头文件中定义
```

**匿名命名空间（C++ 推荐）**：

```cpp
// 替代文件级别的 static
namespace {
    int file_local_var = 10;
    void file_local_func() { }
}
// 等价于 static，但更符合 C++ 风格
```

**可见性总结**：

| 关键字 | 作用域 | 链接性 |
|--------|--------|--------|
| 无 | 全局 | External（全局可见） |
| `extern` | 全局 | External（声明） |
| `static` | 文件/函数 | Internal（仅当前文件） |
| `inline` | 全局 | External（可多次定义） |
| 匿名命名空间 | 文件 | Internal（仅当前文件） |

---

## 5. 静态库和动态库

### 5.1 静态库 (Static Library) 的使用？

**问题描述**：如何创建和使用静态库？

**静态库的特点**：
- 编译时链接
- 代码拷贝到可执行文件
- 文件后缀：`.a`（Linux/macOS）、`.lib`（Windows）

**创建静态库**：

```bash
# 1. 编译源文件为目标文件
g++ -c math.cpp -o math.o
g++ -c utils.cpp -o utils.o

# 2. 创建静态库
ar rcs libmath.a math.o utils.o

# 3. 使用静态库
g++ main.cpp -L. -lmath -o program
# -L.：库搜索路径（当前目录）
# -lmath：链接 libmath.a（省略 lib 前缀和 .a 后缀）
```

**示例**：

```cpp
// math.h
int add(int a, int b);

// math.cpp
int add(int a, int b) {
    return a + b;
}

// main.cpp
#include "math.h"
#include <iostream>

int main() {
    std::cout << add(3, 4) << "\n";
    return 0;
}
```

**优缺点**：

| 优点 | 缺点 |
|------|------|
| 无运行时依赖 | 可执行文件大 |
| 性能稍好（无间接调用） | 更新库需重新编译 |
| 简单 | 多个程序浪费内存 |

---

### 5.2 动态库 (Dynamic Library) 的使用？

**问题描述**：如何创建和使用动态库？

**动态库的特点**：
- 运行时加载
- 多个程序共享
- 文件后缀：`.so`（Linux）、`.dylib`（macOS）、`.dll`（Windows）

**创建动态库**：

```bash
# Linux/macOS
g++ -fPIC -c math.cpp -o math.o
g++ -shared -o libmath.so math.o

# 使用动态库
g++ main.cpp -L. -lmath -o program

# 运行时需要设置库路径
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH  # Linux
export DYLD_LIBRARY_PATH=.:$DYLD_LIBRARY_PATH  # macOS
./program
```

```bash
# Windows
cl /LD math.cpp /Fe:math.dll

# 使用
cl main.cpp math.lib
# 运行时需要 math.dll 在同目录或系统路径
```

**显式加载（dlopen）**：

```cpp
#include <dlfcn.h>  // Linux/macOS

void dynamic_loading() {
    // 加载库
    void* handle = dlopen("./libmath.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot load library: " << dlerror() << "\n";
        return;
    }

    // 获取符号
    typedef int (*add_func_t)(int, int);
    add_func_t add = (add_func_t)dlsym(handle, "add");
    if (!add) {
        std::cerr << "Cannot load symbol: " << dlerror() << "\n";
        dlclose(handle);
        return;
    }

    // 调用函数
    std::cout << add(3, 4) << "\n";

    // 卸载库
    dlclose(handle);
}
```

**优缺点**：

| 优点 | 缺点 |
|------|------|
| 节省内存（共享） | 需要运行时依赖 |
| 可以独立更新 | 性能稍差（间接调用） |
| 插件机制 | 版本兼容性问题 |

---

### 5.3 静态库 vs 动态库？

**选择指南**：

```
使用静态库？
├─ 是：简单、无依赖、性能优先
│   └─ 示例：嵌入式系统、工具程序
└─ 否：使用动态库
    ├─ 需要更新库而不重新编译
    ├─ 多个程序共享（节省内存）
    └─ 插件系统
```

**混合使用**：

```bash
# 部分静态、部分动态
g++ main.cpp -L. -lmath -static-libstdc++ -o program
# -static-libstdc++：静态链接标准库
```

---

## 6. 链接错误

### 6.1 常见链接错误？

**问题描述**：如何诊断和解决链接错误？

**错误 1：Undefined Reference（未定义的引用）**

```cpp
// math.h
int add(int a, int b);

// main.cpp
#include "math.h"
int main() {
    add(3, 4);  // ❌ undefined reference to `add(int, int)'
    return 0;
}

// 原因：声明了但没有定义（没有编译 math.cpp）
// 解决：g++ main.cpp math.cpp -o program
```

**错误 2：Multiple Definition（重复定义）**

```cpp
// header.h
int global_var = 10;  // ❌ 定义在头文件

// file1.cpp
#include "header.h"

// file2.cpp
#include "header.h"

// 链接时：multiple definition of `global_var'

// ✅ 解决方案 1：使用 extern
// header.h
extern int global_var;  // 声明

// one_file.cpp
int global_var = 10;  // 定义

// ✅ 解决方案 2：使用 inline（C++17）
// header.h
inline int global_var = 10;
```

**错误 3：符号修饰不匹配**

```cpp
// C 库函数
void c_func();

// C++ 调用
c_func();  // ❌ undefined reference to `c_func()'
// 编译器找的是 _Z6c_funcv（修饰后的名称）

// ✅ 解决：使用 extern "C"
extern "C" {
    void c_func();
}
```

**错误 4：库顺序错误**

```bash
# ❌ 错误顺序
g++ -lmath main.cpp -o program
# 库在 main.cpp 之前，找不到 main.cpp 中引用的符号

# ✅ 正确顺序
g++ main.cpp -lmath -o program
# 库在源文件之后
```

---

### 6.2 如何调试链接错误？

**工具和技巧**：

```bash
# 1. 查看目标文件的符号
nm math.o | grep add
objdump -t math.o

# 2. 查看未定义的符号
nm -u program.o

# 3. 查看库中的符号
nm libmath.a

# 4. 详细链接信息
g++ main.cpp math.cpp -o program -Wl,--verbose

# 5. 检查库依赖
ldd program  # Linux
otool -L program  # macOS
```

**常见原因**：
1. 忘记编译/链接某个源文件
2. 头文件和实现不匹配
3. 库的搜索路径不正确
4. 链接顺序错误
5. 名称修饰不匹配（C/C++ 混用）

---

## 7. 编译优化

### 7.1 常用编译选项？

**问题描述**：如何控制编译行为？

**优化级别**：

```bash
g++ -O0 main.cpp  # 无优化（默认，调试用）
g++ -O1 main.cpp  # 基本优化
g++ -O2 main.cpp  # 中等优化（推荐）
g++ -O3 main.cpp  # 激进优化（可能增大体积）
g++ -Os main.cpp  # 优化体积
g++ -Og main.cpp  # 调试优化（保留调试信息）
```

**警告选项**：

```bash
g++ -Wall main.cpp        # 常见警告
g++ -Wextra main.cpp      # 额外警告
g++ -Werror main.cpp      # 警告视为错误
g++ -pedantic main.cpp    # 严格标准一致性
```

**调试选项**：

```bash
g++ -g main.cpp           # 生成调试信息
g++ -g3 main.cpp          # 详细调试信息（包含宏）
g++ -ggdb main.cpp        # GDB 专用调试信息
```

**标准版本**：

```bash
g++ -std=c++11 main.cpp
g++ -std=c++14 main.cpp
g++ -std=c++17 main.cpp
g++ -std=c++20 main.cpp
g++ -std=c++23 main.cpp
```

**其他有用选项**：

```bash
g++ -I/path/to/includes   # 头文件搜索路径
g++ -L/path/to/libs       # 库搜索路径
g++ -lmath                # 链接库
g++ -D DEBUG              # 定义宏
g++ -fPIC                 # 位置无关代码（动态库）
g++ -static               # 静态链接
g++ -pthread              # 多线程支持
```

---

### 7.2 编译优化对代码的影响？

**问题描述**：优化可能改变程序行为。

**示例 1：未定义行为可能被优化掉**

```cpp
int* ptr = nullptr;
if (ptr != nullptr) {
    *ptr = 10;  // ❌ 未定义行为
}

// -O0：if 检查存在
// -O2：编译器可能假设 ptr 非空（未定义行为优化）
```

**示例 2：volatile 阻止优化**

```cpp
int x = 0;
for (int i = 0; i < 1000000; ++i) {
    x = i;  // -O2 优化后：直接 x = 999999
}

volatile int y = 0;
for (int i = 0; i < 1000000; ++i) {
    y = i;  // volatile 阻止优化，每次都写入
}
```

**示例 3：内联展开**

```cpp
inline int add(int a, int b) {
    return a + b;
}

int result = add(3, 4);

// -O0：函数调用
// -O2：内联展开 → result = 3 + 4; → result = 7;
```

**关键要点**：
- 优化改变代码结构，但不应改变正确代码的行为
- **未定义行为**在优化后可能暴露
- 调试时使用 `-O0 -g`，发布时使用 `-O2` 或 `-O3`

---

## 8. 构建系统

### 8.1 Makefile 基础？

**问题描述**：自动化编译过程。

**基本 Makefile**：

```makefile
# 变量
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = program

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): main.o math.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o math.o

# 编译
main.o: main.cpp math.h
	$(CXX) $(CXXFLAGS) -c main.cpp

math.o: math.cpp math.h
	$(CXX) $(CXXFLAGS) -c math.cpp

# 清理
clean:
	rm -f *.o $(TARGET)
```

**自动推导**：

```makefile
# 简化版本
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

SRCS = main.cpp math.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
```

---

### 8.2 CMake 基础？

**问题描述**：跨平台构建系统。

**基本 CMakeLists.txt**：

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 可执行文件
add_executable(program main.cpp math.cpp)

# 包含目录
target_include_directories(program PRIVATE include)

# 链接库
target_link_libraries(program pthread)
```

**构建**：

```bash
mkdir build
cd build
cmake ..
make
```

---

## 总结

### 关键概念速查

| 概念 | 说明 |
|------|------|
| **预处理** | 宏展开、文件包含、条件编译 |
| **编译单元** | 一个 `.cpp` 及其包含的头文件 |
| **ODR** | One Definition Rule，定义只能一次 |
| **符号** | 函数名、变量名等标识符 |
| **名称修饰** | 支持重载和命名空间 |
| **静态库** | 编译时链接，`.a`/`.lib` |
| **动态库** | 运行时加载，`.so`/`.dll` |
| **链接错误** | 未定义引用、重复定义 |

### 关键记忆点

1. **头文件放声明，源文件放定义**
2. **使用 `#pragma once` 或头文件保护**
3. **优先使用 `const`、`inline`、模板** 替代宏
4. **`extern "C"`** 用于 C/C++ 互操作
5. **ODR**：非内联函数/变量全局只能定义一次
6. **静态库**简单但体积大，**动态库**节省内存但有依赖
7. **链接错误**：检查源文件、库路径、链接顺序
8. **优化会改变代码结构**，调试用 `-O0 -g`

### 常见陷阱

1. 在头文件中定义变量/函数（违反 ODR）
2. 忘记头文件保护（重复定义）
3. 宏缺少括号（展开错误）
4. C/C++ 混用忘记 `extern "C"`
5. 链接库顺序错误
6. 动态库路径未设置

### 最佳实践

1. **头文件**：只放声明、`inline` 函数、模板
2. **源文件**：放定义
3. **使用构建系统**：CMake、Makefile
4. **优化级别**：调试 `-O0 -g`，发布 `-O2` 或 `-O3`
5. **开启警告**：`-Wall -Wextra -Werror`
6. **现代 C++**：使用 `inline` 变量（C++17）、模块（C++20）
