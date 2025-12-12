# Qt 对象模型（Object Model）

## 概述

Qt 对象模型是 Qt 框架的核心基础，它扩展了 C++ 的功能，提供了信号槽机制、属性系统、对象树、内省等特性。理解对象模型对于深入使用 Qt 至关重要。

---

## 1. QObject 基础

### 1.1 QObject 是什么？

**面试问题**：QObject 是什么？为什么它是 Qt 的核心？

**答案**：

QObject 是 Qt 对象模型的基类，几乎所有 Qt 类都直接或间接继承自它。它提供了：

1. **信号槽机制** - 对象间通信
2. **属性系统** - 动态属性支持
3. **对象树** - 自动内存管理
4. **元对象系统** - 运行时类型信息
5. **事件处理** - 事件过滤和处理

```cpp
#include <QObject>
#include <QDebug>

class MyObject : public QObject {
    Q_OBJECT  // 必须包含这个宏

public:
    explicit MyObject(QObject* parent = nullptr)
        : QObject(parent)
    {
        setObjectName("MyObject");
    }

signals:
    void valueChanged(int newValue);

public slots:
    void setValue(int value) {
        if (m_value != value) {
            m_value = value;
            emit valueChanged(value);
        }
    }

private:
    int m_value = 0;
};
```

**关键要点**：
- 必须包含 `Q_OBJECT` 宏才能使用信号槽
- 构造函数通常接受 `QObject* parent` 参数
- QObject 不能被拷贝（拷贝构造和赋值运算符被删除）

---

### 1.2 Q_OBJECT 宏的作用

**面试问题**：Q_OBJECT 宏做了什么？为什么必须使用它？

**答案**：

`Q_OBJECT` 宏为类启用 Qt 元对象系统的功能：

```cpp
// Q_OBJECT 宏大致展开为：
#define Q_OBJECT \
public: \
    QT_WARNING_PUSH \
    Q_OBJECT_NO_OVERRIDE_WARNING \
    static const QMetaObject staticMetaObject; \
    virtual const QMetaObject *metaObject() const; \
    virtual void *qt_metacast(const char *); \
    virtual int qt_metacall(QMetaObject::Call, int, void **); \
    QT_TR_FUNCTIONS \
private: \
    Q_OBJECT_NO_ATTRIBUTES_WARNING \
    Q_DECL_HIDDEN_STATIC_METACALL static void qt_static_metacall(QObject *, QMetaObject::Call, int, void **); \
    QT_WARNING_POP \
    struct QPrivateSignal {}; \
    QT_ANNOTATE_CLASS(qt_qobject, "")
```

**Q_OBJECT 启用的功能**：

| 功能 | 描述 |
|------|------|
| `metaObject()` | 返回类的元对象 |
| `staticMetaObject` | 静态元对象实例 |
| `qt_metacall()` | 元调用处理 |
| `tr()` | 国际化支持 |
| 信号槽 | connect/disconnect |
| 属性系统 | Q_PROPERTY |
| 动态属性 | setProperty/property |

**常见陷阱**：
```cpp
// 错误：忘记 Q_OBJECT 宏
class BadExample : public QObject {
    // 没有 Q_OBJECT
signals:
    void mySignal();  // 编译通过但信号不工作！
};

// 错误：Q_OBJECT 不在类声明开头
class BadExample2 : public QObject {
    int m_value;
    Q_OBJECT  // 应该放在最开头
};
```

---

### 1.3 QObject 的不可拷贝性

**面试问题**：为什么 QObject 不能被拷贝？

**答案**：

QObject 禁用了拷贝构造函数和赋值运算符：

```cpp
class QObject {
    Q_DISABLE_COPY(QObject)  // 展开为：
    // QObject(const QObject&) = delete;
    // QObject& operator=(const QObject&) = delete;
};
```

**原因**：

1. **对象身份（Identity）**
   - 每个 QObject 有唯一的 objectName
   - 拷贝后哪个是"真正的"对象？

2. **信号槽连接**
   - 连接是针对特定对象实例的
   - 拷贝后连接应该如何处理？

3. **对象树关系**
   - 对象有唯一的父对象
   - 拷贝后父子关系如何处理？

4. **定时器和事件**
   - 定时器 ID 是对象特有的
   - 拷贝后定时器归属不明确

```cpp
// 错误示例
QObject obj1;
QObject obj2 = obj1;  // 编译错误！

// 正确做法：使用指针
QObject* obj1 = new QObject();
QObject* obj2 = obj1;  // 共享同一对象

// 或者克隆数据（如果需要）
class MyData : public QObject {
    Q_OBJECT
public:
    MyData* clone() const {
        MyData* copy = new MyData();
        copy->m_value = m_value;
        return copy;
    }
private:
    int m_value;
};
```

---

## 2. 对象树与内存管理

### 2.1 对象树机制

**面试问题**：什么是 Qt 对象树？它如何帮助内存管理？

**答案**：

Qt 对象树是一种父子关系结构，当父对象被销毁时，会自动销毁所有子对象。

```cpp
#include <QObject>
#include <QDebug>

class Child : public QObject {
    Q_OBJECT
public:
    explicit Child(const QString& name, QObject* parent = nullptr)
        : QObject(parent)
    {
        setObjectName(name);
        qDebug() << "Child created:" << name;
    }

    ~Child() {
        qDebug() << "Child destroyed:" << objectName();
    }
};

void objectTreeDemo() {
    QObject* parent = new QObject();
    parent->setObjectName("Parent");

    // 创建子对象，指定父对象
    new Child("Child1", parent);
    new Child("Child2", parent);
    new Child("Child3", parent);

    // 查看子对象
    qDebug() << "Children:" << parent->children();

    // 删除父对象，子对象自动被删除
    delete parent;
    // 输出：
    // Child destroyed: Child3
    // Child destroyed: Child2
    // Child destroyed: Child1
}
```

**对象树的特点**：

```cpp
// 1. 动态添加/移除子对象
QObject* child = new QObject(parent);
child->setParent(nullptr);  // 从树中移除
child->setParent(parent);   // 重新添加

// 2. 查找子对象
QObject* found = parent->findChild<QObject*>("Child1");

// 3. 查找所有特定类型的子对象
QList<Child*> allChildren = parent->findChildren<Child*>();

// 4. 递归查找
QList<Child*> allDescendants = parent->findChildren<Child*>(
    QString(), Qt::FindChildrenRecursively);
```

---

### 2.2 对象销毁顺序

**面试问题**：Qt 对象树的销毁顺序是怎样的？

**答案**：

```cpp
class TrackedObject : public QObject {
    Q_OBJECT
public:
    explicit TrackedObject(const QString& name, QObject* parent = nullptr)
        : QObject(parent)
    {
        setObjectName(name);
        qDebug() << "Created:" << name;
    }

    ~TrackedObject() {
        qDebug() << "Destroyed:" << objectName();
    }
};

void destructionOrderDemo() {
    // 创建层级结构
    //      Root
    //     /    \
    //   A        B
    //  / \      / \
    // A1  A2  B1  B2

    auto* root = new TrackedObject("Root");
    auto* a = new TrackedObject("A", root);
    auto* b = new TrackedObject("B", root);
    new TrackedObject("A1", a);
    new TrackedObject("A2", a);
    new TrackedObject("B1", b);
    new TrackedObject("B2", b);

    delete root;

    // 输出顺序（后进先出，深度优先）：
    // Destroyed: B2
    // Destroyed: B1
    // Destroyed: B
    // Destroyed: A2
    // Destroyed: A1
    // Destroyed: A
    // Destroyed: Root
}
```

**销毁规则**：
1. 子对象按添加的**逆序**销毁
2. 先销毁子对象，再销毁父对象
3. 递归进行，形成深度优先的逆序遍历

---

### 2.3 deleteLater() 机制

**面试问题**：deleteLater() 是什么？什么时候使用它？

**答案**：

`deleteLater()` 将对象的删除推迟到事件循环的下一次迭代。

```cpp
class Worker : public QObject {
    Q_OBJECT
public:
    void doWork() {
        // 工作完成后自我删除
        emit finished();
        deleteLater();  // 安全删除，不会立即执行
    }

signals:
    void finished();
};

// 使用场景 1：在槽函数中删除发送者
void MyClass::onButtonClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // 错误：直接删除可能导致崩溃
        // delete button;

        // 正确：延迟删除
        button->deleteLater();
    }
}

// 使用场景 2：跨线程删除
void WorkerThread::run() {
    Worker* worker = new Worker();
    connect(worker, &Worker::finished, this, &WorkerThread::quit);

    worker->doWork();

    exec();  // 事件循环
    // deleteLater 会在事件循环中执行删除
}

// 使用场景 3：信号链中的安全删除
class Connection : public QObject {
    Q_OBJECT
public:
    void disconnect() {
        emit aboutToDisconnect();
        // 通知其他对象后再删除
        deleteLater();
        emit disconnected();  // 仍然安全，因为还没被删除
    }

signals:
    void aboutToDisconnect();
    void disconnected();
};
```

**deleteLater 的工作原理**：
1. 投递一个 `QDeferredDeleteEvent` 到事件队列
2. 事件循环处理该事件时执行删除
3. 如果没有事件循环运行，对象不会被删除

```cpp
// 注意：没有事件循环时 deleteLater 不工作
void noEventLoopProblem() {
    QObject* obj = new QObject();
    obj->deleteLater();
    // 如果程序退出前没有事件循环，obj 会泄漏！

    // 解决方案：手动处理事件
    QCoreApplication::processEvents();
    // 或确保有事件循环运行
}
```

---

## 3. 对象标识与查找

### 3.1 objectName 属性

**面试问题**：objectName 有什么用途？

**答案**：

`objectName` 是 QObject 的一个属性，用于标识对象：

```cpp
class NamedObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName NOTIFY objectNameChanged)

public:
    explicit NamedObject(QObject* parent = nullptr)
        : QObject(parent)
    {
        setObjectName("DefaultName");
    }
};

void objectNameDemo() {
    QObject* parent = new QObject();

    QObject* child1 = new QObject(parent);
    child1->setObjectName("ImportantChild");

    QObject* child2 = new QObject(parent);
    child2->setObjectName("AnotherChild");

    // 用途 1：调试输出
    qDebug() << child1;  // QObject(0x..., name = "ImportantChild")

    // 用途 2：通过名称查找对象
    QObject* found = parent->findChild<QObject*>("ImportantChild");

    // 用途 3：样式表选择器
    // QSS: QWidget#ImportantChild { background: red; }

    // 用途 4：测试框架定位元素
    // QTest::keyClick(window->findChild<QLineEdit*>("usernameEdit"), Qt::Key_A);

    delete parent;
}
```

---

### 3.2 findChild 和 findChildren

**面试问题**：如何在对象树中查找特定对象？

**答案**：

```cpp
#include <QObject>
#include <QPushButton>
#include <QLineEdit>
#include <QWidget>

void findChildrenDemo(QWidget* window) {
    // 1. 按名称查找单个对象
    QLineEdit* usernameEdit = window->findChild<QLineEdit*>("usernameEdit");

    // 2. 按类型查找（不指定名称）
    QPushButton* anyButton = window->findChild<QPushButton*>();

    // 3. 查找所有特定类型的对象
    QList<QPushButton*> allButtons = window->findChildren<QPushButton*>();

    // 4. 按名称模式查找（正则表达式）
    QList<QLineEdit*> inputFields = window->findChildren<QLineEdit*>(
        QRegularExpression(".*Input$"));

    // 5. 控制查找深度
    // 只在直接子对象中查找
    QList<QWidget*> directChildren = window->findChildren<QWidget*>(
        QString(), Qt::FindDirectChildrenOnly);

    // 递归查找所有后代（默认行为）
    QList<QWidget*> allDescendants = window->findChildren<QWidget*>(
        QString(), Qt::FindChildrenRecursively);
}

// 实用函数：安全查找并操作
template<typename T>
bool withChild(QObject* parent, const QString& name,
               std::function<void(T*)> action) {
    T* child = parent->findChild<T*>(name);
    if (child) {
        action(child);
        return true;
    }
    qWarning() << "Child not found:" << name;
    return false;
}

// 使用示例
void exampleUsage(QWidget* dialog) {
    withChild<QLineEdit>(dialog, "emailInput", [](QLineEdit* edit) {
        edit->setText("example@email.com");
    });
}
```

---

## 4. 类型转换与内省

### 4.1 qobject_cast

**面试问题**：qobject_cast 和 dynamic_cast 有什么区别？

**答案**：

```cpp
#include <QObject>
#include <QPushButton>
#include <QLabel>

void castComparisonDemo() {
    QObject* obj = new QPushButton("Click me");

    // 方式 1：qobject_cast（推荐）
    QPushButton* btn1 = qobject_cast<QPushButton*>(obj);
    // 优点：不需要 RTTI，使用 Qt 元对象系统
    // 缺点：只能用于 QObject 派生类

    // 方式 2：dynamic_cast
    QPushButton* btn2 = dynamic_cast<QPushButton*>(obj);
    // 优点：标准 C++，适用于任何多态类
    // 缺点：需要 RTTI，某些平台可能被禁用

    // 方式 3：static_cast（危险）
    QPushButton* btn3 = static_cast<QPushButton*>(obj);
    // 缺点：不进行类型检查，错误的转换导致未定义行为

    // 失败时的行为
    QLabel* label1 = qobject_cast<QLabel*>(obj);  // 返回 nullptr
    QLabel* label2 = dynamic_cast<QLabel*>(obj);  // 返回 nullptr
    QLabel* label3 = static_cast<QLabel*>(obj);   // 危险！返回无效指针

    delete obj;
}
```

**qobject_cast 的实现原理**：

```cpp
// 简化的实现原理
template<typename T>
T qobject_cast(QObject* object) {
    // 使用元对象系统检查类型
    using TargetType = std::remove_pointer_t<T>;

    if (object && object->qt_metacast(TargetType::staticMetaObject.className())) {
        return static_cast<T>(object);
    }
    return nullptr;
}

// 使用示例
void safeCastDemo(QObject* sender) {
    // 常见用法：在槽函数中获取发送者
    if (QPushButton* button = qobject_cast<QPushButton*>(sender)) {
        qDebug() << "Button clicked:" << button->text();
    } else if (QAction* action = qobject_cast<QAction*>(sender)) {
        qDebug() << "Action triggered:" << action->text();
    }
}
```

---

### 4.2 inherits() 方法

**面试问题**：如何在运行时检查对象的类型？

**答案**：

```cpp
void typeCheckingDemo() {
    QObject* obj = new QPushButton("Test");

    // 方法 1：inherits() - 检查继承关系
    bool isWidget = obj->inherits("QWidget");      // true
    bool isButton = obj->inherits("QPushButton");  // true
    bool isLabel = obj->inherits("QLabel");        // false

    // 方法 2：metaObject()->className()
    const char* className = obj->metaObject()->className();
    qDebug() << "Class name:" << className;  // "QPushButton"

    // 方法 3：检查元对象继承链
    const QMetaObject* meta = obj->metaObject();
    while (meta) {
        qDebug() << "  -" << meta->className();
        meta = meta->superClass();
    }
    // 输出：
    //   - QPushButton
    //   - QAbstractButton
    //   - QWidget
    //   - QObject

    delete obj;
}

// 实用函数：类型安全的事件处理
class TypedEventFilter : public QObject {
    Q_OBJECT
public:
    bool eventFilter(QObject* watched, QEvent* event) override {
        // 只处理 QPushButton 的事件
        if (!watched->inherits("QPushButton")) {
            return false;
        }

        if (event->type() == QEvent::MouseButtonPress) {
            qDebug() << "Button pressed:" << watched->objectName();
            return true;  // 事件已处理
        }

        return QObject::eventFilter(watched, event);
    }
};
```

---

## 5. 对象生命周期管理

### 5.1 构造和初始化顺序

**面试问题**：QObject 派生类的构造顺序是怎样的？

**答案**：

```cpp
class BaseWidget : public QWidget {
    Q_OBJECT
public:
    explicit BaseWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        qDebug() << "1. BaseWidget constructor";
        // 此时 metaObject() 返回 BaseWidget 的元对象
        qDebug() << "   metaObject:" << metaObject()->className();
    }

    void initialize() {
        qDebug() << "3. BaseWidget::initialize";
        // 虚函数正常工作
        setupUI();
    }

protected:
    virtual void setupUI() {
        qDebug() << "4. BaseWidget::setupUI";
    }
};

class DerivedWidget : public BaseWidget {
    Q_OBJECT
public:
    explicit DerivedWidget(QWidget* parent = nullptr)
        : BaseWidget(parent)
    {
        qDebug() << "2. DerivedWidget constructor";
        // 现在 metaObject() 返回 DerivedWidget 的元对象
        qDebug() << "   metaObject:" << metaObject()->className();

        // 初始化应该在构造函数末尾或单独调用
        initialize();
    }

protected:
    void setupUI() override {
        BaseWidget::setupUI();
        qDebug() << "5. DerivedWidget::setupUI";
    }
};

// 输出：
// 1. BaseWidget constructor
//    metaObject: BaseWidget
// 2. DerivedWidget constructor
//    metaObject: DerivedWidget
// 3. BaseWidget::initialize
// 4. BaseWidget::setupUI
// 5. DerivedWidget::setupUI
```

**关键要点**：
- 构造函数中调用虚函数**不会**调用派生类版本
- 使用两阶段初始化或在派生类构造函数中调用初始化方法

---

### 5.2 QPointer 弱引用

**面试问题**：什么是 QPointer？什么时候使用它？

**答案**：

`QPointer` 是一个模板类，提供对 QObject 的弱引用（guarded pointer）：

```cpp
#include <QPointer>
#include <QWidget>
#include <QTimer>

class SafeReference {
public:
    void setTarget(QWidget* widget) {
        // QPointer 自动追踪对象生命周期
        m_target = widget;
    }

    void doSomethingWithTarget() {
        // 安全检查：如果对象已删除，m_target 自动变为 nullptr
        if (m_target) {
            m_target->show();
        } else {
            qWarning() << "Target widget was deleted!";
        }
    }

private:
    QPointer<QWidget> m_target;
};

void qpointerDemo() {
    QPointer<QWidget> weakRef;

    {
        QWidget* widget = new QWidget();
        weakRef = widget;

        qDebug() << "Before delete:" << (weakRef != nullptr);  // true

        delete widget;
    }

    qDebug() << "After delete:" << (weakRef != nullptr);  // false
    // weakRef 自动变为 nullptr，安全！
}

// 常见使用场景：异步操作
class AsyncWorker : public QObject {
    Q_OBJECT
public:
    void startAsyncOperation(QWidget* targetWidget) {
        m_target = targetWidget;

        // 模拟异步操作
        QTimer::singleShot(5000, this, [this]() {
            // 5秒后，widget 可能已经被删除
            if (m_target) {
                m_target->setWindowTitle("Operation Complete");
            }
            // 如果 widget 已删除，什么都不做（安全）
        });
    }

private:
    QPointer<QWidget> m_target;
};
```

**QPointer vs 原始指针 vs 智能指针**：

| 特性 | 原始指针 | QPointer | QSharedPointer |
|------|----------|----------|----------------|
| 自动置空 | ❌ | ✅ | ✅ |
| 延长生命周期 | ❌ | ❌ | ✅ |
| 适用类型 | 任意 | QObject | 任意 |
| 开销 | 无 | 低 | 中等 |

---

### 5.3 智能指针与 QObject

**面试问题**：如何安全地管理 QObject 的生命周期？

**答案**：

```cpp
#include <QObject>
#include <QSharedPointer>
#include <QScopedPointer>
#include <memory>

// 方式 1：QScopedPointer（作用域指针）
void scopedPointerDemo() {
    QScopedPointer<QObject> obj(new QObject());
    obj->setObjectName("ScopedObject");
    // 离开作用域时自动删除
}

// 方式 2：std::unique_ptr
void uniquePtrDemo() {
    auto obj = std::make_unique<QObject>();
    obj->setObjectName("UniqueObject");
    // 离开作用域时自动删除
}

// 方式 3：QSharedPointer（共享所有权）
void sharedPointerDemo() {
    QSharedPointer<QObject> ptr1(new QObject());
    QSharedPointer<QObject> ptr2 = ptr1;  // 共享所有权

    ptr1.clear();  // ptr2 仍持有对象
    // 当最后一个 QSharedPointer 销毁时，对象被删除
}

// 注意：对象树与智能指针的冲突
void conflictDemo() {
    QWidget* parent = new QWidget();

    // 危险！双重删除
    // std::unique_ptr<QWidget> child(new QWidget(parent));
    // parent 删除时会删除 child，unique_ptr 也会删除 child！

    // 解决方案 1：不使用父对象
    std::unique_ptr<QWidget> safeChild(new QWidget());  // 无父对象

    // 解决方案 2：使用 QPointer 而非智能指针
    QPointer<QWidget> weakChild = new QWidget(parent);

    // 解决方案 3：使用自定义删除器
    std::unique_ptr<QWidget, std::function<void(QWidget*)>> managedChild(
        new QWidget(parent),
        [](QWidget* w) {
            if (w && !w->parent()) {
                delete w;  // 只在没有父对象时删除
            }
        }
    );

    delete parent;
}
```

---

## 6. 线程与对象

### 6.1 对象的线程亲和性

**面试问题**：什么是 QObject 的线程亲和性（thread affinity）？

**答案**：

每个 QObject 都属于特定的线程，决定了它的事件处理和槽函数执行位置：

```cpp
#include <QObject>
#include <QThread>
#include <QDebug>

class Worker : public QObject {
    Q_OBJECT
public:
    Worker() {
        qDebug() << "Worker created in thread:" << QThread::currentThread();
    }

public slots:
    void doWork() {
        qDebug() << "Work executed in thread:" << QThread::currentThread();
    }
};

void threadAffinityDemo() {
    qDebug() << "Main thread:" << QThread::currentThread();

    // 在主线程创建对象
    Worker* worker = new Worker();
    qDebug() << "Worker's thread:" << worker->thread();

    // 创建新线程
    QThread* workerThread = new QThread();

    // 移动对象到新线程
    worker->moveToThread(workerThread);
    qDebug() << "After move, worker's thread:" << worker->thread();

    // 启动线程
    workerThread->start();

    // 现在调用 doWork() 槽会在 workerThread 中执行
    QMetaObject::invokeMethod(worker, "doWork", Qt::QueuedConnection);

    // 清理
    workerThread->quit();
    workerThread->wait();
    delete worker;
    delete workerThread;
}
```

**关键规则**：

```cpp
// 规则 1：对象只能在其所属线程中使用定时器
class TimerWorker : public QObject {
    Q_OBJECT
public:
    void start() {
        // 必须在对象所属线程中调用
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &TimerWorker::onTimeout);
        m_timer->start(1000);
    }

private slots:
    void onTimeout() {
        qDebug() << "Timer in thread:" << QThread::currentThread();
    }

private:
    QTimer* m_timer = nullptr;
};

// 规则 2：不能移动有父对象的对象
void moveWithParentDemo() {
    QObject* parent = new QObject();
    QObject* child = new QObject(parent);

    QThread* thread = new QThread();

    // 错误！child 有父对象
    // child->moveToThread(thread);

    // 正确：移动整个树
    parent->moveToThread(thread);
    // 现在 parent 和 child 都在新线程中

    delete parent;
    delete thread;
}

// 规则 3：只能将对象推送到其他线程，不能拉取
void pushNotPullDemo() {
    QThread* thread = new QThread();
    thread->start();

    QObject* obj = new QObject();

    // 在主线程中可以将对象移动到 workerThread
    obj->moveToThread(thread);  // OK：推送

    // 不能在 workerThread 中将对象移回主线程
    // 必须由拥有对象的线程执行 moveToThread

    thread->quit();
    thread->wait();
    delete obj;
    delete thread;
}
```

---

## 7. 常见面试题

### Q1: QObject 和普通 C++ 类有什么区别？

**答案**：

| 特性 | 普通 C++ 类 | QObject 派生类 |
|------|-------------|----------------|
| 拷贝语义 | 可拷贝 | 禁止拷贝 |
| 信号槽 | 不支持 | 支持 |
| 属性系统 | 不支持 | 支持 |
| 对象树 | 不支持 | 支持 |
| 内省/反射 | 有限（RTTI） | 完整 |
| 动态属性 | 不支持 | 支持 |
| 事件处理 | 不支持 | 支持 |
| 国际化 | 手动实现 | tr() 支持 |

---

### Q2: 为什么子对象的删除顺序是逆序？

**答案**：

1. **依赖关系**：后创建的对象可能依赖先创建的对象
2. **一致性**：与 C++ 局部变量销毁顺序一致（LIFO）
3. **资源释放**：确保依赖资源在被依赖资源之前释放

```cpp
void destructionOrderReason() {
    QObject* parent = new QObject();

    QFile* file = new QFile("data.txt", parent);  // 先创建
    QTextStream* stream = new QTextStream(file, parent);  // 后创建，依赖 file

    // 如果先删除 file，stream 会访问无效的 QFile！
    // 逆序删除：先删 stream，再删 file
    delete parent;
}
```

---

### Q3: moveToThread 的注意事项？

**答案**：

```cpp
// 1. 不能移动有父对象的对象
QObject* parent = new QObject();
QObject* child = new QObject(parent);
child->moveToThread(thread);  // 错误！

// 2. 移动后原线程不应再访问该对象
Worker* worker = new Worker();
worker->moveToThread(thread);
worker->doWork();  // 错误！应该通过信号槽调用

// 3. 不能在目标线程中调用 moveToThread
// 必须在对象当前所属线程中调用

// 4. 移动会影响定时器
QTimer* timer = new QTimer(worker);
worker->moveToThread(thread);
// timer 也被移动，需要重新启动
```

---

## 总结

Qt 对象模型的核心概念：

1. **QObject** - 所有 Qt 对象的基类，提供核心功能
2. **Q_OBJECT 宏** - 启用元对象系统功能
3. **对象树** - 自动化内存管理
4. **不可拷贝性** - 保护对象身份和关系
5. **线程亲和性** - 确保线程安全的事件处理
6. **类型转换** - qobject_cast 提供安全的向下转换

理解这些概念对于编写正确、高效的 Qt 应用程序至关重要。
