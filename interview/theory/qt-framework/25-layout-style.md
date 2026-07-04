# Qt 布局与样式深入解析

布局系统和样式系统是 Qt UI 开发的核心，考察面试者对 Qt 渲染机制的理解深度。

---

## 1. 盒子模型

### 1.1 Qt 盒子模型结构

```
┌─────────────────────────────────────────────────────────────┐
│                      Qt 盒子模型                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    margin                            │   │
│  │  ┌───────────────────────────────────────────────┐  │   │
│  │  │                  border                        │  │   │
│  │  │  ┌─────────────────────────────────────────┐  │  │   │
│  │  │  │               padding                    │  │  │   │
│  │  │  │  ┌───────────────────────────────────┐  │  │  │   │
│  │  │  │  │                                   │  │  │  │   │
│  │  │  │  │           contents                │  │  │  │   │
│  │  │  │  │         （控件内容）               │  │  │  │   │
│  │  │  │  │                                   │  │  │  │   │
│  │  │  │  └───────────────────────────────────┘  │  │  │   │
│  │  │  │                                         │  │  │   │
│  │  │  └─────────────────────────────────────────┘  │  │   │
│  │  │                                               │  │   │
│  │  └───────────────────────────────────────────────┘  │   │
│  │                                                      │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                             │
│  各层含义：                                                  │
│  • margin  - 控件与外部的距离（布局管理）                    │
│  • border  - 边框区域                                       │
│  • padding - 边框与内容的距离                               │
│  • contents - 实际内容区域                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 margin vs padding vs spacing

```
┌─────────────────────────────────────────────────────────────┐
│              margin vs padding vs spacing                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  概念              作用域              设置方式              │
│  ─────────────────────────────────────────────────────────  │
│  margin           控件外部            QSS: margin           │
│                   与相邻元素间距      Layout: setContentsMargins │
│                                                             │
│  padding          控件内部            QSS: padding          │
│                   边框到内容距离      （无直接 API）         │
│                                                             │
│  spacing          布局内部            QLayout::setSpacing   │
│                   子控件之间间距      QSS: spacing (部分)   │
│                                                             │
│  contentsMargins  布局边缘            QLayout::setContentsMargins │
│                   布局到父容器边距                          │
│                                                             │
│  示意图：                                                    │
│  ┌────────────────────────────────────────────┐             │
│  │ Parent Widget                              │             │
│  │  ┌──────────────────────────────────────┐  │             │
│  │  │ Layout (contentsMargins)             │  │             │
│  │  │  ┌──────────┐ spacing ┌──────────┐   │  │             │
│  │  │  │ Widget A │←───────→│ Widget B │   │  │             │
│  │  │  │ (margin) │         │ (margin) │   │  │             │
│  │  │  │┌────────┐│         │┌────────┐│   │  │             │
│  │  │  ││padding ││         ││padding ││   │  │             │
│  │  │  ││content ││         ││content ││   │  │             │
│  │  │  │└────────┘│         │└────────┘│   │  │             │
│  │  │  └──────────┘         └──────────┘   │  │             │
│  │  └──────────────────────────────────────┘  │             │
│  └────────────────────────────────────────────┘             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 代码示例

```cpp
// === Layout 设置 ===
QHBoxLayout* layout = new QHBoxLayout(parent);

// 设置布局到父容器边缘的距离（左、上、右、下）
layout->setContentsMargins(10, 10, 10, 10);

// 设置子控件之间的间距
layout->setSpacing(5);

// 获取 contentsMargins
int left, top, right, bottom;
layout->getContentsMargins(&left, &top, &right, &bottom);

// === 控件本身的 margin（通过 QSS）===
widget->setStyleSheet("margin: 5px;");
widget->setStyleSheet("margin: 5px 10px;");           // 上下 5px，左右 10px
widget->setStyleSheet("margin: 1px 2px 3px 4px;");    // 上、右、下、左

// === padding（通过 QSS）===
widget->setStyleSheet("padding: 10px;");

// === 注意区别 ===
// QLayout::setContentsMargins - 影响布局内所有子控件的位置
// QSS margin - 影响单个控件与相邻元素的距离
// QSS padding - 影响控件内容与边框的距离
```

---

## 2. QLayout 体系

### 2.1 布局类型

```
┌─────────────────────────────────────────────────────────────┐
│                    QLayout 继承体系                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  QLayout (抽象基类)                                          │
│  ├── QBoxLayout                                             │
│  │   ├── QHBoxLayout    水平布局                            │
│  │   └── QVBoxLayout    垂直布局                            │
│  ├── QGridLayout        网格布局                            │
│  ├── QFormLayout        表单布局（标签+输入框）              │
│  └── QStackedLayout     堆叠布局（一次显示一个）             │
│                                                             │
│  使用场景：                                                  │
│  ─────────────────────────────────────────────────────────  │
│  QHBoxLayout    工具栏按钮、水平排列的选项                   │
│  QVBoxLayout    垂直表单、侧边栏菜单                         │
│  QGridLayout    复杂表单、计算器界面、棋盘格                 │
│  QFormLayout    登录框、设置面板                             │
│  QStackedLayout 向导页面、Tab 内容区                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 QGridLayout 详解

```cpp
QGridLayout* grid = new QGridLayout(parent);

// 添加控件：addWidget(widget, row, col, rowSpan, colSpan, alignment)
grid->addWidget(label1, 0, 0);                    // (0,0)
grid->addWidget(edit1, 0, 1);                     // (0,1)
grid->addWidget(label2, 1, 0);                    // (1,0)
grid->addWidget(edit2, 1, 1);                     // (1,1)
grid->addWidget(button, 2, 0, 1, 2);              // 跨 2 列

// 设置列拉伸因子
grid->setColumnStretch(0, 1);   // 第 0 列拉伸因子 1
grid->setColumnStretch(1, 2);   // 第 1 列拉伸因子 2（宽度是第 0 列的 2 倍）

// 设置行拉伸因子
grid->setRowStretch(2, 1);

// 设置最小列宽/行高
grid->setColumnMinimumWidth(0, 100);
grid->setRowMinimumHeight(0, 50);

// 设置水平/垂直间距
grid->setHorizontalSpacing(10);
grid->setVerticalSpacing(5);
```

### 2.3 布局嵌套

```cpp
// 复杂布局通常需要嵌套
QVBoxLayout* mainLayout = new QVBoxLayout(this);

// 顶部：水平布局
QHBoxLayout* topLayout = new QHBoxLayout();
topLayout->addWidget(logoLabel);
topLayout->addStretch();
topLayout->addWidget(userButton);
mainLayout->addLayout(topLayout);

// 中间：网格布局
QGridLayout* centerLayout = new QGridLayout();
// ... 添加控件
mainLayout->addLayout(centerLayout, 1);  // stretch=1，占据剩余空间

// 底部：水平布局
QHBoxLayout* bottomLayout = new QHBoxLayout();
bottomLayout->addStretch();
bottomLayout->addWidget(okButton);
bottomLayout->addWidget(cancelButton);
mainLayout->addLayout(bottomLayout);
```

---

## 3. SizePolicy 详解（重点）

### 3.1 六种策略

```
┌─────────────────────────────────────────────────────────────┐
│                    QSizePolicy 策略                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  策略              行为                      典型应用        │
│  ─────────────────────────────────────────────────────────  │
│  Fixed            固定大小，不可伸缩        图标按钮         │
│                   始终使用 sizeHint                         │
│                                                             │
│  Minimum          可以变大，不能小于        带图标的按钮     │
│                   sizeHint 是最小值                         │
│                                                             │
│  Maximum          可以变小，不能大于        受限空间的标签   │
│                   sizeHint 是最大值                         │
│                                                             │
│  Preferred        可大可小，倾向 sizeHint   普通按钮         │
│                   有空间会适当调整                          │
│                                                             │
│  Expanding        尽可能占据更多空间        编辑框、列表     │
│                   优先于 Preferred 获取空间                 │
│                                                             │
│  Ignored          忽略 sizeHint              特殊场景       │
│                   完全由布局决定大小                        │
│                                                             │
│  MinimumExpanding 至少 sizeHint，尽量更大   可拉伸的面板     │
│                   = Minimum + Expanding                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 SizePolicy 与 Stretch 的交互

```
┌─────────────────────────────────────────────────────────────┐
│            SizePolicy + Stretch Factor 规则                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  布局分配额外空间的优先级：                                  │
│                                                             │
│  1. 先满足 Fixed 控件（固定大小）                           │
│  2. 再满足 Minimum 控件的最小需求                           │
│  3. 剩余空间按 Expanding > Preferred 分配                   │
│  4. 同级别按 stretch factor 比例分配                        │
│                                                             │
│  示例：                                                      │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Button (Fixed, 100px)                               │  │
│  │  ├─────────────────────────────────────────────────→ │  │
│  │  Edit1 (Expanding, stretch=1)                        │  │
│  │  ├─────────────────────────────────────────────────→ │  │
│  │  Edit2 (Expanding, stretch=2)                        │  │
│  │  ├─────────────────────────────────────────────────→ │  │
│  │                                                      │  │
│  │  结果：Button 固定 100px                             │  │
│  │        剩余空间 Edit1:Edit2 = 1:2                    │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 代码示例

```cpp
// 设置 SizePolicy
QLabel* label = new QLabel("Name:");
label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

QLineEdit* edit = new QLineEdit();
edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

// 获取当前策略
QSizePolicy policy = widget->sizePolicy();
policy.setHorizontalPolicy(QSizePolicy::Expanding);
policy.setVerticalPolicy(QSizePolicy::Preferred);
policy.setHorizontalStretch(2);  // 水平拉伸因子
policy.setVerticalStretch(1);    // 垂直拉伸因子
widget->setSizePolicy(policy);

// 常见组合
// 固定宽度的标签 + 自适应的输入框
label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

// 占据所有剩余空间的区域
widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
```

### 3.4 sizeHint 与 minimumSizeHint

```cpp
class CustomWidget : public QWidget {
public:
    // sizeHint - 控件的理想大小
    QSize sizeHint() const override {
        return QSize(200, 100);
    }

    // minimumSizeHint - 控件可用的最小大小
    QSize minimumSizeHint() const override {
        return QSize(50, 30);
    }
};

// 强制设置大小（覆盖布局管理）
widget->setFixedSize(100, 100);       // 固定大小
widget->setFixedWidth(100);           // 固定宽度
widget->setFixedHeight(50);           // 固定高度
widget->setMinimumSize(50, 30);       // 最小大小
widget->setMaximumSize(500, 300);     // 最大大小
```

---

## 4. QSpacerItem 与 Stretch

### 4.1 Spacer 使用

```cpp
// 添加弹性空白（Expanding）
layout->addStretch();       // 等价于 addSpacerItem with Expanding
layout->addStretch(1);      // 指定拉伸因子

// 添加固定空白
layout->addSpacing(20);     // 20px 固定间距

// 添加自定义 SpacerItem
QSpacerItem* spacer = new QSpacerItem(
    100, 50,                           // 宽、高
    QSizePolicy::Expanding,            // 水平策略
    QSizePolicy::Minimum               // 垂直策略
);
layout->addItem(spacer);

// 常见模式：按钮右对齐
QHBoxLayout* layout = new QHBoxLayout();
layout->addStretch();                  // 左侧弹性空白
layout->addWidget(okButton);
layout->addWidget(cancelButton);

// 常见模式：按钮居中
QHBoxLayout* layout = new QHBoxLayout();
layout->addStretch();
layout->addWidget(button);
layout->addStretch();

// 常见模式：两端对齐
QHBoxLayout* layout = new QHBoxLayout();
layout->addWidget(leftButton);
layout->addStretch();
layout->addWidget(rightButton);
```

---

## 5. 自定义 Layout

### 5.1 实现自定义布局

```cpp
class FlowLayout : public QLayout {
public:
    explicit FlowLayout(QWidget* parent = nullptr, int margin = -1,
                        int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    // 必须实现的虚函数
    void addItem(QLayoutItem* item) override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;

    // 大小相关
    QSize sizeHint() const override;
    QSize minimumSize() const override;

    // 核心：设置几何形状
    void setGeometry(const QRect& rect) override;

    // 可选
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

private:
    int doLayout(const QRect& rect, bool testOnly) const;

    QList<QLayoutItem*> itemList_;
    int hSpace_;
    int vSpace_;
};

// 核心实现：setGeometry
void FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

int FlowLayout::doLayout(const QRect& rect, bool testOnly) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem* item : itemList_) {
        QWidget* wid = item->widget();
        int spaceX = hSpace_;
        int spaceY = vSpace_;

        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}
```

---

## 6. QSS 样式原理（重点）

### 6.1 QSS 渲染机制

```
┌─────────────────────────────────────────────────────────────┐
│                    QSS 渲染流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 样式解析                                                 │
│     QString styleSheet → QStyleSheetStyle 解析              │
│     生成 StyleRule 规则集                                   │
│                                                             │
│  2. 规则匹配                                                 │
│     根据选择器匹配控件                                       │
│     计算优先级（specificity）                               │
│                                                             │
│  3. 属性合并                                                 │
│     合并继承的样式                                          │
│     处理属性覆盖                                            │
│                                                             │
│  4. 绘制                                                    │
│     QStyleSheetStyle::drawControl()                         │
│     内部调用 QPainter 绑定绘制                              │
│                                                             │
│  核心类：                                                    │
│  ─────────────────────────────────────────────────────────  │
│  QStyleSheetStyle   - QSS 样式代理                          │
│  QRenderRule        - 解析后的渲染规则                      │
│  QStyleOption       - 控件状态信息                          │
│  QPainter           - 实际绑定                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 border-radius 实现原理

```
┌─────────────────────────────────────────────────────────────┐
│                border-radius 实现原理                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  QSS 设置：                                                  │
│  QPushButton { border-radius: 10px; }                       │
│                                                             │
│  内部实现步骤：                                              │
│  ─────────────────────────────────────────────────────────  │
│  1. 解析 border-radius 值                                   │
│                                                             │
│  2. 创建圆角路径                                             │
│     QPainterPath path;                                      │
│     path.addRoundedRect(rect, rx, ry);                      │
│                                                             │
│  3. 设置裁剪区域                                             │
│     painter.setClipPath(path);                              │
│                                                             │
│  4. 绘制背景                                                 │
│     painter.fillPath(path, brush);                          │
│                                                             │
│  5. 绘制边框                                                 │
│     painter.strokePath(path, pen);                          │
│                                                             │
│  等价的手动实现：                                            │
│  ─────────────────────────────────────────────────────────  │
│  void paintEvent(QPaintEvent*) {                            │
│      QPainter painter(this);                                │
│      painter.setRenderHint(QPainter::Antialiasing);         │
│                                                             │
│      QPainterPath path;                                     │
│      path.addRoundedRect(rect(), 10, 10);                   │
│                                                             │
│      painter.setClipPath(path);                             │
│      painter.fillPath(path, QBrush(Qt::blue));              │
│      painter.setPen(QPen(Qt::black, 2));                    │
│      painter.drawPath(path);                                │
│  }                                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.3 border-radius 不生效的原因

```
┌─────────────────────────────────────────────────────────────┐
│            border-radius 不生效的常见原因                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 未设置 border                                           │
│     ─────────────────────────────────────────────────────   │
│     /* 可能不生效 */                                         │
│     QPushButton { border-radius: 10px; }                    │
│                                                             │
│     /* 需要同时设置 border */                                │
│     QPushButton {                                           │
│         border: 1px solid gray;                             │
│         border-radius: 10px;                                │
│     }                                                       │
│                                                             │
│  2. 背景未裁剪                                               │
│     ─────────────────────────────────────────────────────   │
│     /* 背景会超出圆角 */                                     │
│     QLabel {                                                │
│         background: red;                                    │
│         border-radius: 10px;                                │
│     }                                                       │
│                                                             │
│     /* 需要同时设置 border */                                │
│     QLabel {                                                │
│         background: red;                                    │
│         border: 1px solid red;  /* 或 transparent */        │
│         border-radius: 10px;                                │
│     }                                                       │
│                                                             │
│  3. 控件有原生样式                                           │
│     ─────────────────────────────────────────────────────   │
│     某些控件默认使用原生绘制，需要先移除                     │
│     QWidget { background: transparent; }                    │
│                                                             │
│  4. 子控件覆盖                                               │
│     ─────────────────────────────────────────────────────   │
│     子控件背景可能覆盖父控件圆角                             │
│                                                             │
│  5. 圆角值过大                                               │
│     ─────────────────────────────────────────────────────   │
│     border-radius > min(width, height) / 2 会被截断         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.4 实现圆形头像

```cpp
// === 方法 1: QSS ===
// 需要保证宽高相等
avatarLabel->setFixedSize(100, 100);
avatarLabel->setStyleSheet(R"(
    QLabel {
        border-radius: 50px;
        border: 2px solid white;
    }
)");
avatarLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));
// 问题：QSS 不会裁剪 pixmap！

// === 方法 2: 手动裁剪（推荐）===
QPixmap createCircularPixmap(const QPixmap& src, int size) {
    QPixmap result(size, size);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    // 创建圆形裁剪路径
    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    // 绘制缩放后的图片
    QPixmap scaled = src.scaled(size, size,
                                 Qt::KeepAspectRatioByExpanding,
                                 Qt::SmoothTransformation);
    int x = (size - scaled.width()) / 2;
    int y = (size - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    return result;
}

// 使用
avatarLabel->setPixmap(createCircularPixmap(originalPixmap, 100));

// === 方法 3: setMask ===
QPixmap pixmap("avatar.png");
QBitmap mask(pixmap.size());
mask.fill(Qt::color0);

QPainter painter(&mask);
painter.setBrush(Qt::color1);
painter.drawEllipse(0, 0, mask.width(), mask.height());

label->setPixmap(pixmap);
label->setMask(mask);
```

---

## 7. QSS 选择器与伪状态

### 7.1 选择器类型

```css
/* 类型选择器 */
QPushButton { background: blue; }

/* 类选择器（objectName）*/
QPushButton#okButton { background: green; }

/* 后代选择器 */
QDialog QPushButton { background: gray; }

/* 子选择器 */
QFrame > QPushButton { background: red; }

/* 属性选择器 */
QPushButton[flat="true"] { background: transparent; }

/* 通配符 */
* { font-size: 14px; }
```

### 7.2 伪状态

```css
/* 常用伪状态 */
QPushButton:hover { background: lightblue; }
QPushButton:pressed { background: darkblue; }
QPushButton:disabled { background: gray; }
QPushButton:checked { background: green; }
QPushButton:focus { border: 2px solid blue; }

/* 组合状态 */
QPushButton:hover:!pressed { background: lightblue; }
QPushButton:checked:hover { background: lightgreen; }

/* 列表项状态 */
QListWidget::item:selected { background: blue; }
QListWidget::item:hover { background: lightgray; }
QListWidget::item:selected:active { background: darkblue; }

/* 输入框状态 */
QLineEdit:read-only { background: lightgray; }
QLineEdit:focus { border: 2px solid blue; }
```

### 7.3 子控件选择器

```css
/* QComboBox 子控件 */
QComboBox::drop-down {
    width: 20px;
    border: none;
    background: lightgray;
}
QComboBox::down-arrow {
    image: url(arrow_down.png);
    width: 12px;
    height: 12px;
}

/* QScrollBar 子控件 */
QScrollBar::handle:vertical {
    background: gray;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0px;
}

/* QCheckBox 子控件 */
QCheckBox::indicator {
    width: 16px;
    height: 16px;
}
QCheckBox::indicator:checked {
    image: url(checked.png);
}
QCheckBox::indicator:unchecked {
    image: url(unchecked.png);
}

/* QTabWidget 子控件 */
QTabBar::tab {
    background: lightgray;
    padding: 8px 16px;
}
QTabBar::tab:selected {
    background: white;
    border-bottom: 2px solid blue;
}
```

---

## 8. QSS vs 原生绘制

### 8.1 性能对比

```
┌─────────────────────────────────────────────────────────────┐
│                QSS vs paintEvent 性能对比                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  方式              性能        灵活性      维护性            │
│  ─────────────────────────────────────────────────────────  │
│  QSS              中等        高          高                │
│  自定义 paintEvent 高         极高        中                │
│  QStyle 子类       高         极高        低                │
│                                                             │
│  QSS 性能开销来源：                                          │
│  1. 样式字符串解析                                          │
│  2. 选择器匹配                                              │
│  3. 规则合并计算                                            │
│  4. 额外的绑制层                                            │
│                                                             │
│  优化建议：                                                  │
│  1. 避免频繁动态修改 styleSheet                              │
│  2. 使用 property 选择器而非动态修改样式                     │
│  3. 复杂动画使用自定义绑制                                   │
│  4. 大量同类控件考虑 QStyle 子类                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 动态样式切换

```cpp
// === 不推荐：频繁修改 styleSheet ===
void Widget::setHighlighted(bool on) {
    if (on) {
        setStyleSheet("background: yellow;");  // 触发重新解析
    } else {
        setStyleSheet("background: white;");
    }
}

// === 推荐：使用 property + QSS ===
// QSS:
// QPushButton[highlighted="true"] { background: yellow; }
// QPushButton[highlighted="false"] { background: white; }

void Widget::setHighlighted(bool on) {
    setProperty("highlighted", on);
    style()->unpolish(this);  // 强制刷新样式
    style()->polish(this);
    update();
}

// === 更高效：使用 class 动态切换 ===
void Widget::setHighlighted(bool on) {
    if (on) {
        setObjectName("highlighted");
    } else {
        setObjectName("");
    }
    // 使用 qApp->setStyleSheet() 统一样式
}
```

---

## 9. 样式与布局的交互

### 9.1 QSS margin vs Layout margin

```
┌─────────────────────────────────────────────────────────────┐
│            QSS margin vs Layout contentsMargins             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  QLayout::setContentsMargins(10, 10, 10, 10);               │
│  → 影响布局内所有子控件的位置                               │
│  → 布局边缘到父容器边缘的距离                               │
│                                                             │
│  QSS: margin: 10px;                                         │
│  → 影响单个控件的外边距                                     │
│  → 控件与相邻控件的距离                                     │
│                                                             │
│  组合效果：                                                  │
│  ┌──────────────────────────────────────────────┐           │
│  │ Parent                                        │           │
│  │ ┌──────────────────────────────────────────┐ │           │
│  │ │ Layout (contentsMargins: 10px)           │ │           │
│  │ │ ┌───────────────┐ ┌───────────────┐      │ │           │
│  │ │ │ Widget A      │ │ Widget B      │      │ │           │
│  │ │ │ margin: 5px   │ │ margin: 5px   │      │ │           │
│  │ │ └───────────────┘ └───────────────┘      │ │           │
│  │ │        ↑                                  │ │           │
│  │ │   QSS margin 会额外增加间距               │ │           │
│  │ └──────────────────────────────────────────┘ │           │
│  └──────────────────────────────────────────────┘           │
│                                                             │
│  注意：QSS margin 可能与 Layout spacing 叠加！              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 QSS padding 对 sizeHint 的影响

```cpp
// QSS padding 会影响控件的 sizeHint

// 原始按钮
QPushButton* btn = new QPushButton("Click");
// sizeHint 约 (75, 23)

// 添加 padding
btn->setStyleSheet("padding: 20px;");
// sizeHint 变为约 (75+40, 23+40) = (115, 63)

// 这会影响布局计算！
// 如果使用 QSizePolicy::Fixed，需要重新调整 fixedSize
```

---

## 10. 高 DPI 适配

### 10.1 DPI 感知设置

```cpp
// main.cpp
int main(int argc, char *argv[]) {
    // Qt 5.6+：启用高 DPI 缩放
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Qt 5.14+：使用整数缩放因子
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
    );

    QApplication app(argc, argv);
    // ...
}
```

### 10.2 QSS 中的 DPI 适配

```css
/* 问题：固定像素值在高 DPI 下显得过小 */
QPushButton {
    padding: 8px 16px;
    font-size: 14px;
    border-radius: 4px;
}

/* 解决方案 1：使用 em 单位（相对于字体大小）*/
/* Qt 不支持 em，但可以用变量/预处理 */

/* 解决方案 2：动态生成 QSS */
```

```cpp
// 动态生成 DPI 感知的 QSS
QString generateStyleSheet(qreal dpiScale) {
    int padding = qRound(8 * dpiScale);
    int fontSize = qRound(14 * dpiScale);
    int radius = qRound(4 * dpiScale);

    return QString(R"(
        QPushButton {
            padding: %1px;
            font-size: %2px;
            border-radius: %3px;
        }
    )").arg(padding).arg(fontSize).arg(radius);
}

// 使用
qreal scale = qApp->devicePixelRatio();
qApp->setStyleSheet(generateStyleSheet(scale));
```

---

## 11. 动态换肤实现

### 11.1 换肤架构

```cpp
// === ThemeManager 单例 ===
class ThemeManager : public QObject {
    Q_OBJECT
public:
    enum Theme { Light, Dark, Custom };

    static ThemeManager& instance() {
        static ThemeManager inst;
        return inst;
    }

    void setTheme(Theme theme) {
        currentTheme_ = theme;
        applyTheme();
        emit themeChanged(theme);
    }

    QString getColor(const QString& key) const {
        return colors_.value(key);
    }

signals:
    void themeChanged(Theme theme);

private:
    void applyTheme() {
        QString qss;
        switch (currentTheme_) {
            case Light:
                qss = loadQss(":/themes/light.qss");
                loadColors(":/themes/light.json");
                break;
            case Dark:
                qss = loadQss(":/themes/dark.qss");
                loadColors(":/themes/dark.json");
                break;
            case Custom:
                qss = loadQss(customPath_);
                break;
        }
        qApp->setStyleSheet(qss);
    }

    QString loadQss(const QString& path) {
        QFile file(path);
        file.open(QFile::ReadOnly);
        return file.readAll();
    }

    Theme currentTheme_ = Light;
    QMap<QString, QString> colors_;
    QString customPath_;
};
```

### 11.2 主题文件示例

```css
/* light.qss */
QWidget {
    background-color: #FFFFFF;
    color: #333333;
}

QPushButton {
    background-color: #0078D4;
    color: white;
    border: none;
    padding: 8px 16px;
    border-radius: 4px;
}

QPushButton:hover {
    background-color: #1084D8;
}

/* dark.qss */
QWidget {
    background-color: #1E1E1E;
    color: #D4D4D4;
}

QPushButton {
    background-color: #0E639C;
    color: white;
    border: none;
    padding: 8px 16px;
    border-radius: 4px;
}

QPushButton:hover {
    background-color: #1177BB;
}
```

---

## 12. 布局问题排查

### 12.1 常见问题

```
┌─────────────────────────────────────────────────────────────┐
│                    布局常见问题排查                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  问题 1: 控件不显示                                          │
│  ─────────────────────────────────────────────────────────  │
│  • 检查是否添加到布局                                        │
│  • 检查是否调用 show()                                       │
│  • 检查父控件是否可见                                        │
│  • 检查 sizePolicy 是否为 Ignored                           │
│  • 检查 minimumSize 是否为 0                                │
│                                                             │
│  问题 2: 控件大小异常                                        │
│  ─────────────────────────────────────────────────────────  │
│  • 检查 sizePolicy 设置                                     │
│  • 检查 stretch factor                                      │
│  • 检查 minimumSize/maximumSize                             │
│  • 检查 QSS padding/margin                                  │
│  • 使用 Qt Designer 调试查看                                │
│                                                             │
│  问题 3: 布局不更新                                          │
│  ─────────────────────────────────────────────────────────  │
│  • 调用 layout()->invalidate()                              │
│  • 调用 layout()->activate()                                │
│  • 调用 updateGeometry()                                    │
│  • 检查是否在构造函数中正确设置                              │
│                                                             │
│  问题 4: 嵌套布局问题                                        │
│  ─────────────────────────────────────────────────────────  │
│  • 确保中间容器有正确的 sizePolicy                          │
│  • 检查每层的 contentsMargins                               │
│  • 避免过深的嵌套层级                                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.2 调试技巧

```cpp
// 显示布局边界（调试用）
void showLayoutBorders(QWidget* widget) {
    widget->setStyleSheet(
        "QWidget { border: 1px solid red; }"
    );
}

// 打印布局信息
void debugLayout(QLayout* layout, int indent = 0) {
    QString prefix(indent * 2, ' ');
    qDebug() << prefix << "Layout:" << layout
             << "count:" << layout->count()
             << "margins:" << layout->contentsMargins();

    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem* item = layout->itemAt(i);
        if (QWidget* w = item->widget()) {
            qDebug() << prefix << "  Widget:" << w
                     << "sizeHint:" << w->sizeHint()
                     << "sizePolicy:" << w->sizePolicy();
        } else if (QLayout* l = item->layout()) {
            debugLayout(l, indent + 1);
        } else if (QSpacerItem* s = item->spacerItem()) {
            qDebug() << prefix << "  Spacer:" << s->sizeHint();
        }
    }
}

// 使用
debugLayout(widget->layout());
```

---

## 13. 面试常见问题

### Q1: margin 和 padding 的区别？

```
margin（外边距）：
  • 控件与外部元素的距离
  • 不在控件边框内
  • 背景色不会填充 margin 区域
  • 可以为负值（重叠效果）

padding（内边距）：
  • 控件边框与内容的距离
  • 在控件边框内
  • 背景色会填充 padding 区域
  • 影响控件的 sizeHint

Qt 中的设置：
  • QSS: margin / padding
  • Layout: setContentsMargins() / setSpacing()
  • 注意 QSS margin 和 Layout margin 会叠加
```

### Q2: QSS 中 border-radius 为什么有时不生效？

```
常见原因：
1. 未设置 border（必须有边框才能有圆角）
2. 背景超出圆角范围（需要 border 裁剪）
3. 控件使用原生绘制样式
4. 子控件覆盖了圆角区域
5. 圆角值超过控件尺寸的一半

解决方案：
QPushButton {
    border: 1px solid gray;  /* 必须有 border */
    border-radius: 10px;
    background: blue;
}

对于图片圆角，QSS 无法裁剪 pixmap，需要：
  • 使用 QPainterPath 手动裁剪
  • 或使用 setMask()
```

### Q3: 如何让某个控件占据所有剩余空间？

```cpp
// 方法 1: setSizePolicy + Expanding
widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

// 方法 2: setStretch
layout->addWidget(fixedWidget);
layout->addWidget(expandingWidget, 1);  // stretch = 1

// 方法 3: addStretch
layout->addWidget(topWidget);
layout->addWidget(middleWidget, 1);  // 这个占据剩余空间
layout->addWidget(bottomWidget);

// 方法 4: QGridLayout 中设置 row/column stretch
gridLayout->setRowStretch(1, 1);     // 第 1 行拉伸
gridLayout->setColumnStretch(1, 1);  // 第 1 列拉伸
```

### Q4: SizePolicy 各策略的实际效果？

```
Fixed:     固定大小，等于 sizeHint，不参与拉伸
Minimum:   至少 sizeHint 大小，可以更大
Maximum:   至多 sizeHint 大小，可以更小
Preferred: 接近 sizeHint，可大可小（默认）
Expanding: 尽量占据更多空间，优先级高于 Preferred
Ignored:   忽略 sizeHint，完全由布局决定

优先级：Fixed > Minimum > Expanding > Preferred > Maximum > Ignored

实际选择：
  • 图标/固定按钮 → Fixed
  • 标签 → Preferred
  • 输入框/列表 → Expanding
  • 可收缩面板 → Minimum/Maximum
```

### Q5: 如何实现固定宽度 + 自适应宽度的组合？

```cpp
QHBoxLayout* layout = new QHBoxLayout();

// 固定宽度标签
QLabel* label = new QLabel("Name:");
label->setFixedWidth(80);
// 或 label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

// 自适应宽度输入框
QLineEdit* edit = new QLineEdit();
edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

layout->addWidget(label);
layout->addWidget(edit);
```

### Q6: QSS 和 paintEvent 自定义绑制哪个更好？

```
QSS 优点：
  • 简单快速，无需编写 C++ 代码
  • 易于维护和主题切换
  • 声明式，可读性好

QSS 缺点：
  • 性能开销（解析、匹配）
  • 功能有限（无法实现复杂动画）
  • 某些效果无法实现（如渐变动画）

paintEvent 优点：
  • 完全控制绑制过程
  • 性能更好
  • 可实现任意效果

paintEvent 缺点：
  • 代码量大
  • 难以维护
  • 需要处理各种状态

建议：
  • 简单样式 → QSS
  • 复杂动画/特效 → paintEvent
  • 大量同类控件 → QStyle 子类
```

### Q7: 如何正确实现动态换肤？

```cpp
// 1. 使用 qApp->setStyleSheet()（推荐）
void switchTheme(const QString& qssPath) {
    QFile file(qssPath);
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
}

// 2. 使用 property 选择器避免重新设置 styleSheet
// QSS: QPushButton[theme="dark"] { ... }
widget->setProperty("theme", "dark");
widget->style()->unpolish(widget);
widget->style()->polish(widget);

// 3. 信号通知需要手动刷新的控件
connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
        widget, &Widget::onThemeChanged);
```

### Q8: 布局中的控件不显示怎么排查？

```
排查步骤：
1. 确认控件已添加到布局
   layout->addWidget(widget);

2. 确认父控件可见
   parentWidget->show();

3. 检查 sizePolicy 不是 Ignored + (0,0) sizeHint

4. 检查 minimumSize
   widget->setMinimumSize(1, 1);

5. 检查是否被其他控件遮挡
   widget->raise();

6. 检查 QSS 是否设置了不可见
   visibility: hidden;
   height: 0px;

7. 使用调试边框
   widget->setStyleSheet("border: 1px solid red;");
```

---

## 14. 总结

```
┌─────────────────────────────────────────────────────────────┐
│                  Qt 布局与样式知识图谱                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  盒子模型                                                   │
│  ├── margin（外边距）                                       │
│  ├── border（边框）                                         │
│  ├── padding（内边距）                                      │
│  └── contents（内容区）                                     │
│                                                             │
│  布局系统                                                   │
│  ├── QLayout 体系（Box/Grid/Form/Stacked）                  │
│  ├── SizePolicy 六种策略                                    │
│  ├── sizeHint / minimumSizeHint                             │
│  ├── Stretch Factor                                         │
│  └── 自定义 Layout                                          │
│                                                             │
│  QSS 样式                                                   │
│  ├── 渲染机制（解析→匹配→合并→绑制）                        │
│  ├── border-radius 原理（QPainterPath 裁剪）                │
│  ├── 选择器与伪状态                                         │
│  ├── 子控件选择器（::drop-down 等）                         │
│  └── QSS vs paintEvent                                      │
│                                                             │
│  高级话题                                                   │
│  ├── 样式与布局的交互                                       │
│  ├── 高 DPI 适配                                            │
│  ├── 动态换肤                                               │
│  └── 性能优化                                               │
│                                                             │
│  面试重点                                                   │
│  ├── margin vs padding                                      │
│  ├── border-radius 不生效原因                               │
│  ├── SizePolicy 策略选择                                    │
│  ├── 占据剩余空间的方法                                     │
│  └── QSS vs 自定义绑制                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```
