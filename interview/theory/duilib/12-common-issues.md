# 常见问题与调试

## 概述

Duilib 开发中常遇到的问题涵盖内存泄漏、控件查找失败、消息丢失、DPI 适配、高版本 Windows 兼容性等。本节汇总实际项目中的高频问题及其排查方法，同时介绍 Duilib 调试的常用技巧和工具。

---

## 1. 内存泄漏

### 常见泄漏场景

```cpp
// ❌ 场景1：动态创建控件但未加入容器
void CreateFloatingTip() {
    CLabelUI* pTip = new CLabelUI;
    pTip->SetText(_T("提示信息"));
    // 忘记 Add 到容器，也没有手动 delete
    // → 内存泄漏
}

// ❌ 场景2：Remove 控件时指定不删除，但后续忘记管理
pContainer->Remove(pChild, false);  // false = 不 delete
// pChild 从树上摘下来了，但没人 delete 它

// ❌ 场景3：PostMessage 传递堆对象，消息未被处理
void WorkerFunc() {
    CString* pData = new CString(_T("result"));
    ::PostMessage(m_hWnd, WM_USER_DATA, (WPARAM)pData, 0);
    // 如果窗口已关闭或消息被丢弃 → 泄漏
}
```

### 排查方法

```cpp
// 方法1：使用 CRT 内存泄漏检测
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // 程序退出时自动报告泄漏位置
    // ...
}

// 方法2：重写 new/delete 跟踪分配
// 在 Debug 模式下记录每次 new 的调用栈

// 方法3：Visual Studio 诊断工具
// Debug → Windows → Diagnostic Tools → Memory Usage
// 拍摄快照对比，找出持续增长的对象
```

### GDI 对象泄漏

```cpp
// GDI 泄漏比内存泄漏更隐蔽，程序不会崩溃但会逐渐异常

// 检测：
DWORD nGdiCount = ::GetGuiResources(::GetCurrentProcess(), GR_GDIOBJECTS);
// 正常应用通常 < 1000，持续增长说明有泄漏

// 常见原因：
// 1. CreateFont/CreatePen/CreateBrush 后忘记 DeleteObject
// 2. SelectObject 后忘记恢复旧对象再 Delete 新对象
// 3. CreateCompatibleDC 后忘记 DeleteDC

// 修复模板：
void SafeDraw(HDC hDC) {
    HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

    // 绘制...
    ::MoveToEx(hDC, 0, 0, NULL);
    ::LineTo(hDC, 100, 100);

    ::SelectObject(hDC, hOldPen);  // 恢复
    ::DeleteObject(hPen);           // 释放
}
```

---

## 2. 控件查找失败

### FindControl 返回 NULL 的常见原因

```cpp
CControlUI* pBtn = m_pm.FindControl(_T("btnSubmit"));
// pBtn == NULL，为什么？
```

| 原因 | 排查方法 |
|------|---------|
| 控件名拼写错误 | 检查 XML 中的 name 属性，注意大小写 |
| XML 未正确加载 | 检查 CDialogBuilder::Create 返回值 |
| 控件在 Include 文件中 | 确认 Include 路径正确，文件存在 |
| AttachDialog 未调用 | 检查初始化流程 |
| 控件在 TabLayout 的非活动页 | TabLayout 隐藏页中的控件仍可找到 |
| 查找时机过早 | 确保在 AttachDialog 之后查找 |

```cpp
// 调试技巧：遍历控件树打印所有控件名
void DebugPrintControlTree(CControlUI* pControl, int depth = 0) {
    if (!pControl) return;

    CDuiString indent(_T(' '), depth * 2);
    OutputDebugString(indent + pControl->GetClass()
                      + _T(" name=") + pControl->GetName() + _T("\n"));

    IContainerUI* pContainer = (IContainerUI*)pControl->GetInterface(_T("IContainer"));
    if (pContainer) {
        for (int i = 0; i < pContainer->GetCount(); i++) {
            DebugPrintControlTree(pContainer->GetItemAt(i), depth + 1);
        }
    }
}

// 调用
DebugPrintControlTree(m_pm.GetRoot());
```

---

## 3. 消息相关问题

### Notify 收不到通知

```cpp
// 检查清单：
// 1. 是否调用了 AddNotifier
m_pm.AddNotifier(this);  // ← 忘记这行就收不到任何通知

// 2. 控件是否被 Enabled
// Disabled 的按钮不会触发 click 通知
pBtn->SetEnabled(true);

// 3. 控件是否被遮挡
// 上层有透明控件覆盖，鼠标事件被上层拦截
// 解决：上层控件设置 mouse="false"

// 4. 通知类型字符串是否正确
if (msg.sType == _T("click")) { }     // 注意大小写
if (msg.sType == DUI_MSGTYPE_CLICK) { } // 推荐用宏
```

### 消息被"吞掉"

```cpp
// 问题：某个消息应该触发但没有效果
// 原因：消息在传递链中被提前消费

// 排查：在 MessageHandler 入口打日志
bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes) {
    #ifdef _DEBUG
    if (uMsg == WM_LBUTTONDOWN) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        CControlUI* pHit = FindControl(pt);
        TRACE(_T("LBUTTONDOWN at (%d,%d) hit: %s\n"),
              pt.x, pt.y, pHit ? pHit->GetName().GetData() : _T("NULL"));
    }
    #endif
    // ...
}

// 常见原因：
// - 容器设置了 mouse="true" 但没有转发给子控件
// - 浮动控件遮挡了目标控件
// - WM_NCHITTEST 返回了非 HTCLIENT 值
```

---

## 4. DPI 适配

### 问题表现

```
在 125%/150%/200% 缩放下：
- 界面整体偏小或偏大
- 文字模糊
- 控件位置偏移
- 图片模糊（非整数倍缩放）
```

### 解决方案

```cpp
// 方案1：声明 DPI 感知（推荐 PerMonitor V2）
// 在 manifest 或代码中声明
// app.manifest:
// <dpiAwareness>PerMonitorV2</dpiAwareness>

// 代码声明：
typedef BOOL(WINAPI* SetProcessDpiAwarenessContextProc)(DPI_AWARENESS_CONTEXT);
auto pFunc = (SetProcessDpiAwarenessContextProc)
    GetProcAddress(GetModuleHandle(_T("user32.dll")), "SetProcessDpiAwarenessContext");
if (pFunc) {
    pFunc(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

// 方案2：在 Duilib 中缩放所有尺寸
int ScaleDPI(int value) {
    static int dpi = ::GetDpiForSystem();  // Win10+
    return MulDiv(value, dpi, 96);
}

// 修改 CPaintManagerUI，在 SetWindowAttribute 时缩放
void CPaintManagerUI::SetDPI(int nDPI) {
    m_nDPI = nDPI;
    m_fScale = (float)nDPI / 96.0f;
}

// 所有固定尺寸在布局时乘以缩放系数
int CControlUI::GetFixedWidth() {
    return (int)(m_cxyFixed.cx * m_pManager->GetScale());
}
```

### 图片 DPI 适配

```cpp
// 方案A：提供多套分辨率图片
// images/      → 100% (96 DPI)
// images@1.5x/ → 150% (144 DPI)
// images@2x/   → 200% (192 DPI)

CDuiString GetDPIImagePath(LPCTSTR basePath) {
    float scale = m_pm.GetScale();
    if (scale >= 2.0f) return CDuiString(basePath) + _T("@2x");
    if (scale >= 1.5f) return CDuiString(basePath) + _T("@1.5x");
    return basePath;
}

// 方案B：使用矢量图（SVG）或大尺寸 PNG 缩小
// Duilib 原版不支持 SVG，部分分支已添加
```

---

## 5. 高版本 Windows 兼容性

### Windows 10/11 常见问题

| 问题 | 原因 | 解决 |
|------|------|------|
| 窗口圆角异常 | Win11 自动圆角与自绘冲突 | DwmSetWindowAttribute 禁用圆角 |
| 毛玻璃效果失效 | DWM API 变化 | 使用新版 DwmExtendFrameIntoClientArea |
| 触摸屏长按弹右键 | Windows 触摸反馈 | 注册窗口为 TWF_WANTPALM |
| 暗色模式不生效 | 自绘窗口不跟随系统主题 | 监听主题变化消息手动换肤 |

```cpp
// Win11 禁用自动圆角（保持自定义窗口形状）
#include <dwmapi.h>
DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_DONOTROUND;
::DwmSetWindowAttribute(m_hWnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                         &pref, sizeof(pref));

// 监听系统暗色模式切换
case WM_SETTINGCHANGE: {
    if (lParam && _tcscmp((LPCTSTR)lParam, _T("ImmersiveColorSet")) == 0) {
        // 系统主题发生变化
        bool bDarkMode = IsSystemDarkMode();
        SwitchSkin(bDarkMode ? _T("dark") : _T("light"));
    }
    break;
}
```

### 管理员权限下的拖放问题

```cpp
// 以管理员运行时，普通权限的 Explorer 拖放文件到窗口会失败
// 原因：UIPI（User Interface Privilege Isolation）

// 解决：允许低权限进程发送消息
#include <shellapi.h>
::ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
::ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
::ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);  // WM_COPYGLOBALDATA
```

---

## 6. 常见崩溃场景

### 崩溃1：空指针访问

```cpp
// 原因：FindControl 返回 NULL 后直接使用
CButtonUI* pBtn = (CButtonUI*)m_pm.FindControl(_T("btn"));
pBtn->SetText(_T("Hello"));  // 如果 pBtn == NULL → 崩溃

// 修复：始终检查返回值
CControlUI* pCtrl = m_pm.FindControl(_T("btn"));
if (pCtrl) {
    CButtonUI* pBtn = static_cast<CButtonUI*>(pCtrl);
    pBtn->SetText(_T("Hello"));
}
```

### 崩溃2：控件已被删除后仍持有指针

```cpp
// 场景：定时器回调中使用了已删除的控件指针
class CMyWnd {
    CControlUI* m_pAnimCtrl = nullptr;

    void OnTimer() {
        m_pAnimCtrl->SetValue(++m_nFrame);  // m_pAnimCtrl 可能已被 Remove/delete
    }
};

// 修复：在 Remove 前 KillTimer 并清空指针
void RemoveAnimCtrl() {
    m_pManager->KillTimer(m_pAnimCtrl, TIMER_ANIM);
    m_pContainer->Remove(m_pAnimCtrl);
    m_pAnimCtrl = nullptr;
}
```

### 崩溃3：XML 解析失败导致空根节点

```cpp
// CDialogBuilder::Create 返回 NULL 时
CControlUI* pRoot = builder.Create(_T("main.xml"), 0, this, &m_pm);
m_pm.AttachDialog(pRoot);  // pRoot == NULL → 后续所有操作崩溃

// 修复：检查返回值
if (!pRoot) {
    MessageBox(m_hWnd, _T("UI 加载失败"), _T("错误"), MB_OK);
    return;
}
m_pm.AttachDialog(pRoot);
```

### 崩溃4：Notify 中删除触发通知的控件

```cpp
// Notify 被调用时，消息分发循环还在进行
void Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        m_pContainer->Remove(msg.pSender);  // 危险！
        // msg.pSender 已被 delete，但 SendNotify 循环中可能还引用它
    }
}

// 修复：延迟删除
void Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        ::PostMessage(m_hWnd, WM_USER_REMOVE_CTRL,
                      (WPARAM)msg.pSender, 0);
    }
}
```

---

## 7. 调试技巧

### 控件状态可视化

```cpp
// 在 Debug 模式下绘制控件边框，方便定位布局问题
#ifdef _DEBUG
void DebugDrawControlBounds(HDC hDC, CControlUI* pControl) {
    RECT rc = pControl->GetPos();
    HPEN hPen = ::CreatePen(PS_DOT, 1, RGB(255, 0, 0));
    HPEN hOld = (HPEN)::SelectObject(hDC, hPen);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));

    ::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);

    ::SelectObject(hDC, hOld);
    ::SelectObject(hDC, hOldBrush);
    ::DeleteObject(hPen);
}
#endif
```

### 消息跟踪

```cpp
// 在 HandleMessage 中打印关键消息
#ifdef _DEBUG
LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_KEYDOWN:
        TRACE(_T("MSG: 0x%04X wParam=%d lParam=%d\n"), uMsg, wParam, lParam);
        break;
    }
    // ...
}
#endif
```

### Spy++ 使用

```
Spy++ 可用于：
- 查看窗口层次（确认只有一个顶层 HWND）
- 监控 Windows 消息流（WM_PAINT 频率等）
- 检查窗口样式和扩展样式
- 验证 DPI 设置是否生效

注意：Spy++ 只能看到真实 HWND，Duilib 内部控件对它不可见
```

---

## 8. 面试常见问题

### Q1: Duilib 应用如何排查内存泄漏？

分三层：
1. **C++ 对象泄漏**：CRT leak detection、Visual Studio 诊断工具、手动引用计数
2. **GDI 对象泄漏**：GetGuiResources 监控、任务管理器 GDI Objects 列
3. **图片资源泄漏**：RemoveAllImages 后检查是否完全释放、图片缓存计数

### Q2: 如何调试"控件不显示"的问题？

按以下顺序排查：
1. FindControl 能否找到该控件（存在性）
2. IsVisible() 返回什么（可见性）
3. GetPos() 返回什么矩形（位置/尺寸是否为 0）
4. 父容器是否可见、位置是否正确
5. 是否被其他控件遮挡
6. 是否有图片/颜色设置（完全透明）

### Q3: 生产环境如何收集 Duilib 应用的崩溃信息？

```cpp
// 设置未处理异常过滤器
LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* pExInfo) {
    // 生成 minidump
    HANDLE hFile = CreateFile(_T("crash.dmp"), ...);
    MINIDUMP_EXCEPTION_INFORMATION mei;
    mei.ExceptionPointers = pExInfo;
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                      hFile, MiniDumpWithFullMemory, &mei, NULL, NULL);
    CloseHandle(hFile);
    return EXCEPTION_EXECUTE_HANDLER;
}

SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
```

### Q4: Duilib 窗口无法正常显示在多显示器环境下？

```cpp
// 问题：窗口创建在已拔除的显示器坐标上
// 解决：创建窗口前验证坐标有效性
BOOL IsRectOnScreen(const RECT& rc) {
    HMONITOR hMon = ::MonitorFromRect(&rc, MONITOR_DEFAULTTONULL);
    return hMon != NULL;
}

void ShowWindow() {
    RECT rc = LoadSavedPosition();
    if (!IsRectOnScreen(rc)) {
        // 坐标无效，居中显示在主显示器
        rc = GetDefaultPosition();
    }
    ::MoveWindow(m_hWnd, rc.left, rc.top,
                 rc.right - rc.left, rc.bottom - rc.top, TRUE);
}
```

---

## 9. 调试工具推荐

| 工具 | 用途 |
|------|------|
| Visual Studio Debugger | 断点、内存、调用栈 |
| Spy++ | 窗口消息监控 |
| GDIView (NirSoft) | GDI 对象泄漏检测 |
| Process Explorer | 句柄/线程/内存详情 |
| WinDbg | Crash dump 分析 |
| Application Verifier | 堆损坏、句柄错误检测 |
| UIAutomation Verify | 辅助功能/控件可访问性测试 |
| Dependency Walker | DLL 依赖检查 |

---

## 10. 总结

| 问题类别 | 核心排查思路 |
|---------|-------------|
| **内存泄漏** | CRT 检测 + GDI 计数 + 图片缓存审查 |
| **控件查找失败** | 名称拼写 + 加载时序 + 遍历控件树确认 |
| **消息丢失** | AddNotifier + Enabled + 遮挡 + HitTest 日志 |
| **DPI 问题** | 声明感知 + 尺寸缩放 + 多倍率图片 |
| **兼容性** | 高版本 API 适配 + Manifest 声明 |
| **崩溃** | 空指针检查 + 延迟删除 + 生命周期管理 |
