# Qt 高级主题（Advanced Topics）

## 概述

本文涵盖 Qt 的高级特性，包括插件系统、资源管理、进程间通信、数据库访问等，这些是构建复杂 Qt 应用的重要组成部分。

---

## 1. 插件系统

### 1.1 Qt 插件架构

**面试问题**：如何在 Qt 中实现插件系统？

**答案**：

```cpp
// 1. 定义插件接口
// plugin_interface.h
#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QtPlugin>

class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual void execute() = 0;
};

#define PluginInterface_iid "com.myapp.PluginInterface/1.0"
Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)

#endif

// 2. 实现插件
// my_plugin.h
class MyPlugin : public QObject, public PluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "myplugin.json")
    Q_INTERFACES(PluginInterface)

public:
    QString name() const override { return "My Plugin"; }
    QString version() const override { return "1.0.0"; }
    void execute() override {
        qDebug() << "Plugin executed!";
    }
};

// myplugin.json
// {
//     "name": "My Plugin",
//     "version": "1.0.0",
//     "author": "Developer"
// }

// 3. 加载插件
class PluginLoader {
public:
    void loadPlugins(const QString& pluginsDir) {
        QDir dir(pluginsDir);
        for (const QString& fileName : dir.entryList(QDir::Files)) {
            QString filePath = dir.absoluteFilePath(fileName);

            QPluginLoader loader(filePath);

            // 获取元数据（不加载插件）
            QJsonObject metaData = loader.metaData();
            qDebug() << "Plugin metadata:" << metaData;

            // 加载插件
            if (QObject* plugin = loader.instance()) {
                if (PluginInterface* iface = qobject_cast<PluginInterface*>(plugin)) {
                    m_plugins.append(iface);
                    qDebug() << "Loaded:" << iface->name();
                }
            } else {
                qWarning() << "Failed to load:" << loader.errorString();
            }
        }
    }

    void executeAll() {
        for (PluginInterface* plugin : m_plugins) {
            plugin->execute();
        }
    }

private:
    QList<PluginInterface*> m_plugins;
};
```

---

### 1.2 静态插件

```cpp
// 静态链接的插件
// 在插件 .pro 文件中：
// CONFIG += static

// 在主程序中导入静态插件
Q_IMPORT_PLUGIN(MyStaticPlugin)

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 静态插件自动可用
    for (QObject* plugin : QPluginLoader::staticInstances()) {
        if (PluginInterface* iface = qobject_cast<PluginInterface*>(plugin)) {
            qDebug() << "Static plugin:" << iface->name();
        }
    }

    return app.exec();
}
```

---

## 2. 资源系统

### 2.1 Qt 资源文件

**面试问题**：Qt 资源系统是如何工作的？

**答案**：

```xml
<!-- resources.qrc -->
<RCC>
    <qresource prefix="/images">
        <file>logo.png</file>
        <file>icons/save.svg</file>
        <file alias="background">textures/bg_tile.png</file>
    </qresource>
    <qresource prefix="/translations" lang="zh_CN">
        <file>app_zh.qm</file>
    </qresource>
</RCC>
```

```cpp
void resourceDemo() {
    // 访问资源
    QPixmap logo(":/images/logo.png");
    QIcon saveIcon(":/images/icons/save.svg");

    // 使用别名
    QPixmap bg(":/images/background");

    // 检查资源是否存在
    if (QFile::exists(":/images/logo.png")) {
        qDebug() << "Resource exists";
    }

    // 读取资源文件
    QFile file(":/data/config.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
    }

    // 列出资源
    QDir resourceDir(":/images");
    for (const QString& entry : resourceDir.entryList()) {
        qDebug() << entry;
    }

    // 运行时注册资源
    QResource::registerResource("additional.rcc");

    // 卸载资源
    QResource::unregisterResource("additional.rcc");
}

// 大型资源的外部二进制资源
// rcc -binary resources.qrc -o resources.rcc
// 然后运行时加载
```

---

## 3. 进程与进程间通信

### 3.1 QProcess

**面试问题**：如何在 Qt 中执行外部进程？

**答案**：

```cpp
#include <QProcess>

class ProcessManager : public QObject {
    Q_OBJECT

public:
    // 同步执行
    int executeSync(const QString& program, const QStringList& args) {
        QProcess process;
        process.start(program, args);
        process.waitForFinished(-1);

        m_output = process.readAllStandardOutput();
        m_error = process.readAllStandardError();

        return process.exitCode();
    }

    // 异步执行
    void executeAsync(const QString& program, const QStringList& args) {
        QProcess* process = new QProcess(this);

        connect(process, &QProcess::readyReadStandardOutput, [process]() {
            qDebug() << "Output:" << process->readAllStandardOutput();
        });

        connect(process, &QProcess::readyReadStandardError, [process]() {
            qDebug() << "Error:" << process->readAllStandardError();
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [process](int exitCode, QProcess::ExitStatus status) {
            qDebug() << "Finished with code:" << exitCode;
            process->deleteLater();
        });

        connect(process, &QProcess::errorOccurred,
                [](QProcess::ProcessError error) {
            qDebug() << "Process error:" << error;
        });

        process->start(program, args);
    }

    // 管道：将一个进程的输出连接到另一个进程的输入
    void pipeProcesses() {
        QProcess* grep = new QProcess(this);
        QProcess* sort = new QProcess(this);

        grep->setStandardOutputProcess(sort);

        grep->start("grep", {"pattern", "file.txt"});
        sort->start("sort");

        sort->waitForFinished();
        qDebug() << sort->readAllStandardOutput();
    }

    // 分离进程（独立运行）
    void startDetached(const QString& program, const QStringList& args) {
        qint64 pid;
        QProcess::startDetached(program, args, QString(), &pid);
        qDebug() << "Started process with PID:" << pid;
    }

private:
    QByteArray m_output;
    QByteArray m_error;
};
```

---

### 3.2 共享内存

```cpp
#include <QSharedMemory>

// 写入共享内存
class SharedMemoryWriter {
public:
    bool write(const QString& key, const QByteArray& data) {
        QSharedMemory sharedMemory(key);

        // 尝试附加到现有共享内存
        if (sharedMemory.attach()) {
            sharedMemory.detach();
        }

        // 创建共享内存
        if (!sharedMemory.create(data.size())) {
            qWarning() << "Cannot create shared memory:" << sharedMemory.errorString();
            return false;
        }

        // 写入数据
        sharedMemory.lock();
        memcpy(sharedMemory.data(), data.constData(), data.size());
        sharedMemory.unlock();

        return true;
    }
};

// 读取共享内存
class SharedMemoryReader {
public:
    QByteArray read(const QString& key) {
        QSharedMemory sharedMemory(key);

        if (!sharedMemory.attach(QSharedMemory::ReadOnly)) {
            qWarning() << "Cannot attach to shared memory";
            return QByteArray();
        }

        QByteArray data;
        sharedMemory.lock();
        data = QByteArray(static_cast<const char*>(sharedMemory.constData()),
                          sharedMemory.size());
        sharedMemory.unlock();

        sharedMemory.detach();
        return data;
    }
};
```

---

### 3.3 本地 Socket

```cpp
#include <QLocalServer>
#include <QLocalSocket>

// 本地服务器
class LocalServer : public QObject {
    Q_OBJECT

public:
    bool start(const QString& name) {
        m_server = new QLocalServer(this);

        // 移除旧的 socket 文件
        QLocalServer::removeServer(name);

        if (!m_server->listen(name)) {
            qWarning() << "Cannot start server";
            return false;
        }

        connect(m_server, &QLocalServer::newConnection,
                this, &LocalServer::onNewConnection);

        return true;
    }

private slots:
    void onNewConnection() {
        QLocalSocket* client = m_server->nextPendingConnection();

        connect(client, &QLocalSocket::readyRead, [client]() {
            QByteArray data = client->readAll();
            // 处理数据...
            client->write("Response");
        });

        connect(client, &QLocalSocket::disconnected,
                client, &QObject::deleteLater);
    }

private:
    QLocalServer* m_server;
};

// 本地客户端
class LocalClient : public QObject {
    Q_OBJECT

public:
    void connect(const QString& name) {
        m_socket = new QLocalSocket(this);

        QObject::connect(m_socket, &QLocalSocket::connected, []() {
            qDebug() << "Connected to local server";
        });

        QObject::connect(m_socket, &QLocalSocket::readyRead, [this]() {
            QByteArray data = m_socket->readAll();
            emit dataReceived(data);
        });

        m_socket->connectToServer(name);
    }

    void send(const QByteArray& data) {
        m_socket->write(data);
    }

signals:
    void dataReceived(const QByteArray& data);

private:
    QLocalSocket* m_socket;
};
```

---

## 4. 数据库访问

### 4.1 Qt SQL 模块

**面试问题**：如何在 Qt 中访问数据库？

**答案**：

```cpp
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

class DatabaseManager {
public:
    bool connect(const QString& dbPath) {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(dbPath);

        if (!m_db.open()) {
            qWarning() << "Database error:" << m_db.lastError().text();
            return false;
        }

        return true;
    }

    // 执行查询
    QList<QVariantMap> query(const QString& sql, const QVariantList& params = {}) {
        QSqlQuery query(m_db);
        query.prepare(sql);

        for (int i = 0; i < params.size(); ++i) {
            query.bindValue(i, params[i]);
        }

        QList<QVariantMap> results;

        if (!query.exec()) {
            qWarning() << "Query error:" << query.lastError().text();
            return results;
        }

        QSqlRecord record = query.record();
        while (query.next()) {
            QVariantMap row;
            for (int i = 0; i < record.count(); ++i) {
                row[record.fieldName(i)] = query.value(i);
            }
            results.append(row);
        }

        return results;
    }

    // 执行更新（INSERT, UPDATE, DELETE）
    bool execute(const QString& sql, const QVariantList& params = {}) {
        QSqlQuery query(m_db);
        query.prepare(sql);

        for (int i = 0; i < params.size(); ++i) {
            query.bindValue(i, params[i]);
        }

        if (!query.exec()) {
            qWarning() << "Execute error:" << query.lastError().text();
            return false;
        }

        return true;
    }

    // 事务
    bool transaction(std::function<bool()> operations) {
        m_db.transaction();

        if (operations()) {
            return m_db.commit();
        } else {
            m_db.rollback();
            return false;
        }
    }

private:
    QSqlDatabase m_db;
};

// 使用示例
void databaseDemo() {
    DatabaseManager db;
    db.connect("app.db");

    // 创建表
    db.execute("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT NOT NULL,"
               "email TEXT UNIQUE)");

    // 插入数据
    db.execute("INSERT INTO users (name, email) VALUES (?, ?)",
               {"Alice", "alice@example.com"});

    // 查询
    auto users = db.query("SELECT * FROM users WHERE name LIKE ?", {"%Ali%"});
    for (const auto& user : users) {
        qDebug() << user["name"].toString() << user["email"].toString();
    }

    // 事务
    db.transaction([&]() {
        db.execute("UPDATE users SET name = ? WHERE id = ?", {"Bob", 1});
        db.execute("DELETE FROM users WHERE id = ?", {2});
        return true;  // 提交
    });
}
```

---

### 4.2 QSqlTableModel

```cpp
#include <QSqlTableModel>
#include <QTableView>

void sqlModelDemo() {
    QSqlTableModel* model = new QSqlTableModel();
    model->setTable("users");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // 设置过滤和排序
    model->setFilter("active = 1");
    model->setSort(1, Qt::AscendingOrder);

    model->select();

    // 在视图中显示
    QTableView* view = new QTableView();
    view->setModel(model);

    // 修改数据
    model->setData(model->index(0, 1), "New Name");

    // 提交更改
    if (!model->submitAll()) {
        model->revertAll();
    }

    // 插入行
    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 1), "New User");
    model->submitAll();

    // 删除行
    model->removeRow(0);
    model->submitAll();
}
```

---

## 5. 设置与配置

### 5.1 QSettings

**面试问题**：如何在 Qt 中保存应用设置？

**答案**：

```cpp
#include <QSettings>

class AppSettings {
public:
    AppSettings()
        : m_settings("MyCompany", "MyApp")  // 注册表/plist/ini
    {}

    // 读取设置
    QString username() const {
        return m_settings.value("user/name", "Guest").toString();
    }

    QSize windowSize() const {
        return m_settings.value("window/size", QSize(800, 600)).toSize();
    }

    QStringList recentFiles() const {
        return m_settings.value("recent/files").toStringList();
    }

    // 保存设置
    void setUsername(const QString& name) {
        m_settings.setValue("user/name", name);
    }

    void setWindowSize(const QSize& size) {
        m_settings.setValue("window/size", size);
    }

    void setRecentFiles(const QStringList& files) {
        m_settings.setValue("recent/files", files);
    }

    // 分组操作
    void saveWindowState(QMainWindow* window) {
        m_settings.beginGroup("MainWindow");
        m_settings.setValue("geometry", window->saveGeometry());
        m_settings.setValue("state", window->saveState());
        m_settings.endGroup();
    }

    void restoreWindowState(QMainWindow* window) {
        m_settings.beginGroup("MainWindow");
        window->restoreGeometry(m_settings.value("geometry").toByteArray());
        window->restoreState(m_settings.value("state").toByteArray());
        m_settings.endGroup();
    }

    // 清除设置
    void clear() {
        m_settings.clear();
    }

private:
    QSettings m_settings;
};

// 使用 INI 文件
void iniFileSettings() {
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.setValue("database/host", "localhost");
    settings.setValue("database/port", 5432);
    settings.sync();  // 立即写入文件
}
```

---

## 6. 定时器与延迟

### 6.1 QTimer 高级用法

```cpp
#include <QTimer>

class TimerExamples : public QObject {
    Q_OBJECT

public:
    void demonstrateTimers() {
        // 1. 普通定时器
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TimerExamples::onTimeout);
        timer->start(1000);  // 1 秒间隔

        // 2. 单次定时器
        QTimer::singleShot(5000, this, &TimerExamples::onSingleShot);

        // 3. Lambda 单次定时器
        QTimer::singleShot(2000, []() {
            qDebug() << "Lambda executed after 2 seconds";
        });

        // 4. 精确定时器
        QTimer* preciseTimer = new QTimer(this);
        preciseTimer->setTimerType(Qt::PreciseTimer);
        preciseTimer->start(16);  // 约 60 FPS

        // 5. 零延迟定时器（空闲时执行）
        QTimer* idleTimer = new QTimer(this);
        idleTimer->start(0);  // 事件循环空闲时触发
    }

    // 使用 startTimer（更轻量）
    void basicTimerDemo() {
        m_timerId = startTimer(100);  // 100ms
        m_preciseId = startTimer(16, Qt::PreciseTimer);
    }

protected:
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == m_timerId) {
            // 处理定时器事件
        }
    }

private slots:
    void onTimeout() { qDebug() << "Timeout!"; }
    void onSingleShot() { qDebug() << "Single shot!"; }

private:
    int m_timerId = 0;
    int m_preciseId = 0;
};
```

---

## 7. JSON 处理

### 7.1 QJsonDocument

```cpp
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void jsonDemo() {
    // 创建 JSON
    QJsonObject user;
    user["name"] = "Alice";
    user["age"] = 30;
    user["active"] = true;

    QJsonArray hobbies;
    hobbies.append("reading");
    hobbies.append("coding");
    user["hobbies"] = hobbies;

    QJsonObject address;
    address["city"] = "Beijing";
    address["zip"] = "100000";
    user["address"] = address;

    QJsonDocument doc(user);

    // 转换为字符串
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    QByteArray compactJson = doc.toJson(QJsonDocument::Compact);

    // 解析 JSON
    QJsonParseError error;
    QJsonDocument parsed = QJsonDocument::fromJson(jsonString.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return;
    }

    // 读取值
    QJsonObject obj = parsed.object();
    QString name = obj["name"].toString();
    int age = obj["age"].toInt();
    bool active = obj["active"].toBool();

    QJsonArray hobbiesArr = obj["hobbies"].toArray();
    for (const QJsonValue& hobby : hobbiesArr) {
        qDebug() << hobby.toString();
    }

    // 嵌套对象
    QJsonObject addr = obj["address"].toObject();
    QString city = addr["city"].toString();
}
```

---

## 8. 常见面试题

### Q1: 如何实现单实例应用？

**答案**：

```cpp
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>

class SingleApplication : public QApplication {
    Q_OBJECT

public:
    SingleApplication(int& argc, char** argv, const QString& appKey)
        : QApplication(argc, argv)
        , m_sharedMemory(appKey)
    {
        // 尝试附加到现有共享内存
        if (m_sharedMemory.attach()) {
            m_isRunning = true;
            // 通知已运行的实例
            notifyRunningInstance();
        } else {
            // 创建共享内存
            m_sharedMemory.create(1);
            m_isRunning = false;

            // 启动本地服务器监听
            m_localServer = new QLocalServer(this);
            connect(m_localServer, &QLocalServer::newConnection,
                    this, &SingleApplication::onNewConnection);
            m_localServer->listen(appKey);
        }
    }

    bool isRunning() const { return m_isRunning; }

signals:
    void instanceStarted();

private:
    void notifyRunningInstance() {
        QLocalSocket socket;
        socket.connectToServer(m_sharedMemory.key());
        if (socket.waitForConnected(1000)) {
            socket.write("activate");
            socket.waitForBytesWritten();
        }
    }

private slots:
    void onNewConnection() {
        QLocalSocket* socket = m_localServer->nextPendingConnection();
        connect(socket, &QLocalSocket::readyRead, [this, socket]() {
            emit instanceStarted();
            socket->deleteLater();
        });
    }

private:
    QSharedMemory m_sharedMemory;
    QLocalServer* m_localServer = nullptr;
    bool m_isRunning = false;
};

// 使用
int main(int argc, char* argv[]) {
    SingleApplication app(argc, argv, "MyUniqueAppKey");

    if (app.isRunning()) {
        qDebug() << "Application is already running";
        return 0;
    }

    // ... 正常启动
}
```

---

### Q2: QVariant 有什么用？

**答案**：

```cpp
void variantDemo() {
    // QVariant 是类型擦除容器
    QVariant v1 = 42;
    QVariant v2 = "Hello";
    QVariant v3 = QDateTime::currentDateTime();

    // 类型检查
    if (v1.typeId() == QMetaType::Int) {
        int value = v1.toInt();
    }

    // 可以转换类型
    QString str = v1.toString();  // "42"

    // 存储自定义类型
    struct MyData { int x, y; };
    Q_DECLARE_METATYPE(MyData)

    MyData data{10, 20};
    QVariant v4 = QVariant::fromValue(data);
    MyData retrieved = v4.value<MyData>();

    // 用于属性系统
    object->setProperty("customData", v4);

    // 用于 Model/View
    model->setData(index, v4, Qt::UserRole);
}
```

---

## 总结

Qt 高级主题的核心要点：

1. **插件系统** - QPluginLoader 实现动态加载
2. **资源系统** - .qrc 文件嵌入资源
3. **进程通信** - QProcess、QSharedMemory、QLocalSocket
4. **数据库** - QSqlDatabase、QSqlQuery、QSqlTableModel
5. **设置** - QSettings 持久化配置
6. **JSON** - QJsonDocument 处理 JSON 数据

掌握这些高级特性可以构建功能完善的 Qt 应用程序。
