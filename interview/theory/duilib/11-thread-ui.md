# 多线程与 UI 更新

## 概述

Duilib 和所有 Windows GUI 框架一样，UI 操作必须在主线程（创建窗口的线程）执行。工作线程不能直接操作控件，否则会导致竞态条件、崩溃或绘制异常。本节讲解 Duilib 中跨线程通信的正确方式，以及异步任务与 UI 更新的常见模式。

---

## 1. 核心原则：UI 只在主线程操作

### 为什么不能在工作线程操作 UI？

```cpp
// ❌ 工作线程直接操作控件
std::thread worker([this]() {
    auto result = DoHeavyWork();
    // 以下操作不安全！
    m_pLabel->SetText(result.c_str());   // 可能与主线程绘制冲突
    m_pList->RemoveAll();                // 控件树修改非线程安全
    m_pProgress->SetValue(100);          // Invalidate 在错误线程调用
});
```

原因：
1. CPaintManagerUI 和控件树没有内部锁保护
2. SetText/Invalidate 会触发 InvalidateRect，该 API 虽然线程安全，但后续的绘制逻辑依赖主线程状态
3. 控件增删会修改容器的 m_items 数组，与主线程的遍历（绘制/HitTest）并发访问导致崩溃

---

## 2. 跨线程通信方案

### 方案1：PostMessage（最常用）

```cpp
// 定义自定义消息
#define WM_USER_UPDATE_PROGRESS  (WM_USER + 100)
#define WM_USER_WORK_DONE        (WM_USER + 101)
#define WM_USER_ADD_ITEM         (WM_USER + 102)

// 工作线程中
std::thread worker([this]() {
    for (int i = 0; i <= 100; i++) {
        DoPartialWork(i);
        // 通过 PostMessage 通知主线程更新 UI
        ::PostMessage(m_hWnd, WM_USER_UPDATE_PROGRESS, i, 0);
        Sleep(50);
    }

    // 工作完成，传递结果
    CString* pResult = new CString(GetResult());
    ::PostMessage(m_hWnd, WM_USER_WORK_DONE, (WPARAM)pResult, 0);
});

// 主线程 HandleMessage 中处理
LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_USER_UPDATE_PROGRESS) {
        int progress = (int)wParam;
        m_pProgress->SetValue(progress);
        return 0;
    }
    else if (uMsg == WM_USER_WORK_DONE) {
        CString* pResult = (CString*)wParam;
        m_pLabel->SetText(*pResult);
        delete pResult;  // 释放堆上分配的数据
        return 0;
    }
    // ...
}
```

### 方案2：SendMessage（同步等待）

```cpp
// SendMessage 会阻塞工作线程直到主线程处理完消息
// 适用于需要获取 UI 状态的场景

std::thread worker([this]() {
    // 同步获取输入框内容（阻塞等待主线程处理）
    TCHAR szText[256] = { 0 };
    ::SendMessage(m_hWnd, WM_USER_GET_INPUT, (WPARAM)szText, 256);
    // szText 已填充，可安全使用
    ProcessInput(szText);
});

// 主线程处理
case WM_USER_GET_INPUT: {
    TCHAR* pBuf = (TCHAR*)wParam;
    int nMax = (int)lParam;
    CEditUI* pEdit = (CEditUI*)m_pm.FindControl(_T("editInput"));
    _tcsncpy(pBuf, pEdit->GetText(), nMax);
    return 0;
}
```

**PostMessage vs SendMessage：**

| | PostMessage | SendMessage |
|---|---|---|
| 线程行为 | 异步，立即返回 | 同步，阻塞等待处理完 |
| 数据传递 | wParam/lParam（值或堆指针） | wParam/lParam |
| 使用场景 | 通知更新、触发刷新 | 需要等待结果 |
| 风险 | 堆内存管理要小心 | 可能死锁（互相 Send） |

### 方案3：异步任务队列

```cpp
// 适用于大量异步回调的场景
class CMainWnd {
    std::mutex m_taskMutex;
    std::queue<std::function<void()>> m_taskQueue;

public:
    // 工作线程调用：投递任务到主线程
    void PostTask(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            m_taskQueue.push(std::move(task));
        }
        ::PostMessage(m_hWnd, WM_USER_EXECUTE_TASK, 0, 0);
    }

    // 主线程消息处理中执行任务
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_USER_EXECUTE_TASK) {
            ExecutePendingTasks();
            return 0;
        }
        // ...
    }

private:
    void ExecutePendingTasks() {
        std::queue<std::function<void()>> tasks;
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            std::swap(tasks, m_taskQueue);
        }
        while (!tasks.empty()) {
            tasks.front()();
            tasks.pop();
        }
    }
};

// 使用
std::thread worker([this]() {
    auto data = FetchFromNetwork();
    PostTask([this, data]() {
        // 在主线程安全执行
        m_pLabel->SetText(data.title);
        m_pImage->SetBkImage(data.imageUrl);
    });
});
```

---

## 3. 常见异步场景

### 网络请求 + UI 更新

```cpp
void CMainWnd::OnBtnFetch() {
    m_pBtnFetch->SetEnabled(false);
    m_pLoading->SetVisible(true);

    std::thread([this]() {
        // 工作线程：网络请求
        HttpResponse resp = HttpGet("https://api.example.com/data");

        // 结果回主线程
        PostTask([this, resp]() {
            m_pBtnFetch->SetEnabled(true);
            m_pLoading->SetVisible(false);

            if (resp.success) {
                UpdateList(resp.data);
            } else {
                ShowError(resp.error);
            }
        });
    }).detach();
}
```

### 异步图片加载

```cpp
// 大图加载可能耗时（磁盘 IO + 解码）
void CMainWnd::LoadImageAsync(LPCTSTR path, CControlUI* pControl) {
    CDuiString sPath = path;
    std::thread([this, sPath, pControl]() {
        // 工作线程：加载并解码图片
        HBITMAP hBmp = LoadAndDecodeImage(sPath);

        PostTask([this, pControl, hBmp, sPath]() {
            // 主线程：设置到控件
            if (hBmp) {
                // 注册到 PaintManager 的图片缓存
                TImageInfo* pInfo = new TImageInfo;
                pInfo->hBitmap = hBmp;
                // ...
                m_pm.AddImage(sPath, pInfo);
                pControl->SetBkImage(sPath);
            }
        });
    }).detach();
}
```

### 定时轮询 + UI 刷新

```cpp
// 使用 Windows 定时器（运行在主线程，无需跨线程）
void CMainWnd::StartPolling() {
    ::SetTimer(m_hWnd, TIMER_POLL, 5000, NULL);  // 每 5 秒
}

// WM_TIMER 在主线程触发
case WM_TIMER:
    if (wParam == TIMER_POLL) {
        // 轻量操作直接在主线程做
        UpdateStatus();
        // 重量操作启动工作线程
        std::thread([this]() {
            auto data = PollServer();
            PostTask([this, data]() { RefreshUI(data); });
        }).detach();
    }
    break;
```

---

## 4. 线程安全注意事项

### 控件指针的有效性

```cpp
// ❌ 危险：工作线程持有控件指针，主线程可能已销毁控件
std::thread([pLabel]() {
    Sleep(5000);
    // 如果窗口已关闭，pLabel 已被 delete → 崩溃
    ::PostMessage(???, WM_USER_UPDATE, (WPARAM)pLabel, 0);
}).detach();

// ✅ 通过控件名称查找，PostMessage 到窗口句柄
std::thread([hWnd]() {
    Sleep(5000);
    if (::IsWindow(hWnd)) {  // 检查窗口是否还存在
        ::PostMessage(hWnd, WM_USER_UPDATE, 0, 0);
    }
}).detach();

// 主线程中
case WM_USER_UPDATE: {
    CControlUI* pLabel = m_pm.FindControl(_T("label"));
    if (pLabel) pLabel->SetText(_T("Updated"));
    return 0;
}
```

### 窗口生命周期管理

```cpp
// 问题：窗口关闭后，工作线程的 PostMessage 发到无效 HWND
class CMainWnd {
    std::atomic<bool> m_bClosing{false};

    void OnClose() {
        m_bClosing = true;
        // 等待工作线程结束（或让线程自行检测）
        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
        Close();
    }
};

// 工作线程中检查
void WorkerFunc() {
    while (!m_bClosing) {
        auto data = WaitForData();
        if (m_bClosing) break;
        ::PostMessage(m_hWnd, WM_USER_DATA, ...);
    }
}
```

### 共享数据保护

```cpp
// 如果主线程和工作线程需要共享数据（非 UI 数据）
class CMainWnd {
    std::mutex m_dataMutex;
    std::vector<Record> m_records;

public:
    // 工作线程写入
    void AddRecord(const Record& rec) {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        m_records.push_back(rec);
    }

    // 主线程读取（UI 刷新时）
    void RefreshList() {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        for (auto& rec : m_records) {
            AddListItem(rec);
        }
    }
};
```

---

## 5. 面试常见问题

### Q1: Duilib 控件操作为什么必须在主线程？

Duilib 的 CPaintManagerUI 和控件树没有内部同步机制。控件的属性修改、树结构变化、绘制操作都假定在同一线程执行。如果多线程并发访问：
- 修改控件属性时主线程正在读取（DoPaint）→ 数据不一致
- 修改控件树时主线程正在遍历（FindControl/HitTest）→ 迭代器失效/崩溃
- Invalidate 触发的重绘与另一个线程的修改竞争 → 画面撕裂

### Q2: PostMessage 和 SendMessage 哪个更安全？

PostMessage 更安全：
- 不会死锁（异步投递，立即返回）
- 即使窗口正在处理其他消息也不会阻塞工作线程

SendMessage 的死锁风险：
```cpp
// 死锁场景：
// 主线程：等待工作线程结束（WaitForSingleObject）
// 工作线程：SendMessage 等待主线程处理
// → 互相等待，死锁

// 解决：用 PostMessage 替代 SendMessage
// 或者使用 SendMessageTimeout 设置超时
```

### Q3: 如何安全地在工作线程中取消 UI 更新？

```cpp
// 使用 token/generation 机制
class CMainWnd {
    std::atomic<int> m_nRequestGeneration{0};

    void StartNewRequest() {
        int gen = ++m_nRequestGeneration;  // 递增版本号

        std::thread([this, gen]() {
            auto result = SlowOperation();

            PostTask([this, gen, result]() {
                // 主线程执行前检查版本号
                if (gen != m_nRequestGeneration) return;  // 已过期，跳过
                UpdateUI(result);
            });
        }).detach();
    }
};
```

### Q4: Windows 消息队列有容量限制吗？

有。每个线程的消息队列默认限制约 10000 条消息。如果 PostMessage 过于频繁（如每毫秒 Post 一次进度），队列满后 PostMessage 会返回 FALSE 丢失消息。

解决方案：
```cpp
// 降低 Post 频率
int lastPosted = 0;
for (int i = 0; i < 1000000; i++) {
    DoWork(i);
    int percent = i * 100 / 1000000;
    if (percent != lastPosted) {
        ::PostMessage(m_hWnd, WM_USER_PROGRESS, percent, 0);
        lastPosted = percent;
    }
}
```

---

## 6. 常见陷阱

```cpp
// ❌ PostMessage 传递栈上的指针
void WorkerFunc() {
    CString result = GetResult();
    ::PostMessage(m_hWnd, WM_USER_DONE, (WPARAM)&result, 0);
    // result 在 PostMessage 返回后就析构了！
    // 主线程处理时访问的是悬空指针
}

// ✅ 在堆上分配，主线程负责释放
void WorkerFunc() {
    CString* pResult = new CString(GetResult());
    ::PostMessage(m_hWnd, WM_USER_DONE, (WPARAM)pResult, 0);
}
// 主线程：
case WM_USER_DONE: {
    CString* pResult = (CString*)wParam;
    m_pLabel->SetText(*pResult);
    delete pResult;
    return 0;
}
```

```cpp
// ❌ 工作线程直接调用 Invalidate
std::thread([this]() {
    m_data = LoadData();
    m_pControl->Invalidate();  // 不安全！在错误的线程调用
}).detach();

// ✅ 通过 PostMessage 让主线程刷新
std::thread([this]() {
    m_data = LoadData();
    ::PostMessage(m_hWnd, WM_USER_REFRESH, 0, 0);
}).detach();
```

```cpp
// ❌ detach 线程后无法控制生命周期
void OnButtonClick() {
    std::thread([]() { HeavyWork(); }).detach();
    // 如果 HeavyWork 访问了 this，而窗口提前关闭 → UB
}

// ✅ 保存 thread 对象，关闭时 join
class CMainWnd {
    std::thread m_worker;

    void OnButtonClick() {
        if (m_worker.joinable()) m_worker.join();
        m_worker = std::thread([this]() { HeavyWork(); });
    }

    void OnClose() {
        m_bClosing = true;
        if (m_worker.joinable()) m_worker.join();
        Close();
    }
};
```

---

## 7. 总结

| 特性 | 说明 |
|------|------|
| **核心原则** | UI 操作只在主线程（窗口创建线程）执行 |
| **跨线程通知** | PostMessage（异步）/ SendMessage（同步） |
| **任务队列** | std::function 队列 + PostMessage 触发执行 |
| **数据传递** | 堆分配通过 wParam/lParam 传递，主线程释放 |
| **生命周期** | 检查窗口有效性、版本号机制取消过期更新 |
| **线程管理** | 保存 thread 对象，窗口关闭时 join |
