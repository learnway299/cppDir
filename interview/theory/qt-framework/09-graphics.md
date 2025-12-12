# Qt 图形系统（Graphics System）

## 概述

Qt 提供了多层次的图形系统，从底层的 QPainter 到高级的 Graphics View 框架，再到现代的 Qt Quick。理解这些图形系统对于开发高性能的可视化应用至关重要。

---

## 1. QPainter 基础

### 1.1 QPainter 绑定与状态

**面试问题**：如何正确使用 QPainter？

**答案**：

```cpp
#include <QPainter>
#include <QWidget>

class PainterWidget : public QWidget {
protected:
    void paintEvent(QPaintEvent* event) override {
        // 方式 1：构造时绑定
        QPainter painter(this);

        // 方式 2：手动绑定
        QPainter painter2;
        painter2.begin(this);
        // ... 绑定
        painter2.end();

        // 设置渲染提示
        painter.setRenderHint(QPainter::Antialiasing);        // 抗锯齿
        painter.setRenderHint(QPainter::TextAntialiasing);    // 文字抗锯齿
        painter.setRenderHint(QPainter::SmoothPixmapTransform); // 平滑缩放

        // 保存和恢复状态
        painter.save();
        painter.setPen(Qt::red);
        painter.setBrush(Qt::blue);
        painter.translate(100, 100);
        painter.rotate(45);
        // 绑定操作...
        painter.restore();  // 恢复之前的状态

        // 使用画笔（轮廓）
        QPen pen(Qt::black);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);

        // 使用画刷（填充）
        QBrush brush(Qt::green);
        brush.setStyle(Qt::DiagCrossPattern);
        painter.setBrush(brush);

        // 渐变画刷
        QLinearGradient gradient(0, 0, 100, 100);
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, Qt::blue);
        painter.setBrush(gradient);
    }
};
```

---

### 1.2 基本绑定操作

**面试问题**：QPainter 提供了哪些绑定方法？

**答案**：

```cpp
void drawingOperations(QPainter& painter) {
    // 点
    painter.drawPoint(10, 10);
    painter.drawPoints(pointArray, count);

    // 线
    painter.drawLine(0, 0, 100, 100);
    painter.drawLines(lineArray, count);
    painter.drawPolyline(points, pointCount);

    // 矩形
    painter.drawRect(10, 10, 100, 50);
    painter.drawRoundedRect(10, 10, 100, 50, 10, 10);
    painter.fillRect(10, 10, 100, 50, Qt::red);

    // 椭圆/圆
    painter.drawEllipse(QPoint(50, 50), 30, 20);
    painter.drawEllipse(QRect(10, 10, 60, 40));

    // 弧、扇形、弦
    // 角度单位：1/16 度
    painter.drawArc(rect, startAngle * 16, spanAngle * 16);
    painter.drawPie(rect, startAngle * 16, spanAngle * 16);
    painter.drawChord(rect, startAngle * 16, spanAngle * 16);

    // 多边形
    QPolygon polygon;
    polygon << QPoint(10, 10) << QPoint(50, 10) << QPoint(30, 50);
    painter.drawPolygon(polygon);
    painter.drawConvexPolygon(polygon);  // 凸多边形，更快

    // 路径
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(100, 0);
    path.cubicTo(100, 50, 50, 100, 0, 100);
    path.closeSubpath();
    painter.drawPath(path);
    painter.fillPath(path, Qt::blue);
    painter.strokePath(path, QPen(Qt::red, 2));

    // 文字
    painter.drawText(10, 30, "Hello Qt!");
    painter.drawText(rect, Qt::AlignCenter, "Centered Text");

    QFont font("Arial", 16, QFont::Bold);
    painter.setFont(font);

    // 图像
    QPixmap pixmap("image.png");
    painter.drawPixmap(0, 0, pixmap);
    painter.drawPixmap(rect, pixmap);  // 缩放填充

    QImage image("image.png");
    painter.drawImage(0, 0, image);

    // 平铺
    painter.drawTiledPixmap(rect, pixmap);
}
```

---

### 1.3 坐标变换

**面试问题**：如何使用 QPainter 进行坐标变换？

**答案**：

```cpp
void transformDemo(QPainter& painter) {
    // 基本变换
    painter.translate(100, 100);  // 平移
    painter.rotate(45);           // 旋转（度）
    painter.scale(2.0, 2.0);      // 缩放
    painter.shear(0.5, 0);        // 错切

    // 使用 QTransform
    QTransform transform;
    transform.translate(100, 100);
    transform.rotate(45);
    transform.scale(2, 2);
    painter.setTransform(transform);

    // 组合变换
    painter.setTransform(transform, true);  // 与现有变换组合

    // 获取当前变换
    QTransform currentTransform = painter.transform();

    // 重置变换
    painter.resetTransform();

    // 变换坐标点
    QPointF original(10, 20);
    QPointF transformed = transform.map(original);

    // 逆变换
    QTransform inverted = transform.inverted();
    QPointF back = inverted.map(transformed);

    // 世界变换 vs 设备变换
    painter.setWorldMatrixEnabled(true);   // 应用世界变换
    painter.setWorldMatrixEnabled(false);  // 使用设备坐标

    // 窗口-视口变换
    painter.setWindow(0, 0, 100, 100);     // 逻辑坐标范围
    painter.setViewport(0, 0, 200, 200);   // 设备坐标范围
    // 绑定时 (50, 50) 会映射到设备 (100, 100)
}

// 实例：围绕中心旋转
void rotateAroundCenter(QPainter& painter, const QRectF& rect, qreal angle) {
    QPointF center = rect.center();

    painter.save();
    painter.translate(center);
    painter.rotate(angle);
    painter.translate(-center);
    painter.drawRect(rect);
    painter.restore();
}
```

---

### 1.4 裁剪

**面试问题**：如何限制绑定区域？

**答案**：

```cpp
void clippingDemo(QPainter& painter) {
    // 矩形裁剪
    painter.setClipRect(QRect(10, 10, 100, 100));

    // 区域裁剪
    QRegion region(QRect(0, 0, 50, 50));
    region = region.united(QRect(30, 30, 50, 50));
    painter.setClipRegion(region);

    // 路径裁剪
    QPainterPath clipPath;
    clipPath.addEllipse(QRect(0, 0, 100, 100));
    painter.setClipPath(clipPath);

    // 裁剪操作模式
    painter.setClipRect(rect, Qt::ReplaceClip);   // 替换
    painter.setClipRect(rect2, Qt::IntersectClip); // 交集

    // 禁用裁剪
    painter.setClipping(false);

    // 检查裁剪
    bool hasClip = painter.hasClipping();
    QRegion clipRegion = painter.clipRegion();
}
```

---

## 2. 设备绑定

### 2.1 绑定设备类型

**面试问题**：QPainter 可以绑定到哪些设备？

**答案**：

```cpp
#include <QPixmap>
#include <QImage>
#include <QPicture>
#include <QPdfWriter>
#include <QPrinter>

void paintDevices() {
    // 1. QWidget - 屏幕显示
    // 在 paintEvent 中使用

    // 2. QPixmap - 优化的屏幕图像
    QPixmap pixmap(200, 200);
    pixmap.fill(Qt::white);
    QPainter painter1(&pixmap);
    painter1.drawEllipse(10, 10, 180, 180);
    painter1.end();
    pixmap.save("output.png");

    // 3. QImage - 像素级访问
    QImage image(200, 200, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter2(&image);
    painter2.drawRect(10, 10, 180, 180);
    painter2.end();
    // 可以访问像素
    QRgb pixel = image.pixel(100, 100);

    // 4. QPicture - 记录绘图命令
    QPicture picture;
    QPainter painter3(&picture);
    painter3.drawLine(0, 0, 100, 100);
    painter3.end();
    picture.save("drawing.pic");

    // 回放
    QPainter painter4(someWidget);
    picture.play(&painter4);

    // 5. QPdfWriter - PDF 输出
    QPdfWriter pdfWriter("output.pdf");
    pdfWriter.setPageSize(QPageSize::A4);
    QPainter painter5(&pdfWriter);
    painter5.drawText(100, 100, "Hello PDF!");
    painter5.end();

    // 6. QPrinter - 打印
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("print.pdf");
    QPainter painter6(&printer);
    painter6.drawText(100, 100, "Printed text");
    // 新页面
    printer.newPage();
    painter6.drawText(100, 100, "Page 2");
    painter6.end();
}
```

---

### 2.2 QPixmap vs QImage

**面试问题**：QPixmap 和 QImage 有什么区别？

**答案**：

| 特性 | QPixmap | QImage |
|------|---------|--------|
| 存储位置 | 显存（优化） | 内存 |
| 像素访问 | 慢 | 快 |
| 线程安全 | ❌ 只能在 GUI 线程 | ✅ 可在任何线程 |
| 用途 | 屏幕显示 | 图像处理 |
| 文件加载 | 较快 | 较慢 |

```cpp
void pixmapVsImage() {
    // 在工作线程中处理图像
    QImage image = QtConcurrent::run([]() {
        QImage img("large_image.jpg");

        // 像素操作
        for (int y = 0; y < img.height(); ++y) {
            QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
            for (int x = 0; x < img.width(); ++x) {
                // 转灰度
                int gray = qGray(line[x]);
                line[x] = qRgb(gray, gray, gray);
            }
        }

        return img;
    }).result();

    // 转换为 QPixmap 用于显示
    QPixmap pixmap = QPixmap::fromImage(image);
    label->setPixmap(pixmap);

    // QPixmap 到 QImage
    QImage backToImage = pixmap.toImage();
}
```

---

## 3. Graphics View 框架

### 3.1 架构概述

**面试问题**：Graphics View 框架的核心组件是什么？

**答案**：

```cpp
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

void graphicsViewBasics() {
    // Scene - 管理图形项
    QGraphicsScene* scene = new QGraphicsScene();
    scene->setSceneRect(-500, -500, 1000, 1000);

    // View - 显示场景
    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);

    // Item - 图形项
    QGraphicsRectItem* rect = scene->addRect(0, 0, 100, 50);
    QGraphicsEllipseItem* ellipse = scene->addEllipse(0, 0, 80, 80);
    QGraphicsTextItem* text = scene->addText("Hello");
    QGraphicsPixmapItem* pixmap = scene->addPixmap(QPixmap("image.png"));

    // 设置项属性
    rect->setPos(100, 100);
    rect->setRotation(45);
    rect->setBrush(Qt::blue);
    rect->setPen(QPen(Qt::red, 2));
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    rect->setFlag(QGraphicsItem::ItemIsSelectable);

    // 多个视图可以共享一个场景
    QGraphicsView* view2 = new QGraphicsView(scene);
    view2->scale(0.5, 0.5);  // 缩小显示
}
```

---

### 3.2 自定义图形项

**面试问题**：如何创建自定义图形项？

**答案**：

```cpp
#include <QGraphicsItem>

class CustomItem : public QGraphicsItem {
public:
    CustomItem(QGraphicsItem* parent = nullptr)
        : QGraphicsItem(parent)
    {
        setFlag(ItemIsMovable);
        setFlag(ItemIsSelectable);
        setFlag(ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
    }

    // 必须实现：边界矩形
    QRectF boundingRect() const override {
        return QRectF(-50, -50, 100, 100);
    }

    // 必须实现：绑定
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override {
        Q_UNUSED(widget);

        // 根据选中状态改变颜色
        if (option->state & QStyle::State_Selected) {
            painter->setBrush(Qt::yellow);
        } else if (m_hovered) {
            painter->setBrush(Qt::lightGray);
        } else {
            painter->setBrush(Qt::white);
        }

        painter->setPen(QPen(Qt::black, 2));
        painter->drawRoundedRect(boundingRect(), 10, 10);

        painter->drawText(boundingRect(), Qt::AlignCenter, m_text);
    }

    // 可选：精确形状（用于碰撞检测）
    QPainterPath shape() const override {
        QPainterPath path;
        path.addRoundedRect(boundingRect(), 10, 10);
        return path;
    }

    // 鼠标事件
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        qDebug() << "Item clicked at" << event->pos();
        QGraphicsItem::mousePressEvent(event);
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override {
        emit doubleClicked();
        QGraphicsItem::mouseDoubleClickEvent(event);
    }

    // 悬停事件
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
        m_hovered = true;
        update();
        QGraphicsItem::hoverEnterEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        m_hovered = false;
        update();
        QGraphicsItem::hoverLeaveEvent(event);
    }

    // 属性变化通知
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override {
        if (change == ItemPositionHasChanged) {
            qDebug() << "Item moved to" << value.toPointF();
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void setText(const QString& text) {
        m_text = text;
        update();
    }

private:
    QString m_text = "Custom";
    bool m_hovered = false;
};

// 如果需要信号槽，继承 QGraphicsObject
class SignalItem : public QGraphicsObject {
    Q_OBJECT

public:
    // ... 同上

signals:
    void doubleClicked();
    void positionChanged(QPointF newPos);
};
```

---

### 3.3 场景交互

**面试问题**：如何处理 Graphics View 中的交互？

**答案**：

```cpp
class InteractiveView : public QGraphicsView {
    Q_OBJECT

public:
    InteractiveView(QGraphicsScene* scene, QWidget* parent = nullptr)
        : QGraphicsView(scene, parent)
    {
        setDragMode(RubberBandDrag);  // 框选模式
        // setDragMode(ScrollHandDrag);  // 拖动场景
        // setDragMode(NoDrag);

        setTransformationAnchor(AnchorUnderMouse);
        setResizeAnchor(AnchorUnderMouse);
    }

protected:
    // 鼠标滚轮缩放
    void wheelEvent(QWheelEvent* event) override {
        const double scaleFactor = 1.15;

        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }

    // 鼠标事件
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton) {
            // 中键按下，准备拖动
            setDragMode(ScrollHandDrag);
            QMouseEvent fake(QEvent::MouseButtonPress, event->pos(),
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QGraphicsView::mousePressEvent(&fake);
            return;
        }
        QGraphicsView::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton) {
            setDragMode(RubberBandDrag);
        }
        QGraphicsView::mouseReleaseEvent(event);
    }

    // 键盘事件
    void keyPressEvent(QKeyEvent* event) override {
        switch (event->key()) {
        case Qt::Key_Plus:
            scale(1.2, 1.2);
            break;
        case Qt::Key_Minus:
            scale(1.0 / 1.2, 1.0 / 1.2);
            break;
        case Qt::Key_Delete:
            deleteSelectedItems();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
        }
    }

private:
    void deleteSelectedItems() {
        for (QGraphicsItem* item : scene()->selectedItems()) {
            scene()->removeItem(item);
            delete item;
        }
    }
};

// 场景级别的事件处理
class InteractiveScene : public QGraphicsScene {
    Q_OBJECT

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (!item && event->button() == Qt::LeftButton) {
            // 点击空白处，创建新项
            addEllipse(event->scenePos().x() - 25,
                       event->scenePos().y() - 25,
                       50, 50);
        }
        QGraphicsScene::mousePressEvent(event);
    }

    void dropEvent(QGraphicsSceneDragDropEvent* event) override {
        // 处理拖放
        if (event->mimeData()->hasImage()) {
            QImage image = qvariant_cast<QImage>(event->mimeData()->imageData());
            QGraphicsPixmapItem* item = addPixmap(QPixmap::fromImage(image));
            item->setPos(event->scenePos());
        }
    }
};
```

---

### 3.4 碰撞检测与分组

**面试问题**：如何在 Graphics View 中检测碰撞？

**答案**：

```cpp
void collisionDemo(QGraphicsScene* scene) {
    // 获取特定点的项
    QGraphicsItem* item = scene->itemAt(QPointF(100, 100), QTransform());

    // 获取矩形区域内的项
    QList<QGraphicsItem*> items = scene->items(QRectF(0, 0, 200, 200));

    // 获取路径区域内的项
    QPainterPath path;
    path.addEllipse(0, 0, 100, 100);
    QList<QGraphicsItem*> pathItems = scene->items(path);

    // 检测与特定项碰撞
    QGraphicsRectItem* rect = scene->addRect(0, 0, 50, 50);
    QList<QGraphicsItem*> colliding = scene->collidingItems(rect);

    // 在项内检测
    for (QGraphicsItem* other : scene->items()) {
        if (rect->collidesWithItem(other)) {
            qDebug() << "Collision detected!";
        }
    }
}

// 分组
void groupingDemo(QGraphicsScene* scene) {
    QGraphicsRectItem* rect1 = scene->addRect(0, 0, 50, 50);
    QGraphicsRectItem* rect2 = scene->addRect(60, 0, 50, 50);

    // 创建组
    QGraphicsItemGroup* group = scene->createItemGroup({rect1, rect2});

    // 组作为一个整体操作
    group->setPos(100, 100);
    group->setRotation(45);

    // 解组
    scene->destroyItemGroup(group);
    // rect1 和 rect2 仍然存在
}

// 父子关系
void parentChildDemo(QGraphicsScene* scene) {
    QGraphicsRectItem* parent = scene->addRect(0, 0, 100, 100);

    // 子项的坐标相对于父项
    QGraphicsEllipseItem* child = new QGraphicsEllipseItem(10, 10, 30, 30, parent);

    // 父项移动/旋转/缩放时，子项跟随
    parent->setPos(200, 200);
    parent->setRotation(45);
}
```

---

## 4. 动画

### 4.1 属性动画

**面试问题**：如何实现图形项的动画？

**答案**：

```cpp
#include <QPropertyAnimation>
#include <QGraphicsObject>

// QGraphicsObject 支持属性动画
class AnimatedItem : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    QRectF boundingRect() const override {
        return QRectF(-25, -25, 50, 50);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setBrush(Qt::blue);
        painter->drawEllipse(boundingRect());
    }
};

void animationDemo() {
    QGraphicsScene* scene = new QGraphicsScene();
    AnimatedItem* item = new AnimatedItem();
    scene->addItem(item);

    // 位置动画
    QPropertyAnimation* posAnim = new QPropertyAnimation(item, "pos");
    posAnim->setDuration(2000);
    posAnim->setStartValue(QPointF(0, 0));
    posAnim->setEndValue(QPointF(200, 200));
    posAnim->setEasingCurve(QEasingCurve::OutBounce);

    // 旋转动画
    QPropertyAnimation* rotAnim = new QPropertyAnimation(item, "rotation");
    rotAnim->setDuration(2000);
    rotAnim->setStartValue(0);
    rotAnim->setEndValue(360);

    // 透明度动画
    QPropertyAnimation* opacityAnim = new QPropertyAnimation(item, "opacity");
    opacityAnim->setDuration(1000);
    opacityAnim->setStartValue(1.0);
    opacityAnim->setEndValue(0.3);

    // 并行动画组
    QParallelAnimationGroup* parallel = new QParallelAnimationGroup();
    parallel->addAnimation(posAnim);
    parallel->addAnimation(rotAnim);

    // 顺序动画组
    QSequentialAnimationGroup* sequential = new QSequentialAnimationGroup();
    sequential->addAnimation(parallel);
    sequential->addAnimation(opacityAnim);

    // 循环
    sequential->setLoopCount(-1);  // 无限循环

    sequential->start();
}

// 时间线动画
void timelineAnimation(QGraphicsItem* item) {
    QTimeLine* timeline = new QTimeLine(2000);
    timeline->setFrameRange(0, 100);

    QGraphicsItemAnimation* animation = new QGraphicsItemAnimation();
    animation->setItem(item);
    animation->setTimeLine(timeline);

    // 设置关键帧
    animation->setPosAt(0.0, QPointF(0, 0));
    animation->setPosAt(0.5, QPointF(100, 100));
    animation->setPosAt(1.0, QPointF(200, 0));

    animation->setRotationAt(0.0, 0);
    animation->setRotationAt(1.0, 360);

    timeline->start();
}
```

---

### 4.2 状态机动画

**面试问题**：如何使用状态机实现复杂动画？

**答案**：

```cpp
#include <QStateMachine>
#include <QState>
#include <QSignalTransition>

void stateMachineAnimation(QGraphicsObject* item, QPushButton* button) {
    QStateMachine* machine = new QStateMachine();

    // 定义状态
    QState* state1 = new QState();
    state1->assignProperty(item, "pos", QPointF(0, 0));
    state1->assignProperty(item, "rotation", 0);
    state1->assignProperty(item, "scale", 1.0);

    QState* state2 = new QState();
    state2->assignProperty(item, "pos", QPointF(200, 200));
    state2->assignProperty(item, "rotation", 180);
    state2->assignProperty(item, "scale", 1.5);

    QState* state3 = new QState();
    state3->assignProperty(item, "pos", QPointF(100, 0));
    state3->assignProperty(item, "rotation", 360);
    state3->assignProperty(item, "scale", 0.5);

    // 添加转换
    QSignalTransition* t1 = state1->addTransition(button, &QPushButton::clicked, state2);
    QSignalTransition* t2 = state2->addTransition(button, &QPushButton::clicked, state3);
    QSignalTransition* t3 = state3->addTransition(button, &QPushButton::clicked, state1);

    // 添加动画到转换
    QPropertyAnimation* posAnim = new QPropertyAnimation(item, "pos");
    posAnim->setDuration(500);
    posAnim->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation* rotAnim = new QPropertyAnimation(item, "rotation");
    rotAnim->setDuration(500);

    t1->addAnimation(posAnim);
    t1->addAnimation(rotAnim);
    t2->addAnimation(posAnim);
    t2->addAnimation(rotAnim);
    t3->addAnimation(posAnim);
    t3->addAnimation(rotAnim);

    // 启动状态机
    machine->addState(state1);
    machine->addState(state2);
    machine->addState(state3);
    machine->setInitialState(state1);
    machine->start();
}
```

---

## 5. OpenGL 集成

### 5.1 QOpenGLWidget

**面试问题**：如何在 Qt 中使用 OpenGL？

**答案**：

```cpp
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    GLWidget(QWidget* parent = nullptr)
        : QOpenGLWidget(parent) {}

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        // 初始化着色器、缓冲区等
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
        // 更新投影矩阵等
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // OpenGL 绘制调用
    }
};

// 在 Graphics View 中使用 OpenGL
void openglInGraphicsView(QGraphicsView* view) {
    QOpenGLWidget* glWidget = new QOpenGLWidget();
    view->setViewport(glWidget);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}
```

---

## 6. 常见面试题

### Q1: paintEvent 中应该注意什么？

**答案**：

```cpp
void paintEvent(QPaintEvent* event) override {
    // 1. 只重绘需要更新的区域
    QRect dirtyRect = event->rect();

    // 2. QPainter 必须在栈上或确保 end() 被调用
    QPainter painter(this);

    // 3. 不要在 paintEvent 中做耗时操作
    // 错误：painter.drawPixmap(0, 0, loadExpensiveImage());
    // 正确：使用缓存的图像

    // 4. 使用 update() 而不是 repaint() 触发重绘
    // update() 是异步的，多次调用会合并

    // 5. 设置 setAttribute(Qt::WA_OpaquePaintEvent) 如果完全覆盖背景
}
```

---

### Q2: Graphics View 的性能优化？

**答案**：

```cpp
void optimizeGraphicsView(QGraphicsView* view, QGraphicsScene* scene) {
    // 1. 设置场景边界
    scene->setSceneRect(0, 0, 1000, 1000);

    // 2. 使用 BSP 索引（默认）
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    // 或禁用索引（项目很少时）
    // scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // 3. 优化视图更新
    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    // 或 SmartViewportUpdate, BoundingRectViewportUpdate

    // 4. 缓存项
    item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    // 或 ItemCoordinateCache（项目变换时重绘）

    // 5. 使用 OpenGL
    view->setViewport(new QOpenGLWidget());

    // 6. 优化项的 boundingRect
    // 确保返回紧凑的边界

    // 7. 实现精确的 shape() 用于碰撞检测
}
```

---

### Q3: 如何实现高 DPI 支持？

**答案**：

```cpp
int main(int argc, char* argv[]) {
    // Qt 6 默认启用高 DPI
    // Qt 5 需要设置
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    // ...
}

// 绘制时使用逻辑像素
void paintEvent(QPaintEvent*) override {
    QPainter painter(this);

    // 使用设备像素比
    qreal dpr = devicePixelRatioF();

    // 加载高 DPI 图片
    QPixmap pixmap("image@2x.png");  // Qt 自动选择合适分辨率

    // 或手动处理
    QPixmap highDpi(200 * dpr, 200 * dpr);
    highDpi.setDevicePixelRatio(dpr);
}
```

---

## 总结

Qt 图形系统的核心要点：

1. **QPainter** - 底层 2D 绘图 API
2. **绑定设备** - QPixmap（显示）、QImage（处理）、QPicture（记录）
3. **Graphics View** - 高级场景/视图/项架构
4. **动画** - QPropertyAnimation 和状态机
5. **OpenGL** - QOpenGLWidget 集成

选择合适的图形技术取决于应用需求：简单绘图用 QPainter，复杂交互场景用 Graphics View，3D 图形用 OpenGL。
