# Qt 定时器

> 深入浅出考察 Qt 定时器的实现原理、使用方式与精度问题

---

## 1. 定时器概述

### 1.1 Qt 定时器类型

**面试问题**：Qt 有哪些定时器？它们的区别是什么？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                    Qt 定时器体系                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌───────────────┐  ┌───────────────┐  ┌───────────────────┐   │
│  │   QTimer      │  │ QBasicTimer   │  │ QObject::startTimer│   │
│  │   (高级)       │  │   (轻量)       │  │    (底层)          │   │
│  └───────────────┘  └───────────────┘  └───────────────────┘   │
│         │                  │                    │               │
│         ▼                  ▼                    ▼               │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │              QAbstractEventDispatcher                    │   │
│  │              (平台事件分发器)                              │   │
│  └─────────────────────────────────────────────────────────┘   │
│                            │                                    │
│         ┌──────────────────┼──────────────────┐                │
│         ▼                  ▼                  ▼                │
│  ┌───────────┐     ┌───────────┐      ┌───────────┐           │
│  │ Windows   │     │  Linux    │      │  macOS    │           │
│  │ SetTimer  │     │ timerfd   │      │ CFRunLoop │           │
│  └───────────┘     └───────────┘      └───────────┘           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

| 定时器类型 | 特点 | 使用场景 |
|-----------|------|----------|
| QTimer | 功能完整，支持单次/重复，可绑定信号槽 | 通用场景，最常用 |
| QBasicTimer | 轻量级，无信号槽开销 | 需要大量定时器，性能敏感 |
| startTimer() | QObject 内置方法 | 简单场景，不需要 QTimer 对象 |
| QElapsedTimer | 高精度计时（非定时器） | 性能测量，时间间隔计算 |

---

### 1.2 定时器工作原理

**面试问题**：Qt 定时器是如何工作的？

**答案**：

```cpp
// 定时器工作原理
class TimerMechanism {
public:
    void explain() {
        /*
        1. 注册定时器：
           - 调用 QTimer::start() 或 startTimer()
           - 向事件循环注册定时器

        2. 事件循环检查：
           - 事件循环每次迭代检查定时器是否到期
           - 到期的定时器生成 QTimerEvent

        3. 事件分发：
           - QTimerEvent 被分发到目标 QObject
           - 对于 QTimer：触发 timeout() 信号
           - 对于 startTimer()：调用 timerEvent()

        4. 精度限制：
           - 定时器依赖事件循环
           - 如果主线程阻塞，定时器不会触发
           - 最小精度通常为 1ms（平台相关）
        */
    }
};

// 事件循环与定时器的关系
void eventLoopAndTimer() {
    /*
    事件循环伪代码：

    while (running) {
        // 1. 处理待处理事件
        processEvents();

        // 2. 检查定时器
        QList<int> expiredTimers = getExpiredTimers();
        for (int timerId : expiredTimers) {
            dispatchTimerEvent(timerId);
        }

        // 3. 等待新事件（带超时）
        waitForEvents(nextTimerTimeout);
    }
    */
}
```

---

## 2. QTimer 详解

### 2.1 基本用法

**面试问题**：QTimer 有哪些使用方式？

**答案**：

```cpp
#include <QTimer>
#include <QObject>

class TimerDemo : public QObject {
    Q_OBJECT

public:
    TimerDemo() {
        // 方式 1：成员变量 + 信号槽
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &TimerDemo::onTimeout);
        m_timer->start(1000);  // 每 1000ms 触发一次
    }

    void demonstrateUsage() {
        // 方式 2：单次定时器（one-shot）
        QTimer::singleShot(2000, this, &TimerDemo::onSingleShot);

        // 方式 3：单次定时器 + Lambda（最简洁）
        QTimer::singleShot(3000, this, []() {
            qDebug() << "Single shot with lambda";
        });

        // 方式 4：带上下文的单次定时器
        QTimer::singleShot(4000, this, [this]() {
            if (m_running) {
                doSomething();
            }
        });
    }

    void controlTimer() {
        // 启动定时器
        m_timer->start(500);      // 启动，500ms 间隔
        m_timer->start();         // 使用之前设置的间隔启动

        // 停止定时器
        m_timer->stop();

        // 检查状态
        bool active = m_timer->isActive();
        int interval = m_timer->interval();
        int remaining = m_timer->remainingTime();  // 距离下次触发的时间

        // 修改间隔
        m_timer->setInterval(2000);
    }

private slots:
    void onTimeout() {
        qDebug() << "Timer timeout!";
    }

    void onSingleShot() {
        qDebug() << "Single shot fired!";
    }

private:
    void doSomething() {}

    QTimer* m_timer;
    bool m_running = true;
};
```

---

### 2.2 定时器类型（Qt::TimerType）

**面试问题**：QTimer 的精度类型有什么区别？

**答案**：

```cpp
#include <QTimer>

void timerTypeDemo() {
    QTimer timer;

    // 精确定时器：尽可能准确触发
    // 系统会尽量在指定时间触发，开销较大
    timer.setTimerType(Qt::PreciseTimer);

    // 粗糙定时器（默认）：允许 5% 的误差
    // 系统可能会合并多个定时器，节省电量
    timer.setTimerType(Qt::CoarseTimer);

    // 非常粗糙的定时器：允许更大误差
    // 适合后台任务，最省电
    timer.setTimerType(Qt::VeryCoarseTimer);
}

/*
Qt::TimerType 详解：

┌─────────────────┬────────────────────────────────────────────┐
│ 类型            │ 特点                                        │
├─────────────────┼────────────────────────────────────────────┤
│ PreciseTimer    │ • 尝试 1ms 精度                             │
│                 │ • 不会与其他定时器合并                       │
│                 │ • 开销大，耗电                               │
│                 │ • 适合：动画、精确计时                       │
├─────────────────┼────────────────────────────────────────────┤
│ CoarseTimer     │ • 允许 5% 误差                              │
│                 │ • 系统可能合并相近的定时器                    │
│                 │ • 省电，推荐默认使用                         │
│                 │ • 适合：UI 更新、轮询                        │
├─────────────────┼────────────────────────────────────────────┤
│ VeryCoarseTimer │ • 精度约 1 秒                               │
│                 │ • 最省电                                    │
│                 │ • 适合：后台检查、自动保存                   │
└─────────────────┴────────────────────────────────────────────┘
*/
```

---

### 2.3 线程与定时器

**面试问题**：QTimer 可以在子线程中使用吗？有什么注意事项？

**答案**：

```cpp
#include <QTimer>
#include <QThread>

// 规则：QTimer 必须在有事件循环的线程中使用

class WorkerThread : public QThread {
    Q_OBJECT

protected:
    void run() override {
        // 方式 1：在子线程创建定时器
        QTimer timer;
        connect(&timer, &QTimer::timeout, this, &WorkerThread::doWork);
        timer.start(1000);

        // 必须启动事件循环！
        exec();  // 阻塞直到 quit() 被调用

        // 没有 exec()，定时器不会触发
    }

private slots:
    void doWork() {
        qDebug() << "Working in thread:" << QThread::currentThread();
    }
};

// 更好的方式：使用 moveToThread
class Worker : public QObject {
    Q_OBJECT

public:
    Worker() {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &Worker::process);
    }

public slots:
    void start() {
        m_timer->start(1000);
    }

    void stop() {
        m_timer->stop();
    }

private slots:
    void process() {
        qDebug() << "Processing in thread:" << QThread::currentThread();
    }

private:
    QTimer* m_timer;
};

void useWorkerInThread() {
    QThread* thread = new QThread();
    Worker* worker = new Worker();

    worker->moveToThread(thread);

    // 连接信号以控制生命周期
    QObject::connect(thread, &QThread::started, worker, &Worker::start);
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();

    // ... 稍后停止
    // worker->stop();
    // thread->quit();
    // thread->wait();
}

// 跨线程启动定时器的错误示例
void wrongUsage() {
    QTimer* timer = new QTimer();

    QThread* thread = new QThread();
    thread->start();

    // ❌ 错误：定时器属于主线程，但在子线程启动
    // QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection);

    // ✅ 正确：先 moveToThread
    timer->moveToThread(thread);
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection,
                               Q_ARG(int, 1000));
}
```

**关键点**：
1. QTimer 必须在有事件循环的线程中使用
2. 定时器只能在创建它的线程或 moveToThread 后的线程中启动
3. 跨线程操作定时器需要使用信号槽或 invokeMethod

---

## 3. QBasicTimer 详解

### 3.1 基本用法

**面试问题**：QBasicTimer 和 QTimer 有什么区别？什么时候使用 QBasicTimer？

**答案**：

```cpp
#include <QBasicTimer>
#include <QObject>
#include <QTimerEvent>

class BasicTimerDemo : public QObject {
    Q_OBJECT

public:
    void startTimer() {
        // QBasicTimer 不使用信号槽，更轻量
        m_timer.start(100, this);  // 100ms 间隔，事件发送到 this
    }

    void stopTimer() {
        m_timer.stop();
    }

    bool isActive() const {
        return m_timer.isActive();
    }

    int timerId() const {
        return m_timer.timerId();
    }

protected:
    // 重写 timerEvent 处理定时器事件
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_timer.timerId()) {
            // 处理定时器事件
            qDebug() << "Basic timer fired!";
        }
    }

private:
    QBasicTimer m_timer;
};

// 多个定时器示例
class MultipleTimers : public QObject {
    Q_OBJECT

public:
    MultipleTimers() {
        m_fastTimer.start(50, this);   // 50ms
        m_slowTimer.start(1000, this); // 1000ms
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_fastTimer.timerId()) {
            handleFastTimer();
        } else if (event->timerId() == m_slowTimer.timerId()) {
            handleSlowTimer();
        }
    }

private:
    void handleFastTimer() {
        // 快速定时器逻辑
    }

    void handleSlowTimer() {
        // 慢速定时器逻辑
    }

    QBasicTimer m_fastTimer;
    QBasicTimer m_slowTimer;
};
```

---

### 3.2 QTimer vs QBasicTimer

```cpp
/*
┌─────────────────┬─────────────────────┬─────────────────────┐
│ 特性            │ QTimer               │ QBasicTimer         │
├─────────────────┼─────────────────────┼─────────────────────┤
│ 继承            │ QObject              │ 无（普通类）         │
│ 信号槽          │ ✅ 支持               │ ❌ 不支持            │
│ 单次触发        │ ✅ singleShot         │ ❌ 需手动实现        │
│ 内存开销        │ 较大（QObject）       │ 较小（几个整数）     │
│ 使用方式        │ 信号槽连接            │ 重写 timerEvent      │
│ 定时器类型      │ ✅ 可设置              │ ❌ 默认 CoarseTimer  │
│ 精度控制        │ ✅ 支持                │ ✅ 支持（start重载） │
│ 剩余时间查询    │ ✅ remainingTime()    │ ❌ 不支持            │
└─────────────────┴─────────────────────┴─────────────────────┘

选择建议：
- 一般情况：使用 QTimer（功能完整，易用）
- 性能敏感，大量定时器：使用 QBasicTimer
- 简单场景：使用 QObject::startTimer()
*/

// QBasicTimer 带精度的 start
void basicTimerWithPrecision() {
    QBasicTimer timer;

    // Qt 5.9+ 支持指定定时器类型
    timer.start(100, Qt::PreciseTimer, this);
}
```

---

## 4. QObject::startTimer

### 4.1 基本用法

**面试问题**：QObject 内置的 startTimer/killTimer 如何使用？

**答案**：

```cpp
#include <QObject>
#include <QTimerEvent>

class StartTimerDemo : public QObject {
    Q_OBJECT

public:
    StartTimerDemo() {
        // 启动定时器，返回定时器 ID
        m_timerId = startTimer(1000);  // 1000ms
        m_fastTimerId = startTimer(100, Qt::PreciseTimer);  // 精确定时器
    }

    ~StartTimerDemo() {
        // 销毁时自动停止定时器（QObject 析构会处理）
        // 也可以手动停止
        if (m_timerId != 0) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_timerId) {
            qDebug() << "Normal timer";
        } else if (event->timerId() == m_fastTimerId) {
            qDebug() << "Fast timer";
        }
    }

private:
    int m_timerId = 0;
    int m_fastTimerId = 0;
};

// 动态管理定时器
class DynamicTimerDemo : public QObject {
    Q_OBJECT

public:
    void addTimer(int interval) {
        int id = startTimer(interval);
        m_timerIds.insert(id, interval);
    }

    void removeTimer(int id) {
        if (m_timerIds.contains(id)) {
            killTimer(id);
            m_timerIds.remove(id);
        }
    }

    void removeAllTimers() {
        for (int id : m_timerIds.keys()) {
            killTimer(id);
        }
        m_timerIds.clear();
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        int id = event->timerId();
        if (m_timerIds.contains(id)) {
            qDebug() << "Timer" << id << "interval" << m_timerIds[id];
        }
    }

private:
    QHash<int, int> m_timerIds;  // id -> interval
};
```

---

## 5. QElapsedTimer（高精度计时）

### 5.1 基本用法

**面试问题**：如何进行高精度时间测量？

**答案**：

```cpp
#include <QElapsedTimer>
#include <QDebug>

void elapsedTimerDemo() {
    QElapsedTimer timer;

    // 启动计时
    timer.start();

    // ... 执行一些操作 ...
    doSomeWork();

    // 获取经过的时间
    qint64 elapsedMs = timer.elapsed();      // 毫秒
    qint64 elapsedNs = timer.nsecsElapsed(); // 纳秒

    qDebug() << "Elapsed:" << elapsedMs << "ms";
    qDebug() << "Elapsed:" << elapsedNs << "ns";

    // 重新启动
    timer.restart();  // 返回自上次 start/restart 以来的毫秒数

    // 检查是否超时
    if (timer.hasExpired(5000)) {  // 是否超过 5000ms
        qDebug() << "Timeout!";
    }

    // 检查是否有效
    if (timer.isValid()) {
        // 定时器已启动
    }

    // 使计时器无效
    timer.invalidate();
}

// 性能测量示例
void measurePerformance() {
    const int iterations = 1000000;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
        // 被测试的代码
        QString s = QString::number(i);
    }

    double avgNs = static_cast<double>(timer.nsecsElapsed()) / iterations;
    qDebug() << "Average time per iteration:" << avgNs << "ns";
}

// 时钟类型
void clockTypes() {
    // 查询系统支持的时钟类型
    QElapsedTimer::ClockType type = QElapsedTimer::clockType();

    /*
    可能的时钟类型：
    - SystemTime: 系统时钟（可能受时间调整影响）
    - MonotonicClock: 单调时钟（推荐，不受时间调整影响）
    - TickCounter: Windows 高性能计数器
    - MachAbsoluteTime: macOS 高精度时钟
    - PerformanceCounter: Windows 性能计数器
    */

    // 检查是否单调时钟
    if (QElapsedTimer::isMonotonic()) {
        qDebug() << "Using monotonic clock";
    }
}
```

---

## 6. 定时器精度与陷阱

### 6.1 精度问题

**面试问题**：Qt 定时器的精度如何？有什么限制？

**答案**：

```cpp
// 精度限制因素
class TimerPrecision {
public:
    void limitations() {
        /*
        1. 系统精度限制
           - Windows: 默认 15.6ms，可调整到 1ms
           - Linux: 通常 1-4ms
           - macOS: 通常 1ms

        2. 事件循环阻塞
           - 如果主线程被阻塞，定时器无法触发
           - 定时器事件会排队，可能堆积

        3. 事件处理时间
           - 定时器回调执行时间过长会影响下次触发
           - 间隔时间是两次触发的起点间隔，不是结束到开始

        4. 系统负载
           - 高负载时精度下降
           - 定时器可能延迟触发
        */
    }
};

// 演示精度问题
class PrecisionDemo : public QObject {
    Q_OBJECT

public:
    PrecisionDemo() {
        m_timer = new QTimer(this);
        m_timer->setTimerType(Qt::PreciseTimer);
        connect(m_timer, &QTimer::timeout, this, &PrecisionDemo::onTimeout);
    }

    void start() {
        m_elapsed.start();
        m_lastTime = 0;
        m_timer->start(100);  // 期望每 100ms
    }

private slots:
    void onTimeout() {
        qint64 now = m_elapsed.elapsed();
        qint64 delta = now - m_lastTime;
        m_lastTime = now;

        // 实际间隔可能不是精确的 100ms
        qDebug() << "Actual interval:" << delta << "ms";

        // ❌ 不要这样做：耗时操作阻塞事件循环
        // QThread::msleep(50);
    }

private:
    QTimer* m_timer;
    QElapsedTimer m_elapsed;
    qint64 m_lastTime;
};
```

---

### 6.2 常见陷阱

```cpp
// 陷阱 1：定时器泄漏
class LeakDemo : public QObject {
    Q_OBJECT

public:
    void leakyStart() {
        // ❌ 错误：每次调用都创建新定时器
        QTimer* timer = new QTimer(this);
        timer->start(1000);
        // timer 会一直存在，可能创建多个

        // ✅ 正确：检查是否已存在
        if (!m_timer) {
            m_timer = new QTimer(this);
            connect(m_timer, &QTimer::timeout, this, &LeakDemo::onTimeout);
        }
        m_timer->start(1000);
    }

private slots:
    void onTimeout() {}

private:
    QTimer* m_timer = nullptr;
};

// 陷阱 2：在没有事件循环的线程中使用
void noEventLoop() {
    // ❌ 错误：这个定时器永远不会触发
    QTimer timer;
    timer.start(1000);
    // 没有 exec()，定时器无法工作
}

// 陷阱 3：递归触发导致栈溢出
class RecursiveDemo : public QObject {
    Q_OBJECT

public:
    RecursiveDemo() {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &RecursiveDemo::onTimeout);
    }

private slots:
    void onTimeout() {
        // ❌ 危险：如果处理时间超过间隔，会不断累积
        doHeavyWork();

        // ✅ 更安全的做法：单次触发 + 完成后重新启动
        // m_timer->stop();
        // doHeavyWork();
        // m_timer->start(100);
    }

private:
    void doHeavyWork() { /* 耗时操作 */ }
    QTimer* m_timer;
};

// 陷阱 4：singleShot 捕获的对象已被删除
void danglingCapture() {
    QWidget* widget = new QWidget();

    // ❌ 危险：widget 可能在回调前被删除
    QTimer::singleShot(5000, [widget]() {
        widget->show();  // 可能访问已删除对象
    });

    // ✅ 正确：使用 QPointer 或提供上下文
    QPointer<QWidget> safeWidget = widget;
    QTimer::singleShot(5000, [safeWidget]() {
        if (safeWidget) {
            safeWidget->show();
        }
    });

    // ✅ 或者提供上下文对象
    QTimer::singleShot(5000, widget, [widget]() {
        widget->show();  // widget 被删除时回调自动取消
    });
}
```

---

## 7. 高级技巧

### 7.1 零延迟定时器

**面试问题**：什么是零延迟定时器？有什么用途？

**答案**：

```cpp
#include <QTimer>
#include <QCoreApplication>

// 零延迟定时器：在事件循环空闲时触发
void zeroIntervalTimer() {
    QTimer* idleTimer = new QTimer();
    idleTimer->setInterval(0);  // 零延迟

    QObject::connect(idleTimer, &QTimer::timeout, []() {
        // 在事件循环空闲时执行
        // 用于后台任务、懒加载等
        static int count = 0;
        qDebug() << "Idle work" << ++count;

        // 处理一小批数据，避免阻塞 UI
        QCoreApplication::processEvents();  // 让出控制权
    });

    idleTimer->start();

    // 用途：
    // 1. 批量处理数据，每次处理一部分
    // 2. 后台搜索/过滤
    // 3. 懒加载
    // 4. 空闲时预取数据
}

// 分批处理大量数据
class BatchProcessor : public QObject {
    Q_OBJECT

public:
    void processItems(const QList<int>& items) {
        m_items = items;
        m_currentIndex = 0;

        // 使用零延迟定时器分批处理
        QTimer::singleShot(0, this, &BatchProcessor::processNextBatch);
    }

private slots:
    void processNextBatch() {
        const int batchSize = 100;
        int end = qMin(m_currentIndex + batchSize, m_items.size());

        for (int i = m_currentIndex; i < end; ++i) {
            processItem(m_items[i]);
        }

        m_currentIndex = end;

        if (m_currentIndex < m_items.size()) {
            // 还有更多数据，继续处理
            QTimer::singleShot(0, this, &BatchProcessor::processNextBatch);
        } else {
            emit finished();
        }
    }

signals:
    void finished();

private:
    void processItem(int item) { /* 处理单个项 */ }

    QList<int> m_items;
    int m_currentIndex = 0;
};
```

---

### 7.2 定时器精度提升

```cpp
// Windows 提升定时器精度
#ifdef Q_OS_WIN
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class WindowsTimerPrecision {
public:
    WindowsTimerPrecision() {
        // 设置系统定时器精度为 1ms
        timeBeginPeriod(1);
    }

    ~WindowsTimerPrecision() {
        // 恢复默认精度
        timeEndPeriod(1);
    }
};

// 在应用启动时创建
// static WindowsTimerPrecision s_precision;
#endif

// 使用高精度定时器的动画
class SmoothAnimation : public QObject {
    Q_OBJECT

public:
    SmoothAnimation() {
        m_timer = new QTimer(this);
        m_timer->setTimerType(Qt::PreciseTimer);
        m_timer->setInterval(16);  // ~60fps
        connect(m_timer, &QTimer::timeout, this, &SmoothAnimation::update);
    }

    void start() {
        m_elapsed.start();
        m_timer->start();
    }

private slots:
    void update() {
        // 使用实际经过的时间，而不是假设固定间隔
        qreal dt = m_elapsed.restart() / 1000.0;

        // 基于实际时间更新动画
        m_position += m_velocity * dt;

        emit positionChanged(m_position);
    }

signals:
    void positionChanged(qreal position);

private:
    QTimer* m_timer;
    QElapsedTimer m_elapsed;
    qreal m_position = 0;
    qreal m_velocity = 100;  // 像素/秒
};
```

---

## 8. 面试问题精选

### Q1: 定时器类型选择

**问题**：QTimer、QBasicTimer、startTimer 如何选择？

**参考答案**：

| 场景 | 推荐 |
|------|------|
| 一般用途，需要信号槽 | QTimer |
| 大量定时器，性能敏感 | QBasicTimer |
| 简单场景，已有 QObject | startTimer() |
| 单次延迟执行 | QTimer::singleShot() |

QTimer 功能最完整但开销稍大，QBasicTimer 更轻量但需要重写 timerEvent。

---

### Q2: 定时器精度

**问题**：Qt 定时器的精度如何？如何提高精度？

**参考答案**：

精度受限于：
1. 系统时钟精度（Windows 默认 15.6ms）
2. 事件循环是否阻塞
3. 系统负载

提高精度的方法：
1. 使用 `Qt::PreciseTimer`
2. Windows 下调用 `timeBeginPeriod(1)`
3. 避免主线程阻塞
4. 使用真实时间差（QElapsedTimer）而非假设固定间隔

---

### Q3: 线程中使用定时器

**问题**：如何在子线程中使用 QTimer？

**参考答案**：

两种方式：
1. **继承 QThread，在 run() 中创建定时器并调用 exec()**
2. **使用 moveToThread**（推荐）

关键点：
- 必须有事件循环（exec()）
- 定时器只能在其所属线程启动
- 跨线程操作需要用信号槽

---

### Q4: 零延迟定时器

**问题**：什么是零延迟定时器？有什么用途？

**参考答案**：

零延迟定时器（interval = 0）在事件循环空闲时触发。

用途：
1. 分批处理大量数据，避免阻塞 UI
2. 后台任务（搜索、过滤）
3. 懒加载
4. 空闲时预取

它不是"立即执行"，而是"在处理完其他事件后尽快执行"。

---

### Q5: singleShot 的陷阱

**问题**：QTimer::singleShot 使用 Lambda 时需要注意什么？

**参考答案**：

主要问题：捕获的对象可能在回调前被删除。

解决方案：
1. 使用 QPointer 检查对象有效性
2. 提供上下文对象作为第二个参数：`singleShot(ms, context, lambda)`
   - 当 context 被删除时，回调自动取消

---

## 9. 总结

| 定时器 | 特点 | 适用场景 |
|--------|------|----------|
| QTimer | 完整功能，信号槽 | 通用场景 |
| QBasicTimer | 轻量级 | 大量定时器 |
| startTimer() | QObject 内置 | 简单场景 |
| QElapsedTimer | 高精度计时 | 性能测量 |

**核心要点**：

1. 定时器依赖事件循环，阻塞会导致定时器失效
2. Qt::PreciseTimer 提高精度但开销更大
3. 子线程使用定时器必须有事件循环
4. 使用 QElapsedTimer 基于真实时间差进行动画
5. singleShot 配合上下文对象避免悬垂引用
6. 零延迟定时器用于空闲时处理任务
