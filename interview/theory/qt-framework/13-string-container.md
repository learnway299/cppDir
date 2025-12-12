# Qt 字符串与容器（String & Container）

## 概述

Qt 提供了自己的字符串类 QString 和一系列容器类，它们与 STL 有相似之处，但针对 Qt 应用进行了优化，特别是隐式共享（Copy-on-Write）机制。

---

## 1. QString

### 1.1 QString 基础

**面试问题**：QString 和 std::string 有什么区别？

**答案**：

| 特性 | QString | std::string |
|------|---------|-------------|
| 编码 | UTF-16 | 通常 UTF-8 或 ANSI |
| 隐式共享 | ✅ COW | ❌ |
| Unicode | 原生支持 | C++20 才较好 |
| 与 Qt 集成 | 原生 | 需转换 |

```cpp
#include <QString>

void qstringBasics() {
    // 构造
    QString s1 = "Hello";
    QString s2("World");
    QString s3 = QString::fromUtf8("UTF-8 文本");
    QString s4 = QString::fromLocal8Bit("Local encoding");
    QString s5 = QString::number(42);
    QString s6 = QString::number(3.14, 'f', 2);  // "3.14"

    // 拼接
    QString result = s1 + " " + s2;
    result += "!";
    result.append(" Extra");
    result.prepend("Greeting: ");

    // 格式化
    QString formatted = QString("Name: %1, Age: %2").arg("Alice").arg(25);
    QString formatted2 = QString("%1 + %2 = %3").arg(1).arg(2).arg(1 + 2);

    // 比较
    bool equal = (s1 == s2);
    bool less = (s1 < s2);
    int cmp = QString::compare(s1, s2, Qt::CaseInsensitive);

    // 搜索
    int pos = result.indexOf("World");
    bool contains = result.contains("Hello");
    bool startsWith = result.startsWith("Greeting");
    bool endsWith = result.endsWith("!");

    // 提取
    QString sub = result.mid(0, 5);      // 从位置 0 取 5 个字符
    QString left = result.left(5);       // 前 5 个字符
    QString right = result.right(5);     // 后 5 个字符

    // 修改
    QString modified = result;
    modified.replace("World", "Qt");
    modified.remove("Extra");
    modified.insert(0, "Start ");

    // 大小写
    QString upper = result.toUpper();
    QString lower = result.toLower();

    // 分割和合并
    QStringList parts = result.split(' ');
    QString joined = parts.join('-');

    // 去除空白
    QString trimmed = QString("  text  ").trimmed();     // "text"
    QString simplified = QString("  a   b  ").simplified(); // "a b"
}
```

---

### 1.2 QString 与其他类型转换

```cpp
void qstringConversions() {
    // QString -> 数字
    QString numStr = "42";
    int i = numStr.toInt();
    double d = QString("3.14").toDouble();
    long l = QString("123456").toLong();

    // 带成功标志
    bool ok;
    int value = QString("abc").toInt(&ok);  // ok = false

    // QString -> std::string
    QString qstr = "Hello";
    std::string stdStr = qstr.toStdString();
    QByteArray bytes = qstr.toUtf8();
    QByteArray local = qstr.toLocal8Bit();

    // std::string -> QString
    QString fromStd = QString::fromStdString(stdStr);

    // QByteArray -> QString
    QByteArray ba = "bytes";
    QString fromBa = QString::fromUtf8(ba);

    // const char* -> QString
    const char* cstr = "C string";
    QString fromC = QString::fromUtf8(cstr);
    QString fromC2(cstr);  // 假设 UTF-8

    // QString -> const char*（危险，需要保持 QByteArray 存活）
    QByteArray temp = qstr.toUtf8();
    const char* ptr = temp.constData();

    // Unicode 字符
    QString unicode = QString::fromUtf16(u"UTF-16 文本");
    QString unicode32 = QString::fromUcs4(U"UTF-32 文本");
}
```

---

### 1.3 QStringView（Qt 5.10+）

**面试问题**：QStringView 有什么用？

**答案**：

```cpp
#include <QStringView>

void stringViewDemo() {
    QString str = "Hello World";

    // QStringView 是轻量级的视图，不拥有数据
    QStringView view(str);
    QStringView view2 = str;  // 隐式转换

    // 可以指向子串
    QStringView sub = view.mid(0, 5);  // "Hello"

    // 常用于函数参数，避免复制
    auto printLength = [](QStringView sv) {
        qDebug() << "Length:" << sv.length();
    };

    printLength("literal");  // const char* 隐式转换
    printLength(str);        // QString
    printLength(view);       // QStringView

    // 常用操作
    bool empty = view.isEmpty();
    int len = view.length();
    QChar first = view.front();
    bool starts = view.startsWith("He");

    // 转换为 QString（需要复制）
    QString copy = view.toString();
}
```

---

## 2. QByteArray

### 2.1 QByteArray 基础

```cpp
#include <QByteArray>

void byteArrayBasics() {
    // 构造
    QByteArray ba1("Hello");
    QByteArray ba2 = QByteArray::fromHex("48656c6c6f");  // "Hello"
    QByteArray ba3 = QByteArray::fromBase64("SGVsbG8="); // "Hello"
    QByteArray ba4(10, 'x');  // "xxxxxxxxxx"

    // 操作
    ba1.append(" World");
    ba1.prepend("Say: ");
    ba1.insert(4, "!!!");

    // 查找
    int pos = ba1.indexOf("World");
    bool contains = ba1.contains("Hello");

    // 编码转换
    QString str = QString::fromUtf8(ba1);
    QByteArray utf8 = str.toUtf8();
    QByteArray base64 = ba1.toBase64();
    QByteArray hex = ba1.toHex();

    // 压缩
    QByteArray compressed = qCompress(ba1);
    QByteArray decompressed = qUncompress(compressed);

    // 原始数据访问
    const char* data = ba1.constData();
    char* mutableData = ba1.data();
    int size = ba1.size();

    // 数字转换
    QByteArray numBa = QByteArray::number(42);
    QByteArray floatBa = QByteArray::number(3.14, 'f', 2);
    int num = QByteArray("42").toInt();
}
```

---

## 3. Qt 容器

### 3.1 QList

**面试问题**：QList 和 std::vector 有什么区别？

**答案**：

```cpp
#include <QList>

void qlistDemo() {
    // Qt 6 中 QList 等同于 QVector
    QList<int> list;

    // 添加元素
    list.append(1);
    list.push_back(2);
    list << 3 << 4 << 5;
    list.prepend(0);
    list.insert(3, 100);

    // 访问
    int first = list.first();
    int last = list.last();
    int at2 = list.at(2);
    int& ref = list[2];

    // 查找
    int index = list.indexOf(100);
    bool contains = list.contains(100);
    int count = list.count(1);

    // 删除
    list.removeAt(3);
    list.removeFirst();
    list.removeLast();
    list.removeOne(100);  // 删除第一个匹配项
    list.removeAll(1);    // 删除所有匹配项

    // 迭代
    for (int val : list) {
        qDebug() << val;
    }

    // Java 风格迭代器
    QListIterator<int> it(list);
    while (it.hasNext()) {
        qDebug() << it.next();
    }

    // 可变迭代器
    QMutableListIterator<int> mit(list);
    while (mit.hasNext()) {
        if (mit.next() % 2 == 0) {
            mit.remove();
        }
    }

    // 算法
    std::sort(list.begin(), list.end());
    bool found = std::binary_search(list.begin(), list.end(), 3);
}
```

---

### 3.2 QHash 和 QMap

**面试问题**：QHash 和 QMap 什么时候用哪个？

**答案**：

| 特性 | QHash | QMap |
|------|-------|------|
| 底层结构 | 哈希表 | 红黑树 |
| 查找复杂度 | O(1) 平均 | O(log n) |
| 有序性 | 无序 | 键有序 |
| 内存 | 较多 | 较少 |
| 适用场景 | 快速查找 | 需要排序遍历 |

```cpp
#include <QHash>
#include <QMap>

void hashMapDemo() {
    // QHash - 无序
    QHash<QString, int> hash;
    hash["one"] = 1;
    hash["two"] = 2;
    hash.insert("three", 3);

    int value = hash.value("one", -1);  // 默认值 -1
    bool contains = hash.contains("two");

    // 遍历（顺序不确定）
    for (auto it = hash.begin(); it != hash.end(); ++it) {
        qDebug() << it.key() << it.value();
    }

    // QMap - 有序
    QMap<QString, int> map;
    map["banana"] = 2;
    map["apple"] = 1;
    map["cherry"] = 3;

    // 遍历（按键排序）
    for (const QString& key : map.keys()) {
        qDebug() << key << map[key];  // apple, banana, cherry
    }

    // 范围查找（QMap 特有）
    auto lower = map.lowerBound("b");  // 指向 "banana"
    auto upper = map.upperBound("b");  // 指向 "cherry"

    // 多值映射
    QMultiHash<QString, int> multiHash;
    multiHash.insert("key", 1);
    multiHash.insert("key", 2);
    QList<int> values = multiHash.values("key");  // {1, 2}

    QMultiMap<QString, int> multiMap;
    multiMap.insert("key", 1);
    multiMap.insert("key", 2);
}
```

---

### 3.3 QSet

```cpp
#include <QSet>

void qsetDemo() {
    QSet<QString> set;
    set.insert("apple");
    set.insert("banana");
    set << "cherry" << "apple";  // 重复元素被忽略

    bool contains = set.contains("apple");
    int size = set.size();

    // 集合运算
    QSet<QString> set2 = {"banana", "date", "elderberry"};

    QSet<QString> unionSet = set | set2;      // 并集
    QSet<QString> intersect = set & set2;     // 交集
    QSet<QString> diff = set - set2;          // 差集

    set.unite(set2);        // 原地并集
    set.intersect(set2);    // 原地交集
    set.subtract(set2);     // 原地差集

    // 转换为 QList
    QList<QString> list = set.values();
}
```

---

### 3.4 QVector（Qt 5）/ QList（Qt 6）

```cpp
void vectorDemo() {
    // Qt 6 中 QVector 是 QList 的别名
    QVector<double> vec;
    vec.reserve(100);  // 预分配

    vec.append(1.0);
    vec << 2.0 << 3.0;

    // 直接访问内存
    double* data = vec.data();

    // 调整大小
    vec.resize(10);
    vec.fill(0.0);  // 填充

    // 从 STL 容器转换
    std::vector<double> stdVec = {1.0, 2.0, 3.0};
    QVector<double> fromStd = QVector<double>(stdVec.begin(), stdVec.end());

    // 转换为 STL
    std::vector<double> toStd(vec.begin(), vec.end());
}
```

---

## 4. 隐式共享

### 4.1 Copy-on-Write 机制

**面试问题**：什么是 Qt 的隐式共享？

**答案**：

```cpp
void implicitSharingDemo() {
    // 隐式共享：复制时只复制指针，修改时才复制数据

    QString s1 = "Hello World, this is a long string";
    QString s2 = s1;  // 不复制数据，共享内部数据

    qDebug() << "s1 data:" << (void*)s1.constData();
    qDebug() << "s2 data:" << (void*)s2.constData();
    // 两者指向同一内存

    s2[0] = 'h';  // 写时复制，s2 现在有自己的副本

    qDebug() << "After modification:";
    qDebug() << "s1 data:" << (void*)s1.constData();
    qDebug() << "s2 data:" << (void*)s2.constData();
    // 现在指向不同内存

    // 所有 Qt 容器都支持隐式共享：
    // QString, QByteArray, QList, QVector, QHash, QMap, etc.
}

// 线程安全性
void threadSafetyDemo() {
    // 隐式共享本身是线程安全的（原子引用计数）
    // 但容器的修改操作不是线程安全的

    // 安全：
    QString shared = "data";
    QString copy = shared;  // 线程安全的复制

    // 不安全：同时修改
    // thread1: shared[0] = 'x';  // 不安全
    // thread2: shared.append("y"); // 不安全
}

// 优化：避免不必要的拷贝
void optimizationTips() {
    QList<QString> list;

    // 不好：返回值触发隐式共享检查
    for (int i = 0; i < list.size(); ++i) {
        QString s = list[i];  // 可能触发 detach
        process(s);
    }

    // 好：使用 const 引用
    for (int i = 0; i < list.size(); ++i) {
        const QString& s = list.at(i);  // 不会 detach
        process(s);
    }

    // 更好：范围 for（自动使用 const 引用）
    for (const QString& s : list) {
        process(s);
    }

    // 最好（如果需要修改）：
    for (QString& s : list) {
        modify(s);
    }
}
```

---

## 5. 正则表达式

### 5.1 QRegularExpression

```cpp
#include <QRegularExpression>

void regexDemo() {
    QString text = "The email is alice@example.com and bob@test.org";

    // 创建正则表达式
    QRegularExpression re(R"(\b[\w.]+@[\w.]+\.\w+\b)");

    // 检查有效性
    if (!re.isValid()) {
        qDebug() << "Invalid regex:" << re.errorString();
        return;
    }

    // 单次匹配
    QRegularExpressionMatch match = re.match(text);
    if (match.hasMatch()) {
        qDebug() << "Found:" << match.captured(0);
        qDebug() << "Start:" << match.capturedStart();
        qDebug() << "End:" << match.capturedEnd();
    }

    // 全局匹配
    QRegularExpressionMatchIterator it = re.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        qDebug() << "Email:" << m.captured(0);
    }

    // 捕获组
    QRegularExpression reGroup(R"((\w+)@(\w+)\.(\w+))");
    QRegularExpressionMatch groupMatch = reGroup.match("alice@example.com");
    if (groupMatch.hasMatch()) {
        qDebug() << "Full:" << groupMatch.captured(0);   // alice@example.com
        qDebug() << "User:" << groupMatch.captured(1);   // alice
        qDebug() << "Domain:" << groupMatch.captured(2); // example
        qDebug() << "TLD:" << groupMatch.captured(3);    // com
    }

    // 命名捕获组
    QRegularExpression reNamed(R"((?<user>\w+)@(?<domain>\w+\.\w+))");
    QRegularExpressionMatch namedMatch = reNamed.match("alice@example.com");
    qDebug() << "User:" << namedMatch.captured("user");
    qDebug() << "Domain:" << namedMatch.captured("domain");

    // 替换
    QString replaced = text;
    replaced.replace(re, "[EMAIL]");

    // 分割
    QStringList parts = text.split(QRegularExpression("\\s+"));
}
```

---

## 6. 常见面试题

### Q1: QString::arg() vs sprintf/format 哪个更好？

**答案**：

```cpp
void argVsSprintf() {
    // QString::arg() 优势：
    // 1. 类型安全
    QString safe = QString("Value: %1").arg(42);

    // 2. 支持重排序（国际化友好）
    QString english = QString("From %1 to %2").arg("A").arg("B");
    QString other = QString("到 %2 从 %1").arg("A").arg("B");  // 日语顺序

    // 3. Unicode 安全
    QString unicode = QString("名字: %1").arg("张三");

    // sprintf 问题：
    // - 类型不安全
    // - 不支持 Unicode
    // - 缓冲区溢出风险
}
```

---

### Q2: QList vs QVector vs std::vector？

**答案**：

```cpp
// Qt 6：QList == QVector（底层都是连续存储）
// Qt 5：
// - QVector：连续存储，类似 std::vector
// - QList：存储指针数组（对大对象高效）

// 推荐：
// - Qt 6：始终使用 QList
// - Qt 5：小对象用 QVector，大对象用 QList
// - 需要 STL 兼容：用 std::vector
```

---

### Q3: 容器的线程安全性？

**答案**：

```cpp
void containerThreadSafety() {
    // Qt 容器不是线程安全的

    // 安全操作：
    // 1. 只读访问可以多线程同时进行
    // 2. 隐式共享的复制是原子的

    // 不安全操作：
    // 1. 一个线程写，其他线程读
    // 2. 多个线程同时写

    // 解决方案：
    // 1. 使用锁
    QMutex mutex;
    QList<int> list;

    auto addItem = [&](int item) {
        QMutexLocker locker(&mutex);
        list.append(item);
    };

    // 2. 使用线程安全的容器（Qt 没有，需自己实现或用第三方）
    // 3. 复制后在各自线程使用
}
```

---

## 总结

Qt 字符串与容器的核心要点：

1. **QString** - UTF-16 编码，隐式共享，丰富的字符串操作
2. **QByteArray** - 字节数组，用于二进制数据
3. **QList/QVector** - 动态数组，Qt 6 中等价
4. **QHash/QMap** - 键值对容器，无序/有序
5. **隐式共享** - COW 机制，高效复制
6. **QRegularExpression** - 正则表达式支持

使用 Qt 容器可以获得与 Qt 框架的良好集成和隐式共享带来的性能优势。
