# Qt 元对象系统（Meta-Object System）

## 概述

Qt 元对象系统是 Qt 实现信号槽、属性系统、动态类型信息等功能的基础。它通过 moc（Meta-Object Compiler）在编译时生成额外的 C++ 代码，为 QObject 派生类提供反射能力。

---

## 1. 元对象系统基础

### 1.1 什么是元对象系统？

**面试问题**：什么是 Qt 元对象系统？它提供了哪些功能？

**答案**：

元对象系统是 Qt 对 C++ 的扩展，提供以下核心功能：

1. **信号槽机制** - 对象间松耦合通信
2. **运行时类型信息** - 超越 RTTI 的类型内省
3. **属性系统** - 动态属性访问
4. **国际化支持** - tr() 函数
5. **动态方法调用** - invokeMethod()

```cpp
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>

class Person : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)

public:
    explicit Person(QObject* parent = nullptr) : QObject(parent) {}

    QString name() const { return m_name; }
    void setName(const QString& name) {
        if (m_name != name) {
            m_name = name;
            emit nameChanged(m_name);
        }
    }

    int age() const { return m_age; }
    void setAge(int age) {
        if (m_age != age) {
            m_age = age;
            emit ageChanged(m_age);
        }
    }

signals:
    void nameChanged(const QString& name);
    void ageChanged(int age);

public slots:
    void introduce() {
        qDebug() << "Hi, I'm" << m_name << ", age" << m_age;
    }

private:
    QString m_name;
    int m_age = 0;
};
```

**元对象系统的三大支柱**：

| 组件 | 作用 |
|------|------|
| Q_OBJECT 宏 | 声明类使用元对象系统 |
| moc 工具 | 生成元对象代码 |
| QMetaObject 类 | 运行时访问元信息 |

---

### 1.2 moc 工具

**面试问题**：moc 是什么？它做了什么工作？

**答案**：

moc（Meta-Object Compiler）是 Qt 的代码生成工具：

```cpp
// 源文件：person.h
class Person : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QString name() const;
    void setName(const QString& name);

signals:
    void nameChanged();

public slots:
    void greet();
};

// moc 生成的文件：moc_person.cpp（简化版）
// 包含以下内容：

// 1. 静态元对象
const QMetaObject Person::staticMetaObject = {
    { &QObject::staticMetaObject,       // 父类元对象
      qt_meta_stringdata_Person.data,   // 字符串数据
      qt_meta_data_Person,              // 元数据数组
      qt_static_metacall,               // 静态元调用函数
      nullptr, nullptr }
};

// 2. 信号实现
void Person::nameChanged() {
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// 3. 元调用函数（处理槽调用）
void Person::qt_static_metacall(QObject* _o, QMetaObject::Call _c,
                                 int _id, void** _a) {
    if (_c == QMetaObject::InvokeMetaMethod) {
        Person* _t = static_cast<Person*>(_o);
        switch (_id) {
        case 0: _t->nameChanged(); break;  // 信号
        case 1: _t->greet(); break;        // 槽
        }
    }
    // ... 属性访问等
}
```

**moc 处理的内容**：

| 宏/关键字 | moc 生成的代码 |
|-----------|----------------|
| Q_OBJECT | staticMetaObject、metaObject() |
| signals | 信号函数实现 |
| slots | 元调用注册 |
| Q_PROPERTY | 属性访问代码 |
| Q_ENUM | 枚举元信息 |
| Q_INVOKABLE | 可调用方法注册 |
| Q_CLASSINFO | 类信息键值对 |

---

### 1.3 QMetaObject 类

**面试问题**：如何使用 QMetaObject 获取类的元信息？

**答案**：

```cpp
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QDebug>

void metaObjectDemo(QObject* obj) {
    const QMetaObject* meta = obj->metaObject();

    // 1. 获取类名
    qDebug() << "Class:" << meta->className();

    // 2. 获取父类
    const QMetaObject* super = meta->superClass();
    if (super) {
        qDebug() << "Parent class:" << super->className();
    }

    // 3. 遍历继承链
    qDebug() << "\nInheritance chain:";
    const QMetaObject* current = meta;
    while (current) {
        qDebug() << "  -" << current->className();
        current = current->superClass();
    }

    // 4. 获取属性信息
    qDebug() << "\nProperties:";
    for (int i = meta->propertyOffset(); i < meta->propertyCount(); ++i) {
        QMetaProperty prop = meta->property(i);
        qDebug() << "  " << prop.name() << ":" << prop.typeName();
    }

    // 5. 获取方法信息
    qDebug() << "\nMethods:";
    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
        QMetaMethod method = meta->method(i);
        qDebug() << "  " << method.methodSignature()
                 << "- type:" << method.methodType();
    }
}

// 使用示例
void demoUsage() {
    Person person;
    person.setObjectName("TestPerson");
    person.setName("Alice");
    person.setAge(25);

    metaObjectDemo(&person);
}

// 输出示例：
// Class: Person
// Parent class: QObject
//
// Inheritance chain:
//   - Person
//   - QObject
//
// Properties:
//   name : QString
//   age : int
//
// Methods:
//   nameChanged(QString) - type: Signal
//   ageChanged(int) - type: Signal
//   introduce() - type: Slot
```

---

## 2. 属性系统

### 2.1 Q_PROPERTY 宏

**面试问题**：Q_PROPERTY 宏的各个参数是什么意思？

**答案**：

```cpp
class Widget : public QWidget {
    Q_OBJECT

    // 完整的 Q_PROPERTY 声明
    Q_PROPERTY(
        QString title           // 属性名和类型
        READ title              // getter 函数
        WRITE setTitle          // setter 函数（可选）
        RESET resetTitle        // 重置函数（可选）
        NOTIFY titleChanged     // 通知信号（可选，但推荐）
        REVISION 1              // 版本号（可选）
        DESIGNABLE true         // 是否在设计器中可见
        SCRIPTABLE true         // 是否可被脚本访问
        STORED true             // 是否持久化
        USER false              // 是否是用户可编辑属性
        CONSTANT                // 是否是常量（与 WRITE/NOTIFY 互斥）
        FINAL                   // 是否不能被覆盖
    )

public:
    QString title() const { return m_title; }

    void setTitle(const QString& title) {
        if (m_title != title) {
            m_title = title;
            emit titleChanged();
        }
    }

    void resetTitle() {
        setTitle(QString());
    }

signals:
    void titleChanged();

private:
    QString m_title;
};

// 常见的简化形式
class SimpleExample : public QObject {
    Q_OBJECT

    // 最简形式：只读属性
    Q_PROPERTY(int count READ count CONSTANT)

    // 标准形式：读写 + 通知
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    // MEMBER 简化（Qt 5.1+）
    Q_PROPERTY(int value MEMBER m_value NOTIFY valueChanged)

public:
    int count() const { return 42; }

    QString name() const { return m_name; }
    void setName(const QString& n) {
        if (m_name != n) {
            m_name = n;
            emit nameChanged();
        }
    }

signals:
    void nameChanged();
    void valueChanged();

private:
    QString m_name;
    int m_value = 0;  // MEMBER 直接绑定成员变量
};
```

---

### 2.2 动态属性

**面试问题**：什么是 Qt 的动态属性？如何使用它？

**答案**：

动态属性是在运行时添加到对象的属性，不需要在类定义中声明：

```cpp
#include <QObject>
#include <QVariant>
#include <QDebug>

void dynamicPropertyDemo() {
    QObject obj;

    // 设置动态属性
    obj.setProperty("customId", 12345);
    obj.setProperty("category", "important");
    obj.setProperty("metadata", QVariantMap{{"key", "value"}});

    // 读取动态属性
    int id = obj.property("customId").toInt();
    QString cat = obj.property("category").toString();

    qDebug() << "ID:" << id;          // 12345
    qDebug() << "Category:" << cat;   // "important"

    // 检查属性是否存在
    QVariant val = obj.property("nonexistent");
    if (!val.isValid()) {
        qDebug() << "Property does not exist";
    }

    // 删除动态属性
    obj.setProperty("customId", QVariant());  // 设为无效 QVariant

    // 获取所有动态属性名
    QList<QByteArray> dynProps = obj.dynamicPropertyNames();
    qDebug() << "Dynamic properties:" << dynProps;
}

// 实用场景：样式表中使用动态属性
void styleSheetDemo(QWidget* widget) {
    // 设置动态属性
    widget->setProperty("state", "error");

    // 在 QSS 中使用
    // QWidget[state="error"] { border: 2px solid red; }

    // 更新属性后需要刷新样式
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

// 实用场景：存储额外数据
class DataAttacher {
public:
    static void attachData(QObject* obj, const QString& key, const QVariant& data) {
        obj->setProperty(("_data_" + key).toUtf8().constData(), data);
    }

    static QVariant getData(QObject* obj, const QString& key) {
        return obj->property(("_data_" + key).toUtf8().constData());
    }
};
```

---

### 2.3 属性绑定（Qt 6）

**面试问题**：Qt 6 的属性绑定是什么？

**答案**：

Qt 6 引入了 QProperty 和属性绑定系统：

```cpp
#include <QObject>
#include <QProperty>
#include <QObjectBindableProperty>

// Qt 6 属性绑定
class Rectangle : public QObject {
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged BINDABLE bindableWidth)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged BINDABLE bindableHeight)
    Q_PROPERTY(int area READ area NOTIFY areaChanged BINDABLE bindableArea)

public:
    int width() const { return m_width; }
    void setWidth(int w) { m_width = w; }
    QBindable<int> bindableWidth() { return &m_width; }

    int height() const { return m_height; }
    void setHeight(int h) { m_height = h; }
    QBindable<int> bindableHeight() { return &m_height; }

    int area() const { return m_area; }
    QBindable<int> bindableArea() { return &m_area; }

signals:
    void widthChanged();
    void heightChanged();
    void areaChanged();

private:
    Q_OBJECT_BINDABLE_PROPERTY(Rectangle, int, m_width, &Rectangle::widthChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Rectangle, int, m_height, &Rectangle::heightChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(
        Rectangle, int, m_area,
        [this]() { return m_width * m_height; },  // 绑定表达式
        &Rectangle::areaChanged
    )
};

void bindingDemo() {
    Rectangle rect;

    // 设置宽高
    rect.setWidth(10);
    rect.setHeight(5);

    // area 自动计算为 50
    qDebug() << "Area:" << rect.area();  // 50

    // 修改宽度，area 自动更新
    rect.setWidth(20);
    qDebug() << "Area:" << rect.area();  // 100

    // 创建自定义绑定
    QProperty<int> doubleArea;
    doubleArea.setBinding([&]() { return rect.area() * 2; });

    qDebug() << "Double area:" << doubleArea.value();  // 200
}
```

---

## 3. 运行时类型信息

### 3.1 类型检查与转换

**面试问题**：Qt 提供了哪些运行时类型检查方法？

**答案**：

```cpp
#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QMetaType>
#include <QDebug>

void typeCheckingDemo() {
    QObject* obj = new QPushButton("Click");

    // 1. inherits() - 检查继承关系
    qDebug() << "inherits QWidget:" << obj->inherits("QWidget");      // true
    qDebug() << "inherits QPushButton:" << obj->inherits("QPushButton"); // true
    qDebug() << "inherits QLabel:" << obj->inherits("QLabel");        // false

    // 2. metaObject()->className() - 获取精确类名
    qDebug() << "Exact class:" << obj->metaObject()->className();     // QPushButton

    // 3. qobject_cast - 安全类型转换
    if (QPushButton* btn = qobject_cast<QPushButton*>(obj)) {
        qDebug() << "It's a button:" << btn->text();
    }

    // 4. QMetaType - 类型元信息
    int typeId = QMetaType::type("QString");
    qDebug() << "QString type ID:" << typeId;
    qDebug() << "Type name:" << QMetaType::typeName(typeId);

    delete obj;
}

// 自定义类型注册
class CustomData {
public:
    int value = 0;
    QString name;
};
Q_DECLARE_METATYPE(CustomData)

void customTypeDemo() {
    // 注册类型（用于信号槽队列连接）
    qRegisterMetaType<CustomData>("CustomData");

    // 使用 QVariant 存储
    CustomData data{42, "test"};
    QVariant var = QVariant::fromValue(data);

    // 取出数据
    CustomData retrieved = var.value<CustomData>();
    qDebug() << "Value:" << retrieved.value;
}
```

---

### 3.2 Q_ENUM 和 Q_FLAG

**面试问题**：Q_ENUM 和 Q_FLAG 有什么作用？

**答案**：

```cpp
class Device : public QObject {
    Q_OBJECT

public:
    // Q_ENUM：使枚举可被元对象系统识别
    enum Status {
        Offline,
        Connecting,
        Online,
        Error
    };
    Q_ENUM(Status)

    // Q_FLAG：用于可组合的标志
    enum Permission {
        None = 0x00,
        Read = 0x01,
        Write = 0x02,
        Execute = 0x04,
        All = Read | Write | Execute
    };
    Q_DECLARE_FLAGS(Permissions, Permission)
    Q_FLAG(Permissions)

    Status status() const { return m_status; }
    Permissions permissions() const { return m_permissions; }

private:
    Status m_status = Offline;
    Permissions m_permissions = None;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Device::Permissions)

void enumDemo() {
    Device device;

    // 枚举转字符串
    const QMetaObject* meta = device.metaObject();
    int enumIndex = meta->indexOfEnumerator("Status");
    QMetaEnum metaEnum = meta->enumerator(enumIndex);

    qDebug() << "Enum name:" << metaEnum.name();
    qDebug() << "Key count:" << metaEnum.keyCount();

    // 值转字符串
    const char* statusStr = metaEnum.valueToKey(Device::Online);
    qDebug() << "Status string:" << statusStr;  // "Online"

    // 字符串转值
    int value = metaEnum.keyToValue("Error");
    qDebug() << "Error value:" << value;  // 3

    // 遍历所有枚举值
    qDebug() << "\nAll status values:";
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        qDebug() << "  " << metaEnum.key(i) << "=" << metaEnum.value(i);
    }

    // 标志位操作
    Device::Permissions perms = Device::Read | Device::Write;
    int flagIndex = meta->indexOfEnumerator("Permissions");
    QMetaEnum flagEnum = meta->enumerator(flagIndex);

    // 标志转字符串（自动组合）
    QByteArray flagStr = flagEnum.valueToKeys(perms);
    qDebug() << "Permissions:" << flagStr;  // "Read|Write"
}
```

---

### 3.3 Q_CLASSINFO

**面试问题**：Q_CLASSINFO 有什么用途？

**答案**：

```cpp
class Plugin : public QObject {
    Q_OBJECT
    Q_CLASSINFO("Author", "John Doe")
    Q_CLASSINFO("Version", "1.0.0")
    Q_CLASSINFO("Description", "A sample plugin")
    Q_CLASSINFO("License", "MIT")

public:
    explicit Plugin(QObject* parent = nullptr) : QObject(parent) {}
};

void classInfoDemo() {
    Plugin plugin;
    const QMetaObject* meta = plugin.metaObject();

    // 获取类信息数量
    int count = meta->classInfoCount();
    qDebug() << "Class info count:" << count;

    // 遍历所有类信息
    for (int i = 0; i < count; ++i) {
        QMetaClassInfo info = meta->classInfo(i);
        qDebug() << info.name() << ":" << info.value();
    }

    // 按名称查找
    int index = meta->indexOfClassInfo("Version");
    if (index >= 0) {
        qDebug() << "Version:" << meta->classInfo(index).value();
    }
}

// 实用场景：插件系统
class PluginLoader {
public:
    static QString getPluginInfo(QObject* plugin, const char* key) {
        const QMetaObject* meta = plugin->metaObject();
        int index = meta->indexOfClassInfo(key);
        if (index >= 0) {
            return QString::fromUtf8(meta->classInfo(index).value());
        }
        return QString();
    }

    static void printPluginDetails(QObject* plugin) {
        qDebug() << "Author:" << getPluginInfo(plugin, "Author");
        qDebug() << "Version:" << getPluginInfo(plugin, "Version");
        qDebug() << "Description:" << getPluginInfo(plugin, "Description");
    }
};
```

---

## 4. 动态方法调用

### 4.1 QMetaObject::invokeMethod

**面试问题**：如何动态调用 QObject 的方法？

**答案**：

```cpp
#include <QMetaObject>
#include <QDebug>

class Calculator : public QObject {
    Q_OBJECT

public:
    explicit Calculator(QObject* parent = nullptr) : QObject(parent) {}

    // Q_INVOKABLE 使方法可被元对象系统调用
    Q_INVOKABLE int add(int a, int b) {
        qDebug() << "add called in thread:" << QThread::currentThread();
        return a + b;
    }

    Q_INVOKABLE QString format(const QString& prefix, int value) {
        return QString("%1: %2").arg(prefix).arg(value);
    }

public slots:
    void processData(const QByteArray& data) {
        qDebug() << "Processing:" << data;
    }

    int multiply(int a, int b) {
        return a * b;
    }
};

void invokeMethodDemo() {
    Calculator calc;

    // 方式 1：无返回值调用
    QMetaObject::invokeMethod(&calc, "processData",
                              Q_ARG(QByteArray, "Hello"));

    // 方式 2：获取返回值（Qt 5 风格）
    int result = 0;
    QMetaObject::invokeMethod(&calc, "add",
                              Q_RETURN_ARG(int, result),
                              Q_ARG(int, 10),
                              Q_ARG(int, 20));
    qDebug() << "Add result:" << result;  // 30

    // 方式 3：Qt 6 简化语法
    // auto result = QMetaObject::invokeMethod(&calc, "add", 10, 20);

    // 方式 4：指定连接类型
    QMetaObject::invokeMethod(&calc, "processData",
                              Qt::QueuedConnection,
                              Q_ARG(QByteArray, "Queued call"));

    // 方式 5：Lambda 调用（Qt 5.10+）
    QMetaObject::invokeMethod(&calc, []() {
        qDebug() << "Lambda executed";
    });
}

// 跨线程调用
void crossThreadInvoke(Calculator* calc) {
    // 在工作线程中调用主线程对象的方法

    // 阻塞等待结果
    int result = 0;
    QMetaObject::invokeMethod(calc, "multiply",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, result),
                              Q_ARG(int, 5),
                              Q_ARG(int, 6));
    qDebug() << "Result:" << result;  // 30

    // 非阻塞调用（无法获取返回值）
    QMetaObject::invokeMethod(calc, "processData",
                              Qt::QueuedConnection,
                              Q_ARG(QByteArray, "async data"));
}
```

**连接类型对比**：

| 类型 | 行为 | 返回值 | 线程安全 |
|------|------|--------|----------|
| DirectConnection | 立即同步调用 | 可获取 | 需手动保证 |
| QueuedConnection | 投递到事件队列 | 不可获取 | 线程安全 |
| BlockingQueuedConnection | 阻塞等待执行 | 可获取 | 线程安全 |
| AutoConnection | 自动选择 | 取决于实际类型 | 自动处理 |

---

### 4.2 QMetaMethod

**面试问题**：如何获取和使用方法的元信息？

**答案**：

```cpp
#include <QMetaMethod>

void metaMethodDemo(QObject* obj) {
    const QMetaObject* meta = obj->metaObject();

    qDebug() << "=== Methods of" << meta->className() << "===";

    // 遍历方法（跳过继承的方法）
    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
        QMetaMethod method = meta->method(i);

        qDebug() << "\nMethod:" << method.methodSignature();
        qDebug() << "  Name:" << method.name();
        qDebug() << "  Return type:" << method.typeName();

        // 方法类型
        switch (method.methodType()) {
        case QMetaMethod::Signal:
            qDebug() << "  Type: Signal";
            break;
        case QMetaMethod::Slot:
            qDebug() << "  Type: Slot";
            break;
        case QMetaMethod::Method:
            qDebug() << "  Type: Q_INVOKABLE";
            break;
        case QMetaMethod::Constructor:
            qDebug() << "  Type: Constructor";
            break;
        }

        // 参数信息
        qDebug() << "  Parameters:";
        for (int j = 0; j < method.parameterCount(); ++j) {
            qDebug() << "    " << method.parameterTypes().at(j)
                     << method.parameterNames().at(j);
        }
    }
}

// 通过方法名查找并调用
void findAndInvokeMethod(QObject* obj, const char* methodName) {
    const QMetaObject* meta = obj->metaObject();

    // 查找方法
    int index = meta->indexOfMethod(methodName);
    if (index < 0) {
        // 尝试添加括号
        index = meta->indexOfMethod(QByteArray(methodName) + "()");
    }

    if (index >= 0) {
        QMetaMethod method = meta->method(index);
        qDebug() << "Found method:" << method.methodSignature();

        // 调用方法
        method.invoke(obj);
    } else {
        qDebug() << "Method not found:" << methodName;
    }
}
```

---

## 5. 元对象系统的实现原理

### 5.1 staticMetaObject 结构

**面试问题**：描述 QMetaObject 的内部结构。

**答案**：

```cpp
// QMetaObject 的核心结构（简化）
struct QMetaObject {
    // 父类元对象
    const QMetaObject* superdata;

    // 字符串表（类名、方法名、属性名等）
    const QByteArrayData* stringdata;

    // 元数据数组（方法、属性、枚举的索引和信息）
    const uint* data;

    // 静态元调用函数指针
    void (*static_metacall)(QObject*, QMetaObject::Call, int, void**);

    // 关联的元对象和属性
    const QMetaObject* const* relatedMetaObjects;
    void* extradata;
};

// 元数据数组的结构
// data 数组包含以下信息（以索引形式）：
// - 修订版本号
// - 类名在字符串表的索引
// - 类信息数量和偏移
// - 方法数量和偏移
// - 属性数量和偏移
// - 枚举数量和偏移
// - 构造函数数量和偏移
// - 标志位
// - 信号数量
// - 每个方法的：名称索引、参数数量、参数索引、标签、标志
// - 每个属性的：名称索引、类型、标志

// 示例：手动解析元数据
void parseMetaData(const QMetaObject* meta) {
    const uint* data = meta->d.data;

    int revision = data[0];
    int classNameIndex = data[1];
    int classInfoCount = data[2];
    int classInfoOffset = data[3];
    int methodCount = data[4];
    int methodOffset = data[5];
    int propertyCount = data[6];
    int propertyOffset = data[7];

    qDebug() << "Revision:" << revision;
    qDebug() << "Method count:" << methodCount;
    qDebug() << "Property count:" << propertyCount;
}
```

---

### 5.2 信号的实现机制

**面试问题**：Qt 信号是如何实现的？

**答案**：

```cpp
// 原始类定义
class Sender : public QObject {
    Q_OBJECT
signals:
    void valueChanged(int newValue);
};

// moc 生成的信号实现
void Sender::valueChanged(int newValue) {
    void* args[] = {
        nullptr,                          // 返回值位置
        const_cast<void*>(reinterpret_cast<const void*>(&newValue))
    };

    // 调用元对象系统发射信号
    QMetaObject::activate(
        this,                   // 发送者
        &staticMetaObject,      // 元对象
        0,                      // 信号索引
        args                    // 参数数组
    );
}

// QMetaObject::activate 的工作流程（简化）
void QMetaObject::activate(QObject* sender, const QMetaObject* meta,
                           int signalIndex, void** args) {
    // 1. 获取连接列表
    QObjectPrivate* senderPrivate = QObjectPrivate::get(sender);
    ConnectionList& connections = senderPrivate->connections[signalIndex];

    // 2. 遍历所有连接
    for (Connection& conn : connections) {
        QObject* receiver = conn.receiver;
        int methodIndex = conn.method;

        // 3. 根据连接类型执行
        switch (conn.connectionType) {
        case Qt::DirectConnection:
            // 直接调用接收者的方法
            receiver->qt_metacall(QMetaObject::InvokeMetaMethod,
                                  methodIndex, args);
            break;

        case Qt::QueuedConnection:
            // 将调用打包并投递到接收者的事件队列
            QCoreApplication::postEvent(receiver,
                new QMetaCallEvent(methodIndex, args));
            break;

        case Qt::BlockingQueuedConnection:
            // 投递事件并等待完成
            QSemaphore semaphore;
            QCoreApplication::postEvent(receiver,
                new QMetaCallEvent(methodIndex, args, &semaphore));
            semaphore.acquire();
            break;
        }
    }
}
```

---

## 6. 常见面试题

### Q1: moc 必须处理哪些情况？没有 moc 会怎样？

**答案**：

```cpp
// 必须使用 moc 的情况：
class MyClass : public QObject {
    Q_OBJECT                    // 1. 使用 Q_OBJECT 宏
    Q_PROPERTY(...)            // 2. 声明属性
    Q_ENUM(...)                // 3. 注册枚举
    Q_CLASSINFO(...)           // 4. 类信息

signals:                        // 5. 声明信号
    void mySignal();

public slots:                   // 6. 声明槽（旧语法需要 slots 关键字）
    void mySlot();

    Q_INVOKABLE void myMethod(); // 7. 可调用方法
};

// 没有 moc 的后果：
// - Q_OBJECT 宏定义的函数没有实现 → 链接错误
// - 信号没有实现 → 链接错误
// - 信号槽连接不工作
// - 属性系统不可用
// - 动态方法调用失败
```

---

### Q2: Q_PROPERTY 中 NOTIFY 为什么重要？

**答案**：

```cpp
class Model : public QObject {
    Q_OBJECT

    // 没有 NOTIFY 的属性
    Q_PROPERTY(int badValue READ badValue WRITE setBadValue)

    // 有 NOTIFY 的属性
    Q_PROPERTY(int goodValue READ goodValue WRITE setGoodValue NOTIFY goodValueChanged)

public:
    int badValue() const { return m_badValue; }
    void setBadValue(int v) { m_badValue = v; }

    int goodValue() const { return m_goodValue; }
    void setGoodValue(int v) {
        if (m_goodValue != v) {
            m_goodValue = v;
            emit goodValueChanged();
        }
    }

signals:
    void goodValueChanged();

private:
    int m_badValue = 0;
    int m_goodValue = 0;
};

// NOTIFY 的重要性：
// 1. QML 绑定需要 NOTIFY 才能自动更新
// 2. Qt Quick Controls 依赖属性通知
// 3. 属性绑定系统需要通知
// 4. Model/View 框架使用通知更新视图

// QML 示例：
// Text {
//     text: model.goodValue  // goodValue 变化时自动更新
//     // text: model.badValue  // badValue 变化时不会更新！
// }
```

---

### Q3: 如何在不使用 Q_INVOKABLE 的情况下动态调用方法？

**答案**：

```cpp
class MyClass : public QObject {
    Q_OBJECT

public:
    // 普通方法（无法通过 invokeMethod 调用）
    void normalMethod() {
        qDebug() << "Normal method";
    }

public slots:
    // 槽函数可以被动态调用
    void slotMethod() {
        qDebug() << "Slot method";
    }
};

void dynamicCallDemo() {
    MyClass obj;

    // 槽函数可以直接调用
    QMetaObject::invokeMethod(&obj, "slotMethod");  // 成功

    // 普通方法无法调用
    // QMetaObject::invokeMethod(&obj, "normalMethod");  // 失败

    // 解决方案 1：将方法声明为 Q_INVOKABLE
    // 解决方案 2：将方法声明为槽
    // 解决方案 3：手动查找并调用

    // 方案 3 的实现
    const QMetaObject* meta = obj.metaObject();
    for (int i = 0; i < meta->methodCount(); ++i) {
        QMetaMethod method = meta->method(i);
        if (method.name() == "slotMethod") {
            method.invoke(&obj);
            break;
        }
    }
}
```

---

### Q4: 元对象系统对性能的影响？

**答案**：

```cpp
// 性能考虑

// 1. 编译时间
// - moc 增加编译步骤
// - 生成额外代码增加编译时间
// - 大型项目影响明显

// 2. 可执行文件大小
// - 每个 Q_OBJECT 类增加元数据
// - 字符串表存储方法名、属性名
// - 典型增加几KB到几十KB

// 3. 运行时开销
class PerformanceTest : public QObject {
    Q_OBJECT
public:
    void directCall() { /* 直接调用 */ }

public slots:
    void slotCall() { /* 通过槽调用 */ }
};

void performanceDemo() {
    PerformanceTest obj;

    // 直接调用：最快（内联可能）
    obj.directCall();

    // 信号槽连接调用：有额外开销
    // - 查找连接列表
    // - 参数打包
    // - 虚函数调用
    emit someSignal();  // 约 10-100 纳秒的额外开销

    // QMetaObject::invokeMethod：最慢
    // - 字符串查找方法
    // - 参数类型转换
    // - 间接调用
    QMetaObject::invokeMethod(&obj, "slotCall");
}

// 优化建议：
// - 高频调用使用直接方法调用
// - 信号槽用于事件驱动场景
// - 避免在性能关键路径使用 invokeMethod
```

---

## 总结

Qt 元对象系统的核心要点：

1. **moc 工具** - 编译时生成元对象代码
2. **Q_OBJECT 宏** - 启用元对象功能的关键
3. **QMetaObject** - 运行时类型信息的入口
4. **属性系统** - Q_PROPERTY 提供动态属性访问
5. **动态调用** - invokeMethod 实现运行时方法调用
6. **类型注册** - Q_ENUM、Q_FLAG、Q_DECLARE_METATYPE

理解元对象系统是深入理解 Qt 框架的基础，它使 C++ 具备了类似动态语言的反射能力。
