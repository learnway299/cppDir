  # 04 - CEF 深入剖析

## 概述

CEF (Chromium Embedded Framework) 是最广泛使用的 Chromium 嵌入式框架。本章深入剖析 CEF 的架构设计、核心组件、Handler 体系及实战技巧。

---

## 1. CEF 架构总览

### 1.1 层次结构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          CEF 架构层次                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        应用程序层                                    │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                 │   │
│  │  │  业务逻辑   │  │   UI 框架   │  │  原生功能   │                 │   │
│  │  │  (C++/.NET) │  │  (Qt/WPF)   │  │  (文件/网络)│                 │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘                 │   │
│  └───────────────────────────┬─────────────────────────────────────────┘   │
│                              │                                              │
│  ┌───────────────────────────▼─────────────────────────────────────────┐   │
│  │                         CEF API 层                                   │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌───────────┐  │   │
│  │  │ CefBrowser  │  │  CefFrame   │  │  CefClient  │  │ CefApp    │  │   │
│  │  │ 浏览器实例  │  │  页面框架   │  │  事件处理   │  │ 应用入口  │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └───────────┘  │   │
│  │                                                                     │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌───────────┐  │   │
│  │  │ Handlers    │  │  Request    │  │  Response   │  │ Callbacks │  │   │
│  │  │ 各类处理器  │  │  请求对象   │  │  响应对象   │  │ 异步回调  │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └───────────┘  │   │
│  └───────────────────────────┬─────────────────────────────────────────┘   │
│                              │                                              │
│  ┌───────────────────────────▼─────────────────────────────────────────┐   │
│  │                       CEF 内部实现层                                 │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                    libcef (CEF 核心库)                       │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └───────────────────────────┬─────────────────────────────────────────┘   │
│                              │                                              │
│  ┌───────────────────────────▼─────────────────────────────────────────┐   │
│  │                       Chromium 内核层                                │   │
│  │  ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌───────────┐        │   │
│  │  │   Blink   │  │    V8     │  │   Skia    │  │   Net     │        │   │
│  │  │  渲染引擎 │  │  JS 引擎  │  │  图形库   │  │  网络栈   │        │   │
│  │  └───────────┘  └───────────┘  └───────────┘  └───────────┘        │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 进程模型

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          CEF 进程模型                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  主可执行文件 (your_app.exe)                                                │
│  │                                                                          │
│  ├── Browser Process (主进程)                                               │
│  │   ├── UI Thread: 处理用户界面                                            │
│  │   ├── IO Thread: 处理 IPC 消息                                           │
│  │   └── 其他线程: 网络、文件等                                             │
│  │                                                                          │
│  ├── Renderer Process (渲染进程) x N                                        │
│  │   ├── Main Thread: Blink 渲染                                            │
│  │   └── Worker Threads: Web Workers                                        │
│  │                                                                          │
│  ├── GPU Process (GPU 进程)                                                 │
│  │   └── 处理 3D 渲染、视频解码                                             │
│  │                                                                          │
│  └── Utility Process (辅助进程)                                             │
│      └── 音频、PDF 解析等                                                   │
│                                                                             │
│  进程启动方式:                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  CEF 默认使用同一个可执行文件启动所有进程                            │   │
│  │                                                                     │   │
│  │  your_app.exe                  → Browser Process                    │   │
│  │  your_app.exe --type=renderer  → Renderer Process                   │   │
│  │  your_app.exe --type=gpu       → GPU Process                        │   │
│  │                                                                     │   │
│  │  判断进程类型:                                                      │   │
│  │  int exit_code = CefExecuteProcess(main_args, app, nullptr);        │   │
│  │  if (exit_code >= 0) {                                              │   │
│  │      // 子进程，直接退出                                            │   │
│  │      return exit_code;                                              │   │
│  │  }                                                                  │   │
│  │  // exit_code < 0 表示这是 Browser 进程                             │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. 核心类详解

### 2.1 CefApp - 应用程序入口

```cpp
// CefApp 是应用程序入口，用于定制各进程行为

class MyApp : public CefApp,
              public CefBrowserProcessHandler,
              public CefRenderProcessHandler {
public:
    // ==================== CefApp 接口 ====================

    // 返回 Browser 进程处理器
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // 返回 Renderer 进程处理器
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
        return this;
    }

    // 自定义命令行参数
    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override {

        // 启用硬件加速
        command_line->AppendSwitch("enable-gpu");

        // 禁用同源策略 (仅开发用)
        // command_line->AppendSwitch("disable-web-security");

        // 设置代理
        // command_line->AppendSwitchWithValue("proxy-server", "127.0.0.1:8080");
    }

    // 注册自定义 Scheme
    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override {
        // 注册自定义协议 app://
        registrar->AddCustomScheme(
            "app",
            CEF_SCHEME_OPTION_STANDARD |
            CEF_SCHEME_OPTION_SECURE |
            CEF_SCHEME_OPTION_CORS_ENABLED
        );
    }

    // ==================== BrowserProcessHandler ====================

    void OnContextInitialized() override {
        // CEF 上下文初始化完成，可以创建浏览器了
        LOG(INFO) << "CEF context initialized";

        // 注册自定义 Scheme 处理器
        CefRegisterSchemeHandlerFactory(
            "app", "local",
            new MySchemeHandlerFactory()
        );
    }

    // ==================== RenderProcessHandler ====================

    void OnWebKitInitialized() override {
        // WebKit 初始化完成，可以注册 V8 扩展
        std::string extensionCode = R"(
            var myExtension;
            if (!myExtension) {
                myExtension = {};
            }
            (function() {
                myExtension.hello = function() {
                    native function hello();
                    return hello();
                };
            })();
        )";

        CefRegisterExtension("v8/myExtension", extensionCode, new MyV8Handler());
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override {
        // 每个 Frame 的 JS 上下文创建时调用
        // 可以注入全局对象
        CefRefPtr<CefV8Value> global = context->GetGlobal();

        CefRefPtr<CefV8Value> nativeObj = CefV8Value::CreateObject(nullptr, nullptr);
        // 添加方法和属性...

        global->SetValue("native", nativeObj, V8_PROPERTY_ATTRIBUTE_NONE);
    }

private:
    IMPLEMENT_REFCOUNTING(MyApp);
};
```

### 2.2 CefClient - 事件处理中心

```cpp
// CefClient 是浏览器事件的总入口，返回各种 Handler

class MyClient : public CefClient,
                 public CefLifeSpanHandler,
                 public CefLoadHandler,
                 public CefDisplayHandler,
                 public CefRequestHandler,
                 public CefKeyboardHandler,
                 public CefContextMenuHandler,
                 public CefDownloadHandler,
                 public CefDragHandler,
                 public CefFocusHandler {
public:
    // ==================== Handler 分发 ====================

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override { return this; }
    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override { return this; }
    CefRefPtr<CefDragHandler> GetDragHandler() override { return this; }
    CefRefPtr<CefFocusHandler> GetFocusHandler() override { return this; }

    // ==================== LifeSpanHandler ====================

    // 新窗口请求 (window.open, target="_blank")
    bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       const CefString& target_url,
                       const CefString& target_frame_name,
                       CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                       bool user_gesture,
                       const CefPopupFeatures& popupFeatures,
                       CefWindowInfo& windowInfo,
                       CefRefPtr<CefClient>& client,
                       CefBrowserSettings& settings,
                       CefRefPtr<CefDictionaryValue>& extra_info,
                       bool* no_javascript_access) override {
        // 返回 true 阻止弹窗，可以在当前窗口打开
        browser->GetMainFrame()->LoadURL(target_url);
        return true;  // 阻止弹窗
    }

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        browser_list_.push_back(browser);
    }

    bool DoClose(CefRefPtr<CefBrowser> browser) override {
        // 返回 false 允许关闭
        return false;
    }

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        browser_list_.remove(browser);
        if (browser_list_.empty()) {
            CefQuitMessageLoop();
        }
    }

    // ==================== LoadHandler ====================

    void OnLoadStart(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     TransitionType transition_type) override {
        if (frame->IsMain()) {
            // 主框架开始加载
            ShowLoadingIndicator();
        }
    }

    void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   int httpStatusCode) override {
        if (frame->IsMain()) {
            // 主框架加载完成
            HideLoadingIndicator();

            // 注入 JS
            frame->ExecuteJavaScript(
                "console.log('Page loaded');",
                frame->GetURL(), 0);
        }
    }

    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     ErrorCode errorCode,
                     const CefString& errorText,
                     const CefString& failedUrl) override {
        // 显示自定义错误页
        if (errorCode == ERR_ABORTED) return;  // 忽略用户取消

        std::string html = "<html><body><h1>Error</h1><p>" +
                           errorText.ToString() + "</p></body></html>";
        frame->LoadURL("data:text/html," + html);
    }

    // ==================== DisplayHandler ====================

    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                       const CefString& title) override {
        // 更新窗口标题
        SetWindowTitle(title.ToString());
    }

    void OnAddressChange(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         const CefString& url) override {
        if (frame->IsMain()) {
            // 更新地址栏
            UpdateAddressBar(url.ToString());
        }
    }

    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                          cef_log_severity_t level,
                          const CefString& message,
                          const CefString& source,
                          int line) override {
        // 捕获 console.log 输出
        LOG(INFO) << "[JS] " << source.ToString() << ":" << line
                  << " " << message.ToString();
        return false;  // 继续默认处理
    }

    // ==================== RequestHandler ====================

    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefRequest> request,
                        bool user_gesture,
                        bool is_redirect) override {
        std::string url = request->GetURL().ToString();

        // 拦截特定协议
        if (url.find("myapp://") == 0) {
            HandleCustomProtocol(url);
            return true;  // 阻止导航
        }

        return false;  // 允许导航
    }

    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_navigation,
        bool is_download,
        const CefString& request_initiator,
        bool& disable_default_handling) override {
        // 返回资源请求处理器
        return new MyResourceRequestHandler();
    }

private:
    std::list<CefRefPtr<CefBrowser>> browser_list_;

    IMPLEMENT_REFCOUNTING(MyClient);
};
```

### 2.3 CefBrowser 和 CefFrame

```cpp
// CefBrowser 代表一个浏览器实例
// CefFrame 代表页面中的一个框架 (主框架或 iframe)

class BrowserController {
public:
    void NavigateTo(const std::string& url) {
        browser_->GetMainFrame()->LoadURL(url);
    }

    void GoBack() {
        if (browser_->CanGoBack()) {
            browser_->GoBack();
        }
    }

    void GoForward() {
        if (browser_->CanGoForward()) {
            browser_->GoForward();
        }
    }

    void Reload(bool ignoreCache = false) {
        if (ignoreCache) {
            browser_->ReloadIgnoreCache();
        } else {
            browser_->Reload();
        }
    }

    void StopLoad() {
        browser_->StopLoad();
    }

    void ExecuteJS(const std::string& code) {
        browser_->GetMainFrame()->ExecuteJavaScript(
            code,
            browser_->GetMainFrame()->GetURL(),
            0  // 行号
        );
    }

    void GetPageSource(std::function<void(const std::string&)> callback) {
        // 使用 Visitor 模式获取源码
        class SourceVisitor : public CefStringVisitor {
        public:
            explicit SourceVisitor(std::function<void(const std::string&)> cb)
                : callback_(std::move(cb)) {}

            void Visit(const CefString& string) override {
                callback_(string.ToString());
            }

        private:
            std::function<void(const std::string&)> callback_;
            IMPLEMENT_REFCOUNTING(SourceVisitor);
        };

        browser_->GetMainFrame()->GetSource(new SourceVisitor(callback));
    }

    void PrintPage() {
        browser_->GetHost()->Print();
    }

    void ZoomIn() {
        double level = browser_->GetHost()->GetZoomLevel();
        browser_->GetHost()->SetZoomLevel(level + 0.5);
    }

    void ZoomOut() {
        double level = browser_->GetHost()->GetZoomLevel();
        browser_->GetHost()->SetZoomLevel(level - 0.5);
    }

    void ShowDevTools() {
        CefWindowInfo windowInfo;
        windowInfo.SetAsPopup(nullptr, "DevTools");

        browser_->GetHost()->ShowDevTools(
            windowInfo,
            new DevToolsClient(),
            CefBrowserSettings(),
            CefPoint()
        );
    }

    void CloseDevTools() {
        browser_->GetHost()->CloseDevTools();
    }

    // 获取所有框架
    void EnumerateFrames() {
        std::vector<int64_t> frameIds;
        browser_->GetFrameIdentifiers(frameIds);

        for (auto id : frameIds) {
            CefRefPtr<CefFrame> frame = browser_->GetFrameByIdentifier(id);
            LOG(INFO) << "Frame: " << frame->GetName().ToString()
                      << " URL: " << frame->GetURL().ToString()
                      << " IsMain: " << frame->IsMain();
        }
    }

private:
    CefRefPtr<CefBrowser> browser_;
};
```

---

## 3. Handler 体系详解

### 3.1 Handler 完整列表

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          CEF Handler 体系                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  CefClient (总入口)                                                         │
│  ├── CefLifeSpanHandler      - 生命周期 (创建/关闭/弹窗)                    │
│  ├── CefLoadHandler          - 加载事件 (开始/完成/错误)                    │
│  ├── CefDisplayHandler       - 显示事件 (标题/地址/控制台)                  │
│  ├── CefRequestHandler       - 请求拦截 (导航/资源/证书)                    │
│  ├── CefResourceRequestHandler - 资源请求详细控制                           │
│  ├── CefKeyboardHandler      - 键盘事件 (按键拦截)                          │
│  ├── CefContextMenuHandler   - 右键菜单                                     │
│  ├── CefDialogHandler        - 对话框 (文件选择/JS alert)                   │
│  ├── CefDownloadHandler      - 下载管理                                     │
│  ├── CefDragHandler          - 拖放处理                                     │
│  ├── CefFocusHandler         - 焦点管理                                     │
│  ├── CefJSDialogHandler      - JS 对话框 (alert/confirm/prompt)             │
│  ├── CefRenderHandler        - 离屏渲染 (OSR)                               │
│  ├── CefFindHandler          - 页面查找                                     │
│  ├── CefPrintHandler         - 打印处理                                     │
│  ├── CefAudioHandler         - 音频处理                                     │
│  └── CefPermissionHandler    - 权限请求 (摄像头/麦克风等)                   │
│                                                                             │
│  CefApp                                                                     │
│  ├── CefBrowserProcessHandler - Browser 进程初始化                          │
│  ├── CefRenderProcessHandler  - Renderer 进程事件                           │
│  └── CefResourceBundleHandler - 资源包处理                                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 资源请求拦截

```cpp
// 完整的资源请求拦截示例

class MyResourceRequestHandler : public CefResourceRequestHandler {
public:
    // 请求发送前拦截
    CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) override {

        std::string url = request->GetURL().ToString();

        // 拦截本地资源请求
        if (url.find("app://") == 0) {
            return new LocalResourceHandler(url);
        }

        // 返回 nullptr 使用默认网络请求
        return nullptr;
    }

    // 修改请求
    CefResourceRequestHandler::ReturnValue OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override {

        // 添加自定义请求头
        CefRequest::HeaderMap headers;
        request->GetHeaderMap(headers);
        headers.insert(std::make_pair("X-Custom-Header", "MyApp/1.0"));
        request->SetHeaderMap(headers);

        return RV_CONTINUE;
    }

    // 过滤响应
    CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefResponse> response) override {

        // 可以返回一个 Filter 来修改响应内容
        return nullptr;
    }

    IMPLEMENT_REFCOUNTING(MyResourceRequestHandler);
};

// 本地资源处理器
class LocalResourceHandler : public CefResourceHandler {
public:
    explicit LocalResourceHandler(const std::string& url) : url_(url) {}

    bool Open(CefRefPtr<CefRequest> request,
              bool& handle_request,
              CefRefPtr<CefCallback> callback) override {
        // 解析 URL 获取本地文件路径
        // app://local/index.html → ./resources/index.html
        std::string path = MapUrlToLocalPath(url_);

        // 读取文件内容
        if (ReadFile(path, data_)) {
            mime_type_ = GetMimeType(path);
            handle_request = true;
            return true;
        }

        handle_request = false;
        return false;
    }

    void GetResponseHeaders(CefRefPtr<CefResponse> response,
                            int64_t& response_length,
                            CefString& redirectUrl) override {
        response->SetMimeType(mime_type_);
        response->SetStatus(200);
        response_length = data_.size();
    }

    bool Read(void* data_out,
              int bytes_to_read,
              int& bytes_read,
              CefRefPtr<CefResourceReadCallback> callback) override {
        if (offset_ >= data_.size()) {
            bytes_read = 0;
            return false;
        }

        int remaining = data_.size() - offset_;
        bytes_read = std::min(bytes_to_read, remaining);
        memcpy(data_out, data_.data() + offset_, bytes_read);
        offset_ += bytes_read;

        return true;
    }

    void Cancel() override {}

private:
    std::string url_;
    std::string mime_type_;
    std::vector<uint8_t> data_;
    size_t offset_ = 0;

    IMPLEMENT_REFCOUNTING(LocalResourceHandler);
};
```

### 3.3 自定义 Scheme

```cpp
// 注册自定义协议处理器

class MySchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request) override {

        // 处理 myapp:// 协议
        return new MySchemeHandler(request->GetURL());
    }

    IMPLEMENT_REFCOUNTING(MySchemeHandlerFactory);
};

// 在 OnContextInitialized 中注册
void OnContextInitialized() override {
    CefRegisterSchemeHandlerFactory(
        "myapp",      // scheme
        "resources",  // domain (可选)
        new MySchemeHandlerFactory()
    );
}

// 使用:
// browser->GetMainFrame()->LoadURL("myapp://resources/index.html");
```

---

## 4. Cookie 和存储管理

### 4.1 Cookie 操作

```cpp
class CookieManager {
public:
    // 设置 Cookie
    void SetCookie(const std::string& url,
                   const std::string& name,
                   const std::string& value) {
        CefRefPtr<CefCookieManager> manager =
            CefCookieManager::GetGlobalManager(nullptr);

        CefCookie cookie;
        CefString(&cookie.name).FromString(name);
        CefString(&cookie.value).FromString(value);
        CefString(&cookie.domain).FromString(GetDomain(url));
        CefString(&cookie.path).FromString("/");
        cookie.secure = false;
        cookie.httponly = false;

        // 设置过期时间 (7 天后)
        cookie.has_expires = true;
        auto now = std::chrono::system_clock::now();
        auto expires = now + std::chrono::hours(24 * 7);
        cookie.expires = CefTimeFrom(expires);

        manager->SetCookie(url, cookie, nullptr);
    }

    // 获取 Cookie
    void GetCookies(const std::string& url,
                    std::function<void(const std::vector<CefCookie>&)> callback) {

        class CookieVisitor : public CefCookieVisitor {
        public:
            explicit CookieVisitor(
                std::function<void(const std::vector<CefCookie>&)> cb)
                : callback_(std::move(cb)) {}

            bool Visit(const CefCookie& cookie,
                       int count, int total,
                       bool& deleteCookie) override {
                cookies_.push_back(cookie);
                if (count == total - 1) {
                    callback_(cookies_);
                }
                return true;
            }

        private:
            std::function<void(const std::vector<CefCookie>&)> callback_;
            std::vector<CefCookie> cookies_;
            IMPLEMENT_REFCOUNTING(CookieVisitor);
        };

        CefRefPtr<CefCookieManager> manager =
            CefCookieManager::GetGlobalManager(nullptr);
        manager->VisitUrlCookies(url, true, new CookieVisitor(callback));
    }

    // 删除 Cookie
    void DeleteCookies(const std::string& url, const std::string& name) {
        CefRefPtr<CefCookieManager> manager =
            CefCookieManager::GetGlobalManager(nullptr);
        manager->DeleteCookies(url, name, nullptr);
    }

    // 清除所有 Cookie
    void ClearAllCookies() {
        CefRefPtr<CefCookieManager> manager =
            CefCookieManager::GetGlobalManager(nullptr);
        manager->DeleteCookies("", "", nullptr);
    }
};
```

### 4.2 LocalStorage 操作

```cpp
// 通过执行 JS 操作 LocalStorage

void SetLocalStorage(CefRefPtr<CefFrame> frame,
                     const std::string& key,
                     const std::string& value) {
    std::string js = "localStorage.setItem('" + key + "', '" + value + "');";
    frame->ExecuteJavaScript(js, frame->GetURL(), 0);
}

void GetLocalStorage(CefRefPtr<CefFrame> frame,
                     const std::string& key,
                     std::function<void(const std::string&)> callback) {
    // 需要通过 IPC 获取返回值
    // 使用 CefMessageRouter 或 V8 绑定
}

void ClearLocalStorage(CefRefPtr<CefFrame> frame) {
    frame->ExecuteJavaScript("localStorage.clear();", frame->GetURL(), 0);
}
```

---

## 5. 离屏渲染 (OSR)

### 5.1 OSR 配置

```cpp
// 离屏渲染模式配置

void CreateOSRBrowser() {
    CefWindowInfo window_info;

    // 设置为离屏渲染模式
    window_info.SetAsWindowless(0);  // 0 表示无父窗口

    CefBrowserSettings settings;
    settings.windowless_frame_rate = 60;  // 帧率

    CefBrowserHost::CreateBrowser(
        window_info,
        new OSRClient(),
        "https://example.com",
        settings,
        nullptr,
        nullptr
    );
}
```

### 5.2 OSR 渲染处理

```cpp
class OSRClient : public CefClient, public CefRenderHandler {
public:
    CefRefPtr<CefRenderHandler> GetRenderHandler() override {
        return this;
    }

    // 获取视图大小
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
        rect = CefRect(0, 0, width_, height_);
    }

    // 接收渲染帧
    void OnPaint(CefRefPtr<CefBrowser> browser,
                 PaintElementType type,
                 const RectList& dirtyRects,
                 const void* buffer,
                 int width, int height) override {

        if (type == PET_VIEW) {
            // buffer 是 BGRA 格式的像素数据
            // 上传到 GPU 纹理或保存为图片

            // 示例：复制到 OpenGL 纹理
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         width, height, 0,
                         GL_BGRA, GL_UNSIGNED_BYTE, buffer);
        }
    }

    // GPU 加速渲染 (共享纹理)
    void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                            PaintElementType type,
                            const RectList& dirtyRects,
                            const CefAcceleratedPaintInfo& info) override {
        // info.shared_texture_handle 包含 GPU 纹理句柄
        // 可以直接在 GPU 间共享
    }

    // 处理鼠标输入
    void SendMouseClick(int x, int y, cef_mouse_button_type_t button) {
        CefMouseEvent event;
        event.x = x;
        event.y = y;
        event.modifiers = 0;

        browser_->GetHost()->SendMouseClickEvent(event, button, false, 1);
        browser_->GetHost()->SendMouseClickEvent(event, button, true, 1);
    }

    // 处理键盘输入
    void SendKeyEvent(int key, bool keyDown) {
        CefKeyEvent event;
        event.windows_key_code = key;
        event.type = keyDown ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;

        browser_->GetHost()->SendKeyEvent(event);
    }

    // 处理滚轮
    void SendMouseWheel(int x, int y, int deltaX, int deltaY) {
        CefMouseEvent event;
        event.x = x;
        event.y = y;
        event.modifiers = 0;

        browser_->GetHost()->SendMouseWheelEvent(event, deltaX, deltaY);
    }

    // 调整大小
    void Resize(int width, int height) {
        width_ = width;
        height_ = height;
        browser_->GetHost()->WasResized();
    }

private:
    CefRefPtr<CefBrowser> browser_;
    int width_ = 1280;
    int height_ = 720;
    GLuint texture_id_;

    IMPLEMENT_REFCOUNTING(OSRClient);
};
```

---

## 6. 常见问题与最佳实践

### 6.1 线程安全

```cpp
// CEF 线程模型
/*
  UI Thread (TID_UI): 主线程，处理 UI 事件
  IO Thread (TID_IO): 处理 IPC 消息
  File Thread (TID_FILE): 文件操作
  Renderer Thread: V8/Blink 执行

  规则：
  1. CefBrowser 操作必须在 UI 线程
  2. CefV8 操作必须在 Renderer 主线程
  3. 跨线程使用 CefPostTask
*/

// 示例：从工作线程更新 UI
void WorkerThread() {
    // 在 IO 线程做一些工作
    std::string result = DoHeavyWork();

    // 切换到 UI 线程更新浏览器
    CefPostTask(TID_UI, base::BindOnce([](const std::string& result) {
        // 现在在 UI 线程
        browser_->GetMainFrame()->ExecuteJavaScript(
            "updateResult('" + result + "')",
            browser_->GetMainFrame()->GetURL(), 0);
    }, result));
}

// 检查当前线程
void SomeFunction() {
    if (!CefCurrentlyOn(TID_UI)) {
        // 不在 UI 线程，转发
        CefPostTask(TID_UI, base::BindOnce(&SomeFunction));
        return;
    }

    // 在 UI 线程，安全操作
    browser_->Reload();
}
```

### 6.2 内存管理

```cpp
// CEF 使用引用计数管理内存

// CefRefPtr: 智能指针，自动管理引用计数
CefRefPtr<CefBrowser> browser;  // 正确

// 错误：不要使用裸指针
// CefBrowser* browser = ...;  // 危险！

// IMPLEMENT_REFCOUNTING 宏
class MyHandler : public CefSomeHandler {
    // 必须在类末尾添加
    IMPLEMENT_REFCOUNTING(MyHandler);
};

// AddRef/Release 规则
// - CefRefPtr 自动处理
// - 如果必须用裸指针，确保正确调用 AddRef/Release
```

### 6.3 调试技巧

```cpp
// 启用日志
CefSettings settings;
settings.log_severity = LOGSEVERITY_VERBOSE;
CefString(&settings.log_file).FromASCII("cef_debug.log");

// 远程调试
CefSettings settings;
settings.remote_debugging_port = 9222;
// 访问 http://localhost:9222 使用 DevTools

// 检查进程类型
void LogProcessType() {
    CefRefPtr<CefCommandLine> cmd = CefCommandLine::GetGlobalCommandLine();
    if (cmd->HasSwitch("type")) {
        std::string type = cmd->GetSwitchValue("type").ToString();
        LOG(INFO) << "Process type: " << type;
    } else {
        LOG(INFO) << "Process type: browser";
    }
}
```

---

## 7. 面试常见问题

### Q1: CEF 多进程架构的优缺点？

**答案：**

**优点：**
- 稳定性：单个页面崩溃不影响其他页面
- 安全性：沙箱隔离，渲染进程权限受限
- 性能：利用多核并行处理

**缺点：**
- 内存占用：每个进程约 50-100MB 基础开销
- 启动时间：进程创建需要时间
- 通信开销：跨进程 IPC 有延迟

### Q2: 如何实现 JS 调用 C++ 函数并获取返回值？

**答案：**

1. **同步方式（V8 绑定）**：在 Renderer 进程内完成
```cpp
void OnContextCreated(...) {
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("getData", handler);
    global->SetValue("getData", func, V8_PROPERTY_ATTRIBUTE_NONE);
}
// JS: const result = getData();
```

2. **异步方式（CefMessageRouter）**：跨进程通信
```javascript
window.cefQuery({
    request: 'getData',
    onSuccess: function(response) { /* 处理结果 */ }
});
```

### Q3: CEF 离屏渲染适用于什么场景？

**答案：**
- 游戏内嵌 UI（Unity/Unreal 中嵌入 Web 界面）
- 3D 场景中的网页纹理
- 无窗口渲染（截图服务、自动化测试）
- 自定义窗口系统

### Q4: 如何处理 CEF 的内存泄漏问题？

**答案：**
1. 使用 `CefRefPtr` 而非裸指针
2. 避免循环引用（A 持有 B，B 持有 A）
3. 确保所有 Handler 类实现 `IMPLEMENT_REFCOUNTING`
4. 正确释放 Browser 实例（调用 CloseBrowser）
5. 关闭时清理 MessageRouter
6. 使用 Chromium 的内存工具检测泄漏

### Q5: 如何优化 CEF 应用的启动速度？

**答案：**
1. **预创建 Browser**：应用启动时预创建隐藏的 Browser
2. **资源预加载**：提前加载常用资源到缓存
3. **减少子进程**：合理配置进程模型（单进程模式慎用）
4. **延迟加载**：非必要模块延迟初始化
5. **使用 OSR**：某些场景 OSR 初始化更快
6. **精简配置**：禁用不需要的功能（如 GPU、插件）
