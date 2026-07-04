# 05 - Electron 架构详解

## 概述

Electron 是基于 Chromium 和 Node.js 的跨平台桌面应用开发框架，广泛应用于 VS Code、Slack、Discord 等知名应用。本章深入剖析 Electron 的架构设计和核心机制。

---

## 1. Electron 架构总览

### 1.1 核心组成

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        Electron 核心架构                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        Electron 应用                                 │   │
│  │                                                                     │   │
│  │  ┌───────────────────────┐     ┌───────────────────────┐           │   │
│  │  │    Main Process       │     │   Renderer Process    │           │   │
│  │  │       (主进程)         │     │      (渲染进程)        │           │   │
│  │  │                       │     │                       │           │   │
│  │  │  ┌─────────────────┐  │     │  ┌─────────────────┐  │           │   │
│  │  │  │    Node.js      │  │     │  │   Chromium      │  │           │   │
│  │  │  │  - 文件系统     │  │     │  │   - HTML/CSS    │  │           │   │
│  │  │  │  - 网络         │  │     │  │   - JavaScript  │  │           │   │
│  │  │  │  - 子进程       │  │     │  │   - DOM         │  │           │   │
│  │  │  │  - Native 模块  │  │     │  │   - Canvas      │  │           │   │
│  │  │  └─────────────────┘  │     │  └─────────────────┘  │           │   │
│  │  │                       │     │                       │           │   │
│  │  │  ┌─────────────────┐  │ IPC │  ┌─────────────────┐  │           │   │
│  │  │  │  Electron API   │◄─┼─────┼─►│  Preload Script │  │           │   │
│  │  │  │  - app          │  │     │  │  - contextBridge│  │           │   │
│  │  │  │  - BrowserWindow│  │     │  │  - ipcRenderer  │  │           │   │
│  │  │  │  - Menu         │  │     │  │                 │  │           │   │
│  │  │  │  - dialog       │  │     │  └─────────────────┘  │           │   │
│  │  │  │  - ipcMain      │  │     │                       │           │   │
│  │  │  └─────────────────┘  │     │                       │           │   │
│  │  └───────────────────────┘     └───────────────────────┘           │   │
│  │            │                             │                          │   │
│  │            └──────────┬──────────────────┘                          │   │
│  │                       ▼                                             │   │
│  │               ┌───────────────┐                                     │   │
│  │               │  系统原生 API │                                     │   │
│  │               │  Windows/macOS/Linux                               │   │
│  │               └───────────────┘                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 进程模型对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     Electron vs CEF 进程模型                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  CEF:                                                                       │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Browser Process (C++)     ◄──IPC──►     Renderer Process           │   │
│  │      │                                        │                     │   │
│  │      └── 应用逻辑在 C++ 中                    └── 纯 Web 内容        │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Electron:                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  Main Process (Node.js)    ◄──IPC──►     Renderer Process           │   │
│  │      │                                        │                     │   │
│  │      ├── 应用逻辑在 JavaScript 中             ├── Web 内容           │   │
│  │      └── 可使用 Node.js 生态                  └── 可有限访问 Node.js │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  关键区别:                                                                  │
│  ┌─────────────┬───────────────────────┬───────────────────────┐           │
│  │             │        CEF            │       Electron        │           │
│  ├─────────────┼───────────────────────┼───────────────────────┤           │
│  │ 主进程语言  │        C++            │       Node.js         │           │
│  │ 渲染进程    │        纯 Web         │       Web + Node.js   │           │
│  │ Native 绑定 │        V8 Extension   │       N-API           │           │
│  │ 生态系统    │        C++ 库         │       npm 生态        │           │
│  │ 包体大小    │        ~100MB         │       ~150MB          │           │
│  │ 学习曲线    │        陡峭 (C++)     │       平缓 (JS)       │           │
│  └─────────────┴───────────────────────┴───────────────────────┘           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Main Process (主进程)

### 2.1 应用生命周期

```javascript
// main.js

const { app, BrowserWindow, Menu, Tray, dialog } = require('electron');
const path = require('path');

let mainWindow;
let tray;

// ==================== 生命周期事件 ====================

// 应用准备就绪
app.whenReady().then(() => {
    createWindow();

    // macOS: 点击 dock 图标时重新创建窗口
    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});

// 所有窗口关闭
app.on('window-all-closed', () => {
    // macOS: 关闭窗口不退出应用
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

// 应用即将退出
app.on('before-quit', (event) => {
    // 可以阻止退出
    // event.preventDefault();

    // 保存状态
    saveAppState();
});

// 应用退出
app.on('quit', () => {
    console.log('Application quit');
});

// 第二个实例启动 (单实例应用)
const gotTheLock = app.requestSingleInstanceLock();
if (!gotTheLock) {
    app.quit();
} else {
    app.on('second-instance', (event, commandLine, workingDirectory) => {
        // 聚焦主窗口
        if (mainWindow) {
            if (mainWindow.isMinimized()) mainWindow.restore();
            mainWindow.focus();
        }
    });
}

// GPU 进程崩溃
app.on('gpu-process-crashed', (event, killed) => {
    console.log('GPU process crashed', killed);
});

// 渲染进程崩溃
app.on('render-process-gone', (event, webContents, details) => {
    console.log('Renderer crashed:', details.reason);
});

// ==================== 窗口创建 ====================

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 1200,
        height: 800,
        minWidth: 800,
        minHeight: 600,
        title: 'My Electron App',
        icon: path.join(__dirname, 'assets/icon.png'),

        // 窗口样式
        frame: true,           // 显示标题栏
        titleBarStyle: 'hidden', // macOS: 隐藏标题栏但保留交通灯
        transparent: false,
        backgroundColor: '#ffffff',

        // Web 首选项
        webPreferences: {
            nodeIntegration: false,        // 禁用 Node.js 集成
            contextIsolation: true,        // 启用上下文隔离
            sandbox: true,                 // 启用沙箱
            preload: path.join(__dirname, 'preload.js'),
            webSecurity: true,             // 启用同源策略
            allowRunningInsecureContent: false,
        }
    });

    // 加载页面
    if (process.env.NODE_ENV === 'development') {
        mainWindow.loadURL('http://localhost:3000');
        mainWindow.webContents.openDevTools();
    } else {
        mainWindow.loadFile('dist/index.html');
    }

    // 窗口事件
    mainWindow.on('closed', () => {
        mainWindow = null;
    });

    mainWindow.on('ready-to-show', () => {
        mainWindow.show();
    });

    // 创建菜单
    createMenu();

    // 创建托盘
    createTray();
}

function createMenu() {
    const template = [
        {
            label: 'File',
            submenu: [
                {
                    label: 'New',
                    accelerator: 'CmdOrCtrl+N',
                    click: () => { /* 新建 */ }
                },
                {
                    label: 'Open',
                    accelerator: 'CmdOrCtrl+O',
                    click: async () => {
                        const result = await dialog.showOpenDialog({
                            properties: ['openFile']
                        });
                        if (!result.canceled) {
                            openFile(result.filePaths[0]);
                        }
                    }
                },
                { type: 'separator' },
                { role: 'quit' }
            ]
        },
        {
            label: 'Edit',
            submenu: [
                { role: 'undo' },
                { role: 'redo' },
                { type: 'separator' },
                { role: 'cut' },
                { role: 'copy' },
                { role: 'paste' },
            ]
        },
        {
            label: 'View',
            submenu: [
                { role: 'reload' },
                { role: 'forceReload' },
                { role: 'toggleDevTools' },
                { type: 'separator' },
                { role: 'togglefullscreen' }
            ]
        }
    ];

    const menu = Menu.buildFromTemplate(template);
    Menu.setApplicationMenu(menu);
}

function createTray() {
    tray = new Tray(path.join(__dirname, 'assets/tray-icon.png'));

    const contextMenu = Menu.buildFromTemplate([
        { label: 'Show', click: () => mainWindow.show() },
        { label: 'Quit', click: () => app.quit() }
    ]);

    tray.setToolTip('My Electron App');
    tray.setContextMenu(contextMenu);

    tray.on('click', () => {
        mainWindow.show();
    });
}
```

### 2.2 IPC 通信处理

```javascript
// main.js - IPC 处理

const { ipcMain, dialog, shell, clipboard, nativeImage } = require('electron');
const fs = require('fs').promises;
const path = require('path');

// ==================== invoke/handle 模式 (推荐) ====================

// 文件操作
ipcMain.handle('file:read', async (event, filePath) => {
    try {
        const content = await fs.readFile(filePath, 'utf-8');
        return { success: true, content };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

ipcMain.handle('file:write', async (event, filePath, content) => {
    try {
        await fs.writeFile(filePath, content, 'utf-8');
        return { success: true };
    } catch (error) {
        return { success: false, error: error.message };
    }
});

ipcMain.handle('file:selectFolder', async () => {
    const result = await dialog.showOpenDialog(mainWindow, {
        properties: ['openDirectory', 'createDirectory']
    });
    return result;
});

// 对话框
ipcMain.handle('dialog:showMessage', async (event, options) => {
    return await dialog.showMessageBox(mainWindow, options);
});

ipcMain.handle('dialog:showError', (event, title, content) => {
    dialog.showErrorBox(title, content);
});

// Shell 操作
ipcMain.handle('shell:openExternal', async (event, url) => {
    await shell.openExternal(url);
});

ipcMain.handle('shell:showItemInFolder', (event, filePath) => {
    shell.showItemInFolder(filePath);
});

// 剪贴板
ipcMain.handle('clipboard:writeText', (event, text) => {
    clipboard.writeText(text);
});

ipcMain.handle('clipboard:readText', () => {
    return clipboard.readText();
});

ipcMain.handle('clipboard:writeImage', (event, dataURL) => {
    const image = nativeImage.createFromDataURL(dataURL);
    clipboard.writeImage(image);
});

// 系统信息
ipcMain.handle('system:getInfo', () => {
    return {
        platform: process.platform,
        arch: process.arch,
        version: process.version,
        appVersion: app.getVersion(),
        appPath: app.getAppPath(),
        userData: app.getPath('userData'),
    };
});

// ==================== send/on 模式 (单向) ====================

// 接收渲染进程消息
ipcMain.on('log', (event, message) => {
    console.log('[Renderer]', message);
});

// 发送到渲染进程
function notifyRenderer(channel, data) {
    if (mainWindow && !mainWindow.isDestroyed()) {
        mainWindow.webContents.send(channel, data);
    }
}

// 示例：文件监听
const chokidar = require('chokidar');
let watcher;

ipcMain.handle('file:watch', (event, dirPath) => {
    if (watcher) watcher.close();

    watcher = chokidar.watch(dirPath, {
        persistent: true,
        ignoreInitial: true
    });

    watcher.on('change', (filePath) => {
        notifyRenderer('file:changed', { filePath, type: 'change' });
    });

    watcher.on('add', (filePath) => {
        notifyRenderer('file:changed', { filePath, type: 'add' });
    });

    watcher.on('unlink', (filePath) => {
        notifyRenderer('file:changed', { filePath, type: 'delete' });
    });

    return { success: true };
});
```

---

## 3. Preload Script (预加载脚本)

### 3.1 contextBridge 安全暴露 API

```javascript
// preload.js

const { contextBridge, ipcRenderer } = require('electron');

// 安全地暴露 API 到渲染进程
contextBridge.exposeInMainWorld('electronAPI', {
    // ==================== 文件操作 ====================
    file: {
        read: (filePath) => ipcRenderer.invoke('file:read', filePath),
        write: (filePath, content) => ipcRenderer.invoke('file:write', filePath, content),
        selectFolder: () => ipcRenderer.invoke('file:selectFolder'),

        // 监听文件变化
        watch: (dirPath) => ipcRenderer.invoke('file:watch', dirPath),
        onChanged: (callback) => {
            const handler = (event, data) => callback(data);
            ipcRenderer.on('file:changed', handler);
            // 返回取消订阅函数
            return () => ipcRenderer.removeListener('file:changed', handler);
        }
    },

    // ==================== 对话框 ====================
    dialog: {
        showMessage: (options) => ipcRenderer.invoke('dialog:showMessage', options),
        showError: (title, content) => ipcRenderer.invoke('dialog:showError', title, content),
    },

    // ==================== Shell ====================
    shell: {
        openExternal: (url) => ipcRenderer.invoke('shell:openExternal', url),
        showInFolder: (path) => ipcRenderer.invoke('shell:showItemInFolder', path),
    },

    // ==================== 剪贴板 ====================
    clipboard: {
        writeText: (text) => ipcRenderer.invoke('clipboard:writeText', text),
        readText: () => ipcRenderer.invoke('clipboard:readText'),
        writeImage: (dataURL) => ipcRenderer.invoke('clipboard:writeImage', dataURL),
    },

    // ==================== 系统信息 ====================
    system: {
        getInfo: () => ipcRenderer.invoke('system:getInfo'),
    },

    // ==================== 日志 ====================
    log: (message) => ipcRenderer.send('log', message),

    // ==================== 版本信息 ====================
    versions: {
        node: process.versions.node,
        chrome: process.versions.chrome,
        electron: process.versions.electron,
    }
});

// 注意：不要暴露完整的 ipcRenderer
// 错误示例：contextBridge.exposeInMainWorld('ipc', ipcRenderer);
// 这会让渲染进程可以调用任意 IPC channel
```

### 3.2 渲染进程使用

```javascript
// renderer.js

// 使用暴露的 API
async function loadFile() {
    const result = await window.electronAPI.file.selectFolder();
    if (!result.canceled) {
        const folderPath = result.filePaths[0];
        console.log('Selected folder:', folderPath);

        // 监听文件变化
        const unsubscribe = window.electronAPI.file.onChanged((data) => {
            console.log('File changed:', data);
            updateFileList();
        });

        // 开始监听
        await window.electronAPI.file.watch(folderPath);

        // 清理时取消订阅
        // unsubscribe();
    }
}

async function saveFile(filePath, content) {
    const result = await window.electronAPI.file.write(filePath, content);
    if (result.success) {
        await window.electronAPI.dialog.showMessage({
            type: 'info',
            title: 'Saved',
            message: 'File saved successfully'
        });
    } else {
        window.electronAPI.dialog.showError('Error', result.error);
    }
}

async function openExternalLink(url) {
    await window.electronAPI.shell.openExternal(url);
}

// 获取系统信息
async function showSystemInfo() {
    const info = await window.electronAPI.system.getInfo();
    console.log('Platform:', info.platform);
    console.log('App Version:', info.appVersion);

    // 版本信息 (同步)
    console.log('Node:', window.electronAPI.versions.node);
    console.log('Chrome:', window.electronAPI.versions.chrome);
    console.log('Electron:', window.electronAPI.versions.electron);
}
```

---

## 4. 安全模型

### 4.1 安全配置演进

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Electron 安全配置演进                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  旧版 (不安全):                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  webPreferences: {                                                  │   │
│  │      nodeIntegration: true,       // 渲染进程可用 Node.js           │   │
│  │      contextIsolation: false,     // JS 上下文不隔离                │   │
│  │      // preload 脚本可直接污染 window                               │   │
│  │  }                                                                  │   │
│  │                                                                     │   │
│  │  风险: XSS 攻击可获得系统完全控制权                                 │   │
│  │  // 渲染进程中                                                      │   │
│  │  require('child_process').exec('rm -rf /');  // 灾难性后果          │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  新版 (安全):                                                               │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  webPreferences: {                                                  │   │
│  │      nodeIntegration: false,      // 禁用 Node.js                   │   │
│  │      contextIsolation: true,      // 隔离 JS 上下文                 │   │
│  │      sandbox: true,               // 启用沙箱                       │   │
│  │      preload: 'preload.js',       // 安全的预加载脚本               │   │
│  │      webSecurity: true,           // 启用同源策略                   │   │
│  │  }                                                                  │   │
│  │                                                                     │   │
│  │  安全边界: preload 脚本通过 contextBridge 暴露有限 API              │   │
│  │  XSS 攻击只能调用暴露的白名单 API，无法访问 Node.js                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 安全最佳实践

```javascript
// 安全检查清单

// 1. 禁用 nodeIntegration
webPreferences: {
    nodeIntegration: false,
}

// 2. 启用 contextIsolation
webPreferences: {
    contextIsolation: true,
}

// 3. 启用沙箱
webPreferences: {
    sandbox: true,
}

// 4. 验证 IPC 消息
ipcMain.handle('file:read', async (event, filePath) => {
    // 验证路径，防止路径遍历攻击
    const safePath = path.resolve(allowedDir, filePath);
    if (!safePath.startsWith(allowedDir)) {
        throw new Error('Access denied');
    }
    // ...
});

// 5. 限制导航
mainWindow.webContents.on('will-navigate', (event, url) => {
    const parsedUrl = new URL(url);
    if (parsedUrl.origin !== expectedOrigin) {
        event.preventDefault();
    }
});

// 6. 禁止新窗口打开不信任的 URL
mainWindow.webContents.setWindowOpenHandler(({ url }) => {
    if (isTrustedUrl(url)) {
        return { action: 'allow' };
    }
    shell.openExternal(url);  // 在系统浏览器中打开
    return { action: 'deny' };
});

// 7. 禁用 remote 模块 (已废弃)
// Electron 14+ 已移除

// 8. 设置 CSP
mainWindow.webContents.session.webRequest.onHeadersReceived((details, callback) => {
    callback({
        responseHeaders: {
            ...details.responseHeaders,
            'Content-Security-Policy': [
                "default-src 'self'",
                "script-src 'self'",
                "style-src 'self' 'unsafe-inline'"
            ].join('; ')
        }
    });
});
```

---

## 5. 原生模块集成

### 5.1 N-API 原生模块

```cpp
// native_module.cc - 使用 N-API 编写原生模块

#include <napi.h>

// 同步函数
Napi::String HelloWorld(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, "Hello from C++!");
}

// 带参数的函数
Napi::Number Add(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected two numbers").ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }

    double a = info[0].As<Napi::Number>().DoubleValue();
    double b = info[1].As<Napi::Number>().DoubleValue();

    return Napi::Number::New(env, a + b);
}

// 异步函数
class AsyncWorker : public Napi::AsyncWorker {
public:
    AsyncWorker(Napi::Function& callback, std::string input)
        : Napi::AsyncWorker(callback), input_(input) {}

    void Execute() override {
        // 在工作线程执行
        result_ = DoHeavyWork(input_);
    }

    void OnOK() override {
        // 回到主线程
        Callback().Call({Env().Null(), Napi::String::New(Env(), result_)});
    }

private:
    std::string input_;
    std::string result_;
};

Napi::Value DoAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string input = info[0].As<Napi::String>().Utf8Value();
    Napi::Function callback = info[1].As<Napi::Function>();

    AsyncWorker* worker = new AsyncWorker(callback, input);
    worker->Queue();

    return env.Undefined();
}

// 模块初始化
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("hello", Napi::Function::New(env, HelloWorld));
    exports.Set("add", Napi::Function::New(env, Add));
    exports.Set("doAsync", Napi::Function::New(env, DoAsync));
    return exports;
}

NODE_API_MODULE(native_module, Init)
```

```javascript
// 使用原生模块
const nativeModule = require('./build/Release/native_module.node');

console.log(nativeModule.hello());  // "Hello from C++!"
console.log(nativeModule.add(1, 2));  // 3

nativeModule.doAsync('input', (err, result) => {
    if (err) console.error(err);
    else console.log(result);
});
```

### 5.2 使用 ffi-napi 调用系统 DLL

```javascript
// 调用 Windows DLL

const ffi = require('ffi-napi');
const ref = require('ref-napi');

// 加载 user32.dll
const user32 = ffi.Library('user32', {
    'MessageBoxW': ['int', ['pointer', 'pointer', 'pointer', 'int']]
});

// 调用 MessageBox
const MB_OK = 0;
const MB_ICONINFORMATION = 0x40;

function showMessageBox(title, message) {
    user32.MessageBoxW(
        null,
        Buffer.from(message + '\0', 'ucs2'),
        Buffer.from(title + '\0', 'ucs2'),
        MB_OK | MB_ICONINFORMATION
    );
}

showMessageBox('标题', '这是消息内容');
```

---

## 6. 打包与分发

### 6.1 Electron Builder 配置

```javascript
// package.json
{
    "name": "my-electron-app",
    "version": "1.0.0",
    "main": "main.js",
    "scripts": {
        "start": "electron .",
        "build": "electron-builder"
    },
    "build": {
        "appId": "com.example.myapp",
        "productName": "My App",
        "directories": {
            "output": "dist"
        },
        "files": [
            "main.js",
            "preload.js",
            "dist/**/*",
            "assets/**/*"
        ],
        "win": {
            "target": ["nsis", "portable"],
            "icon": "assets/icon.ico"
        },
        "mac": {
            "target": ["dmg", "zip"],
            "icon": "assets/icon.icns",
            "category": "public.app-category.productivity"
        },
        "linux": {
            "target": ["AppImage", "deb"],
            "icon": "assets/icon.png"
        },
        "nsis": {
            "oneClick": false,
            "allowToChangeInstallationDirectory": true,
            "createDesktopShortcut": true,
            "createStartMenuShortcut": true
        }
    }
}
```

### 6.2 自动更新

```javascript
// main.js - 自动更新

const { autoUpdater } = require('electron-updater');

// 配置更新源
autoUpdater.setFeedURL({
    provider: 'github',
    owner: 'your-username',
    repo: 'your-repo'
});

// 检查更新
app.whenReady().then(() => {
    autoUpdater.checkForUpdatesAndNotify();
});

// 更新事件
autoUpdater.on('update-available', (info) => {
    notifyRenderer('update:available', info);
});

autoUpdater.on('download-progress', (progress) => {
    notifyRenderer('update:progress', progress);
});

autoUpdater.on('update-downloaded', (info) => {
    // 提示用户重启
    dialog.showMessageBox({
        type: 'info',
        title: 'Update Ready',
        message: 'A new version is ready. Restart now?',
        buttons: ['Restart', 'Later']
    }).then((result) => {
        if (result.response === 0) {
            autoUpdater.quitAndInstall();
        }
    });
});

autoUpdater.on('error', (err) => {
    console.error('Update error:', err);
});
```

---

## 7. 性能优化

### 7.1 启动优化

```javascript
// 延迟加载模块
let heavyModule;
function getHeavyModule() {
    if (!heavyModule) {
        heavyModule = require('./heavy-module');
    }
    return heavyModule;
}

// 使用 V8 快照 (编译时生成)
// package.json
{
    "build": {
        "electronCompile": true
    }
}

// 预热浏览器窗口
let hiddenWindow;
app.whenReady().then(() => {
    // 创建隐藏窗口预热
    hiddenWindow = new BrowserWindow({
        show: false,
        webPreferences: { preload: 'preload.js' }
    });
    hiddenWindow.loadFile('preload.html');
});

function showNewWindow() {
    if (hiddenWindow) {
        hiddenWindow.show();
        hiddenWindow = null;
        // 预创建下一个
        createHiddenWindow();
    } else {
        createNewWindow();
    }
}
```

### 7.2 内存优化

```javascript
// 监控内存使用
setInterval(() => {
    const usage = process.memoryUsage();
    console.log('Memory:', {
        rss: Math.round(usage.rss / 1024 / 1024) + 'MB',
        heapTotal: Math.round(usage.heapTotal / 1024 / 1024) + 'MB',
        heapUsed: Math.round(usage.heapUsed / 1024 / 1024) + 'MB',
    });
}, 30000);

// 关闭不需要的窗口
function closeIdleWindows() {
    const windows = BrowserWindow.getAllWindows();
    windows.forEach(win => {
        if (win.isMinimized() && win !== mainWindow) {
            win.close();
        }
    });
}

// 渲染进程内存限制
new BrowserWindow({
    webPreferences: {
        // 限制 V8 堆大小 (MB)
        // additionalArguments: ['--js-flags=--max_old_space_size=512']
    }
});
```

---

## 8. 面试常见问题

### Q1: Electron 的 Main Process 和 Renderer Process 的区别？

**答案：**

| 特性 | Main Process | Renderer Process |
|------|-------------|------------------|
| 数量 | 唯一 | 多个 (每个窗口一个) |
| 职责 | 窗口管理、系统 API | 页面渲染、用户交互 |
| Node.js | 完整访问 | 受限/禁用 |
| Electron API | app, BrowserWindow, Menu | 仅通过 preload 暴露 |
| 崩溃影响 | 整个应用 | 仅当前窗口 |

### Q2: contextIsolation 和 nodeIntegration 的作用？

**答案：**
- **nodeIntegration**：控制渲染进程是否可以使用 Node.js API
  - `true`：可使用 `require()`, `process` 等
  - `false`：禁用，更安全
- **contextIsolation**：隔离 preload 脚本和页面的 JS 上下文
  - `true`：preload 和页面在不同上下文，需要 contextBridge
  - `false`：共享上下文，preload 可直接修改 window

### Q3: 如何在 Electron 中实现安全的 IPC 通信？

**答案：**
1. 使用 `contextBridge.exposeInMainWorld()` 暴露白名单 API
2. 主进程中验证所有 IPC 参数
3. 避免暴露完整的 `ipcRenderer`
4. 使用 `invoke/handle` 而非 `send/on` 获取返回值
5. 限制允许的 channel 列表

### Q4: Electron 应用如何实现自动更新？

**答案：**
1. 使用 `electron-updater` 模块
2. 配置更新源（GitHub Releases、私有服务器）
3. 监听 `update-available`, `download-progress`, `update-downloaded` 事件
4. 下载完成后调用 `quitAndInstall()` 安装更新
5. 需要代码签名才能进行静默更新

### Q5: 如何优化 Electron 应用的启动速度？

**答案：**
1. **延迟加载**：非必要模块按需 require
2. **V8 快照**：使用 electron-mksnapshot 预编译
3. **预热窗口**：提前创建隐藏窗口
4. **减少主进程代码**：初始化时只加载必要模块
5. **使用 asar 打包**：加速文件读取
6. **优化 preload**：减少 preload 脚本体积
