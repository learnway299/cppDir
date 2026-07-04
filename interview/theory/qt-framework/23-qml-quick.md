# QML 与 Qt Quick

> 深入浅出考察 QML 语言基础、Qt Quick 框架原理与 C++ 交互

---

## 1. QML 概述

### 1.1 什么是 QML？

**面试问题**：QML 和 Qt Widgets 有什么区别？各自适合什么场景？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                    Qt UI 技术对比                                │
├─────────────────────────────────┬───────────────────────────────┤
│         Qt Widgets              │         QML/Qt Quick          │
├─────────────────────────────────┼───────────────────────────────┤
│ C++ 代码定义 UI                  │ 声明式语言定义 UI              │
│ 命令式编程                       │ 声明式编程                     │
│ CPU 渲染 (QPainter)             │ GPU 渲染 (Scene Graph)        │
│ 成熟稳定，控件丰富               │ 现代、动画流畅                  │
│ 桌面应用为主                     │ 移动/嵌入式/触屏优先            │
│ 学习曲线较陡                     │ 上手快，设计师友好              │
│ 静态 UI 为主                     │ 动态/响应式 UI                 │
└─────────────────────────────────┴───────────────────────────────┘
```

```qml
// QML 示例：声明式定义 UI
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300
    title: "QML Demo"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Hello, QML!"
            font.pixelSize: 24
            color: "steelblue"
        }

        Button {
            text: "Click Me"
            onClicked: console.log("Button clicked!")
        }
    }
}
```

```cpp
// 对比 Qt Widgets：命令式定义 UI
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Widgets Demo");
    window.resize(400, 300);

    QWidget* central = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(central);

    QLabel* label = new QLabel("Hello, Widgets!");
    label->setStyleSheet("font-size: 24px; color: steelblue;");

    QPushButton* button = new QPushButton("Click Me");
    QObject::connect(button, &QPushButton::clicked, []() {
        qDebug() << "Button clicked!";
    });

    layout->addWidget(label);
    layout->addWidget(button);
    layout->setAlignment(Qt::AlignCenter);

    window.setCentralWidget(central);
    window.show();

    return app.exec();
}
```

**场景选择**：

| 场景 | 推荐 |
|------|------|
| 传统桌面应用（IDE、办公软件） | Qt Widgets |
| 移动应用 | QML/Qt Quick |
| 嵌入式触屏设备 | QML/Qt Quick |
| 需要复杂动画/过渡效果 | QML/Qt Quick |
| 已有大量 Widgets 代码 | Qt Widgets |
| 设计师需要参与 UI 开发 | QML/Qt Quick |
| 需要原生平台外观 | Qt Widgets |

---

### 1.2 QML 引擎架构

**面试问题**：QML 引擎是如何工作的？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                      QML 引擎架构                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐ │
│  │   .qml 文件  │ -> │  QML 编译器  │ -> │  QML 字节码/缓存     │ │
│  └─────────────┘    └─────────────┘    └─────────────────────┘ │
│                                               │                 │
│                                               ▼                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    QQmlEngine                            │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐  │   │
│  │  │ QML Context │  │ Object Tree │  │ Type Registry   │  │   │
│  │  │ (属性绑定)   │  │ (对象实例)   │  │ (类型注册)      │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────────┘  │   │
│  └─────────────────────────────────────────────────────────┘   │
│                            │                                    │
│                            ▼                                    │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                  Qt Quick Scene Graph                    │   │
│  │  (GPU 加速渲染，批量处理，OpenGL/Vulkan/Metal)            │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

```cpp
// C++ 中加载 QML
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 设置上下文属性（向 QML 暴露 C++ 数据）
    engine.rootContext()->setContextProperty("appVersion", "1.0.0");

    // 加载 QML 文件
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

---

## 2. QML 语言基础

### 2.1 基本语法

**面试问题**：QML 的基本语法结构是什么？

**答案**：

```qml
// QML 文件结构
import QtQuick 2.15          // 1. 导入语句
import QtQuick.Controls 2.15
import "components"          // 导入本地 QML 目录
import MyModule 1.0          // 导入自定义 C++ 模块

// 2. 根元素（每个 QML 文件只有一个根元素）
Rectangle {
    // 3. 属性
    id: root                 // 唯一标识符
    width: 400
    height: 300
    color: "lightgray"

    // 4. 属性绑定（响应式）
    property int counter: 0
    property string message: "Count: " + counter  // 自动更新

    // 5. 信号定义
    signal buttonClicked(string text)

    // 6. 子元素
    Text {
        id: label
        text: root.message
        anchors.centerIn: parent
        font.pixelSize: 20
    }

    // 7. JavaScript 函数
    function incrementCounter() {
        counter++
    }

    // 8. 信号处理器
    MouseArea {
        anchors.fill: parent
        onClicked: {
            incrementCounter()
            root.buttonClicked("Clicked at " + mouse.x + "," + mouse.y)
        }
    }

    // 9. 组件生命周期
    Component.onCompleted: {
        console.log("Component loaded")
    }

    Component.onDestruction: {
        console.log("Component destroyed")
    }
}
```

---

### 2.2 属性绑定

**面试问题**：QML 的属性绑定是如何实现的？有什么注意事项？

**答案**：

```qml
import QtQuick 2.15

Rectangle {
    id: root
    width: 400
    height: 300

    // 基本属性绑定
    property int baseSize: 100
    property int scaledSize: baseSize * 2  // 当 baseSize 变化时自动更新

    Rectangle {
        id: box
        width: root.scaledSize   // 绑定到父元素属性
        height: width            // 绑定到自身属性
        color: width > 150 ? "red" : "blue"  // 条件绑定
        anchors.centerIn: parent
    }

    // 双向绑定问题
    property int value1: 10
    property int value2: value1  // 单向绑定：value2 依赖 value1

    // ❌ 错误：打破绑定
    // Component.onCompleted: {
    //     value2 = 20  // 这会打破绑定！value2 不再跟随 value1
    // }

    // ✅ 正确：使用 Qt.binding 重新建立绑定
    function resetBinding() {
        value2 = Qt.binding(function() { return value1 * 2 })
    }

    // 属性绑定 vs 赋值
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            // 赋值：打破绑定
            // box.width = 200

            // 保持绑定：修改源属性
            root.baseSize += 10
        }
    }
}

// 绑定循环问题
Rectangle {
    // ❌ 错误：绑定循环
    // width: height + 10
    // height: width + 10

    // ✅ 正确：避免循环依赖
    property int size: 100
    width: size
    height: size
}
```

**属性绑定原理**：

```
属性绑定实现机制：
1. QML 引擎创建绑定表达式
2. 表达式中引用的属性被添加到依赖列表
3. 当依赖属性变化时，触发重新求值
4. 新值赋给绑定属性

性能注意事项：
- 避免在绑定中进行复杂计算
- 避免绑定循环
- 避免在高频更新属性上使用复杂绑定
- JavaScript 赋值会打破绑定
```

---

### 2.3 信号与槽

**面试问题**：QML 中的信号与槽和 C++ 中的有什么区别？

**答案**：

```qml
import QtQuick 2.15

Rectangle {
    id: root
    width: 400
    height: 300

    // 1. 定义信号
    signal userLoggedIn(string username, int userId)
    signal simpleSignal()

    // 2. 信号处理器（on<Signal> 语法）
    onUserLoggedIn: (username, userId) => {
        console.log("User logged in:", username, userId)
    }

    // 3. 内置信号处理
    onWidthChanged: console.log("Width changed to:", width)

    // 4. 发射信号
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.userLoggedIn("Alice", 123)
            root.simpleSignal()
        }
    }

    // 5. Connections 对象（动态连接）
    Connections {
        target: root
        function onSimpleSignal() {
            console.log("Simple signal received via Connections")
        }
    }

    // 6. 子组件信号
    MyButton {
        id: myBtn
        onClicked: console.log("MyButton clicked")
    }
}

// MyButton.qml - 自定义组件
// Rectangle {
//     id: root
//     signal clicked()
//
//     MouseArea {
//         anchors.fill: parent
//         onClicked: root.clicked()
//     }
// }
```

**QML 信号 vs C++ 信号**：

| 特性 | QML 信号 | C++ 信号 |
|------|----------|----------|
| 定义方式 | `signal name(params)` | `signals: void name(params);` |
| 连接方式 | `on<Signal>` 或 `Connections` | `connect()` |
| 类型检查 | 运行时 | 编译时 |
| 性能 | 较低（JavaScript 调用） | 较高 |
| 灵活性 | 高（动态） | 中等 |

---

## 3. Qt Quick 核心概念

### 3.1 Item 与可视元素

**面试问题**：QML 中 Item、Rectangle、Image 等元素的继承关系是什么？

**答案**：

```
QObject
    └── QQuickItem (对应 QML 的 Item)
            ├── QQuickRectangle (Rectangle)
            ├── QQuickImage (Image)
            ├── QQuickText (Text)
            ├── QQuickMouseArea (MouseArea)
            ├── QQuickFlickable (Flickable)
            │       └── QQuickListView (ListView)
            └── ...
```

```qml
import QtQuick 2.15

// Item：所有可视元素的基类
Item {
    id: root
    width: 400
    height: 300

    // Item 核心属性
    // x, y, z, width, height
    // visible, enabled, opacity
    // scale, rotation
    // anchors, parent, children
    // focus, activeFocus

    Rectangle {
        id: rect1
        width: 100
        height: 100
        color: "red"

        // 层叠顺序
        z: 1

        // 透明度
        opacity: 0.8

        // 变换
        scale: 1.2
        rotation: 15
        transformOrigin: Item.Center
    }

    // 锚点布局
    Rectangle {
        id: rect2
        width: 50
        height: 50
        color: "blue"

        // 锚点到其他元素
        anchors.left: rect1.right
        anchors.leftMargin: 10
        anchors.verticalCenter: rect1.verticalCenter
    }

    // 非可视元素也继承 QObject
    Timer {
        interval: 1000
        running: true
        onTriggered: console.log("Timer fired")
    }
}
```

---

### 3.2 布局系统

**面试问题**：QML 有哪些布局方式？各自的特点是什么？

**答案**：

```qml
import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    width: 600
    height: 400

    // 方式 1：锚点布局（Anchors）
    // 特点：灵活，性能好，适合相对定位
    Rectangle {
        id: anchorDemo
        width: 100
        height: 100
        color: "red"

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
    }

    // 方式 2：定位器（Positioners）
    // 特点：自动排列子元素，简单易用
    Column {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 5

        Repeater {
            model: 3
            Rectangle {
                width: 50
                height: 30
                color: "green"
            }
        }
    }

    Row {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        spacing: 5

        Repeater {
            model: 4
            Rectangle {
                width: 30
                height: 30
                color: "blue"
            }
        }
    }

    Grid {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        columns: 3
        spacing: 5

        Repeater {
            model: 9
            Rectangle {
                width: 25
                height: 25
                color: "orange"
            }
        }
    }

    // 方式 3：Qt Quick Layouts
    // 特点：类似 Widgets 布局，支持拉伸和尺寸策略
    RowLayout {
        anchors.centerIn: parent
        spacing: 10

        Rectangle {
            Layout.preferredWidth: 100
            Layout.preferredHeight: 50
            Layout.fillHeight: true
            color: "purple"
        }

        Rectangle {
            Layout.preferredWidth: 100
            Layout.preferredHeight: 50
            Layout.fillWidth: true
            color: "cyan"
        }
    }
}

// 布局对比
/*
┌─────────────────┬──────────────────────────────────────────┐
│ 布局方式         │ 适用场景                                  │
├─────────────────┼──────────────────────────────────────────┤
│ Anchors         │ 相对定位，自适应布局，最常用               │
│ Column/Row/Grid │ 简单线性排列，固定尺寸子元素               │
│ RowLayout 等    │ 需要自动伸缩，复杂布局                     │
│ 绝对定位 (x, y) │ 特殊需求，如拖拽、动画目标位置             │
└─────────────────┴──────────────────────────────────────────┘
*/
```

---

### 3.3 Scene Graph 渲染

**面试问题**：Qt Quick 的 Scene Graph 是什么？为什么比 QPainter 快？

**答案**：

```
┌─────────────────────────────────────────────────────────────────┐
│                    Qt Quick Scene Graph                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  QML 元素树                      Scene Graph 节点树              │
│  ┌─────────┐                    ┌─────────────┐                 │
│  │ Window  │                    │ Root Node   │                 │
│  └────┬────┘                    └──────┬──────┘                 │
│       │                                │                        │
│  ┌────┴────┐                    ┌──────┴──────┐                 │
│  │Rectangle│  ───────────────>  │ Geometry    │                 │
│  └────┬────┘    转换             │ + Material  │                 │
│       │                         └──────┬──────┘                 │
│  ┌────┴────┐                    ┌──────┴──────┐                 │
│  │  Text   │  ───────────────>  │ Glyph Node  │                 │
│  └─────────┘                    └─────────────┘                 │
│                                                                 │
│  渲染流程：                                                      │
│  1. 构建/更新 Scene Graph 节点                                   │
│  2. 预处理（变换计算、裁剪）                                      │
│  3. 批量合并（Batching）                                         │
│  4. GPU 渲染                                                     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

```cpp
// Scene Graph 高性能的原因
class SceneGraphAdvantages {
public:
    void explain() {
        // 1. GPU 加速
        // QPainter 默认使用 CPU 光栅化
        // Scene Graph 使用 OpenGL/Vulkan/Metal 进行 GPU 渲染

        // 2. 批量处理（Batching）
        // 相同材质的图元合并成一个 draw call
        // 减少 CPU-GPU 通信开销

        // 3. 增量更新
        // 只更新变化的节点，不需要完全重绘

        // 4. 专用渲染线程
        // Scene Graph 在独立线程渲染，不阻塞主线程

        // 5. 缓存与复用
        // 纹理、着色器等资源缓存复用
    }
};
```

```qml
// 自定义 Scene Graph 节点（高级）
import QtQuick 2.15

// 使用 ShaderEffect 自定义着色器
ShaderEffect {
    width: 200
    height: 200

    property real time: 0

    NumberAnimation on time {
        from: 0
        to: Math.PI * 2
        duration: 1000
        loops: Animation.Infinite
    }

    fragmentShader: "
        uniform highp float time;
        varying highp vec2 qt_TexCoord0;
        void main() {
            highp float wave = sin(qt_TexCoord0.x * 10.0 + time) * 0.5 + 0.5;
            gl_FragColor = vec4(wave, 0.5, 1.0 - wave, 1.0);
        }
    "
}
```

---

## 4. C++ 与 QML 交互

### 4.1 暴露 C++ 对象到 QML

**面试问题**：如何在 QML 中使用 C++ 类？有哪些方式？

**答案**：

```cpp
// 方式 1：上下文属性（Context Property）
// 适合：单例对象、全局配置

// backend.h
class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit Backend(QObject* parent = nullptr) : QObject(parent) {}

    QString userName() const { return m_userName; }
    void setUserName(const QString& name) {
        if (m_userName != name) {
            m_userName = name;
            emit userNameChanged();
        }
    }

    int count() const { return m_count; }

    // Q_INVOKABLE 使方法可从 QML 调用
    Q_INVOKABLE void increment() {
        m_count++;
        emit countChanged();
    }

    Q_INVOKABLE QString formatMessage(const QString& msg) {
        return QString("[%1] %2").arg(m_userName, msg);
    }

signals:
    void userNameChanged();
    void countChanged();
    void errorOccurred(const QString& error);

private:
    QString m_userName = "Guest";
    int m_count = 0;
};

// main.cpp
int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    Backend backend;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
```

```qml
// main.qml - 使用上下文属性
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Column {
        anchors.centerIn: parent
        spacing: 10

        Text {
            text: "User: " + backend.userName
        }

        Text {
            text: "Count: " + backend.count
        }

        Button {
            text: "Increment"
            onClicked: backend.increment()
        }

        TextField {
            id: nameInput
            placeholderText: "Enter name"
            onTextChanged: backend.userName = text
        }
    }

    // 处理 C++ 信号
    Connections {
        target: backend
        function onErrorOccurred(error) {
            console.log("Error:", error)
        }
    }
}
```

---

### 4.2 注册 C++ 类型到 QML

```cpp
// 方式 2：注册 QML 类型
// 适合：可复用组件、需要在 QML 中创建实例

// colorpicker.h
class ColorPicker : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)
    QML_ELEMENT  // Qt 6 方式

public:
    explicit ColorPicker(QQuickItem* parent = nullptr);

    QColor currentColor() const { return m_color; }
    void setCurrentColor(const QColor& color);

signals:
    void currentColorChanged();
    void colorPicked(const QColor& color);

private:
    QColor m_color = Qt::white;
};

// main.cpp
#include <QtQml>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    // Qt 5 注册方式
    qmlRegisterType<ColorPicker>("MyComponents", 1, 0, "ColorPicker");

    // Qt 6 使用 QML_ELEMENT 宏 + CMake 配置
    // qt_add_qml_module(myapp
    //     URI MyComponents
    //     VERSION 1.0
    //     SOURCES colorpicker.h colorpicker.cpp
    // )

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
```

```qml
// main.qml - 使用注册的类型
import QtQuick 2.15
import MyComponents 1.0  // 导入自定义模块

Rectangle {
    width: 400
    height: 300

    ColorPicker {
        id: picker
        anchors.centerIn: parent
        width: 200
        height: 200

        onColorPicked: (color) => {
            console.log("Picked color:", color)
        }
    }

    Rectangle {
        width: 50
        height: 50
        color: picker.currentColor
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
```

---

### 4.3 枚举和单例

```cpp
// 枚举暴露
class Status : public QObject {
    Q_OBJECT

public:
    enum State {
        Idle,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(State)
};

// 单例模式
class AppSettings : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    QML_ELEMENT
    QML_SINGLETON  // Qt 6

public:
    static AppSettings* instance() {
        static AppSettings inst;
        return &inst;
    }

    QString theme() const { return m_theme; }
    void setTheme(const QString& theme) { /* ... */ }

signals:
    void themeChanged();

private:
    AppSettings() = default;
    QString m_theme = "light";
};

// Qt 5 注册单例
// qmlRegisterSingletonType<AppSettings>("App", 1, 0, "Settings",
//     [](QQmlEngine*, QJSEngine*) -> QObject* {
//         return AppSettings::instance();
//     });
```

```qml
// 使用枚举和单例
import QtQuick 2.15
import App 1.0

Rectangle {
    // 使用枚举
    property int currentState: Status.Idle

    // 使用单例
    color: Settings.theme === "dark" ? "black" : "white"

    Component.onCompleted: {
        Settings.theme = "dark"
    }
}
```

---

## 5. QML 动画系统

### 5.1 动画类型

**面试问题**：QML 有哪些动画类型？如何选择？

**答案**：

```qml
import QtQuick 2.15

Rectangle {
    id: root
    width: 400
    height: 400

    // 1. PropertyAnimation - 基础属性动画
    Rectangle {
        id: rect1
        width: 50; height: 50
        color: "red"

        PropertyAnimation on x {
            from: 0
            to: 300
            duration: 2000
            loops: Animation.Infinite
        }
    }

    // 2. NumberAnimation - 数值动画
    Rectangle {
        id: rect2
        y: 60
        width: 50; height: 50
        color: "blue"

        NumberAnimation on opacity {
            from: 1.0
            to: 0.2
            duration: 1000
            loops: Animation.Infinite
            easing.type: Easing.InOutQuad
        }
    }

    // 3. ColorAnimation - 颜色动画
    Rectangle {
        id: rect3
        y: 120
        width: 50; height: 50

        ColorAnimation on color {
            from: "green"
            to: "yellow"
            duration: 1500
            loops: Animation.Infinite
        }
    }

    // 4. RotationAnimation - 旋转动画
    Rectangle {
        id: rect4
        x: 175; y: 180
        width: 50; height: 50
        color: "purple"

        RotationAnimation on rotation {
            from: 0
            to: 360
            duration: 2000
            loops: Animation.Infinite
        }
    }

    // 5. PathAnimation - 路径动画
    Rectangle {
        id: rect5
        width: 30; height: 30
        color: "orange"

        PathAnimation {
            target: rect5
            duration: 3000
            loops: Animation.Infinite
            path: Path {
                startX: 50; startY: 300
                PathQuad { x: 350; y: 300; controlX: 200; controlY: 200 }
            }
        }
    }

    // 6. Behavior - 属性变化时自动动画
    Rectangle {
        id: rect6
        x: 50; y: 350
        width: 50; height: 50
        color: "cyan"

        Behavior on x {
            NumberAnimation { duration: 500; easing.type: Easing.OutBounce }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rect6.x = rect6.x === 50 ? 300 : 50
        }
    }
}
```

---

### 5.2 动画组合

```qml
import QtQuick 2.15

Rectangle {
    id: root
    width: 400
    height: 300

    Rectangle {
        id: animTarget
        width: 50; height: 50
        color: "red"
        x: 50; y: 100
    }

    // 顺序动画
    SequentialAnimation {
        id: seqAnim

        NumberAnimation {
            target: animTarget
            property: "x"
            to: 300
            duration: 500
        }

        PauseAnimation { duration: 200 }

        NumberAnimation {
            target: animTarget
            property: "y"
            to: 200
            duration: 500
        }

        NumberAnimation {
            target: animTarget
            properties: "x,y"
            to: 50
            duration: 500
        }
    }

    // 并行动画
    ParallelAnimation {
        id: parAnim

        NumberAnimation {
            target: animTarget
            property: "x"
            to: 300
            duration: 1000
        }

        NumberAnimation {
            target: animTarget
            property: "rotation"
            to: 360
            duration: 1000
        }

        ColorAnimation {
            target: animTarget
            property: "color"
            to: "blue"
            duration: 1000
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Button {
            text: "Sequential"
            onClicked: seqAnim.start()
        }

        Button {
            text: "Parallel"
            onClicked: parAnim.start()
        }
    }
}
```

---

## 6. 面试问题精选

### Q1: QML vs Widgets

**问题**：QML 和 Qt Widgets 各自的优缺点？如何选择？

**参考答案**：

| 方面 | QML | Qt Widgets |
|------|-----|------------|
| 渲染 | GPU (Scene Graph) | CPU (QPainter) |
| 性能 | 动画/图形效果更流畅 | 大量文本/表格更高效 |
| 开发效率 | 声明式，快速迭代 | 命令式，逻辑清晰 |
| 外观 | 需要自定义，灵活 | 原生外观，成熟控件 |
| 学习曲线 | 需要学习新语言 | 纯 C++ |
| 调试 | 相对困难 | 标准 C++ 调试 |
| 适用场景 | 移动/嵌入式/现代 UI | 桌面/传统应用 |

选择建议：新项目优先考虑 QML，已有 Widgets 代码则继续使用。

---

### Q2: 属性绑定原理

**问题**：QML 属性绑定是如何实现的？如何避免绑定被破坏？

**参考答案**：

属性绑定通过 QML 引擎的依赖追踪实现：
1. 引擎执行绑定表达式，记录访问的属性
2. 建立依赖关系（观察者模式）
3. 依赖属性变化时，重新求值

避免绑定破坏：
- 不要在 JavaScript 中直接赋值给绑定属性
- 使用 `Qt.binding()` 重新建立绑定
- 修改源属性而不是绑定属性

---

### Q3: C++ 与 QML 交互方式

**问题**：C++ 和 QML 交互有哪些方式？各自适合什么场景？

**参考答案**：

1. **上下文属性**：`setContextProperty()`
   - 适合：全局单例、配置对象
   - 缺点：没有类型检查，不可复用

2. **注册类型**：`qmlRegisterType()`
   - 适合：可复用组件
   - 可在 QML 中创建实例

3. **单例**：`qmlRegisterSingletonType()`
   - 适合：全局服务、设置

4. **QML_ELEMENT (Qt 6)**
   - 编译时注册，类型安全

---

### Q4: Scene Graph 优势

**问题**：Qt Quick 的 Scene Graph 相比 QPainter 有什么优势？

**参考答案**：

1. **GPU 加速**：利用显卡进行渲染
2. **批量处理**：合并 draw call，减少 CPU-GPU 通信
3. **独立渲染线程**：不阻塞主线程
4. **增量更新**：只更新变化的节点
5. **纹理缓存**：复用已上传的纹理

结果：动画更流畅，帧率更稳定，适合复杂动画和图形效果。

---

### Q5: QML 性能优化

**问题**：如何优化 QML 应用性能？

**参考答案**：

1. **减少绑定复杂度**：避免在绑定中进行复杂计算
2. **使用 Loader**：延迟加载不可见组件
3. **避免 JavaScript**：在性能关键路径使用 C++
4. **使用异步加载**：`Image { asynchronous: true }`
5. **减少层级**：扁平化元素树
6. **使用 ShaderEffect**：GPU 处理复杂效果
7. **避免透明度叠加**：多层半透明元素开销大
8. **使用 `enabled: false`**：禁用不需要交互的元素

---

## 7. 总结

| 主题 | 要点 |
|------|------|
| QML 基础 | 声明式语法、属性绑定、信号槽 |
| Qt Quick | Item 层次、布局系统、Scene Graph |
| C++ 交互 | 上下文属性、类型注册、Q_PROPERTY |
| 动画 | PropertyAnimation、Behavior、动画组合 |
| 性能 | GPU 渲染、批量处理、优化技巧 |

**核心理解**：
1. QML 是声明式 UI 语言，Qt Quick 是 UI 框架
2. 属性绑定是响应式的核心，JavaScript 赋值会破坏绑定
3. Scene Graph 使用 GPU 渲染，性能优于 QPainter
4. C++ 处理业务逻辑，QML 处理 UI
5. 动画系统强大灵活，支持组合和状态机
