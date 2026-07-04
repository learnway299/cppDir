# 03 - 进程间通信协议

## 概述

本章深入剖析前端容器中 Browser 进程与 Renderer 进程之间的通信机制，包括 IPC 管道、消息序列化、Mojo 框架等核心技术。

---

## 1. 为什么需要 IPC

### 1.1 多进程架构下的通信需求

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       多进程通信场景                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Browser Process                         Renderer Process                   │
│  ┌────────────────┐                     ┌────────────────┐                 │
│  │  - 网络请求    │ ◄──── IPC ────────► │  - 页面渲染    │                 │
│  │  - 文件访问    │                     │  - JS 执行     │                 │
│  │  - 系统 API    │                     │  - DOM 操作    │                 │
│  │  - 进程管理    │                     │  - 事件处理    │                 │
│  └────────────────┘                     └────────────────┘                 │
│                                                                             │
│  通信场景:                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ 1. Renderer → Browser:                                              │   │
│  │    - 发起网络请求                                                   │   │
│  │    - 请求文件读写                                                   │   │
│  │    - 调用系统 API (剪贴板、通知等)                                  │   │
│  │    - 创建新窗口/标签页                                              │   │
│  │                                                                     │   │
│  │ 2. Browser → Renderer:                                              │   │
│  │    - 传递网络响应数据                                               │   │
│  │    - 通知导航事件                                                   │   │
│  │    - 触发 JS 执行                                                   │   │
│  │    - 转发用户输入事件                                               │   │
│  │                                                                     │   │
│  │ 3. Renderer ↔ GPU:                                                  │   │
│  │    - 提交绘制命令                                                   │   │
│  │    - 获取合成结果                                                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 IPC 设计考量

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          IPC 设计考量                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  性能:                                                                      │
│  ├── 延迟: 尽可能低的消息传递延迟                                          │
│  ├── 吞吐量: 支持大量小消息和少量大消息                                    │
│  └── 零拷贝: 避免不必要的内存复制                                          │
│                                                                             │
│  安全:                                                                      │
│  ├── 验证: Browser 必须验证所有 Renderer 请求                              │
│  ├── 隔离: 不同 Renderer 进程互相隔离                                      │
│  └── 权限: 最小权限原则                                                    │
│                                                                             │
│  可靠性:                                                                    │
│  ├── 有序性: 消息按发送顺序到达                                            │
│  ├── 完整性: 消息不会丢失或损坏                                            │
│  └── 错误处理: 优雅处理进程崩溃                                            │
│                                                                             │
│  易用性:                                                                    │
│  ├── 类型安全: 编译时检查消息结构                                          │
│  ├── 异步支持: 非阻塞的消息收发                                            │
│  └── 接口简洁: 易于使用和维护                                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Chromium IPC 机制演进

### 2.1 传统 IPC (Chrome IPC)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    传统 Chrome IPC 架构                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────┐      Named Pipe       ┌─────────────────┐             │
│  │ Browser Process │ ◄───────────────────► │ Renderer Process│             │
│  │                 │                       │                 │             │
│  │ ┌─────────────┐ │                       │ ┌─────────────┐ │             │
│  │ │ IPC::Channel │ │                       │ │ IPC::Channel │ │             │
│  │ │             │ │      Message          │ │             │ │             │
│  │ │ ┌─────────┐ │ │  ←──────────────────  │ │ ┌─────────┐ │ │             │
│  │ │ │ Listener │ │ │                       │ │ │ Sender  │ │ │             │
│  │ │ └─────────┘ │ │  ──────────────────→  │ │ └─────────┘ │ │             │
│  │ │ ┌─────────┐ │ │                       │ │ ┌─────────┐ │ │             │
│  │ │ │ Sender  │ │ │                       │ │ │ Listener │ │ │             │
│  │ │ └─────────┘ │ │                       │ │ └─────────┘ │ │             │
│  │ └─────────────┘ │                       │ └─────────────┘ │             │
│  └─────────────────┘                       └─────────────────┘             │
│                                                                             │
│  消息格式:                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Header (12 bytes)                                                  │   │
│  │  ├── payload_size (4 bytes)                                         │   │
│  │  ├── routing_id (4 bytes)                                           │   │
│  │  └── type (4 bytes) - 消息类型 ID                                   │   │
│  │                                                                     │   │
│  │  Payload (变长)                                                     │   │
│  │  └── Pickle 序列化的参数数据                                        │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Mojo IPC (现代架构)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Mojo IPC 架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  特点:                                                                      │
│  ├── 面向接口的设计 (Mojom IDL)                                            │
│  ├── 支持跨进程和进程内通信                                                │
│  ├── 内置能力转移 (Capability Transfer)                                    │
│  └── 支持共享内存和句柄传递                                                │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                      Mojom 接口定义                                  │   │
│  │                                                                     │   │
│  │  // example.mojom                                                   │   │
│  │  module example.mojom;                                              │   │
│  │                                                                     │   │
│  │  interface FileReader {                                             │   │
│  │      ReadFile(string path) => (bool success, array<uint8> data);    │   │
│  │      WatchFile(string path) => (pending_receiver<FileWatcher> w);   │   │
│  │  };                                                                 │   │
│  │                                                                     │   │
│  │  interface FileWatcher {                                            │   │
│  │      OnFileChanged(string path);                                    │   │
│  │  };                                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  通信模型:                                                                  │
│  ┌────────────┐                           ┌────────────┐                   │
│  │   Client   │                           │   Server   │                   │
│  │ (Renderer) │                           │ (Browser)  │                   │
│  │            │        Message Pipe       │            │                   │
│  │ ┌────────┐ │    ┌────────────────┐     │ ┌────────┐ │                   │
│  │ │ Remote │─┼────┤ Handle0 Handle1│─────┼─│Receiver│ │                   │
│  │ └────────┘ │    └────────────────┘     │ └────────┘ │                   │
│  │            │                           │            │                   │
│  └────────────┘                           └────────────┘                   │
│                                                                             │
│  Remote<T>: 发送请求的一端                                                  │
│  Receiver<T>: 接收请求的一端                                                │
│  Message Pipe: 双向通道，由两个 Handle 组成                                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.3 Mojo 使用示例

```cpp
// 定义接口 (file_service.mojom)
module file.mojom;

interface FileService {
    // 同步请求 (返回值)
    ReadFile(string path) => (bool success, string content);

    // 异步请求 (无返回值)
    WriteFile(string path, string content);

    // 带回调的请求
    DeleteFile(string path) => (bool success, string error_message);
};

// ==================== 服务端实现 ====================

class FileServiceImpl : public file::mojom::FileService {
public:
    // 绑定 Receiver
    explicit FileServiceImpl(
        mojo::PendingReceiver<file::mojom::FileService> receiver)
        : receiver_(this, std::move(receiver)) {}

    // 实现接口方法
    void ReadFile(const std::string& path, ReadFileCallback callback) override {
        std::string content;
        bool success = ReadFileFromDisk(path, &content);
        std::move(callback).Run(success, content);
    }

    void WriteFile(const std::string& path, const std::string& content) override {
        WriteFileToDisk(path, content);
    }

    void DeleteFile(const std::string& path, DeleteFileCallback callback) override {
        std::string error;
        bool success = DeleteFileFromDisk(path, &error);
        std::move(callback).Run(success, error);
    }

private:
    mojo::Receiver<file::mojom::FileService> receiver_;
};

// ==================== 客户端调用 ====================

class FileClient {
public:
    void Init() {
        // 创建 Message Pipe
        mojo::PendingRemote<file::mojom::FileService> remote;
        auto receiver = remote.InitWithNewPipeAndPassReceiver();

        // 服务端绑定 Receiver
        file_service_impl_ = std::make_unique<FileServiceImpl>(std::move(receiver));

        // 客户端持有 Remote
        file_service_.Bind(std::move(remote));
    }

    void ReadFile(const std::string& path) {
        // 异步调用
        file_service_->ReadFile(path,
            base::BindOnce(&FileClient::OnReadComplete, base::Unretained(this)));
    }

    void OnReadComplete(bool success, const std::string& content) {
        if (success) {
            LOG(INFO) << "File content: " << content;
        } else {
            LOG(ERROR) << "Failed to read file";
        }
    }

private:
    std::unique_ptr<FileServiceImpl> file_service_impl_;
    mojo::Remote<file::mojom::FileService> file_service_;
};
```

---

## 3. CEF IPC 机制

### 3.1 CefProcessMessage

```cpp
// ==================== Renderer 端发送消息 ====================

class MyRenderProcessHandler : public CefRenderProcessHandler {
public:
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override {
        // 发送消息到 Browser 进程
        CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("QueryData");

        CefRefPtr<CefListValue> args = msg->GetArgumentList();
        args->SetString(0, "user_info");
        args->SetInt(1, 12345);
        args->SetBool(2, true);

        // 发送
        frame->SendProcessMessage(PID_BROWSER, msg);
    }

    // 接收 Browser 进程的响应
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override {
        if (message->GetName() == "QueryDataResponse") {
            CefRefPtr<CefListValue> args = message->GetArgumentList();
            std::string data = args->GetString(0).ToString();

            // 执行 JS 回调
            frame->ExecuteJavaScript(
                "window.onDataReceived('" + data + "')",
                frame->GetURL(), 0);
            return true;
        }
        return false;
    }
};

// ==================== Browser 端接收消息 ====================

class MyClient : public CefClient {
public:
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override {
        if (message->GetName() == "QueryData") {
            CefRefPtr<CefListValue> args = message->GetArgumentList();
            std::string query = args->GetString(0).ToString();
            int id = args->GetInt(1);

            // 处理请求
            std::string result = ProcessQuery(query, id);

            // 发送响应
            CefRefPtr<CefProcessMessage> response =
                CefProcessMessage::Create("QueryDataResponse");
            response->GetArgumentList()->SetString(0, result);

            frame->SendProcessMessage(PID_RENDERER, response);
            return true;
        }
        return false;
    }

private:
    IMPLEMENT_REFCOUNTING(MyClient);
};
```

### 3.2 CefMessageRouter (高级封装)

```cpp
// ==================== 配置 MessageRouter ====================

// Browser 进程
class MyClient : public CefClient,
                 public CefRequestHandler,
                 public CefLifeSpanHandler {
public:
    MyClient() {
        // 创建 MessageRouter
        CefMessageRouterConfig config;
        config.js_query_function = "cefQuery";      // JS 调用函数名
        config.js_cancel_function = "cefQueryCancel"; // 取消函数名

        message_router_ = CefMessageRouterBrowserSide::Create(config);
        message_router_->AddHandler(new MyQueryHandler(), false);
    }

    bool OnProcessMessageReceived(...) override {
        return message_router_->OnProcessMessageReceived(...);
    }

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        message_router_->OnBeforeClose(browser);
    }

private:
    CefRefPtr<CefMessageRouterBrowserSide> message_router_;
};

// ==================== 请求处理器 ====================

class MyQueryHandler : public CefMessageRouterBrowserSide::Handler {
public:
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64_t query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override {

        // 解析请求 JSON
        // {"action": "getFile", "path": "/path/to/file"}

        if (request.ToString().find("getFile") != std::string::npos) {
            // 异步处理
            std::thread([callback, request]() {
                std::string content = "File content...";

                // 成功响应
                callback->Success(content);

                // 或失败响应
                // callback->Failure(404, "File not found");
            }).detach();

            return true;  // 已处理
        }

        return false;  // 未处理，交给下一个 Handler
    }

    void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64_t query_id) override {
        // 查询被取消（页面导航、浏览器关闭等）
        CancelPendingOperation(query_id);
    }
};

// ==================== JavaScript 端调用 ====================

/*
// 发送查询
window.cefQuery({
    request: JSON.stringify({action: 'getFile', path: '/path/to/file'}),
    persistent: false,  // true 表示持久查询（如事件订阅）
    onSuccess: function(response) {
        console.log('Success:', response);
    },
    onFailure: function(errorCode, errorMessage) {
        console.error('Error:', errorCode, errorMessage);
    }
});

// 取消查询
var queryId = window.cefQuery({...});
window.cefQueryCancel(queryId);
*/
```

---

## 4. Electron IPC

### 4.1 ipcMain / ipcRenderer

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       Electron IPC 模型                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Main Process                              Renderer Process                │
│   ┌────────────────────┐                   ┌────────────────────┐          │
│   │      ipcMain       │                   │    ipcRenderer     │          │
│   │                    │                   │                    │          │
│   │  .on()             │ ◄──── send ─────  │  .send()           │          │
│   │  .handle()         │ ◄──── invoke ───  │  .invoke()         │          │
│   │                    │                   │                    │          │
│   │  webContents.send()│ ───── reply ────► │  .on()             │          │
│   └────────────────────┘                   └────────────────────┘          │
│                                                                             │
│   通信模式:                                                                 │
│   ┌─────────────────────────────────────────────────────────────────────┐  │
│   │                                                                     │  │
│   │  1. 单向通信 (Renderer → Main)                                      │  │
│   │     ipcRenderer.send('channel', data)                               │  │
│   │     ipcMain.on('channel', (event, data) => {...})                   │  │
│   │                                                                     │  │
│   │  2. 双向通信 - invoke/handle (推荐)                                 │  │
│   │     const result = await ipcRenderer.invoke('channel', data)        │  │
│   │     ipcMain.handle('channel', async (event, data) => result)        │  │
│   │                                                                     │  │
│   │  3. 单向通信 (Main → Renderer)                                      │  │
│   │     webContents.send('channel', data)                               │  │
│   │     ipcRenderer.on('channel', (event, data) => {...})               │  │
│   │                                                                     │  │
│   │  4. 同步通信 (阻塞，不推荐)                                         │  │
│   │     const result = ipcRenderer.sendSync('channel', data)            │  │
│   │     ipcMain.on('channel', (event, data) => {                        │  │
│   │         event.returnValue = result;                                 │  │
│   │     })                                                              │  │
│   │                                                                     │  │
│   └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 完整示例

```javascript
// ==================== main.js (主进程) ====================

const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const fs = require('fs').promises;
const path = require('path');

let mainWindow;

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1200,
        height: 800,
        webPreferences: {
            nodeIntegration: false,
            contextIsolation: true,
            preload: path.join(__dirname, 'preload.js')
        }
    });

    mainWindow.loadFile('index.html');
}

// 处理文件读取请求
ipcMain.handle('read-file', async (event, filePath) => {
    try {
        const content = await fs.readFile(filePath, 'utf-8');
        return { success: true, content };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

// 处理文件保存请求
ipcMain.handle('save-file', async (event, { filePath, content }) => {
    try {
        await fs.writeFile(filePath, content, 'utf-8');
        return { success: true };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

// 处理打开文件对话框
ipcMain.handle('show-open-dialog', async (event, options) => {
    const result = await dialog.showOpenDialog(mainWindow, options);
    return result;
});

// 单向通知 (Renderer → Main)
ipcMain.on('log-message', (event, message) => {
    console.log('[Renderer]', message);
});

// 主进程主动推送消息
function notifyRenderer(channel, data) {
    if (mainWindow) {
        mainWindow.webContents.send(channel, data);
    }
}

// 示例：文件变更通知
const chokidar = require('chokidar');
ipcMain.handle('watch-directory', async (event, dirPath) => {
    const watcher = chokidar.watch(dirPath);

    watcher.on('change', (filePath) => {
        notifyRenderer('file-changed', { filePath });
    });

    return { success: true };
});

app.whenReady().then(createWindow);
```

```javascript
// ==================== preload.js (预加载脚本) ====================

const { contextBridge, ipcRenderer } = require('electron');

// 安全地暴露 API
contextBridge.exposeInMainWorld('electronAPI', {
    // 文件操作
    readFile: (filePath) => ipcRenderer.invoke('read-file', filePath),
    saveFile: (filePath, content) => ipcRenderer.invoke('save-file', { filePath, content }),

    // 对话框
    showOpenDialog: (options) => ipcRenderer.invoke('show-open-dialog', options),

    // 单向消息
    log: (message) => ipcRenderer.send('log-message', message),

    // 监听主进程消息
    onFileChanged: (callback) => {
        ipcRenderer.on('file-changed', (event, data) => callback(data));
    },

    // 文件监听
    watchDirectory: (dirPath) => ipcRenderer.invoke('watch-directory', dirPath),

    // 移除监听器
    removeAllListeners: (channel) => {
        ipcRenderer.removeAllListeners(channel);
    }
});
```

```javascript
// ==================== renderer.js (渲染进程) ====================

async function openFile() {
    // 打开文件选择对话框
    const result = await window.electronAPI.showOpenDialog({
        properties: ['openFile'],
        filters: [
            { name: 'Text Files', extensions: ['txt', 'md'] },
            { name: 'All Files', extensions: ['*'] }
        ]
    });

    if (!result.canceled && result.filePaths.length > 0) {
        const filePath = result.filePaths[0];

        // 读取文件
        const { success, content, error } = await window.electronAPI.readFile(filePath);

        if (success) {
            document.getElementById('editor').value = content;
            currentFilePath = filePath;
        } else {
            alert('Failed to read file: ' + error);
        }
    }
}

async function saveFile() {
    if (currentFilePath) {
        const content = document.getElementById('editor').value;
        const { success, error } = await window.electronAPI.saveFile(currentFilePath, content);

        if (!success) {
            alert('Failed to save file: ' + error);
        }
    }
}

// 监听文件变更
window.electronAPI.onFileChanged(({ filePath }) => {
    console.log('File changed:', filePath);
    // 可以提示用户重新加载
});

// 记录日志
window.electronAPI.log('Renderer process started');
```

---

## 5. 消息序列化

### 5.1 序列化方案对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       序列化方案对比                                        │
├─────────────┬──────────────┬──────────────┬──────────────┬─────────────────┤
│   方案       │    性能      │   包体大小   │   可读性     │    类型安全     │
├─────────────┼──────────────┼──────────────┼──────────────┼─────────────────┤
│ JSON        │  中等        │  较大        │  高          │  运行时         │
│ Pickle      │  快          │  中等        │  低(二进制)  │  运行时         │
│ Protobuf    │  很快        │  小          │  低(二进制)  │  编译时         │
│ FlatBuffers │  最快(零拷贝)│  最小        │  低(二进制)  │  编译时         │
│ MessagePack │  快          │  较小        │  低(二进制)  │  运行时         │
└─────────────┴──────────────┴──────────────┴──────────────┴─────────────────┘
```

### 5.2 Chromium Pickle

```cpp
// Pickle: Chromium 传统序列化方案

#include "base/pickle.h"

// 序列化
void SerializeData(base::Pickle* pickle) {
    pickle->WriteInt(42);
    pickle->WriteString("Hello");
    pickle->WriteBool(true);
    pickle->WriteFloat(3.14f);

    // 写入数组
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    pickle->WriteInt(numbers.size());
    for (int n : numbers) {
        pickle->WriteInt(n);
    }
}

// 反序列化
void DeserializeData(const base::Pickle& pickle) {
    base::PickleIterator iter(pickle);

    int intValue;
    std::string stringValue;
    bool boolValue;
    float floatValue;

    if (!iter.ReadInt(&intValue)) return;
    if (!iter.ReadString(&stringValue)) return;
    if (!iter.ReadBool(&boolValue)) return;
    if (!iter.ReadFloat(&floatValue)) return;

    // 读取数组
    int arraySize;
    if (!iter.ReadInt(&arraySize)) return;

    std::vector<int> numbers;
    numbers.reserve(arraySize);
    for (int i = 0; i < arraySize; ++i) {
        int n;
        if (!iter.ReadInt(&n)) return;
        numbers.push_back(n);
    }
}
```

### 5.3 CEF 支持的数据类型

```cpp
// CefListValue 支持的类型
void SerializeCefValue(CefRefPtr<CefListValue> list) {
    list->SetBool(0, true);                     // Boolean
    list->SetInt(1, 42);                        // Integer
    list->SetDouble(2, 3.14159);                // Double
    list->SetString(3, "Hello World");          // String
    list->SetNull(4);                           // Null

    // 嵌套列表
    CefRefPtr<CefListValue> nestedList = CefListValue::Create();
    nestedList->SetInt(0, 1);
    nestedList->SetInt(1, 2);
    nestedList->SetInt(2, 3);
    list->SetList(5, nestedList);

    // 字典对象
    CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();
    dict->SetString("name", "John");
    dict->SetInt("age", 30);
    list->SetDictionary(6, dict);

    // 二进制数据
    std::vector<uint8_t> binaryData = {0x01, 0x02, 0x03, 0x04};
    CefRefPtr<CefBinaryValue> binary = CefBinaryValue::Create(
        binaryData.data(), binaryData.size());
    list->SetBinary(7, binary);
}
```

---

## 6. 共享内存

### 6.1 为什么使用共享内存

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       IPC vs 共享内存                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  传统 IPC (消息传递):                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Process A          Kernel           Process B                      │   │
│  │  ┌────────┐        ┌────────┐        ┌────────┐                    │   │
│  │  │ Buffer │──Copy─▶│ Buffer │──Copy─▶│ Buffer │                    │   │
│  │  └────────┘        └────────┘        └────────┘                    │   │
│  │                                                                     │   │
│  │  问题: 大数据需要两次内存拷贝                                       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  共享内存:                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Process A                           Process B                      │   │
│  │  ┌────────┐                          ┌────────┐                    │   │
│  │  │ 虚拟地址│──────────┐  ┌───────────│ 虚拟地址│                    │   │
│  │  └────────┘          │  │            └────────┘                    │   │
│  │                      ▼  ▼                                          │   │
│  │               ┌──────────────┐                                     │   │
│  │               │  共享内存区  │ (物理内存)                          │   │
│  │               └──────────────┘                                     │   │
│  │                                                                     │   │
│  │  优势: 零拷贝，直接读写同一块内存                                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  适用场景:                                                                  │
│  - 视频帧传输                                                              │
│  - 大型图片数据                                                            │
│  - 音频流                                                                  │
│  - 高频更新的大型数据结构                                                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 6.2 Chromium 共享内存 API

```cpp
#include "base/memory/shared_memory_region.h"
#include "base/memory/writable_shared_memory_region.h"

// ==================== 创建共享内存 ====================

class SharedMemoryExample {
public:
    bool CreateSharedMemory(size_t size) {
        // 创建可写共享内存区域
        auto region = base::WritableSharedMemoryRegion::Create(size);
        if (!region.IsValid()) {
            return false;
        }

        // 映射到当前进程地址空间
        mapping_ = region.Map();
        if (!mapping_.IsValid()) {
            return false;
        }

        // 转换为只读区域用于传递给其他进程
        read_only_region_ = base::WritableSharedMemoryRegion::ConvertToReadOnly(
            std::move(region));

        return true;
    }

    // 写入数据
    void WriteData(const void* data, size_t size) {
        memcpy(mapping_.memory(), data, size);
    }

    // 获取只读句柄 (用于 IPC 传递)
    base::ReadOnlySharedMemoryRegion GetReadOnlyRegion() {
        return read_only_region_.Duplicate();
    }

private:
    base::WritableSharedMemoryMapping mapping_;
    base::ReadOnlySharedMemoryRegion read_only_region_;
};

// ==================== 接收端 ====================

class SharedMemoryReceiver {
public:
    bool AttachToSharedMemory(base::ReadOnlySharedMemoryRegion region) {
        if (!region.IsValid()) {
            return false;
        }

        mapping_ = region.Map();
        return mapping_.IsValid();
    }

    const void* GetData() const {
        return mapping_.memory();
    }

    size_t GetSize() const {
        return mapping_.size();
    }

private:
    base::ReadOnlySharedMemoryMapping mapping_;
};
```

### 6.3 JavaScript SharedArrayBuffer

```javascript
// 注意: 需要设置特定的 HTTP 头
// Cross-Origin-Opener-Policy: same-origin
// Cross-Origin-Embedder-Policy: require-corp

// 创建共享内存
const sharedBuffer = new SharedArrayBuffer(1024 * 1024); // 1MB

// 创建视图
const sharedArray = new Int32Array(sharedBuffer);

// 主线程写入
sharedArray[0] = 42;

// Worker 中读取 (无需拷贝)
// worker.js
self.onmessage = (e) => {
    const sharedArray = new Int32Array(e.data);
    console.log(sharedArray[0]); // 42

    // 原子操作 (避免竞争)
    Atomics.add(sharedArray, 0, 1);
    console.log(sharedArray[0]); // 43
};

// 传递 SharedArrayBuffer 到 Worker
worker.postMessage(sharedBuffer);
```

---

## 7. 面试常见问题

### Q1: Chromium 为什么从传统 IPC 迁移到 Mojo？

**答案：**
1. **类型安全**：Mojom IDL 在编译时生成强类型代码，减少运行时错误
2. **接口导向**：更清晰的服务边界和依赖关系
3. **能力转移**：可以安全地传递接口句柄，动态建立通信
4. **统一模型**：同一套 API 支持跨进程和进程内通信
5. **性能**：内置共享内存和句柄传递支持

### Q2: CEF 的 CefProcessMessage 和 CefMessageRouter 的区别？

**答案：**
| 特性 | CefProcessMessage | CefMessageRouter |
|------|-------------------|------------------|
| 抽象级别 | 底层 | 高层封装 |
| 请求-响应 | 需手动实现 | 内置支持 |
| 取消机制 | 无 | 支持查询取消 |
| 持久查询 | 无 | 支持事件订阅 |
| JS 集成 | 需手动绑定 | 自动注入 `cefQuery` |
| 适用场景 | 简单通知 | 复杂请求-响应模式 |

### Q3: Electron 的 invoke/handle 比 send/on 有什么优势？

**答案：**
1. **返回值**：invoke 返回 Promise，可以直接获取结果
2. **错误处理**：异常自动传播到调用方
3. **类型推断**：更好的 TypeScript 支持
4. **代码清晰**：请求-响应模式更直观
5. **避免回调地狱**：支持 async/await

```javascript
// send/on 模式 (复杂)
ipcRenderer.send('request', data);
ipcRenderer.on('response', (event, result) => {
    // 需要手动关联请求和响应
});

// invoke/handle 模式 (简洁)
const result = await ipcRenderer.invoke('request', data);
```

### Q4: 如何安全地在进程间传递大量二进制数据？

**答案：**
1. **共享内存**：创建 SharedMemoryRegion，传递句柄而非数据
2. **分块传输**：大文件分成多个小块顺序传输
3. **文件传递**：写入临时文件，传递文件路径
4. **零拷贝**：使用 transferable 对象（如 ArrayBuffer）

```cpp
// 推荐：共享内存方式
auto region = base::WritableSharedMemoryRegion::Create(dataSize);
auto mapping = region.Map();
memcpy(mapping.memory(), largeData, dataSize);

// 只传递句柄，不拷贝数据
SendHandleToOtherProcess(region.Duplicate());
```

### Q5: 如何防止恶意 Renderer 进程滥用 IPC？

**答案：**
1. **验证所有输入**：Browser 进程必须验证每个 IPC 参数
2. **权限检查**：敏感操作需要验证调用者权限
3. **限流**：防止请求泛滥
4. **白名单**：只允许预定义的消息类型
5. **沙箱**：Renderer 在受限环境运行

```cpp
// Browser 端示例
bool OnProcessMessageReceived(...) {
    // 1. 验证消息来源
    if (source_process != PID_RENDERER) return false;

    // 2. 验证消息类型
    if (!IsAllowedMessageType(message->GetName())) return false;

    // 3. 验证参数
    auto args = message->GetArgumentList();
    if (!ValidateArguments(args)) return false;

    // 4. 权限检查
    if (!HasPermission(browser, message->GetName())) return false;

    // 5. 处理请求
    ProcessMessage(message);
    return true;
}
```
