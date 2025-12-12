# Qt 内部机制

## 概述

深入理解 Qt 的内部实现机制有助于写出更高效的代码和解决复杂问题。本文档涵盖 Qt 核心内部机制的面试常见问题。

---

## 1. 元对象系统内部实现

### 1.1 moc 生成代码分析

```cpp
// 原始类定义
class MyClass : public QObject {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
public:
    int value() const { return m_value; }
    void setValue(int v) {
        if (m_value != v) {
            m_value = v;
            emit valueChanged(v);
        }
    }
signals:
    void valueChanged(int newValue);
private:
    int m_value = 0;
};

// moc 生成的代码（简化版）
// moc_myclass.cpp
#include "myclass.h"
#include <QtCore/qmetatype.h>

// 字符串表：存储类名、方法名、属性名等
static const char qt_meta_stringdata_MyClass[] = {
    "MyClass\0value\0valueChanged\0newValue\0"
    "setValue\0v\0"
};

// 元数据数组
static const uint qt_meta_data_MyClass[] = {
    // content:
    9,       // revision
    0,       // classname (index in stringdata)
    0,    0, // classinfo
    2,   14, // methods (count, offset)
    1,   28, // properties (count, offset)
    // ...
};

// QMetaObject 静态实例
const QMetaObject MyClass::staticMetaObject = {
    { &QObject::staticMetaObject,           // 父类元对象
      qt_meta_stringdata_MyClass,           // 字符串数据
      qt_meta_data_MyClass,                 // 元数据
      qt_static_metacall,                   // 静态元调用函数
      nullptr, nullptr }
};

// 虚函数实现
const QMetaObject *MyClass::metaObject() const {
    return &staticMetaObject;
}

// 静态元调用
void MyClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c,
                                  int _id, void **_a) {
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyClass *>(_o);
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast<int*>(_a[1]))); break;
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MyClass *>(_o);
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_a[0]) = _t->value(); break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<MyClass *>(_o);
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast<int*>(_a[1])); break;
        }
    }
}
```

### 1.2 信号实现细节

```cpp
// moc 生成的信号实现
void MyClass::valueChanged(int _t1) {
    void *_a[] = {
        nullptr,                    // 返回值（信号无返回值）
        const_cast<void*>(reinterpret_cast<const void*>(&_t1))
    };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// QMetaObject::activate 核心流程（简化）
void QMetaObject::activate(QObject *sender, const QMetaObject *m,
                           int local_signal_index, void **argv) {
    QObjectPrivate *d = QObjectPrivate::get(sender);

    // 获取连接列表
    QObjectPrivate::ConnectionList &connectionList =
        d->connectionLists->at(local_signal_index);

    // 遍历所有连接
    for (QObjectPrivate::Connection *c = connectionList.first; c; c = c->nextConnectionList) {
        QObject *receiver = c->receiver;

        // 检查连接类型
        if (c->connectionType == Qt::DirectConnection) {
            // 直接调用
            callSlot(receiver, c->method, argv);
        } else if (c->connectionType == Qt::QueuedConnection) {
            // 投递事件
            postEvent(receiver, createMetaCallEvent(c, argv));
        } else if (c->connectionType == Qt::AutoConnection) {
            // 自动判断
            if (receiver->thread() == QThread::currentThread()) {
                callSlot(receiver, c->method, argv);
            } else {
                postEvent(receiver, createMetaCallEvent(c, argv));
            }
        }
    }
}
```

### 1.3 属性系统实现

```cpp
// 属性读取流程
QVariant QObject::property(const char *name) const {
    const QMetaObject *meta = metaObject();
    int index = meta->indexOfProperty(name);
    if (index < 0) {
        // 检查动态属性
        return d_ptr->extraData->propertyValues.value(name);
    }
    QMetaProperty prop = meta->property(index);
    return prop.read(this);
}

// QMetaProperty::read 实现
QVariant QMetaProperty::read(const QObject *obj) const {
    QVariant result;
    void *argv[] = { &result };
    // 调用静态元调用函数
    mobj->d.static_metacall(const_cast<QObject*>(obj),
                            QMetaObject::ReadProperty,
                            idx, argv);
    return result;
}
```

---

## 2. 对象树与内存管理

### 2.1 QObject 内部结构

```cpp
// QObject 私有数据结构（简化）
class QObjectPrivate {
public:
    QObject *q_ptr;              // 指向公共类
    QObject *parent;             // 父对象
    QObjectList children;        // 子对象列表

    // 连接数据
    QObjectConnectionListVector *connectionLists;
    Connection *senders;         // 发送者列表

    // 额外数据
    ExtraData *extraData;        // 动态属性、用户数据等

    // 线程关联
    QThreadData *threadData;

    // 标志位
    uint isWidget : 1;
    uint blockSig : 1;
    uint wasDeleted : 1;
    // ...
};

// ExtraData 结构
struct ExtraData {
    QList<QByteArray> propertyNames;
    QList<QVariant> propertyValues;
    QVector<QObject*> runningTimers;
    // ...
};
```

### 2.2 对象删除机制

```cpp
// QObject 析构函数核心逻辑（简化）
QObject::~QObject() {
    QObjectPrivate *d = d_func();

    // 1. 标记为已删除
    d->wasDeleted = true;

    // 2. 发送 destroyed 信号
    emit destroyed(this);

    // 3. 断开所有信号连接
    if (d->connectionLists) {
        // 断开作为发送者的连接
        disconnectAllSignals();
        // 断开作为接收者的连接
        disconnectAllSlots();
    }

    // 4. 删除所有子对象
    for (QObject *child : d->children) {
        delete child;
    }

    // 5. 从父对象中移除
    if (d->parent) {
        d->parent->d_func()->children.removeOne(this);
    }

    // 6. 清理定时器
    if (d->extraData && !d->extraData->runningTimers.isEmpty()) {
        for (int id : d->extraData->runningTimers) {
            QCoreApplication::removePostedEvents(this, QEvent::Timer);
        }
    }
}

// deleteLater 实现
void QObject::deleteLater() {
    QCoreApplication::postEvent(this, new QDeferredDeleteEvent());
}
```

### 2.3 线程关联

```cpp
// moveToThread 实现核心逻辑
void QObject::moveToThread(QThread *targetThread) {
    QObjectPrivate *d = d_func();

    // 检查：有父对象的不能移动
    if (d->parent != nullptr) {
        qWarning("Cannot move object with parent");
        return;
    }

    // 检查：只能从当前线程移动
    if (d->threadData->thread != QThread::currentThread()) {
        qWarning("Cannot move from non-owner thread");
        return;
    }

    // 发送 ThreadChange 事件
    QEvent e(QEvent::ThreadChange);
    QCoreApplication::sendEvent(this, &e);

    // 递归移动子对象
    for (QObject *child : d->children) {
        child->moveToThread(targetThread);
    }

    // 更新线程数据
    QThreadData *currentData = d->threadData;
    QThreadData *targetData = QThreadData::get2(targetThread);

    // 移动定时器
    moveTimers(currentData, targetData);

    // 更新关联
    d->threadData = targetData;
}
```

---

## 3. 事件系统内部实现

### 3.1 事件分发流程

```cpp
// QCoreApplication::notify 核心流程
bool QCoreApplication::notify(QObject *receiver, QEvent *event) {
    // 1. 全局事件过滤器
    for (QObject *filter : d_func()->eventFilters) {
        if (filter->eventFilter(receiver, event))
            return true;
    }

    // 2. 对象事件过滤器
    QObjectPrivate *d = receiver->d_func();
    for (QObject *filter : d->extraData->eventFilters) {
        if (filter->eventFilter(receiver, event))
            return true;
    }

    // 3. 调用 receiver 的 event() 方法
    return receiver->event(event);
}

// QObject::event 默认实现
bool QObject::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::Timer:
        timerEvent(static_cast<QTimerEvent*>(e));
        break;
    case QEvent::ChildAdded:
    case QEvent::ChildRemoved:
        childEvent(static_cast<QChildEvent*>(e));
        break;
    case QEvent::DeferredDelete:
        // deleteLater 的实际删除
        delete this;
        break;
    case QEvent::MetaCall:
        // 跨线程槽调用
        d_func()->currentSender = nullptr;
        QMetaCallEvent *mce = static_cast<QMetaCallEvent*>(e);
        mce->placeMetaCall(this);
        break;
    // ...
    }
    return true;
}
```

### 3.2 事件循环实现

```cpp
// QEventLoop::exec 核心流程
int QEventLoop::exec(ProcessEventsFlags flags) {
    QEventLoopPrivate *d = d_func();

    // 进入循环
    d->exit = false;
    d->returnCode = 0;

    while (!d->exit) {
        // 处理事件
        QCoreApplication::processEvents(flags | WaitForMoreEvents);
    }

    return d->returnCode;
}

// QCoreApplication::processEvents 实现（简化）
bool QCoreApplication::processEvents(QEventLoop::ProcessEventsFlags flags) {
    QThreadData *data = QThreadData::current();

    // 处理投递的事件
    if (!(flags & QEventLoop::ExcludeUserInputEvents)) {
        while (!data->postEventList.isEmpty()) {
            QPostEvent pe = data->postEventList.takeFirst();
            sendEvent(pe.receiver, pe.event);
            delete pe.event;
        }
    }

    // 处理系统事件（平台相关）
    return data->eventDispatcher->processEvents(flags);
}
```

### 3.3 事件压缩

```cpp
// 鼠标移动事件压缩（QWidget 中）
void QWidgetPrivate::compressMouseMoveEvent(QMouseEvent *event) {
    // 检查事件队列中是否有待处理的鼠标移动事件
    QCoreApplication *app = QCoreApplication::instance();
    QList<QEvent*> &events = app->d_func()->postedEvents;

    for (int i = events.size() - 1; i >= 0; --i) {
        QEvent *e = events.at(i);
        if (e->type() == QEvent::MouseMove &&
            static_cast<QMouseEvent*>(e)->widget() == q_ptr) {
            // 合并事件：更新位置，删除旧事件
            events.removeAt(i);
            delete e;
            break;
        }
    }
}

// 使用压缩标志
void QCoreApplication::postEvent(QObject *receiver, QEvent *event,
                                  int priority) {
    // 某些事件类型默认压缩
    if (event->type() == QEvent::UpdateRequest ||
        event->type() == QEvent::LayoutRequest) {
        // 检查是否已存在相同类型事件
        if (compressEvent(receiver, event)) {
            delete event;
            return;
        }
    }

    // 添加到事件队列
    postEventList.addEvent(receiver, event, priority);
}
```

---

## 4. 隐式共享（写时复制）内部实现

### 4.1 QSharedData 机制

```cpp
// Qt 的引用计数基类
class QSharedData {
public:
    mutable QAtomicInt ref;  // 原子引用计数

    QSharedData() : ref(0) { }
    QSharedData(const QSharedData&) : ref(0) { }  // 拷贝不继承计数

    // 禁止赋值
    QSharedData& operator=(const QSharedData&) = delete;
};

// 智能指针包装
template <class T>
class QSharedDataPointer {
public:
    T *d;

    // 获取数据（const 版本，不触发分离）
    const T* constData() const { return d; }
    const T* data() const { return d; }

    // 获取数据（非 const，可能触发分离）
    T* data() {
        detach();
        return d;
    }

    // 分离：如果共享则复制
    void detach() {
        if (d && d->ref.loadRelaxed() != 1) {
            // 创建副本
            T *newData = new T(*d);
            // 减少原数据引用计数
            if (!d->ref.deref())
                delete d;
            // 使用新数据
            d = newData;
            d->ref.ref();
        }
    }

    // 拷贝构造：增加引用计数
    QSharedDataPointer(const QSharedDataPointer &o) : d(o.d) {
        if (d) d->ref.ref();
    }

    // 析构：减少引用计数
    ~QSharedDataPointer() {
        if (d && !d->ref.deref())
            delete d;
    }
};
```

### 4.2 QString 内部结构

```cpp
// QString 内部数据（简化）
struct QStringData {
    QAtomicInt ref;
    int size;
    int alloc;          // 分配的容量
    uint capacityReserved : 1;
    uint flags : 31;
    // 字符数据紧跟在结构体后面
    // QChar data[alloc];
};

// QString 实现细节
class QString {
private:
    QStringData *d;

public:
    // 获取字符（非 const - 可能触发分离）
    QChar& operator[](int i) {
        Q_ASSERT(i >= 0 && i < size());
        detach();  // 写操作前分离
        return data()[i];
    }

    // 获取字符（const - 不分离）
    QChar operator[](int i) const {
        Q_ASSERT(i >= 0 && i < size());
        return constData()[i];
    }

    // 分离实现
    void detach() {
        if (d->ref.loadRelaxed() != 1 || d->flags & QStringData::Static)
            reallocData();
    }

    // 判断是否共享
    bool isDetached() const {
        return d->ref.loadRelaxed() == 1;
    }
};
```

### 4.3 QList 内部结构

```cpp
// QList 内部数据（Qt 6 简化版）
template <typename T>
struct QListData {
    QAtomicInt ref;
    qsizetype size;
    qsizetype alloc;
    T *begin;
    T *end;
};

// QList 核心操作
template <typename T>
class QList {
    QListData<T> *d;

public:
    // 追加元素
    void append(const T &value) {
        // 需要扩容或共享时
        if (d->ref.loadRelaxed() != 1 || d->size >= d->alloc) {
            detach(d->size + 1);
        }
        new (d->end++) T(value);
        ++d->size;
    }

    // 下标访问
    T& operator[](qsizetype i) {
        Q_ASSERT(i >= 0 && i < d->size);
        detach();
        return d->begin[i];
    }

    // 预分配空间
    void reserve(qsizetype asize) {
        if (asize > d->alloc || d->ref.loadRelaxed() != 1) {
            detach(asize);
        }
    }

    // squeeze：释放多余内存
    void squeeze() {
        if (d->size < d->alloc) {
            reallocData(d->size);
        }
    }
};
```

---

## 5. 插件系统内部

### 5.1 插件加载机制

```cpp
// QPluginLoader 核心实现
class QPluginLoader {
public:
    bool load() {
        // 1. 检查是否已加载
        if (d->instance)
            return true;

        // 2. 加载动态库
        if (!d->library.load())
            return false;

        // 3. 查找元数据符号
        typedef const char* (*QtPluginQueryVerificationData)();
        auto queryFunc = (QtPluginQueryVerificationData)
            d->library.resolve("qt_plugin_query_metadata");

        if (!queryFunc)
            return false;

        // 4. 验证插件兼容性
        const char *metadata = queryFunc();
        if (!verifyPluginMetadata(metadata))
            return false;

        // 5. 创建实例
        typedef QObject* (*QtPluginInstanceFunction)();
        auto instanceFunc = (QtPluginInstanceFunction)
            d->library.resolve("qt_plugin_instance");

        if (!instanceFunc)
            return false;

        d->instance = instanceFunc();
        return d->instance != nullptr;
    }
};

// Q_PLUGIN_METADATA 宏展开
#define Q_PLUGIN_METADATA(x) \
    static constexpr auto qt_meta_stringdata = ...; \
    extern "C" Q_DECL_EXPORT const char *qt_plugin_query_metadata() { \
        return reinterpret_cast<const char*>(&qt_meta_stringdata); \
    } \
    extern "C" Q_DECL_EXPORT QObject *qt_plugin_instance() { \
        static QPointer<QObject> instance; \
        if (!instance) \
            instance = new ThisClass; \
        return instance; \
    }
```

### 5.2 插件元数据

```cpp
// 插件 JSON 元数据格式
/*
{
    "IID": "org.qt-project.Qt.Plugin",
    "className": "MyPlugin",
    "version": "1.0.0",
    "MetaData": {
        "name": "My Plugin",
        "vendor": "My Company"
    }
}
*/

// 读取插件元数据
QPluginLoader loader("myplugin.dll");
QJsonObject metadata = loader.metaData();
QString iid = metadata["IID"].toString();
QJsonObject customData = metadata["MetaData"].toObject();
```

---

## 6. 资源系统内部

### 6.1 资源编译

```cpp
// rcc 生成的代码结构
static const unsigned char qt_resource_data[] = {
    // 压缩的文件数据
    0x00, 0x00, 0x01, 0x23, ...
};

static const unsigned char qt_resource_name[] = {
    // 文件名数据
    0x00, 0x05, 'i', 'm', 'a', 'g', 'e', ...
};

static const unsigned char qt_resource_struct[] = {
    // 目录结构
    0x00, 0x00, 0x00, 0x00, ...
};

// 资源注册
QT_RCC_MANGLE_NAMESPACE(qInitResources_resources)() {
    QResource::registerResource(qt_resource_struct,
                                qt_resource_name,
                                qt_resource_data);
}

// 自动注册
static struct ResourceInitializer {
    ResourceInitializer() {
        QT_RCC_MANGLE_NAMESPACE(qInitResources_resources)();
    }
} resourceInitializer;
```

### 6.2 QResource 实现

```cpp
// QResource 核心实现
class QResourcePrivate {
public:
    QString absoluteFilePath;
    const uchar *data;
    qint64 size;
    bool compressed;

    // 查找资源
    bool load(const QString &path) {
        // 1. 在已注册的资源树中查找
        for (QResourceRoot *root : registeredResources) {
            if (root->contains(path)) {
                data = root->data(path, &size, &compressed);
                return true;
            }
        }
        return false;
    }
};

// 资源访问
QResource resource(":/images/logo.png");
if (resource.isValid()) {
    const uchar *data = resource.data();
    qint64 size = resource.size();

    // 如果压缩，需要解压
    if (resource.isCompressed()) {
        QByteArray uncompressed = qUncompress(data, size);
    }
}
```

---

## 7. 定时器内部实现

### 7.1 定时器注册

```cpp
// startTimer 实现
int QObject::startTimer(int interval, Qt::TimerType timerType) {
    QObjectPrivate *d = d_func();

    // 获取当前线程的事件分发器
    QAbstractEventDispatcher *dispatcher =
        d->threadData->eventDispatcher.loadRelaxed();

    if (!dispatcher) {
        qWarning("Cannot start timer in thread without event loop");
        return 0;
    }

    // 注册定时器
    int id = dispatcher->registerTimer(interval, timerType, this);

    // 记录定时器 ID
    if (!d->extraData)
        d->extraData = new QObjectPrivate::ExtraData;
    d->extraData->runningTimers.append(id);

    return id;
}

// killTimer 实现
void QObject::killTimer(int id) {
    QObjectPrivate *d = d_func();
    QAbstractEventDispatcher *dispatcher =
        d->threadData->eventDispatcher.loadRelaxed();

    if (dispatcher)
        dispatcher->unregisterTimer(id);

    if (d->extraData)
        d->extraData->runningTimers.removeOne(id);
}
```

### 7.2 平台实现差异

```cpp
// Windows 实现
class QWindowsTimerScheduler {
    // 使用 SetTimer/KillTimer
    void registerTimer(int interval, QObject *obj) {
        UINT_PTR id = SetTimer(nullptr, 0, interval, timerCallback);
        timerMap[id] = obj;
    }

    static void CALLBACK timerCallback(HWND, UINT, UINT_PTR id, DWORD) {
        QObject *obj = timerMap[id];
        QTimerEvent e(id);
        QCoreApplication::sendEvent(obj, &e);
    }
};

// Linux 实现
class QUnixTimerScheduler {
    // 使用 timerfd 或 select/poll 超时
    void registerTimer(int interval, QObject *obj) {
        int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
        struct itimerspec ts;
        ts.it_value.tv_sec = interval / 1000;
        ts.it_value.tv_nsec = (interval % 1000) * 1000000;
        timerfd_settime(fd, 0, &ts, nullptr);
        // 添加到事件循环监控
    }
};
```

---

## 8. 属性绑定（Qt 6）

### 8.1 QProperty 内部

```cpp
// QProperty 简化实现
template <typename T>
class QProperty {
    T m_value;
    QPropertyBindingData m_bindingData;

public:
    T value() const {
        // 记录依赖关系（用于自动更新）
        m_bindingData.registerDependency();
        return m_value;
    }

    void setValue(T newValue) {
        if (m_value != newValue) {
            m_value = newValue;
            // 通知所有依赖此属性的绑定
            m_bindingData.notifyObservers();
        }
    }

    // 设置绑定
    QPropertyBinding<T> setBinding(std::function<T()> func) {
        auto binding = QPropertyBinding<T>(func);
        m_bindingData.setBinding(binding);
        // 立即计算一次
        m_value = func();
        return binding;
    }
};
```

### 8.2 绑定求值

```cpp
// 绑定表达式求值
class QPropertyBindingPrivate {
    std::function<QVariant()> m_evaluationFunction;
    QPropertyObserverPointer m_firstObserver;

    void evaluate() {
        // 记录当前正在求值的绑定
        QBindingStatus *status = QBindingStatus::current();
        status->currentlyEvaluatingBinding = this;

        // 清除旧依赖
        clearDependencies();

        // 执行绑定函数（会自动收集依赖）
        QVariant newValue = m_evaluationFunction();

        // 恢复状态
        status->currentlyEvaluatingBinding = nullptr;

        // 更新值并通知观察者
        if (m_value != newValue) {
            m_value = newValue;
            notifyAllObservers();
        }
    }
};
```

---

## 面试常见问题

### Q1: moc 的作用是什么？生成了什么内容？

**答案**：
moc（Meta-Object Compiler）是 Qt 的元对象编译器，处理包含 Q_OBJECT 宏的头文件，生成：
1. 静态元对象（staticMetaObject）包含类信息
2. 字符串表存储类名、方法名、属性名
3. 元数据数组描述信号、槽、属性的结构
4. 信号实现函数（调用 QMetaObject::activate）
5. qt_static_metacall 函数用于反射调用

### Q2: Qt 的信号是如何实现的？

**答案**：
1. 信号被 moc 生成为普通成员函数
2. 函数体调用 `QMetaObject::activate()`
3. activate() 遍历连接列表，根据连接类型：
   - 直接连接：直接调用槽函数
   - 队列连接：创建 QMetaCallEvent 投递到接收者线程
   - 自动连接：判断线程决定使用哪种方式
4. 参数通过 void** 数组传递

### Q3: 什么是隐式共享？如何实现？

**答案**：
隐式共享（写时复制/COW）允许多个对象共享相同数据，只在修改时才复制：
1. 使用引用计数跟踪共享数
2. 拷贝时只增加计数，不复制数据
3. 修改前检查计数，如果 >1 则分离（创建副本）
4. Qt 使用 QSharedData 和 QSharedDataPointer 实现
5. QString、QByteArray、QList 等都使用此机制

### Q4: deleteLater() 是如何工作的？

**答案**：
1. `deleteLater()` 调用 `postEvent()` 投递 QDeferredDeleteEvent
2. 事件被添加到对象所属线程的事件队列
3. 事件循环处理到该事件时调用 `delete this`
4. 好处：
   - 安全地在槽函数中删除发送者
   - 确保删除发生在正确线程
   - 允许当前事件处理完成

### Q5: Qt 插件是如何加载的？

**答案**：
1. QPluginLoader 使用 QLibrary 加载动态库
2. 查找 `qt_plugin_query_metadata` 符号获取元数据
3. 验证插件兼容性（Qt 版本、调试/发布模式等）
4. 查找 `qt_plugin_instance` 符号创建插件实例
5. Q_PLUGIN_METADATA 宏生成这些必要的导出函数

### Q6: QObject::moveToThread() 的限制是什么？

**答案**：
1. 只能从对象当前所属线程调用
2. 有父对象的对象不能单独移动
3. 子对象会随父对象一起移动
4. 移动后定时器会重新注册到新线程
5. 挂起的事件不会自动移动

### Q7: 事件过滤器的执行顺序？

**答案**：
事件分发顺序：
1. 全局事件过滤器（QCoreApplication 上安装的）
2. 对象自身的事件过滤器（后安装的先执行）
3. 对象的 event() 方法
4. 具体事件处理函数（如 keyPressEvent）

任何一层返回 true 都会阻止事件继续传播。

---

## 总结

Qt 内部机制关键点：

| 机制 | 核心实现 |
|------|---------|
| 元对象系统 | moc 生成静态元数据和反射函数 |
| 信号槽 | QMetaObject::activate + 连接列表 |
| 对象树 | 父子关系自动内存管理 |
| 事件系统 | 事件队列 + 分发器 + 过滤器链 |
| 隐式共享 | QSharedData + 引用计数 + 写时复制 |
| 插件系统 | QLibrary + 元数据验证 + 实例工厂 |
| 资源系统 | rcc 编译 + 运行时注册 |
| 定时器 | 平台事件循环集成 |

理解这些内部机制有助于：
1. 编写更高效的代码
2. 正确使用跨线程功能
3. 调试复杂问题
4. 扩展 Qt 功能
