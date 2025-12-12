# Qt 调试与性能分析

## 概述

Qt 提供了丰富的调试和性能分析工具，包括内置的调试输出、Qt Creator 集成调试器、QML Profiler、内存分析等。本文档涵盖 Qt 调试与性能分析的面试常见问题。

---

## 1. 调试输出

### 1.1 qDebug 系列函数

```cpp
#include <QDebug>

void debugExamples() {
    // 基本调试输出
    qDebug() << "Debug message";
    qInfo() << "Info message";
    qWarning() << "Warning message";
    qCritical() << "Critical message";
    // qFatal() << "Fatal message";  // 会终止程序

    // 输出变量
    int value = 42;
    QString name = "Test";
    qDebug() << "Value:" << value << "Name:" << name;

    // 格式化输出（类似 printf）
    qDebug("Value: %d, Name: %s", value, qPrintable(name));

    // 输出 Qt 类型
    QRect rect(0, 0, 100, 200);
    qDebug() << "Rect:" << rect;  // QRect(0,0 100x200)

    QPoint point(10, 20);
    qDebug() << "Point:" << point;  // QPoint(10,20)

    // 输出容器
    QList<int> list = {1, 2, 3};
    qDebug() << "List:" << list;  // QList(1, 2, 3)
}
```

### 1.2 自定义类型调试输出

```cpp
// 为自定义类型添加调试输出支持
class Person {
public:
    QString name;
    int age;
};

// 方法1：重载 operator<<
QDebug operator<<(QDebug debug, const Person& p) {
    QDebugStateSaver saver(debug);  // 保存格式状态
    debug.nospace() << "Person(" << p.name << ", " << p.age << ")";
    return debug;
}

// 方法2：使用 Q_DECLARE_METATYPE 和 qRegisterMetaType
Q_DECLARE_METATYPE(Person)

// 使用
Person person{"Alice", 30};
qDebug() << person;  // Person(Alice, 30)
```

### 1.3 分类日志

```cpp
// 定义日志分类
Q_LOGGING_CATEGORY(lcNetwork, "app.network")
Q_LOGGING_CATEGORY(lcDatabase, "app.database")
Q_LOGGING_CATEGORY(lcUI, "app.ui")

void categorizedLogging() {
    // 使用分类日志
    qCDebug(lcNetwork) << "Connecting to server...";
    qCInfo(lcDatabase) << "Query executed";
    qCWarning(lcUI) << "Widget not found";

    // 条件日志（避免不必要的字符串构造）
    if (lcNetwork().isDebugEnabled()) {
        qCDebug(lcNetwork) << expensiveOperation();
    }
}

// 配置日志规则（通过环境变量或代码）
// QT_LOGGING_RULES="app.network.debug=true;app.database.warning=false"

void configureLogging() {
    QLoggingCategory::setFilterRules(
        "app.network.debug=true\n"
        "app.database.*=false\n"
        "*.warning=true"
    );
}
```

### 1.4 自定义消息处理器

```cpp
// 自定义消息处理器
void customMessageHandler(QtMsgType type, const QMessageLogContext& context,
                          const QString& msg) {
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1 (%2:%3)")
            .arg(msg).arg(context.file).arg(context.line);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    // 输出到文件
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ")
       << txt << Qt::endl;
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(customMessageHandler);
    QApplication app(argc, argv);
    // ...
}
```

---

## 2. 断言和检查

### 2.1 Qt 断言宏

```cpp
void assertExamples() {
    int* ptr = getPointer();

    // Q_ASSERT：调试版本检查
    Q_ASSERT(ptr != nullptr);  // 失败时中止程序（调试版本）

    // Q_ASSERT_X：带消息的断言
    Q_ASSERT_X(ptr != nullptr, "assertExamples", "Pointer must not be null");

    // Q_CHECK_PTR：指针检查
    ptr = new int[1000000];
    Q_CHECK_PTR(ptr);  // 如果分配失败则终止

    // Q_ASSUME：编译器优化提示
    Q_ASSUME(ptr != nullptr);  // 告诉编译器可以假设条件为真
}

// 条件编译
#ifdef QT_NO_DEBUG
    // 发布版本代码
#else
    // 调试版本代码
#endif
```

### 2.2 静态断言

```cpp
// 编译时断言
template <typename T>
class TypedContainer {
    // 确保 T 是 QObject 子类
    static_assert(std::is_base_of_v<QObject, T>,
                  "T must be derived from QObject");

    // 确保 T 是可拷贝的
    static_assert(std::is_copy_constructible_v<T>,
                  "T must be copy constructible");
};

// Qt 类型特征
static_assert(QTypeInfo<QString>::isRelocatable);
static_assert(QTypeInfo<int>::isIntegral);
```

---

## 3. 内存调试

### 3.1 对象追踪

```cpp
// 追踪 QObject 创建和删除
class ObjectTracker : public QObject {
    Q_OBJECT
public:
    ObjectTracker() {
        qDebug() << "Created:" << this;
    }
    ~ObjectTracker() {
        qDebug() << "Destroyed:" << this;
    }
};

// 使用 QObject::dumpObjectTree()
void debugObjectTree(QObject* root) {
    qDebug() << "Object tree from:" << root->objectName();
    root->dumpObjectTree();  // 打印对象树结构
}

// 使用 QObject::dumpObjectInfo()
void debugObjectInfo(QObject* obj) {
    obj->dumpObjectInfo();  // 打印信号槽连接信息
}
```

### 3.2 内存泄漏检测

```cpp
// Windows 下的 CRT 内存检测
#ifdef Q_OS_WIN
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int main(int argc, char *argv[]) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    QApplication app(argc, argv);
    // ...
    return app.exec();
}
#endif

// 使用 Valgrind（Linux）
// valgrind --leak-check=full ./myapp

// 使用 AddressSanitizer
// qmake: QMAKE_CXXFLAGS += -fsanitize=address
// cmake: target_compile_options(myapp PRIVATE -fsanitize=address)
```

### 3.3 QPointer 检测悬垂指针

```cpp
void checkDanglingPointer() {
    QPointer<QWidget> widget = new QWidget();

    // 某处删除了 widget
    delete widget;

    // QPointer 自动变为 nullptr
    if (widget) {
        widget->show();  // 不会执行
    } else {
        qDebug() << "Widget was deleted";
    }
}

// 在槽函数中使用
class SafeHandler : public QObject {
    Q_OBJECT
    QPointer<QWidget> m_target;

public slots:
    void handleClick() {
        if (m_target) {
            m_target->update();
        }
    }
};
```

---

## 4. Qt Creator 调试功能

### 4.1 调试器增强

```cpp
// Qt Creator 自动美化显示 Qt 类型
// QString、QList、QMap 等会以可读形式显示

// 条件断点
// 在 Qt Creator 中设置断点条件
// 例如: i == 100 或 str.isEmpty()

// 日志断点（不暂停）
// 设置断点动作为打印消息而非暂停
```

### 4.2 调试辅助函数

```cpp
// 用于调试器查看的辅助函数
class DebugHelper {
public:
    // 在调试器中调用以查看私有数据
    static QString inspectWidget(QWidget* w) {
        return QString("Widget: %1, Size: %2x%3, Visible: %4")
            .arg(w->objectName())
            .arg(w->width())
            .arg(w->height())
            .arg(w->isVisible());
    }

    static void printConnections(QObject* obj) {
        obj->dumpObjectInfo();
    }
};

// 在调试器控制台中调用:
// DebugHelper::inspectWidget(widget)
```

### 4.3 GDB/LLDB Pretty Printers

```python
# GDB Pretty Printer for QString (gdbinit)
class QStringPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        d = self.val['d']
        data = d.cast(gdb.lookup_type('QStringData').pointer())
        size = int(data['size'])
        # 返回字符串内容
        return data['data()'].string('utf-16', length=size*2)

def qt_lookup_function(val):
    lookup_tag = val.type.tag
    if lookup_tag == 'QString':
        return QStringPrinter(val)
    return None

gdb.pretty_printers.append(qt_lookup_function)
```

---

## 5. QML 调试

### 5.1 QML 调试输出

```qml
// QML 中的调试
Rectangle {
    Component.onCompleted: {
        console.log("Rectangle created")
        console.debug("Debug info")
        console.warn("Warning message")
        console.error("Error message")
    }

    MouseArea {
        onClicked: {
            console.log("Position:", mouseX, mouseY)
            console.log("Object:", JSON.stringify(parent))
        }
    }
}
```

### 5.2 QML Profiler

```cpp
// 启用 QML 分析
// 启动参数: -qmljsdebugger=port:3768,block

// 或在代码中启用
#include <QQmlDebuggingEnabler>
QQmlDebuggingEnabler enabler;

// Qt Creator 中使用 QML Profiler
// - Analyze -> QML Profiler
// - 查看绑定评估、信号处理、创建时间等
```

### 5.3 QML 运行时检查

```cpp
// 在 C++ 中检查 QML 错误
QQmlEngine engine;
QQmlComponent component(&engine, QUrl("qrc:/main.qml"));

if (component.status() == QQmlComponent::Error) {
    for (const QQmlError& error : component.errors()) {
        qWarning() << "QML Error:" << error.toString();
    }
}

// 连接 QML 引擎警告信号
QObject::connect(&engine, &QQmlEngine::warnings,
                 [](const QList<QQmlError>& warnings) {
                     for (const auto& w : warnings) {
                         qWarning() << "QML Warning:" << w;
                     }
                 });
```

---

## 6. 性能分析

### 6.1 QElapsedTimer

```cpp
#include <QElapsedTimer>

void measurePerformance() {
    QElapsedTimer timer;
    timer.start();

    // 执行要测量的操作
    doExpensiveOperation();

    qint64 elapsed = timer.elapsed();  // 毫秒
    qDebug() << "Operation took" << elapsed << "ms";

    // 纳秒精度
    qint64 nanos = timer.nsecsElapsed();
    qDebug() << "Operation took" << nanos << "ns";

    // 检查是否超时
    timer.restart();
    while (!timer.hasExpired(1000)) {  // 1秒超时
        processNextItem();
    }
}

// 作用域计时器
class ScopedTimer {
    QElapsedTimer timer;
    QString name;
public:
    ScopedTimer(const QString& n) : name(n) { timer.start(); }
    ~ScopedTimer() {
        qDebug() << name << "took" << timer.elapsed() << "ms";
    }
};

void function() {
    ScopedTimer t("function");
    // ... 函数代码
}  // 自动打印耗时
```

### 6.2 Qt 性能工具

```cpp
// 事件循环监控
class EventLoopMonitor : public QObject {
    Q_OBJECT
    QElapsedTimer eventTimer;

public:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::Timer) {
            eventTimer.start();
        }
        return false;
    }

    void checkEventLatency() {
        if (eventTimer.elapsed() > 16) {  // 超过一帧
            qWarning() << "Event processing too slow:"
                       << eventTimer.elapsed() << "ms";
        }
    }
};

// 渲染性能
class RenderMonitor : public QWidget {
protected:
    void paintEvent(QPaintEvent* event) override {
        QElapsedTimer timer;
        timer.start();

        // 绑定绑定...
        QWidget::paintEvent(event);

        if (timer.elapsed() > 16) {
            qWarning() << "Slow paint:" << timer.elapsed() << "ms";
        }
    }
};
```

### 6.3 外部性能分析工具

```bash
# Linux: perf
perf record ./myapp
perf report

# Linux: Valgrind Callgrind
valgrind --tool=callgrind ./myapp
kcachegrind callgrind.out.*

# macOS: Instruments
instruments -t "Time Profiler" ./myapp

# Windows: Visual Studio Profiler
# 或 Intel VTune

# 所有平台: Qt Creator 内置分析器
# Analyze -> Performance Analyzer
```

---

## 7. 事件和信号槽调试

### 7.1 事件监控

```cpp
// 全局事件过滤器监控所有事件
class GlobalEventFilter : public QObject {
public:
    bool eventFilter(QObject* obj, QEvent* event) override {
        // 记录特定类型事件
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            qDebug() << "Mouse press on" << obj->objectName();
            break;
        case QEvent::KeyPress:
            qDebug() << "Key press on" << obj->objectName();
            break;
        case QEvent::FocusIn:
            qDebug() << "Focus in" << obj->objectName();
            break;
        default:
            break;
        }
        return false;  // 不拦截事件
    }
};

// 安装
app.installEventFilter(new GlobalEventFilter);

// 事件类型名称查找
QString eventTypeName(QEvent::Type type) {
    static const QMetaObject& mo = QEvent::staticMetaObject;
    int index = mo.indexOfEnumerator("Type");
    return mo.enumerator(index).valueToKey(type);
}
```

### 7.2 信号槽调试

```cpp
// 调试信号发射
class SignalSpy : public QObject {
    Q_OBJECT
public:
    void connectToAll(QObject* obj) {
        const QMetaObject* meta = obj->metaObject();
        for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
            QMetaMethod method = meta->method(i);
            if (method.methodType() == QMetaMethod::Signal) {
                // 连接到调试槽
                connect(obj, method, this,
                        metaObject()->method(metaObject()->indexOfSlot("logSignal()")));
            }
        }
    }

public slots:
    void logSignal() {
        qDebug() << "Signal emitted from" << sender()
                 << "signal index:" << senderSignalIndex();
    }
};

// 使用 QSignalSpy（测试用）
#include <QSignalSpy>

void testSignals() {
    MyClass obj;
    QSignalSpy spy(&obj, &MyClass::valueChanged);

    obj.setValue(42);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toInt(), 42);
}
```

### 7.3 连接诊断

```cpp
// 检查连接是否成功
bool connected = connect(sender, &Sender::signal, receiver, &Receiver::slot);
if (!connected) {
    qWarning() << "Failed to connect signal to slot";
}

// 使用 Qt::UniqueConnection 避免重复连接
connect(sender, &Sender::signal, receiver, &Receiver::slot,
        Qt::UniqueConnection);

// 列出对象的所有连接
void dumpConnections(QObject* obj) {
    obj->dumpObjectInfo();
    // 输出信号槽连接信息
}
```

---

## 8. 常见调试场景

### 8.1 崩溃调试

```cpp
// 安装崩溃处理器
#include <csignal>

void crashHandler(int sig) {
    qCritical() << "Crash! Signal:" << sig;

    // 打印堆栈跟踪（平台相关）
#ifdef Q_OS_LINUX
    void* array[50];
    int size = backtrace(array, 50);
    char** symbols = backtrace_symbols(array, size);
    for (int i = 0; i < size; ++i) {
        qCritical() << symbols[i];
    }
    free(symbols);
#endif

    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, crashHandler);
    signal(SIGABRT, crashHandler);

    QApplication app(argc, argv);
    // ...
}
```

### 8.2 死锁调试

```cpp
// 检测潜在死锁
class DeadlockDetector {
    static QMutex s_mutex;
    static QMap<QThread*, QMutex*> s_heldLocks;

public:
    static void beforeLock(QMutex* mutex) {
        QMutexLocker locker(&s_mutex);
        QThread* current = QThread::currentThread();

        // 检查是否会导致死锁
        for (auto it = s_heldLocks.begin(); it != s_heldLocks.end(); ++it) {
            if (it.value() == mutex && it.key() != current) {
                qWarning() << "Potential deadlock detected!"
                           << "Thread" << current << "waiting for mutex held by"
                           << it.key();
            }
        }
    }

    static void afterLock(QMutex* mutex) {
        QMutexLocker locker(&s_mutex);
        s_heldLocks[QThread::currentThread()] = mutex;
    }

    static void afterUnlock(QMutex* mutex) {
        QMutexLocker locker(&s_mutex);
        s_heldLocks.remove(QThread::currentThread());
    }
};
```

### 8.3 UI 冻结调试

```cpp
// 检测主线程阻塞
class MainThreadWatchdog : public QObject {
    Q_OBJECT
    QTimer* m_timer;
    QElapsedTimer m_elapsed;

public:
    MainThreadWatchdog(QObject* parent = nullptr) : QObject(parent) {
        m_timer = new QTimer(this);
        m_timer->setInterval(100);  // 100ms 检查间隔
        connect(m_timer, &QTimer::timeout, this, &MainThreadWatchdog::check);
    }

    void start() {
        m_elapsed.start();
        m_timer->start();
    }

private slots:
    void check() {
        qint64 elapsed = m_elapsed.restart();
        if (elapsed > 200) {  // 超过 200ms
            qWarning() << "Main thread blocked for" << elapsed << "ms!";
            // 可以在这里打印堆栈
        }
    }
};
```

---

## 面试常见问题

### Q1: qDebug、qInfo、qWarning、qCritical 的区别？

**答案**：
| 级别 | 用途 | 默认行为 |
|------|------|---------|
| qDebug | 调试信息 | 发布版本可禁用 |
| qInfo | 一般信息 | 始终输出 |
| qWarning | 警告 | 始终输出 |
| qCritical | 严重错误 | 始终输出 |
| qFatal | 致命错误 | 输出后终止程序 |

可通过 QT_NO_DEBUG_OUTPUT 等宏或日志规则控制输出。

### Q2: 如何为自定义类型添加调试输出支持？

**答案**：
重载 `operator<<(QDebug, const MyType&)`：
```cpp
QDebug operator<<(QDebug dbg, const MyType& obj) {
    QDebugStateSaver saver(dbg);  // 保存并恢复格式状态
    dbg.nospace() << "MyType(" << obj.value << ")";
    return dbg;
}
```

### Q3: 如何检测 Qt 应用中的内存泄漏？

**答案**：
1. **QPointer**：检测悬垂指针
2. **Valgrind**（Linux）：`valgrind --leak-check=full ./app`
3. **AddressSanitizer**：编译时添加 `-fsanitize=address`
4. **CRT Debug**（Windows）：`_CrtSetDbgFlag`
5. **Qt Creator Memory Analyzer**：集成分析工具
6. **dumpObjectTree()**：检查对象树是否正确

### Q4: QML Profiler 能分析什么？

**答案**：
1. **创建时间**：组件实例化耗时
2. **绑定评估**：属性绑定的执行时间和频率
3. **信号处理**：JavaScript 信号处理器执行时间
4. **渲染**：场景图渲染性能
5. **内存**：QML 对象内存使用
6. **输入处理**：事件处理延迟

### Q5: 如何调试信号槽连接问题？

**答案**：
1. 检查 connect 返回值
2. 使用 `dumpObjectInfo()` 查看连接
3. 使用 `QSignalSpy` 监控信号
4. 确保信号槽签名匹配
5. 检查对象生命周期（使用 QPointer）
6. 验证线程关联（跨线程需要队列连接）

### Q6: 如何测量代码性能？

**答案**：
1. **QElapsedTimer**：精确计时
   ```cpp
   QElapsedTimer timer;
   timer.start();
   // 代码
   qDebug() << timer.elapsed() << "ms";
   ```
2. **Qt Creator Profiler**：集成分析
3. **外部工具**：perf、Valgrind、Instruments
4. **作用域计时器**：RAII 模式自动测量

### Q7: 如何调试主线程卡顿？

**答案**：
1. 使用看门狗定时器检测阻塞
2. 检查是否有长时间操作在主线程
3. 查找是否有阻塞的同步调用
4. 使用 Profiler 找出耗时函数
5. 将耗时操作移到工作线程
6. 使用 processEvents() 分割长任务（不推荐）

---

## 总结

Qt 调试与性能分析关键点：

| 工具/技术 | 用途 |
|----------|------|
| qDebug 系列 | 日志输出 |
| Q_ASSERT | 调试断言 |
| QLoggingCategory | 分类日志 |
| dumpObjectTree/Info | 对象调试 |
| QPointer | 悬垂指针检测 |
| QElapsedTimer | 性能计时 |
| QSignalSpy | 信号测试 |
| QML Profiler | QML 性能分析 |
| Valgrind/ASan | 内存调试 |

调试最佳实践：
1. 使用分类日志便于过滤
2. 为自定义类型实现调试输出
3. 使用 QPointer 防止悬垂指针
4. 在开发时启用断言
5. 定期进行性能分析
6. 使用工具检测内存泄漏
7. 监控主线程响应性
