# 06 - WebView2 (Microsoft Edge WebView)

## 概述

WebView2 是微软基于 Edge (Chromium) 的 Web 嵌入控件，适用于 Windows 原生应用。它使用系统预装的 Edge 运行时，无需捆绑浏览器内核，显著减小应用体积。

---

## 1. WebView2 架构

### 1.1 运行时模型

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        WebView2 运行时模型                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                      应用程序 (.exe)                                │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                   WebView2 控件                              │   │   │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │   │   │
│  │  │  │ CoreWebView2│  │ Controller  │  │    Event Handlers   │  │   │   │
│  │  │  │   (接口)    │  │  (窗口管理) │  │     (事件处理)      │  │   │   │
│  │  │  └─────────────┘  └─────────────┘  └─────────────────────┘  │   │   │
│  │  └──────────────────────────┬──────────────────────────────────┘   │   │
│  └─────────────────────────────┼───────────────────────────────────────┘   │
│                                │                                            │
│                                │ COM 接口                                   │
│                                ▼                                            │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │              WebView2 Runtime / Edge 浏览器                         │   │
│  │  ┌───────────────────────────────────────────────────────────────┐ │   │
│  │  │                    Edge Chromium 内核                         │ │   │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │ │   │
│  │  │  │   Blink     │  │     V8      │  │    GPU      │           │ │   │
│  │  │  │   渲染引擎  │  │   JS 引擎   │  │   进程      │           │ │   │
│  │  │  └─────────────┘  └─────────────┘  └─────────────┘           │ │   │
│  │  └───────────────────────────────────────────────────────────────┘ │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  运行时分发选项:                                                            │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ 1. Evergreen (推荐)                                                 │   │
│  │    - 使用系统预装的 WebView2 Runtime                                │   │
│  │    - 自动更新，始终最新                                             │   │
│  │    - 无额外包体积                                                   │   │
│  │                                                                     │   │
│  │ 2. Fixed Version                                                    │   │
│  │    - 捆绑特定版本的 WebView2                                        │   │
│  │    - 需要手动更新                                                   │   │
│  │    - 增加约 100MB 包体积                                            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 WebView2 vs CEF 对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     WebView2 vs CEF 对比                                    │
├─────────────────────┬─────────────────────┬─────────────────────────────────┤
│        特性         │      WebView2       │              CEF                │
├─────────────────────┼─────────────────────┼─────────────────────────────────┤
│ 平台支持            │ Windows 10+ 仅      │ Windows/macOS/Linux            │
│ 内核版本            │ 跟随 Edge 更新      │ 需手动升级                      │
│ 包体积              │ 0 (使用系统 Runtime) │ ~100MB+                        │
│ 接口语言            │ C++/C#/.NET/Win32   │ C/C++                          │
│ 学习曲线            │ 较平缓 (COM)        │ 较陡峭                          │
│ 多进程              │ 共享 Edge 进程      │ 独立进程                        │
│ 版本控制            │ 自动更新 (Evergreen) │ 完全控制                       │
│ 离线支持            │ 需要 Fixed Version  │ 完全离线                        │
└─────────────────────┴─────────────────────┴─────────────────────────────────┘
```

---

## 2. C++ 集成 (Win32)

### 2.1 初始化 WebView2

```cpp
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

using namespace Microsoft::WRL;

class WebView2Window {
public:
    HWND hwnd_ = nullptr;
    wil::com_ptr<ICoreWebView2Controller> controller_;
    wil::com_ptr<ICoreWebView2> webview_;

    // 初始化 WebView2
    HRESULT Initialize(HWND hwnd) {
        hwnd_ = hwnd;

        // 创建环境选项
        auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
        options->put_AdditionalBrowserArguments(L"--disable-features=msSmartScreenProtection");

        // 创建 WebView2 环境
        return CreateCoreWebView2EnvironmentWithOptions(
            nullptr,  // 使用已安装的 WebView2 Runtime
            nullptr,  // 用户数据目录 (nullptr 使用默认)
            options.Get(),
            Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                this, &WebView2Window::OnEnvironmentCreated).Get()
        );
    }

    // 环境创建完成回调
    HRESULT OnEnvironmentCreated(HRESULT result, ICoreWebView2Environment* env) {
        if (FAILED(result)) return result;

        // 创建 WebView2 控制器
        return env->CreateCoreWebView2Controller(
            hwnd_,
            Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                this, &WebView2Window::OnControllerCreated).Get()
        );
    }

    // 控制器创建完成回调
    HRESULT OnControllerCreated(HRESULT result, ICoreWebView2Controller* controller) {
        if (FAILED(result)) return result;

        controller_ = controller;
        controller_->get_CoreWebView2(&webview_);

        // 设置初始大小
        RECT bounds;
        GetClientRect(hwnd_, &bounds);
        controller_->put_Bounds(bounds);

        // 配置设置
        ConfigureSettings();

        // 注册事件处理
        RegisterEventHandlers();

        // 加载初始页面
        webview_->Navigate(L"https://example.com");

        return S_OK;
    }

    void ConfigureSettings() {
        wil::com_ptr<ICoreWebView2Settings> settings;
        webview_->get_Settings(&settings);

        settings->put_IsScriptEnabled(TRUE);
        settings->put_AreDefaultScriptDialogsEnabled(TRUE);
        settings->put_IsWebMessageEnabled(TRUE);
        settings->put_AreDevToolsEnabled(TRUE);

        // WebView2 设置3 (需要 QI)
        wil::com_ptr<ICoreWebView2Settings3> settings3;
        settings->QueryInterface(IID_PPV_ARGS(&settings3));
        if (settings3) {
            settings3->put_AreBrowserAcceleratorKeysEnabled(TRUE);
        }
    }

    void RegisterEventHandlers() {
        // 导航完成事件
        webview_->add_NavigationCompleted(
            Callback<ICoreWebView2NavigationCompletedEventHandler>(
                [](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                    BOOL success;
                    args->get_IsSuccess(&success);
                    if (success) {
                        OutputDebugString(L"Navigation completed successfully\n");
                    }
                    return S_OK;
                }).Get(),
            nullptr
        );

        // Web 消息接收事件
        webview_->add_WebMessageReceived(
            Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                this, &WebView2Window::OnWebMessageReceived).Get(),
            nullptr
        );

        // 新窗口请求事件
        webview_->add_NewWindowRequested(
            Callback<ICoreWebView2NewWindowRequestedEventHandler>(
                [](ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) -> HRESULT {
                    // 阻止新窗口，在当前窗口打开
                    wil::unique_cotaskmem_string uri;
                    args->get_Uri(&uri);
                    sender->Navigate(uri.get());
                    args->put_Handled(TRUE);
                    return S_OK;
                }).Get(),
            nullptr
        );
    }

    // 处理 Web 消息
    HRESULT OnWebMessageReceived(ICoreWebView2* sender,
                                  ICoreWebView2WebMessageReceivedEventArgs* args) {
        wil::unique_cotaskmem_string message;
        args->TryGetWebMessageAsString(&message);

        // 处理来自 JS 的消息
        std::wstring msg(message.get());
        if (msg == L"ping") {
            // 发送响应
            webview_->PostWebMessageAsString(L"pong");
        }

        return S_OK;
    }

    // 窗口大小改变
    void Resize() {
        if (controller_) {
            RECT bounds;
            GetClientRect(hwnd_, &bounds);
            controller_->put_Bounds(bounds);
        }
    }

    // 导航
    void Navigate(const std::wstring& url) {
        if (webview_) {
            webview_->Navigate(url.c_str());
        }
    }

    // 执行 JavaScript
    void ExecuteScript(const std::wstring& script) {
        if (webview_) {
            webview_->ExecuteScript(script.c_str(),
                Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
                    [](HRESULT error, LPCWSTR result) -> HRESULT {
                        if (SUCCEEDED(error)) {
                            // result 是 JSON 格式的返回值
                            OutputDebugStringW(result);
                        }
                        return S_OK;
                    }).Get()
            );
        }
    }
};
```

### 2.2 WinRT/C++ 集成

```cpp
// 使用 C++/WinRT 的更现代方式

#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.Web.WebView2.Core.h>

using namespace winrt;
using namespace Microsoft::Web::WebView2::Core;

class WebView2Controller {
public:
    CoreWebView2 webview_{ nullptr };
    CoreWebView2Controller controller_{ nullptr };

    fire_and_forget InitializeAsync(HWND hwnd) {
        // 创建环境
        auto environment = co_await CoreWebView2Environment::CreateAsync();

        // 创建控制器
        controller_ = co_await environment.CreateCoreWebView2ControllerAsync(
            reinterpret_cast<CoreWebView2ControllerWindowReference>(hwnd));

        webview_ = controller_.CoreWebView2();

        // 设置事件处理
        webview_.NavigationCompleted([](auto const& sender, auto const& args) {
            if (args.IsSuccess()) {
                OutputDebugString(L"Navigation completed\n");
            }
        });

        webview_.WebMessageReceived([this](auto const& sender, auto const& args) {
            auto message = args.TryGetWebMessageAsString();
            HandleWebMessage(message);
        });

        // 导航到初始页面
        webview_.Navigate(L"https://example.com");
    }

    void HandleWebMessage(hstring const& message) {
        // 处理 JS 消息
    }
};
```

---

## 3. JS 与 Native 通信

### 3.1 WebMessage 通信

```cpp
// C++ 端

// 发送消息到 JS
void SendToJS(const std::wstring& message) {
    webview_->PostWebMessageAsString(message.c_str());
}

// 发送 JSON
void SendJsonToJS(const std::wstring& json) {
    webview_->PostWebMessageAsJson(json.c_str());
}

// 接收 JS 消息
HRESULT OnWebMessageReceived(ICoreWebView2* sender,
                              ICoreWebView2WebMessageReceivedEventArgs* args) {
    wil::unique_cotaskmem_string message;
    args->TryGetWebMessageAsString(&message);

    // 解析 JSON
    // args->get_WebMessageAsJson(&json);

    ProcessMessage(message.get());
    return S_OK;
}
```

```javascript
// JavaScript 端

// 发送消息到 C++
window.chrome.webview.postMessage('Hello from JS');

// 发送 JSON
window.chrome.webview.postMessage({
    action: 'openFile',
    path: '/path/to/file'
});

// 接收 C++ 消息
window.chrome.webview.addEventListener('message', (event) => {
    console.log('Message from C++:', event.data);
});
```

### 3.2 Host Object 注入

```cpp
// 创建可在 JS 中调用的 COM 对象

class __declspec(uuid("...")) HostObject : public IDispatch {
public:
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDispatch) {
            *ppv = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return ++refCount_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG count = --refCount_;
        if (count == 0) delete this;
        return count;
    }

    // IDispatch
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override {
        *pctinfo = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(
        REFIID, LPOLESTR* rgszNames, UINT cNames, LCID, DISPID* rgDispId) override {

        for (UINT i = 0; i < cNames; i++) {
            if (wcscmp(rgszNames[i], L"showMessage") == 0) {
                rgDispId[i] = 1;
            } else if (wcscmp(rgszNames[i], L"readFile") == 0) {
                rgDispId[i] = 2;
            } else {
                rgDispId[i] = DISPID_UNKNOWN;
            }
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Invoke(
        DISPID dispId, REFIID, LCID, WORD wFlags,
        DISPPARAMS* pDispParams, VARIANT* pVarResult,
        EXCEPINFO*, UINT*) override {

        if (dispId == 1) {  // showMessage
            // 获取参数
            if (pDispParams->cArgs > 0 && pDispParams->rgvarg[0].vt == VT_BSTR) {
                MessageBoxW(nullptr, pDispParams->rgvarg[0].bstrVal, L"Message", MB_OK);
            }
            return S_OK;
        }

        if (dispId == 2) {  // readFile
            if (pDispParams->cArgs > 0 && pDispParams->rgvarg[0].vt == VT_BSTR) {
                std::wstring content = ReadFileContent(pDispParams->rgvarg[0].bstrVal);

                // 返回结果
                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(content.c_str());
            }
            return S_OK;
        }

        return DISP_E_MEMBERNOTFOUND;
    }

private:
    std::atomic<ULONG> refCount_{ 1 };
};

// 注入 Host Object
void InjectHostObject() {
    VARIANT hostObj;
    hostObj.vt = VT_DISPATCH;
    hostObj.pdispVal = new HostObject();

    webview_->AddHostObjectToScript(L"native", &hostObj);

    VariantClear(&hostObj);
}
```

```javascript
// JavaScript 端使用 Host Object

// 调用同步方法
window.chrome.webview.hostObjects.native.showMessage('Hello!');

// 调用返回值的方法
const content = await window.chrome.webview.hostObjects.native.readFile('C:\\file.txt');
console.log(content);

// 注意：Host Object 调用默认是异步的
// 使用 sync 版本进行同步调用 (会阻塞)
const syncContent = window.chrome.webview.hostObjects.sync.native.readFile('C:\\file.txt');
```

---

## 4. C# / .NET 集成

### 4.1 WPF 集成

```xml
<!-- XAML -->
<Window x:Class="WebView2Demo.MainWindow"
        xmlns:wv2="clr-namespace:Microsoft.Web.WebView2.Wpf;assembly=Microsoft.Web.WebView2.Wpf">
    <Grid>
        <wv2:WebView2 Name="webView"
                      Source="https://example.com"
                      NavigationCompleted="WebView_NavigationCompleted"
                      WebMessageReceived="WebView_WebMessageReceived"/>
    </Grid>
</Window>
```

```csharp
// MainWindow.xaml.cs

using Microsoft.Web.WebView2.Core;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        InitializeAsync();
    }

    async void InitializeAsync()
    {
        // 确保 WebView2 初始化完成
        await webView.EnsureCoreWebView2Async(null);

        // 配置设置
        webView.CoreWebView2.Settings.IsScriptEnabled = true;
        webView.CoreWebView2.Settings.AreDefaultScriptDialogsEnabled = true;

        // 注入 Host Object
        webView.CoreWebView2.AddHostObjectToScript("native", new NativeAPI());

        // 添加初始化脚本
        await webView.CoreWebView2.AddScriptToExecuteOnDocumentCreatedAsync(@"
            window.sendToNative = (msg) => {
                window.chrome.webview.postMessage(msg);
            };
        ");
    }

    private void WebView_NavigationCompleted(object sender,
        CoreWebView2NavigationCompletedEventArgs e)
    {
        if (e.IsSuccess)
        {
            Title = webView.CoreWebView2.DocumentTitle;
        }
    }

    private void WebView_WebMessageReceived(object sender,
        CoreWebView2WebMessageReceivedEventArgs e)
    {
        string message = e.TryGetWebMessageAsString();
        ProcessMessage(message);
    }

    // 导航
    public void Navigate(string url)
    {
        webView.CoreWebView2.Navigate(url);
    }

    // 执行 JS
    public async Task<string> ExecuteScriptAsync(string script)
    {
        return await webView.CoreWebView2.ExecuteScriptAsync(script);
    }

    // 发送消息到 JS
    public void PostMessage(string message)
    {
        webView.CoreWebView2.PostWebMessageAsString(message);
    }
}

// Host Object 类
[ClassInterface(ClassInterfaceType.AutoDual)]
[ComVisible(true)]
public class NativeAPI
{
    public void ShowMessage(string message)
    {
        MessageBox.Show(message);
    }

    public string ReadFile(string path)
    {
        return File.ReadAllText(path);
    }

    public async Task<string> ReadFileAsync(string path)
    {
        return await File.ReadAllTextAsync(path);
    }
}
```

### 4.2 WinForms 集成

```csharp
// Form1.cs

using Microsoft.Web.WebView2.WinForms;
using Microsoft.Web.WebView2.Core;

public partial class Form1 : Form
{
    private WebView2 webView;

    public Form1()
    {
        InitializeComponent();

        webView = new WebView2
        {
            Dock = DockStyle.Fill
        };
        Controls.Add(webView);

        InitWebView();
    }

    private async void InitWebView()
    {
        // 指定用户数据目录
        var env = await CoreWebView2Environment.CreateAsync(
            null,
            Path.Combine(Path.GetTempPath(), "WebView2Data"),
            new CoreWebView2EnvironmentOptions()
        );

        await webView.EnsureCoreWebView2Async(env);

        // 事件处理
        webView.CoreWebView2.NavigationStarting += (s, e) =>
        {
            // 可以取消导航
            // e.Cancel = true;
        };

        webView.CoreWebView2.SourceChanged += (s, e) =>
        {
            Text = webView.Source.ToString();
        };

        webView.Source = new Uri("https://example.com");
    }
}
```

---

## 5. 高级功能

### 5.1 自定义协议处理

```cpp
// 注册自定义协议

void RegisterCustomScheme() {
    // 添加虚拟主机映射
    webview_->SetVirtualHostNameToFolderMapping(
        L"app.local",                    // 虚拟主机名
        L"C:\\MyApp\\WebContent",        // 本地文件夹
        COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW
    );

    // 现在可以访问 https://app.local/index.html
}

// 或者使用资源请求过滤器
void SetupResourceFilter() {
    webview_->AddWebResourceRequestedFilter(
        L"*://app.local/*",
        COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL
    );

    webview_->add_WebResourceRequested(
        Callback<ICoreWebView2WebResourceRequestedEventHandler>(
            [](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT {
                wil::com_ptr<ICoreWebView2WebResourceRequest> request;
                args->get_Request(&request);

                wil::unique_cotaskmem_string uri;
                request->get_Uri(&uri);

                // 根据 URI 返回本地内容
                std::wstring content = LoadLocalResource(uri.get());

                wil::com_ptr<ICoreWebView2Environment> env;
                sender->get_Environment(&env);

                wil::com_ptr<ICoreWebView2WebResourceResponse> response;
                IStream* stream = SHCreateMemStream(
                    (BYTE*)content.c_str(),
                    content.size() * sizeof(wchar_t)
                );

                env->CreateWebResourceResponse(
                    stream,
                    200,
                    L"OK",
                    L"Content-Type: text/html; charset=utf-8",
                    &response
                );

                args->put_Response(response.get());
                return S_OK;
            }).Get(),
        nullptr
    );
}
```

### 5.2 Cookie 管理

```cpp
// 获取 Cookie Manager
wil::com_ptr<ICoreWebView2CookieManager> GetCookieManager() {
    wil::com_ptr<ICoreWebView2_2> webview2;
    webview_->QueryInterface(IID_PPV_ARGS(&webview2));

    wil::com_ptr<ICoreWebView2CookieManager> cookieManager;
    webview2->get_CookieManager(&cookieManager);

    return cookieManager;
}

// 设置 Cookie
void SetCookie(const std::wstring& name, const std::wstring& value,
               const std::wstring& domain) {
    auto cookieManager = GetCookieManager();

    wil::com_ptr<ICoreWebView2Cookie> cookie;
    cookieManager->CreateCookie(
        name.c_str(),
        value.c_str(),
        domain.c_str(),
        L"/",
        &cookie
    );

    cookie->put_IsSecure(TRUE);
    cookie->put_IsHttpOnly(FALSE);

    cookieManager->AddOrUpdateCookie(cookie.get());
}

// 获取 Cookie
void GetCookies(const std::wstring& uri) {
    auto cookieManager = GetCookieManager();

    cookieManager->GetCookies(uri.c_str(),
        Callback<ICoreWebView2GetCookiesCompletedHandler>(
            [](HRESULT error, ICoreWebView2CookieList* cookieList) -> HRESULT {
                UINT count;
                cookieList->get_Count(&count);

                for (UINT i = 0; i < count; i++) {
                    wil::com_ptr<ICoreWebView2Cookie> cookie;
                    cookieList->GetValueAtIndex(i, &cookie);

                    wil::unique_cotaskmem_string name;
                    cookie->get_Name(&name);

                    wil::unique_cotaskmem_string value;
                    cookie->get_Value(&value);

                    // 使用 cookie...
                }
                return S_OK;
            }).Get()
    );
}

// 删除 Cookie
void DeleteCookies(const std::wstring& name, const std::wstring& uri) {
    auto cookieManager = GetCookieManager();
    cookieManager->DeleteCookiesWithDomainAndPath(name.c_str(), L"", L"/");
}
```

### 5.3 下载管理

```cpp
// 处理下载
void SetupDownloadHandler() {
    wil::com_ptr<ICoreWebView2_4> webview4;
    webview_->QueryInterface(IID_PPV_ARGS(&webview4));

    webview4->add_DownloadStarting(
        Callback<ICoreWebView2DownloadStartingEventHandler>(
            [](ICoreWebView2* sender, ICoreWebView2DownloadStartingEventArgs* args) -> HRESULT {

                wil::com_ptr<ICoreWebView2DownloadOperation> download;
                args->get_DownloadOperation(&download);

                // 获取下载信息
                wil::unique_cotaskmem_string uri;
                download->get_Uri(&uri);

                wil::unique_cotaskmem_string contentDisposition;
                download->get_ContentDisposition(&contentDisposition);

                INT64 totalBytes;
                download->get_TotalBytesToReceive(&totalBytes);

                // 修改保存路径
                args->put_ResultFilePath(L"C:\\Downloads\\myfile.zip");

                // 监听下载进度
                download->add_BytesReceivedChanged(
                    Callback<ICoreWebView2BytesReceivedChangedEventHandler>(
                        [](ICoreWebView2DownloadOperation* sender, IUnknown* args) -> HRESULT {
                            INT64 received;
                            sender->get_BytesReceived(&received);

                            INT64 total;
                            sender->get_TotalBytesToReceive(&total);

                            double progress = (double)received / total * 100;
                            // 更新进度...

                            return S_OK;
                        }).Get(),
                    nullptr
                );

                // 取消下载
                // args->put_Cancel(TRUE);

                return S_OK;
            }).Get(),
        nullptr
    );
}
```

---

## 6. 面试常见问题

### Q1: WebView2 的 Evergreen 和 Fixed Version 分发模式有什么区别？

**答案：**

| 特性 | Evergreen | Fixed Version |
|------|-----------|---------------|
| 运行时 | 使用系统安装的 | 捆绑在应用中 |
| 更新 | 自动（随 Edge 更新） | 手动更新 |
| 包体积 | 无额外体积 | +100MB |
| 版本控制 | 无法固定版本 | 完全控制 |
| 适用场景 | 普通应用 | 企业环境/离线 |

### Q2: WebView2 如何实现 JS 调用 Native 代码？

**答案：**

1. **WebMessage**：简单的字符串/JSON 消息传递
   ```javascript
   window.chrome.webview.postMessage('message');
   ```

2. **Host Object**：注入 COM 对象，JS 直接调用方法
   ```javascript
   await window.chrome.webview.hostObjects.native.doSomething();
   ```

3. **ExecuteScript 回调**：Native 执行 JS 并获取返回值

### Q3: WebView2 与 CEF 相比有什么优缺点？

**答案：**

**优点：**
- 零额外包体积（使用系统 Runtime）
- 自动更新，始终是最新版 Chromium
- 与 Windows 深度集成
- 支持 .NET 和 WinRT

**缺点：**
- 仅支持 Windows（10+）
- 版本不可控（Evergreen 模式）
- 共享 Edge 进程，可能受其他应用影响
- API 相比 CEF 较少

### Q4: 如何处理 WebView2 运行时未安装的情况？

**答案：**
```csharp
async Task InitWebView()
{
    try
    {
        await webView.EnsureCoreWebView2Async();
    }
    catch (WebView2RuntimeNotFoundException)
    {
        // 运行时未安装，提示用户下载
        var result = MessageBox.Show(
            "WebView2 运行时未安装，是否下载安装？",
            "提示",
            MessageBoxButtons.YesNo);

        if (result == DialogResult.Yes)
        {
            // 打开下载页面
            Process.Start(new ProcessStartInfo
            {
                FileName = "https://developer.microsoft.com/microsoft-edge/webview2/",
                UseShellExecute = true
            });
        }
    }
}
```

### Q5: WebView2 的多实例内存占用如何优化？

**答案：**
1. **共享用户数据目录**：多个 WebView2 使用同一个用户数据目录
2. **共享进程**：设置 `WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS=--single-process`（不推荐，影响稳定性）
3. **延迟初始化**：按需创建 WebView2 实例
4. **使用 Fixed Version**：可以更精细控制进程模型
