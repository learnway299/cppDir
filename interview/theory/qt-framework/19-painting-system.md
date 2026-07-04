# Qt 绘制/渲染系统原理（Painting System）

## 概述

Qt 的绘制系统是一个多层抽象架构，从高层的 QPainter API 到底层的平台渲染后端。理解这套系统对于优化渲染性能、解决绘制问题至关重要。

---

## 1. 绘制设备体系

### 1.1 QPaintDevice 继承体系

**面试问题**：Qt 中绘制是在什么上进行的？QPaintDevice 是什么？

**答案**：

所有可以被 QPainter 绑定的对象都继承自 `QPaintDevice`：

```
QPaintDevice（抽象基类）
├── QWidget          - 屏幕控件
├── QPixmap          - 优化的屏幕图像（显存）
├── QImage           - 独立于硬件的图像（内存）
├── QPicture         - 绘制命令记录器
├── QOpenGLPaintDevice - OpenGL 渲染表面
├── QPdfWriter       - PDF 输出
├── QPrinter         - 打印机
└── QSvgGenerator    - SVG 输出
```

```cpp
// QPaintDevice 核心接口
class QPaintDevice {
public:
    virtual int devType() const;           // 设备类型
    virtual QPaintEngine* paintEngine() const = 0;  // 绘制引擎

    int width() const;          // 宽度（逻辑像素）
    int height() const;         // 高度（逻辑像素）
    int widthMM() const;        // 物理宽度（毫米）
    int heightMM() const;       // 物理高度（毫米）
    int logicalDpiX() const;    // 逻辑 DPI
    int logicalDpiY() const;
    int physicalDpiX() const;   // 物理 DPI
    int physicalDpiY() const;
    qreal devicePixelRatio() const;  // 设备像素比（高 DPI）
};
```

### 1.2 逻辑绘制 vs 硬件操作

**面试问题**：QPainter 的绘制是逻辑操作还是直接操作硬件？

**答案**：

**QPainter 是逻辑层 API**，不直接操作硬件。绘制流程如下：

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Application)                     │
│  QPainter::drawRect(), drawText(), drawPixmap() ...         │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    绘制引擎层 (paintEngine)                    │
│  QPaintEngine - 将绘制命令转换为底层操作                      │
│  ├── QRasterPaintEngine  (软件光栅化)                        │
│  ├── QOpenGLPaintEngine  (OpenGL 加速)                       │
│  └── QPdfEngine, QSvgEngine, QPrintEngine...                │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  平台抽象层 (QPA - Qt Platform Abstraction)   │
│  QPlatformBackingStore - 管理窗口后备缓冲区                   │
│  QPlatformWindow - 平台原生窗口封装                           │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      操作系统 / 硬件层                        │
│  Windows: GDI/Direct2D    macOS: CoreGraphics/Metal         │
│  Linux: X11/Wayland       Embedded: DirectFB/EGLFS          │
└─────────────────────────────────────────────────────────────┘
```

```cpp
// 绘制引擎的选择是透明的
void MyWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // 这些调用不直接操作硬件
    // 而是生成绘制命令，由 paintEngine 处理
    painter.drawRect(10, 10, 100, 50);
    painter.drawText(20, 30, "Hello");

    // QPainter 内部调用:
    // paintEngine()->drawRect(...)
    // paintEngine()->drawTextItem(...)
}

// 获取当前使用的绘制引擎
void checkPaintEngine(QWidget* widget) {
    QPainter painter(widget);
    QPaintEngine* engine = painter.paintEngine();

    qDebug() << "Engine type:" << engine->type();
    // 可能输出: Raster, OpenGL2, CoreGraphics, Direct2D...
}
```

### 1.3 渲染后端类型

**面试问题**：Qt 有哪些渲染后端？

**答案**：

| 后端 | 说明 | 使用场景 |
|------|------|----------|
| **Raster** | 纯软件光栅化，CPU 渲染 | 默认后端，兼容性最好 |
| **OpenGL** | GPU 加速渲染 | 高性能 2D/3D 图形 |
| **Direct2D** | Windows GPU 加速 | Windows 平台优化 |
| **CoreGraphics** | macOS 原生渲染 | macOS 平台 |
| **RHI** | Qt 6 抽象渲染层 | 统一 Vulkan/Metal/D3D12/OpenGL |

```cpp
// Qt 5: 强制使用 OpenGL 后端
int main(int argc, char* argv[]) {
    // 方式 1: 环境变量
    qputenv("QT_QUICK_BACKEND", "opengl");

    // 方式 2: 应用属性
    QApplication::setAttribute(Qt::AA_UseOpenGLES);

    QApplication app(argc, argv);
    // ...
}

// Qt 6: 使用 RHI 抽象层
// 自动选择最佳后端: Vulkan > Metal > D3D12 > OpenGL
```

---

## 2. Qt 坐标系统

### 2.1 Qt 坐标系 vs 数学坐标系

**面试问题**：Qt 坐标系与数学/经典坐标系有什么区别？

**答案**：

```
数学坐标系 (Cartesian)          Qt 坐标系 (Screen)
        Y                              (0,0)────────► X
        ▲                                │
        │                                │
        │                                │
        │                                ▼
────────┼────────► X                     Y
        │
        │
       (0,0)

Y 轴向上，原点在左下              Y 轴向下，原点在左上
```

```cpp
// Qt 坐标系特点
void paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // (0, 0) 在左上角
    // X 轴向右增大
    // Y 轴向下增大

    painter.drawRect(0, 0, 100, 50);
    //              ↑  ↑
    //           左上角坐标

    // 如果需要数学坐标系，可以翻转 Y 轴
    painter.scale(1, -1);
    painter.translate(0, -height());
    // 现在 Y 轴向上，原点在左下
}
```

**为什么 Qt 使用屏幕坐标系？**

1. **与硬件一致**：显示器扫描从左上角开始
2. **文字排版自然**：文字从上往下排列
3. **GUI 布局直观**：控件从上往下、从左往右摆放
4. **与所有主流 GUI 系统一致**：Windows、macOS、X11 都使用这套坐标系

### 2.2 多种坐标系统

**面试问题**：Qt 有哪些坐标系统？它们之间如何转换？

**答案**：

```cpp
class CoordinateSystemDemo : public QWidget {
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);

        // 1. 设备坐标 (Device Coordinates)
        //    - 像素级别，(0,0) 在 widget 左上角
        //    - 这是最终渲染到屏幕的坐标

        // 2. 逻辑坐标 (Logical Coordinates)
        //    - 经过变换矩阵 (QTransform) 转换后的坐标
        //    - 由 translate/rotate/scale 影响

        painter.translate(100, 100);  // 平移原点
        painter.rotate(45);            // 旋转
        painter.scale(2, 2);           // 缩放

        // 此时逻辑坐标 (0,0) 对应设备坐标约 (100,100)

        // 3. 窗口坐标 (Window) 和视口坐标 (Viewport)
        //    - 用于建立逻辑坐标到设备坐标的映射

        painter.setWindow(0, 0, 100, 100);   // 逻辑范围
        painter.setViewport(0, 0, 200, 200); // 设备范围
        // 现在逻辑坐标 (50, 50) 映射到设备坐标 (100, 100)
    }
};

// 坐标转换工具函数
void coordinateConversion(QWidget* widget) {
    // Widget 坐标 → 全局屏幕坐标
    QPoint global = widget->mapToGlobal(QPoint(10, 10));

    // 全局屏幕坐标 → Widget 坐标
    QPoint local = widget->mapFromGlobal(global);

    // 父控件坐标 → 子控件坐标
    QPoint childPos = widget->mapFromParent(QPoint(50, 50));

    // 子控件坐标 → 父控件坐标
    QPoint parentPos = widget->mapToParent(QPoint(10, 10));
}
```

### 2.3 逻辑像素 vs 设备像素（高 DPI）

**面试问题**：Qt 如何处理高 DPI 显示器？

**答案**：

```cpp
// 设备像素比 (Device Pixel Ratio, DPR)
// DPR = 物理像素 / 逻辑像素
// 普通显示器: DPR = 1.0
// Retina/4K: DPR = 2.0 或更高

class HighDpiWidget : public QWidget {
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);

        // 获取设备像素比
        qreal dpr = devicePixelRatioF();
        qDebug() << "Device Pixel Ratio:" << dpr;

        // 方式 1：使用逻辑像素（推荐）
        // Qt 自动处理缩放，代码与 DPI 无关
        painter.drawRect(10, 10, 100, 50);  // 逻辑像素

        // 方式 2：手动处理（需要自己管理高分辨率资源）
        QPixmap pixmap("icon.png");
        // 在高 DPI 下可能模糊

        // 方式 3：使用 @2x 资源
        // icon.png (100x100) + icon@2x.png (200x200)
        // Qt 自动选择合适的版本
        QPixmap autoPixmap(":/icons/icon.png");

        // 方式 4：手动创建高 DPI 位图
        QPixmap highDpiPixmap(100 * dpr, 100 * dpr);
        highDpiPixmap.setDevicePixelRatio(dpr);
        // 绘制时使用逻辑尺寸
        painter.drawPixmap(0, 0, highDpiPixmap);  // 显示为 100x100 逻辑像素
    }
};

// Qt 5 启用高 DPI 支持
int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    // ...
}

// Qt 6 默认启用高 DPI 支持
```

---

## 3. QWindow 与 QWidget 层级关系

### 3.1 类继承体系

**面试问题**：QWindow 和 QWidget 是什么关系？

**答案**：

```
QObject
├── QWindow            (Qt 5+) 轻量级窗口，直接对应原生窗口
│   ├── QRasterWindow  软件渲染窗口
│   └── QOpenGLWindow  OpenGL 渲染窗口
│
└── QWidget            传统控件（内部可能包含 QWindow）
    ├── QMainWindow
    ├── QDialog
    ├── QFrame
    │   └── QLabel, QLineEdit...
    └── QAbstractButton
        └── QPushButton, QCheckBox...
```

```cpp
// QWindow - Qt 5 引入的底层窗口类
// 直接封装原生窗口句柄 (HWND, NSWindow, X11 Window)

class MyWindow : public QRasterWindow {
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(0, 0, width(), height(), Qt::white);
        painter.drawText(10, 30, "QWindow 直接渲染");
    }
};

// QWidget - 传统控件基类
// 可能是"原生窗口"或"非原生子控件"

class MyWidget : public QWidget {
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::lightGray);
        painter.drawText(10, 30, "QWidget 渲染");
    }
};
```

### 3.2 顶层窗口 vs 子控件

**面试问题**：Qt 中的 Native Window 和 Non-native Widget 有什么区别？

**答案**：

```cpp
// 顶层窗口 (Top-Level Window)
// - 没有父控件，或父控件为 nullptr
// - 拥有独立的原生窗口句柄
// - 可以出现在任务栏

QWidget* topLevel = new QWidget(nullptr);  // 顶层窗口
topLevel->setWindowTitle("Top Level");
topLevel->show();

// 子控件 (Child Widget)
// - 有父控件
// - 默认是"非原生"的，不拥有独立窗口句柄
// - 渲染在父控件的缓冲区上

QWidget* child = new QWidget(topLevel);    // 子控件
child->setGeometry(10, 10, 100, 50);
// child 不会有独立的 HWND/NSView，只是父控件的一部分

// 强制子控件成为原生窗口
child->setAttribute(Qt::WA_NativeWindow);
// 或
child->winId();  // 调用 winId() 会强制创建原生窗口

// 检查是否是原生窗口
bool isNative = child->testAttribute(Qt::WA_NativeWindow);
```

**Native vs Non-native 对比：**

| 特性 | Native Window | Non-native Widget |
|------|---------------|-------------------|
| 窗口句柄 | 有独立句柄 | 共享父控件句柄 |
| 渲染缓冲区 | 独立缓冲区 | 父控件缓冲区的一部分 |
| 内存开销 | 较高 | 较低 |
| 透明/混合 | 可能有限制 | 灵活 |
| 遮挡处理 | 操作系统处理 | Qt 自己管理 Z 序 |
| 嵌入外部内容 | 支持 | 不支持 |

```cpp
// 何时需要 Native Window？
void whenNeedNativeWindow() {
    // 1. 嵌入外部窗口（如视频播放器、浏览器控件）
    QWidget* container = new QWidget();
    container->setAttribute(Qt::WA_NativeWindow);
    WId handle = container->winId();
    // 将 handle 传给外部库

    // 2. OpenGL 渲染（Qt 5.4+）
    QOpenGLWidget* glWidget = new QOpenGLWidget(parent);
    // QOpenGLWidget 内部使用 Native Window

    // 3. 某些平台特性需要原生句柄
    #ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(widget->winId());
    // 使用 Windows API 操作窗口
    #endif
}
```

### 3.3 窗口层级与 Z 序

**面试问题**：Qt 中如何管理控件的层叠顺序？

**答案**：

```cpp
void zOrderDemo(QWidget* parent) {
    QLabel* label1 = new QLabel("Bottom", parent);
    label1->setGeometry(10, 10, 100, 100);
    label1->setStyleSheet("background: red;");

    QLabel* label2 = new QLabel("Middle", parent);
    label2->setGeometry(30, 30, 100, 100);
    label2->setStyleSheet("background: green;");

    QLabel* label3 = new QLabel("Top", parent);
    label3->setGeometry(50, 50, 100, 100);
    label3->setStyleSheet("background: blue;");

    // 默认 Z 序：后创建的在上面

    // 调整 Z 序
    label1->raise();       // 提升到最上层
    label3->lower();       // 降到最下层
    label2->stackUnder(label1);  // 放到 label1 下面

    // 对于顶层窗口
    QWidget* window = new QWidget();
    window->setWindowFlags(Qt::WindowStaysOnTopHint);  // 总在最上层
    window->setWindowFlags(Qt::WindowStaysOnBottomHint); // 总在最下层
}
```

---

## 4. 跨平台绘制实现

### 4.1 QPA (Qt Platform Abstraction)

**面试问题**：Qt 如何实现跨平台渲染？

**答案**：

Qt 5 引入了 QPA（Qt Platform Abstraction），将平台相关代码隔离到插件中：

```
┌──────────────────────────────────────────┐
│           Qt 应用程序 / Widgets           │
└──────────────────────────────────────────┘
                    │
                    ▼
┌──────────────────────────────────────────┐
│         Qt GUI / QPainter / QWindow       │
└──────────────────────────────────────────┘
                    │
                    ▼
┌──────────────────────────────────────────┐
│              QPA 接口层                   │
│  QPlatformIntegration                    │
│  QPlatformWindow                         │
│  QPlatformBackingStore                   │
│  QPlatformOpenGLContext                  │
└──────────────────────────────────────────┘
                    │
        ┌───────────┼───────────┬──────────────┐
        ▼           ▼           ▼              ▼
   ┌─────────┐ ┌─────────┐ ┌─────────┐   ┌──────────┐
   │ Windows │ │  macOS  │ │  X11    │   │  Wayland │
   │ Plugin  │ │ Plugin  │ │ Plugin  │   │  Plugin  │
   │ (qwindows)│(qcocoa) │ │ (qxcb)  │   │(qwayland)│
   └─────────┘ └─────────┘ └─────────┘   └──────────┘
        │           │           │              │
        ▼           ▼           ▼              ▼
   ┌─────────┐ ┌─────────┐ ┌─────────┐   ┌──────────┐
   │ Win32   │ │ Cocoa   │ │ X11     │   │ Wayland  │
   │ API     │ │ API     │ │ Protocol│   │ Protocol │
   └─────────┘ └─────────┘ └─────────┘   └──────────┘
```

```cpp
// QPA 核心类
class QPlatformIntegration {
public:
    // 创建平台窗口
    virtual QPlatformWindow* createPlatformWindow(QWindow*) = 0;

    // 创建后备存储（渲染缓冲区）
    virtual QPlatformBackingStore* createPlatformBackingStore(QWindow*) = 0;

    // 创建 OpenGL 上下文
    virtual QPlatformOpenGLContext* createPlatformOpenGLContext(QOpenGLContext*);

    // 获取可用屏幕
    virtual QAbstractEventDispatcher* createEventDispatcher() = 0;
};

// 后备存储 - 管理窗口的渲染缓冲区
class QPlatformBackingStore {
public:
    // 开始绘制
    virtual QPaintDevice* paintDevice() = 0;

    // 将缓冲区刷新到屏幕
    virtual void flush(QWindow*, const QRegion&, const QPoint&) = 0;

    // 调整缓冲区大小
    virtual void resize(const QSize&, const QRegion&) = 0;
};
```

### 4.2 渲染流程详解

**面试问题**：从 QPainter::drawRect() 到像素显示在屏幕上，经历了哪些步骤？

**答案**：

```cpp
// 完整的渲染流程

// 1. 应用层调用
void MyWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawRect(10, 10, 100, 50);
}

// 2. QPainter 转发到 paintEngine
// QPainter::drawRect() 内部：
//   d->engine->drawRects(&rect, 1);

// 3. QPaintEngine 执行实际绘制
// 对于 QRasterPaintEngine：
//   - 将矩形光栅化为像素
//   - 写入到后备缓冲区 (QImage)

// 4. paintEvent 返回后，Qt 自动 flush
// QWidgetRepaintManager::flush()
//   - 将脏区域的像素复制到 QPlatformBackingStore
//   - 调用 QPlatformBackingStore::flush()

// 5. 平台后备存储刷新到屏幕
// Windows: BitBlt() 或 Direct2D
// macOS: CGContextDrawImage() 或 Metal
// X11: XPutImage() 或 XCopyArea()
// Wayland: wl_surface_commit()
```

```cpp
// 不同平台的实现差异
#ifdef Q_OS_WIN
// Windows 平台
class QWindowsBackingStore : public QPlatformBackingStore {
    void flush(QWindow* window, const QRegion& region, const QPoint& offset) override {
        // 使用 GDI BitBlt 或 Direct2D
        HDC dc = GetDC(hwnd);
        BitBlt(dc, ...);
        ReleaseDC(hwnd, dc);
    }
};
#endif

#ifdef Q_OS_MACOS
// macOS 平台
class QCocoaBackingStore : public QPlatformBackingStore {
    void flush(QWindow* window, const QRegion& region, const QPoint& offset) override {
        // 使用 Core Graphics
        CGContextDrawImage(context, rect, cgImage);
    }
};
#endif
```

### 4.3 Qt 6 RHI (Rendering Hardware Interface)

**面试问题**：Qt 6 的 RHI 是什么？

**答案**：

```cpp
// RHI = Rendering Hardware Interface
// Qt 6 引入的统一图形抽象层

// RHI 支持的后端：
// - Vulkan (跨平台)
// - Metal (macOS/iOS)
// - Direct3D 11/12 (Windows)
// - OpenGL / OpenGL ES (兼容后端)

// RHI 架构
┌──────────────────────────────────┐
│      Qt Quick / QPainter         │
└──────────────────────────────────┘
                │
                ▼
┌──────────────────────────────────┐
│           QRhi (抽象层)           │
└──────────────────────────────────┘
                │
    ┌───────────┼───────────┬───────────┐
    ▼           ▼           ▼           ▼
┌────────┐ ┌────────┐ ┌─────────┐ ┌─────────┐
│ Vulkan │ │ Metal  │ │ D3D11/12│ │ OpenGL  │
└────────┘ └────────┘ └─────────┘ └─────────┘

// 选择 RHI 后端
int main(int argc, char* argv[]) {
    // 环境变量方式
    qputenv("QSG_RHI_BACKEND", "vulkan");  // vulkan, metal, d3d11, opengl

    QApplication app(argc, argv);
    // ...
}
```

---

## 5. 更新与重绘机制

### 5.1 update() vs repaint()

**面试问题**：update() 和 repaint() 有什么区别？

**答案**：

```cpp
class UpdateVsRepaint : public QWidget {
public:
    void triggerUpdate() {
        // update() - 推荐方式
        // 1. 异步：投递一个绘制事件到事件队列
        // 2. 合并：多次 update() 会合并为一次 paintEvent
        // 3. 优化：只在事件循环空闲时重绘

        update();           // 标记整个 widget 需要重绘
        update(QRect(0, 0, 100, 100));  // 只标记特定区域
        update(QRegion(...));

        // 连续调用不会多次触发 paintEvent
        update();
        update();
        update();  // 这三次会合并为一次
    }

    void triggerRepaint() {
        // repaint() - 谨慎使用
        // 1. 同步：立即调用 paintEvent()
        // 2. 不合并：每次调用都会触发
        // 3. 可能导致性能问题

        repaint();          // 立即重绘整个 widget
        repaint(QRect(...)); // 立即重绘特定区域

        // ⚠️ 危险：在 paintEvent 中调用 repaint 会导致无限循环
    }
};
```

**使用建议：**

| 场景 | 推荐方法 |
|------|----------|
| 动画/常规更新 | `update()` |
| 必须立即显示 | `repaint()` |
| 高频更新（如游戏） | `update()` + 定时器 |
| 响应用户输入 | `update()` |

### 5.2 脏区域管理

**面试问题**：Qt 如何管理需要重绘的区域？

**答案**：

```cpp
class DirtyRegionDemo : public QWidget {
protected:
    void paintEvent(QPaintEvent* event) override {
        // event->rect() 返回需要重绘的区域（脏区域）
        QRect dirtyRect = event->rect();

        // event->region() 返回更精确的脏区域（可能是复杂形状）
        QRegion dirtyRegion = event->region();

        QPainter painter(this);

        // 优化：只重绘脏区域
        // 方式 1：检查并跳过不需要绘制的内容
        if (!dirtyRegion.intersects(someExpensiveArea)) {
            // 跳过昂贵的绘制操作
        }

        // 方式 2：设置裁剪区域
        painter.setClipRegion(dirtyRegion);

        // 绘制操作...
    }

    void updatePart() {
        // 只标记部分区域为脏
        update(QRect(10, 10, 50, 50));

        // 多个区域会自动合并
        update(QRect(100, 100, 50, 50));
        update(QRect(120, 120, 50, 50));
        // 最终脏区域是这些矩形的并集
    }
};

// 脏区域合并策略
void dirtyRegionMerging() {
    // Qt 内部使用 QWidgetRepaintManager 管理脏区域

    // 合并策略：
    // 1. 相邻/重叠的矩形合并为更大的矩形
    // 2. 太多小区域时合并为一个大矩形
    // 3. 考虑子控件的边界

    // 设置属性影响重绘行为
    widget->setAttribute(Qt::WA_OpaquePaintEvent);
    // 告诉 Qt 此控件会完全覆盖背景，无需先清除

    widget->setAttribute(Qt::WA_NoSystemBackground);
    // 不绘制背景
}
```

### 5.3 重绘触发时机

**面试问题**：什么情况会触发控件重绘？

**答案**：

```cpp
// 1. 显式调用 update() 或 repaint()
widget->update();

// 2. 控件首次显示
widget->show();

// 3. 窗口大小改变
void resizeEvent(QResizeEvent* event) {
    // 自动触发整个控件的重绘
}

// 4. 控件被遮挡后再次露出
// 由窗口系统自动处理

// 5. 滚动
scrollArea->scroll(dx, dy);
// 只重绘新露出的部分

// 6. 属性变化
widget->setEnabled(false);  // 触发重绘
widget->setVisible(true);   // 触发重绘
widget->setStyleSheet(...); // 触发重绘

// 7. 父控件重绘时（透明控件）
parentWidget->update();
// 如果子控件是透明的，也需要重绘

// 8. 布局变化
layout->invalidate();
// 可能触发相关控件重绘
```

### 5.4 重绘频率与 VSync

**面试问题**：Qt 重绘的频率是多少？如何与显示器刷新同步？

**答案**：

```cpp
// Qt Widgets 默认不与 VSync 同步
// 重绘频率取决于 update() 调用频率和事件循环处理速度

class AnimationWidget : public QWidget {
public:
    AnimationWidget() {
        // 使用定时器控制重绘频率
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));

        // 60 FPS
        timer->start(1000 / 60);  // 约 16ms

        // 或者使用 QBasicTimer 更轻量
        // basicTimer.start(16, this);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        // 绘制动画帧
    }

    // 使用 QBasicTimer
    void timerEvent(QTimerEvent* event) override {
        if (event->timerId() == basicTimer.timerId()) {
            update();
        }
    }

private:
    QBasicTimer basicTimer;
};

// Qt Quick / QML 默认与 VSync 同步
// 使用 QQuickWindow::frameSwapped() 信号

// 强制 VSync（需要 OpenGL）
void enableVSync(QOpenGLWidget* glWidget) {
    QSurfaceFormat format;
    format.setSwapInterval(1);  // 1 = VSync 开启, 0 = 关闭
    glWidget->setFormat(format);
}
```

### 5.5 部分重绘 vs 全窗口重绘

**面试问题**：Qt 是如何决定重绘整个窗口还是部分区域的？

**答案**：

```cpp
// 控件级别的重绘
void partialUpdate() {
    QWidget* child = new QWidget(parent);

    // 只更新子控件
    child->update();
    // 只有 child 的 paintEvent 被调用

    // 如果 child 是透明的
    child->setAttribute(Qt::WA_TranslucentBackground);
    child->update();
    // 父控件也需要重绘（以提供背景）
}

// 影响重绘范围的属性
void paintingAttributes() {
    // 1. 不透明控件：只重绘自己
    widget->setAttribute(Qt::WA_OpaquePaintEvent);

    // 2. 透明控件：需要重绘父控件
    widget->setAttribute(Qt::WA_TranslucentBackground);

    // 3. 静态内容：内容不变时跳过重绘
    widget->setAttribute(Qt::WA_StaticContents);
    // 窗口放大时，只重绘新增区域

    // 4. 无系统背景
    widget->setAttribute(Qt::WA_NoSystemBackground);
}

// Graphics View 的优化更新模式
void graphicsViewUpdateMode(QGraphicsView* view) {
    // 最小更新：只重绘改变的项
    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    // 智能更新：Qt 自动选择策略
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // 边界矩形更新：重绘所有改变项的边界矩形
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    // 全视口更新：每次都重绘整个视口
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}
```

---

## 6. 常见面试题

### Q1: QPainter 必须在 paintEvent 中使用吗？

**答案**：

```cpp
// 不是！QPainter 可以绘制到任何 QPaintDevice

// 1. 在 paintEvent 中（最常见）
void paintEvent(QPaintEvent*) override {
    QPainter painter(this);  // 绘制到 QWidget
    painter.drawRect(10, 10, 100, 50);
}

// 2. 绘制到 QPixmap
QPixmap pixmap(200, 200);
pixmap.fill(Qt::white);
{
    QPainter painter(&pixmap);
    painter.drawEllipse(10, 10, 180, 180);
}  // painter 析构，结束绘制
pixmap.save("output.png");

// 3. 绘制到 QImage
QImage image(200, 200, QImage::Format_ARGB32);
QPainter painter(&image);
painter.fillRect(0, 0, 200, 200, Qt::blue);
painter.end();

// 4. 绘制到 QPdfWriter
QPdfWriter pdf("document.pdf");
QPainter painter(&pdf);
painter.drawText(100, 100, "PDF Content");
painter.end();
```

---

### Q2: 为什么子控件的绘制顺序很重要？

**答案**：

```cpp
// 子控件按创建顺序绘制（后创建的在上面）
// 这影响遮挡关系

void paintEvent(QPaintEvent*) override {
    QPainter painter(this);

    // 1. 先绘制父控件自身内容

    // 2. 然后 Qt 按顺序绘制子控件
    //    - 先创建的先绘制（在下层）
    //    - 后创建的后绘制（在上层）

    // 3. 可以通过 raise()/lower() 调整
}

// 注意：paintEvent 只负责绘制自身
// 子控件有自己的 paintEvent
class ParentWidget : public QWidget {
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::gray);  // 绘制背景
        // 子控件的绘制由各自的 paintEvent 处理
    }
};
```

---

### Q3: 如何实现控件的透明效果？

**答案**：

```cpp
class TransparentWidget : public QWidget {
public:
    TransparentWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 方式 1：设置窗口透明度（整体）
        setWindowOpacity(0.8);  // 0.0 完全透明，1.0 完全不透明

        // 方式 2：透明背景（需要父控件支持）
        setAttribute(Qt::WA_TranslucentBackground);

        // 方式 3：顶层窗口完全透明（用于异形窗口）
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 使用半透明颜色
        painter.setBrush(QColor(255, 0, 0, 128));  // RGBA，128 = 50% 透明
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 10, 10);
    }
};
```

---

### Q4: 双缓冲是如何工作的？

**答案**：

```cpp
// Qt Widgets 默认启用双缓冲
// 所有绘制先在后备缓冲区进行，完成后一次性刷新到屏幕

// 双缓冲原理：
┌─────────────────────────────────────────────┐
│              应用程序绘制                    │
│     QPainter painter(widget)                 │
│     painter.drawRect(...)                    │
│     painter.drawText(...)                    │
└─────────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────┐
│            后备缓冲区 (Back Buffer)          │
│         QImage / QPixmap 在内存中            │
│              用户看不到                      │
└─────────────────────────────────────────────┘
                    │
               paintEvent 返回后
                    │
                    ▼
┌─────────────────────────────────────────────┐
│            前台缓冲区 (Front Buffer)         │
│              屏幕显示                        │
└─────────────────────────────────────────────┘

// 禁用双缓冲（很少需要）
widget->setAttribute(Qt::WA_PaintOnScreen);
// 警告：可能导致闪烁

// 手动实现双缓冲（不需要，Qt 自动处理）
class ManualDoubleBuffer : public QWidget {
    QPixmap buffer;

protected:
    void resizeEvent(QResizeEvent*) override {
        buffer = QPixmap(size());
    }

    void paintEvent(QPaintEvent*) override {
        // 绘制到缓冲区
        QPainter bufferPainter(&buffer);
        paintContent(&bufferPainter);
        bufferPainter.end();

        // 将缓冲区绘制到屏幕
        QPainter screenPainter(this);
        screenPainter.drawPixmap(0, 0, buffer);
    }

    void paintContent(QPainter* painter) {
        // 实际绘制内容
    }
};
```

---

### Q5: 如何优化大量控件的渲染性能？

**答案**：

```cpp
// 1. 使用视口裁剪
void paintEvent(QPaintEvent* event) override {
    QPainter painter(this);

    // 只处理可见区域内的元素
    QRect visibleRect = event->rect();

    for (const auto& item : items) {
        if (item.rect.intersects(visibleRect)) {
            drawItem(&painter, item);
        }
    }
}

// 2. 使用缓存
class CachedWidget : public QWidget {
    QPixmap cache;
    bool cacheValid = false;

protected:
    void paintEvent(QPaintEvent*) override {
        if (!cacheValid) {
            cache = QPixmap(size());
            cache.fill(Qt::transparent);
            QPainter cachePainter(&cache);
            paintExpensiveContent(&cachePainter);
            cacheValid = true;
        }

        QPainter painter(this);
        painter.drawPixmap(0, 0, cache);
    }

    void invalidateCache() {
        cacheValid = false;
        update();
    }
};

// 3. 使用 Graphics View 的项缓存
item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

// 4. 减少重绘区域
widget->setAttribute(Qt::WA_OpaquePaintEvent);  // 不透明
widget->setAttribute(Qt::WA_StaticContents);    // 静态内容

// 5. 使用 OpenGL 加速
view->setViewport(new QOpenGLWidget());
```

---

## 7. 总结

Qt 绘制系统的核心要点：

| 概念 | 说明 |
|------|------|
| **QPaintDevice** | 所有可绘制对象的基类 |
| **QPainter** | 高层绘制 API，与设备无关 |
| **QPaintEngine** | 将绘制命令转换为底层操作 |
| **QPA** | 平台抽象层，实现跨平台 |
| **坐标系** | Y 轴向下，原点左上角 |
| **双缓冲** | 默认启用，避免闪烁 |
| **update()** | 异步合并的更新方式（推荐） |
| **repaint()** | 同步立即重绘（谨慎使用） |
| **脏区域** | 只重绘需要更新的区域 |

**面试必记：**

1. QPainter 是逻辑层 API，通过 paintEngine 转换为底层操作
2. Qt 坐标系 Y 轴向下，与数学坐标系相反
3. QWindow 是底层原生窗口封装，QWidget 可能是 Native 或 Non-native
4. update() 异步合并，repaint() 同步立即执行
5. Qt 自动管理脏区域和双缓冲
6. 跨平台通过 QPA 插件实现
