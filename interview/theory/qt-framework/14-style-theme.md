# Qt 样式与主题（Style & Theme）

## 概述

Qt 提供了灵活的样式系统，允许自定义应用程序的外观。主要方式包括 Qt Style Sheets (QSS)、QPalette 和自定义 QStyle。

---

## 1. Qt Style Sheets (QSS)

### 1.1 QSS 基础

**面试问题**：如何使用 Qt 样式表？

**答案**：

```cpp
#include <QApplication>
#include <QWidget>

void qssBasics() {
    // 应用级样式
    qApp->setStyleSheet(R"(
        QWidget {
            background-color: #f0f0f0;
            font-family: "Microsoft YaHei";
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
        }

        QPushButton:hover {
            background-color: #45a049;
        }

        QPushButton:pressed {
            background-color: #3d8b40;
        }

        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
    )");

    // 控件级样式
    QPushButton* btn = new QPushButton("Click Me");
    btn->setStyleSheet("background-color: red; color: white;");

    // 从文件加载
    QFile file(":/styles/dark.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qApp->setStyleSheet(file.readAll());
    }
}
```

---

### 1.2 选择器

```cpp
void qssSelectors() {
    QString qss = R"(
        /* 类型选择器 */
        QPushButton { background: blue; }

        /* ID 选择器 */
        QPushButton#submitButton { background: green; }

        /* 类选择器（自定义属性） */
        QPushButton[class="danger"] { background: red; }

        /* 后代选择器 */
        QDialog QPushButton { background: gray; }

        /* 直接子元素选择器 */
        QDialog > QPushButton { background: white; }

        /* 子控件选择器 */
        QComboBox::drop-down { image: url(:/icons/arrow.png); }
        QScrollBar::handle:vertical { background: #888; }

        /* 伪状态选择器 */
        QPushButton:hover { background: lightblue; }
        QPushButton:pressed { background: darkblue; }
        QPushButton:disabled { background: gray; }
        QPushButton:checked { background: orange; }
        QPushButton:!checked { background: white; }

        /* 组合伪状态 */
        QPushButton:hover:!pressed { background: cyan; }

        /* 多个选择器 */
        QPushButton, QToolButton { border: 1px solid gray; }
    )";
}
```

---

### 1.3 常用属性

```cpp
void qssProperties() {
    QString qss = R"(
        QWidget {
            /* 背景 */
            background-color: #ffffff;
            background-image: url(:/images/bg.png);
            background-repeat: no-repeat;
            background-position: center;

            /* 边框 */
            border: 1px solid #cccccc;
            border-radius: 5px;
            border-top: 2px solid red;
            border-left-color: blue;

            /* 内边距 */
            padding: 10px;
            padding-left: 20px;

            /* 外边距 */
            margin: 5px;

            /* 字体 */
            font-family: "Arial";
            font-size: 14px;
            font-weight: bold;
            font-style: italic;
            color: #333333;

            /* 大小 */
            min-width: 100px;
            max-width: 200px;
            min-height: 30px;
            max-height: 50px;
        }

        QLineEdit {
            /* 选中文本 */
            selection-background-color: #3399ff;
            selection-color: white;

            /* 占位符 */
            placeholder-text-color: #999999;
        }

        QToolTip {
            background-color: #ffffcc;
            border: 1px solid #cccccc;
            color: black;
        }
    )";
}
```

---

### 1.4 子控件样式

```cpp
void qssSubControls() {
    QString qss = R"(
        /* QComboBox 下拉按钮 */
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid gray;
        }

        QComboBox::down-arrow {
            image: url(:/icons/down-arrow.png);
            width: 12px;
            height: 12px;
        }

        /* QSpinBox 按钮 */
        QSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 16px;
        }

        QSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 16px;
        }

        /* QScrollBar */
        QScrollBar:vertical {
            width: 12px;
            background: #f0f0f0;
        }

        QScrollBar::handle:vertical {
            background: #c0c0c0;
            min-height: 20px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical:hover {
            background: #a0a0a0;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }

        /* QTabWidget */
        QTabWidget::pane {
            border: 1px solid #cccccc;
        }

        QTabBar::tab {
            background: #e0e0e0;
            padding: 8px 16px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }

        QTabBar::tab:selected {
            background: white;
            border-bottom: 2px solid #3399ff;
        }

        /* QCheckBox */
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
        }

        QCheckBox::indicator:unchecked {
            image: url(:/icons/checkbox-unchecked.png);
        }

        QCheckBox::indicator:checked {
            image: url(:/icons/checkbox-checked.png);
        }
    )";
}
```

---

## 2. QPalette

### 2.1 调色板基础

**面试问题**：QPalette 是什么？如何使用它？

**答案**：

```cpp
#include <QPalette>

void paletteBasics() {
    // 获取当前调色板
    QPalette palette = qApp->palette();

    // 设置颜色
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(225, 225, 225));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Highlight, QColor(51, 153, 255));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    // 应用调色板
    qApp->setPalette(palette);

    // 控件级调色板
    QLabel* label = new QLabel("Text");
    QPalette labelPalette = label->palette();
    labelPalette.setColor(QPalette::WindowText, Qt::red);
    label->setPalette(labelPalette);
    label->setAutoFillBackground(true);
}

// 暗色主题
void darkPalette() {
    QPalette darkPalette;

    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    // 禁用状态颜色
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(128, 128, 128));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128));

    qApp->setPalette(darkPalette);
}
```

---

### 2.2 颜色角色

```cpp
void colorRoles() {
    // QPalette::ColorRole 定义了不同 UI 元素的颜色
    //
    // Window        - 通用背景色
    // WindowText    - 通用前景色
    // Base          - 输入控件背景
    // AlternateBase - 交替行背景
    // Text          - 输入控件文本
    // Button        - 按钮背景
    // ButtonText    - 按钮文本
    // BrightText    - 与暗色形成对比的亮文本
    // Light         - 比 Button 亮
    // Midlight      - Button 和 Light 之间
    // Mid           - Button 和 Dark 之间
    // Dark          - 比 Button 暗
    // Shadow        - 非常暗的颜色
    // Highlight     - 选中项背景
    // HighlightedText - 选中项文本
    // Link          - 链接
    // LinkVisited   - 已访问链接
    // ToolTipBase   - 工具提示背景
    // ToolTipText   - 工具提示文本
    // PlaceholderText - 占位符文本
}
```

---

## 3. 自定义 QStyle

### 3.1 QProxyStyle

**面试问题**：如何自定义控件的绘制？

**答案**：

```cpp
#include <QProxyStyle>
#include <QPainter>

class CustomStyle : public QProxyStyle {
public:
    CustomStyle() : QProxyStyle("Fusion") {}

    // 自定义原始元素绘制
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
                       QPainter* painter, const QWidget* widget) const override {
        if (element == PE_IndicatorCheckBox) {
            // 自定义复选框绘制
            QRect rect = option->rect;
            painter->save();

            if (option->state & State_On) {
                painter->fillRect(rect, QColor(51, 153, 255));
                painter->setPen(Qt::white);
                painter->drawText(rect, Qt::AlignCenter, "✓");
            } else {
                painter->fillRect(rect, Qt::white);
                painter->setPen(QColor(200, 200, 200));
                painter->drawRect(rect.adjusted(0, 0, -1, -1));
            }

            painter->restore();
            return;
        }

        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }

    // 自定义控件绘制
    void drawControl(ControlElement element, const QStyleOption* option,
                     QPainter* painter, const QWidget* widget) const override {
        if (element == CE_PushButton) {
            const QStyleOptionButton* btnOption =
                qstyleoption_cast<const QStyleOptionButton*>(option);

            if (btnOption) {
                QRect rect = btnOption->rect;
                bool hover = option->state & State_MouseOver;
                bool pressed = option->state & State_Sunken;

                // 绘制背景
                QColor bgColor = pressed ? QColor(60, 140, 60) :
                                 hover ? QColor(80, 180, 80) :
                                         QColor(70, 160, 70);
                painter->fillRect(rect, bgColor);

                // 绘制文本
                painter->setPen(Qt::white);
                painter->drawText(rect, Qt::AlignCenter, btnOption->text);
                return;
            }
        }

        QProxyStyle::drawControl(element, option, painter, widget);
    }

    // 自定义大小
    QSize sizeFromContents(ContentsType type, const QStyleOption* option,
                           const QSize& size, const QWidget* widget) const override {
        if (type == CT_PushButton) {
            return QSize(size.width() + 20, size.height() + 10);
        }

        return QProxyStyle::sizeFromContents(type, option, size, widget);
    }

    // 自定义像素度量
    int pixelMetric(PixelMetric metric, const QStyleOption* option,
                    const QWidget* widget) const override {
        if (metric == PM_ButtonMargin) {
            return 8;
        }

        return QProxyStyle::pixelMetric(metric, option, widget);
    }
};

// 使用
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle(new CustomStyle());
    // ...
}
```

---

## 4. 动态属性

### 4.1 使用动态属性控制样式

```cpp
void dynamicPropertyStyle() {
    // QSS 支持基于属性的选择器
    QString qss = R"(
        QPushButton[buttonType="primary"] {
            background-color: #3399ff;
            color: white;
        }

        QPushButton[buttonType="danger"] {
            background-color: #ff3333;
            color: white;
        }

        QPushButton[buttonType="success"] {
            background-color: #33cc33;
            color: white;
        }

        QLineEdit[valid="true"] {
            border: 2px solid green;
        }

        QLineEdit[valid="false"] {
            border: 2px solid red;
        }
    )";

    qApp->setStyleSheet(qss);

    // 设置动态属性
    QPushButton* primaryBtn = new QPushButton("Primary");
    primaryBtn->setProperty("buttonType", "primary");

    QPushButton* dangerBtn = new QPushButton("Danger");
    dangerBtn->setProperty("buttonType", "danger");

    // 动态更改属性
    QLineEdit* edit = new QLineEdit();
    edit->setProperty("valid", true);

    // 更改属性后需要刷新样式
    QObject::connect(edit, &QLineEdit::textChanged, [edit](const QString& text) {
        bool isValid = !text.isEmpty() && text.length() >= 3;
        edit->setProperty("valid", isValid);

        // 强制刷新样式
        edit->style()->unpolish(edit);
        edit->style()->polish(edit);
    });
}
```

---

## 5. 主题切换

### 5.1 实现主题切换

```cpp
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme { Light, Dark, Custom };

    static ThemeManager& instance() {
        static ThemeManager instance;
        return instance;
    }

    void setTheme(Theme theme) {
        m_currentTheme = theme;

        switch (theme) {
        case Light:
            applyLightTheme();
            break;
        case Dark:
            applyDarkTheme();
            break;
        case Custom:
            applyCustomTheme();
            break;
        }

        emit themeChanged(theme);
    }

    Theme currentTheme() const { return m_currentTheme; }

signals:
    void themeChanged(Theme theme);

private:
    void applyLightTheme() {
        qApp->setStyle("Fusion");
        qApp->setPalette(QApplication::style()->standardPalette());
        qApp->setStyleSheet("");
    }

    void applyDarkTheme() {
        qApp->setStyle("Fusion");

        QPalette dark;
        dark.setColor(QPalette::Window, QColor(53, 53, 53));
        dark.setColor(QPalette::WindowText, Qt::white);
        // ... 其他颜色设置

        qApp->setPalette(dark);

        qApp->setStyleSheet(R"(
            QToolTip {
                color: white;
                background-color: #2a2a2a;
                border: 1px solid white;
            }
        )");
    }

    void applyCustomTheme() {
        QFile file(":/themes/custom.qss");
        if (file.open(QIODevice::ReadOnly)) {
            qApp->setStyleSheet(file.readAll());
        }
    }

    Theme m_currentTheme = Light;
};

// 使用
void setupThemeSwitcher(QComboBox* themeCombo) {
    themeCombo->addItem("Light", ThemeManager::Light);
    themeCombo->addItem("Dark", ThemeManager::Dark);
    themeCombo->addItem("Custom", ThemeManager::Custom);

    QObject::connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [themeCombo](int index) {
        ThemeManager::Theme theme =
            static_cast<ThemeManager::Theme>(themeCombo->itemData(index).toInt());
        ThemeManager::instance().setTheme(theme);
    });
}
```

---

## 6. 常见面试题

### Q1: QSS 和 QPalette 有什么区别？

**答案**：

| 特性 | QSS | QPalette |
|------|-----|----------|
| 灵活性 | 高 | 中 |
| 性能 | 较低 | 高 |
| 语法 | CSS-like | C++ API |
| 细节控制 | 可控制子控件 | 只能控制颜色 |
| 与系统主题 | 可能覆盖 | 可继承 |

---

### Q2: 如何让 QSS 样式立即生效？

**答案**：

```cpp
void refreshStyle(QWidget* widget) {
    // 方法 1：unpolish/polish
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);

    // 方法 2：重设样式表
    QString style = widget->styleSheet();
    widget->setStyleSheet("");
    widget->setStyleSheet(style);

    // 方法 3：强制更新
    widget->update();
}
```

---

## 总结

Qt 样式与主题的核心要点：

1. **QSS** - 类 CSS 语法，灵活强大
2. **选择器** - 类型、ID、伪状态、子控件
3. **QPalette** - 系统级颜色管理
4. **QProxyStyle** - 自定义控件绘制
5. **动态属性** - 基于属性的条件样式
6. **主题切换** - 运行时更换外观

合理使用这些技术可以创建美观、一致的用户界面。
