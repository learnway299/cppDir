# Qt 事件系统（Event System）

## 概述

Qt 事件系统是 Qt 应用程序处理用户输入和系统通知的核心机制。与信号槽不同，事件系统采用分发模式，事件对象在对象层次结构中传递，可以被拦截、过滤和处理。

---

## 1. 事件基础

### 1.1 什么是 Qt 事件？

**面试问题**：Qt 中的事件是什么？与信号槽有什么区别？

**答案**：

事件是描述应用程序内部或外部发生的某件事情的对象：

```cpp
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>

// 事件是 QEvent 的派生类
void eventBasicsDemo() {
    // 鼠标事件
    QMouseEvent mouseEvent(
        QEvent::MouseButtonPress,      // 事件类型
        QPointF(100, 200),             // 位置
        Qt::LeftButton,                // 按钮
        Qt::LeftButton,                // 当前按下的按钮
        Qt::NoModifier                 // 修饰键
    );

    qDebug() << "Event type:" << mouseEvent.type();
    qDebug() << "Position:" << mouseEvent.pos();
    qDebug() << "Button:" << mouseEvent.button();

    // 键盘事件
    QKeyEvent keyEvent(
        QEvent::KeyPress,
        Qt::Key_A,
        Qt::ControlModifier,
        "a"
    );

    qDebug() << "Key:" << keyEvent.key();
    qDebug() << "Modifiers:" << keyEvent.modifiers();
}
```

**事件 vs 信号槽**：

| 特性 | 事件 | 信号槽 |
|------|------|--------|
| 方向 | 系统→对象（推送） | 对象→对象（发布订阅） |
| 处理方式 | 重写虚函数 | 连接槽函数 |
| 可拦截 | ✅ 可以拦截和过滤 | ❌ 无法拦截 |
| 传播 | 沿对象树传播 | 直接调用 |
| 典型用途 | 输入处理、绘制 | 业务逻辑通信 |
| 异步性 | 可同步或异步 | 取决于连接类型 |

---

### 1.2 常见事件类型

**面试问题**：列举 Qt 中常见的事件类型。

**答案**：

```cpp
// QEvent::Type 枚举定义了所有事件类型
void eventTypesOverview() {
    // 输入事件
    // QEvent::MouseButtonPress    - 鼠标按下
    // QEvent::MouseButtonRelease  - 鼠标释放
    // QEvent::MouseButtonDblClick - 鼠标双击
    // QEvent::MouseMove           - 鼠标移动
    // QEvent::KeyPress            - 键盘按下
    // QEvent::KeyRelease          - 键盘释放
    // QEvent::Wheel               - 鼠标滚轮

    // 窗口事件
    // QEvent::Resize              - 窗口大小改变
    // QEvent::Move                - 窗口移动
    // QEvent::Show                - 窗口显示
    // QEvent::Hide                - 窗口隐藏
    // QEvent::Close               - 窗口关闭
    // QEvent::WindowActivate      - 窗口激活
    // QEvent::WindowDeactivate    - 窗口失活

    // 焦点事件
    // QEvent::FocusIn             - 获得焦点
    // QEvent::FocusOut            - 失去焦点
    // QEvent::FocusAboutToChange  - 焦点即将改变

    // 绘制事件
    // QEvent::Paint               - 需要重绘
    // QEvent::UpdateRequest       - 更新请求

    // 定时器事件
    // QEvent::Timer               - 定时器触发

    // 拖放事件
    // QEvent::DragEnter           - 拖拽进入
    // QEvent::DragMove            - 拖拽移动
    // QEvent::DragLeave           - 拖拽离开
    // QEvent::Drop                - 放下

    // 其他
    // QEvent::Enter               - 鼠标进入控件
    // QEvent::Leave               - 鼠标离开控件
    // QEvent::ContextMenu         - 上下文菜单
    // QEvent::ToolTip             - 工具提示
}

// 事件类型检查示例
bool checkEventType(QEvent* event) {
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        qDebug() << "Mouse press event";
        return true;
    case QEvent::KeyPress:
        qDebug() << "Key press event";
        return true;
    case QEvent::Paint:
        qDebug() << "Paint event";
        return true;
    default:
        qDebug() << "Other event:" << event->type();
        return false;
    }
}
```

---

## 2. 事件处理

### 2.1 重写事件处理函数

**面试问题**：如何在 Qt 中处理事件？

**答案**：

最常见的方式是重写特定的事件处理虚函数：

```cpp
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>

class CustomWidget : public QWidget {
    Q_OBJECT

public:
    explicit CustomWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMouseTracking(true);  // 启用鼠标追踪
        setFocusPolicy(Qt::StrongFocus);  // 允许键盘焦点
    }

protected:
    // 鼠标按下事件
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            m_lastPos = event->pos();
            m_pressed = true;
            qDebug() << "Left button pressed at" << event->pos();
        }
        // 调用基类处理（可选）
        QWidget::mousePressEvent(event);
    }

    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent* event) override {
        if (m_pressed) {
            QPoint delta = event->pos() - m_lastPos;
            qDebug() << "Dragging, delta:" << delta;
            m_lastPos = event->pos();
        }
        QWidget::mouseMoveEvent(event);
    }

    // 鼠标释放事件
    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            m_pressed = false;
            qDebug() << "Left button released";
        }
        QWidget::mouseReleaseEvent(event);
    }

    // 键盘按下事件
    void keyPressEvent(QKeyEvent* event) override {
        switch (event->key()) {
        case Qt::Key_Escape:
            close();
            break;
        case Qt::Key_Space:
            qDebug() << "Space pressed";
            break;
        default:
            // 未处理的按键传递给基类
            QWidget::keyPressEvent(event);
        }
    }

    // 绘制事件
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.fillRect(event->rect(), Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Custom Widget");
    }

    // 大小改变事件
    void resizeEvent(QResizeEvent* event) override {
        qDebug() << "Resized from" << event->oldSize()
                 << "to" << event->size();
        QWidget::resizeEvent(event);
    }

    // 进入/离开事件
    void enterEvent(QEnterEvent* event) override {
        qDebug() << "Mouse entered";
        setCursor(Qt::CrossCursor);
        QWidget::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        qDebug() << "Mouse left";
        setCursor(Qt::ArrowCursor);
        QWidget::leaveEvent(event);
    }

private:
    QPoint m_lastPos;
    bool m_pressed = false;
};
```

---

### 2.2 重写 event() 函数

**面试问题**：event() 函数的作用是什么？什么时候需要重写它？

**答案**：

`event()` 是所有事件的入口点，在分发到具体处理函数之前被调用：

```cpp
class EventDispatchWidget : public QWidget {
    Q_OBJECT

protected:
    // event() 是事件分发的中心
    bool event(QEvent* event) override {
        // 在这里可以：
        // 1. 处理没有专门处理函数的事件
        // 2. 在分发前进行统一处理
        // 3. 完全接管事件处理

        switch (event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            // Tab 键处理（焦点切换前拦截）
            if (keyEvent->key() == Qt::Key_Tab) {
                qDebug() << "Tab intercepted in event()";
                // 返回 true 表示事件已处理，不再传播
                return true;
            }
            break;
        }

        case QEvent::ToolTip:
            // 处理工具提示（没有专门的虚函数）
            handleToolTip(static_cast<QHelpEvent*>(event));
            return true;

        case QEvent::Gesture:
            // 处理手势事件
            handleGesture(static_cast<QGestureEvent*>(event));
            return true;

        default:
            break;
        }

        // 调用基类 event() 进行默认分发
        return QWidget::event(event);
    }

private:
    void handleToolTip(QHelpEvent* event) {
        QToolTip::showText(event->globalPos(), "This is a tooltip");
    }

    void handleGesture(QGestureEvent* event) {
        if (QGesture* pinch = event->gesture(Qt::PinchGesture)) {
            qDebug() << "Pinch gesture detected";
        }
    }
};
```

**event() 的调用流程**：

```
QApplication::notify()
        ↓
QObject::event()  ← 可以在这里拦截
        ↓
mousePressEvent() / keyPressEvent() / etc.
```

---

### 2.3 事件接受与忽略

**面试问题**：event->accept() 和 event->ignore() 有什么区别？

**答案**：

```cpp
class EventAcceptDemo : public QWidget {
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Escape) {
            // 处理了这个事件
            qDebug() << "Escape handled";
            event->accept();  // 明确接受（其实默认就是接受）
            return;
        }

        // 不处理这个按键，让父控件处理
        event->ignore();
        // 或者调用基类（基类通常会 ignore）
        // QWidget::keyPressEvent(event);
    }

    void closeEvent(QCloseEvent* event) override {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Confirm", "Are you sure you want to close?");

        if (reply == QMessageBox::Yes) {
            event->accept();  // 允许关闭
        } else {
            event->ignore();  // 阻止关闭
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            // 处理左键点击
            handleLeftClick(event->pos());
            event->accept();
        } else {
            // 右键等其他按钮不处理，传递给父控件
            event->ignore();
        }
    }

private:
    void handleLeftClick(const QPoint& pos) {
        qDebug() << "Left click at" << pos;
    }
};
```

**accept/ignore 的影响**：

| 事件状态 | 效果 |
|----------|------|
| accept() | 事件处理完成，停止传播 |
| ignore() | 事件未处理，传递给父控件 |

```cpp
// 事件传播示例
class Parent : public QWidget {
protected:
    void mousePressEvent(QMouseEvent* event) override {
        qDebug() << "Parent received mouse press";
    }
};

class Child : public QWidget {
protected:
    void mousePressEvent(QMouseEvent* event) override {
        qDebug() << "Child received mouse press";
        event->ignore();  // 让父控件也收到这个事件
    }
};

// 使用：
// Parent parent;
// Child* child = new Child(&parent);
// 点击 child 时输出：
// "Child received mouse press"
// "Parent received mouse press"  ← 因为 child ignore 了事件
```

---

## 3. 事件过滤器

### 3.1 安装事件过滤器

**面试问题**：什么是事件过滤器？如何使用它？

**答案**：

事件过滤器允许一个对象监视并拦截发送给其他对象的事件：

```cpp
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>

class KeyFilter : public QObject {
    Q_OBJECT

public:
    explicit KeyFilter(QObject* parent = nullptr)
        : QObject(parent) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            // 拦截 Enter 键
            if (keyEvent->key() == Qt::Key_Return ||
                keyEvent->key() == Qt::Key_Enter) {
                qDebug() << "Enter pressed in" << watched->objectName();
                emit enterPressed(watched);
                return true;  // 事件已处理，不再传递
            }

            // 拦截 Tab 键（焦点切换前）
            if (keyEvent->key() == Qt::Key_Tab) {
                qDebug() << "Tab pressed, custom handling";
                emit tabPressed(watched);
                return true;
            }
        }

        // 其他事件正常传递
        return QObject::eventFilter(watched, event);
    }

signals:
    void enterPressed(QObject* source);
    void tabPressed(QObject* source);
};

// 使用事件过滤器
class FormWidget : public QWidget {
    Q_OBJECT

public:
    FormWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // 创建输入框
        m_nameEdit = new QLineEdit(this);
        m_nameEdit->setObjectName("nameEdit");

        m_emailEdit = new QLineEdit(this);
        m_emailEdit->setObjectName("emailEdit");

        // 创建并安装事件过滤器
        m_keyFilter = new KeyFilter(this);
        m_nameEdit->installEventFilter(m_keyFilter);
        m_emailEdit->installEventFilter(m_keyFilter);

        // 连接信号
        connect(m_keyFilter, &KeyFilter::enterPressed,
                this, &FormWidget::onEnterPressed);
    }

private slots:
    void onEnterPressed(QObject* source) {
        // Enter 键跳转到下一个输入框
        if (source == m_nameEdit) {
            m_emailEdit->setFocus();
        } else if (source == m_emailEdit) {
            submit();
        }
    }

    void submit() {
        qDebug() << "Form submitted";
    }

private:
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    KeyFilter* m_keyFilter;
};
```

---

### 3.2 应用程序级事件过滤

**面试问题**：如何实现全局的事件监控？

**答案**：

```cpp
#include <QApplication>

class GlobalEventFilter : public QObject {
    Q_OBJECT

public:
    explicit GlobalEventFilter(QObject* parent = nullptr)
        : QObject(parent) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        // 统计所有鼠标点击
        if (event->type() == QEvent::MouseButtonPress) {
            m_clickCount++;
            qDebug() << "Global click count:" << m_clickCount;
        }

        // 监控所有键盘输入（用于快捷键）
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            // 全局快捷键：Ctrl+Shift+D
            if (keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) &&
                keyEvent->key() == Qt::Key_D) {
                qDebug() << "Debug shortcut triggered";
                emit debugRequested();
                return true;  // 拦截这个快捷键
            }
        }

        // 监控焦点变化
        if (event->type() == QEvent::FocusIn) {
            if (QWidget* widget = qobject_cast<QWidget*>(watched)) {
                qDebug() << "Focus moved to:" << widget->objectName();
            }
        }

        return QObject::eventFilter(watched, event);
    }

signals:
    void debugRequested();

private:
    int m_clickCount = 0;
};

// 在 main() 中安装
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 安装全局事件过滤器
    GlobalEventFilter* globalFilter = new GlobalEventFilter(&app);
    app.installEventFilter(globalFilter);

    // ... 创建窗口等
    return app.exec();
}
```

---

### 3.3 事件过滤器 vs 重写事件处理函数

**面试问题**：什么时候用事件过滤器，什么时候重写事件处理函数？

**答案**：

```cpp
// 场景 1：修改自己的行为 → 重写事件处理函数
class MyButton : public QPushButton {
protected:
    void mousePressEvent(QMouseEvent* event) override {
        // 自定义按钮的点击行为
        if (event->button() == Qt::RightButton) {
            showContextMenu(event->pos());
            return;
        }
        QPushButton::mousePressEvent(event);
    }
};

// 场景 2：监控/修改其他对象的行为 → 事件过滤器
class ClickLogger : public QObject {
    Q_OBJECT
protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            qDebug() << watched->objectName() << "was clicked";
        }
        return false;  // 不拦截，只监控
    }
};

// 场景 3：批量处理多个控件 → 事件过滤器
class FormValidator : public QObject {
    Q_OBJECT
public:
    void watchLineEdits(const QList<QLineEdit*>& edits) {
        for (QLineEdit* edit : edits) {
            edit->installEventFilter(this);
        }
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::FocusOut) {
            if (QLineEdit* edit = qobject_cast<QLineEdit*>(watched)) {
                validateEdit(edit);
            }
        }
        return false;
    }

private:
    void validateEdit(QLineEdit* edit) {
        // 验证输入
    }
};
```

**对比总结**：

| 方面 | 事件过滤器 | 重写事件函数 |
|------|------------|--------------|
| 目标 | 其他对象 | 自身 |
| 灵活性 | 高（运行时安装/移除） | 低（编译时确定） |
| 代码位置 | 独立类 | 目标类内部 |
| 性能 | 略低（额外调用） | 较高 |
| 适用场景 | 监控、批量处理 | 自定义行为 |

---

## 4. 自定义事件

### 4.1 创建自定义事件

**面试问题**：如何创建和使用自定义事件？

**答案**：

```cpp
#include <QEvent>
#include <QCoreApplication>

// 方式 1：继承 QEvent
class DataReadyEvent : public QEvent {
public:
    // 注册自定义事件类型
    static const QEvent::Type EventType;

    explicit DataReadyEvent(const QByteArray& data)
        : QEvent(EventType)
        , m_data(data)
    {}

    QByteArray data() const { return m_data; }

private:
    QByteArray m_data;
};

// 静态成员初始化（在 .cpp 文件中）
const QEvent::Type DataReadyEvent::EventType =
    static_cast<QEvent::Type>(QEvent::registerEventType());

// 方式 2：使用 QEvent::Type 枚举值
class ProgressEvent : public QEvent {
public:
    static const QEvent::Type EventType;

    ProgressEvent(int current, int total)
        : QEvent(EventType)
        , m_current(current)
        , m_total(total)
    {}

    int current() const { return m_current; }
    int total() const { return m_total; }
    double percentage() const {
        return m_total > 0 ? 100.0 * m_current / m_total : 0;
    }

private:
    int m_current;
    int m_total;
};

const QEvent::Type ProgressEvent::EventType =
    static_cast<QEvent::Type>(QEvent::registerEventType());
```

---

### 4.2 发送自定义事件

**面试问题**：sendEvent 和 postEvent 有什么区别？

**答案**：

```cpp
class DataProcessor : public QObject {
    Q_OBJECT

public:
    void processData(const QByteArray& data) {
        // 方式 1：sendEvent - 同步发送
        // 事件立即被处理，函数返回后事件对象仍有效
        DataReadyEvent event1(data);
        QCoreApplication::sendEvent(m_receiver, &event1);
        // event1 可以继续使用

        // 方式 2：postEvent - 异步发送
        // 事件被放入队列，稍后处理
        // 事件对象必须用 new 创建，Qt 会负责删除
        DataReadyEvent* event2 = new DataReadyEvent(data);
        QCoreApplication::postEvent(m_receiver, event2);
        // 不能再使用 event2，所有权已转移

        // 方式 3：带优先级的 postEvent
        ProgressEvent* progressEvent = new ProgressEvent(50, 100);
        QCoreApplication::postEvent(m_receiver, progressEvent,
                                    Qt::HighEventPriority);
    }

private:
    QObject* m_receiver = nullptr;
};

// 接收自定义事件
class DataReceiver : public QObject {
    Q_OBJECT

protected:
    bool event(QEvent* event) override {
        if (event->type() == DataReadyEvent::EventType) {
            DataReadyEvent* dataEvent = static_cast<DataReadyEvent*>(event);
            handleDataReady(dataEvent->data());
            return true;
        }

        if (event->type() == ProgressEvent::EventType) {
            ProgressEvent* progressEvent = static_cast<ProgressEvent*>(event);
            handleProgress(progressEvent->current(), progressEvent->total());
            return true;
        }

        return QObject::event(event);
    }

private:
    void handleDataReady(const QByteArray& data) {
        qDebug() << "Data received:" << data.size() << "bytes";
    }

    void handleProgress(int current, int total) {
        qDebug() << "Progress:" << current << "/" << total;
    }
};
```

**sendEvent vs postEvent**：

| 特性 | sendEvent | postEvent |
|------|-----------|-----------|
| 执行时机 | 立即同步执行 | 异步，事件循环处理 |
| 事件对象 | 栈上或堆上 | 必须堆上（new） |
| 内存管理 | 调用者负责 | Qt 自动删除 |
| 线程安全 | 不安全 | 安全 |
| 返回值 | 可获取处理结果 | 无法获取 |
| 使用场景 | 需要同步结果 | 跨线程、延迟处理 |

---

### 4.3 跨线程事件通信

**面试问题**：如何使用事件在线程间通信？

**答案**：

```cpp
#include <QThread>
#include <QCoreApplication>

// 工作线程发送事件到主线程
class WorkerThread : public QThread {
    Q_OBJECT

public:
    explicit WorkerThread(QObject* receiver, QObject* parent = nullptr)
        : QThread(parent)
        , m_receiver(receiver)
    {}

protected:
    void run() override {
        for (int i = 0; i <= 100; i += 10) {
            // 模拟耗时操作
            QThread::msleep(100);

            // 发送进度事件到主线程（线程安全）
            ProgressEvent* event = new ProgressEvent(i, 100);
            QCoreApplication::postEvent(m_receiver, event);
        }

        // 发送完成事件
        DataReadyEvent* doneEvent = new DataReadyEvent("Work completed");
        QCoreApplication::postEvent(m_receiver, doneEvent);
    }

private:
    QObject* m_receiver;
};

// 主线程接收器
class MainReceiver : public QObject {
    Q_OBJECT

public:
    void startWork() {
        WorkerThread* worker = new WorkerThread(this, this);
        connect(worker, &QThread::finished, worker, &QObject::deleteLater);
        worker->start();
    }

protected:
    bool event(QEvent* event) override {
        if (event->type() == ProgressEvent::EventType) {
            auto* e = static_cast<ProgressEvent*>(event);
            qDebug() << "Progress:" << e->percentage() << "%";
            emit progressChanged(e->percentage());
            return true;
        }

        if (event->type() == DataReadyEvent::EventType) {
            auto* e = static_cast<DataReadyEvent*>(event);
            qDebug() << "Completed:" << e->data();
            emit workCompleted();
            return true;
        }

        return QObject::event(event);
    }

signals:
    void progressChanged(double percentage);
    void workCompleted();
};
```

---

## 5. 特殊事件处理

### 5.1 定时器事件

**面试问题**：Qt 中如何使用定时器事件？

**答案**：

```cpp
#include <QTimerEvent>

class TimerWidget : public QWidget {
    Q_OBJECT

public:
    TimerWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // 启动定时器，返回定时器 ID
        m_updateTimerId = startTimer(16);      // ~60 FPS
        m_saveTimerId = startTimer(30000);     // 30 秒
        m_animTimerId = startTimer(100);       // 100ms
    }

    ~TimerWidget() {
        // 停止定时器（可选，对象销毁时自动停止）
        killTimer(m_updateTimerId);
        killTimer(m_saveTimerId);
        killTimer(m_animTimerId);
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_updateTimerId) {
            // 高频更新
            updateDisplay();
        } else if (event->timerId() == m_saveTimerId) {
            // 定期保存
            autoSave();
        } else if (event->timerId() == m_animTimerId) {
            // 动画帧
            updateAnimation();
        }
    }

private:
    void updateDisplay() { /* 更新显示 */ }
    void autoSave() { /* 自动保存 */ }
    void updateAnimation() {
        m_animFrame++;
        update();  // 触发重绘
    }

    int m_updateTimerId = 0;
    int m_saveTimerId = 0;
    int m_animTimerId = 0;
    int m_animFrame = 0;
};

// 对比：QTimer vs timerEvent
class TimerComparison : public QObject {
    Q_OBJECT

public:
    TimerComparison() {
        // 方式 1：QTimer（推荐，更灵活）
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &TimerComparison::onTimeout);
        m_timer->start(1000);

        // 方式 2：startTimer + timerEvent（更轻量）
        m_timerId = startTimer(1000);
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_timerId) {
            qDebug() << "timerEvent triggered";
        }
    }

private slots:
    void onTimeout() {
        qDebug() << "QTimer timeout";
    }

private:
    QTimer* m_timer;
    int m_timerId;
};
```

---

### 5.2 绘制事件

**面试问题**：paintEvent 的工作原理是什么？

**答案**：

```cpp
#include <QPaintEvent>
#include <QPainter>

class PaintWidget : public QWidget {
    Q_OBJECT

public:
    PaintWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // 设置属性优化绘制
        setAttribute(Qt::WA_OpaquePaintEvent);  // 不透明，不需要擦除背景
        // setAttribute(Qt::WA_NoSystemBackground);  // 无系统背景
    }

    void setColor(const QColor& color) {
        m_color = color;

        // 触发重绘的几种方式
        update();           // 推荐：异步，会合并多次调用
        // repaint();       // 同步：立即重绘，慎用
        // update(rect);    // 只更新指定区域
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);

        // 1. 获取需要重绘的区域
        QRect dirtyRect = event->rect();
        QRegion dirtyRegion = event->region();

        // 2. 设置裁剪（优化）
        painter.setClipRegion(dirtyRegion);

        // 3. 绘制背景
        painter.fillRect(dirtyRect, m_color);

        // 4. 绘制内容
        painter.setPen(Qt::black);
        painter.drawText(rect(), Qt::AlignCenter, "Hello Paint!");

        // 5. 绘制边框
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }

private:
    QColor m_color = Qt::white;
};

// 双缓冲绘制
class DoubleBufferedWidget : public QWidget {
    Q_OBJECT

public:
    DoubleBufferedWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // Qt 默认已启用双缓冲
        // 可以使用 QPixmap 进行额外的缓冲
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        // 检查缓冲是否需要更新
        if (m_buffer.isNull() || m_buffer.size() != size()) {
            m_buffer = QPixmap(size());
            m_bufferDirty = true;
        }

        if (m_bufferDirty) {
            // 绘制到缓冲
            QPainter bufferPainter(&m_buffer);
            drawContent(&bufferPainter);
            m_bufferDirty = false;
        }

        // 将缓冲绘制到屏幕
        QPainter painter(this);
        painter.drawPixmap(event->rect(), m_buffer, event->rect());
    }

private:
    void drawContent(QPainter* painter) {
        // 复杂的绘制操作
        painter->fillRect(rect(), Qt::white);
        // ... 更多绘制
    }

    QPixmap m_buffer;
    bool m_bufferDirty = true;
};
```

---

### 5.3 关闭事件

**面试问题**：如何正确处理窗口关闭事件？

**答案**：

```cpp
#include <QCloseEvent>
#include <QMessageBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {}

protected:
    void closeEvent(QCloseEvent* event) override {
        // 检查是否有未保存的更改
        if (hasUnsavedChanges()) {
            QMessageBox::StandardButton reply = QMessageBox::warning(
                this,
                "Unsaved Changes",
                "You have unsaved changes. Do you want to save before closing?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
            );

            switch (reply) {
            case QMessageBox::Save:
                if (save()) {
                    event->accept();  // 保存成功，允许关闭
                } else {
                    event->ignore();  // 保存失败，取消关闭
                }
                break;

            case QMessageBox::Discard:
                event->accept();  // 放弃更改，允许关闭
                break;

            case QMessageBox::Cancel:
            default:
                event->ignore();  // 取消，不关闭
                break;
            }
        } else {
            // 没有未保存更改，直接关闭
            event->accept();
        }

        // 如果接受关闭，保存窗口状态
        if (event->isAccepted()) {
            saveWindowState();
        }
    }

private:
    bool hasUnsavedChanges() const {
        return m_modified;
    }

    bool save() {
        // 保存逻辑
        return true;
    }

    void saveWindowState() {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
    }

    bool m_modified = false;
};
```

---

## 6. 事件处理流程

### 6.1 完整的事件分发流程

**面试问题**：描述 Qt 事件从产生到处理的完整流程。

**答案**：

```cpp
// Qt 事件分发流程（简化）

// 1. 事件产生
//    - 系统事件（鼠标、键盘）由操作系统产生
//    - Qt 事件由 QCoreApplication::postEvent/sendEvent 产生

// 2. QCoreApplication::notify() - 事件分发入口
class MyApplication : public QApplication {
public:
    bool notify(QObject* receiver, QEvent* event) override {
        // 这里可以捕获所有未处理的异常
        try {
            return QApplication::notify(receiver, event);
        } catch (std::exception& e) {
            qCritical() << "Exception in event handler:" << e.what();
            return false;
        }
    }
};

// 3. 事件过滤器（从应用级到对象级）
//    app.installEventFilter(globalFilter)
//    ↓
//    object->installEventFilter(localFilter)

// 4. QObject::event() - 对象级事件分发

// 5. 具体事件处理函数
//    mousePressEvent(), keyPressEvent(), etc.

// 6. 事件传播（如果 ignore）
//    子控件 → 父控件 → 祖父控件 → ...

// 完整流程图：
/*
系统/Qt产生事件
       ↓
QCoreApplication::notify()
       ↓
应用级事件过滤器 (app.installEventFilter)
       ↓ (如果返回 false)
对象级事件过滤器 (obj->installEventFilter)
       ↓ (如果返回 false)
QObject::event()
       ↓
具体事件处理函数 (mousePressEvent, etc.)
       ↓ (如果 ignore)
传递给父对象
*/

// 示例代码展示流程
class EventFlowDemo : public QWidget {
    Q_OBJECT

protected:
    bool event(QEvent* e) override {
        qDebug() << "2. event() called, type:" << e->type();
        return QWidget::event(e);
    }

    void mousePressEvent(QMouseEvent* e) override {
        qDebug() << "3. mousePressEvent() called";
        // e->ignore();  // 如果取消注释，会传递给父控件
        QWidget::mousePressEvent(e);
    }
};

class EventFlowFilter : public QObject {
protected:
    bool eventFilter(QObject* obj, QEvent* e) override {
        if (e->type() == QEvent::MouseButtonPress) {
            qDebug() << "1. eventFilter() called";
        }
        return false;  // 继续传递
    }
};
```

---

## 7. 常见面试题

### Q1: 为什么 Qt 需要事件系统，信号槽不够吗？

**答案**：

```cpp
// 事件系统的独特优势：

// 1. 底层输入处理
// 鼠标、键盘等硬件输入天然适合事件模型
void mousePressEvent(QMouseEvent* e) {
    // 获取位置、按钮、修饰键等详细信息
}

// 2. 事件过滤和拦截
// 信号无法被拦截，事件可以
bool eventFilter(QObject* obj, QEvent* e) {
    if (shouldBlock(e)) return true;  // 拦截
    return false;
}

// 3. 事件传播
// 事件可以沿对象树传播，信号不行
void keyPressEvent(QKeyEvent* e) {
    if (!handleKey(e)) {
        e->ignore();  // 传递给父控件
    }
}

// 4. 绘制系统
// paintEvent 是绘制的唯一入口
void paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    // 绘制逻辑
}

// 5. 合并优化
// 多个 update() 调用会合并为一个 paintEvent
// 信号每次都会触发槽
```

---

### Q2: 如何阻止事件传播？

**答案**：

```cpp
// 方式 1：event->accept()（默认）
void mousePressEvent(QMouseEvent* e) {
    handleClick();
    e->accept();  // 事件已处理，不传播
}

// 方式 2：事件处理函数返回 true
bool event(QEvent* e) override {
    if (e->type() == QEvent::KeyPress) {
        return true;  // 已处理
    }
    return QWidget::event(e);
}

// 方式 3：事件过滤器返回 true
bool eventFilter(QObject* obj, QEvent* e) override {
    if (shouldIntercept(e)) {
        return true;  // 拦截，不传递
    }
    return false;
}

// 注意：不调用基类实现
void keyPressEvent(QKeyEvent* e) override {
    if (e->key() == Qt::Key_Escape) {
        close();
        // 不调用 QWidget::keyPressEvent(e)
        return;
    }
    QWidget::keyPressEvent(e);  // 其他按键正常处理
}
```

---

### Q3: update() 和 repaint() 的区别？

**答案**：

```cpp
class UpdateVsRepaint : public QWidget {
public:
    void demonstrateDifference() {
        // update() - 推荐使用
        // 1. 异步：投递绘制事件到队列
        // 2. 合并：多次调用只触发一次 paintEvent
        // 3. 高效：不会阻塞当前代码
        update();
        update();
        update();  // 三次调用只触发一次 paintEvent

        // repaint() - 谨慎使用
        // 1. 同步：立即触发 paintEvent
        // 2. 不合并：每次调用都触发重绘
        // 3. 可能导致性能问题
        repaint();
        repaint();
        repaint();  // 三次调用触发三次 paintEvent

        // 使用场景
        // update()：常规 UI 更新
        // repaint()：必须立即显示（如打印预览）
    }
};
```

---

### Q4: 如何处理没有专门虚函数的事件？

**答案**：

```cpp
class CustomEventHandler : public QWidget {
    Q_OBJECT

protected:
    bool event(QEvent* e) override {
        switch (e->type()) {
        // 手势事件（没有专门的虚函数）
        case QEvent::Gesture:
            return handleGesture(static_cast<QGestureEvent*>(e));

        // 工具提示事件
        case QEvent::ToolTip:
            return handleToolTip(static_cast<QHelpEvent*>(e));

        // 状态提示事件
        case QEvent::StatusTip:
            return handleStatusTip(static_cast<QStatusTipEvent*>(e));

        // 查询事件
        case QEvent::WhatsThis:
            return handleWhatsThis(static_cast<QHelpEvent*>(e));

        // 输入法事件
        case QEvent::InputMethod:
            return handleInputMethod(static_cast<QInputMethodEvent*>(e));

        default:
            return QWidget::event(e);
        }
    }

private:
    bool handleGesture(QGestureEvent* e) {
        if (QGesture* pinch = e->gesture(Qt::PinchGesture)) {
            // 处理捏合手势
        }
        return true;
    }

    bool handleToolTip(QHelpEvent* e) {
        QToolTip::showText(e->globalPos(), "Custom tooltip");
        return true;
    }

    // ... 其他处理函数
};
```

---

## 总结

Qt 事件系统的核心要点：

1. **事件类型** - QEvent 及其子类代表不同类型的事件
2. **事件处理** - 重写虚函数或 event() 处理事件
3. **事件过滤** - installEventFilter 监控其他对象
4. **事件传播** - accept/ignore 控制传播行为
5. **自定义事件** - registerEventType 创建自定义类型
6. **sendEvent vs postEvent** - 同步 vs 异步发送

理解事件系统对于开发交互式 Qt 应用至关重要，特别是在处理用户输入和自定义控件时。
