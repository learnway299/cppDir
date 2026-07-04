# 08 - Tauri

## 概述

Tauri 是一个使用 Rust 构建的轻量级跨平台桌面应用框架。它使用系统原生 WebView 而非捆绑 Chromium，显著减小应用体积（通常 5-10MB vs Electron 的 150MB+）。

---

## 1. Tauri 架构

### 1.1 核心架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Tauri 架构                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                       Tauri 应用程序                                 │   │
│  │                                                                     │   │
│  │  ┌───────────────────────┐     ┌───────────────────────────────┐   │   │
│  │  │    Rust 后端 (Core)    │     │      Web 前端 (Webview)        │   │   │
│  │  │                       │     │                               │   │   │
│  │  │  ┌─────────────────┐  │     │  ┌─────────────────────────┐  │   │   │
│  │  │  │  Tauri Runtime  │  │     │  │    任意前端框架         │  │   │   │
│  │  │  │  - 窗口管理     │  │ IPC │  │  - React / Vue / Svelte │  │   │   │
│  │  │  │  - 系统 API     │◄─┼─────┼─►│  - 纯 HTML/CSS/JS       │  │   │   │
│  │  │  │  - 文件系统     │  │     │  │  - Angular / Solid      │  │   │   │
│  │  │  │  - HTTP 客户端  │  │     │  └─────────────────────────┘  │   │   │
│  │  │  └─────────────────┘  │     │                               │   │   │
│  │  │                       │     │  ┌─────────────────────────┐  │   │   │
│  │  │  ┌─────────────────┐  │     │  │    @tauri-apps/api      │  │   │   │
│  │  │  │  自定义 Commands │  │     │  │   (JS/TS 绑定库)        │  │   │   │
│  │  │  │  (业务逻辑)     │  │     │  └─────────────────────────┘  │   │   │
│  │  │  └─────────────────┘  │     │                               │   │   │
│  │  └───────────────────────┘     └───────────────────────────────┘   │   │
│  │            │                                │                       │   │
│  │            └────────────┬───────────────────┘                       │   │
│  │                         ▼                                           │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                   系统原生 WebView                          │   │   │
│  │  │  Windows: WebView2 (Edge/Chromium)                         │   │   │
│  │  │  macOS: WKWebView (WebKit)                                 │   │   │
│  │  │  Linux: WebKitGTK                                          │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Tauri vs Electron 对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Tauri vs Electron 对比                                 │
├─────────────────────┬─────────────────────┬─────────────────────────────────┤
│        特性         │       Tauri         │           Electron              │
├─────────────────────┼─────────────────────┼─────────────────────────────────┤
│ 后端语言            │ Rust                │ Node.js (JavaScript)            │
│ 浏览器引擎          │ 系统 WebView        │ 捆绑 Chromium                   │
│ 包体积              │ ~5-10 MB            │ ~150+ MB                        │
│ 内存占用            │ 较低                │ 较高                            │
│ 跨平台一致性        │ 中等 (不同 WebView)  │ 高 (同一 Chromium)              │
│ 安全性              │ 高 (Rust + 沙箱)    │ 中等                            │
│ 性能                │ 高 (Rust 后端)      │ 中等 (JS 后端)                  │
│ 生态系统            │ 较新，成长中        │ 成熟，丰富                      │
│ 学习曲线            │ 较陡 (需要 Rust)    │ 平缓 (全 JS)                    │
│ 自动更新            │ 内置支持            │ 需要 electron-updater           │
│ 系统托盘            │ 内置支持            │ 需要额外配置                    │
└─────────────────────┴─────────────────────┴─────────────────────────────────┘
```

---

## 2. 项目结构

### 2.1 典型项目结构

```
my-tauri-app/
├── src/                      # 前端源码
│   ├── index.html
│   ├── main.js
│   └── styles.css
│
├── src-tauri/                # Rust 后端
│   ├── Cargo.toml            # Rust 依赖配置
│   ├── build.rs              # 构建脚本
│   ├── tauri.conf.json       # Tauri 配置
│   ├── icons/                # 应用图标
│   └── src/
│       ├── main.rs           # 入口点
│       ├── lib.rs            # 库代码
│       └── commands.rs       # 自定义命令
│
├── package.json              # 前端依赖
└── vite.config.js            # 前端构建配置 (或其他)
```

### 2.2 配置文件

```json
// src-tauri/tauri.conf.json
{
  "$schema": "https://schema.tauri.app/config/2",
  "productName": "My Tauri App",
  "version": "1.0.0",
  "identifier": "com.example.myapp",
  "build": {
    "beforeDevCommand": "npm run dev",
    "devUrl": "http://localhost:3000",
    "beforeBuildCommand": "npm run build",
    "frontendDist": "../dist"
  },
  "app": {
    "windows": [
      {
        "title": "My Tauri App",
        "width": 1200,
        "height": 800,
        "minWidth": 800,
        "minHeight": 600,
        "resizable": true,
        "fullscreen": false,
        "decorations": true,
        "transparent": false
      }
    ],
    "security": {
      "csp": "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'",
      "dangerousDisableAssetCspModification": false
    },
    "trayIcon": {
      "iconPath": "icons/tray.png",
      "iconAsTemplate": true
    }
  },
  "bundle": {
    "active": true,
    "targets": ["msi", "dmg", "deb"],
    "icon": ["icons/icon.ico", "icons/icon.icns", "icons/icon.png"],
    "windows": {
      "wix": {
        "language": ["en-US", "zh-CN"]
      }
    }
  },
  "plugins": {
    "updater": {
      "active": true,
      "endpoints": ["https://releases.example.com/{{target}}/{{arch}}/{{current_version}}"],
      "pubkey": "..."
    }
  }
}
```

---

## 3. Rust 后端开发

### 3.1 基础命令定义

```rust
// src-tauri/src/main.rs

#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use tauri::Manager;

// 定义命令 (同步)
#[tauri::command]
fn greet(name: &str) -> String {
    format!("Hello, {}! Welcome to Tauri!", name)
}

// 带复杂返回值的命令
#[tauri::command]
fn get_system_info() -> serde_json::Value {
    serde_json::json!({
        "os": std::env::consts::OS,
        "arch": std::env::consts::ARCH,
        "family": std::env::consts::FAMILY,
    })
}

// 异步命令
#[tauri::command]
async fn fetch_data(url: String) -> Result<String, String> {
    match reqwest::get(&url).await {
        Ok(response) => {
            match response.text().await {
                Ok(text) => Ok(text),
                Err(e) => Err(e.to_string()),
            }
        }
        Err(e) => Err(e.to_string()),
    }
}

// 可失败的命令
#[tauri::command]
fn read_file(path: String) -> Result<String, String> {
    std::fs::read_to_string(&path)
        .map_err(|e| format!("Failed to read file: {}", e))
}

#[tauri::command]
fn write_file(path: String, content: String) -> Result<(), String> {
    std::fs::write(&path, &content)
        .map_err(|e| format!("Failed to write file: {}", e))
}

fn main() {
    tauri::Builder::default()
        // 注册命令
        .invoke_handler(tauri::generate_handler![
            greet,
            get_system_info,
            fetch_data,
            read_file,
            write_file,
        ])
        // 窗口事件处理
        .on_window_event(|window, event| {
            match event {
                tauri::WindowEvent::CloseRequested { api, .. } => {
                    // 可以阻止关闭
                    // api.prevent_close();
                    println!("Window close requested");
                }
                tauri::WindowEvent::Focused(focused) => {
                    println!("Window focused: {}", focused);
                }
                _ => {}
            }
        })
        // 设置完成回调
        .setup(|app| {
            let window = app.get_webview_window("main").unwrap();
            window.set_title("My App - Ready").unwrap();

            // 可以启动后台任务
            let app_handle = app.handle().clone();
            std::thread::spawn(move || {
                // 后台任务
            });

            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
```

### 3.2 状态管理

```rust
// src-tauri/src/state.rs

use std::sync::Mutex;
use tauri::State;

// 应用状态
pub struct AppState {
    pub counter: Mutex<i32>,
    pub config: Mutex<AppConfig>,
}

#[derive(Default, Clone, serde::Serialize, serde::Deserialize)]
pub struct AppConfig {
    pub theme: String,
    pub language: String,
    pub auto_save: bool,
}

impl Default for AppState {
    fn default() -> Self {
        Self {
            counter: Mutex::new(0),
            config: Mutex::new(AppConfig::default()),
        }
    }
}

// 使用状态的命令
#[tauri::command]
fn increment(state: State<AppState>) -> i32 {
    let mut counter = state.counter.lock().unwrap();
    *counter += 1;
    *counter
}

#[tauri::command]
fn get_counter(state: State<AppState>) -> i32 {
    *state.counter.lock().unwrap()
}

#[tauri::command]
fn get_config(state: State<AppState>) -> AppConfig {
    state.config.lock().unwrap().clone()
}

#[tauri::command]
fn set_config(state: State<AppState>, config: AppConfig) {
    *state.config.lock().unwrap() = config;
}

// 在 main.rs 中注册状态
fn main() {
    tauri::Builder::default()
        .manage(AppState::default())  // 注册状态
        .invoke_handler(tauri::generate_handler![
            increment,
            get_counter,
            get_config,
            set_config,
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
```

### 3.3 事件系统

```rust
// Rust 端发送事件到前端

use tauri::{AppHandle, Emitter};

#[tauri::command]
fn start_task(app: AppHandle) {
    std::thread::spawn(move || {
        for i in 0..100 {
            std::thread::sleep(std::time::Duration::from_millis(100));

            // 发送进度事件
            app.emit("task-progress", serde_json::json!({
                "progress": i,
                "message": format!("Processing step {}", i)
            })).unwrap();
        }

        // 发送完成事件
        app.emit("task-complete", serde_json::json!({
            "success": true,
            "result": "Task completed!"
        })).unwrap();
    });
}

// 监听前端事件
fn setup_event_listeners(app: &tauri::App) {
    let handle = app.handle().clone();

    app.listen("user-action", move |event| {
        println!("Received event: {:?}", event.payload());

        // 处理事件并响应
        handle.emit("action-response", "Action processed").unwrap();
    });
}
```

---

## 4. 前端开发

### 4.1 使用 @tauri-apps/api

```typescript
// 使用 Tauri API

import { invoke } from '@tauri-apps/api/core';
import { listen, emit } from '@tauri-apps/api/event';
import { open, save } from '@tauri-apps/plugin-dialog';
import { readTextFile, writeTextFile } from '@tauri-apps/plugin-fs';
import { appDataDir, join } from '@tauri-apps/api/path';

// 调用 Rust 命令
async function callRustCommand() {
    // 简单调用
    const greeting = await invoke<string>('greet', { name: 'World' });
    console.log(greeting);

    // 获取系统信息
    const sysInfo = await invoke<object>('get_system_info');
    console.log('System:', sysInfo);

    // 带错误处理
    try {
        const content = await invoke<string>('read_file', {
            path: '/path/to/file.txt'
        });
        console.log('File content:', content);
    } catch (error) {
        console.error('Error:', error);
    }
}

// 监听 Rust 事件
async function setupEventListeners() {
    // 监听进度事件
    const unlisten = await listen<{ progress: number; message: string }>(
        'task-progress',
        (event) => {
            console.log('Progress:', event.payload.progress);
            updateProgressBar(event.payload.progress);
        }
    );

    // 监听完成事件
    await listen('task-complete', (event) => {
        console.log('Task completed:', event.payload);
    });

    // 取消监听
    // unlisten();
}

// 发送事件到 Rust
async function sendEventToRust() {
    await emit('user-action', { type: 'click', data: 'button1' });
}

// 文件对话框
async function selectFile() {
    const selected = await open({
        multiple: false,
        filters: [{
            name: 'Text',
            extensions: ['txt', 'md']
        }]
    });

    if (selected) {
        const content = await readTextFile(selected);
        return content;
    }
}

async function saveFile(content: string) {
    const filePath = await save({
        filters: [{
            name: 'Text',
            extensions: ['txt']
        }]
    });

    if (filePath) {
        await writeTextFile(filePath, content);
    }
}

// 获取应用路径
async function getAppPaths() {
    const dataDir = await appDataDir();
    const configPath = await join(dataDir, 'config.json');
    console.log('Config path:', configPath);
}
```

### 4.2 React + Tauri 示例

```tsx
// App.tsx

import { useState, useEffect } from 'react';
import { invoke } from '@tauri-apps/api/core';
import { listen } from '@tauri-apps/api/event';

interface SystemInfo {
    os: string;
    arch: string;
    family: string;
}

function App() {
    const [greeting, setGreeting] = useState('');
    const [sysInfo, setSysInfo] = useState<SystemInfo | null>(null);
    const [counter, setCounter] = useState(0);
    const [progress, setProgress] = useState(0);

    useEffect(() => {
        // 加载系统信息
        invoke<SystemInfo>('get_system_info').then(setSysInfo);

        // 加载计数器
        invoke<number>('get_counter').then(setCounter);

        // 监听进度事件
        const unlisten = listen<{ progress: number }>('task-progress', (e) => {
            setProgress(e.payload.progress);
        });

        return () => {
            unlisten.then(fn => fn());
        };
    }, []);

    const handleGreet = async () => {
        const result = await invoke<string>('greet', { name: 'Tauri User' });
        setGreeting(result);
    };

    const handleIncrement = async () => {
        const newValue = await invoke<number>('increment');
        setCounter(newValue);
    };

    const handleStartTask = async () => {
        setProgress(0);
        await invoke('start_task');
    };

    return (
        <div className="container">
            <h1>Tauri + React</h1>

            {sysInfo && (
                <p>Running on {sysInfo.os} ({sysInfo.arch})</p>
            )}

            <div>
                <button onClick={handleGreet}>Greet</button>
                {greeting && <p>{greeting}</p>}
            </div>

            <div>
                <p>Counter: {counter}</p>
                <button onClick={handleIncrement}>Increment</button>
            </div>

            <div>
                <button onClick={handleStartTask}>Start Task</button>
                <progress value={progress} max={100} />
            </div>
        </div>
    );
}

export default App;
```

---

## 5. 高级功能

### 5.1 系统托盘

```rust
// src-tauri/src/tray.rs

use tauri::{
    menu::{Menu, MenuItem},
    tray::{TrayIcon, TrayIconBuilder},
    Manager, Runtime,
};

pub fn create_tray<R: Runtime>(app: &tauri::App<R>) -> tauri::Result<TrayIcon<R>> {
    // 创建菜单
    let show = MenuItem::with_id(app, "show", "Show", true, None::<&str>)?;
    let hide = MenuItem::with_id(app, "hide", "Hide", true, None::<&str>)?;
    let quit = MenuItem::with_id(app, "quit", "Quit", true, None::<&str>)?;

    let menu = Menu::with_items(app, &[&show, &hide, &quit])?;

    // 创建托盘图标
    let tray = TrayIconBuilder::new()
        .icon(app.default_window_icon().unwrap().clone())
        .tooltip("My Tauri App")
        .menu(&menu)
        .on_menu_event(|app, event| {
            match event.id.as_ref() {
                "show" => {
                    if let Some(window) = app.get_webview_window("main") {
                        window.show().unwrap();
                        window.set_focus().unwrap();
                    }
                }
                "hide" => {
                    if let Some(window) = app.get_webview_window("main") {
                        window.hide().unwrap();
                    }
                }
                "quit" => {
                    app.exit(0);
                }
                _ => {}
            }
        })
        .on_tray_icon_event(|tray, event| {
            use tauri::tray::TrayIconEvent;
            match event {
                TrayIconEvent::Click { button, .. } => {
                    if button == tauri::tray::MouseButton::Left {
                        if let Some(window) = tray.app_handle().get_webview_window("main") {
                            window.show().unwrap();
                            window.set_focus().unwrap();
                        }
                    }
                }
                _ => {}
            }
        })
        .build(app)?;

    Ok(tray)
}
```

### 5.2 自动更新

```rust
// Cargo.toml
[dependencies]
tauri-plugin-updater = "2"

// main.rs
fn main() {
    tauri::Builder::default()
        .plugin(tauri_plugin_updater::Builder::new().build())
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
```

```typescript
// 前端检查更新
import { check } from '@tauri-apps/plugin-updater';
import { relaunch } from '@tauri-apps/plugin-process';

async function checkForUpdates() {
    const update = await check();

    if (update) {
        console.log(`Found update: ${update.version}`);

        // 下载更新
        await update.downloadAndInstall((event) => {
            switch (event.event) {
                case 'Started':
                    console.log('Download started');
                    break;
                case 'Progress':
                    console.log(`Downloaded ${event.data.chunkLength} bytes`);
                    break;
                case 'Finished':
                    console.log('Download finished');
                    break;
            }
        });

        // 重启应用
        await relaunch();
    }
}
```

### 5.3 窗口管理

```rust
// 创建多窗口
use tauri::{WebviewUrl, WebviewWindowBuilder};

#[tauri::command]
async fn open_settings(app: tauri::AppHandle) -> Result<(), String> {
    let _settings_window = WebviewWindowBuilder::new(
        &app,
        "settings",
        WebviewUrl::App("settings.html".into()),
    )
    .title("Settings")
    .inner_size(600.0, 400.0)
    .resizable(false)
    .center()
    .build()
    .map_err(|e| e.to_string())?;

    Ok(())
}

// 窗口间通信
#[tauri::command]
fn send_to_window(app: tauri::AppHandle, window_label: String, message: String) {
    if let Some(window) = app.get_webview_window(&window_label) {
        window.emit("message", message).unwrap();
    }
}
```

```typescript
// 前端窗口操作
import { getCurrentWebviewWindow, WebviewWindow } from '@tauri-apps/api/webviewWindow';

// 获取当前窗口
const currentWindow = getCurrentWebviewWindow();

// 窗口操作
await currentWindow.minimize();
await currentWindow.maximize();
await currentWindow.unmaximize();
await currentWindow.toggleMaximize();
await currentWindow.close();

// 设置标题
await currentWindow.setTitle('New Title');

// 全屏
await currentWindow.setFullscreen(true);

// 创建新窗口
const newWindow = new WebviewWindow('new-window', {
    url: '/another-page',
    title: 'New Window',
    width: 800,
    height: 600,
});

// 监听窗口事件
newWindow.once('tauri://created', () => {
    console.log('Window created');
});

newWindow.once('tauri://error', (e) => {
    console.error('Window creation error:', e);
});
```

---

## 6. 安全特性

### 6.1 命令权限

```json
// src-tauri/capabilities/default.json
{
  "$schema": "https://schema.tauri.app/config/2/capability",
  "identifier": "default",
  "description": "Default capability",
  "windows": ["main"],
  "permissions": [
    "core:default",
    "fs:allow-read-text-file",
    "fs:allow-write-text-file",
    "dialog:allow-open",
    "dialog:allow-save",
    {
      "identifier": "fs:scope",
      "allow": [
        "$APPDATA/*",
        "$DOCUMENT/*"
      ]
    }
  ]
}
```

### 6.2 CSP 配置

```json
// tauri.conf.json
{
  "app": {
    "security": {
      "csp": {
        "default-src": "'self'",
        "script-src": "'self'",
        "style-src": "'self' 'unsafe-inline'",
        "img-src": "'self' data: https:",
        "connect-src": "'self' https://api.example.com"
      }
    }
  }
}
```

---

## 7. 面试常见问题

### Q1: Tauri 相比 Electron 有什么优势和劣势？

**答案：**

**优势：**
- 包体积小（5-10MB vs 150MB+）
- 内存占用低
- Rust 后端性能高、安全性好
- 内置自动更新、系统托盘等功能
- 更强的安全模型（权限系统）

**劣势：**
- 不同平台 WebView 可能有差异
- 需要学习 Rust
- 生态系统相对较新
- Windows 7 支持需要额外配置

### Q2: Tauri 的 IPC 机制是如何工作的？

**答案：**
1. 前端调用 `invoke()` 函数
2. 请求通过 WebView 的 postMessage 发送
3. Tauri 核心接收并路由到对应的 Rust 命令
4. Rust 命令执行并返回结果
5. 结果序列化为 JSON 返回给前端
6. `invoke()` 的 Promise 解析为结果

### Q3: 如何在 Tauri 中实现跨窗口通信？

**答案：**
1. **通过事件系统**：
   - Rust 端使用 `app.emit()` 广播
   - 前端使用 `listen()` 监听
2. **通过命令**：
   - 定义接收目标窗口 label 的命令
   - 使用 `app.get_webview_window()` 获取目标窗口
   - 向目标窗口发送事件

### Q4: Tauri 的权限系统如何工作？

**答案：**
- 在 `capabilities/*.json` 中定义权限
- 每个窗口关联一个或多个 capability
- capability 包含允许的权限列表
- 权限可以限制文件系统访问范围
- 未授权的操作会被拒绝

### Q5: 如何处理 Tauri 应用在不同平台 WebView 的差异？

**答案：**
1. 使用 CSS 前缀和 polyfill 处理样式差异
2. 使用 feature detection 而非 browser detection
3. 测试所有目标平台
4. 对于关键功能，考虑使用 Rust 后端实现
5. 使用 Tauri 的条件编译处理平台特定代码：
   ```rust
   #[cfg(target_os = "windows")]
   fn platform_specific() { /* Windows 代码 */ }

   #[cfg(target_os = "macos")]
   fn platform_specific() { /* macOS 代码 */ }
   ```
