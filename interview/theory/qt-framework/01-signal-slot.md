# 信号与槽 (Signal & Slot)

## 概述

信号与槽是 Qt 框架的核心机制，用于对象间的通信。它提供了一种类型安全、松耦合的方式来连接事件发送者和接收者。

---

## 1. 基本概念

### 什么是信号与槽？

```cpp
// 信号：当特定事件发生时发射
// 槽：响应信号的函数
class Counter : public QObject {
    Q_OBJECT  // 必须包含此宏

public:
    Counter() : m_value(0) {}
    int value() const { return m_value; }

public slots:
    void setValue(int value) {
        if (value != m_value) {
            m_value = value;
            emit valueChanged(value);  // 发射信号
        }
    }

signals:
    void valueChanged(int newValue);  // 信号声明，无需实现

private:
    int m_value;
};
```

### 连接信号与槽

```cpp
// Qt5 新语法（推荐）- 编译时类型检查
Counter a, b;
QObject::connect(&a, &Counter::valueChanged,
                 &b, &Counter::setValue);

// Qt4 旧语法 - 运行时检查，容易出错
connect(&a, SIGNAL(valueChanged(int)),
        &b, SLOT(setValue(int)));

// Lambda 表达式
connect(&a, &Counter::valueChanged, [](int value) {
    qDebug() << "Value changed to:" << value;
});
```

---

## 2. 信号槽的连接类型

### Qt::ConnectionType 枚举

```cpp
enum ConnectionType {
    AutoConnection,       // 默认，自动选择
    DirectConnection,     // 直接调用，同步
    QueuedConnection,     // 队列调用，异步
    BlockingQueuedConnection,  // 阻塞队列调用
    UniqueConnection      // 唯一连接（可与其他类型组合）
};
```

### 各连接类型详解

```cpp
// 1. AutoConnection（默认）
// 同线程 -> DirectConnection
// 跨线程 -> QueuedConnection
connect(sender, &Sender::signal, receiver, &Receiver::slot);

// 2. DirectConnection - 立即调用，在发送者线程执行
connect(sender, &Sender::signal,
        receiver, &Receiver::slot, Qt::DirectConnection);

// 3. QueuedConnection - 投递到接收者线程的事件队列
connect(sender, &Sender::signal,
        receiver, &Receiver::slot, Qt::QueuedConnection);

// 4. BlockingQueuedConnection - 阻塞直到槽执行完毕
// 注意：发送者和接收者必须在不同线程，否则死锁
connect(sender, &Sender::signal,
        receiver, &Receiver::slot, Qt::BlockingQueuedConnection);

// 5. UniqueConnection - 防止重复连接
connect(sender, &Sender::signal,
        receiver, &Receiver::slot,
        Qt::AutoConnection | Qt::UniqueConnection);
```

### 面试题：跨线程信号槽如何工作？

```cpp
// 跨线程通信示例
class Worker : public QObject {
    Q_OBJECT
public slots:
    void doWork() {
        // 在工作线程执行
        QString result = heavyComputation();
        emit workDone(result);
    }
signals:
    void workDone(const QString& result);
};

// 主线程
Worker* worker = new Worker;
QThread* thread = new QThread;

worker->moveToThread(thread);

// QueuedConnection：信号参数被复制并投递到事件队列
connect(this, &Controller::startWork,
        worker, &Worker::doWork);  // 跨线程，自动 Queued
connect(worker, &Worker::workDone,
        this, &Controller::handleResult);  // 跨线程，自动 Queued

thread->start();
```

**关键要点：**
- 参数通过 `QMetaType` 系统序列化
- 自定义类型需要 `qRegisterMetaType<T>()`
- 槽函数在接收者所属线程的事件循环中执行

---

## 3. 信号槽的实现原理

### MOC (Meta-Object Compiler) 的作用

```cpp
// 源代码
class MyClass : public QObject {
    Q_OBJECT
signals:
    void mySignal(int value);
};

// MOC 生成的代码（简化）
void MyClass::mySignal(int value) {
    QMetaObject::activate(this, &staticMetaObject,
                          0,  // 信号索引
                          const_cast<void**>(reinterpret_cast<void* const*>(&value)));
}
```

### 连接的内部实现

```cpp
// QObject 内部维护连接列表
struct Connection {
    QObject* sender;
    QObject* receiver;
    int signalIndex;
    int slotIndex;
    Qt::ConnectionType type;
    // ...
};

// 发射信号时
void QMetaObject::activate(QObject* sender,
                           const QMetaObject* m,
                           int signalIndex,
                           void** argv) {
    // 1. 获取该信号的所有连接
    // 2. 根据 ConnectionType 决定调用方式
    // 3. 直接调用或投递事件
}
```

### 面试题：信号槽比回调函数的优缺点？

**优点：**
1. **类型安全**：编译时检查参数类型（Qt5 新语法）
2. **松耦合**：发送者不需要知道接收者
3. **多对多**：一个信号可连接多个槽，一个槽可接收多个信号
4. **跨线程**：内置线程安全的跨线程通信
5. **自动断开**：对象销毁时自动断开连接

**缺点：**
1. **性能开销**：比直接函数调用慢约 10 倍
2. **需要 MOC**：增加编译复杂性
3. **调试困难**：信号槽链路不如直接调用直观

---

## 4. 常见面试问题

### Q1: 信号可以连接信号吗？

```cpp
// 可以！信号可以连接到另一个信号
class Wrapper : public QObject {
    Q_OBJECT
signals:
    void wrapperSignal(int value);
};

// 当 counter.valueChanged 发射时，wrapper.wrapperSignal 也会发射
connect(&counter, &Counter::valueChanged,
        &wrapper, &Wrapper::wrapperSignal);
```

### Q2: 如何断开连接？

```cpp
// 方法1：断开特定连接
QObject::disconnect(&sender, &Sender::signal,
                    &receiver, &Receiver::slot);

// 方法2：断开发送者的所有信号
sender.disconnect();

// 方法3：断开接收者的所有槽
receiver.disconnect();

// 方法4：使用连接句柄
QMetaObject::Connection conn = connect(...);
QObject::disconnect(conn);

// 方法5：接收者销毁时自动断开
// QObject 析构函数会自动处理
```

### Q3: 槽函数可以有返回值吗？

```cpp
// 技术上可以，但通常没有意义
class MyClass : public QObject {
    Q_OBJECT
public slots:
    int mySlot() { return 42; }  // 返回值会被忽略
};

// 原因：
// 1. 一个信号可能连接多个槽，返回哪个？
// 2. QueuedConnection 是异步的，无法获取返回值
```

### Q4: 如何在非 QObject 类中使用信号槽？

```cpp
// 不能直接使用，但有替代方案：

// 方案1：组合模式
class NonQObject {
public:
    QObject* signalHelper() { return &m_helper; }
private:
    QObject m_helper;  // 内部 QObject 代理信号
};

// 方案2：使用 std::function 回调
class NonQObject {
public:
    void setCallback(std::function<void(int)> cb) { m_callback = cb; }
    void triggerCallback(int val) { if (m_callback) m_callback(val); }
private:
    std::function<void(int)> m_callback;
};
```

### Q5: 信号槽的线程安全性？

```cpp
// connect/disconnect 是线程安全的
// 但需注意：

// 1. 同一信号的多个槽执行顺序不确定
connect(&a, &A::sig, &b, &B::slot1);
connect(&a, &A::sig, &c, &C::slot2);
// slot1 和 slot2 的执行顺序未定义

// 2. DirectConnection 在发送者线程执行
// 如果槽访问接收者数据，需要额外同步

// 3. QueuedConnection 参数必须可复制
// 自定义类型需要注册
qRegisterMetaType<MyType>("MyType");
```

---

## 5. Qt5 vs Qt4 语法对比

### 新语法优势

```cpp
// Qt4 语法 - 字符串匹配，运行时检查
connect(sender, SIGNAL(valueChanged(int)),
        receiver, SLOT(setValue(int)));
// 问题：拼写错误只能在运行时发现

// Qt5 语法 - 函数指针，编译时检查
connect(sender, &Sender::valueChanged,
        receiver, &Receiver::setValue);
// 优势：编译器检查类型匹配

// Qt5 处理重载函数
connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MyClass::onValueChanged);

// C++14 简化写法
connect(spinBox, qOverload<int>(&QSpinBox::valueChanged),
        this, &MyClass::onValueChanged);
```

---

## 6. 性能优化

### 信号槽性能考量

```cpp
// 1. 避免在高频循环中发射信号
// 差的做法
for (int i = 0; i < 1000000; ++i) {
    emit progressChanged(i);  // 性能问题
}

// 好的做法
for (int i = 0; i < 1000000; ++i) {
    if (i % 1000 == 0)
        emit progressChanged(i);  // 降低频率
}

// 2. 批量更新时阻塞信号
widget->blockSignals(true);
widget->setValue(1);
widget->setValue(2);
widget->setValue(3);
widget->blockSignals(false);
emit widget->valueChanged(3);  // 手动发射一次

// 3. 使用 DirectConnection 避免队列开销（同线程时）
connect(a, &A::sig, b, &B::slot, Qt::DirectConnection);
```

---

## 7. 常见陷阱

### 陷阱1：忘记 Q_OBJECT 宏

```cpp
class MyClass : public QObject {
    // Q_OBJECT  // 忘记添加！
signals:
    void mySignal();  // 编译通过但信号不工作
};
```

### 陷阱2：跨线程传递指针

```cpp
// 危险！指针可能在另一线程失效
connect(worker, &Worker::done,
        this, [](Data* ptr) {  // ptr 可能悬空
            ptr->process();
        });

// 安全：传值或使用智能指针
connect(worker, &Worker::done,
        this, [](std::shared_ptr<Data> data) {
            data->process();
        });
```

### 陷阱3：Lambda 捕获悬空引用

```cpp
void MyClass::setupConnection() {
    int localVar = 42;
    connect(button, &QPushButton::clicked, [&localVar]() {
        qDebug() << localVar;  // 悬空引用！
    });
}  // localVar 已销毁

// 修复：按值捕获
connect(button, &QPushButton::clicked, [localVar]() {
    qDebug() << localVar;  // 安全
});
```

### 陷阱4：BlockingQueuedConnection 死锁

```cpp
// 同一线程使用 BlockingQueuedConnection 会死锁
// 发送者等待槽执行完毕，但槽在同一线程的事件队列中
connect(this, &MyClass::mySignal,
        this, &MyClass::mySlot,
        Qt::BlockingQueuedConnection);  // 死锁！

emit mySignal();  // 永远阻塞
```

---

## 8. 总结

| 特性 | 说明 |
|------|------|
| **核心机制** | MOC 生成元信息，运行时动态查找连接 |
| **连接类型** | Auto/Direct/Queued/BlockingQueued |
| **跨线程** | QueuedConnection 自动处理 |
| **类型安全** | Qt5 新语法编译时检查 |
| **生命周期** | 对象销毁时自动断开 |
| **性能** | 比直接调用慢，高频场景需优化 |

**面试必记：**
1. MOC 是什么，做了什么
2. 五种连接类型及适用场景
3. 跨线程信号槽的工作原理
4. Qt5 vs Qt4 语法差异
5. 常见陷阱：Q_OBJECT、跨线程指针、Lambda 捕获
