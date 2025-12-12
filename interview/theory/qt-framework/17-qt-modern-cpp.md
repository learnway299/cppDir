# Qt 与现代 C++

## 概述

Qt 框架持续演进以支持现代 C++ 特性，从 Qt 5 开始逐步引入 C++11/14/17/20 支持。本文档涵盖 Qt 与现代 C++ 结合使用的面试常见问题。

---

## 1. Lambda 表达式与信号槽

### 1.1 基本 Lambda 连接

```cpp
// 传统方式（需要定义槽函数）
connect(button, &QPushButton::clicked, this, &MyClass::onButtonClicked);

// Lambda 方式（更简洁）
connect(button, &QPushButton::clicked, [this]() {
    qDebug() << "Button clicked!";
    updateUI();
});

// 带参数的信号
connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        [this](int value) {
            m_value = value;
            emit valueChanged(value);
        });

// 忽略参数
connect(lineEdit, &QLineEdit::textChanged, [this](const QString&) {
    validateForm();
});
```

### 1.2 Lambda 捕获与对象生命周期

```cpp
// 危险：捕获 this 但对象可能被删除
connect(button, &QPushButton::clicked, [this]() {
    // 如果 this 已被删除，这里会崩溃
    doSomething();
});

// 安全方式 1：使用 QPointer
QPointer<MyClass> self = this;
connect(button, &QPushButton::clicked, [self]() {
    if (self) {
        self->doSomething();
    }
});

// 安全方式 2：指定上下文对象（推荐）
connect(button, &QPushButton::clicked, this, [this]() {
    // this 被删除时连接自动断开
    doSomething();
});

// 安全方式 3：使用 QObject::connect 的返回值
QMetaObject::Connection conn = connect(button, &QPushButton::clicked, [this]() {
    doSomething();
});
// 稍后断开
QObject::disconnect(conn);
```

### 1.3 泛型 Lambda（C++14）

```cpp
// C++14 泛型 Lambda
auto handler = [](auto value) {
    qDebug() << "Value:" << value;
};

connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), handler);
connect(slider, &QSlider::valueChanged, handler);

// C++20 模板 Lambda
auto templateHandler = []<typename T>(T value) {
    if constexpr (std::is_same_v<T, int>) {
        qDebug() << "Int:" << value;
    } else if constexpr (std::is_same_v<T, QString>) {
        qDebug() << "String:" << value;
    }
};
```

---

## 2. 智能指针与 Qt

### 2.1 QScopedPointer

```cpp
// Qt 自己的作用域指针（类似 std::unique_ptr）
class MyClass {
    QScopedPointer<QFile> m_file;
    QScopedArrayPointer<char> m_buffer;  // 数组版本

public:
    MyClass() : m_file(new QFile("data.txt")),
                m_buffer(new char[1024]) {}

    void process() {
        // 使用 -> 访问
        if (m_file->open(QIODevice::ReadOnly)) {
            m_file->read(m_buffer.data(), 1024);
        }
    }
    // 析构时自动删除
};

// 自定义删除器
struct FileCloser {
    static void cleanup(FILE *file) {
        if (file) fclose(file);
    }
};
QScopedPointer<FILE, FileCloser> cFile(fopen("data.txt", "r"));
```

### 2.2 QSharedPointer 与 std::shared_ptr

```cpp
// QSharedPointer
QSharedPointer<MyObject> ptr1 = QSharedPointer<MyObject>::create();
QSharedPointer<MyObject> ptr2 = ptr1;  // 共享所有权

// 使用 make 函数（避免额外分配）
auto ptr3 = QSharedPointer<MyObject>::create(arg1, arg2);

// 转换为 std::shared_ptr（Qt 6）
std::shared_ptr<MyObject> stdPtr = ptr1.toStdSharedPointer();

// 从 std::shared_ptr 创建
auto fromStd = QSharedPointer<MyObject>::fromStdSharedPointer(stdPtr);

// 弱引用
QWeakPointer<MyObject> weak = ptr1;
if (QSharedPointer<MyObject> locked = weak.lock()) {
    locked->doSomething();
}
```

### 2.3 std::unique_ptr 与 Qt 对象

```cpp
// 使用 std::unique_ptr 管理非 QObject
class DataProcessor {
    std::unique_ptr<std::vector<int>> m_data;

public:
    DataProcessor() : m_data(std::make_unique<std::vector<int>>()) {}
};

// 注意：QObject 子类通常用对象树管理
class MyWidget : public QWidget {
public:
    MyWidget(QWidget *parent = nullptr) : QWidget(parent) {
        // 子对象由父对象管理，不需要智能指针
        auto label = new QLabel("Text", this);  // parent 负责删除
    }
};

// 无父对象的 QObject 可以用智能指针
std::unique_ptr<QObject> obj = std::make_unique<QObject>();
// 但要小心：如果设置了父对象，会导致双重删除！
```

---

## 3. 移动语义

### 3.1 Qt 容器的移动支持

```cpp
// Qt 容器支持移动语义
QVector<int> createVector() {
    QVector<int> vec{1, 2, 3, 4, 5};
    return vec;  // 移动返回，不复制
}

void processVector() {
    QVector<int> vec1 = createVector();  // 移动构造

    QVector<int> vec2;
    vec2 = std::move(vec1);  // 移动赋值
    // vec1 现在为空

    // 移动到容器
    QList<QVector<int>> list;
    list.append(std::move(vec2));  // 移动追加
}
```

### 3.2 QString 和 QByteArray 的移动

```cpp
// QString 支持移动
QString createString() {
    QString str = "Hello";
    str += " World";
    return str;  // 移动返回
}

// QByteArray 同样支持
QByteArray readData() {
    QFile file("data.bin");
    file.open(QIODevice::ReadOnly);
    return file.readAll();  // 移动返回
}

// 显式移动
void processData() {
    QByteArray data = readData();
    QByteArray data2 = std::move(data);  // 移动
    // data 现在为空，但仍可安全使用
    data = "new data";  // 重新赋值
}
```

### 3.3 自定义类的移动支持

```cpp
// 自定义类实现移动语义
class DataHolder {
    QVector<double> m_data;
    QString m_name;

public:
    // 移动构造函数
    DataHolder(DataHolder&& other) noexcept
        : m_data(std::move(other.m_data)),
          m_name(std::move(other.m_name)) {}

    // 移动赋值运算符
    DataHolder& operator=(DataHolder&& other) noexcept {
        if (this != &other) {
            m_data = std::move(other.m_data);
            m_name = std::move(other.m_name);
        }
        return *this;
    }

    // 接受右值引用的方法
    void setData(QVector<double>&& data) {
        m_data = std::move(data);
    }
};

// 使用 emplace 避免临时对象
QVector<DataHolder> holders;
holders.emplaceBack();  // Qt 6 / C++17
// holders.push_back(DataHolder{});  // 可能有移动
```

---

## 4. Range-based for 与 Qt 容器

### 4.1 基本用法

```cpp
QList<int> numbers = {1, 2, 3, 4, 5};

// 只读迭代
for (int n : numbers) {
    qDebug() << n;
}

// const 引用（推荐用于只读）
for (const int& n : numbers) {
    qDebug() << n;
}

// 修改元素
for (int& n : numbers) {
    n *= 2;
}

// Qt 6 的 asKeyValueRange
QMap<QString, int> map = {{"a", 1}, {"b", 2}};
for (auto [key, value] : map.asKeyValueRange()) {
    qDebug() << key << ":" << value;
}
```

### 4.2 隐式共享的陷阱

```cpp
// 危险：可能触发深拷贝
QList<int> numbers = {1, 2, 3};
for (int& n : numbers) {  // 非 const 引用触发 detach
    n *= 2;
}

// 安全：使用 const 引用
for (const int& n : numbers) {  // 不触发 detach
    qDebug() << n;
}

// Qt 6 改进：qAsConst
QList<int> shared = numbers;  // 共享数据
for (const int& n : qAsConst(shared)) {  // 强制 const，不 detach
    qDebug() << n;
}

// C++17 std::as_const
for (const int& n : std::as_const(shared)) {
    qDebug() << n;
}
```

### 4.3 结构化绑定（C++17）

```cpp
// 遍历 QMap
QMap<QString, int> scores = {{"Alice", 95}, {"Bob", 87}};
for (auto it = scores.begin(); it != scores.end(); ++it) {
    auto [key, value] = std::make_pair(it.key(), it.value());
    qDebug() << key << ":" << value;
}

// Qt 6 更简洁的方式
for (auto [key, value] : scores.asKeyValueRange()) {
    qDebug() << key << ":" << value;
}

// 解构 QPair
QList<QPair<QString, int>> pairs = {{"a", 1}, {"b", 2}};
for (auto [first, second] : pairs) {
    qDebug() << first << second;
}

// 解构自定义类型
struct Point { int x, y; };
QList<Point> points = {{1, 2}, {3, 4}};
for (auto [x, y] : points) {
    qDebug() << x << y;
}
```

---

## 5. auto 和类型推导

### 5.1 基本使用

```cpp
// 迭代器类型推导
QMap<QString, QVector<int>> data;
auto it = data.begin();  // QMap<QString, QVector<int>>::iterator

// 返回值类型推导
auto widget = new QWidget();  // QWidget*
auto result = someFunction(); // 自动推导

// 避免冗余类型
QSharedPointer<MyClass> ptr = QSharedPointer<MyClass>::create();  // 冗余
auto ptr2 = QSharedPointer<MyClass>::create();  // 简洁
```

### 5.2 与 Qt 类型的交互

```cpp
// QString 相关
auto str = QString("hello");  // QString
auto cstr = str.toUtf8();     // QByteArray
auto data = cstr.data();      // const char*

// 注意 auto 不保留引用
QList<int> list = {1, 2, 3};
auto item = list[0];   // int（拷贝）
auto& ref = list[0];   // int&（引用）
const auto& cref = list[0];  // const int&

// decltype 保留完整类型
decltype(auto) result = someFunction();  // 保留引用和 const
```

### 5.3 返回类型推导

```cpp
// C++14 返回类型推导
auto createWidget() {
    return new QWidget();  // 返回 QWidget*
}

// 尾置返回类型（复杂情况）
template <typename T>
auto getValue(T* obj) -> decltype(obj->value()) {
    return obj->value();
}

// C++17 类模板参数推导
QList list = {1, 2, 3};  // QList<int>
QPair pair = {"key", 42}; // QPair<const char*, int>

// Qt 6 支持 CTAD
QSharedPointer ptr(new MyClass());  // QSharedPointer<MyClass>
```

---

## 6. constexpr 与 Qt

### 6.1 编译时常量

```cpp
// Qt 6 引入更多 constexpr 支持
constexpr QChar null('\0');
constexpr QLatin1String hello("hello");

// 编译时字符串字面量
constexpr auto str = QLatin1StringView("compile time");

// Qt 6 的 QStringView
constexpr QStringView view = u"unicode string";
```

### 6.2 constexpr 函数

```cpp
// 可以在编译时求值的函数
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// Qt 6 QColor 支持 constexpr
constexpr QColor red(255, 0, 0);
constexpr QColor blue = QColor::fromRgb(0, 0, 255);

// 编译时计算尺寸
constexpr QSize defaultSize(800, 600);
constexpr QRect defaultRect(0, 0, 800, 600);
```

### 6.3 if constexpr（C++17）

```cpp
template <typename T>
void processValue(const T& value) {
    if constexpr (std::is_same_v<T, QString>) {
        qDebug() << "String:" << value;
    } else if constexpr (std::is_arithmetic_v<T>) {
        qDebug() << "Number:" << value;
    } else if constexpr (std::is_pointer_v<T>) {
        qDebug() << "Pointer:" << *value;
    }
}

// 用于 Qt 类型判断
template <typename T>
void handleQtType(T* obj) {
    if constexpr (std::is_base_of_v<QWidget, T>) {
        obj->show();
    } else if constexpr (std::is_base_of_v<QObject, T>) {
        qDebug() << obj->objectName();
    }
}
```

---

## 7. std::optional 与 std::variant

### 7.1 std::optional 替代魔法值

```cpp
// 传统方式：使用魔法值
int findIndex(const QString& target) {
    // 找不到返回 -1
    return -1;
}

// 现代方式：使用 std::optional
std::optional<int> findIndex(const QList<QString>& list, const QString& target) {
    int idx = list.indexOf(target);
    if (idx >= 0) {
        return idx;
    }
    return std::nullopt;
}

// 使用
if (auto idx = findIndex(list, "target")) {
    qDebug() << "Found at:" << *idx;
} else {
    qDebug() << "Not found";
}

// 配合 Qt 类型
std::optional<QVariant> getValue(const QSettings& settings, const QString& key) {
    if (settings.contains(key)) {
        return settings.value(key);
    }
    return std::nullopt;
}
```

### 7.2 std::variant 类型安全联合

```cpp
// 替代 QVariant 的某些用法
using ConfigValue = std::variant<int, double, QString, bool>;

class Config {
    QMap<QString, ConfigValue> m_values;

public:
    template <typename T>
    void setValue(const QString& key, T value) {
        m_values[key] = value;
    }

    template <typename T>
    std::optional<T> getValue(const QString& key) const {
        auto it = m_values.find(key);
        if (it != m_values.end()) {
            if (auto* val = std::get_if<T>(&it.value())) {
                return *val;
            }
        }
        return std::nullopt;
    }
};

// 使用 std::visit
void printConfig(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, QString>) {
            qDebug() << "String:" << arg;
        } else {
            qDebug() << "Value:" << arg;
        }
    }, value);
}
```

### 7.3 与 QVariant 的比较

```cpp
// QVariant：运行时类型
QVariant var = 42;
var = QString("hello");  // 可以改变类型
int val = var.toInt();    // 运行时转换

// std::variant：编译时类型约束
std::variant<int, QString> v = 42;
v = QString("hello");     // OK，QString 在类型列表中
// v = QDate::currentDate();  // 编译错误！

// 何时使用 QVariant
// - 与 Qt 元对象系统交互（属性、Model/View）
// - 需要存储任意类型
// - 序列化/反序列化

// 何时使用 std::variant
// - 已知类型集合
// - 需要编译时类型安全
// - 性能关键场景
```

---

## 8. 并发与异步

### 8.1 std::thread vs QThread

```cpp
// std::thread
std::thread t([]() {
    // 后台工作
});
t.join();

// QThread 的现代用法
class Worker : public QObject {
    Q_OBJECT
public slots:
    void doWork() {
        // 工作内容
        emit finished();
    }
signals:
    void finished();
};

auto thread = new QThread;
auto worker = new Worker;
worker->moveToThread(thread);

connect(thread, &QThread::started, worker, &Worker::doWork);
connect(worker, &Worker::finished, thread, &QThread::quit);
connect(worker, &Worker::finished, worker, &QObject::deleteLater);
connect(thread, &QThread::finished, thread, &QObject::deleteLater);

thread->start();
```

### 8.2 std::async 和 QFuture

```cpp
// std::async
auto future = std::async(std::launch::async, []() {
    return computeResult();
});
auto result = future.get();  // 阻塞等待

// Qt Concurrent
QFuture<int> qtFuture = QtConcurrent::run([]() {
    return computeResult();
});

// 非阻塞等待
QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);
connect(watcher, &QFutureWatcher<int>::finished, [watcher]() {
    int result = watcher->result();
    qDebug() << "Result:" << result;
    watcher->deleteLater();
});
watcher->setFuture(qtFuture);
```

### 8.3 std::mutex vs QMutex

```cpp
// std::mutex
std::mutex mtx;
{
    std::lock_guard<std::mutex> lock(mtx);
    // 临界区
}

// QMutex
QMutex qmtx;
{
    QMutexLocker locker(&qmtx);
    // 临界区
}

// C++17 std::scoped_lock（多锁）
std::mutex m1, m2;
{
    std::scoped_lock lock(m1, m2);  // 同时锁定，避免死锁
}

// Qt 6 读写锁
QReadWriteLock rwLock;
{
    QReadLocker readLock(&rwLock);
    // 读取操作
}
{
    QWriteLocker writeLock(&rwLock);
    // 写入操作
}
```

---

## 9. Qt 6 的现代 C++ 改进

### 9.1 范围（C++20 Ranges）支持

```cpp
// Qt 6 容器支持 ranges
#include <ranges>

QList<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 过滤和转换
auto result = numbers
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * n; });

for (int n : result) {
    qDebug() << n;  // 4, 16, 36, 64, 100
}

// 与 Qt 算法结合
QList<int> filtered;
std::ranges::copy_if(numbers, std::back_inserter(filtered),
                     [](int n) { return n > 5; });
```

### 9.2 QStringView（轻量字符串视图）

```cpp
// QStringView：不拥有数据，类似 std::string_view
void processText(QStringView text) {
    qDebug() << text.length();
    qDebug() << text.mid(0, 5);
}

// 可以接受多种字符串类型
QString str = "Hello World";
processText(str);           // QString
processText(u"Hello");      // UTF-16 字面量
processText(QLatin1String("Hello"));

// 避免临时 QString 创建
void oldWay(const QString& text);     // 可能创建临时对象
void newWay(QStringView text);        // 零拷贝
```

### 9.3 属性绑定系统

```cpp
// Qt 6 的声明式属性绑定
class Rectangle : public QObject {
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth BINDABLE bindableWidth)
    Q_PROPERTY(int height READ height WRITE setHeight BINDABLE bindableHeight)
    Q_PROPERTY(int area READ area BINDABLE bindableArea)

public:
    int width() const { return m_width; }
    void setWidth(int w) { m_width = w; }
    QBindable<int> bindableWidth() { return &m_width; }

    int height() const { return m_height; }
    void setHeight(int h) { m_height = h; }
    QBindable<int> bindableHeight() { return &m_height; }

    int area() const { return m_area; }
    QBindable<int> bindableArea() { return &m_area; }

private:
    Q_OBJECT_BINDABLE_PROPERTY(Rectangle, int, m_width)
    Q_OBJECT_BINDABLE_PROPERTY(Rectangle, int, m_height)
    Q_OBJECT_COMPUTED_PROPERTY(Rectangle, int, m_area,
        &Rectangle::computeArea)

    int computeArea() const { return m_width * m_height; }
};

// 使用
Rectangle rect;
rect.setWidth(10);
rect.setHeight(20);
qDebug() << rect.area();  // 200

// 设置绑定
rect.bindableWidth().setBinding([&rect]() {
    return rect.height() * 2;  // width 始终是 height 的两倍
});
```

---

## 面试常见问题

### Q1: Lambda 连接信号槽时如何避免悬垂指针？

**答案**：
1. 指定上下文对象（最推荐）：
   ```cpp
   connect(sender, &Sender::signal, receiver, [receiver]() {
       receiver->doSomething();
   });
   ```
   当 receiver 被删除时连接自动断开。

2. 使用 QPointer：
   ```cpp
   QPointer<MyClass> self = this;
   connect(sender, &Sender::signal, [self]() {
       if (self) self->doSomething();
   });
   ```

3. 保存连接并手动断开：
   ```cpp
   auto conn = connect(...);
   // 在析构函数中 disconnect(conn);
   ```

### Q2: Qt 容器和 STL 容器的主要区别？

**答案**：
| 特性 | Qt 容器 | STL 容器 |
|------|---------|----------|
| 隐式共享 | 是（COW） | 否 |
| 线程安全 | 读取共享安全 | 无 |
| 迭代器失效 | COW 时全部失效 | 部分操作失效 |
| 内存分配 | 可能更少 | 标准行为 |
| 与 Qt API 集成 | 原生支持 | 需要转换 |

Qt 6 中 QList 和 QVector 合并为 QList（基于 std::vector）。

### Q3: std::optional 相比 Qt 的传统做法有什么优势？

**答案**：
传统做法：
- 返回魔法值（-1、nullptr、空字符串）
- 输出参数 + bool 返回值
- 抛出异常

std::optional 优势：
- 类型安全，编译时检查
- 语义清晰，不需要记忆魔法值
- 链式操作支持（value_or、transform 等）
- 不占用类型的有效值空间

### Q4: QStringView 的使用场景？

**答案**：
QStringView 是轻量级字符串视图，适用于：
1. 函数参数：避免临时 QString 创建
2. 字符串操作：不需要修改的只读操作
3. 性能关键代码：零拷贝

注意：
- 不拥有数据，需确保源字符串生命周期
- 不能用于需要持久存储的场景
- 类似 std::string_view

### Q5: Qt 6 的属性绑定系统有什么优势？

**答案**：
1. 声明式编程：属性自动响应依赖变化
2. 减少样板代码：不需要手动连接信号更新
3. 避免 bug：不会遗漏更新
4. 性能优化：只在需要时求值
5. 与 QML 更好集成：共享绑定引擎

### Q6: 如何在 Qt 项目中安全使用 std::thread？

**答案**：
可以使用但需注意：
1. 不能直接操作 Qt GUI（必须在主线程）
2. 用信号槽跨线程通信：
   ```cpp
   std::thread t([this]() {
       // 后台工作
       QMetaObject::invokeMethod(this, "onComplete", Qt::QueuedConnection);
   });
   ```
3. 或使用 QtConcurrent，它与 Qt 事件循环集成更好
4. 确保线程结束前不要销毁相关 Qt 对象

### Q7: range-based for 循环遍历 Qt 容器的注意事项？

**答案**：
1. 使用 const 引用避免触发 detach：
   ```cpp
   for (const auto& item : container) { }
   ```
2. 共享容器使用 qAsConst 或 std::as_const
3. 遍历时不要修改容器结构（添加/删除元素）
4. Qt 6 使用 asKeyValueRange() 遍历关联容器

---

## 总结

Qt 与现代 C++ 结合要点：

| 特性 | 使用建议 |
|------|---------|
| Lambda | 信号槽连接时指定上下文对象 |
| 智能指针 | QObject 用对象树，其他用智能指针 |
| 移动语义 | Qt 容器已支持，自定义类记得实现 |
| auto | 注意隐式共享的 detach 问题 |
| range-for | 使用 const 引用或 qAsConst |
| std::optional | 替代魔法值返回 |
| std::variant | 类型安全场景使用，与 QVariant 互补 |
| 并发 | 优先使用 Qt Concurrent |

Qt 6 改进：
1. 要求 C++17
2. QList 和 QVector 统一
3. QStringView 轻量视图
4. 属性绑定系统
5. 更好的 STL 兼容性
