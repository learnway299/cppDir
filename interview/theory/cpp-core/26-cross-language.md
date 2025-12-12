# 26 - 跨语言互操作 (Cross-Language Interop)

## 简介

C++ 经常需要与其他语言（C、Python、Java、C# 等）进行互操作。理解跨语言调用的机制和最佳实践对于集成多语言系统至关重要。

**核心概念**：
- **C 接口**：最通用的跨语言边界
- **外部函数接口（FFI）**：调用外部语言
- **嵌入与扩展**：Python、Lua 等脚本语言
- **JNI**：Java Native Interface
- **COM**：Windows 组件对象模型

---

## 1. C++ 与 C 互操作

### 1.1 extern "C"？

```cpp
// C++ 代码导出给 C 使用
extern "C" {
    int add(int a, int b) {
        return a + b;
    }

    void process_data(const char* data) {
        // 处理数据
    }
}

// 或使用宏
#ifdef __cplusplus
extern "C" {
#endif

int multiply(int a, int b);

#ifdef __cplusplus
}
#endif
```

**原因**：
- C++ 有名称修饰（name mangling）
- C 没有名称修饰
- `extern "C"` 禁用名称修饰，使 C 能调用

**名称修饰示例**：
```cpp
// C++ 函数
void foo(int x);
// 编译后符号: _Z3fooi (GCC)

// extern "C" 函数
extern "C" void foo(int x);
// 编译后符号: foo
```

### 1.2 C++ 调用 C 代码？

```cpp
// C 头文件（mylib.h）
#ifdef __cplusplus
extern "C" {
#endif

void c_function(int x);

#ifdef __cplusplus
}
#endif

// C++ 代码调用
#include "mylib.h"

void cpp_code() {
    c_function(42);  // 直接调用
}
```

### 1.3 数据类型对应？

| C 类型 | C++ 类型 | 说明 |
|--------|----------|------|
| `int` | `int` | ✅ 直接对应 |
| `char*` | `char*` | ✅ C 字符串 |
| `void*` | `void*` | ✅ 通用指针 |
| `struct` | `struct` | ✅ POD 类型 |
| - | `std::string` | ❌ C++ 专有，需转换 |
| - | `std::vector` | ❌ C++ 专有，需转换 |

**传递 C++ 对象到 C**：
```cpp
// ❌ 不能直接传递
void c_function(std::string str);  // C 无法理解

// ✅ 转换为 C 兼容类型
extern "C" void c_function(const char* str) {
    std::string cpp_str(str);  // 在 C++ 侧转换
    // ...
}
```

---

## 2. C++ 与 Python 互操作

### 2.1 Python 调用 C++（pybind11）？

```cpp
#include <pybind11/pybind11.h>

namespace py = pybind11;

// C++ 函数
int add(int a, int b) {
    return a + b;
}

// C++ 类
class Calculator {
public:
    int multiply(int a, int b) {
        return a * b;
    }
};

// 绑定到 Python
PYBIND11_MODULE(mymodule, m) {
    m.doc() = "示例模块";

    // 绑定函数
    m.def("add", &add, "加法函数");

    // 绑定类
    py::class_<Calculator>(m, "Calculator")
        .def(py::init<>())
        .def("multiply", &Calculator::multiply);
}
```

**Python 使用**：
```python
import mymodule

result = mymodule.add(1, 2)  # 3

calc = mymodule.Calculator()
result = calc.multiply(3, 4)  # 12
```

### 2.2 C++ 嵌入 Python？

```cpp
#include <Python.h>

void embed_python() {
    Py_Initialize();  // 初始化 Python 解释器

    // 执行 Python 代码
    PyRun_SimpleString("print('Hello from Python')");

    // 调用 Python 函数
    PyObject* pModule = PyImport_ImportModule("math");
    PyObject* pFunc = PyObject_GetAttrString(pModule, "sqrt");
    PyObject* pArgs = PyTuple_Pack(1, PyFloat_FromDouble(16.0));
    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);

    double result = PyFloat_AsDouble(pResult);
    std::cout << "sqrt(16) = " << result << "\n";  // 4.0

    // 清理
    Py_DECREF(pResult);
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    Py_Finalize();  // 关闭 Python 解释器
}
```

### 2.3 NumPy 数组互操作？

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

void process_array(py::array_t<double> input) {
    auto buf = input.request();
    double* ptr = static_cast<double*>(buf.ptr);

    // 处理数组数据
    for (size_t i = 0; i < buf.size; i++) {
        ptr[i] *= 2.0;  // 每个元素乘以 2
    }
}

PYBIND11_MODULE(mymodule, m) {
    m.def("process_array", &process_array);
}
```

**Python 使用**：
```python
import numpy as np
import mymodule

arr = np.array([1.0, 2.0, 3.0])
mymodule.process_array(arr)  # arr 变为 [2.0, 4.0, 6.0]
```

---

## 3. C++ 与 Java 互操作（JNI）

### 3.1 Java 调用 C++？

**Java 代码**：
```java
public class NativeLib {
    // 加载本地库
    static {
        System.loadLibrary("native");
    }

    // 声明本地方法
    public native int add(int a, int b);
    public native String getVersion();
}
```

**生成 JNI 头文件**：
```bash
javac NativeLib.java
javah -jni NativeLib
```

**C++ 实现**：
```cpp
#include <jni.h>
#include "NativeLib.h"

JNIEXPORT jint JNICALL Java_NativeLib_add(
    JNIEnv* env,
    jobject obj,
    jint a,
    jint b
) {
    return a + b;
}

JNIEXPORT jstring JNICALL Java_NativeLib_getVersion(
    JNIEnv* env,
    jobject obj
) {
    return env->NewStringUTF("1.0.0");
}
```

### 3.2 C++ 调用 Java？

```cpp
#include <jni.h>

void call_java_method() {
    JavaVM* jvm;
    JNIEnv* env;

    // 创建 JVM（省略初始化代码）
    // ...

    // 查找类
    jclass cls = env->FindClass("java/lang/String");

    // 获取方法 ID
    jmethodID mid = env->GetMethodID(cls, "length", "()I");

    // 创建对象
    jstring str = env->NewStringUTF("Hello");

    // 调用方法
    jint length = env->CallIntMethod(str, mid);

    std::cout << "字符串长度: " << length << "\n";
}
```

### 3.3 JNI 类型映射？

| Java 类型 | JNI 类型 | C++ 类型 |
|-----------|----------|----------|
| `boolean` | `jboolean` | `unsigned char` |
| `byte` | `jbyte` | `signed char` |
| `char` | `jchar` | `unsigned short` |
| `short` | `jshort` | `short` |
| `int` | `jint` | `int` |
| `long` | `jlong` | `long long` |
| `float` | `jfloat` | `float` |
| `double` | `jdouble` | `double` |
| `String` | `jstring` | - |
| `Object` | `jobject` | - |

---

## 4. C++ 与 C# 互操作

### 4.1 C# 调用 C++（P/Invoke）？

**C++ 代码（DLL）**：
```cpp
// mylib.h
extern "C" {
    __declspec(dllexport) int add(int a, int b);
    __declspec(dllexport) const char* get_message();
}

// mylib.cpp
extern "C" {
    __declspec(dllexport) int add(int a, int b) {
        return a + b;
    }

    __declspec(dllexport) const char* get_message() {
        return "Hello from C++";
    }
}
```

**C# 代码**：
```csharp
using System;
using System.Runtime.InteropServices;

class Program {
    [DllImport("mylib.dll")]
    public static extern int add(int a, int b);

    [DllImport("mylib.dll")]
    [return: MarshalAs(UnmanagedType.LPStr)]
    public static extern string get_message();

    static void Main() {
        int result = add(1, 2);  // 3
        string msg = get_message();  // "Hello from C++"
    }
}
```

### 4.2 C++/CLI？

```cpp
// C++/CLI 代码（.NET 与 C++ 的桥梁）
#using <System.dll>

using namespace System;

public ref class ManagedWrapper {
public:
    int Add(int a, int b) {
        return native_add(a, b);  // 调用本地 C++
    }

private:
    int native_add(int a, int b) {
        return a + b;
    }
};

// C# 使用
// var wrapper = new ManagedWrapper();
// int result = wrapper.Add(1, 2);
```

---

## 5. C++ 与 JavaScript 互操作

### 5.1 WebAssembly（Emscripten）？

**C++ 代码**：
```cpp
#include <emscripten.h>

// 导出函数到 JavaScript
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int add(int a, int b) {
        return a + b;
    }
}
```

**编译**：
```bash
emcc mycode.cpp -o mycode.js -s EXPORTED_FUNCTIONS='["_add"]'
```

**JavaScript 使用**：
```javascript
const Module = require('./mycode.js');

Module.onRuntimeInitialized = () => {
    const result = Module._add(1, 2);  // 3
    console.log(result);
};
```

### 5.2 Node.js（N-API）？

```cpp
#include <node_api.h>

napi_value Add(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    int32_t a, b;
    napi_get_value_int32(env, args[0], &a);
    napi_get_value_int32(env, args[1], &b);

    napi_value result;
    napi_create_int32(env, a + b, &result);
    return result;
}

napi_value Init(napi_env env, napi_value exports) {
    napi_value fn;
    napi_create_function(env, nullptr, 0, Add, nullptr, &fn);
    napi_set_named_property(env, exports, "add", fn);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
```

---

## 6. C++ 与脚本语言（Lua）

### 6.1 Lua 调用 C++？

```cpp
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

// C++ 函数
int cpp_add(lua_State* L) {
    int a = luaL_checkinteger(L, 1);
    int b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a + b);
    return 1;  // 返回值个数
}

// 注册函数
void register_functions(lua_State* L) {
    lua_register(L, "add", cpp_add);
}

// 使用
lua_State* L = luaL_newstate();
luaL_openlibs(L);

register_functions(L);

luaL_dostring(L, "print(add(1, 2))");  // 输出 3

lua_close(L);
```

### 6.2 C++ 调用 Lua？

```cpp
lua_State* L = luaL_newstate();
luaL_openlibs(L);

// 加载 Lua 脚本
luaL_dofile(L, "script.lua");

// 调用 Lua 函数
lua_getglobal(L, "multiply");  // 获取函数
lua_pushinteger(L, 3);         // 参数 1
lua_pushinteger(L, 4);         // 参数 2
lua_call(L, 2, 1);             // 调用（2 个参数，1 个返回值）

int result = lua_tointeger(L, -1);  // 获取返回值
lua_pop(L, 1);

std::cout << "结果: " << result << "\n";

lua_close(L);
```

---

## 7. 跨语言最佳实践

### 7.1 通用原则？

1. **使用 C 接口作为边界**：
   - 最广泛支持
   - 避免名称修饰问题
   - 避免 C++ 特定类型（`std::string`、`std::vector`）

2. **明确所有权**：
   - 谁分配，谁释放
   - 避免跨语言边界传递裸指针

3. **错误处理**：
   - 不要跨语言边界抛异常
   - 使用错误码或回调

4. **线程安全**：
   - 确保跨语言调用的线程安全
   - 注意 GIL（Python Global Interpreter Lock）

### 7.2 数据传递策略？

**简单类型**：
```cpp
// ✅ 直接传递
extern "C" int add(int a, int b);
```

**字符串**：
```cpp
// ✅ C 字符串
extern "C" void process(const char* str);

// ❌ 不要传递 std::string
extern "C" void process(std::string str);  // 错误
```

**复杂数据**：
```cpp
// ✅ POD 结构体
struct Data {
    int id;
    double value;
    char name[32];
};

extern "C" void process(const Data* data);

// ❌ 不要包含 C++ 对象
struct BadData {
    std::string name;  // 错误
    std::vector<int> values;  // 错误
};
```

**数组**：
```cpp
// ✅ 指针 + 长度
extern "C" void process_array(const int* data, size_t length);

// ❌ 不要传递 std::vector
extern "C" void process_array(const std::vector<int>& data);  // 错误
```

### 7.3 生命周期管理？

```cpp
// ✅ 方案 1：调用者负责释放
extern "C" char* get_message() {
    char* msg = (char*)malloc(100);
    strcpy(msg, "Hello");
    return msg;  // 调用者需要 free
}

// ✅ 方案 2：提供释放函数
extern "C" char* create_message();
extern "C" void free_message(char* msg) {
    free(msg);
}

// ✅ 方案 3：使用不透明句柄
typedef struct MessageImpl* MessageHandle;

extern "C" MessageHandle message_create();
extern "C" const char* message_get(MessageHandle handle);
extern "C" void message_destroy(MessageHandle handle);
```

---

## 8. 常见问题

### 8.1 名称修饰问题？

```bash
# 查看符号
nm mylib.so | grep add

# 未加 extern "C"
# _Z3addii  (mangled)

# 加了 extern "C"
# add  (C linkage)

# 使用 c++filt 解码
c++filt _Z3addii  # 输出: add(int, int)
```

### 8.2 ABI 兼容性？

```cpp
// ❌ 不同编译器的 std::string 布局不同
extern "C" void process(std::string str);  // 危险

// ✅ 使用 C 兼容类型
extern "C" void process(const char* str);
```

### 8.3 异常处理？

```cpp
// ❌ 异常跨越语言边界
extern "C" int risky_function() {
    throw std::runtime_error("error");  // 危险
}

// ✅ 捕获所有异常
extern "C" int safe_function() {
    try {
        // 可能抛异常的代码
        return 0;
    } catch (...) {
        return -1;  // 错误码
    }
}
```

---

## 总结

### 跨语言方案选择

| 目标语言 | 推荐方案 | 工具/库 |
|----------|----------|---------|
| C | `extern "C"` | 无 |
| Python | 扩展模块 | pybind11, ctypes |
| Java | JNI | JNI |
| C# | P/Invoke, C++/CLI | .NET |
| JavaScript | WebAssembly, N-API | Emscripten |
| Lua | Lua C API | - |

### 关键记忆点

1. **C 接口是通用边界**：`extern "C"` + POD 类型
2. **避免 C++ 特定类型**：`std::string`、`std::vector` 等
3. **不要跨边界抛异常**
4. **明确内存所有权**：谁分配，谁释放
5. **使用不透明句柄**：隐藏实现细节
6. **工具**：pybind11（Python）、JNI（Java）、P/Invoke（C#）
7. **WebAssembly**：C++ 编译为浏览器可执行代码

---

## 参考资源

- [pybind11 Documentation](https://pybind11.readthedocs.io/)
- [JNI Specification](https://docs.oracle.com/javase/8/docs/technotes/guides/jni/)
- [Emscripten](https://emscripten.org/)
- [Lua C API](https://www.lua.org/manual/5.4/manual.html#4)
- [P/Invoke Tutorial](https://docs.microsoft.com/en-us/dotnet/standard/native-interop/pinvoke)
