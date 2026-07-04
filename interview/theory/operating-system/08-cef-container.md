# 08 - CEF 容器 (Chromium Embedded Framework)

## 简介

CEF (Chromium Embedded Framework) 是一个基于 Chromium 的开源框架，允许开发者在应用程序中嵌入浏览器功能。广泛应用于桌面应用、游戏内嵌浏览器、IDE 等场景。

---

## 1. CEF 基础概念

### 1.1 什么是 CEF

```
┌─────────────────────────────────────────────────────────────────────┐
│                         CEF 定位                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ CEF = Chromium 内核 + 简化的嵌入式 API                              │
│                                                                     │
│ 核心价值：                                                          │
│   - 在原生应用中嵌入完整的 Web 浏览器功能                           │
│   - 支持 HTML5、CSS3、JavaScript、WebGL                             │
│   - 跨平台：Windows、macOS、Linux                                   │
│   - C/C++ API，支持多语言绑定                                       │
│                                                                     │
│ 典型应用：                                                          │
│   - Electron（VS Code、Slack、Discord）                             │
│   - 游戏内嵌浏览器（Steam Overlay）                                 │
│   - 桌面应用（Spotify、微信 PC 版）                                 │
│   - IDE 预览面板                                                    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.2 CEF 架构概览

```
┌─────────────────────────────────────────────────────────────────────┐
│                        应用程序 (Host)                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                      CEF 框架层                              │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │    │
│  │  │ CefBrowser  │  │ CefFrame    │  │ CefClient   │          │    │
│  │  │ 浏览器实例  │  │ 页面框架    │  │ 事件回调    │          │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘          │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                      │
│  ┌───────────────────────────▼─────────────────────────────────┐    │
│  │                    Chromium 内核                             │    │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐        │    │
│  │  │  Blink   │ │   V8     │ │ 网络栈   │ │  GPU     │        │    │
│  │  │ 渲染引擎 │ │ JS 引擎  │ │          │ │ 加速     │        │    │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘        │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.3 CEF vs Electron vs WebView

```
┌─────────────────┬─────────────────────────────────────────────────────┐
│ 特性            │ CEF           │ Electron      │ WebView2/WebKit    │
├─────────────────┼───────────────┼───────────────┼────────────────────┤
│ 语言            │ C/C++         │ JavaScript    │ 系统原生           │
├─────────────────┼───────────────┼───────────────┼────────────────────┤
│ 打包体积        │ ~100MB        │ ~150MB        │ 系统自带           │
├─────────────────┼───────────────┼───────────────┼────────────────────┤
│ 内存占用        │ 较高          │ 高            │ 较低               │
├─────────────────┼───────────────┼───────────────┼────────────────────┤
│ 定制性          │ 高            │ 中            │ 低                 │
├─────────────────┼───────────────┼───────────────┼────────────────────┤
│ 学习曲线        │ 陡峭          │ 平缓          │ 中等               │
├─────────────────┼───────────────┼───────────────┼────────────────────┤
│ 适用场景        │ 高性能桌面应用│ 快速开发      │ 轻量级嵌入         │
└─────────────────┴───────────────┴───────────────┴────────────────────┘
```

---

## 2. CEF 多进程架构

### 2.1 进程模型

```
CEF 继承 Chromium 的多进程架构：

┌─────────────────────────────────────────────────────────────────────┐
│                        Browser Process（主进程）                    │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │ 职责：                                                       │    │
│  │   - 窗口管理、用户输入                                       │    │
│  │   - 网络请求（Network Service）                              │    │
│  │   - 存储管理（Cookie、LocalStorage）                         │    │
│  │   - 进程管理、IPC 协调                                       │    │
│  │   - 插件管理                                                 │    │
│  └─────────────────────────────────────────────────────────────┘    │
│         │                    │                    │                 │
│         │ IPC                │ IPC                │ IPC             │
│         ▼                    ▼                    ▼                 │
│  ┌────────────┐       ┌────────────┐       ┌────────────┐          │
│  │ Renderer 1 │       │ Renderer 2 │       │ Renderer N │          │
│  │ (渲染进程) │       │ (渲染进程) │       │ (渲染进程) │          │
│  │            │       │            │       │            │          │
│  │ - Blink    │       │ - Blink    │       │ - Blink    │          │
│  │ - V8       │       │ - V8       │       │ - V8       │          │
│  │ - DOM/CSS  │       │ - DOM/CSS  │       │ - DOM/CSS  │          │
│  └────────────┘       └────────────┘       └────────────┘          │
│         │                                                           │
│         │                    GPU Process                            │
│         │              ┌────────────────┐                           │
│         └─────────────►│ GPU 加速渲染   │                           │
│                        │ WebGL、视频    │                           │
│                        └────────────────┘                           │
│                                                                     │
│  其他进程：                                                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐              │
│  │ Utility      │  │ Plugin       │  │ Network      │              │
│  │ (工具进程)   │  │ (插件进程)   │  │ (网络服务)   │              │
│  └──────────────┘  └──────────────┘  └──────────────┘              │
└─────────────────────────────────────────────────────────────────────┘

进程隔离优势：
  - 安全性：渲染进程沙箱化，一个页面崩溃不影响其他
  - 稳定性：进程独立，故障隔离
  - 性能：多核 CPU 并行处理
```

### 2.2 进程通信 (IPC)

```
CEF 进程间通信机制：

┌─────────────────────────────────────────────────────────────────────┐
│                     IPC 通信方式                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 1. CefProcessMessage（进程消息）                                    │
│                                                                     │
│    Browser Process                      Renderer Process            │
│         │                                      │                    │
│         │ ── SendProcessMessage ─────────────► │                    │
│         │                                      │                    │
│         │ ◄── SendProcessMessage ────────────  │                    │
│         │                                      │                    │
│                                                                     │
│    代码示例：                                                       │
│    // Browser → Renderer                                            │
│    CefRefPtr<CefProcessMessage> msg =                               │
│        CefProcessMessage::Create("my_message");                     │
│    msg->GetArgumentList()->SetString(0, "hello");                   │
│    browser->GetMainFrame()->SendProcessMessage(                     │
│        PID_RENDERER, msg);                                          │
│                                                                     │
│    // Renderer 接收                                                 │
│    bool OnProcessMessageReceived(                                   │
│        CefRefPtr<CefBrowser> browser,                               │
│        CefRefPtr<CefFrame> frame,                                   │
│        CefProcessId source_process,                                 │
│        CefRefPtr<CefProcessMessage> message) {                      │
│        if (message->GetName() == "my_message") {                    │
│            // 处理消息                                              │
│        }                                                            │
│        return true;                                                 │
│    }                                                                │
│                                                                     │
│ 2. CefV8Value（JavaScript 绑定）                                    │
│                                                                     │
│    C++ 函数注册到 JavaScript：                                      │
│    - 在 Renderer 进程中执行                                         │
│    - 通过 CefV8Handler 处理调用                                     │
│                                                                     │
│ 3. 共享内存（高性能数据传输）                                       │
│                                                                     │
│    用于大量数据传输，避免序列化开销                                 │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 2.3 线程模型

```
CEF 线程架构：

┌─────────────────────────────────────────────────────────────────────┐
│                      Browser Process 线程                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ ┌─────────────────┐                                                 │
│ │ TID_UI          │  UI 线程（主线程）                              │
│ │                 │  - 窗口消息循环                                 │
│ │                 │  - 用户输入处理                                 │
│ │                 │  - CEF 回调执行                                 │
│ └─────────────────┘                                                 │
│                                                                     │
│ ┌─────────────────┐                                                 │
│ │ TID_IO          │  IO 线程                                        │
│ │                 │  - IPC 消息处理                                 │
│ │                 │  - 网络事件                                     │
│ └─────────────────┘                                                 │
│                                                                     │
│ ┌─────────────────┐                                                 │
│ │ TID_FILE        │  文件线程                                       │
│ │                 │  - 文件 I/O 操作                                │
│ │                 │  - 避免阻塞 UI                                  │
│ └─────────────────┘                                                 │
│                                                                     │
│ ┌─────────────────┐                                                 │
│ │ TID_RENDERER    │  渲染器线程（在 Renderer 进程）                 │
│ │                 │  - JavaScript 执行                              │
│ │                 │  - DOM 操作                                     │
│ └─────────────────┘                                                 │
│                                                                     │
│ 线程安全规则：                                                      │
│   - CEF 对象通常只能在创建它的线程上访问                            │
│   - 使用 CefPostTask 在线程间传递任务                               │
│   - CefRefPtr 提供线程安全的引用计数                                │
│                                                                     │
│ // 跨线程执行                                                       │
│ CefPostTask(TID_UI, base::BindOnce(&MyClass::DoWork, this));        │
│                                                                     │
│ // 检查当前线程                                                     │
│ if (CefCurrentlyOn(TID_UI)) {                                       │
│     // 在 UI 线程                                                   │
│ }                                                                   │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. CEF 核心组件

### 3.1 CefApp

```cpp
// CefApp：应用程序入口，管理进程生命周期

class MyApp : public CefApp,
              public CefBrowserProcessHandler,
              public CefRenderProcessHandler {
public:
    // 返回 Browser 进程处理器
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // 返回 Renderer 进程处理器
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    // Browser 进程：上下文初始化完成
    void OnContextInitialized() override {
        // 创建浏览器窗口
        CefBrowserHost::CreateBrowser(window_info, client, url, settings, nullptr, nullptr);
    }

    // Renderer 进程：V8 上下文创建
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) override {
        // 注册 JavaScript 绑定
    }

    IMPLEMENT_REFCOUNTING(MyApp);
};
```

### 3.2 CefClient

```cpp
// CefClient：浏览器事件回调接口

class MyClient : public CefClient,
                 public CefDisplayHandler,
                 public CefLifeSpanHandler,
                 public CefLoadHandler {
public:
    // 返回各种 Handler
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }

    // === CefDisplayHandler ===
    // 标题变化
    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                      const CefString& title) override {
        // 更新窗口标题
        SetWindowText(hwnd_, title.c_str());
    }

    // 地址变化
    void OnAddressChange(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        const CefString& url) override {
        // 更新地址栏
    }

    // === CefLifeSpanHandler ===
    // 浏览器创建前
    bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      const CefString& target_url,
                      ...) override {
        // 返回 true 阻止弹窗
        return false;
    }

    // 浏览器创建后
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        browser_ = browser;
    }

    // 浏览器关闭前
    bool DoClose(CefRefPtr<CefBrowser> browser) override {
        return false;  // 允许关闭
    }

    // === CefLoadHandler ===
    // 加载状态变化
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                             bool isLoading,
                             bool canGoBack,
                             bool canGoForward) override {
        // 更新前进/后退按钮状态
    }

    // 加载完成
    void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                  CefRefPtr<CefFrame> frame,
                  int httpStatusCode) override {
        // 页面加载完成
    }

    // 加载错误
    void OnLoadError(CefRefPtr<CefBrowser> browser,
                    CefRefPtr<CefFrame> frame,
                    ErrorCode errorCode,
                    const CefString& errorText,
                    const CefString& failedUrl) override {
        // 显示错误页面
    }

    IMPLEMENT_REFCOUNTING(MyClient);
};
```

### 3.3 CefBrowser 与 CefFrame

```cpp
// CefBrowser：浏览器实例
// CefFrame：页面框架（主框架或 iframe）

┌─────────────────────────────────────────────────────────────────────┐
│                        CefBrowser                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 主要方法：                                                          │
│                                                                     │
│ // 导航                                                             │
│ browser->GetMainFrame()->LoadURL("https://example.com");            │
│ browser->GoBack();                                                  │
│ browser->GoForward();                                               │
│ browser->Reload();                                                  │
│ browser->StopLoad();                                                │
│                                                                     │
│ // 获取信息                                                         │
│ int id = browser->GetIdentifier();                                  │
│ bool loading = browser->IsLoading();                                │
│ CefRefPtr<CefFrame> frame = browser->GetMainFrame();                │
│                                                                     │
│ // 关闭                                                             │
│ browser->GetHost()->CloseBrowser(true);  // force_close             │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│                        CefFrame                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 主要方法：                                                          │
│                                                                     │
│ // 导航                                                             │
│ frame->LoadURL(url);                                                │
│ CefString url = frame->GetURL();                                    │
│ CefString name = frame->GetName();                                  │
│                                                                     │
│ // 执行 JavaScript                                                  │
│ frame->ExecuteJavaScript(                                           │
│     "alert('Hello from C++');",                                     │
│     frame->GetURL(),                                                │
│     0  // line number                                               │
│ );                                                                  │
│                                                                     │
│ // 获取页面源码                                                     │
│ class SourceVisitor : public CefStringVisitor {                     │
│     void Visit(const CefString& string) override {                  │
│         // string 包含 HTML 源码                                    │
│     }                                                               │
│ };                                                                  │
│ frame->GetSource(new SourceVisitor());                              │
│                                                                     │
│ // 框架层次                                                         │
│ bool isMain = frame->IsMain();                                      │
│ CefRefPtr<CefFrame> parent = frame->GetParent();                    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 4. JavaScript 与 C++ 交互

### 4.1 C++ 调用 JavaScript

```cpp
// 方法 1：ExecuteJavaScript（无返回值）
frame->ExecuteJavaScript(
    "document.body.style.backgroundColor = 'red';",
    frame->GetURL(),
    0
);

// 方法 2：CefV8Value 执行（有返回值，需在 Renderer 进程）
void OnContextCreated(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefV8Context> context) {
    context->Enter();

    CefRefPtr<CefV8Value> retval;
    CefRefPtr<CefV8Exception> exception;

    bool success = context->Eval(
        "1 + 2",
        frame->GetURL(),
        0,
        retval,
        exception
    );

    if (success && retval->IsInt()) {
        int result = retval->GetIntValue();  // 3
    }

    context->Exit();
}
```

### 4.2 JavaScript 调用 C++

```cpp
// 在 Renderer 进程中注册 C++ 函数

class MyV8Handler : public CefV8Handler {
public:
    bool Execute(const CefString& name,
                CefRefPtr<CefV8Value> object,
                const CefV8ValueList& arguments,
                CefRefPtr<CefV8Value>& retval,
                CefString& exception) override {

        if (name == "myNativeFunction") {
            // 获取参数
            if (arguments.size() > 0 && arguments[0]->IsString()) {
                std::string arg = arguments[0]->GetStringValue();

                // 处理逻辑
                std::string result = "Hello, " + arg;

                // 返回值
                retval = CefV8Value::CreateString(result);
                return true;
            }
        }

        return false;
    }

    IMPLEMENT_REFCOUNTING(MyV8Handler);
};

// 注册到 window 对象
void OnContextCreated(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefV8Context> context) {
    CefRefPtr<CefV8Value> global = context->GetGlobal();

    CefRefPtr<MyV8Handler> handler = new MyV8Handler();
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myNativeFunction", handler);

    global->SetValue("myNativeFunction", func, V8_PROPERTY_ATTRIBUTE_NONE);
}

// JavaScript 中调用
// let result = window.myNativeFunction("World");
// console.log(result);  // "Hello, World"
```

### 4.3 异步通信模式

```cpp
// 由于 C++ 和 JavaScript 在不同进程，复杂操作需要异步

┌─────────────────────────────────────────────────────────────────────┐
│                    异步通信流程                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ JavaScript                  IPC                    C++ (Browser)    │
│     │                        │                          │           │
│     │ ── callNative(data) ──►│                          │           │
│     │                        │ ── ProcessMessage ──────►│           │
│     │                        │                          │           │
│     │                        │         处理请求         │           │
│     │                        │                          │           │
│     │                        │ ◄── ProcessMessage ──────│           │
│     │ ◄── callback(result) ──│                          │           │
│     │                        │                          │           │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘

// Renderer 端：发送请求
void CallNativeAsync(const CefString& data,
                    CefRefPtr<CefV8Value> callback) {
    // 保存回调
    int callbackId = SaveCallback(callback);

    // 发送消息到 Browser
    CefRefPtr<CefProcessMessage> msg =
        CefProcessMessage::Create("native_call");
    msg->GetArgumentList()->SetInt(0, callbackId);
    msg->GetArgumentList()->SetString(1, data);

    browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, msg);
}

// Browser 端：处理请求
bool OnProcessMessageReceived(..., CefRefPtr<CefProcessMessage> message) {
    if (message->GetName() == "native_call") {
        int callbackId = message->GetArgumentList()->GetInt(0);
        CefString data = message->GetArgumentList()->GetString(1);

        // 处理请求（可能是耗时操作）
        std::string result = ProcessData(data);

        // 发送响应
        CefRefPtr<CefProcessMessage> response =
            CefProcessMessage::Create("native_response");
        response->GetArgumentList()->SetInt(0, callbackId);
        response->GetArgumentList()->SetString(1, result);

        browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, response);
        return true;
    }
    return false;
}

// Renderer 端：接收响应，调用回调
bool OnProcessMessageReceived(..., CefRefPtr<CefProcessMessage> message) {
    if (message->GetName() == "native_response") {
        int callbackId = message->GetArgumentList()->GetInt(0);
        CefString result = message->GetArgumentList()->GetString(1);

        // 获取保存的回调并执行
        CefRefPtr<CefV8Value> callback = GetCallback(callbackId);
        CefV8ValueList args;
        args.push_back(CefV8Value::CreateString(result));
        callback->ExecuteFunction(nullptr, args);

        return true;
    }
    return false;
}
```

---

## 5. 离屏渲染 (OSR)

### 5.1 OSR 概念

```
┌─────────────────────────────────────────────────────────────────────┐
│                    离屏渲染 (Off-Screen Rendering)                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 传统模式：CEF 创建原生窗口渲染                                      │
│ OSR 模式：CEF 渲染到内存缓冲区，应用自行绘制                        │
│                                                                     │
│ OSR 流程：                                                          │
│                                                                     │
│ CEF 渲染 ──► 像素缓冲区 ──► 应用获取 ──► 自定义绘制                 │
│                                                                     │
│ 应用场景：                                                          │
│   - 游戏内 UI（Unity、Unreal 集成）                                 │
│   - 自定义窗口系统                                                  │
│   - 透明/异形窗口                                                   │
│   - 多浏览器合成                                                    │
│   - 截图/录屏                                                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 5.2 OSR 实现

```cpp
class OSRClient : public CefClient, public CefRenderHandler {
public:
    CefRefPtr<CefRenderHandler> GetRenderHandler() override {
        return this;
    }

    // 获取视图大小
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
        rect.x = rect.y = 0;
        rect.width = width_;
        rect.height = height_;
    }

    // 渲染完成回调（核心）
    void OnPaint(CefRefPtr<CefBrowser> browser,
                PaintElementType type,
                const RectList& dirtyRects,
                const void* buffer,
                int width,
                int height) override {

        // buffer 包含 BGRA 格式的像素数据
        // 大小 = width * height * 4 字节

        if (type == PET_VIEW) {
            // 页面内容
            UpdateTexture(buffer, width, height, dirtyRects);
        } else if (type == PET_POPUP) {
            // 弹出层（如下拉菜单）
            UpdatePopupTexture(buffer, width, height);
        }
    }

    // 处理鼠标事件（需要手动转发）
    void SendMouseEvent(int x, int y, CefBrowserHost::MouseButtonType btn,
                       bool mouseUp, int clickCount) {
        CefMouseEvent event;
        event.x = x;
        event.y = y;
        event.modifiers = 0;

        browser_->GetHost()->SendMouseClickEvent(event, btn, mouseUp, clickCount);
    }

    // 处理键盘事件
    void SendKeyEvent(const CefKeyEvent& event) {
        browser_->GetHost()->SendKeyEvent(event);
    }

    // 处理滚轮
    void SendMouseWheelEvent(int x, int y, int deltaX, int deltaY) {
        CefMouseEvent event;
        event.x = x;
        event.y = y;
        browser_->GetHost()->SendMouseWheelEvent(event, deltaX, deltaY);
    }

private:
    void UpdateTexture(const void* buffer, int width, int height,
                      const RectList& dirtyRects) {
        // 更新 GPU 纹理（例如 OpenGL）
        glBindTexture(GL_TEXTURE_2D, textureId_);

        for (const auto& rect : dirtyRects) {
            // 只更新脏区域（优化）
            glTexSubImage2D(GL_TEXTURE_2D, 0,
                           rect.x, rect.y, rect.width, rect.height,
                           GL_BGRA, GL_UNSIGNED_BYTE,
                           (char*)buffer + (rect.y * width + rect.x) * 4);
        }
    }

    IMPLEMENT_REFCOUNTING(OSRClient);
};

// 创建 OSR 浏览器
CefWindowInfo window_info;
window_info.SetAsWindowless(nullptr);  // 无窗口模式

CefBrowserSettings settings;
settings.windowless_frame_rate = 60;  // 帧率

CefBrowserHost::CreateBrowser(window_info, client, url, settings, nullptr, nullptr);
```

### 5.3 OSR 性能优化

```
┌─────────────────────────────────────────────────────────────────────┐
│                    OSR 性能优化策略                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 1. 脏区域更新                                                       │
│    - 只更新 dirtyRects 区域，不是整个画面                           │
│    - 减少 CPU→GPU 数据传输                                          │
│                                                                     │
│ 2. 共享纹理（硬件加速）                                             │
│    - 使用 CefRenderHandler::OnAcceleratedPaint                      │
│    - 直接获取 GPU 纹理句柄                                          │
│    - 避免 CPU 拷贝                                                  │
│                                                                     │
│    void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,           │
│                           PaintElementType type,                    │
│                           const RectList& dirtyRects,               │
│                           void* shared_handle) override {           │
│        // shared_handle 是 GPU 纹理句柄                             │
│        // Windows: D3D11 共享纹理                                   │
│    }                                                                │
│                                                                     │
│ 3. 帧率控制                                                         │
│    - 根据场景调整 windowless_frame_rate                             │
│    - 静态页面可降低帧率                                             │
│                                                                     │
│ 4. 减少重绘                                                         │
│    - 避免频繁触发布局                                               │
│    - CSS 使用 transform 代替 top/left                               │
│                                                                     │
│ 5. 多线程处理                                                       │
│    - OnPaint 在 UI 线程调用                                         │
│    - 纹理更新可以异步                                               │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 6. 网络与资源拦截

### 6.1 请求拦截

```cpp
// CefRequestHandler：拦截网络请求

class MyRequestHandler : public CefRequestHandler {
public:
    // 请求发送前
    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request,
            bool is_navigation,
            bool is_download,
            const CefString& request_initiator,
            bool& disable_default_handling) override {
        return new MyResourceRequestHandler();
    }

    IMPLEMENT_REFCOUNTING(MyRequestHandler);
};

class MyResourceRequestHandler : public CefResourceRequestHandler {
public:
    // 请求前拦截
    CefResourceRequestHandler::ReturnValue OnBeforeResourceLoad(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request,
            CefRefPtr<CefCallback> callback) override {

        CefString url = request->GetURL();

        // 阻止某些请求
        if (url.ToString().find("ads.example.com") != std::string::npos) {
            return RV_CANCEL;
        }

        // 修改请求头
        CefRequest::HeaderMap headers;
        request->GetHeaderMap(headers);
        headers.insert(std::make_pair("X-Custom-Header", "value"));
        request->SetHeaderMap(headers);

        return RV_CONTINUE;
    }

    // 自定义资源处理
    CefRefPtr<CefResourceHandler> GetResourceHandler(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefRequest> request) override {

        CefString url = request->GetURL();

        // 拦截特定 URL，返回自定义内容
        if (url == "https://example.com/custom") {
            return new MyResourceHandler();
        }

        return nullptr;  // 使用默认处理
    }

    IMPLEMENT_REFCOUNTING(MyResourceRequestHandler);
};
```

### 6.2 自定义协议

```cpp
// 注册自定义协议（如 myapp://）

class MySchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    CefRefPtr<CefResourceHandler> Create(
            CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            const CefString& scheme_name,
            CefRefPtr<CefRequest> request) override {

        // myapp://resource/page.html
        CefString url = request->GetURL();
        std::string path = ParsePath(url);

        // 从本地文件系统加载
        std::string content = LoadLocalFile(path);
        std::string mimeType = GetMimeType(path);

        return new CefStreamResourceHandler(
            mimeType,
            CefStreamReader::CreateForData(content.data(), content.size())
        );
    }

    IMPLEMENT_REFCOUNTING(MySchemeHandlerFactory);
};

// 注册协议
void OnContextInitialized() {
    CefRegisterSchemeHandlerFactory(
        "myapp",                      // scheme
        "resource",                   // domain（可选）
        new MySchemeHandlerFactory()
    );
}

// 使用
// browser->GetMainFrame()->LoadURL("myapp://resource/index.html");
```

### 6.3 Cookie 管理

```cpp
// 获取 Cookie 管理器
CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);

// 设置 Cookie
CefCookie cookie;
CefString(&cookie.name) = "session_id";
CefString(&cookie.value) = "abc123";
CefString(&cookie.domain) = ".example.com";
CefString(&cookie.path) = "/";
cookie.httponly = true;
cookie.secure = true;

manager->SetCookie("https://example.com", cookie, nullptr);

// 获取 Cookie
class CookieVisitor : public CefCookieVisitor {
public:
    bool Visit(const CefCookie& cookie, int count, int total, bool& deleteCookie) override {
        std::string name = CefString(&cookie.name);
        std::string value = CefString(&cookie.value);
        // 处理 cookie
        return true;  // 继续遍历
    }
    IMPLEMENT_REFCOUNTING(CookieVisitor);
};

manager->VisitUrlCookies("https://example.com", true, new CookieVisitor());

// 删除 Cookie
manager->DeleteCookies("https://example.com", "session_id", nullptr);
```

---

## 7. 安全与沙箱

### 7.1 沙箱机制

```
┌─────────────────────────────────────────────────────────────────────┐
│                    CEF 沙箱架构                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ Browser Process（非沙箱）                                           │
│   - 完全权限                                                        │
│   - 管理其他进程                                                    │
│                                                                     │
│ Renderer Process（沙箱）                                            │
│   ┌─────────────────────────────────────────────────────────────┐   │
│   │ 限制：                                                       │   │
│   │   - 无法直接访问文件系统                                     │   │
│   │   - 无法直接访问网络（通过 Browser 代理）                    │   │
│   │   - 无法创建新进程                                           │   │
│   │   - 有限的系统调用                                           │   │
│   │                                                               │   │
│   │ 好处：                                                        │   │
│   │   - 恶意 JavaScript 无法直接危害系统                         │   │
│   │   - 漏洞利用难度增加                                         │   │
│   └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│ 启用沙箱：                                                          │
│   // Windows 需要额外配置                                           │
│   CefSettings settings;                                             │
│   settings.no_sandbox = false;  // 启用沙箱                         │
│                                                                     │
│   // 需要 cef_sandbox.lib 和正确的 manifest                         │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 7.2 安全策略

```cpp
// 1. 限制 JavaScript 能力
CefBrowserSettings settings;
settings.javascript_access_clipboard = STATE_DISABLED;  // 禁止访问剪贴板
settings.javascript_dom_paste = STATE_DISABLED;         // 禁止粘贴
settings.plugins = STATE_DISABLED;                      // 禁止插件

// 2. 限制导航
class MyRequestHandler : public CefRequestHandler {
    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       CefRefPtr<CefRequest> request,
                       bool user_gesture,
                       bool is_redirect) override {

        CefString url = request->GetURL();

        // 只允许特定域名
        if (!IsAllowedDomain(url)) {
            return true;  // 阻止导航
        }

        return false;  // 允许导航
    }
};

// 3. 内容安全策略 (CSP)
// 通过 HTTP 头或 meta 标签设置
// Content-Security-Policy: default-src 'self'; script-src 'self'

// 4. 禁用开发者工具（生产环境）
CefSettings settings;
settings.remote_debugging_port = 0;  // 禁用远程调试
```

---

## 8. 调试与性能分析

### 8.1 远程调试

```
CEF 支持 Chrome DevTools 协议：

┌─────────────────────────────────────────────────────────────────────┐
│                    启用远程调试                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ // 启动时设置调试端口                                               │
│ CefSettings settings;                                               │
│ settings.remote_debugging_port = 9222;                              │
│                                                                     │
│ // 或通过命令行                                                     │
│ --remote-debugging-port=9222                                        │
│                                                                     │
│ 访问调试页面：                                                      │
│ http://localhost:9222                                               │
│                                                                     │
│ DevTools 功能：                                                     │
│   - Elements：DOM 和样式检查                                        │
│   - Console：JavaScript 控制台                                      │
│   - Sources：断点调试                                               │
│   - Network：网络请求                                               │
│   - Performance：性能分析                                           │
│   - Memory：内存分析                                                │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 8.2 日志系统

```cpp
// CEF 日志配置
CefSettings settings;
settings.log_severity = LOGSEVERITY_INFO;  // 日志级别
CefString(&settings.log_file) = "cef_debug.log";

// 自定义日志处理
class MyLogHandler {
public:
    static void Log(const char* file, int line, const char* msg) {
        // 输出到自己的日志系统
        printf("[CEF] %s:%d %s\n", file, line, msg);
    }
};

// 日志级别：
// LOGSEVERITY_DEFAULT  - 默认
// LOGSEVERITY_VERBOSE  - 详细
// LOGSEVERITY_DEBUG    - 调试
// LOGSEVERITY_INFO     - 信息
// LOGSEVERITY_WARNING  - 警告
// LOGSEVERITY_ERROR    - 错误
// LOGSEVERITY_FATAL    - 致命
// LOGSEVERITY_DISABLE  - 禁用
```

### 8.3 性能优化

```
┌─────────────────────────────────────────────────────────────────────┐
│                    CEF 性能优化策略                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 1. 减少进程数                                                       │
│    CefSettings settings;                                            │
│    // 单进程模式（仅用于调试，不推荐生产）                          │
│    settings.single_process = true;                                  │
│                                                                     │
│ 2. 控制渲染帧率                                                     │
│    CefBrowserSettings browser_settings;                             │
│    browser_settings.windowless_frame_rate = 30;  // OSR 模式        │
│                                                                     │
│ 3. 禁用不需要的功能                                                 │
│    browser_settings.javascript = STATE_DISABLED;  // 禁用 JS        │
│    browser_settings.plugins = STATE_DISABLED;     // 禁用插件       │
│    browser_settings.webgl = STATE_DISABLED;       // 禁用 WebGL     │
│                                                                     │
│ 4. 内存优化                                                         │
│    CefSettings settings;                                            │
│    settings.persist_session_cookies = false;  // 不持久化 Cookie    │
│    // 定期清理缓存                                                  │
│    CefRequestContext::GetGlobalContext()->ClearCache(nullptr);      │
│                                                                     │
│ 5. 预热浏览器                                                       │
│    // 提前创建隐藏的浏览器实例                                      │
│    // 首次加载时直接使用                                            │
│                                                                     │
│ 6. 合理使用缓存                                                     │
│    CefString(&settings.cache_path) = "cache";                       │
│    // 设置合理的缓存大小和策略                                      │
│                                                                     │
│ 7. 减少 IPC 通信                                                    │
│    // 批量发送消息                                                  │
│    // 减少 JavaScript ↔ C++ 频繁调用                                │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 9. 面试深度问题

### 9.1 基础问题

**Q1：CEF 是什么？为什么使用它？**
```
CEF = Chromium Embedded Framework

使用场景：
- 桌面应用需要嵌入 Web 内容
- 需要完整的现代浏览器功能
- 需要 C++ 与 JavaScript 深度交互
- 需要离屏渲染（游戏 UI 等）

优势：
- 完整的 Chromium 功能
- 跨平台支持
- 高度可定制
- 活跃的社区
```

**Q2：CEF 多进程架构的优势和劣势？**
```
优势：
- 安全：渲染进程沙箱化
- 稳定：一个页面崩溃不影响其他
- 性能：多核并行

劣势：
- 内存占用高：每个进程独立内存
- 启动慢：需要创建多个进程
- IPC 开销：跨进程通信有延迟
- 复杂度：开发调试更复杂
```

**Q3：Browser 进程和 Renderer 进程的区别？**
```
Browser 进程：
- 主进程，有完全权限
- 管理窗口、网络、存储
- 协调其他进程
- 只有一个

Renderer 进程：
- 渲染网页内容
- 执行 JavaScript
- 沙箱化，受限权限
- 每个标签页/域名可能有独立进程
```

### 9.2 进阶问题

**Q4：如何实现 JavaScript 调用 C++ 并获取返回值？**
```
两种方式：

1. 同步（在 Renderer 进程）
   - 使用 CefV8Handler
   - 直接返回 CefV8Value
   - 只能访问 Renderer 进程资源

2. 异步（跨进程）
   - JavaScript 调用时传入回调
   - 通过 IPC 发送到 Browser
   - Browser 处理后发送响应
   - Renderer 接收响应，调用回调
```

**Q5：OSR 模式下如何优化性能？**
```
1. 脏区域更新：只更新变化部分
2. 共享纹理：使用 GPU 共享，避免 CPU 拷贝
3. 合理帧率：根据场景调整
4. 异步更新：OnPaint 快速返回，异步处理纹理
5. 减少重绘：优化 CSS，避免频繁布局
```

**Q6：如何拦截和修改网络请求？**
```
使用 CefRequestHandler + CefResourceRequestHandler：

1. OnBeforeResourceLoad：请求发送前
   - 可以取消请求
   - 可以修改请求头

2. GetResourceHandler：返回自定义处理器
   - 可以返回本地内容
   - 实现自定义协议

3. OnResourceResponse：响应返回后
   - 检查响应状态
   - 可以重定向
```

### 9.3 高级问题

**Q7：CEF 中如何处理线程安全？**
```
规则：
- CEF 对象通常只能在创建线程访问
- 使用 CefPostTask 跨线程执行
- CefRefPtr 提供线程安全引用计数

示例：
// 在 IO 线程执行
CefPostTask(TID_IO, base::BindOnce(&DoNetworkWork));

// 检查线程
DCHECK(CefCurrentlyOn(TID_UI));
```

**Q8：如何处理大量浏览器实例的内存问题？**
```
优化策略：
1. 共享 Renderer 进程
   - 同源页面共享进程

2. 延迟创建
   - 需要时才创建浏览器

3. 定期清理
   - 清理缓存
   - 销毁不使用的浏览器

4. 资源限制
   - 限制最大进程数
   - 限制内存使用

5. 使用单进程模式（不推荐）
   - 牺牲稳定性换内存
```

**Q9：CEF 与 Electron 的技术区别？**
```
CEF：
- C/C++ API
- 更底层，更可控
- 更小的运行时
- 适合原生应用集成
- 需要更多开发工作

Electron：
- JavaScript/Node.js API
- 更高层，开发快
- 包含 Node.js 运行时
- 适合独立 Web 应用
- 社区生态丰富

技术栈：
CEF：Chromium + C++ API
Electron：Chromium + Node.js + 高层 API
```

**Q10：如何实现 CEF 与 OpenGL/DirectX 集成？**
```
步骤：
1. 使用 OSR 模式
   - SetAsWindowless

2. 实现 OnPaint
   - 获取像素数据
   - 更新 GPU 纹理

3. 渲染循环
   - 在游戏引擎渲染循环中绘制纹理

4. 输入转发
   - 将鼠标键盘事件转发给 CEF

5. 高级优化
   - 使用 OnAcceleratedPaint
   - 共享 GPU 纹理
   - 减少 CPU 拷贝
```

---

## 10. 常见问题与解决方案

```
┌─────────────────────────────────────────────────────────────────────┐
│                    常见问题排查                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│ 问题：白屏                                                          │
│ 原因：                                                              │
│   - GPU 加速问题                                                    │
│   - 资源加载失败                                                    │
│   - JavaScript 错误                                                 │
│ 解决：                                                              │
│   - 尝试禁用 GPU：--disable-gpu                                     │
│   - 检查控制台错误                                                  │
│   - 检查网络请求                                                    │
│                                                                     │
│ 问题：内存泄漏                                                      │
│ 原因：                                                              │
│   - CefRefPtr 未正确释放                                            │
│   - JavaScript 引用未清理                                           │
│   - 浏览器未正确关闭                                                │
│ 解决：                                                              │
│   - 使用内存分析工具                                                │
│   - 确保 CloseBrowser 被调用                                        │
│   - 检查循环引用                                                    │
│                                                                     │
│ 问题：崩溃                                                          │
│ 原因：                                                              │
│   - 线程安全问题                                                    │
│   - 空指针访问                                                      │
│   - 生命周期管理错误                                                │
│ 解决：                                                              │
│   - 使用崩溃报告（CefCrashReporter）                                │
│   - 检查线程调用                                                    │
│   - 启用日志                                                        │
│                                                                     │
│ 问题：启动慢                                                        │
│ 原因：                                                              │
│   - 多进程初始化                                                    │
│   - 资源加载                                                        │
│ 解决：                                                              │
│   - 预热浏览器实例                                                  │
│   - 延迟加载非必要资源                                              │
│   - 使用持久化缓存                                                  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```
