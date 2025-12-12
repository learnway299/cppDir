# Qt 常用控件（Widgets）

## 概述

Qt Widgets 是 Qt 的传统 GUI 模块，提供了丰富的桌面控件。本文介绍常用控件的使用方法和面试要点。

---

## 1. 基础控件

### 1.1 按钮类

```cpp
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>

void buttonWidgets() {
    // QPushButton - 普通按钮
    QPushButton* btn = new QPushButton("Click Me");
    btn->setIcon(QIcon(":/icons/action.png"));
    btn->setShortcut(QKeySequence("Ctrl+S"));
    btn->setDefault(true);  // 默认按钮

    connect(btn, &QPushButton::clicked, []() {
        qDebug() << "Button clicked";
    });

    // QToolButton - 工具栏按钮
    QToolButton* toolBtn = new QToolButton();
    toolBtn->setIcon(QIcon(":/icons/tool.png"));
    toolBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBtn->setPopupMode(QToolButton::MenuButtonPopup);

    QMenu* menu = new QMenu();
    menu->addAction("Option 1");
    menu->addAction("Option 2");
    toolBtn->setMenu(menu);

    // QRadioButton - 单选按钮
    QRadioButton* radio1 = new QRadioButton("Option A");
    QRadioButton* radio2 = new QRadioButton("Option B");
    radio1->setChecked(true);

    // 使用 QButtonGroup 管理互斥
    QButtonGroup* group = new QButtonGroup();
    group->addButton(radio1, 0);
    group->addButton(radio2, 1);

    connect(group, &QButtonGroup::idClicked, [](int id) {
        qDebug() << "Selected option:" << id;
    });

    // QCheckBox - 复选框
    QCheckBox* check = new QCheckBox("Enable feature");
    check->setTristate(true);  // 三态：选中/未选/部分选中

    connect(check, &QCheckBox::stateChanged, [](int state) {
        // Qt::Unchecked, Qt::PartiallyChecked, Qt::Checked
        qDebug() << "State:" << state;
    });
}
```

---

### 1.2 输入控件

```cpp
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>

void inputWidgets() {
    // QLineEdit - 单行文本输入
    QLineEdit* lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("Enter your name...");
    lineEdit->setMaxLength(50);
    lineEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z ]+"), lineEdit));
    lineEdit->setEchoMode(QLineEdit::Password);  // 密码模式
    lineEdit->setClearButtonEnabled(true);

    connect(lineEdit, &QLineEdit::textChanged, [](const QString& text) {
        qDebug() << "Text changed:" << text;
    });

    connect(lineEdit, &QLineEdit::returnPressed, []() {
        qDebug() << "Enter pressed";
    });

    // QTextEdit - 富文本编辑
    QTextEdit* textEdit = new QTextEdit();
    textEdit->setHtml("<b>Bold</b> and <i>italic</i>");
    textEdit->setAcceptRichText(true);

    // QPlainTextEdit - 纯文本编辑（大文件更高效）
    QPlainTextEdit* plainEdit = new QPlainTextEdit();
    plainEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    // QSpinBox - 整数选择
    QSpinBox* spinBox = new QSpinBox();
    spinBox->setRange(0, 100);
    spinBox->setSingleStep(5);
    spinBox->setPrefix("$");
    spinBox->setSuffix(" USD");

    // QDoubleSpinBox - 浮点数选择
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setRange(0.0, 100.0);
    doubleSpinBox->setDecimals(2);

    // QSlider - 滑块
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(10);

    // 联动 slider 和 spinBox
    QObject::connect(slider, &QSlider::valueChanged,
                     spinBox, &QSpinBox::setValue);
    QObject::connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     slider, &QSlider::setValue);

    // QComboBox - 下拉框
    QComboBox* combo = new QComboBox();
    combo->addItem("Option 1", QVariant(1));
    combo->addItem("Option 2", QVariant(2));
    combo->addItems({"A", "B", "C"});
    combo->setEditable(true);  // 可编辑
    combo->setInsertPolicy(QComboBox::InsertAtBottom);

    connect(combo, &QComboBox::currentIndexChanged, [combo](int index) {
        QVariant data = combo->itemData(index);
        qDebug() << "Selected:" << combo->currentText() << data;
    });
}
```

---

### 1.3 显示控件

```cpp
#include <QLabel>
#include <QProgressBar>
#include <QLCDNumber>

void displayWidgets() {
    // QLabel - 标签
    QLabel* textLabel = new QLabel("Hello, Qt!");
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setWordWrap(true);

    // 富文本标签
    QLabel* richLabel = new QLabel("<a href='http://qt.io'>Qt Website</a>");
    richLabel->setTextFormat(Qt::RichText);
    richLabel->setOpenExternalLinks(true);

    // 图片标签
    QLabel* imageLabel = new QLabel();
    imageLabel->setPixmap(QPixmap(":/images/logo.png"));
    imageLabel->setScaledContents(true);

    // QProgressBar - 进度条
    QProgressBar* progress = new QProgressBar();
    progress->setRange(0, 100);
    progress->setValue(50);
    progress->setTextVisible(true);
    progress->setFormat("%v / %m (%p%)");

    // 忙碌状态（无限循环）
    progress->setRange(0, 0);

    // QLCDNumber - LCD 数字显示
    QLCDNumber* lcd = new QLCDNumber();
    lcd->setDigitCount(8);
    lcd->setMode(QLCDNumber::Dec);
    lcd->display(12345);
}
```

---

## 2. 容器控件

### 2.1 分组和框架

```cpp
#include <QGroupBox>
#include <QFrame>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QScrollArea>

void containerWidgets() {
    // QGroupBox - 分组框
    QGroupBox* group = new QGroupBox("Settings");
    QVBoxLayout* groupLayout = new QVBoxLayout(group);
    groupLayout->addWidget(new QCheckBox("Option 1"));
    groupLayout->addWidget(new QCheckBox("Option 2"));

    group->setCheckable(true);  // 可折叠

    // QFrame - 框架
    QFrame* frame = new QFrame();
    frame->setFrameStyle(QFrame::Box | QFrame::Raised);
    frame->setLineWidth(2);

    // QSplitter - 分割器
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(new QListWidget());
    splitter->addWidget(new QTextEdit());
    splitter->setSizes({200, 400});

    // QTabWidget - 选项卡
    QTabWidget* tabs = new QTabWidget();
    tabs->addTab(new QWidget(), QIcon(":/icons/home.png"), "Home");
    tabs->addTab(new QWidget(), "Settings");
    tabs->setTabsClosable(true);
    tabs->setMovable(true);

    QObject::connect(tabs, &QTabWidget::tabCloseRequested, [tabs](int index) {
        tabs->removeTab(index);
    });

    // QStackedWidget - 堆栈控件
    QStackedWidget* stack = new QStackedWidget();
    stack->addWidget(new QLabel("Page 1"));
    stack->addWidget(new QLabel("Page 2"));
    stack->setCurrentIndex(0);

    // QScrollArea - 滚动区域
    QScrollArea* scroll = new QScrollArea();
    QLabel* largeContent = new QLabel();
    largeContent->setPixmap(QPixmap(":/images/large.png"));
    scroll->setWidget(largeContent);
    scroll->setWidgetResizable(true);
}
```

---

### 2.2 工具栏和状态栏

```cpp
#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QDockWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow() {
        // 菜单栏
        QMenuBar* menuBar = this->menuBar();
        QMenu* fileMenu = menuBar->addMenu("&File");
        QAction* newAction = fileMenu->addAction("&New", QKeySequence::New);
        QAction* openAction = fileMenu->addAction("&Open", QKeySequence::Open);
        fileMenu->addSeparator();
        QAction* exitAction = fileMenu->addAction("E&xit", QKeySequence::Quit);

        connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

        // 工具栏
        QToolBar* toolbar = addToolBar("Main Toolbar");
        toolbar->setMovable(true);
        toolbar->setFloatable(true);
        toolbar->addAction(newAction);
        toolbar->addAction(openAction);
        toolbar->addSeparator();

        // 添加自定义控件到工具栏
        QComboBox* combo = new QComboBox();
        combo->addItems({"100%", "150%", "200%"});
        toolbar->addWidget(combo);

        // 状态栏
        QStatusBar* statusBar = this->statusBar();
        statusBar->showMessage("Ready");

        // 永久状态栏控件
        QLabel* permLabel = new QLabel("Line: 1, Col: 1");
        statusBar->addPermanentWidget(permLabel);

        // 停靠窗口
        QDockWidget* dock = new QDockWidget("Properties", this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        dock->setWidget(new QListWidget());
        addDockWidget(Qt::RightDockWidgetArea, dock);

        // 中央控件
        setCentralWidget(new QTextEdit());
    }
};
```

---

## 3. 列表和树控件

### 3.1 QListWidget

```cpp
#include <QListWidget>

void listWidgetDemo() {
    QListWidget* list = new QListWidget();

    // 添加项
    list->addItem("Item 1");
    list->addItems({"Item 2", "Item 3"});

    // 带图标的项
    QListWidgetItem* item = new QListWidgetItem(QIcon(":/icon.png"), "Icon Item");
    item->setData(Qt::UserRole, QVariant(42));  // 自定义数据
    item->setCheckState(Qt::Unchecked);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    list->addItem(item);

    // 设置
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    list->setDragDropMode(QAbstractItemView::InternalMove);
    list->setViewMode(QListView::IconMode);  // 图标模式
    list->setIconSize(QSize(64, 64));

    // 信号
    QObject::connect(list, &QListWidget::itemClicked, [](QListWidgetItem* item) {
        qDebug() << "Clicked:" << item->text();
    });

    QObject::connect(list, &QListWidget::itemDoubleClicked, [](QListWidgetItem* item) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    });

    // 获取选中项
    QList<QListWidgetItem*> selected = list->selectedItems();
}
```

---

### 3.2 QTreeWidget

```cpp
#include <QTreeWidget>

void treeWidgetDemo() {
    QTreeWidget* tree = new QTreeWidget();

    // 设置列
    tree->setColumnCount(3);
    tree->setHeaderLabels({"Name", "Size", "Date"});

    // 添加顶级项
    QTreeWidgetItem* root1 = new QTreeWidgetItem(tree, {"Documents"});
    QTreeWidgetItem* root2 = new QTreeWidgetItem(tree, {"Pictures"});

    // 添加子项
    QTreeWidgetItem* child1 = new QTreeWidgetItem(root1, {"file1.txt", "10 KB", "2024-01-01"});
    QTreeWidgetItem* child2 = new QTreeWidgetItem(root1, {"file2.txt", "20 KB", "2024-01-02"});

    // 设置图标
    root1->setIcon(0, QIcon(":/icons/folder.png"));
    child1->setIcon(0, QIcon(":/icons/file.png"));

    // 展开/折叠
    tree->expandAll();
    tree->collapseAll();
    root1->setExpanded(true);

    // 查找项
    QList<QTreeWidgetItem*> found = tree->findItems("file", Qt::MatchContains | Qt::MatchRecursive);

    // 遍历
    QTreeWidgetItemIterator it(tree);
    while (*it) {
        qDebug() << (*it)->text(0);
        ++it;
    }

    // 信号
    QObject::connect(tree, &QTreeWidget::itemExpanded, [](QTreeWidgetItem* item) {
        // 延迟加载子项
    });
}
```

---

### 3.3 QTableWidget

```cpp
#include <QTableWidget>

void tableWidgetDemo() {
    QTableWidget* table = new QTableWidget(5, 3);  // 5 行 3 列

    // 设置表头
    table->setHorizontalHeaderLabels({"Name", "Age", "City"});
    table->setVerticalHeaderLabels({"1", "2", "3", "4", "5"});

    // 设置单元格
    table->setItem(0, 0, new QTableWidgetItem("Alice"));
    table->setItem(0, 1, new QTableWidgetItem("25"));
    table->setItem(0, 2, new QTableWidgetItem("Beijing"));

    // 单元格控件
    QComboBox* combo = new QComboBox();
    combo->addItems({"Option A", "Option B"});
    table->setCellWidget(1, 0, combo);

    // 单元格属性
    QTableWidgetItem* item = table->item(0, 0);
    if (item) {
        item->setBackground(Qt::yellow);
        item->setForeground(Qt::red);
        item->setFont(QFont("Arial", 12, QFont::Bold));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);  // 只读
    }

    // 设置
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setSortingEnabled(true);

    // 自动调整列宽
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->resizeColumnsToContents();

    // 隐藏行/列
    table->hideRow(2);
    table->hideColumn(1);

    // 合并单元格
    table->setSpan(3, 0, 1, 3);  // 从 (3,0) 开始，1行3列
}
```

---

## 4. 对话框

### 4.1 标准对话框

```cpp
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QProgressDialog>

void standardDialogs(QWidget* parent) {
    // 消息框
    QMessageBox::information(parent, "Info", "Operation completed");
    QMessageBox::warning(parent, "Warning", "Disk space low");
    QMessageBox::critical(parent, "Error", "File not found");

    QMessageBox::StandardButton reply = QMessageBox::question(
        parent, "Confirm", "Are you sure?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        // 用户点击 Yes
    }

    // 自定义消息框
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Save changes?");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    // 文件对话框
    QString fileName = QFileDialog::getOpenFileName(
        parent, "Open File", QDir::homePath(),
        "Text Files (*.txt);;All Files (*)");

    QStringList files = QFileDialog::getOpenFileNames(
        parent, "Select Files", QString(),
        "Images (*.png *.jpg);;All Files (*)");

    QString saveFile = QFileDialog::getSaveFileName(
        parent, "Save As", QString(),
        "Text Files (*.txt)");

    QString dir = QFileDialog::getExistingDirectory(
        parent, "Select Directory", QDir::homePath());

    // 颜色对话框
    QColor color = QColorDialog::getColor(Qt::white, parent, "Select Color");
    if (color.isValid()) {
        qDebug() << "Selected color:" << color.name();
    }

    // 字体对话框
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Arial", 12), parent, "Select Font");
    if (ok) {
        qDebug() << "Selected font:" << font.family();
    }

    // 输入对话框
    QString text = QInputDialog::getText(parent, "Input", "Enter your name:");
    int number = QInputDialog::getInt(parent, "Input", "Enter a number:", 0, 0, 100);
    double value = QInputDialog::getDouble(parent, "Input", "Enter a value:");
    QString item = QInputDialog::getItem(parent, "Select", "Choose an option:",
                                         {"A", "B", "C"}, 0, false);

    // 进度对话框
    QProgressDialog progress("Processing...", "Cancel", 0, 100, parent);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i <= 100; ++i) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;
        QThread::msleep(50);
    }
}
```

---

### 4.2 自定义对话框

```cpp
#include <QDialog>
#include <QDialogButtonBox>

class CustomDialog : public QDialog {
    Q_OBJECT

public:
    CustomDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Custom Dialog");
        setModal(true);

        QVBoxLayout* layout = new QVBoxLayout(this);

        // 内容
        m_nameEdit = new QLineEdit();
        m_emailEdit = new QLineEdit();

        QFormLayout* form = new QFormLayout();
        form->addRow("Name:", m_nameEdit);
        form->addRow("Email:", m_emailEdit);
        layout->addLayout(form);

        // 按钮
        QDialogButtonBox* buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        connect(buttons, &QDialogButtonBox::accepted, this, &CustomDialog::onAccept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        layout->addWidget(buttons);
    }

    QString name() const { return m_nameEdit->text(); }
    QString email() const { return m_emailEdit->text(); }

private slots:
    void onAccept() {
        // 验证
        if (m_nameEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Error", "Name is required");
            return;
        }
        accept();
    }

private:
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
};

// 使用
void showCustomDialog(QWidget* parent) {
    CustomDialog dialog(parent);

    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "Name:" << dialog.name();
        qDebug() << "Email:" << dialog.email();
    }
}
```

---

## 5. 布局

### 5.1 布局管理器

```cpp
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

void layoutDemo() {
    QWidget* widget = new QWidget();

    // 水平布局
    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(new QPushButton("Left"));
    hbox->addStretch();  // 弹簧
    hbox->addWidget(new QPushButton("Right"));
    hbox->setSpacing(10);
    hbox->setContentsMargins(5, 5, 5, 5);

    // 垂直布局
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(new QLabel("Top"));
    vbox->addLayout(hbox);  // 嵌套布局
    vbox->addWidget(new QLabel("Bottom"));

    // 网格布局
    QGridLayout* grid = new QGridLayout();
    grid->addWidget(new QLabel("Name:"), 0, 0);
    grid->addWidget(new QLineEdit(), 0, 1);
    grid->addWidget(new QLabel("Age:"), 1, 0);
    grid->addWidget(new QSpinBox(), 1, 1);
    grid->addWidget(new QPushButton("Submit"), 2, 0, 1, 2);  // 跨 2 列
    grid->setColumnStretch(1, 1);  // 第 1 列可伸展

    // 表单布局
    QFormLayout* form = new QFormLayout();
    form->addRow("Username:", new QLineEdit());
    form->addRow("Password:", new QLineEdit());
    form->addRow(new QPushButton("Login"));
    form->setLabelAlignment(Qt::AlignRight);

    widget->setLayout(vbox);
}
```

---

## 6. 常见面试题

### Q1: QWidget 和 QDialog 的区别？

**答案**：

| 特性 | QWidget | QDialog |
|------|---------|---------|
| 用途 | 通用控件基类 | 对话框 |
| 模态 | 不支持 | 支持 (exec()) |
| 返回值 | 无 | Accepted/Rejected |
| 默认行为 | 无 | Escape 关闭 |

---

### Q2: 如何自定义控件？

**答案**：

```cpp
class CustomWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    CustomWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    int value() const { return m_value; }
    void setValue(int v) {
        if (m_value != v) {
            m_value = v;
            emit valueChanged(v);
            update();
        }
    }

    QSize sizeHint() const override {
        return QSize(200, 30);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        // 自定义绘制
    }

    void mousePressEvent(QMouseEvent* event) override {
        // 处理鼠标
    }

signals:
    void valueChanged(int value);

private:
    int m_value = 0;
};
```

---

## 总结

Qt Widgets 的核心要点：

1. **基础控件** - QPushButton、QLineEdit、QLabel 等
2. **容器控件** - QGroupBox、QTabWidget、QSplitter
3. **列表控件** - QListWidget、QTreeWidget、QTableWidget
4. **对话框** - 标准对话框和自定义 QDialog
5. **布局** - QHBoxLayout、QVBoxLayout、QGridLayout、QFormLayout

熟练使用这些控件和布局是 Qt GUI 开发的基础。
