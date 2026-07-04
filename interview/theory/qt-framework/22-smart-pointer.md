# Qt 智能指针

> 深入浅出考察 Qt 智能指针的实现原理、使用场景与最佳实践

---

## 1. 智能指针概述

### 1.1 为什么 Qt 需要自己的智能指针？

**面试问题**：Qt 为什么不直接使用 C++ 标准智能指针？

**答案**：

```cpp
// 历史原因：Qt 的智能指针早于 C++11
// QPointer - Qt 2.x (1999)
// QSharedPointer - Qt 4.5 (2009)
// std::shared_ptr - C++11 (2011)

// 技术原因：Qt 对象模型的特殊性
class QtObjectModelIssues {
public:
    void demonstrate() {
        // 问题 1：QObject 有父子树，父对象会 delete 子对象
        QWidget* parent = new QWidget();
        QWidget* child = new QWidget(parent);

        // ❌ 危险：double delete
        // std::unique_ptr<QWidget> ptr(child);
        // delete parent;  // parent 会 delete child
        // ptr 析构时再次 delete child -> crash

        // 问题 2：QObject 不可拷贝
        // std::shared_ptr 要求类型可拷贝（用于 weak_ptr 等场景）
        // QObject 禁用了拷贝构造和赋值

        // 问题 3：跨 DLL 边界
        // Qt 模块间传递对象时，需要确保使用相同的内存分配器
        // Qt 智能指针与 Qt 内存管理集成更好
    }
};

// Qt 智能指针体系
/*
┌─────────────────────────────────────────────────────────────┐
│                    Qt 智能指针                               │
├─────────────────┬─────────────────┬─────────────────────────┤
│   QPointer      │ QScopedPointer  │ QSharedPointer          │
│   (弱引用)       │ (独占所有权)     │ (共享所有权)             │
│                 │                 │                         │
│ 只能用于 QObject │ 类似 unique_ptr │ 类似 shared_ptr         │
│ 对象删除后自动   │ 不可拷贝        │ 引用计数                 │
│ 变为 nullptr    │ 作用域结束释放   │ + QWeakPointer          │
└─────────────────┴─────────────────┴─────────────────────────┘
*/
```

---

### 1.2 Qt 智能指针 vs C++ 标准智能指针

| 特性 | Qt 智能指针 | C++ 标准智能指针 |
|------|-------------|------------------|
| 历史 | Qt 2.x ~ 4.x | C++11 (2011) |
| QObject 支持 | QPointer 专门支持 | 不感知 QObject 生命周期 |
| 父子树兼容 | QPointer 兼容 | 需要小心使用 |
| 跨模块 | Qt 模块间安全 | 需要相同分配器 |
| 性能 | 原子引用计数 | 原子引用计数 |
| 互操作 | Qt 6 支持互转 | - |

**选型建议**：

```cpp
// 1. 管理 QObject 派生类 -> QPointer（观察）或父子树（所有权）
QPointer<QWidget> observed = someWidget;  // 只观察，不拥有

// 2. 管理非 QObject 对象 -> std::unique_ptr / std::shared_ptr
std::unique_ptr<MyData> data = std::make_unique<MyData>();

// 3. 跨线程共享数据 -> QSharedPointer（原子引用计数）
QSharedPointer<LargeData> shared = QSharedPointer<LargeData>::create();

// 4. 需要与旧 Qt 代码兼容 -> Qt 智能指针
// 5. Qt 6 新项目 -> 优先考虑标准智能指针
```

---

## 2. QPointer 深度剖析

### 2.1 QPointer 是什么？

**面试问题**：QPointer 和普通指针、std::weak_ptr 有什么区别？

**答案**：

```cpp
#include <QPointer>
#include <QWidget>

// QPointer 是专门为 QObject 设计的"守护指针"（guarded pointer）
// 当指向的 QObject 被删除时，QPointer 自动变为 nullptr

void qpointerBasics() {
    QWidget* widget = new QWidget();
    QPointer<QWidget> guarded = widget;

    qDebug() << guarded.isNull();  // false
    qDebug() << (guarded == widget);  // true

    delete widget;

    // 关键特性：自动感知对象删除
    qDebug() << guarded.isNull();  // true
    qDebug() << (guarded == nullptr);  // true

    // 安全访问
    if (guarded) {
        guarded->show();  // 不会执行
    }
}

// 对比普通指针
void rawPointerDanger() {
    QWidget* widget = new QWidget();
    QWidget* raw = widget;

    delete widget;

    // raw 仍然指向已释放的内存（悬垂指针）
    // raw->show();  // 未定义行为！可能 crash
}

// 对比 std::weak_ptr
void weakPtrComparison() {
    // std::weak_ptr 需要配合 std::shared_ptr 使用
    // 不能直接用于 QObject（QObject 不可拷贝）

    // QPointer 可以直接指向任何 QObject
    // 不需要对象被 shared_ptr 管理
}
```

---

### 2.2 QPointer 实现原理

**面试问题**：QPointer 是如何知道对象被删除的？

**答案**：

```cpp
// QPointer 的核心原理：连接到 QObject::destroyed 信号

// 简化版实现（展示原理）
template <typename T>
class QPointerSimplified {
    // 实际指向的对象
    T* m_ptr = nullptr;

public:
    QPointerSimplified(T* ptr = nullptr) : m_ptr(ptr) {
        if (m_ptr) {
            // 关键：连接 destroyed 信号
            QObject::connect(m_ptr, &QObject::destroyed,
                             [this]() { m_ptr = nullptr; });
        }
    }

    // 当对象被删除时，destroyed 信号触发，m_ptr 被置为 nullptr
};

// 实际 Qt 实现更复杂，使用 QObjectPrivate 中的内部数据结构
// 而不是信号槽连接（性能更好）

// Qt 实际实现（简化版）
/*
class QPointerBase {
protected:
    // 使用 QWeakPointer<QObjectData> 存储
    // QObjectData 是 QObject 的 d_ptr 指向的数据
    QWeakPointer<QObjectData> wp;
};

当 QObject 被删除时：
1. ~QObject() 执行
2. 所有指向该对象的 QPointer 的 wp 自动失效
3. QPointer::isNull() 返回 true
*/

// 验证 destroyed 信号时机
void destroyedSignalTiming() {
    QWidget* widget = new QWidget();
    QPointer<QWidget> guarded = widget;

    QObject::connect(widget, &QObject::destroyed, []() {
        qDebug() << "destroyed signal emitted";
    });

    delete widget;
    // 输出：destroyed signal emitted
    // 此时 guarded 已经是 nullptr
}
```

---

### 2.3 QPointer 使用场景

**面试问题**：什么时候应该使用 QPointer？

**答案**：

```cpp
// 场景 1：槽函数中访问可能已删除的对象
class SafeSlotHandler : public QObject {
    Q_OBJECT
    QPointer<QWidget> m_targetWidget;

public:
    void setTarget(QWidget* widget) {
        m_targetWidget = widget;
    }

public slots:
    void onTimeout() {
        // m_targetWidget 可能已被其他代码删除
        if (m_targetWidget) {
            m_targetWidget->update();
        } else {
            qDebug() << "Target widget was deleted";
        }
    }
};

// 场景 2：Lambda 捕获 this 指针
class MyWidget : public QWidget {
    Q_OBJECT

public:
    void setupAsyncOperation() {
        // ❌ 危险：this 可能在回调前被删除
        // QTimer::singleShot(5000, [this]() {
        //     this->doSomething();
        // });

        // ✅ 安全：使用 QPointer
        QPointer<MyWidget> self = this;
        QTimer::singleShot(5000, [self]() {
            if (self) {
                self->doSomething();
            }
        });
    }

    void doSomething() { /* ... */ }
};

// 场景 3：缓存/弱引用场景
class WidgetCache {
    QHash<QString, QPointer<QWidget>> m_cache;

public:
    void cacheWidget(const QString& key, QWidget* widget) {
        m_cache[key] = widget;
    }

    QWidget* getWidget(const QString& key) {
        // 自动处理已删除的 widget
        QPointer<QWidget>& ptr = m_cache[key];
        return ptr.data();  // 如果已删除返回 nullptr
    }

    void cleanup() {
        // 移除所有已删除的条目
        for (auto it = m_cache.begin(); it != m_cache.end(); ) {
            if (it.value().isNull()) {
                it = m_cache.erase(it);
            } else {
                ++it;
            }
        }
    }
};

// 场景 4：跨对象引用（避免循环引用）
class Observer : public QObject {
    Q_OBJECT
    QPointer<QObject> m_subject;  // 不拥有 subject

public:
    void observe(QObject* subject) {
        if (m_subject) {
            disconnect(m_subject, nullptr, this, nullptr);
        }
        m_subject = subject;
        if (m_subject) {
            connect(m_subject, &QObject::destroyed,
                    this, &Observer::onSubjectDestroyed);
        }
    }

private slots:
    void onSubjectDestroyed() {
        qDebug() << "Subject was destroyed";
        // m_subject 已经自动变为 nullptr
    }
};
```

---

### 2.4 QPointer 的限制

```cpp
// 限制 1：只能用于 QObject 派生类
class NotQObject {};
// QPointer<NotQObject> ptr;  // 编译错误

// 限制 2：不提供所有权管理
void noOwnership() {
    QWidget* widget = new QWidget();
    QPointer<QWidget> guarded = widget;

    // QPointer 不会 delete 对象
    // 需要其他机制管理生命周期（父对象或手动 delete）
}

// 限制 3：线程安全
// QPointer 本身不是完全线程安全的
// 在多线程中使用时需要额外同步
void threadSafety() {
    QPointer<QObject> ptr;

    // 线程 A
    // if (ptr) { ptr->method(); }

    // 线程 B
    // delete ptr.data();

    // 即使检查了 ptr，在调用 method() 之前对象可能被删除
    // 需要使用互斥锁保护
}
```

---

## 3. QScopedPointer 深度剖析

### 3.1 QScopedPointer 基础

**面试问题**：QScopedPointer 和 std::unique_ptr 有什么区别？

**答案**：

```cpp
#include <QScopedPointer>
#include <memory>

// 基本用法对比
void basicComparison() {
    // QScopedPointer
    QScopedPointer<QFile> qtPtr(new QFile("data.txt"));

    // std::unique_ptr
    std::unique_ptr<QFile> stdPtr(new QFile("data.txt"));
    auto stdPtr2 = std::make_unique<QFile>("data.txt");  // C++14

    // 两者都：
    // - 独占所有权
    // - 不可拷贝
    // - 作用域结束时自动删除
}

// 关键区别
class DifferenceDemo {
public:
    void differences() {
        // 区别 1：移动语义
        std::unique_ptr<int> u1(new int(42));
        std::unique_ptr<int> u2 = std::move(u1);  // ✅ 可移动

        QScopedPointer<int> q1(new int(42));
        // QScopedPointer<int> q2 = std::move(q1);  // ❌ 不可移动

        // 区别 2：容器存储
        std::vector<std::unique_ptr<int>> vec;
        vec.push_back(std::make_unique<int>(1));  // ✅

        // QScopedPointer 不能放入容器（不可移动）

        // 区别 3：作为返回值
        // std::unique_ptr 可以作为函数返回值（移动语义）
        // QScopedPointer 不能
    }

    std::unique_ptr<int> createUnique() {
        return std::make_unique<int>(42);  // ✅
    }

    // QScopedPointer<int> createScoped() {
    //     return QScopedPointer<int>(new int(42));  // ❌ 不可移动
    // }
};

// 结论：优先使用 std::unique_ptr，除非需要与旧 Qt 代码兼容
```

---

### 3.2 自定义删除器

**面试问题**：如何让 QScopedPointer 管理非 new 分配的资源？

**答案**：

```cpp
#include <QScopedPointer>
#include <cstdio>

// 自定义删除器（通过模板参数）
struct FileCloser {
    static void cleanup(FILE* file) {
        if (file) {
            fclose(file);
            qDebug() << "File closed";
        }
    }
};

void customDeleterExample() {
    // 管理 C 风格文件句柄
    QScopedPointer<FILE, FileCloser> file(fopen("data.txt", "r"));

    if (file) {
        char buffer[100];
        fgets(buffer, sizeof(buffer), file.data());
    }
    // 作用域结束时自动调用 fclose
}

// 更多删除器示例
struct MallocDeleter {
    static void cleanup(void* ptr) {
        if (ptr) free(ptr);
    }
};

struct HandleCloser {
    static void cleanup(HANDLE h) {
        if (h && h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
        }
    }
};

// 对比 std::unique_ptr 的删除器
void stdDeleterComparison() {
    // std::unique_ptr 删除器可以是 lambda
    auto fileDeleter = [](FILE* f) { if (f) fclose(f); };
    std::unique_ptr<FILE, decltype(fileDeleter)> file(
        fopen("data.txt", "r"), fileDeleter);

    // QScopedPointer 删除器必须是类型（静态成员函数）
}
```

---

### 3.3 QScopedArrayPointer

```cpp
#include <QScopedArrayPointer>

// 数组版本
void arrayPointerExample() {
    // 管理动态数组
    QScopedArrayPointer<int> arr(new int[100]);

    // 使用下标访问
    arr[0] = 42;
    arr[99] = 100;

    // 作用域结束时调用 delete[]
}

// 对比 std::unique_ptr<T[]>
void arrayComparison() {
    std::unique_ptr<int[]> stdArr(new int[100]);
    stdArr[0] = 42;

    QScopedArrayPointer<int> qtArr(new int[100]);
    qtArr[0] = 42;

    // 功能相同，但 std::unique_ptr<T[]> 支持移动
}
```

---

## 4. QSharedPointer 深度剖析

### 4.1 基础用法

**面试问题**：QSharedPointer 如何工作？它和 std::shared_ptr 有什么区别？

**答案**：

```cpp
#include <QSharedPointer>
#include <QWeakPointer>

// 基本用法
void basicUsage() {
    // 创建方式 1：构造函数
    QSharedPointer<MyClass> ptr1(new MyClass());

    // 创建方式 2：create 静态方法（推荐，类似 make_shared）
    auto ptr2 = QSharedPointer<MyClass>::create(arg1, arg2);

    // 共享所有权
    QSharedPointer<MyClass> ptr3 = ptr1;
    qDebug() << "ref count:" << ptr1.data()->/* 没有直接获取引用计数的方法 */;

    // 使用
    ptr1->method();
    (*ptr1).method();
    MyClass* raw = ptr1.data();  // 获取原始指针
}

// 引用计数演示
void refCountDemo() {
    QSharedPointer<QObject> ptr1 = QSharedPointer<QObject>::create();
    // 引用计数 = 1

    {
        QSharedPointer<QObject> ptr2 = ptr1;
        // 引用计数 = 2

        QSharedPointer<QObject> ptr3 = ptr1;
        // 引用计数 = 3
    }
    // ptr2, ptr3 离开作用域，引用计数 = 1

    ptr1.reset();
    // 引用计数 = 0，对象被删除
}
```

---

### 4.2 QSharedPointer 实现原理

**面试问题**：QSharedPointer 的引用计数是如何实现的？是否线程安全？

**答案**：

```cpp
// QSharedPointer 内部结构（简化版）
/*
┌─────────────────────────────────────────────┐
│ QSharedPointer<T>                           │
├─────────────────────────────────────────────┤
│ T* d;                    // 指向实际对象     │
│ QtSharedPointer::ExternalRefCountData* ref; │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│ ExternalRefCountData                        │
├─────────────────────────────────────────────┤
│ QAtomicInt strongRef;    // 强引用计数       │
│ QAtomicInt weakRef;      // 弱引用计数       │
│ destroy();               // 删除器          │
└─────────────────────────────────────────────┘
*/

// 关键点：使用原子操作保证线程安全
class RefCountExplained {
public:
    void atomicOperations() {
        // QAtomicInt 提供原子操作
        QAtomicInt count(1);

        // 原子增加
        count.ref();  // ++count (原子)

        // 原子减少并检查
        if (!count.deref()) {  // --count == 0?
            // 引用计数归零，删除对象
        }
    }

    void threadSafety() {
        QSharedPointer<MyClass> shared = QSharedPointer<MyClass>::create();

        // 线程 A：拷贝（原子增加引用计数）
        // QSharedPointer<MyClass> copy1 = shared;

        // 线程 B：拷贝（原子增加引用计数）
        // QSharedPointer<MyClass> copy2 = shared;

        // 线程安全：拷贝、赋值、析构都是原子操作

        // 但是！访问对象本身不是线程安全的
        // shared->nonThreadSafeMethod();  // 需要额外同步
    }
};

// create() vs 构造函数
void createVsConstructor() {
    // 构造函数：两次内存分配
    QSharedPointer<MyClass> ptr1(new MyClass());
    // 1. new MyClass() - 分配对象
    // 2. new ExternalRefCountData - 分配控制块

    // create()：一次内存分配（类似 std::make_shared）
    auto ptr2 = QSharedPointer<MyClass>::create();
    // 一次分配：对象和控制块连续存储

    // create() 优势：
    // - 更好的缓存局部性
    // - 减少内存碎片
    // - 异常安全
}
```

---

### 4.3 QWeakPointer

**面试问题**：QWeakPointer 的作用是什么？如何避免循环引用？

**答案**：

```cpp
#include <QSharedPointer>
#include <QWeakPointer>

// QWeakPointer 基础
void weakPointerBasics() {
    QSharedPointer<MyClass> strong = QSharedPointer<MyClass>::create();
    QWeakPointer<MyClass> weak = strong;

    // weak 不增加强引用计数
    // 但增加弱引用计数（防止控制块被删除）

    // 使用 weak：必须先转换为 strong
    if (QSharedPointer<MyClass> locked = weak.lock()) {
        // 对象仍然存在
        locked->method();
    } else {
        // 对象已被删除
    }

    // 或使用 toStrongRef()
    QSharedPointer<MyClass> strong2 = weak.toStrongRef();
    if (strong2) {
        strong2->method();
    }
}

// 循环引用问题
class Parent;
class Child;

class Parent {
public:
    QSharedPointer<Child> child;
    ~Parent() { qDebug() << "Parent destroyed"; }
};

class Child {
public:
    // ❌ 错误：会导致循环引用
    // QSharedPointer<Parent> parent;

    // ✅ 正确：使用弱引用
    QWeakPointer<Parent> parent;

    ~Child() { qDebug() << "Child destroyed"; }
};

void circularReferenceDemo() {
    // 错误示例（如果 Child::parent 是 QSharedPointer）
    // {
    //     auto parent = QSharedPointer<Parent>::create();
    //     auto child = QSharedPointer<Child>::create();
    //     parent->child = child;
    //     child->parent = parent;  // 循环引用！
    // }
    // parent 引用 child，child 引用 parent
    // 两者都无法被删除 -> 内存泄漏

    // 正确示例（Child::parent 是 QWeakPointer）
    {
        auto parent = QSharedPointer<Parent>::create();
        auto child = QSharedPointer<Child>::create();
        parent->child = child;
        child->parent = parent;  // 弱引用，不增加引用计数
    }
    // 输出：
    // Parent destroyed
    // Child destroyed
}

// 观察者模式中的应用
class Subject {
    QList<QWeakPointer<Observer>> m_observers;

public:
    void addObserver(QSharedPointer<Observer> observer) {
        m_observers.append(observer.toWeakRef());
    }

    void notify() {
        for (auto it = m_observers.begin(); it != m_observers.end(); ) {
            if (auto observer = it->lock()) {
                observer->update();
                ++it;
            } else {
                // 观察者已被删除，移除弱引用
                it = m_observers.erase(it);
            }
        }
    }
};
```

---

### 4.4 QEnableSharedFromThis

**面试问题**：如何从对象内部获取指向自身的 QSharedPointer？

**答案**：

```cpp
#include <QEnableSharedFromThis>

// 问题：从 this 创建 QSharedPointer
class BadExample {
public:
    QSharedPointer<BadExample> getShared() {
        // ❌ 错误：会创建新的控制块
        return QSharedPointer<BadExample>(this);
        // 结果：两个独立的 QSharedPointer 管理同一个对象
        // 对象会被 delete 两次！
    }
};

// 解决方案：QEnableSharedFromThis
class GoodExample : public QEnableSharedFromThis<GoodExample> {
public:
    QSharedPointer<GoodExample> getShared() {
        // ✅ 正确：返回共享同一控制块的 QSharedPointer
        return sharedFromThis();
    }

    void registerSelf(Registry& registry) {
        // 在对象内部需要传递 shared_ptr 时
        registry.add(sharedFromThis());
    }
};

// 使用示例
void enableSharedFromThisUsage() {
    // 必须先被 QSharedPointer 管理
    auto obj = QSharedPointer<GoodExample>::create();

    // 然后才能调用 sharedFromThis()
    QSharedPointer<GoodExample> ptr1 = obj->getShared();
    QSharedPointer<GoodExample> ptr2 = obj->getShared();

    // ptr1, ptr2, obj 共享同一个控制块

    // ❌ 错误：对象未被 QSharedPointer 管理时调用
    // GoodExample* raw = new GoodExample();
    // raw->sharedFromThis();  // 未定义行为或 crash
}

// 实现原理
/*
QEnableSharedFromThis 内部持有一个 QWeakPointer<T>
当对象被 QSharedPointer 管理时，这个 weak pointer 被初始化
sharedFromThis() 调用 weak.lock() 返回 QSharedPointer
*/
```

---

### 4.5 与 std::shared_ptr 互操作（Qt 6）

```cpp
#include <QSharedPointer>
#include <memory>

// Qt 6 支持与标准智能指针互转
void interoperability() {
    // QSharedPointer -> std::shared_ptr
    QSharedPointer<MyClass> qPtr = QSharedPointer<MyClass>::create();
    std::shared_ptr<MyClass> stdPtr = qPtr.toStdSharedPointer();

    // std::shared_ptr -> QSharedPointer
    std::shared_ptr<MyClass> stdPtr2 = std::make_shared<MyClass>();
    QSharedPointer<MyClass> qPtr2 =
        QSharedPointer<MyClass>::fromStdSharedPointer(stdPtr2);

    // 注意：转换后共享同一个对象，但控制块是独立的
    // 需要小心生命周期管理
}
```

---

## 5. 智能指针与 Qt 对象模型

### 5.1 QObject 与智能指针的兼容性

**面试问题**：为什么不推荐用 std::shared_ptr 管理 QObject？

**答案**：

```cpp
#include <memory>
#include <QObject>
#include <QWidget>

// 问题 1：父子树冲突
void parentChildConflict() {
    auto parent = std::make_shared<QWidget>();
    auto child = std::make_shared<QWidget>(parent.get());

    // child 设置了 parent
    // 当 parent 被删除时，Qt 会自动 delete child
    // 然后 child 的 shared_ptr 析构时再次 delete
    // -> double delete, crash!
}

// 问题 2：deleteLater 冲突
void deleteLaterConflict() {
    auto obj = std::make_shared<QObject>();
    obj->deleteLater();

    // deleteLater 会在事件循环中删除对象
    // shared_ptr 也会在引用计数归零时删除
    // -> double delete!
}

// 问题 3：跨线程删除
void crossThreadDeletion() {
    std::shared_ptr<QObject> obj;

    // 线程 A 创建
    obj = std::make_shared<QObject>();
    obj->moveToThread(threadB);

    // 线程 A 的 shared_ptr 析构可能在线程 A
    // 但 QObject 应该在其所属线程删除
    // -> 可能导致问题
}

// 推荐做法
void recommendedApproach() {
    // 1. 使用 Qt 父子树管理 QObject 生命周期
    QWidget* parent = new QWidget();
    QWidget* child = new QWidget(parent);
    // parent 析构时自动删除 child

    // 2. 使用 QPointer 观察 QObject
    QPointer<QWidget> observed = child;

    // 3. 如果必须使用智能指针，使用自定义删除器
    std::shared_ptr<QObject> safe(
        new QObject(),
        [](QObject* obj) {
            if (obj->thread() == QThread::currentThread()) {
                delete obj;
            } else {
                obj->deleteLater();  // 安全的跨线程删除
            }
        }
    );
}
```

---

### 5.2 推荐的 QObject 生命周期管理

```cpp
// 最佳实践总结
class BestPractices {
public:
    // 1. 优先使用父子树
    void useParentChild() {
        QWidget* mainWindow = new QWidget();
        QLabel* label = new QLabel("Hello", mainWindow);
        QPushButton* button = new QPushButton("Click", mainWindow);
        // mainWindow 析构时自动清理所有子对象
    }

    // 2. 顶层对象使用栈分配或 unique_ptr
    void topLevelObjects() {
        QApplication app(argc, argv);
        QMainWindow mainWindow;  // 栈上
        mainWindow.show();
        app.exec();
        // 自动清理
    }

    // 3. 需要观察生命周期时使用 QPointer
    void observeLifetime() {
        QPointer<QWidget> observed = someWidget;
        // 可以安全检查 observed 是否仍然有效
    }

    // 4. 跨线程传递数据使用 QSharedPointer（非 QObject）
    void crossThreadData() {
        auto data = QSharedPointer<MyData>::create();
        emit dataReady(data);  // 安全传递给其他线程
    }
};
```

---

## 6. 性能对比

### 6.1 智能指针性能特点

```cpp
// 性能对比测试
void performanceComparison() {
    const int N = 1000000;

    // 1. 原始指针（最快，但不安全）
    {
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < N; ++i) {
            int* p = new int(i);
            delete p;
        }
        qDebug() << "Raw pointer:" << timer.elapsed() << "ms";
    }

    // 2. QScopedPointer（栈上开销小）
    {
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < N; ++i) {
            QScopedPointer<int> p(new int(i));
        }
        qDebug() << "QScopedPointer:" << timer.elapsed() << "ms";
    }

    // 3. std::unique_ptr（与 QScopedPointer 类似）
    {
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < N; ++i) {
            auto p = std::make_unique<int>(i);
        }
        qDebug() << "std::unique_ptr:" << timer.elapsed() << "ms";
    }

    // 4. QSharedPointer（引用计数开销）
    {
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < N; ++i) {
            auto p = QSharedPointer<int>::create(i);
        }
        qDebug() << "QSharedPointer:" << timer.elapsed() << "ms";
    }

    // 5. std::shared_ptr（与 QSharedPointer 类似）
    {
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < N; ++i) {
            auto p = std::make_shared<int>(i);
        }
        qDebug() << "std::shared_ptr:" << timer.elapsed() << "ms";
    }
}

/*
典型结果：
Raw pointer:      ~50ms
QScopedPointer:   ~55ms
std::unique_ptr:  ~55ms
QSharedPointer:   ~150ms  (原子操作开销)
std::shared_ptr:  ~150ms
*/
```

---

### 6.2 选择指南

```cpp
/*
┌─────────────────────────────────────────────────────────────────┐
│                    Qt 智能指针选择指南                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  需要管理 QObject 派生类？                                       │
│       │                                                         │
│       ├── 是 ──→ 使用 Qt 父子树（首选）                          │
│       │         或 QPointer（观察，不拥有）                       │
│       │                                                         │
│       └── 否 ──→ 需要共享所有权？                                │
│                      │                                          │
│                      ├── 是 ──→ QSharedPointer 或 std::shared_ptr│
│                      │         (跨线程用 QSharedPointer)         │
│                      │                                          │
│                      └── 否 ──→ std::unique_ptr（首选）          │
│                                 或 QScopedPointer               │
│                                                                 │
│  额外考虑：                                                      │
│  - Qt 6 新项目：优先标准智能指针                                  │
│  - 需要与旧 Qt 代码兼容：Qt 智能指针                              │
│  - 性能敏感：避免不必要的共享指针                                  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
*/
```

---

## 7. 面试问题精选

### Q1: QPointer 实现原理

**问题**：QPointer 是如何知道对象被删除的？

**参考答案**：

QPointer 内部使用 `QWeakPointer<QObjectData>` 存储对对象的引用。当 QObject 被删除时：

1. QObject 析构函数执行
2. QObjectPrivate 中的数据被标记为无效
3. 所有指向该对象的 QPointer 的内部弱引用自动失效
4. QPointer::isNull() 返回 true

这种机制不使用信号槽（避免性能开销），而是直接操作 QObject 内部数据结构。

---

### Q2: shared_ptr 管理 QObject 的问题

**问题**：为什么不推荐用 std::shared_ptr 管理 QObject？

**参考答案**：

三个主要问题：

1. **父子树冲突**：QObject 父对象会自动 delete 子对象，与 shared_ptr 的 delete 形成 double delete

2. **deleteLater 冲突**：deleteLater 和 shared_ptr 都会尝试删除对象

3. **线程关联**：QObject 应该在其所属线程删除，shared_ptr 可能在任意线程析构

推荐使用 Qt 父子树管理 QObject 生命周期，用 QPointer 观察对象。

---

### Q3: QSharedPointer::create() 的优势

**问题**：QSharedPointer::create() 和直接构造有什么区别？

**参考答案**：

```cpp
// 直接构造：两次内存分配
QSharedPointer<MyClass> ptr1(new MyClass());
// 1. new MyClass()
// 2. new 控制块

// create()：一次内存分配
auto ptr2 = QSharedPointer<MyClass>::create();
// 对象和控制块连续分配
```

create() 优势：
- 更好的缓存局部性
- 减少内存碎片
- 异常安全（避免 new 成功但控制块分配失败的情况）

---

### Q4: 循环引用的解决

**问题**：如何用 Qt 智能指针解决循环引用？

**参考答案**：

使用 QWeakPointer 打破循环：

```cpp
class Parent {
    QSharedPointer<Child> child;  // 强引用
};

class Child {
    QWeakPointer<Parent> parent;  // 弱引用，打破循环
};
```

原则：确定所有权关系，拥有者用强引用，被拥有者用弱引用指向拥有者。

---

### Q5: QPointer 的线程安全性

**问题**：QPointer 是线程安全的吗？

**参考答案**：

QPointer 的赋值和置空是线程安全的（原子操作），但使用模式不是：

```cpp
QPointer<QObject> ptr = obj;

// 线程 A
if (ptr) {
    // 线程 B 可能在此时删除对象
    ptr->method();  // 可能访问已删除对象
}
```

正确做法：在多线程中需要额外同步，或将 QPointer 转换为强引用后使用。

---

### Q6: 智能指针选型

**问题**：如何选择合适的智能指针？

**参考答案**：

| 场景 | 推荐 |
|------|------|
| 管理 QObject | Qt 父子树 + QPointer 观察 |
| 独占所有权 | std::unique_ptr（或 QScopedPointer） |
| 共享所有权 | std::shared_ptr / QSharedPointer |
| 跨线程传递数据 | QSharedPointer（原子引用计数） |
| 避免循环引用 | QWeakPointer / std::weak_ptr |
| 从 this 获取 shared | QEnableSharedFromThis |

---

## 8. 总结

| 智能指针 | 用途 | 特点 |
|----------|------|------|
| QPointer | 观察 QObject | 对象删除后自动 nullptr |
| QScopedPointer | 独占所有权 | 不可移动，作用域结束释放 |
| QSharedPointer | 共享所有权 | 原子引用计数，线程安全拷贝 |
| QWeakPointer | 弱引用 | 不增加引用计数，配合 QSharedPointer |

**核心要点**：

1. QPointer 专门为 QObject 设计，解决悬垂指针问题
2. QObject 优先使用父子树管理生命周期
3. 非 QObject 对象优先使用标准智能指针
4. QSharedPointer::create() 比构造函数更高效
5. 使用 QWeakPointer 打破循环引用
6. Qt 6 支持与标准智能指针互转
