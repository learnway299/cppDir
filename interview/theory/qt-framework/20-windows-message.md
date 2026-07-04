 # Windows 消息机制（Windows Message System）

## 概述

Windows 消息机制是 Windows GUI 编程的核心，理解它有助于深入理解 Qt 在 Windows 平台的底层实现，以及进行 Qt 与原生 Windows API 的混合编程。

---

## 1. 消息机制基础

### 1.1 什么是 Windows 消息？

**面试问题**：Windows 消息机制的核心概念是什么？

**答案**：

Windows 是基于消息驱动的操作系统。所有的用户输入、系统事件都被封装成消息，投递到消息队列，由应用程序循环处理。

```cpp
// 消息结构体
typedef struct tagMSG {
    HWND   hwnd;       // 目标窗口句柄
    UINT   message;    // 消息类型 (WM_xxx)
    WPARAM wParam;     // 消息参数 1（字大小）
    LPARAM lParam;     // 消息参数 2（长整型）
    DWORD  time;       // 消息产生时间
    POINT  pt;         // 消息产生时的鼠标位置
} MSG;

// 消息类型示例
// WM_CREATE     - 窗口创建
// WM_DESTROY    - 窗口销毁
// WM_PAINT      - 需要重绘
// WM_SIZE       - 窗口大小改变
// WM_MOUSEMOVE  - 鼠标移动
// WM_LBUTTONDOWN - 鼠标左键按下
// WM_KEYDOWN    - 键盘按下
// WM_TIMER      - 定时器
// WM_COMMAND    - 菜单/控件命令
// WM_USER       - 用户自定义消息起始值
```

### 1.2 消息循环

**面试问题**：描述 Windows 消息循环的工作流程？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                        Windows 系统                              │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              系统消息队列 (System Message Queue)           │  │
│  │    键盘、鼠标、硬件中断等原始输入事件                        │  │
│  └───────────────────────────────────────────────────────────┘  │
│                              │                                   │
│                              ▼ 分发到对应线程                    │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │           线程消息队列 (Thread Message Queue)              │  │
│  │    每个 GUI 线程有自己的消息队列                            │  │
│  └───────────────────────────────────────────────────────────┘  │
│                              │                                   │
└──────────────────────────────┼───────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                        应用程序                                  │
│                                                                  │
│    while (GetMessage(&msg, NULL, 0, 0)) {  ◄── 从队列取消息     │
│        TranslateMessage(&msg);              ◄── 翻译键盘消息     │
│        DispatchMessage(&msg);               ◄── 分发到窗口过程   │
│    }                                                             │
│                               │                                  │
│                               ▼                                  │
│    ┌─────────────────────────────────────────────────────────┐  │
│    │              窗口过程 (WndProc)                          │  │
│    │    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) │  │
│    │    {                                                     │  │
│    │        switch (msg) {                                    │  │
│    │            case WM_PAINT: ... break;                     │  │
│    │            case WM_LBUTTONDOWN: ... break;               │  │
│    │            default: return DefWindowProc(...);           │  │
│    │        }                                                 │  │
│    │    }                                                     │  │
│    └─────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

```cpp
#include <windows.h>

// 标准消息循环
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    // 1. 注册窗口类
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassEx(&wc);

    // 2. 创建窗口
    HWND hwnd = CreateWindowEx(
        0, L"MyWindowClass", L"My Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 3. 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);  // 将虚拟键消息转换为字符消息
        DispatchMessage(&msg);   // 分发消息到窗口过程
    }

    return (int)msg.wParam;
}
```

### 1.3 GetMessage vs PeekMessage

**面试问题**：GetMessage 和 PeekMessage 有什么区别？

**答案**：

```cpp
// GetMessage - 阻塞式获取消息
// 如果队列为空，线程会阻塞等待
// 返回 0 表示收到 WM_QUIT
// 返回 -1 表示错误
// 返回 > 0 表示获取到消息

while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}

// PeekMessage - 非阻塞式查看/获取消息
// 无论队列是否有消息都立即返回
// 返回 TRUE 表示有消息，FALSE 表示无消息
// 第 5 个参数控制是否移除消息

// 游戏循环常用模式
while (running) {
    // 处理所有待处理消息
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running = false;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 执行游戏逻辑
    UpdateGame();
    RenderFrame();
}

// PM_NOREMOVE - 只查看，不从队列移除
// PM_REMOVE   - 获取并从队列移除
// PM_NOYIELD  - 不让出 CPU 时间片（16 位 Windows 遗留）
```

**对比表：**

| 特性 | GetMessage | PeekMessage |
|------|------------|-------------|
| 阻塞性 | 阻塞（队列空时） | 非阻塞 |
| 返回值 | 0/正数/-1 | TRUE/FALSE |
| 适用场景 | 普通 GUI 程序 | 游戏、实时应用 |
| CPU 占用 | 低（阻塞时不占 CPU） | 高（需自己控制循环） |

---

## 2. 窗口过程（Window Procedure）

### 2.1 WndProc 函数

**面试问题**：窗口过程函数是什么？它是如何被调用的？

**答案**：

```cpp
// 窗口过程：处理发送给窗口的所有消息
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    // ========== 生命周期消息 ==========
    case WM_CREATE: {
        // 窗口创建时调用
        // lParam 指向 CREATESTRUCT，包含创建参数
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        // 初始化...
        return 0;  // 返回 0 表示继续创建，-1 表示取消
    }

    case WM_DESTROY:
        // 窗口被销毁时调用
        PostQuitMessage(0);  // 发送 WM_QUIT 退出消息循环
        return 0;

    case WM_CLOSE:
        // 用户点击关闭按钮
        // 可以在这里询问是否保存
        if (MessageBox(hwnd, L"确定退出？", L"确认", MB_YESNO) == IDYES) {
            DestroyWindow(hwnd);
        }
        return 0;

    // ========== 绘制消息 ==========
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 只重绘无效区域
        RECT* rcPaint = &ps.rcPaint;

        // 绘制操作
        TextOut(hdc, 10, 10, L"Hello Windows!", 14);
        Rectangle(hdc, 50, 50, 150, 100);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        // 擦除背景
        // 返回非 0 表示已处理，防止闪烁
        return 1;

    // ========== 鼠标消息 ==========
    case WM_MOUSEMOVE: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        // wParam 包含按键状态 (MK_LBUTTON, MK_RBUTTON, etc.)
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        SetCapture(hwnd);  // 捕获鼠标
        return 0;
    }

    case WM_LBUTTONUP:
        ReleaseCapture();  // 释放鼠标捕获
        return 0;

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        // delta > 0 向上滚动，< 0 向下滚动
        return 0;
    }

    // ========== 键盘消息 ==========
    case WM_KEYDOWN:
        // wParam = 虚拟键码 (VK_xxx)
        // lParam = 扫描码、重复次数等
        if (wParam == VK_ESCAPE) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        return 0;

    case WM_CHAR:
        // 字符消息（经过 TranslateMessage 转换）
        // wParam = 字符码
        wchar_t ch = (wchar_t)wParam;
        return 0;

    // ========== 大小/位置消息 ==========
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        // wParam: SIZE_RESTORED, SIZE_MINIMIZED, SIZE_MAXIMIZED
        return 0;
    }

    case WM_MOVE: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        return 0;
    }

    // ========== 定时器消息 ==========
    case WM_TIMER:
        // wParam = 定时器 ID
        if (wParam == TIMER_ID_UPDATE) {
            // 处理定时器事件
        }
        return 0;

    // ========== 命令消息 ==========
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);     // 菜单/控件 ID
        int wmEvent = HIWORD(wParam);  // 通知码
        // lParam = 控件句柄（如果来自控件）

        switch (wmId) {
        case ID_FILE_OPEN:
            // 处理菜单命令
            break;
        }
        return 0;
    }

    default:
        // 未处理的消息交给默认处理
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
```

### 2.2 消息处理的返回值

**面试问题**：WndProc 的返回值有什么含义？

**答案**：

```cpp
// 不同消息对返回值的要求不同

// 大多数消息：返回 0 表示已处理
case WM_PAINT:
    // ...
    return 0;

// WM_CREATE：返回 0 继续创建，-1 取消创建
case WM_CREATE:
    if (初始化失败) return -1;
    return 0;

// WM_ERASEBKGND：返回非 0 表示已擦除背景
case WM_ERASEBKGND:
    return 1;  // 不让系统擦除，防止闪烁

// WM_NCHITTEST：返回命中测试结果
case WM_NCHITTEST:
    return HTCLIENT;  // 或 HTCAPTION, HTBORDER 等

// WM_SETCURSOR：返回 TRUE 表示已设置光标
case WM_SETCURSOR:
    SetCursor(hCustomCursor);
    return TRUE;

// 未处理的消息：必须调用 DefWindowProc
default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
```

---

## 3. 消息发送与投递

### 3.1 SendMessage vs PostMessage

**面试问题**：SendMessage 和 PostMessage 有什么区别？

**答案**：

```cpp
// SendMessage - 同步发送
// 1. 直接调用目标窗口的 WndProc
// 2. 阻塞直到消息处理完成
// 3. 可以获取返回值
// 4. 跨线程时会等待目标线程处理

LRESULT result = SendMessage(hwnd, WM_USER + 1, wParam, lParam);
// result 是 WndProc 的返回值

// PostMessage - 异步投递
// 1. 将消息放入目标窗口的消息队列
// 2. 立即返回，不等待处理
// 3. 无法获取处理结果
// 4. 不能发送需要同步的消息

BOOL success = PostMessage(hwnd, WM_USER + 1, wParam, lParam);
// success 只表示是否成功放入队列

// PostThreadMessage - 投递到线程（不关联窗口）
PostThreadMessage(threadId, WM_USER + 1, wParam, lParam);
```

**对比表：**

| 特性 | SendMessage | PostMessage |
|------|-------------|-------------|
| 执行方式 | 同步（阻塞） | 异步（非阻塞） |
| 返回值 | WndProc 返回值 | 是否成功入队 |
| 跨线程 | 切换到目标线程执行 | 放入目标线程队列 |
| 使用场景 | 需要结果、需要同步 | 通知、不关心结果 |
| 死锁风险 | 跨线程时可能死锁 | 无死锁风险 |

```cpp
// 跨线程 SendMessage 的潜在死锁
// 线程 A                    线程 B
// SendMessage(hwndB, ...)   SendMessage(hwndA, ...)
//    等待 B 处理 ────────────►  等待 A 处理
//                ◄────────────
//              死锁！

// 解决方案：使用 SendMessageTimeout
DWORD_PTR result;
LRESULT lr = SendMessageTimeout(
    hwnd, WM_USER, 0, 0,
    SMTO_ABORTIFHUNG,  // 如果目标窗口无响应则中止
    5000,              // 超时时间（毫秒）
    &result
);
```

### 3.2 发送自定义消息

```cpp
// 自定义消息范围
// WM_USER (0x0400) ~ 0x7FFF：窗口类私有消息
// WM_APP (0x8000) ~ 0xBFFF：应用程序消息
// RegisterWindowMessage：系统范围唯一消息

#define WM_MY_CUSTOM    (WM_USER + 100)
#define WM_APP_NOTIFY   (WM_APP + 1)

// 注册系统唯一消息
UINT WM_MY_GLOBAL = RegisterWindowMessage(L"MyUniqueMessage");

// 在 WndProc 中处理
case WM_MY_CUSTOM:
    // 处理自定义消息
    return 0;

// 动态处理注册消息
if (msg == WM_MY_GLOBAL) {
    // 处理全局消息
    return 0;
}
```

### 3.3 广播消息

```cpp
// 向所有顶层窗口广播
SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);

// 向所有窗口发送（包括子窗口）
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    SendMessage(hwnd, WM_MY_MESSAGE, 0, lParam);
    return TRUE;  // 继续枚举
}
EnumWindows(EnumWindowsProc, (LPARAM)myData);
```

---

## 4. 消息队列

### 4.1 消息队列结构

**面试问题**：Windows 有几种消息队列？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                      系统消息队列                                │
│    (Raw Input Queue - 全系统唯一)                               │
│    存放来自键盘、鼠标等硬件的原始输入                            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ 分发
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    线程消息队列 (每个 GUI 线程一个)              │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ Posted Messages Queue (投递消息队列)                     │   │
│  │   PostMessage 投递的消息                                 │   │
│  └─────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ Sent Messages Queue (发送消息队列)                       │   │
│  │   其他线程 SendMessage 的消息                            │   │
│  └─────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ Input Queue (输入队列)                                   │   │
│  │   从系统队列分发来的输入消息                              │   │
│  └─────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ Paint Queue (绘制队列)                                   │   │
│  │   WM_PAINT 消息（合并优化）                               │   │
│  └─────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ Timer Queue (定时器队列)                                 │   │
│  │   WM_TIMER 消息                                          │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

**消息优先级**（GetMessage 获取顺序）：

1. Sent Messages（最高优先级）
2. Posted Messages
3. Input Messages（QS_INPUT）
4. WM_PAINT（合并后的）
5. WM_TIMER（最低优先级）

```cpp
// 检查消息队列状态
DWORD status = GetQueueStatus(QS_ALLINPUT);
// QS_KEY        - 键盘消息
// QS_MOUSEMOVE  - 鼠标移动
// QS_MOUSEBUTTON - 鼠标按钮
// QS_PAINT      - 绘制消息
// QS_TIMER      - 定时器消息
// QS_POSTMESSAGE - 投递消息
// QS_SENDMESSAGE - 发送消息
```

### 4.2 消息过滤

```cpp
// GetMessage 可以过滤消息
// 只获取指定窗口的消息
GetMessage(&msg, hwnd, 0, 0);

// 只获取指定范围的消息
GetMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST);  // 只获取键盘消息
GetMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST);  // 只获取鼠标消息

// 消息范围常量
// WM_KEYFIRST ~ WM_KEYLAST
// WM_MOUSEFIRST ~ WM_MOUSELAST
```

---

## 5. 消息钩子（Hook）

### 5.1 钩子类型

**面试问题**：Windows 消息钩子有什么用途？有哪些类型？

**答案**：

```cpp
// 钩子：拦截消息的机制
// 可以在消息到达目标窗口前进行处理

// 钩子类型
// WH_KEYBOARD     - 键盘消息
// WH_KEYBOARD_LL  - 低级键盘钩子（全局）
// WH_MOUSE        - 鼠标消息
// WH_MOUSE_LL     - 低级鼠标钩子（全局）
// WH_MSGFILTER    - 对话框/菜单/滚动条消息
// WH_CALLWNDPROC  - SendMessage 发送的消息
// WH_CALLWNDPROCRET - SendMessage 返回后
// WH_GETMESSAGE   - GetMessage/PeekMessage 获取的消息
// WH_CBT          - 窗口操作（创建、激活等）
// WH_SHELL        - Shell 事件

// 安装钩子
HHOOK hHook = SetWindowsHookEx(
    WH_KEYBOARD_LL,      // 钩子类型
    LowLevelKeyboardProc, // 钩子函数
    hInstance,           // 模块句柄
    0                    // 线程 ID（0 = 全局）
);

// 钩子函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKB = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            // 记录按键
            printf("Key: %d\n", pKB->vkCode);

            // 阻止按键（返回非 0）
            if (pKB->vkCode == VK_F12) {
                return 1;  // 阻止 F12
            }
        }
    }

    // 传递给下一个钩子
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// 卸载钩子
UnhookWindowsHookEx(hHook);
```

### 5.2 局部钩子 vs 全局钩子

```cpp
// 局部钩子：只拦截本线程/进程的消息
HHOOK hLocalHook = SetWindowsHookEx(
    WH_KEYBOARD,
    KeyboardProc,
    NULL,               // 本进程时可以为 NULL
    GetCurrentThreadId() // 指定线程
);

// 全局钩子：拦截所有进程的消息
// 必须在 DLL 中实现，因为需要注入到其他进程
// 钩子 DLL 代码
__declspec(dllexport) LRESULT CALLBACK KeyboardProc(
    int nCode, WPARAM wParam, LPARAM lParam) {
    // ...
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// 主程序安装
HMODULE hDll = LoadLibrary(L"HookDll.dll");
HOOKPROC proc = (HOOKPROC)GetProcAddress(hDll, "KeyboardProc");
HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD, proc, hDll, 0);
```

---

## 6. 常见消息详解

### 6.1 生命周期消息

```cpp
// 窗口创建过程中的消息顺序
// WM_NCCREATE     - 非客户区创建
// WM_CREATE       - 客户区创建
// WM_SIZE         - 设置初始大小
// WM_MOVE         - 设置初始位置
// WM_SHOWWINDOW   - 显示窗口（如果 ShowWindow 调用）

// 窗口销毁过程中的消息顺序
// WM_CLOSE        - 请求关闭（可拦截）
// WM_DESTROY      - 窗口正在销毁
// WM_NCDESTROY    - 非客户区销毁（最后一个消息）

case WM_NCDESTROY:
    // 清理与窗口关联的数据
    MyData* data = (MyData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    delete data;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    return 0;
```

### 6.2 绘制消息

```cpp
// WM_PAINT 的特殊性
// 1. 低优先级，会被合并
// 2. 只有无效区域需要重绘时才产生
// 3. 必须调用 BeginPaint/EndPaint 或 ValidateRect

case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    // ps.rcPaint 是无效区域

    // 绘制...

    EndPaint(hwnd, &ps);  // 必须调用！否则持续收到 WM_PAINT
    return 0;
}

// 触发重绘的方式
InvalidateRect(hwnd, NULL, TRUE);  // 使整个客户区无效
InvalidateRect(hwnd, &rect, FALSE); // 使指定区域无效，不擦除背景
UpdateWindow(hwnd);  // 立即发送 WM_PAINT（不经过队列）
RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

// WM_ERASEBKGND
// 在 WM_PAINT 之前发送，用于擦除背景
case WM_ERASEBKGND:
    // 返回非 0 表示已处理，避免闪烁
    return 1;
```

### 6.3 鼠标消息

```cpp
// 客户区鼠标消息
// WM_MOUSEMOVE      - 移动
// WM_LBUTTONDOWN    - 左键按下
// WM_LBUTTONUP      - 左键释放
// WM_LBUTTONDBLCLK  - 左键双击（需要 CS_DBLCLKS 样式）
// WM_RBUTTONDOWN    - 右键按下
// WM_MBUTTONDOWN    - 中键按下
// WM_MOUSEWHEEL     - 滚轮

// 非客户区鼠标消息（标题栏、边框等）
// WM_NCMOUSEMOVE
// WM_NCLBUTTONDOWN
// ...

// 获取鼠标位置
case WM_MOUSEMOVE: {
    int x = GET_X_LPARAM(lParam);  // 推荐用宏
    int y = GET_Y_LPARAM(lParam);
    // 或
    int x2 = LOWORD(lParam);  // 可能在多显示器时有问题
    int y2 = HIWORD(lParam);

    // 检查按键状态
    bool leftDown = (wParam & MK_LBUTTON) != 0;
    bool ctrlDown = (wParam & MK_CONTROL) != 0;
    bool shiftDown = (wParam & MK_SHIFT) != 0;
    return 0;
}

// 鼠标捕获
case WM_LBUTTONDOWN:
    SetCapture(hwnd);  // 捕获鼠标，即使移出窗口也能收到消息
    return 0;

case WM_LBUTTONUP:
    ReleaseCapture();
    return 0;

// 鼠标追踪（悬停和离开检测）
case WM_MOUSEMOVE: {
    TRACKMOUSEEVENT tme = { sizeof(tme) };
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = hwnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&tme);
    return 0;
}

case WM_MOUSEHOVER:
    // 鼠标悬停
    return 0;

case WM_MOUSELEAVE:
    // 鼠标离开
    return 0;
```

### 6.4 键盘消息

```cpp
// WM_KEYDOWN  - 按键按下
// WM_KEYUP    - 按键释放
// WM_CHAR     - 字符输入（经过 TranslateMessage 转换）
// WM_SYSKEYDOWN - 系统按键（Alt 组合键）
// WM_SYSKEYUP

case WM_KEYDOWN: {
    int vkCode = (int)wParam;  // 虚拟键码

    // lParam 包含更多信息
    int repeatCount = lParam & 0xFFFF;
    int scanCode = (lParam >> 16) & 0xFF;
    bool extended = (lParam >> 24) & 1;  // 扩展键（如右 Ctrl）
    bool wasDown = (lParam >> 30) & 1;   // 之前是否已按下

    switch (vkCode) {
    case VK_RETURN:  // Enter
    case VK_ESCAPE:  // Esc
    case VK_SPACE:   // 空格
    case 'A':        // A 键（大写字母）
    case VK_F1:      // F1
    case VK_LEFT:    // 左箭头
        break;
    }
    return 0;
}

case WM_CHAR: {
    wchar_t ch = (wchar_t)wParam;
    // 已转换的字符，可直接使用
    return 0;
}

// 检查按键状态
if (GetKeyState(VK_CONTROL) & 0x8000) {
    // Ctrl 键按下
}
if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
    // Shift 键按下（异步检查）
}
```

---

## 7. 与 Qt 的关系

### 7.1 Qt 对 Windows 消息的封装

**面试问题**：Qt 如何处理 Windows 原生消息？

**答案**：

```cpp
// Qt 内部有自己的消息循环
// QEventLoop::exec() 内部调用 Windows 消息 API

// Qt 事件与 Windows 消息的对应关系
// WM_PAINT        → QPaintEvent
// WM_SIZE         → QResizeEvent
// WM_MOVE         → QMoveEvent
// WM_MOUSEMOVE    → QMouseEvent (QEvent::MouseMove)
// WM_LBUTTONDOWN  → QMouseEvent (QEvent::MouseButtonPress)
// WM_KEYDOWN      → QKeyEvent (QEvent::KeyPress)
// WM_TIMER        → QTimerEvent
// WM_CLOSE        → QCloseEvent

// Qt 5: 使用 nativeEvent 接收原生消息
class MyWidget : public QWidget {
protected:
    bool nativeEvent(const QByteArray& eventType,
                     void* message, long* result) override {
        if (eventType == "windows_generic_MSG") {
            MSG* msg = static_cast<MSG*>(message);

            if (msg->message == WM_NCHITTEST) {
                // 自定义命中测试（实现无边框窗口拖动）
                *result = HTCAPTION;
                return true;  // 已处理
            }
        }
        return QWidget::nativeEvent(eventType, message, result);
    }
};

// Qt 6: nativeEvent 的签名略有不同
bool nativeEvent(const QByteArray& eventType,
                 void* message, qintptr* result) override;
```

### 7.2 Qt 与原生 Windows API 混合

```cpp
// 获取 Qt 窗口的原生句柄
QWidget* widget = new QWidget();
HWND hwnd = reinterpret_cast<HWND>(widget->winId());

// 使用原生 API 操作
SetWindowText(hwnd, L"Native Title");
SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

// 发送原生消息
SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

// 创建原生子窗口嵌入 Qt
HWND nativeChild = CreateWindowEx(
    0, L"STATIC", L"Native",
    WS_CHILD | WS_VISIBLE,
    0, 0, 100, 30,
    hwnd, nullptr, nullptr, nullptr
);

// 将原生窗口包装为 QWidget
QWidget* wrapped = QWidget::createWindowContainer(
    QWindow::fromWinId((WId)nativeHwnd), parent);
```

### 7.3 Qt 消息循环与 Windows 消息循环

```cpp
// Qt 的 exec() 内部实现（简化版）
int QEventLoop::exec() {
    while (!d->exit) {
        // 处理 Qt 事件
        processEvents();
    }
    return d->returnCode;
}

// processEvents 在 Windows 上大致这样工作：
void QEventDispatcherWin32::processEvents() {
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            // 处理退出
            return;
        }

        // 翻译并分发
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 处理 Qt 内部事件（信号槽、定时器等）
    // ...
}

// 在非 GUI 线程使用 Windows 消息
// 需要创建消息队列
QThread* thread = new QThread();
// moveToThread 会确保线程有消息队列
```

---

## 8. 常见面试题

### Q1: 为什么 WM_PAINT 优先级低？

**答案**：

```cpp
// WM_PAINT 是低优先级消息，有几个原因：

// 1. 绘制可以合并优化
//    多次 InvalidateRect 只产生一次 WM_PAINT
//    减少重复绘制

// 2. 用户输入更重要
//    键盘、鼠标消息应该优先响应
//    绘制可以稍后进行

// 3. 避免阻塞
//    复杂的绘制操作不应阻塞输入处理

// 验证：快速调用多次 InvalidateRect
for (int i = 0; i < 100; ++i) {
    InvalidateRect(hwnd, NULL, FALSE);
}
// 只会产生 1 次 WM_PAINT（在消息循环空闲时）

// 如果需要立即绘制
UpdateWindow(hwnd);  // 绕过队列，直接调用 WndProc
```

### Q2: SendMessage 跨线程时如何工作？

**答案**：

```cpp
// 跨线程 SendMessage 的工作流程：

// 线程 A 调用 SendMessage(hwndB, ...)
// 1. 系统检测到 hwndB 属于线程 B
// 2. 将消息放入线程 B 的 Sent Messages 队列
// 3. 线程 A 阻塞等待
// 4. 线程 B 在消息循环中处理时：
//    - GetMessage/PeekMessage 检查 Sent Messages
//    - 调用目标窗口的 WndProc
//    - 将返回值传回线程 A
// 5. 线程 A 收到返回值，继续执行

// 关键点：
// - 线程 B 必须运行消息循环
// - 如果线程 B 阻塞，线程 A 也会阻塞
// - 可能导致死锁

// 避免死锁的方法：
// 1. 使用 PostMessage 代替
// 2. 使用 SendMessageTimeout
// 3. 使用 SendNotifyMessage（类似 PostMessage，但同进程时同步）
// 4. 使用 ReplyMessage 提前返回
```

### Q3: 如何实现无边框窗口的拖动？

**答案**：

```cpp
// 方法 1：处理 WM_NCHITTEST
case WM_NCHITTEST: {
    // 获取鼠标位置
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ScreenToClient(hwnd, &pt);

    // 判断是否在标题栏区域
    RECT titleBar = { 0, 0, width, 30 };  // 假设标题栏高 30
    if (PtInRect(&titleBar, pt)) {
        return HTCAPTION;  // 假装是标题栏
    }

    // 边缘调整大小
    // 返回 HTLEFT, HTRIGHT, HTTOP, HTBOTTOM 等

    return HTCLIENT;
}

// 方法 2：手动处理拖动
static POINT dragStart;
static bool dragging = false;

case WM_LBUTTONDOWN:
    if (/* 在标题栏区域 */) {
        dragging = true;
        GetCursorPos(&dragStart);
        SetCapture(hwnd);
    }
    return 0;

case WM_MOUSEMOVE:
    if (dragging) {
        POINT pt;
        GetCursorPos(&pt);
        RECT rect;
        GetWindowRect(hwnd, &rect);
        SetWindowPos(hwnd, NULL,
            rect.left + (pt.x - dragStart.x),
            rect.top + (pt.y - dragStart.y),
            0, 0, SWP_NOSIZE | SWP_NOZORDER);
        dragStart = pt;
    }
    return 0;

case WM_LBUTTONUP:
    if (dragging) {
        dragging = false;
        ReleaseCapture();
    }
    return 0;
```

### Q4: WM_TIMER 的精度如何？

**答案**：

```cpp
// WM_TIMER 的特点：

// 1. 低精度
//    - 默认精度约 15.6ms（64 Hz）
//    - 受系统时钟分辨率影响

// 2. 低优先级
//    - 只有当消息队列为空时才处理
//    - 可能被延迟

// 3. 不保证定时
//    - 繁忙时可能丢失
//    - 只保证不会过早触发

// 设置定时器
UINT_PTR timerId = SetTimer(hwnd, 1, 100, NULL);  // 100ms

// 高精度定时器方案：
// 1. timeSetEvent (Multimedia Timer)
timeSetEvent(10, 1, TimerCallback, 0, TIME_PERIODIC);

// 2. CreateTimerQueueTimer
CreateTimerQueueTimer(&hTimer, NULL, Callback, NULL, 0, 10, 0);

// 3. 独立线程 + Sleep/WaitableTimer
// 4. QueryPerformanceCounter 忙等待（精度最高）
```

### Q5: 如何在 Qt 中处理 Windows 不支持的特性？

**答案**：

```cpp
// 通过 nativeEvent 处理原生消息

class CustomWidget : public QWidget {
protected:
    bool nativeEvent(const QByteArray& eventType,
                     void* message, long* result) override {
        MSG* msg = static_cast<MSG*>(message);

        switch (msg->message) {
        case WM_DEVICECHANGE:
            // USB 设备插拔
            handleDeviceChange(msg->wParam, msg->lParam);
            return true;

        case WM_POWERBROADCAST:
            // 电源状态变化
            handlePowerChange(msg->wParam);
            return true;

        case WM_DISPLAYCHANGE:
            // 显示器分辨率变化
            handleDisplayChange();
            return true;

        case WM_DWMCOMPOSITIONCHANGED:
            // DWM 合成状态变化
            handleDwmChange();
            return true;
        }

        return QWidget::nativeEvent(eventType, message, result);
    }
};

// 安装全局消息过滤器
class NativeEventFilter : public QAbstractNativeEventFilter {
public:
    bool nativeEventFilter(const QByteArray& eventType,
                           void* message, long* result) override {
        // 处理所有原生消息
        return false;  // 继续传递
    }
};

// 安装
QCoreApplication::instance()->installNativeEventFilter(new NativeEventFilter);
```

---

## 9. 总结

### 核心要点

| 概念 | 说明 |
|------|------|
| **消息循环** | GetMessage/TranslateMessage/DispatchMessage |
| **窗口过程** | WndProc 处理所有发送给窗口的消息 |
| **消息队列** | 每个 GUI 线程一个，多个子队列 |
| **SendMessage** | 同步发送，可能跨线程阻塞 |
| **PostMessage** | 异步投递，立即返回 |
| **消息钩子** | 拦截消息的机制 |
| **Qt 封装** | nativeEvent 处理原生消息 |

### 面试必记

1. **GetMessage vs PeekMessage**：阻塞 vs 非阻塞
2. **SendMessage vs PostMessage**：同步 vs 异步
3. **消息优先级**：Sent > Posted > Input > Paint > Timer
4. **WM_PAINT 特性**：低优先级、可合并、必须 BeginPaint/EndPaint
5. **Qt 集成**：nativeEvent、winId()、原生句柄操作
