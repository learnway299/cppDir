# 12 - CEF 实战场景考察

---

## 场景 1：把 CEF 嵌进现有 Win32/Qt/wx 主窗

1. 主程序入口先 `CefExecuteProcess`  
2. `CefInitialize` + 资源路径  
3. 父窗客户区创建 Browser  
4. 宿主泵 `CefDoMessageLoopWork` 或官方集成  
5. resize/DPI/focus 同步  
6. 退出按关闭顺序  

**翻车**：忘记 ExecuteProcess；pak 路径错；尺寸 0。

---

## 场景 2：白屏 / 黑屏

| 检查 | 动作 |
|------|------|
| 子进程 | 路径、杀毒拦截、任务管理器有无 renderer |
| 资源 | icudtl、locales、pak |
| 循环 | 是否在泵消息 |
| GPU | 兼容模式/开关对比 |
| 日志 | cef log_severity |

---

## 场景 3：设计登录后 JS 调 C++ 打开原生对话框

1. 定义消息 `app.openDialog`  
2. Renderer 注入桥  
3. Browser 收消息 → UI 线程弹窗  
4. 结果 IPC/JS 回调  
5. 鉴权：仅本地页面可调  

---

## 场景 4：退出仍有残留进程

1. 日志是否走到 `OnBeforeClose`  
2. 是否还有 `CefRefPtr` 持有  
3. 是否跳过 Shutdown  
4. 多 Browser 是否都关  

---

## 场景 5：升级 Chromium / CEF 大版本

1. 读 breaking changes  
2. 回归：启动、嵌窗、桥、下载、证书、OSR  
3. 性能/内存基线对比  
4. 灰度发布  

---

## 场景 6：内存投诉

1. 空白页基线 vs 业务页  
2. 标签/Browser 数量  
3. 关页是否释放  
4. 页面堆分析（DevTools）  
5. 升级引擎  

---

## 场景 7：选型口述

| 需求 | 倾向 |
|------|------|
| 已有大型 C++ 客户端嵌网页 | CEF |
| 全新技术栈 Web 做桌面 | Electron/Tauri |
| 仅 Windows、要薄 | WebView2 |
| Qt 全家桶 | Qt WebEngine |

---

## 场景 8：安全评审

- 沙箱开  
- 桥白名单  
- 禁随意 file  
- 调试端口不暴露  
- 升级 SLA  

---

## 场景速查

| 场景 | 关键词 |
|------|--------|
| 嵌入 | ExecuteProcess、WindowInfo、泵 |
| 白屏 | 资源、子进程、日志 |
| 桥 | V8 + IPC + 鉴权 |
| 退出 | BeforeClose、Shutdown |
| 升级 | 回归清单 |
| 选型 | 宿主语言与平台 |
