# 02 - JS 与原生代码绑定机制

## 概述

本章深入剖析前端容器中 JavaScript 与 C++/原生代码的交互机制，包括 V8 绑定原理、同步/异步调用、数据序列化等核心技术。

---

## 1. 绑定机制概览

### 1.1 为什么需要 JS-Native 绑定

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       JS-Native 绑定的必要性                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  JavaScript 能力边界:                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  ✓ DOM 操作         ✓ 网络请求 (fetch)    ✓ Canvas/WebGL           │   │
│  │  ✓ 事件处理         ✓ LocalStorage        ✓ WebSocket              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  原生代码能力 (JS 无法直接访问):                                            │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  ✗ 文件系统读写      ✗ 系统进程管理       ✗ 硬件设备访问            │   │
│  │  ✗ 原生 UI 控件      ✗ 数据库直连         ✗ 高性能计算              │   │
│  │  ✗ 系统通知          ✗ 剪贴板深度操作     ✗ 注册表/系统配置         │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  解决方案: 通过绑定机制，将原生能力暴露给 JavaScript                        │
│                                                                             │
│  ┌───────────┐        绑定层          ┌───────────┐                        │
│  │    JS     │ ◄─────────────────────► │  Native   │                        │
│  │  前端代码  │   (V8 Extension/IPC)   │  C++ 代码  │                        │
│  └───────────┘                        └───────────┘                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 绑定方式对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          绑定方式对比                                       │
├─────────────┬───────────────┬───────────────┬───────────────────────────────┤
│   方式       │   执行位置     │    性能      │           适用场景            │
├─────────────┼───────────────┼───────────────┼───────────────────────────────┤
│ V8 Extension │ Renderer进程  │  最高(同进程) │ 高频调用、计算密集型          │
│ IPC Message  │ 跨进程        │  中等(序列化) │ 需要访问主进程资源            │
│ DevTools协议 │ 跨进程(调试)  │  较低        │ 调试、自动化测试              │
│ WebSocket    │ 网络通信      │  较低        │ 远程调用、跨机器              │
└─────────────┴───────────────┴───────────────┴───────────────────────────────┘
```

---

## 2. V8 引擎绑定原理

### 2.1 V8 核心概念

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          V8 核心对象模型                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Isolate (隔离区)                                                           │
│  └── 独立的 V8 实例，拥有独立的堆内存                                       │
│      └── 一个进程可以有多个 Isolate                                         │
│                                                                             │
│  Context (上下文)                                                           │
│  └── JavaScript 执行环境                                                    │
│      └── 包含全局对象 (window)                                              │
│      └── 一个 Isolate 可以有多个 Context                                    │
│                                                                             │
│  Handle (句柄)                                                              │
│  └── 指向 V8 堆中对象的智能指针                                             │
│      ├── Local<T>: 栈上临时句柄，作用域结束自动释放                         │
│      ├── Persistent<T>: 持久句柄，需手动释放                                │
│      └── Global<T>: 类似 Persistent，支持移动语义                           │
│                                                                             │
│  HandleScope (句柄作用域)                                                   │
│  └── 管理 Local 句柄的生命周期                                              │
│      └── RAII 风格，离开作用域自动清理所有 Local 句柄                       │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Isolate                                                            │   │
│  │  ┌─────────────────────────┐  ┌─────────────────────────┐          │   │
│  │  │ Context A              │  │ Context B              │          │   │
│  │  │ ┌─────────────────────┐│  │ ┌─────────────────────┐│          │   │
│  │  │ │ Global Object       ││  │ │ Global Object       ││          │   │
│  │  │ │  - window           ││  │ │  - window           ││          │   │
│  │  │ │  - document         ││  │ │  - document         ││          │   │
│  │  │ │  - nativeAPI ←绑定  ││  │ │  - nativeAPI ←绑定  ││          │   │
│  │  │ └─────────────────────┘│  │ └─────────────────────┘│          │   │
│  │  └─────────────────────────┘  └─────────────────────────┘          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 V8 原生函数绑定

```cpp
#include <v8.h>

using namespace v8;

// 原生函数：计算文件 MD5
void CalculateFileMD5(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope handle_scope(isolate);

    // 1. 参数校验
    if (args.Length() < 1 || !args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8Literal(isolate, "Expected string argument")));
        return;
    }

    // 2. 获取参数
    String::Utf8Value filepath(isolate, args[0]);
    std::string path(*filepath);

    // 3. 调用原生代码
    std::string md5 = native::calculateMD5(path);  // 假设的原生函数

    // 4. 返回结果
    Local<String> result = String::NewFromUtf8(
        isolate, md5.c_str(), NewStringType::kNormal).ToLocalChecked();
    args.GetReturnValue().Set(result);
}

// 注册函数到全局对象
void RegisterNativeAPI(Local<Context> context) {
    Isolate* isolate = context->GetIsolate();
    HandleScope handle_scope(isolate);

    Local<Object> global = context->Global();

    // 创建 native 命名空间对象
    Local<Object> nativeObj = Object::New(isolate);

    // 绑定函数
    Local<FunctionTemplate> md5Func = FunctionTemplate::New(isolate, CalculateFileMD5);
    nativeObj->Set(context,
        String::NewFromUtf8Literal(isolate, "calculateMD5"),
        md5Func->GetFunction(context).ToLocalChecked()
    ).Check();

    // 挂载到全局对象
    global->Set(context,
        String::NewFromUtf8Literal(isolate, "native"),
        nativeObj
    ).Check();
}

// JavaScript 调用示例:
// const hash = native.calculateMD5("/path/to/file.txt");
// console.log(hash);  // "d41d8cd98f00b204e9800998ecf8427e"
```

### 2.3 异步函数绑定

```cpp
// 异步读取文件内容
void ReadFileAsync(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope handle_scope(isolate);

    if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8Literal(isolate, "Expected (string, callback)")));
        return;
    }

    // 获取文件路径
    String::Utf8Value filepath(isolate, args[0]);
    std::string path(*filepath);

    // 保存回调函数 (使用 Persistent 跨作用域保持)
    auto callback = std::make_shared<Global<Function>>(isolate, Local<Function>::Cast(args[1]));

    // 保存 Isolate 引用
    auto isolate_ptr = isolate;

    // 在工作线程中执行 IO 操作
    std::thread([path, callback, isolate_ptr]() {
        // 读取文件 (阻塞操作)
        std::string content;
        bool success = native::readFile(path, content);

        // 回到 V8 线程执行回调
        // 注意：实际实现需要使用平台特定的任务调度机制
        PostToV8Thread([callback, content, success, isolate_ptr]() {
            Isolate* isolate = isolate_ptr;
            HandleScope handle_scope(isolate);
            Local<Context> context = isolate->GetCurrentContext();

            Local<Function> func = callback->Get(isolate);

            Local<Value> argv[2];
            if (success) {
                argv[0] = Null(isolate);
                argv[1] = String::NewFromUtf8(isolate, content.c_str()).ToLocalChecked();
            } else {
                argv[0] = Exception::Error(
                    String::NewFromUtf8Literal(isolate, "Failed to read file"));
                argv[1] = Undefined(isolate);
            }

            func->Call(context, context->Global(), 2, argv).ToLocalChecked();
        });
    }).detach();
}

// JavaScript 调用:
// native.readFile("/path/to/file.txt", (err, content) => {
//     if (err) console.error(err);
//     else console.log(content);
// });
```

### 2.4 Promise 支持

```cpp
// 返回 Promise 的异步函数
void FetchDataAsync(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope handle_scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    // 创建 Promise resolver
    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();

    // 返回 Promise 给 JS
    args.GetReturnValue().Set(resolver->GetPromise());

    // 保存 resolver 用于异步回调
    auto resolver_ptr = std::make_shared<Global<Promise::Resolver>>(isolate, resolver);

    String::Utf8Value url(isolate, args[0]);
    std::string urlStr(*url);

    std::thread([resolver_ptr, urlStr, isolate]() {
        // 模拟网络请求
        std::string data;
        bool success = native::fetchData(urlStr, data);

        PostToV8Thread([resolver_ptr, data, success, isolate]() {
            HandleScope handle_scope(isolate);
            Local<Context> context = isolate->GetCurrentContext();
            Local<Promise::Resolver> resolver = resolver_ptr->Get(isolate);

            if (success) {
                Local<String> result = String::NewFromUtf8(
                    isolate, data.c_str()).ToLocalChecked();
                resolver->Resolve(context, result).Check();
            } else {
                Local<Value> error = Exception::Error(
                    String::NewFromUtf8Literal(isolate, "Fetch failed"));
                resolver->Reject(context, error).Check();
            }
        });
    }).detach();
}

// JavaScript 调用:
// const data = await native.fetchData("https://api.example.com/data");
// 或
// native.fetchData("https://api.example.com/data")
//     .then(data => console.log(data))
//     .catch(err => console.error(err));
```

---

## 3. CEF 中的 JS 绑定

### 3.1 CefV8Handler 方式

```cpp
#include "include/cef_v8.h"

class MyV8Handler : public CefV8Handler {
public:
    bool Execute(const CefString& name,
                 CefRefPtr<CefV8Value> object,
                 const CefV8ValueList& arguments,
                 CefRefPtr<CefV8Value>& retval,
                 CefString& exception) override {

        if (name == "showMessage") {
            // 获取参数
            if (arguments.size() > 0 && arguments[0]->IsString()) {
                CefString message = arguments[0]->GetStringValue();

                // 调用原生消息框 (需要通过 IPC 发送到 Browser 进程)
                CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ShowMessage");
                msg->GetArgumentList()->SetString(0, message);

                CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
                browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, msg);

                retval = CefV8Value::CreateBool(true);
                return true;
            }
            exception = "Expected string argument";
            return true;
        }

        if (name == "getSystemInfo") {
            // 同步返回对象
            CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(nullptr, nullptr);
            obj->SetValue("platform", CefV8Value::CreateString("Windows"),
                          V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("version", CefV8Value::CreateString("10.0"),
                          V8_PROPERTY_ATTRIBUTE_READONLY);
            obj->SetValue("arch", CefV8Value::CreateString("x64"),
                          V8_PROPERTY_ATTRIBUTE_READONLY);
            retval = obj;
            return true;
        }

        return false;  // 未处理的函数
    }

    IMPLEMENT_REFCOUNTING(MyV8Handler);
};

// 在 Renderer 进程中注册
class MyRenderProcessHandler : public CefRenderProcessHandler {
public:
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override {
        // 获取全局对象
        CefRefPtr<CefV8Value> global = context->GetGlobal();

        // 创建 native 对象
        CefRefPtr<CefV8Value> nativeObj = CefV8Value::CreateObject(nullptr, nullptr);

        // 创建函数
        CefRefPtr<MyV8Handler> handler = new MyV8Handler();

        CefRefPtr<CefV8Value> showMsgFunc = CefV8Value::CreateFunction("showMessage", handler);
        CefRefPtr<CefV8Value> getInfoFunc = CefV8Value::CreateFunction("getSystemInfo", handler);

        nativeObj->SetValue("showMessage", showMsgFunc, V8_PROPERTY_ATTRIBUTE_NONE);
        nativeObj->SetValue("getSystemInfo", getInfoFunc, V8_PROPERTY_ATTRIBUTE_NONE);

        // 挂载到 window 对象
        global->SetValue("native", nativeObj, V8_PROPERTY_ATTRIBUTE_NONE);
    }

    IMPLEMENT_REFCOUNTING(MyRenderProcessHandler);
};

// JavaScript 调用:
// native.showMessage("Hello from JS!");
// const info = native.getSystemInfo();
// console.log(info.platform);  // "Windows"
```

### 3.2 使用 CefMessageRouter (推荐)

```cpp
// 更简洁的异步消息机制
#include "include/wrapper/cef_message_router.h"

class MyMessageHandler : public CefMessageRouterBrowserSide::Handler {
public:
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64_t query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override {

        // 解析请求 (假设是 JSON)
        // {"action": "readFile", "path": "/path/to/file"}
        std::string requestStr = request.ToString();

        if (requestStr.find("readFile") != std::string::npos) {
            // 异步读取文件
            std::thread([callback, requestStr]() {
                std::string content = "File content here...";
                bool success = true;

                if (success) {
                    callback->Success(content);
                } else {
                    callback->Failure(404, "File not found");
                }
            }).detach();

            return true;  // 已处理
        }

        return false;  // 未处理
    }

    void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64_t query_id) override {
        // 查询被取消 (例如页面导航)
    }
};

// JavaScript 端调用:
// window.cefQuery({
//     request: JSON.stringify({action: "readFile", path: "/path/to/file"}),
//     onSuccess: function(response) {
//         console.log("File content:", response);
//     },
//     onFailure: function(errorCode, errorMessage) {
//         console.error("Error:", errorCode, errorMessage);
//     }
// });
```

---

## 4. Electron 中的 JS 绑定

### 4.1 contextBridge (安全方式)

```javascript
// preload.js - 在渲染进程中但有 Node.js 访问权限
const { contextBridge, ipcRenderer } = require('electron');

// 暴露安全的 API 到渲染进程
contextBridge.exposeInMainWorld('electronAPI', {
    // 单向通信: Renderer → Main
    sendMessage: (channel, data) => {
        const validChannels = ['save-file', 'open-dialog', 'minimize'];
        if (validChannels.includes(channel)) {
            ipcRenderer.send(channel, data);
        }
    },

    // 双向通信: Renderer ↔ Main (异步)
    invoke: async (channel, data) => {
        const validChannels = ['read-file', 'get-app-path', 'show-dialog'];
        if (validChannels.includes(channel)) {
            return await ipcRenderer.invoke(channel, data);
        }
        throw new Error('Invalid channel');
    },

    // 监听 Main → Renderer 消息
    on: (channel, callback) => {
        const validChannels = ['update-available', 'download-progress'];
        if (validChannels.includes(channel)) {
            ipcRenderer.on(channel, (event, ...args) => callback(...args));
        }
    },

    // 同步调用 (阻塞，慎用)
    sendSync: (channel, data) => {
        return ipcRenderer.sendSync(channel, data);
    }
});
```

```javascript
// main.js - 主进程
const { ipcMain, dialog } = require('electron');
const fs = require('fs');

// 处理 invoke 调用
ipcMain.handle('read-file', async (event, filePath) => {
    try {
        const content = await fs.promises.readFile(filePath, 'utf-8');
        return { success: true, data: content };
    } catch (err) {
        return { success: false, error: err.message };
    }
});

ipcMain.handle('show-dialog', async (event, options) => {
    const result = await dialog.showOpenDialog(options);
    return result;
});

// 处理 send 调用
ipcMain.on('save-file', (event, { path, content }) => {
    fs.writeFileSync(path, content);
    event.reply('save-complete', { success: true });
});
```

```javascript
// renderer.js - 渲染进程中使用
// 异步调用
const content = await window.electronAPI.invoke('read-file', '/path/to/file');
console.log(content);

// 单向发送
window.electronAPI.sendMessage('save-file', { path: '/path', content: 'data' });

// 监听主进程消息
window.electronAPI.on('update-available', (version) => {
    console.log('New version:', version);
});
```

### 4.2 Electron 安全模型演进

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Electron 安全模型演进                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  旧模式 (不推荐):                                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ nodeIntegration: true                                               │   │
│  │ ├── 渲染进程可直接使用 require()                                    │   │
│  │ ├── 可访问所有 Node.js API                                          │   │
│  │ └── 安全风险: XSS 攻击可获得系统完全控制权                          │   │
│  │                                                                     │   │
│  │ // 渲染进程中直接使用 (危险!)                                       │   │
│  │ const fs = require('fs');                                           │   │
│  │ fs.unlinkSync('/important/file');  // XSS 可执行此代码!             │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  新模式 (推荐):                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ nodeIntegration: false                                              │   │
│  │ contextIsolation: true                                              │   │
│  │ sandbox: true                                                       │   │
│  │                                                                     │   │
│  │ ├── 渲染进程无法访问 Node.js                                        │   │
│  │ ├── 通过 contextBridge 暴露白名单 API                               │   │
│  │ ├── preload 脚本作为安全边界                                        │   │
│  │ └── XSS 攻击只能访问暴露的有限 API                                  │   │
│  │                                                                     │   │
│  │ // 渲染进程只能使用暴露的 API                                       │   │
│  │ window.electronAPI.readFile(path);  // 安全，有验证                 │   │
│  │ require('fs');  // 报错: require is not defined                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 5. 数据类型转换

### 5.1 JS 与 C++ 类型映射

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      JS ↔ C++ 类型映射表                                    │
├─────────────────────┬───────────────────────┬───────────────────────────────┤
│    JavaScript       │       C++ (V8)         │          说明               │
├─────────────────────┼───────────────────────┼───────────────────────────────┤
│ undefined           │ Undefined              │ 未定义值                     │
│ null                │ Null                   │ 空值                         │
│ boolean             │ Boolean                │ true/false                   │
│ number              │ Number / Integer       │ IEEE 754 双精度 / 整数       │
│ bigint              │ BigInt                 │ 任意精度整数 (ES2020)        │
│ string              │ String                 │ UTF-16 编码                  │
│ symbol              │ Symbol                 │ 唯一标识符                   │
│ object              │ Object                 │ 键值对                       │
│ array               │ Array                  │ 索引数组                     │
│ function            │ Function               │ 可调用对象                   │
│ ArrayBuffer         │ ArrayBuffer            │ 原始二进制数据               │
│ TypedArray          │ Uint8Array 等          │ 类型化数组                   │
│ Promise             │ Promise                │ 异步操作                     │
│ Date                │ Date                   │ 日期时间                     │
│ Map/Set             │ Map/Set                │ 集合类型                     │
└─────────────────────┴───────────────────────┴───────────────────────────────┘
```

### 5.2 复杂类型转换示例

```cpp
// C++ 对象转 JS 对象
CefRefPtr<CefV8Value> CppToJsObject(const UserInfo& user) {
    CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(nullptr, nullptr);

    obj->SetValue("id", CefV8Value::CreateInt(user.id),
                  V8_PROPERTY_ATTRIBUTE_NONE);
    obj->SetValue("name", CefV8Value::CreateString(user.name),
                  V8_PROPERTY_ATTRIBUTE_NONE);
    obj->SetValue("email", CefV8Value::CreateString(user.email),
                  V8_PROPERTY_ATTRIBUTE_NONE);
    obj->SetValue("isActive", CefV8Value::CreateBool(user.isActive),
                  V8_PROPERTY_ATTRIBUTE_NONE);

    // 嵌套数组
    CefRefPtr<CefV8Value> roles = CefV8Value::CreateArray(user.roles.size());
    for (size_t i = 0; i < user.roles.size(); ++i) {
        roles->SetValue(i, CefV8Value::CreateString(user.roles[i]));
    }
    obj->SetValue("roles", roles, V8_PROPERTY_ATTRIBUTE_NONE);

    return obj;
}

// JS 对象转 C++ 对象
bool JsToCppObject(CefRefPtr<CefV8Value> jsObj, UserInfo& user) {
    if (!jsObj->IsObject()) return false;

    CefRefPtr<CefV8Value> idVal = jsObj->GetValue("id");
    CefRefPtr<CefV8Value> nameVal = jsObj->GetValue("name");

    if (!idVal->IsInt() || !nameVal->IsString()) return false;

    user.id = idVal->GetIntValue();
    user.name = nameVal->GetStringValue().ToString();

    CefRefPtr<CefV8Value> emailVal = jsObj->GetValue("email");
    if (emailVal->IsString()) {
        user.email = emailVal->GetStringValue().ToString();
    }

    return true;
}
```

### 5.3 二进制数据传输

```cpp
// 传输二进制数据 (图片、文件等)
void SendBinaryData(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    // 读取文件为二进制
    std::vector<uint8_t> fileData = readBinaryFile("/path/to/image.png");

    // 创建 ArrayBuffer
    Local<ArrayBuffer> buffer = ArrayBuffer::New(
        isolate,
        fileData.size()
    );

    // 复制数据到 ArrayBuffer
    memcpy(buffer->GetBackingStore()->Data(),
           fileData.data(),
           fileData.size());

    // 创建 Uint8Array 视图
    Local<Uint8Array> uint8Array = Uint8Array::New(buffer, 0, fileData.size());

    args.GetReturnValue().Set(uint8Array);
}

// JavaScript 端:
// const data = native.readBinaryFile("/path/to/image.png");
// const blob = new Blob([data], { type: 'image/png' });
// const url = URL.createObjectURL(blob);
```

---

## 6. 性能优化

### 6.1 减少跨边界调用

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      性能优化策略                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  问题: JS ↔ Native 调用有开销                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ V8 调用: ~100-500ns 每次                                            │   │
│  │ IPC 调用: ~1-10μs 每次 (含序列化)                                   │   │
│  │ 跨进程: ~10-100μs 每次                                              │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  优化策略:                                                                  │
│                                                                             │
│  1. 批量调用 (Batching)                                                    │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ // 不好: 多次调用                                                │    │
│     │ for (const id of ids) {                                         │    │
│     │     await native.getItem(id);  // N 次 IPC                       │    │
│     │ }                                                                │    │
│     │                                                                  │    │
│     │ // 好: 一次调用                                                  │    │
│     │ const items = await native.getItems(ids);  // 1 次 IPC           │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  2. 缓存结果                                                               │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │ const cache = new Map();                                         │    │
│     │ async function getCached(key) {                                  │    │
│     │     if (cache.has(key)) return cache.get(key);                   │    │
│     │     const value = await native.getValue(key);                    │    │
│     │     cache.set(key, value);                                       │    │
│     │     return value;                                                │    │
│     │ }                                                                │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
│  3. 使用 SharedArrayBuffer (共享内存)                                      │
│     - 避免数据复制                                                         │
│     - 适用于大量数据交换                                                   │
│     - 需要处理同步问题 (Atomics)                                           │
│                                                                             │
│  4. 在 Native 侧完成计算密集型任务                                         │
│     - 避免频繁回调 JS                                                      │
│     - 一次性返回结果                                                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 6.2 异步优先

```cpp
// 避免阻塞 UI 线程
class AsyncTask {
public:
    // 在工作线程执行
    void Execute() {
        result_ = performHeavyComputation();
    }

    // 完成后回调 (在 V8 线程)
    void OnComplete(const FunctionCallbackInfo<Value>& callback) {
        Isolate* isolate = callback.GetIsolate();
        Local<Value> result = String::NewFromUtf8(
            isolate, result_.c_str()).ToLocalChecked();
        callback.GetReturnValue().Set(result);
    }

private:
    std::string result_;
};
```

---

## 7. 面试常见问题

### Q1: V8 的 HandleScope 是什么？为什么需要它？

**答案：**
- HandleScope 是 V8 的句柄作用域管理器，用于管理 Local 句柄的生命周期
- V8 使用 GC 管理堆内存，HandleScope 防止函数执行期间创建的对象被 GC 回收
- 离开作用域时，所有在该作用域内创建的 Local 句柄会被批量释放
- 如果不使用 HandleScope，每次创建对象都需要手动管理生命周期

```cpp
void Example(Isolate* isolate) {
    HandleScope scope(isolate);  // 创建作用域

    // 这些 Local 句柄在 scope 结束时自动释放
    Local<String> str1 = String::NewFromUtf8Literal(isolate, "hello");
    Local<String> str2 = String::NewFromUtf8Literal(isolate, "world");

    // 如果需要返回值，使用 EscapableHandleScope
}  // scope 结束，str1、str2 释放
```

### Q2: CEF 中 Renderer 进程如何安全地调用 Browser 进程的功能？

**答案：**
1. Renderer 进程通过 `CefProcessMessage` 发送 IPC 消息
2. Browser 进程在 `OnProcessMessageReceived` 中接收和验证
3. 关键点：
   - Browser 进程必须验证所有参数（Renderer 不可信）
   - 敏感操作应该有权限检查
   - 使用 CefMessageRouter 可以简化异步通信

### Q3: Electron 的 contextIsolation 解决什么问题？

**答案：**
- **问题**：如果 preload 脚本和网页共享同一个 JavaScript 上下文，恶意网页可以通过原型链污染等方式访问 Node.js API
- **解决**：contextIsolation 让 preload 脚本在独立的上下文中运行
- **实现**：通过 contextBridge 安全地暴露白名单 API
- **效果**：即使网页被 XSS 攻击，也无法访问未暴露的 Node.js 功能

### Q4: 如何高效地在 JS 和 Native 之间传输大量二进制数据？

**答案：**
1. **使用 ArrayBuffer/TypedArray**：避免 Base64 编码开销
2. **SharedArrayBuffer**：真正的零拷贝共享内存（需要 COOP/COEP 头）
3. **分块传输**：大文件分批传输，避免阻塞
4. **流式处理**：使用可读/可写流逐步处理

```javascript
// 使用 SharedArrayBuffer 实现零拷贝
const sharedBuffer = new SharedArrayBuffer(1024 * 1024);  // 1MB
const view = new Uint8Array(sharedBuffer);

// Native 端直接写入 sharedBuffer
native.writeToSharedBuffer(sharedBuffer);

// JS 端直接读取，无需复制
console.log(view[0], view[1], view[2]);
```

### Q5: 为什么 Chromium 的 JS 绑定函数不能直接访问 DOM？

**答案：**
- DOM 由 Blink 引擎管理，V8 只是 JS 执行引擎
- 原生绑定函数在 V8 层执行，Blink DOM 对象需要通过特定的绑定机制访问
- Blink 使用 IDL (Interface Definition Language) 生成 DOM 绑定代码
- 自定义绑定如需操作 DOM，应该通过执行 JS 代码间接实现

```cpp
// 正确方式：通过执行 JS 代码操作 DOM
frame->ExecuteJavaScript(
    "document.getElementById('result').textContent = 'Hello';",
    frame->GetURL(),
    0
);
```
