# 07 - Qt WebEngine

## 概述

Qt WebEngine 是 Qt 框架中基于 Chromium 的 Web 渲染引擎，提供与 Qt 信号槽机制深度集成的 Web 嵌入能力。适用于需要在 Qt 应用中嵌入 Web 内容的场景。

---

## 1. Qt WebEngine 架构

### 1.1 组件结构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Qt WebEngine 架构                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        Qt Application                               │   │
│  │  ┌───────────────────────────────────────────────────────────────┐ │   │
│  │  │                    Qt WebEngine 模块                          │ │   │
│  │  │                                                               │ │   │
│  │  │  ┌─────────────────┐  ┌─────────────────┐  ┌───────────────┐ │ │   │
│  │  │  │ QWebEngineView  │  │ QWebEnginePage  │  │QWebEngineProfile││ │   │
│  │  │  │    (Widget)     │  │   (页面逻辑)     │  │   (配置)      │ │ │   │
│  │  │  └─────────────────┘  └─────────────────┘  └───────────────┘ │ │   │
│  │  │                                                               │ │   │
│  │  │  ┌─────────────────┐  ┌─────────────────┐  ┌───────────────┐ │ │   │
│  │  │  │ QWebChannel     │  │QWebEngineScript │  │UrlRequestInterceptor││   │
│  │  │  │  (JS 桥接)      │  │   (脚本注入)    │  │  (请求拦截)   │ │ │   │
│  │  │  └─────────────────┘  └─────────────────┘  └───────────────┘ │ │   │
│  │  └───────────────────────────────────────────────────────────────┘ │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                               │                                             │
│                               ▼                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Chromium 内核 (via QtWebEngineCore)              │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │  Blink  │   V8   │  Skia  │  Net Stack  │  GPU Process     │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  模块组成:                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │ Qt WebEngine Widgets  - 传统 Widget 集成 (QWebEngineView)           │   │
│  │ Qt WebEngine          - QML 集成 (WebEngineView)                    │   │
│  │ Qt WebEngine Core     - 核心功能 (QWebEnginePage, Profile 等)       │   │
│  │ Qt WebChannel         - C++/JS 通信桥接                             │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 进程模型

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Qt WebEngine 进程模型                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                      应用程序主进程                                 │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │  Qt 应用程序逻辑  +  Qt WebEngine Browser Process          │   │   │
│  │  │  (UI、信号槽、QWebEnginePage 等)                           │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └────────────────────────────┬────────────────────────────────────────┘   │
│                               │ IPC                                         │
│              ┌────────────────┼────────────────┐                            │
│              │                │                │                            │
│              ▼                ▼                ▼                            │
│  ┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐            │
│  │ Renderer Process │ │ Renderer Process │ │   GPU Process    │            │
│  │    (每个 Tab)    │ │    (每个 Tab)    │ │                  │            │
│  └──────────────────┘ └──────────────────┘ └──────────────────┘            │
│                                                                             │
│  可执行文件:                                                                │
│  - yourapp.exe                  → 主进程                                    │
│  - QtWebEngineProcess.exe       → 渲染进程 / GPU 进程                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Widgets 集成

### 2.1 基础使用

```cpp
// main.cpp

#include <QApplication>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

int main(int argc, char *argv[])
{
    // 必须在 QApplication 之前设置
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    // 创建 WebEngineView
    QWebEngineView view;

    // 配置设置
    QWebEngineSettings *settings = view.settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, false);

    // 设置用户代理
    view.page()->profile()->setHttpUserAgent("MyApp/1.0");

    // 加载页面
    view.load(QUrl("https://example.com"));
    view.resize(1280, 720);
    view.show();

    return app.exec();
}
```

### 2.2 自定义 WebEnginePage

```cpp
// custompage.h

#include <QWebEnginePage>
#include <QWebEngineProfile>

class CustomPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit CustomPage(QWebEngineProfile *profile, QObject *parent = nullptr)
        : QWebEnginePage(profile, parent)
    {
        // 连接信号
        connect(this, &QWebEnginePage::loadStarted, this, &CustomPage::onLoadStarted);
        connect(this, &QWebEnginePage::loadProgress, this, &CustomPage::onLoadProgress);
        connect(this, &QWebEnginePage::loadFinished, this, &CustomPage::onLoadFinished);
        connect(this, &QWebEnginePage::urlChanged, this, &CustomPage::onUrlChanged);
    }

protected:
    // 处理 JavaScript 对话框
    void javaScriptAlert(const QUrl &securityOrigin, const QString &msg) override
    {
        qDebug() << "JS Alert:" << msg;
        // 可以显示自定义对话框
        QMessageBox::information(nullptr, "Alert", msg);
    }

    bool javaScriptConfirm(const QUrl &securityOrigin, const QString &msg) override
    {
        return QMessageBox::question(nullptr, "Confirm", msg) == QMessageBox::Yes;
    }

    bool javaScriptPrompt(const QUrl &securityOrigin, const QString &msg,
                          const QString &defaultValue, QString *result) override
    {
        bool ok;
        *result = QInputDialog::getText(nullptr, "Prompt", msg,
                                        QLineEdit::Normal, defaultValue, &ok);
        return ok;
    }

    // 处理 console.log
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                  const QString &message,
                                  int lineNumber,
                                  const QString &sourceID) override
    {
        QString levelStr;
        switch (level) {
            case InfoMessageLevel: levelStr = "INFO"; break;
            case WarningMessageLevel: levelStr = "WARN"; break;
            case ErrorMessageLevel: levelStr = "ERROR"; break;
        }
        qDebug() << QString("[%1] %2:%3 %4")
                        .arg(levelStr)
                        .arg(sourceID)
                        .arg(lineNumber)
                        .arg(message);
    }

    // 处理新窗口请求
    bool acceptNavigationRequest(const QUrl &url,
                                 NavigationType type,
                                 bool isMainFrame) override
    {
        // 拦截外部链接
        if (type == NavigationTypeLinkClicked && !isMainFrame) {
            QDesktopServices::openUrl(url);
            return false;
        }
        return true;
    }

    // 创建新窗口
    QWebEnginePage *createWindow(WebWindowType type) override
    {
        // 在当前页面打开，而非新窗口
        return this;
    }

private slots:
    void onLoadStarted()
    {
        qDebug() << "Load started";
        emit loadingChanged(true);
    }

    void onLoadProgress(int progress)
    {
        emit progressChanged(progress);
    }

    void onLoadFinished(bool ok)
    {
        qDebug() << "Load finished:" << ok;
        emit loadingChanged(false);

        if (ok) {
            // 注入 JavaScript
            runJavaScript("console.log('Page loaded');");
        }
    }

    void onUrlChanged(const QUrl &url)
    {
        qDebug() << "URL changed:" << url.toString();
    }

signals:
    void loadingChanged(bool loading);
    void progressChanged(int progress);
};
```

### 2.3 执行 JavaScript 并获取返回值

```cpp
// 执行 JavaScript 并获取返回值

void WebBrowser::executeJavaScript(const QString &script)
{
    // 异步执行
    m_page->runJavaScript(script, [](const QVariant &result) {
        qDebug() << "JS Result:" << result;
    });
}

// 使用 World 隔离
void WebBrowser::executeInIsolatedWorld(const QString &script)
{
    // 在隔离的 JS 环境中执行，不影响页面全局变量
    m_page->runJavaScript(script, QWebEngineScript::ApplicationWorld,
                          [](const QVariant &result) {
        qDebug() << "Isolated JS Result:" << result;
    });
}

// 获取 DOM 元素值
void WebBrowser::getInputValue(const QString &elementId)
{
    QString script = QString(
        "document.getElementById('%1').value"
    ).arg(elementId);

    m_page->runJavaScript(script, [](const QVariant &result) {
        QString value = result.toString();
        qDebug() << "Input value:" << value;
    });
}

// 操作 DOM
void WebBrowser::setInputValue(const QString &elementId, const QString &value)
{
    QString script = QString(
        "document.getElementById('%1').value = '%2'"
    ).arg(elementId, value);

    m_page->runJavaScript(script);
}
```

---

## 3. QWebChannel - C++/JS 通信

### 3.1 C++ 端设置

```cpp
// bridge.h - 定义暴露给 JS 的对象

#include <QObject>
#include <QVariant>

class Bridge : public QObject
{
    Q_OBJECT

    // 属性 (JS 可读写)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    explicit Bridge(QObject *parent = nullptr) : QObject(parent) {}

    QString title() const { return m_title; }
    void setTitle(const QString &title) {
        if (m_title != title) {
            m_title = title;
            emit titleChanged(title);
        }
    }

public slots:
    // JS 可调用的方法
    QString readFile(const QString &path)
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString::fromUtf8(file.readAll());
        }
        return QString();
    }

    bool writeFile(const QString &path, const QString &content)
    {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(content.toUtf8());
            return true;
        }
        return false;
    }

    void showMessage(const QString &message)
    {
        QMessageBox::information(nullptr, "Message", message);
    }

    // 返回复杂对象
    QVariantMap getSystemInfo()
    {
        QVariantMap info;
        info["os"] = QSysInfo::productType();
        info["version"] = QSysInfo::productVersion();
        info["arch"] = QSysInfo::currentCpuArchitecture();
        info["hostname"] = QSysInfo::machineHostName();
        return info;
    }

    // 异步操作 (使用回调)
    void fetchData(const QString &url)
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished,
                [this, manager](QNetworkReply *reply) {
            emit dataFetched(reply->readAll());
            manager->deleteLater();
        });
        manager->get(QNetworkRequest(QUrl(url)));
    }

signals:
    void titleChanged(const QString &title);
    void dataFetched(const QString &data);  // 通知 JS

private:
    QString m_title;
};
```

```cpp
// main.cpp - 设置 WebChannel

#include <QWebChannel>
#include <QWebEngineView>
#include <QWebEngineProfile>

void setupWebChannel(QWebEngineView *view)
{
    // 创建 WebChannel
    QWebChannel *channel = new QWebChannel(view);

    // 注册对象
    Bridge *bridge = new Bridge(view);
    channel->registerObject("native", bridge);

    // 附加到页面
    view->page()->setWebChannel(channel);

    // 注入 qwebchannel.js
    // 方法 1: 自动注入
    QFile file(":/qtwebchannel/qwebchannel.js");
    file.open(QIODevice::ReadOnly);
    QString qwebchannelJs = file.readAll();

    QWebEngineScript script;
    script.setSourceCode(qwebchannelJs +
        "\n"
        "new QWebChannel(qt.webChannelTransport, function(channel) {"
        "    window.native = channel.objects.native;"
        "    window.dispatchEvent(new Event('nativeReady'));"
        "});"
    );
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setWorldId(QWebEngineScript::MainWorld);
    view->page()->scripts().insert(script);
}
```

### 3.2 JavaScript 端使用

```javascript
// 等待 native 对象就绪
window.addEventListener('nativeReady', function() {
    console.log('Native bridge is ready');

    // 调用 C++ 方法
    const content = native.readFile('/path/to/file.txt');
    console.log('File content:', content);

    // 异步获取系统信息
    const info = native.getSystemInfo();
    console.log('System:', info.os, info.version);

    // 显示消息
    native.showMessage('Hello from JavaScript!');

    // 读写属性
    console.log('Current title:', native.title);
    native.title = 'New Title';

    // 监听属性变化
    native.titleChanged.connect(function(newTitle) {
        console.log('Title changed to:', newTitle);
    });

    // 监听 C++ 发出的信号
    native.dataFetched.connect(function(data) {
        console.log('Data fetched:', data);
    });

    // 触发异步操作
    native.fetchData('https://api.example.com/data');
});
```

### 3.3 HTML 页面模板

```html
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Qt WebChannel Demo</title>
    <script src="qrc:///qtwebchannel/qwebchannel.js"></script>
</head>
<body>
    <h1>Qt WebChannel Demo</h1>
    <button id="btnRead">Read File</button>
    <button id="btnWrite">Write File</button>
    <div id="output"></div>

    <script>
        let native = null;

        // 初始化 WebChannel
        new QWebChannel(qt.webChannelTransport, function(channel) {
            native = channel.objects.native;

            document.getElementById('btnRead').onclick = function() {
                const content = native.readFile('C:/test.txt');
                document.getElementById('output').textContent = content;
            };

            document.getElementById('btnWrite').onclick = function() {
                const success = native.writeFile('C:/test.txt', 'Hello from JS');
                alert(success ? 'Saved!' : 'Failed!');
            };

            // 监听信号
            native.dataFetched.connect(function(data) {
                document.getElementById('output').textContent = data;
            });
        });
    </script>
</body>
</html>
```

---

## 4. QML 集成

### 4.1 基础 QML 用法

```qml
// main.qml

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebEngine 1.10
import QtWebChannel 1.0

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true
    title: webView.title

    // 工具栏
    header: ToolBar {
        Row {
            anchors.fill: parent
            spacing: 4

            ToolButton {
                text: "←"
                enabled: webView.canGoBack
                onClicked: webView.goBack()
            }

            ToolButton {
                text: "→"
                enabled: webView.canGoForward
                onClicked: webView.goForward()
            }

            ToolButton {
                text: "⟳"
                onClicked: webView.reload()
            }

            TextField {
                id: urlField
                width: parent.width - 150
                text: webView.url
                onAccepted: webView.url = text
            }
        }
    }

    // WebEngineView
    WebEngineView {
        id: webView
        anchors.fill: parent
        url: "https://example.com"

        // 配置
        settings.javascriptEnabled: true
        settings.localStorageEnabled: true
        settings.webGLEnabled: true

        // 设置 WebChannel
        webChannel: channel

        // 加载进度
        onLoadingChanged: {
            if (loadRequest.status === WebEngineView.LoadSucceededStatus) {
                console.log("Page loaded successfully");
            }
        }

        onLoadProgress: {
            progressBar.value = progress / 100;
        }

        // 处理新窗口请求
        onNewViewRequested: {
            // 在当前视图打开
            webView.url = request.requestedUrl;
        }

        // JavaScript 对话框
        onJavaScriptDialogRequested: {
            if (request.type === JavaScriptDialogRequest.DialogTypeAlert) {
                messageDialog.text = request.message;
                messageDialog.open();
                request.accepted = true;
            }
        }
    }

    // WebChannel
    WebChannel {
        id: channel
        registeredObjects: [nativeBridge]
    }

    // 暴露给 JS 的对象
    QtObject {
        id: nativeBridge
        WebChannel.id: "native"

        property string appVersion: "1.0.0"

        function showMessage(msg) {
            messageDialog.text = msg;
            messageDialog.open();
        }

        function getWindowSize() {
            return { width: window.width, height: window.height };
        }

        signal dataReady(string data)
    }

    // 进度条
    ProgressBar {
        id: progressBar
        anchors.bottom: parent.bottom
        width: parent.width
        visible: webView.loading
    }

    // 消息对话框
    Dialog {
        id: messageDialog
        property alias text: messageText.text
        modal: true
        anchors.centerIn: parent

        Label {
            id: messageText
        }

        standardButtons: Dialog.Ok
    }
}
```

### 4.2 QML 与 C++ 交互

```cpp
// main.cpp

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtWebEngine>

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)

public:
    explicit Backend(QObject *parent = nullptr) : QObject(parent) {}

    QString status() const { return m_status; }
    void setStatus(const QString &status) {
        if (m_status != status) {
            m_status = status;
            emit statusChanged();
        }
    }

public slots:
    QString processData(const QString &input) {
        // 处理数据
        return input.toUpper();
    }

signals:
    void statusChanged();

private:
    QString m_status;
};

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtWebEngine::initialize();

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 注册 C++ 对象
    Backend backend;
    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
```

---

## 5. 高级功能

### 5.1 请求拦截

```cpp
// urlrequestinterceptor.h

#include <QWebEngineUrlRequestInterceptor>

class UrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit UrlRequestInterceptor(QObject *parent = nullptr)
        : QWebEngineUrlRequestInterceptor(parent) {}

    void interceptRequest(QWebEngineUrlRequestInfo &info) override
    {
        QUrl url = info.requestUrl();

        // 阻止广告请求
        if (isAdUrl(url)) {
            info.block(true);
            return;
        }

        // 重定向请求
        if (url.host() == "old.example.com") {
            QUrl newUrl = url;
            newUrl.setHost("new.example.com");
            info.redirect(newUrl);
            return;
        }

        // 修改请求头
        info.setHttpHeader("X-Custom-Header", "MyValue");
        info.setHttpHeader("User-Agent", "MyApp/1.0");

        // 根据资源类型处理
        switch (info.resourceType()) {
            case QWebEngineUrlRequestInfo::ResourceTypeImage:
                // 可以阻止图片加载以节省流量
                // info.block(true);
                break;
            case QWebEngineUrlRequestInfo::ResourceTypeScript:
                qDebug() << "Loading script:" << url.toString();
                break;
            default:
                break;
        }
    }

private:
    bool isAdUrl(const QUrl &url) {
        static QStringList adHosts = {
            "ads.example.com",
            "tracking.example.com"
        };
        return adHosts.contains(url.host());
    }
};

// 使用
void setupInterceptor(QWebEngineProfile *profile)
{
    UrlRequestInterceptor *interceptor = new UrlRequestInterceptor(profile);
    profile->setUrlRequestInterceptor(interceptor);
}
```

### 5.2 自定义 Scheme

```cpp
// customschemehandler.h

#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestJob>
#include <QBuffer>

class CustomSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:
    explicit CustomSchemeHandler(QObject *parent = nullptr)
        : QWebEngineUrlSchemeHandler(parent) {}

    void requestStarted(QWebEngineUrlRequestJob *job) override
    {
        QUrl url = job->requestUrl();
        QString path = url.path();

        // 处理 app:// 协议
        if (path == "/index.html") {
            serveFile(job, ":/web/index.html", "text/html");
        } else if (path.endsWith(".js")) {
            serveFile(job, ":/web" + path, "application/javascript");
        } else if (path.endsWith(".css")) {
            serveFile(job, ":/web" + path, "text/css");
        } else {
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        }
    }

private:
    void serveFile(QWebEngineUrlRequestJob *job,
                   const QString &filePath,
                   const QString &mimeType)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);
            return;
        }

        QByteArray content = file.readAll();
        QBuffer *buffer = new QBuffer(job);
        buffer->setData(content);
        buffer->open(QIODevice::ReadOnly);

        job->reply(mimeType.toUtf8(), buffer);
    }
};

// 注册 Scheme (必须在 QApplication 创建前)
void registerCustomScheme()
{
    QWebEngineUrlScheme scheme("app");
    scheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    scheme.setFlags(QWebEngineUrlScheme::SecureScheme |
                    QWebEngineUrlScheme::LocalAccessAllowed);
    QWebEngineUrlScheme::registerScheme(scheme);
}

// 安装 Handler
void setupSchemeHandler(QWebEngineProfile *profile)
{
    CustomSchemeHandler *handler = new CustomSchemeHandler(profile);
    profile->installUrlSchemeHandler("app", handler);
}

// 使用
// view->load(QUrl("app:///index.html"));
```

### 5.3 脚本注入

```cpp
// 注入 JavaScript 脚本

void injectScripts(QWebEnginePage *page)
{
    // 注入初始化脚本 (在 DOM 创建时)
    QWebEngineScript initScript;
    initScript.setName("init");
    initScript.setSourceCode(R"(
        console.log('Init script loaded');
        window.appVersion = '1.0.0';
    )");
    initScript.setInjectionPoint(QWebEngineScript::DocumentCreation);
    initScript.setWorldId(QWebEngineScript::MainWorld);
    initScript.setRunsOnSubFrames(false);
    page->scripts().insert(initScript);

    // 注入 DOM 就绪脚本
    QWebEngineScript domReadyScript;
    domReadyScript.setName("domReady");
    domReadyScript.setSourceCode(R"(
        document.addEventListener('DOMContentLoaded', function() {
            console.log('DOM is ready');
            // 添加自定义样式
            var style = document.createElement('style');
            style.textContent = 'body { font-family: "Microsoft YaHei", sans-serif; }';
            document.head.appendChild(style);
        });
    )");
    domReadyScript.setInjectionPoint(QWebEngineScript::DocumentReady);
    domReadyScript.setWorldId(QWebEngineScript::MainWorld);
    page->scripts().insert(domReadyScript);

    // 从文件加载脚本
    QFile scriptFile(":/scripts/custom.js");
    if (scriptFile.open(QIODevice::ReadOnly)) {
        QWebEngineScript fileScript;
        fileScript.setName("customScript");
        fileScript.setSourceCode(scriptFile.readAll());
        fileScript.setInjectionPoint(QWebEngineScript::Deferred);
        fileScript.setWorldId(QWebEngineScript::MainWorld);
        page->scripts().insert(fileScript);
    }
}
```

---

## 6. 面试常见问题

### Q1: Qt WebChannel 是如何实现 C++ 和 JS 通信的？

**答案：**
Qt WebChannel 使用 WebSocket 协议实现通信：

1. C++ 端创建 `QWebChannel` 并注册 QObject
2. QObject 的信号、槽和属性被序列化为 JSON 格式
3. 通过 `qt.webChannelTransport` 传输到 JS
4. JS 端使用 `qwebchannel.js` 库解析并创建代理对象
5. 代理对象的方法调用会序列化为消息发送回 C++

### Q2: Qt WebEngine 的进程模型与 CEF 有什么区别？

**答案：**

| 特性 | Qt WebEngine | CEF |
|------|-------------|-----|
| 进程可执行文件 | QtWebEngineProcess.exe | 同主程序 (your_app.exe) |
| 进程配置 | 较少可控 | 完全可控 |
| 单进程模式 | 不支持 | 支持 |
| 进程数量 | 自动管理 | 可精细控制 |

### Q3: 如何在 Qt WebEngine 中拦截和修改网络请求？

**答案：**
1. 继承 `QWebEngineUrlRequestInterceptor`
2. 重写 `interceptRequest()` 方法
3. 使用 `QWebEngineUrlRequestInfo` 的方法：
   - `block(true)` 阻止请求
   - `redirect(url)` 重定向
   - `setHttpHeader()` 修改请求头

### Q4: QWebChannel 支持异步调用吗？

**答案：**
- 默认情况下，C++ 槽函数是同步调用的
- 返回值会直接返回给 JS
- 对于异步操作，需要使用信号：
  1. C++ 方法启动异步操作后立即返回
  2. 异步操作完成后发出信号
  3. JS 监听该信号获取结果

```cpp
// C++ 端
void asyncOperation() {
    // 启动异步操作
    QTimer::singleShot(1000, [this]() {
        emit operationComplete("result");
    });
}

signals:
    void operationComplete(const QString &result);
```

```javascript
// JS 端
native.asyncOperation();
native.operationComplete.connect(function(result) {
    console.log('Result:', result);
});
```

### Q5: Qt WebEngine 的性能优化建议？

**答案：**
1. **使用独立 Profile**：避免多个 WebView 共享状态
2. **禁用不需要的功能**：如 WebGL、插件
3. **缓存配置**：设置合理的缓存大小和路径
4. **离屏渲染**：对于不显示的页面使用离屏渲染
5. **脚本优化**：在 DocumentCreation 注入必要脚本，避免重复注入
6. **请求拦截**：阻止不必要的资源加载
