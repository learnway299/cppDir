# Qt 事件循环（Event Loop）

## 概述

事件循环是 Qt 应用程序的心脏，它不断从事件队列中取出事件并分发处理。理解事件循环对于编写响应式、非阻塞的 Qt 应用程序至关重要。

---

## 1. 事件循环基础

### 1.1 什么是事件循环？

**面试问题**：什么是 Qt 事件循环？它的作用是什么？

**答案**：

事件循环是一个无限循环，不断地：
1. 等待事件发生
2. 从队列中取出事件
3. 分发事件给目标对象
4. 重复以上步骤

```cpp
#include <QCoreApplication>
#include <QDebug>

// 事件循环的伪代码实现
void pseudoEventLoop() {
    while (!shouldQuit) {
        // 1. 等待并获取下一个事件
        QEvent* event = waitForNextEvent();

        // 2. 找到事件的目标对象
        QObject* receiver = event->receiver();

        // 3. 分发事件
        QCoreApplication::sendEvent(receiver, event);

        // 4. 处理延迟删除的对象
        processDeferredDeletes();

        // 5. 处理已投递的事件
        processPostedEvents();
    }
}

// 实际使用
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    qDebug() << "Before event loop";

    // exec() 启动事件循环，阻塞直到 quit() 被调用
    int result = app.exec();

    qDebug() << "After event loop, result:" << result;
    return result;
}
```

**事件循环的职责**：

| 职责 | 说明 |
|------|------|
| 事件分发 | 将事件传递给正确的对象 |
| 信号槽调用 | 处理队列连接的槽调用 |
| 定时器管理 | 触发到期的定时器 |
| 网络 IO | 处理 socket 事件 |
| 延迟删除 | 执行 deleteLater() |
| 系统集成 | 与操作系统消息循环交互 |

---

### 1.2 QCoreApplication、QGuiApplication、QApplication

**面试问题**：这三个类有什么区别？

**答案**：

```cpp
// 1. QCoreApplication - 无 GUI 应用程序
// 用于：命令行工具、服务、后台程序
#include <QCoreApplication>

int consoleApp(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // 可以使用：信号槽、定时器、网络、文件 IO、线程等
    // 不能使用：任何 GUI 组件

    QTimer::singleShot(1000, &app, &QCoreApplication::quit);
    return app.exec();
}

// 2. QGuiApplication - 基础 GUI 应用程序
// 用于：Qt Quick/QML 应用、自定义渲染
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int qmlApp(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    // 可以使用：窗口系统、OpenGL、QML
    // 不能使用：Qt Widgets

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));
    return app.exec();
}

// 3. QApplication - 完整 GUI 应用程序
// 用于：Qt Widgets 应用
#include <QApplication>
#include <QWidget>

int widgetApp(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 可以使用：所有 Qt 功能，包括 Widgets
    // 提供：样式、字体、调色板等管理

    QWidget window;
    window.show();
    return app.exec();
}
```

**对比**：

| 特性 | QCoreApplication | QGuiApplication | QApplication |
|------|------------------|-----------------|--------------|
| GUI 支持 | ❌ | ✅ 基础 | ✅ 完整 |
| Widgets | ❌ | ❌ | ✅ |
| QML | ❌ | ✅ | ✅ |
| 事件循环 | ✅ | ✅ | ✅ |
| 依赖库 | QtCore | QtCore + QtGui | QtCore + QtGui + QtWidgets |

---

### 1.3 exec() 和 quit()

**面试问题**：exec() 和 quit() 是如何工作的？

**答案**：

```cpp
#include <QCoreApplication>
#include <QTimer>

class AppController : public QObject {
    Q_OBJECT

public:
    void start() {
        qDebug() << "App starting...";

        // 设置 5 秒后退出
        QTimer::singleShot(5000, this, &AppController::onTimeout);
    }

private slots:
    void onTimeout() {
        qDebug() << "Timeout, quitting...";

        // 方式 1：正常退出（返回码 0）
        QCoreApplication::quit();

        // 方式 2：带返回码退出
        // QCoreApplication::exit(1);

        // 注意：quit/exit 不会立即返回
        // 它们设置一个标志，事件循环在下次迭代时检测并退出
        qDebug() << "quit() called, but still executing...";
    }
};

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    AppController controller;
    controller.start();

    qDebug() << "Entering event loop...";

    // exec() 阻塞，直到 quit()/exit() 被调用
    int result = app.exec();

    qDebug() << "Event loop exited with code:" << result;
    return result;
}

// 输出：
// App starting...
// Entering event loop...
// (5 秒后)
// Timeout, quitting...
// quit() called, but still executing...
// Event loop exited with code: 0
```

**多个事件循环**：

```cpp
// 嵌套事件循环（模态对话框原理）
void nestedEventLoop() {
    qDebug() << "Outer code running";

    // 创建局部事件循环
    QEventLoop loop;

    // 设置退出条件
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);

    qDebug() << "Starting nested loop";
    loop.exec();  // 阻塞 2 秒
    qDebug() << "Nested loop exited";

    // 继续外部代码
    qDebug() << "Back to outer code";
}

// 带超时的等待
bool waitForSignal(QObject* obj, const char* signal, int timeout) {
    QEventLoop loop;

    // 信号触发时退出循环
    QObject::connect(obj, signal, &loop, SLOT(quit()));

    // 超时退出
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeout);

    loop.exec();

    return timer.isActive();  // true = 信号触发，false = 超时
}
```

---

## 2. 事件处理机制

### 2.1 processEvents()

**面试问题**：processEvents() 有什么用？什么时候使用它？

**答案**：

```cpp
#include <QCoreApplication>
#include <QProgressDialog>

// 场景：长时间运算保持界面响应
void longOperation(QProgressDialog* progress) {
    for (int i = 0; i < 1000000; ++i) {
        // 执行耗时操作
        doSomething(i);

        // 定期处理事件，保持界面响应
        if (i % 1000 == 0) {
            progress->setValue(i / 10000);

            // 处理待处理的事件（包括重绘、用户输入）
            QCoreApplication::processEvents();

            // 检查用户是否取消
            if (progress->wasCanceled()) {
                break;
            }
        }
    }
}

// processEvents 的选项
void processEventsOptions() {
    // 处理所有待处理的事件
    QCoreApplication::processEvents();

    // 只处理特定类型的事件
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // 处理事件，如果没有事件则等待
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

    // 处理所有事件，包括延迟删除
    QCoreApplication::processEvents(QEventLoop::AllEvents);

    // 设置最大处理时间
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);  // 最多 100ms
}
```

**processEvents 的陷阱**：

```cpp
class DangerousExample : public QObject {
    Q_OBJECT

public:
    void process() {
        m_processing = true;

        while (m_processing) {
            doWork();

            // 危险：可能导致重入问题
            QCoreApplication::processEvents();

            // 如果在 processEvents 期间触发了某个信号
            // 而该信号的槽调用了 process()，就会重入
        }
    }

public slots:
    void stop() {
        m_processing = false;
    }

private:
    bool m_processing = false;
};

// 更安全的做法：使用标志防止重入
class SaferExample : public QObject {
    Q_OBJECT

public:
    void process() {
        if (m_reentryGuard) return;  // 防止重入
        m_reentryGuard = true;

        m_processing = true;
        while (m_processing) {
            doWork();
            QCoreApplication::processEvents();
        }

        m_reentryGuard = false;
    }

private:
    bool m_processing = false;
    bool m_reentryGuard = false;
};

// 最佳做法：使用异步方式
class BestPractice : public QObject {
    Q_OBJECT

public:
    void startProcessing() {
        // 使用定时器分批处理，不阻塞事件循环
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &BestPractice::processChunk);
        m_timer->start(0);  // 空闲时处理
    }

private slots:
    void processChunk() {
        // 处理一小块工作
        for (int i = 0; i < 100 && m_index < m_total; ++i, ++m_index) {
            doWork(m_index);
        }

        if (m_index >= m_total) {
            m_timer->stop();
            emit finished();
        }
    }

signals:
    void finished();

private:
    QTimer* m_timer = nullptr;
    int m_index = 0;
    int m_total = 10000;
};
```

---

### 2.2 事件队列

**面试问题**：Qt 的事件队列是如何工作的？

**答案**：

```cpp
#include <QCoreApplication>

// Qt 维护一个事件队列，postEvent 向队列添加事件
void eventQueueDemo() {
    QObject receiver;

    // 1. 普通优先级
    QCoreApplication::postEvent(&receiver,
        new QEvent(QEvent::User));

    // 2. 高优先级（插入队列前面）
    QCoreApplication::postEvent(&receiver,
        new QEvent(QEvent::User),
        Qt::HighEventPriority);

    // 3. 低优先级（插入队列后面）
    QCoreApplication::postEvent(&receiver,
        new QEvent(QEvent::User),
        Qt::LowEventPriority);

    // 4. 普通优先级的具体值
    // Qt::HighEventPriority = INT_MAX
    // Qt::NormalEventPriority = 0
    // Qt::LowEventPriority = -INT_MAX

    // 事件处理顺序：高优先级 → 普通优先级 → 低优先级
}

// 事件压缩（Event Compression）
void eventCompressionDemo(QWidget* widget) {
    // Qt 自动压缩某些事件类型
    // 多个相同类型的事件会合并为一个

    // 例如：paint 事件会被合并
    widget->update();
    widget->update();
    widget->update();
    // 只会产生一个 paintEvent

    // move/resize 事件也会被压缩
    widget->move(10, 10);
    widget->move(20, 20);
    widget->move(30, 30);
    // 可能只处理最后一个位置

    // 手动发送事件不会被压缩
    QCoreApplication::postEvent(widget, new QPaintEvent(widget->rect()));
    QCoreApplication::postEvent(widget, new QPaintEvent(widget->rect()));
    // 两个事件都会被处理
}

// 清除特定对象的事件
void removeEventsDemo(QObject* obj) {
    // 移除对象的所有待处理事件
    QCoreApplication::removePostedEvents(obj);

    // 移除特定类型的事件
    QCoreApplication::removePostedEvents(obj, QEvent::Timer);
    QCoreApplication::removePostedEvents(obj, QEvent::MetaCall);
}
```

---

### 2.3 定时器与事件循环

**面试问题**：Qt 定时器与事件循环的关系是什么？

**答案**：

```cpp
#include <QTimer>
#include <QElapsedTimer>

class TimerDemo : public QObject {
    Q_OBJECT

public:
    void demonstrateTimers() {
        // 1. QTimer（推荐）
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TimerDemo::onTimeout);
        timer->start(1000);  // 1 秒间隔

        // 2. 单次定时器
        QTimer::singleShot(5000, this, &TimerDemo::onSingleShot);

        // 3. Lambda 单次定时器
        QTimer::singleShot(2000, []() {
            qDebug() << "Lambda timer fired";
        });

        // 4. 基础定时器 API（轻量级）
        m_basicTimerId = startTimer(500);  // 500ms

        // 5. 精确定时器（Qt 5.0+）
        QTimer* preciseTimer = new QTimer(this);
        preciseTimer->setTimerType(Qt::PreciseTimer);
        preciseTimer->start(16);  // ~60 FPS

        // 定时器类型：
        // Qt::PreciseTimer - 精确到毫秒
        // Qt::CoarseTimer - 5% 误差，省电
        // Qt::VeryCoarseTimer - 秒级精度，最省电
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_basicTimerId) {
            qDebug() << "Basic timer fired";
        }
    }

private slots:
    void onTimeout() {
        qDebug() << "QTimer timeout";
    }

    void onSingleShot() {
        qDebug() << "Single shot timer fired";
    }

private:
    int m_basicTimerId = 0;
};

// 定时器精度测试
void timerAccuracyTest() {
    QElapsedTimer elapsed;
    int count = 0;

    QTimer timer;
    timer.setInterval(100);  // 期望 100ms

    QObject::connect(&timer, &QTimer::timeout, [&]() {
        qint64 actualInterval = elapsed.restart();
        qDebug() << "Interval:" << actualInterval << "ms";

        if (++count >= 10) {
            QCoreApplication::quit();
        }
    });

    elapsed.start();
    timer.start();

    // 注意：定时器精度取决于：
    // 1. 事件循环是否被阻塞
    // 2. 系统负载
    // 3. 定时器类型设置
}
```

**定时器与事件循环阻塞**：

```cpp
void timerBlockingDemo() {
    QTimer timer;
    timer.setInterval(100);

    QObject::connect(&timer, &QTimer::timeout, []() {
        qDebug() << "Timer fired at" << QTime::currentTime();
    });

    timer.start();

    // 如果事件循环被阻塞...
    QTimer::singleShot(500, []() {
        qDebug() << "Starting blocking operation";

        // 这会阻塞事件循环 2 秒
        QThread::sleep(2);

        qDebug() << "Blocking operation done";
        // 在这 2 秒内，定时器不会触发！
        // 阻塞结束后，不会补发错过的定时器事件
    });
}
```

---

## 3. 异步编程模式

### 3.1 使用信号槽的异步模式

**面试问题**：如何在 Qt 中实现异步操作？

**答案**：

```cpp
#include <QNetworkAccessManager>
#include <QNetworkReply>

// 模式 1：信号槽异步
class AsyncDownloader : public QObject {
    Q_OBJECT

public:
    void download(const QUrl& url) {
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);

        QNetworkReply* reply = manager->get(QNetworkRequest(url));

        // 异步：连接信号，立即返回
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray data = reply->readAll();
                emit downloadFinished(data);
            } else {
                emit downloadError(reply->errorString());
            }
            reply->deleteLater();
        });

        // 进度报告
        connect(reply, &QNetworkReply::downloadProgress,
                this, &AsyncDownloader::progressChanged);
    }

signals:
    void downloadFinished(const QByteArray& data);
    void downloadError(const QString& error);
    void progressChanged(qint64 received, qint64 total);
};

// 模式 2：QFuture 异步（Qt Concurrent）
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

class FutureExample : public QObject {
    Q_OBJECT

public:
    void startHeavyComputation() {
        // 在线程池中执行
        QFuture<int> future = QtConcurrent::run([]() {
            // 耗时计算
            int result = 0;
            for (int i = 0; i < 1000000; ++i) {
                result += i;
            }
            return result;
        });

        // 使用 watcher 监控完成
        QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);

        connect(watcher, &QFutureWatcher<int>::finished, this, [this, watcher]() {
            int result = watcher->result();
            qDebug() << "Computation result:" << result;
            emit computationFinished(result);
            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }

signals:
    void computationFinished(int result);
};
```

---

### 3.2 基于事件循环的等待

**面试问题**：如何实现同步等待异步操作完成？

**答案**：

```cpp
#include <QEventLoop>
#include <QTimer>

// 方式 1：QEventLoop 等待信号
class SyncWaitExample {
public:
    QByteArray downloadSync(const QUrl& url, int timeout = 30000) {
        QNetworkAccessManager manager;
        QNetworkReply* reply = manager.get(QNetworkRequest(url));

        // 创建局部事件循环
        QEventLoop loop;

        // 连接完成信号到退出循环
        QObject::connect(reply, &QNetworkReply::finished,
                         &loop, &QEventLoop::quit);

        // 超时处理
        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(timeout);

        // 阻塞等待
        loop.exec();

        // 检查结果
        if (!timer.isActive()) {
            // 超时
            reply->abort();
            reply->deleteLater();
            throw std::runtime_error("Download timeout");
        }

        timer.stop();
        QByteArray data = reply->readAll();
        reply->deleteLater();
        return data;
    }
};

// 方式 2：QSignalSpy（测试用）
#include <QSignalSpy>

void testAsyncOperation() {
    AsyncDownloader downloader;

    QSignalSpy spy(&downloader, &AsyncDownloader::downloadFinished);

    downloader.download(QUrl("http://example.com"));

    // 等待信号，最多 5 秒
    QVERIFY(spy.wait(5000));

    // 检查信号参数
    QList<QVariant> arguments = spy.takeFirst();
    QByteArray data = arguments.at(0).toByteArray();
    QVERIFY(!data.isEmpty());
}

// 方式 3：自定义等待函数
template<typename Func>
bool waitFor(Func condition, int timeout = 5000) {
    QElapsedTimer timer;
    timer.start();

    while (!condition()) {
        if (timer.elapsed() > timeout) {
            return false;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    return true;
}

// 使用示例
void waitForExample() {
    bool operationComplete = false;

    // 启动异步操作
    QTimer::singleShot(1000, [&]() {
        operationComplete = true;
    });

    // 等待条件满足
    if (waitFor([&]() { return operationComplete; }, 5000)) {
        qDebug() << "Operation completed";
    } else {
        qDebug() << "Timeout";
    }
}
```

---

### 3.3 避免阻塞事件循环

**面试问题**：为什么不能阻塞事件循环？如何避免？

**答案**：

```cpp
// 阻塞事件循环的后果
void blockingConsequences() {
    // 1. 界面冻结（无法重绘）
    // 2. 用户输入无响应
    // 3. 定时器不触发
    // 4. 网络请求无法完成
    // 5. 动画停止
    // 6. 系统可能判定程序"无响应"
}

// 错误示例：在主线程中阻塞
class BadExample : public QMainWindow {
    void onButtonClicked() {
        // 错误：阻塞主线程
        QThread::sleep(5);  // 界面冻结 5 秒！

        // 错误：忙等待
        while (!conditionMet) {
            // 界面完全冻结
        }
    }
};

// 正确做法 1：使用工作线程
class WorkerThread : public QThread {
    Q_OBJECT

protected:
    void run() override {
        // 在独立线程中执行耗时操作
        QThread::sleep(5);
        emit workDone();
    }

signals:
    void workDone();
};

class GoodExample1 : public QMainWindow {
    Q_OBJECT

    void onButtonClicked() {
        WorkerThread* worker = new WorkerThread(this);
        connect(worker, &WorkerThread::workDone, this, &GoodExample1::onWorkDone);
        connect(worker, &QThread::finished, worker, &QObject::deleteLater);
        worker->start();
    }

private slots:
    void onWorkDone() {
        qDebug() << "Work completed, UI still responsive";
    }
};

// 正确做法 2：分块处理
class GoodExample2 : public QObject {
    Q_OBJECT

public:
    void processLargeData(const QList<int>& data) {
        m_data = data;
        m_index = 0;

        // 使用定时器分块处理
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &GoodExample2::processChunk);
        m_timer->start(0);  // 空闲时处理
    }

private slots:
    void processChunk() {
        // 每次处理一小块
        int chunkSize = 100;
        int end = qMin(m_index + chunkSize, m_data.size());

        for (int i = m_index; i < end; ++i) {
            processItem(m_data[i]);
        }

        m_index = end;

        if (m_index >= m_data.size()) {
            m_timer->stop();
            m_timer->deleteLater();
            emit processingComplete();
        }
    }

signals:
    void processingComplete();

private:
    QList<int> m_data;
    int m_index = 0;
    QTimer* m_timer = nullptr;

    void processItem(int item) { /* ... */ }
};

// 正确做法 3：Qt Concurrent
class GoodExample3 : public QObject {
    Q_OBJECT

public:
    void processData() {
        QFuture<void> future = QtConcurrent::run([this]() {
            // 在线程池中执行
            heavyComputation();
        });

        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
            qDebug() << "Done";
            watcher->deleteLater();
        });
        watcher->setFuture(future);
    }

private:
    void heavyComputation() { /* ... */ }
};
```

---

## 4. 线程与事件循环

### 4.1 每个线程的事件循环

**面试问题**：QThread 如何拥有自己的事件循环？

**答案**：

```cpp
#include <QThread>

// 方式 1：重写 run()，手动启动事件循环
class ThreadWithEventLoop : public QThread {
    Q_OBJECT

protected:
    void run() override {
        qDebug() << "Thread started, id:" << QThread::currentThreadId();

        // 创建只属于这个线程的对象
        QTimer timer;
        connect(&timer, &QTimer::timeout, this, &ThreadWithEventLoop::onTimeout);
        timer.start(1000);

        // 启动线程的事件循环
        exec();  // 阻塞直到 quit() 被调用

        qDebug() << "Thread event loop exited";
    }

private slots:
    void onTimeout() {
        qDebug() << "Timer in thread:" << QThread::currentThreadId();
    }
};

// 方式 2：使用 moveToThread（推荐）
class Worker : public QObject {
    Q_OBJECT

public slots:
    void doWork() {
        qDebug() << "Working in thread:" << QThread::currentThreadId();

        // 模拟工作
        QThread::sleep(1);

        emit workDone();
    }

signals:
    void workDone();
};

class Controller : public QObject {
    Q_OBJECT

public:
    Controller() {
        Worker* worker = new Worker();
        QThread* thread = new QThread();

        // 将 worker 移动到新线程
        worker->moveToThread(thread);

        // 连接信号
        connect(thread, &QThread::started, worker, &Worker::doWork);
        connect(worker, &Worker::workDone, thread, &QThread::quit);
        connect(thread, &QThread::finished, worker, &QObject::deleteLater);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        // 启动线程（自动启动事件循环）
        thread->start();
    }
};

// 线程事件循环的作用
void threadEventLoopPurpose() {
    // 1. 处理 queued connections 的槽调用
    // 2. 处理线程内的定时器
    // 3. 处理 postEvent 发送的事件
    // 4. 执行 deleteLater 的对象删除

    // 没有事件循环的线程：
    // - 无法接收 queued 信号
    // - 定时器不工作
    // - deleteLater 不执行
}
```

---

### 4.2 跨线程信号槽

**面试问题**：跨线程的信号槽是如何工作的？

**答案**：

```cpp
// 跨线程信号槽使用 QueuedConnection
class CrossThreadDemo : public QObject {
    Q_OBJECT

public:
    void demonstrate() {
        QThread* workerThread = new QThread();
        Worker* worker = new Worker();
        worker->moveToThread(workerThread);

        // 从主线程发信号到工作线程
        // 自动使用 QueuedConnection
        connect(this, &CrossThreadDemo::startWork,
                worker, &Worker::doWork);

        // 从工作线程发信号到主线程
        // 自动使用 QueuedConnection
        connect(worker, &Worker::resultReady,
                this, &CrossThreadDemo::handleResult);

        // 显式指定连接类型
        connect(this, &CrossThreadDemo::startWork,
                worker, &Worker::doWork,
                Qt::QueuedConnection);  // 异步，线程安全

        // Qt::BlockingQueuedConnection - 阻塞等待
        // 危险：可能导致死锁！
        connect(this, &CrossThreadDemo::syncRequest,
                worker, &Worker::syncResponse,
                Qt::BlockingQueuedConnection);

        workerThread->start();
    }

signals:
    void startWork();
    void syncRequest();

private slots:
    void handleResult(int result) {
        // 在主线程中执行
        qDebug() << "Result:" << result << "in thread:"
                 << QThread::currentThreadId();
    }
};

// 连接类型详解
void connectionTypesExplained() {
    // Qt::AutoConnection（默认）
    // - 同线程：DirectConnection
    // - 跨线程：QueuedConnection

    // Qt::DirectConnection
    // - 槽在信号发出的线程中立即执行
    // - 跨线程使用不安全

    // Qt::QueuedConnection
    // - 槽在接收者所在线程的事件循环中执行
    // - 线程安全
    // - 参数需要是可拷贝的（或注册过的类型）

    // Qt::BlockingQueuedConnection
    // - 像 Queued，但发送者阻塞等待槽执行完成
    // - 可能导致死锁

    // Qt::UniqueConnection
    // - 防止重复连接
    // - 可以与其他类型组合：Qt::QueuedConnection | Qt::UniqueConnection
}
```

---

### 4.3 线程安全的事件投递

**面试问题**：postEvent 是线程安全的吗？

**答案**：

```cpp
// postEvent 是线程安全的，可以从任何线程调用
class ThreadSafeEventDemo {
public:
    void sendEventFromAnyThread(QObject* receiver) {
        // 安全：从任何线程调用
        QCoreApplication::postEvent(receiver, new QEvent(QEvent::User));

        // 事件会被放入接收者所在线程的事件队列
        // 在该线程的事件循环中处理
    }

    // 对比：sendEvent 不是线程安全的
    void sendEventMustBeSameThread(QObject* receiver) {
        // 只能在接收者所在的线程中调用
        // 否则行为未定义
        QCoreApplication::sendEvent(receiver, new QEvent(QEvent::User));
    }
};

// 实用模式：从工作线程更新 UI
class UIUpdater : public QObject {
    Q_OBJECT

public:
    // 从任何线程安全地更新 UI
    void updateUIFromThread(QWidget* widget, const QString& text) {
        // 检查是否在 UI 线程
        if (QThread::currentThread() == widget->thread()) {
            // 已在 UI 线程，直接更新
            updateWidgetText(widget, text);
        } else {
            // 在其他线程，投递事件
            QMetaObject::invokeMethod(widget, [widget, text]() {
                // 这个 lambda 会在 widget 的线程中执行
                widget->setWindowTitle(text);
            }, Qt::QueuedConnection);
        }
    }

private:
    void updateWidgetText(QWidget* widget, const QString& text) {
        widget->setWindowTitle(text);
    }
};
```

---

## 5. 高级主题

### 5.1 事件调度器

**面试问题**：Qt 的事件调度器是如何工作的？

**答案**：

```cpp
// Qt 使用平台特定的事件调度器
// - Windows: QWindowsEventDispatcher (基于 Windows 消息循环)
// - macOS: QCocoaEventDispatcher (基于 CFRunLoop)
// - Linux: QUnixEventDispatcher (基于 select/poll/epoll)

// 事件调度器的职责
void eventDispatcherRole() {
    // 1. 与操作系统事件系统集成
    // 2. 管理定时器
    // 3. 管理 socket notifiers
    // 4. 唤醒事件循环

    // 获取当前事件调度器
    QAbstractEventDispatcher* dispatcher =
        QAbstractEventDispatcher::instance();

    if (dispatcher) {
        // 在事件循环空闲时执行
        QObject::connect(dispatcher, &QAbstractEventDispatcher::aboutToBlock,
                         []() {
            qDebug() << "Event loop about to block (idle)";
        });

        // 事件循环被唤醒时
        QObject::connect(dispatcher, &QAbstractEventDispatcher::awake,
                         []() {
            qDebug() << "Event loop awakened";
        });
    }
}

// 手动唤醒事件循环
void wakeUpEventLoop() {
    QAbstractEventDispatcher* dispatcher =
        QAbstractEventDispatcher::instance();

    if (dispatcher) {
        // 从其他线程唤醒事件循环
        dispatcher->wakeUp();
    }
}
```

---

### 5.2 自定义事件调度

**面试问题**：如何在空闲时执行任务？

**答案**：

```cpp
// 方式 1：0 间隔定时器
class IdleProcessor : public QObject {
    Q_OBJECT

public:
    void start() {
        m_timer = new QTimer(this);
        m_timer->setInterval(0);  // 0 间隔 = 空闲时执行
        connect(m_timer, &QTimer::timeout, this, &IdleProcessor::processIdle);
        m_timer->start();
    }

private slots:
    void processIdle() {
        // 在事件循环空闲时执行
        if (hasWork()) {
            doSomeWork();
        } else {
            m_timer->stop();
        }
    }

private:
    QTimer* m_timer = nullptr;
    bool hasWork() { return true; }
    void doSomeWork() { /* ... */ }
};

// 方式 2：使用 aboutToBlock 信号
class IdleHandler : public QObject {
    Q_OBJECT

public:
    IdleHandler() {
        QAbstractEventDispatcher* dispatcher =
            QAbstractEventDispatcher::instance();

        if (dispatcher) {
            connect(dispatcher, &QAbstractEventDispatcher::aboutToBlock,
                    this, &IdleHandler::onIdle);
        }
    }

private slots:
    void onIdle() {
        // 事件循环即将阻塞（空闲）
        if (hasPendingWork()) {
            doIdleWork();
            // 如果还有工作，确保事件循环不阻塞
            QCoreApplication::processEvents();
        }
    }

private:
    bool hasPendingWork() { return false; }
    void doIdleWork() { /* ... */ }
};

// 方式 3：低优先级事件
class LowPriorityWork : public QObject {
    Q_OBJECT

public:
    void scheduleWork() {
        // 投递低优先级事件
        QCoreApplication::postEvent(this,
            new QEvent(QEvent::User),
            Qt::LowEventPriority);
    }

protected:
    bool event(QEvent* e) override {
        if (e->type() == QEvent::User) {
            doWork();

            // 如果还有工作，继续调度
            if (hasMoreWork()) {
                scheduleWork();
            }
            return true;
        }
        return QObject::event(e);
    }

private:
    void doWork() { /* ... */ }
    bool hasMoreWork() { return false; }
};
```

---

## 6. 常见面试题

### Q1: 为什么 GUI 操作必须在主线程？

**答案**：

```cpp
// 原因：
// 1. GUI 框架（包括 Qt Widgets）通常不是线程安全的
// 2. 底层图形系统（X11、Windows GDI、Cocoa）要求在主线程操作
// 3. 事件处理假设在单一线程中进行

// 错误示例
void dangerousGUIAccess() {
    QtConcurrent::run([this]() {
        // 危险：在工作线程中操作 GUI
        m_label->setText("Updated");  // 可能崩溃！
    });
}

// 正确做法
void safeGUIAccess() {
    QtConcurrent::run([this]() {
        QString result = computeResult();

        // 通过信号槽更新 GUI
        QMetaObject::invokeMethod(m_label, "setText",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, result));

        // 或使用 lambda
        QMetaObject::invokeMethod(this, [this, result]() {
            m_label->setText(result);
        }, Qt::QueuedConnection);
    });
}
```

---

### Q2: deleteLater() 何时执行？

**答案**：

```cpp
void deleteLaterTiming() {
    QObject* obj = new QObject();

    obj->deleteLater();

    // deleteLater 的执行时机：
    // 1. 当控制返回事件循环时
    // 2. 具体是在处理 QEvent::DeferredDelete 事件时

    // 如果没有事件循环运行：
    // - deleteLater 投递的事件不会被处理
    // - 对象不会被删除（内存泄漏）

    // 示例：
    void noEventLoop() {
        QObject* obj = new QObject();
        obj->deleteLater();
        // 程序退出，obj 泄漏！
    }

    // 解决方案
    void withEventLoop() {
        QCoreApplication app(argc, argv);

        QObject* obj = new QObject();
        obj->deleteLater();

        QTimer::singleShot(0, &app, &QCoreApplication::quit);

        app.exec();  // 事件循环处理 deleteLater
    }
}
```

---

### Q3: 如何安全地从工作线程退出应用？

**答案**：

```cpp
class SafeQuitExample : public QObject {
    Q_OBJECT

public:
    void startWork() {
        QThread* thread = new QThread();
        Worker* worker = new Worker();
        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &Worker::doWork);
        connect(worker, &Worker::finished, this, &SafeQuitExample::onWorkFinished);
        connect(worker, &Worker::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, worker, &QObject::deleteLater);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();
    }

private slots:
    void onWorkFinished() {
        // 安全：在主线程中调用 quit
        QCoreApplication::quit();
    }
};

// 错误做法
void dangerousQuit() {
    QtConcurrent::run([]() {
        // 危险：从工作线程调用
        // QCoreApplication::quit();  // 可能不安全

        // 安全做法：投递到主线程
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
    });
}
```

---

### Q4: processEvents 的危险是什么？

**答案**：

```cpp
class ProcessEventsDangers : public QObject {
    Q_OBJECT

public:
    // 危险 1：重入
    void reentryDanger() {
        m_processing = true;
        while (m_processing) {
            doSomeWork();

            QCoreApplication::processEvents();
            // 如果 processEvents 处理了某个信号
            // 而该信号的槽又调用了这个函数
            // 就会产生无限递归
        }
    }

    // 危险 2：对象被删除
    void deletionDanger() {
        for (QWidget* widget : m_widgets) {
            widget->update();
            QCoreApplication::processEvents();
            // 如果 processEvents 处理了关闭事件
            // widget 可能被删除
            // 下次循环访问 widget 就是悬挂指针！
        }
    }

    // 危险 3：状态不一致
    void stateInconsistency() {
        m_state = State::Processing;
        doStep1();

        QCoreApplication::processEvents();
        // 某个事件处理可能改变了 m_state

        doStep2();  // 假设 m_state == Processing，可能出错
    }

    // 安全做法
    void saferApproach() {
        // 使用标志防止重入
        if (m_busy) return;
        m_busy = true;

        // 使用 QPointer 检测对象删除
        QPointer<QWidget> safeWidget = m_widget;

        QCoreApplication::processEvents();

        if (safeWidget) {
            safeWidget->update();
        }

        m_busy = false;
    }

private:
    bool m_processing = false;
    bool m_busy = false;
    QList<QWidget*> m_widgets;
    QWidget* m_widget = nullptr;
    enum class State { Idle, Processing };
    State m_state = State::Idle;
};
```

---

## 总结

Qt 事件循环的核心要点：

1. **事件循环** - 应用程序的心脏，不断处理事件
2. **exec/quit** - 启动和退出事件循环
3. **processEvents** - 手动处理待处理事件（谨慎使用）
4. **定时器** - 依赖事件循环触发
5. **线程事件循环** - 每个线程可以有自己的事件循环
6. **跨线程通信** - 使用 QueuedConnection 或 postEvent
7. **避免阻塞** - 使用异步模式保持响应性

理解事件循环是编写响应式、高效 Qt 应用程序的关键。
