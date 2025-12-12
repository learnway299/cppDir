# Qt 网络与 IO（Network & IO）

## 概述

Qt 提供了完整的网络和文件 IO 支持，包括高级的 HTTP 客户端、底层的 TCP/UDP socket，以及统一的文件操作接口。

---

## 1. 文件操作

### 1.1 QFile 基础

**面试问题**：如何使用 QFile 进行文件读写？

**答案**：

```cpp
#include <QFile>
#include <QTextStream>
#include <QDataStream>

void fileBasics() {
    // 读取文本文件
    QFile file("data.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);

        while (!in.atEnd()) {
            QString line = in.readLine();
            qDebug() << line;
        }
        file.close();
    }

    // 写入文本文件
    QFile outFile("output.txt");
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&outFile);
        out << "Hello, Qt!" << Qt::endl;
        out << "Line 2" << Qt::endl;
        outFile.close();
    }

    // 追加模式
    QFile appendFile("log.txt");
    if (appendFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&appendFile);
        out << QDateTime::currentDateTime().toString() << " - Log entry\n";
    }

    // 一次性读取全部内容
    QFile readAll("small.txt");
    if (readAll.open(QIODevice::ReadOnly)) {
        QByteArray data = readAll.readAll();
        QString text = QString::fromUtf8(data);
    }
}

// 二进制文件操作
void binaryFileIO() {
    // 写入二进制数据
    QFile binFile("data.bin");
    if (binFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&binFile);
        out.setVersion(QDataStream::Qt_6_0);

        // 写入各种类型
        out << qint32(42);
        out << QString("Hello");
        out << QDateTime::currentDateTime();
        out << QList<int>{1, 2, 3, 4, 5};
    }

    // 读取二进制数据
    if (binFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&binFile);
        in.setVersion(QDataStream::Qt_6_0);

        qint32 number;
        QString text;
        QDateTime time;
        QList<int> list;

        in >> number >> text >> time >> list;
    }
}
```

---

### 1.2 QDir 和 QFileInfo

**面试问题**：如何遍历目录和获取文件信息？

**答案**：

```cpp
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>

void directoryOperations() {
    QDir dir("/home/user/documents");

    // 检查目录是否存在
    if (!dir.exists()) {
        dir.mkpath(".");  // 创建目录
    }

    // 列出文件
    QStringList files = dir.entryList(QDir::Files);
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // 按过滤器列出
    QStringList txtFiles = dir.entryList({"*.txt", "*.md"}, QDir::Files);

    // 获取详细信息
    for (const QFileInfo& info : dir.entryInfoList(QDir::Files)) {
        qDebug() << "Name:" << info.fileName();
        qDebug() << "Size:" << info.size();
        qDebug() << "Modified:" << info.lastModified();
        qDebug() << "Is readable:" << info.isReadable();
        qDebug() << "Absolute path:" << info.absoluteFilePath();
    }

    // 递归遍历
    QDirIterator it(dir.path(), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        qDebug() << filePath;
    }

    // 带过滤的递归遍历
    QDirIterator filtered(dir.path(), {"*.cpp", "*.h"},
                          QDir::Files, QDirIterator::Subdirectories);

    // 目录操作
    dir.mkdir("subdir");           // 创建子目录
    dir.rename("old.txt", "new.txt"); // 重命名
    dir.remove("file.txt");         // 删除文件
    dir.rmdir("emptydir");          // 删除空目录
}

void fileInfoDemo() {
    QFileInfo info("/path/to/file.txt");

    // 路径信息
    qDebug() << "Absolute path:" << info.absoluteFilePath();
    qDebug() << "Canonical path:" << info.canonicalFilePath();  // 解析符号链接
    qDebug() << "Directory:" << info.absoluteDir().path();
    qDebug() << "File name:" << info.fileName();
    qDebug() << "Base name:" << info.baseName();       // "file"
    qDebug() << "Complete base:" << info.completeBaseName(); // "file" (不含最后的后缀)
    qDebug() << "Suffix:" << info.suffix();            // "txt"
    qDebug() << "Complete suffix:" << info.completeSuffix(); // "txt"

    // 类型检查
    qDebug() << "Exists:" << info.exists();
    qDebug() << "Is file:" << info.isFile();
    qDebug() << "Is dir:" << info.isDir();
    qDebug() << "Is symlink:" << info.isSymLink();
    qDebug() << "Is hidden:" << info.isHidden();

    // 权限
    qDebug() << "Is readable:" << info.isReadable();
    qDebug() << "Is writable:" << info.isWritable();
    qDebug() << "Is executable:" << info.isExecutable();

    // 时间
    qDebug() << "Created:" << info.birthTime();
    qDebug() << "Modified:" << info.lastModified();
    qDebug() << "Read:" << info.lastRead();
}
```

---

### 1.3 QSaveFile 和临时文件

**面试问题**：如何安全地保存文件？

**答案**：

```cpp
#include <QSaveFile>
#include <QTemporaryFile>
#include <QTemporaryDir>

void safeFileSave() {
    // QSaveFile：原子写入，失败时不会损坏原文件
    QSaveFile file("important.dat");

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << QString("Important data");

        // commit() 才真正写入，之前可以 cancelWriting()
        if (!file.commit()) {
            qWarning() << "Failed to save file";
        }
    }
}

void temporaryFiles() {
    // 临时文件
    QTemporaryFile tempFile;
    if (tempFile.open()) {
        qDebug() << "Temp file:" << tempFile.fileName();
        tempFile.write("Temporary data");
        // 文件在对象销毁时自动删除
    }

    // 保留临时文件
    QTemporaryFile keepFile;
    keepFile.setAutoRemove(false);
    if (keepFile.open()) {
        // 文件不会被自动删除
    }

    // 指定模板
    QTemporaryFile namedTemp("myapp_XXXXXX.tmp");
    namedTemp.open();
    // 文件名类似 "myapp_abc123.tmp"

    // 临时目录
    QTemporaryDir tempDir;
    if (tempDir.isValid()) {
        qDebug() << "Temp dir:" << tempDir.path();
        // 可以在其中创建文件
        QFile::copy("source.txt", tempDir.filePath("copy.txt"));
    }
}
```

---

## 2. 网络编程

### 2.1 QNetworkAccessManager

**面试问题**：如何使用 Qt 进行 HTTP 请求？

**答案**：

```cpp
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class HttpClient : public QObject {
    Q_OBJECT

public:
    HttpClient(QObject* parent = nullptr)
        : QObject(parent)
        , m_manager(new QNetworkAccessManager(this))
    {}

    // GET 请求
    void get(const QUrl& url) {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "MyApp/1.0");

        QNetworkReply* reply = m_manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            handleReply(reply);
        });

        connect(reply, &QNetworkReply::downloadProgress,
                this, &HttpClient::onProgress);
    }

    // POST 请求
    void post(const QUrl& url, const QByteArray& data) {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/json");

        QNetworkReply* reply = m_manager->post(request, data);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            handleReply(reply);
        });
    }

    // POST 表单数据
    void postForm(const QUrl& url, const QUrlQuery& formData) {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/x-www-form-urlencoded");

        QNetworkReply* reply = m_manager->post(request,
                                               formData.toString().toUtf8());
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            handleReply(reply);
        });
    }

    // 上传文件
    void uploadFile(const QUrl& url, const QString& filePath) {
        QFile* file = new QFile(filePath, this);
        if (!file->open(QIODevice::ReadOnly)) {
            emit error("Cannot open file");
            return;
        }

        QNetworkRequest request(url);
        QNetworkReply* reply = m_manager->post(request, file);
        file->setParent(reply);  // 自动清理

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            handleReply(reply);
        });
    }

    // 下载文件
    void downloadFile(const QUrl& url, const QString& savePath) {
        QNetworkRequest request(url);
        QNetworkReply* reply = m_manager->get(request);

        QFile* file = new QFile(savePath, reply);
        if (!file->open(QIODevice::WriteOnly)) {
            reply->abort();
            emit error("Cannot create file");
            return;
        }

        connect(reply, &QNetworkReply::readyRead, [reply, file]() {
            file->write(reply->readAll());
        });

        connect(reply, &QNetworkReply::finished, this, [this, reply, file]() {
            file->close();
            if (reply->error() == QNetworkReply::NoError) {
                emit downloadComplete();
            } else {
                emit error(reply->errorString());
            }
            reply->deleteLater();
        });
    }

signals:
    void finished(const QByteArray& data);
    void error(const QString& message);
    void downloadComplete();

private slots:
    void handleReply(QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            emit finished(reply->readAll());
        } else {
            emit error(reply->errorString());
        }
        reply->deleteLater();
    }

    void onProgress(qint64 received, qint64 total) {
        if (total > 0) {
            qDebug() << "Progress:" << (received * 100 / total) << "%";
        }
    }

private:
    QNetworkAccessManager* m_manager;
};
```

---

### 2.2 QTcpSocket

**面试问题**：如何使用 Qt 进行 TCP 通信？

**答案**：

```cpp
#include <QTcpSocket>
#include <QTcpServer>

// TCP 客户端
class TcpClient : public QObject {
    Q_OBJECT

public:
    TcpClient(QObject* parent = nullptr)
        : QObject(parent)
        , m_socket(new QTcpSocket(this))
    {
        connect(m_socket, &QTcpSocket::connected,
                this, &TcpClient::onConnected);
        connect(m_socket, &QTcpSocket::disconnected,
                this, &TcpClient::onDisconnected);
        connect(m_socket, &QTcpSocket::readyRead,
                this, &TcpClient::onReadyRead);
        connect(m_socket, &QTcpSocket::errorOccurred,
                this, &TcpClient::onError);
    }

    void connectToServer(const QString& host, quint16 port) {
        m_socket->connectToHost(host, port);
    }

    void send(const QByteArray& data) {
        if (m_socket->state() == QAbstractSocket::ConnectedState) {
            m_socket->write(data);
        }
    }

    void disconnect() {
        m_socket->disconnectFromHost();
    }

private slots:
    void onConnected() {
        qDebug() << "Connected to server";
    }

    void onDisconnected() {
        qDebug() << "Disconnected from server";
    }

    void onReadyRead() {
        QByteArray data = m_socket->readAll();
        emit dataReceived(data);
    }

    void onError(QAbstractSocket::SocketError error) {
        qDebug() << "Socket error:" << m_socket->errorString();
    }

signals:
    void dataReceived(const QByteArray& data);

private:
    QTcpSocket* m_socket;
};

// TCP 服务器
class TcpServer : public QObject {
    Q_OBJECT

public:
    TcpServer(QObject* parent = nullptr)
        : QObject(parent)
        , m_server(new QTcpServer(this))
    {
        connect(m_server, &QTcpServer::newConnection,
                this, &TcpServer::onNewConnection);
    }

    bool start(quint16 port) {
        return m_server->listen(QHostAddress::Any, port);
    }

    void broadcast(const QByteArray& data) {
        for (QTcpSocket* client : m_clients) {
            client->write(data);
        }
    }

private slots:
    void onNewConnection() {
        QTcpSocket* client = m_server->nextPendingConnection();
        m_clients.append(client);

        connect(client, &QTcpSocket::readyRead, this, [this, client]() {
            QByteArray data = client->readAll();
            emit clientData(client, data);
        });

        connect(client, &QTcpSocket::disconnected, this, [this, client]() {
            m_clients.removeOne(client);
            client->deleteLater();
        });

        emit clientConnected(client);
    }

signals:
    void clientConnected(QTcpSocket* client);
    void clientData(QTcpSocket* client, const QByteArray& data);

private:
    QTcpServer* m_server;
    QList<QTcpSocket*> m_clients;
};
```

---

### 2.3 QUdpSocket

**面试问题**：如何使用 Qt 进行 UDP 通信？

**答案**：

```cpp
#include <QUdpSocket>

class UdpCommunicator : public QObject {
    Q_OBJECT

public:
    UdpCommunicator(QObject* parent = nullptr)
        : QObject(parent)
        , m_socket(new QUdpSocket(this))
    {
        connect(m_socket, &QUdpSocket::readyRead,
                this, &UdpCommunicator::onReadyRead);
    }

    bool bind(quint16 port) {
        return m_socket->bind(QHostAddress::Any, port);
    }

    void send(const QByteArray& data, const QHostAddress& host, quint16 port) {
        m_socket->writeDatagram(data, host, port);
    }

    // 广播
    void broadcast(const QByteArray& data, quint16 port) {
        m_socket->writeDatagram(data, QHostAddress::Broadcast, port);
    }

    // 组播
    void joinMulticastGroup(const QHostAddress& group) {
        m_socket->joinMulticastGroup(group);
    }

    void leaveMulticastGroup(const QHostAddress& group) {
        m_socket->leaveMulticastGroup(group);
    }

private slots:
    void onReadyRead() {
        while (m_socket->hasPendingDatagrams()) {
            QByteArray data;
            data.resize(m_socket->pendingDatagramSize());

            QHostAddress sender;
            quint16 senderPort;

            m_socket->readDatagram(data.data(), data.size(),
                                   &sender, &senderPort);

            emit dataReceived(data, sender, senderPort);
        }
    }

signals:
    void dataReceived(const QByteArray& data,
                      const QHostAddress& sender, quint16 port);

private:
    QUdpSocket* m_socket;
};
```

---

## 3. 异步 IO

### 3.1 QIODevice 接口

**面试问题**：QIODevice 提供了哪些异步操作支持？

**答案**：

```cpp
#include <QIODevice>

// QIODevice 是所有 IO 设备的基类
// QFile, QTcpSocket, QUdpSocket, QProcess 等都继承自它

void asyncIODemo() {
    QTcpSocket* socket = new QTcpSocket();

    // 异步读取信号
    QObject::connect(socket, &QIODevice::readyRead, [socket]() {
        // 有数据可读
        while (socket->bytesAvailable() > 0) {
            // 读取一行
            if (socket->canReadLine()) {
                QByteArray line = socket->readLine();
            }
            // 或读取指定数量
            QByteArray data = socket->read(1024);
            // 或读取全部
            QByteArray all = socket->readAll();
        }
    });

    // 异步写入
    QObject::connect(socket, &QIODevice::bytesWritten, [](qint64 bytes) {
        qDebug() << "Written:" << bytes << "bytes";
    });

    // 检查可写
    if (socket->isWritable()) {
        qint64 written = socket->write("Hello");
        // write 是非阻塞的，数据进入缓冲区
    }

    // 等待数据（阻塞，慎用）
    if (socket->waitForReadyRead(3000)) {
        QByteArray data = socket->readAll();
    }
}
```

---

### 3.2 协议解析

**面试问题**：如何处理 TCP 粘包问题？

**答案**：

```cpp
// 方案 1：固定长度消息
class FixedLengthProtocol : public QObject {
    Q_OBJECT

public:
    static const int MESSAGE_SIZE = 100;

    void onDataReceived(QTcpSocket* socket) {
        m_buffer.append(socket->readAll());

        while (m_buffer.size() >= MESSAGE_SIZE) {
            QByteArray message = m_buffer.left(MESSAGE_SIZE);
            m_buffer.remove(0, MESSAGE_SIZE);
            processMessage(message);
        }
    }

private:
    QByteArray m_buffer;
};

// 方案 2：长度前缀
class LengthPrefixProtocol : public QObject {
    Q_OBJECT

public:
    void onDataReceived(QTcpSocket* socket) {
        m_buffer.append(socket->readAll());

        while (true) {
            if (m_buffer.size() < 4) break;  // 需要至少 4 字节长度头

            // 读取长度（大端序）
            QDataStream stream(m_buffer);
            stream.setByteOrder(QDataStream::BigEndian);

            quint32 length;
            stream >> length;

            if (m_buffer.size() < 4 + length) break;  // 数据不完整

            // 提取完整消息
            QByteArray message = m_buffer.mid(4, length);
            m_buffer.remove(0, 4 + length);

            processMessage(message);
        }
    }

    void sendMessage(QTcpSocket* socket, const QByteArray& data) {
        QByteArray packet;
        QDataStream stream(&packet, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);

        stream << quint32(data.size());
        packet.append(data);

        socket->write(packet);
    }

private:
    QByteArray m_buffer;
};

// 方案 3：分隔符
class DelimiterProtocol : public QObject {
    Q_OBJECT

public:
    void onDataReceived(QTcpSocket* socket) {
        m_buffer.append(socket->readAll());

        int pos;
        while ((pos = m_buffer.indexOf('\n')) != -1) {
            QByteArray message = m_buffer.left(pos);
            m_buffer.remove(0, pos + 1);
            processMessage(message);
        }
    }

private:
    QByteArray m_buffer;
};
```

---

## 4. SSL/TLS 支持

### 4.1 QSslSocket

**面试问题**：如何在 Qt 中使用 SSL/TLS？

**答案**：

```cpp
#include <QSslSocket>
#include <QSslConfiguration>

class SecureClient : public QObject {
    Q_OBJECT

public:
    SecureClient(QObject* parent = nullptr)
        : QObject(parent)
        , m_socket(new QSslSocket(this))
    {
        // SSL 配置
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::TlsV1_2OrLater);
        m_socket->setSslConfiguration(config);

        // 连接信号
        connect(m_socket, &QSslSocket::encrypted,
                this, &SecureClient::onEncrypted);
        connect(m_socket, &QSslSocket::sslErrors,
                this, &SecureClient::onSslErrors);
        connect(m_socket, &QSslSocket::readyRead,
                this, &SecureClient::onReadyRead);
    }

    void connectToSecureHost(const QString& host, quint16 port = 443) {
        m_socket->connectToHostEncrypted(host, port);
    }

    void send(const QByteArray& data) {
        m_socket->write(data);
    }

private slots:
    void onEncrypted() {
        qDebug() << "Connection encrypted";
        qDebug() << "Cipher:" << m_socket->sessionCipher().name();
        qDebug() << "Protocol:" << m_socket->sessionProtocol();
    }

    void onSslErrors(const QList<QSslError>& errors) {
        for (const QSslError& error : errors) {
            qDebug() << "SSL Error:" << error.errorString();
        }

        // 忽略证书错误（仅用于测试！）
        // m_socket->ignoreSslErrors();
    }

    void onReadyRead() {
        emit dataReceived(m_socket->readAll());
    }

signals:
    void dataReceived(const QByteArray& data);

private:
    QSslSocket* m_socket;
};

// SSL 服务器
class SecureServer : public QObject {
    Q_OBJECT

public:
    SecureServer(QObject* parent = nullptr)
        : QObject(parent)
        , m_server(new QTcpServer(this))
    {
        connect(m_server, &QTcpServer::newConnection,
                this, &SecureServer::onNewConnection);
    }

    bool start(quint16 port) {
        return m_server->listen(QHostAddress::Any, port);
    }

    void setCertificate(const QString& certFile, const QString& keyFile) {
        m_certificate = QSslCertificate::fromPath(certFile).first();
        QFile file(keyFile);
        file.open(QIODevice::ReadOnly);
        m_privateKey = QSslKey(&file, QSsl::Rsa);
    }

private slots:
    void onNewConnection() {
        QTcpSocket* tcpSocket = m_server->nextPendingConnection();

        QSslSocket* sslSocket = new QSslSocket(this);
        sslSocket->setSocketDescriptor(tcpSocket->socketDescriptor());
        sslSocket->setLocalCertificate(m_certificate);
        sslSocket->setPrivateKey(m_privateKey);

        connect(sslSocket, &QSslSocket::encrypted, [sslSocket]() {
            qDebug() << "Client connected securely";
        });

        sslSocket->startServerEncryption();
        tcpSocket->deleteLater();
    }

private:
    QTcpServer* m_server;
    QSslCertificate m_certificate;
    QSslKey m_privateKey;
};
```

---

## 5. 其他网络功能

### 5.1 网络信息

```cpp
#include <QNetworkInterface>
#include <QHostInfo>

void networkInfo() {
    // 获取所有网络接口
    for (const QNetworkInterface& iface : QNetworkInterface::allInterfaces()) {
        qDebug() << "Interface:" << iface.name();
        qDebug() << "  Hardware address:" << iface.hardwareAddress();

        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            qDebug() << "  IP:" << entry.ip().toString();
            qDebug() << "  Netmask:" << entry.netmask().toString();
        }
    }

    // DNS 查询
    QHostInfo::lookupHost("www.example.com", nullptr, [](const QHostInfo& info) {
        if (info.error() == QHostInfo::NoError) {
            for (const QHostAddress& addr : info.addresses()) {
                qDebug() << "IP:" << addr.toString();
            }
        }
    });

    // 同步 DNS 查询（阻塞）
    QHostInfo info = QHostInfo::fromName("www.example.com");
}
```

---

### 5.2 WebSocket

```cpp
#include <QWebSocket>
#include <QWebSocketServer>

class WebSocketClient : public QObject {
    Q_OBJECT

public:
    WebSocketClient(QObject* parent = nullptr)
        : QObject(parent)
        , m_socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    {
        connect(m_socket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
        connect(m_socket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
        connect(m_socket, &QWebSocket::textMessageReceived,
                this, &WebSocketClient::onTextMessage);
        connect(m_socket, &QWebSocket::binaryMessageReceived,
                this, &WebSocketClient::onBinaryMessage);
    }

    void connect(const QUrl& url) {
        m_socket->open(url);
    }

    void sendText(const QString& message) {
        m_socket->sendTextMessage(message);
    }

    void sendBinary(const QByteArray& data) {
        m_socket->sendBinaryMessage(data);
    }

private slots:
    void onConnected() {
        qDebug() << "WebSocket connected";
    }

    void onDisconnected() {
        qDebug() << "WebSocket disconnected";
    }

    void onTextMessage(const QString& message) {
        emit textReceived(message);
    }

    void onBinaryMessage(const QByteArray& data) {
        emit binaryReceived(data);
    }

signals:
    void textReceived(const QString& message);
    void binaryReceived(const QByteArray& data);

private:
    QWebSocket* m_socket;
};
```

---

## 6. 常见面试题

### Q1: QFile 和 std::fstream 哪个更好？

**答案**：

| 特性 | QFile | std::fstream |
|------|-------|--------------|
| 跨平台 | ✅ 自动处理 | ⚠️ 需注意编码 |
| Unicode 路径 | ✅ 原生支持 | ⚠️ Windows 需特殊处理 |
| Qt 集成 | ✅ 信号槽、资源系统 | ❌ |
| 性能 | 相当 | 相当 |
| 便捷性 | ✅ QTextStream/QDataStream | ⚠️ 较繁琐 |

---

### Q2: TCP 和 UDP 什么时候用？

**答案**：

| 场景 | 推荐 | 原因 |
|------|------|------|
| 可靠传输 | TCP | 保证送达、顺序 |
| 实时游戏 | UDP | 低延迟优先 |
| 视频流 | UDP | 可容忍丢包 |
| 文件传输 | TCP | 需要完整性 |
| 聊天消息 | TCP | 需要可靠 |
| 广播发现 | UDP | 支持广播 |

---

### Q3: 如何处理网络超时？

**答案**：

```cpp
void handleTimeout(QNetworkAccessManager* manager) {
    QNetworkRequest request(QUrl("http://example.com"));
    request.setTransferTimeout(5000);  // Qt 5.15+

    QNetworkReply* reply = manager->get(request);

    // 或使用 QTimer
    QTimer* timer = new QTimer();
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [reply]() {
        if (reply->isRunning()) {
            reply->abort();
        }
    });
    timer->start(5000);

    QObject::connect(reply, &QNetworkReply::finished, timer, &QTimer::stop);
}
```

---

## 总结

Qt 网络与 IO 的核心要点：

1. **文件操作** - QFile、QDir、QFileInfo
2. **HTTP 客户端** - QNetworkAccessManager
3. **TCP 通信** - QTcpSocket、QTcpServer
4. **UDP 通信** - QUdpSocket
5. **SSL/TLS** - QSslSocket
6. **异步模式** - 基于信号槽的非阻塞 IO

Qt 的网络 IO 设计遵循异步模式，与事件循环紧密集成，适合构建高响应性的网络应用。
