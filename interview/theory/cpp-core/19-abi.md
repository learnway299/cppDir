# 19 - ABI 与二进制兼容性 (ABI and Binary Compatibility)

## 简介

ABI（Application Binary Interface）是软件库的二进制接口规范。理解 ABI 对于开发稳定的库、避免兼容性问题至关重要。

**核心概念**：
- **ABI 定义**：二进制层面的接口约定
- **二进制兼容性**：库更新后不需要重新编译客户端代码
- **符号管理**：C++ 名称修饰与符号导出
- **版本控制**：如何管理库的不同版本

---

## 1. ABI 基础

### 1.1 什么是 ABI？

**ABI 包含的内容**：
- 函数调用约定（参数传递、返回值、栈帧布局）
- 数据类型大小与对齐
- 虚函数表布局
- 异常处理机制
- 名称修饰（Name Mangling）规则
- 对象内存布局

**ABI vs API**：

| 特性 | API（源码接口） | ABI（二进制接口） |
|------|----------------|------------------|
| 层次 | 源代码层 | 二进制层 |
| 稳定性 | 重新编译即可 | 需要二进制兼容 |
| 检查 | 编译期 | 运行期 |
| 影响范围 | 开发者 | 最终用户 |

### 1.2 为什么 ABI 重要？

```cpp
// libfoo.so v1.0
class Widget {
public:
    void func1();
    void func2();
private:
    int data1;
    int data2;
};

// libfoo.so v1.1（添加了新成员）
class Widget {
public:
    void func1();
    void func2();
    void func3();  // ❌ ABI 破坏：虚表布局改变
private:
    int data1;
    int data2;
    int data3;     // ❌ ABI 破坏：对象大小改变
};

// 旧客户端程序在运行时加载新库 → 崩溃
```

---

## 2. C++ ABI 破坏因素

### 2.1 类布局变化？

**破坏 ABI 的改动**：
```cpp
// v1.0
class Base {
    virtual void foo();
};

// v1.1 - ❌ 添加虚函数
class Base {
    virtual void foo();
    virtual void bar();  // ❌ 虚表指针位置可能变化
};

// v1.0
class Data {
    int x;
    int y;
};

// v1.1 - ❌ 改变成员顺序
class Data {
    int y;  // ❌ 偏移量改变
    int x;
};

// v1.1 - ❌ 改变继承关系
class Data : public NewBase {  // ❌ 布局完全改变
    int x;
    int y;
};
```

### 2.2 函数签名变化？

```cpp
// v1.0
void process(int x);

// v1.1 - ❌ 改变参数类型
void process(long x);  // ❌ 调用约定可能不同

// v1.1 - ❌ 添加默认参数（在源码层兼容，但 ABI 不同）
void process(int x, int y = 0);  // ❌ 名称修饰改变

// v1.1 - ❌ 改变返回值
int process(int x);  // ❌ 返回值处理不同
```

### 2.3 模板实例化？

```cpp
// v1.0
template<typename T>
class Vector {
    T* data;
    size_t size;
};

// v1.1 - ❌ 添加成员
template<typename T>
class Vector {
    T* data;
    size_t size;
    size_t capacity;  // ❌ 对象大小改变
};
```

---

## 3. 保持 ABI 兼容

### 3.1 Pimpl 惯用法？

```cpp
// widget.h（v1.0 和 v1.1 完全相同）
class Widget {
public:
    Widget();
    ~Widget();

    void func1();
    void func2();
    void func3();  // ✅ 可以安全添加

private:
    class Impl;  // 前向声明
    Impl* pimpl;  // ✅ 指针大小不变
};

// widget.cpp（v1.1 可以修改实现）
class Widget::Impl {
public:
    int data1;
    int data2;
    int data3;  // ✅ 可以安全添加成员

    void func1_impl();
    void func2_impl();
    void func3_impl();  // ✅ 可以添加新方法
};

Widget::Widget() : pimpl(new Impl()) {}
Widget::~Widget() { delete pimpl; }

void Widget::func1() { pimpl->func1_impl(); }
void Widget::func2() { pimpl->func2_impl(); }
void Widget::func3() { pimpl->func3_impl(); }
```

**优点**：
- 头文件不变，ABI 稳定
- 可以自由修改实现
- 隐藏实现细节

**缺点**：
- 额外的间接层
- 动态分配开销

### 3.2 在末尾添加成员？

```cpp
// v1.0
class Config {
public:
    void set_x(int value);
    void set_y(int value);

private:
    int x;
    int y;
};

// v1.1 - ✅ 在末尾添加成员（部分兼容）
class Config {
public:
    void set_x(int value);
    void set_y(int value);
    void set_z(int value);  // ✅ 新方法

private:
    int x;
    int y;
    int z;  // ⚠️ 对象大小改变，但旧代码访问 x, y 仍然正确
};
```

**注意**：
- 只有当对象由库内部创建时才安全
- 如果客户端栈上分配对象，则不兼容

### 3.3 使用非虚接口？

```cpp
// v1.0
class Interface {
public:
    void execute() {  // ✅ 非虚公共接口
        pre_execute();
        do_execute();   // 虚函数
        post_execute();
    }

private:
    virtual void do_execute() = 0;

    void pre_execute();   // ✅ 可以修改实现
    void post_execute();  // ✅ 可以修改实现
};

// v1.1 - ✅ 可以安全扩展
class Interface {
public:
    void execute() {
        pre_execute();
        validate();       // ✅ 新增辅助方法
        do_execute();
        post_execute();
        log();            // ✅ 新增辅助方法
    }

private:
    virtual void do_execute() = 0;

    void pre_execute();
    void post_execute();
    void validate();  // ✅ 新方法
    void log();       // ✅ 新方法
};
```

---

## 4. 符号版本管理

### 4.1 符号版本控制（Linux）？

```cpp
// 使用 .symver 指令管理符号版本
__asm__(".symver original_foo,foo@LIBFOO_1.0");
__asm__(".symver new_foo,foo@@LIBFOO_2.0");

// v1.0 实现
void original_foo(int x) {
    // 旧实现
}

// v2.0 实现（默认版本）
void new_foo(int x, int y) {
    // 新实现
}
```

**版本脚本（libfoo.map）**：
```
LIBFOO_1.0 {
    global:
        foo;
        bar;
    local:
        *;
};

LIBFOO_2.0 {
    global:
        foo;  # 新版本的 foo
        baz;  # 新函数
} LIBFOO_1.0;
```

**编译**：
```bash
g++ -shared -o libfoo.so foo.cpp -Wl,--version-script=libfoo.map
```

### 4.2 C++ 导出控制？

```cpp
// 显式导出符号
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

class EXPORT Widget {
public:
    void public_method();  // 导出

private:
    void private_method();  // 不导出（但可能被意外导出）
};

// 隐藏内部实现
namespace internal {
    class Helper {  // 不导出
        // ...
    };
}
```

---

## 5. ABI 检查工具

### 5.1 abidiff（libabigail）？

```bash
# 比较两个库的 ABI
abidiff libfoo-v1.so libfoo-v2.so

# 输出示例
Functions changes summary: 2 Removed, 3 Changed
Variables changes summary: 1 Removed, 2 Changed

[C] 'method void Widget::func(int)' at widget.h:10:1 has some sub-type changes:
  parameter 1 of type 'int' changed:
    type name changed from 'int' to 'long'
```

### 5.2 ABI Compliance Checker？

```bash
# 检查 ABI 兼容性
abi-compliance-checker -lib libfoo \
    -old libfoo-v1.xml \
    -new libfoo-v2.xml

# XML 描述文件
<version>1.0</version>
<headers>
    <path>include/</path>
</headers>
<libs>
    <path>lib/libfoo.so.1.0</path>
</libs>
```

### 5.3 abi-dumper？

```bash
# 导出 ABI 信息
abi-dumper libfoo.so -o ABI-1.dump -lver 1.0

# 比较两个版本
abi-compliance-checker -l libfoo \
    -old ABI-1.dump \
    -new ABI-2.dump
```

---

## 6. 平台差异

### 6.1 Windows vs Linux？

| 特性 | Windows（MSVC） | Linux（GCC/Clang） |
|------|----------------|-------------------|
| 调用约定 | stdcall, cdecl, fastcall | System V AMD64 ABI |
| 名称修饰 | 独特规则 | Itanium C++ ABI |
| 异常处理 | SEH | Dwarf/SJLJ |
| 导出控制 | dllexport/dllimport | visibility |

### 6.2 名称修饰示例？

```cpp
void foo(int x);

// GCC/Clang: _Z3fooi
// MSVC: ?foo@@YAXH@Z

void bar(int x, double y);

// GCC/Clang: _Z3barif
// MSVC: ?bar@@YAXHN@Z

class Widget {
    void method(int x);
};

// GCC/Clang: _ZN6Widget6methodEi
// MSVC: ?method@Widget@@QEAAXH@Z
```

**查看符号**：
```bash
# Linux
nm -C libfoo.so

# Windows
dumpbin /exports foo.dll
```

---

## 7. 最佳实践

### 7.1 设计稳定的 ABI？

**1. 使用 Pimpl 隐藏实现**：
```cpp
class EXPORT StableWidget {
public:
    StableWidget();
    ~StableWidget();

    void do_work();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};
```

**2. 使用纯虚接口**：
```cpp
class EXPORT IService {
public:
    virtual ~IService() = default;
    virtual void execute() = 0;
};

// 工厂函数
EXPORT IService* create_service();
EXPORT void destroy_service(IService* service);
```

**3. 避免内联函数**：
```cpp
// ❌ 内联：客户端代码包含实现
inline int get_value() { return 42; }

// ✅ 非内联：实现在库中
EXPORT int get_value();
```

**4. 使用不透明句柄**：
```cpp
// 公共头文件
typedef struct WidgetImpl* WidgetHandle;

EXPORT WidgetHandle widget_create();
EXPORT void widget_destroy(WidgetHandle handle);
EXPORT void widget_process(WidgetHandle handle);
```

### 7.2 版本控制策略？

**语义化版本（Semantic Versioning）**：
- **主版本**（Major）：ABI 不兼容变更
- **次版本**（Minor）：ABI 兼容的新功能
- **修订版本**（Patch）：ABI 兼容的 bug 修复

```
libfoo.so.2.3.1
         │ │ └─ 修订版本（Patch）
         │ └─── 次版本（Minor）
         └───── 主版本（Major）
```

---

## 总结

### ABI 破坏因素速查

| 改动 | 是否破坏 ABI | 说明 |
|------|-------------|------|
| 添加非虚成员函数 | ✅ 通常不破坏 | 不影响布局 |
| 添加虚函数 | ❌ 破坏 | 虚表布局改变 |
| 添加数据成员 | ❌ 破坏 | 对象大小改变 |
| 改变成员顺序 | ❌ 破坏 | 偏移量改变 |
| 改变继承关系 | ❌ 破坏 | 布局完全改变 |
| 改变函数签名 | ❌ 破坏 | 调用约定改变 |
| Pimpl 添加成员 | ✅ 不破坏 | 实现隐藏 |

### 关键记忆点

1. **ABI 包含**：调用约定、内存布局、符号名称
2. **Pimpl 是稳定 ABI 的最佳实践**
3. **添加虚函数、数据成员会破坏 ABI**
4. **使用符号版本控制管理不兼容变更**
5. **工具**：abidiff、ABI Compliance Checker
6. **版本号**：主版本表示 ABI 不兼容

---

## 参考资源

- [Itanium C++ ABI](https://itanium-cxx-abi.github.io/cxx-abi/)
- [GCC Visibility](https://gcc.gnu.org/wiki/Visibility)
- [KDE Binary Compatibility Guide](https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B)
