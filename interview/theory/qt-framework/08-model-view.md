# Qt Model/View 架构（Model/View Architecture）

## 概述

Qt 的 Model/View 架构是一种设计模式，将数据（Model）与显示（View）分离，通过委托（Delegate）处理渲染和编辑。这种架构提高了代码的可维护性和复用性。

---

## 1. 架构基础

### 1.1 MVC vs Model/View

**面试问题**：Qt 的 Model/View 和传统 MVC 有什么区别？

**答案**：

```
传统 MVC：
┌─────────┐    ┌─────────┐    ┌─────────┐
│  Model  │◄───│Controller│───►│  View   │
└─────────┘    └─────────┘    └─────────┘

Qt Model/View：
┌─────────┐    ┌─────────┐    ┌─────────┐
│  Model  │◄───│ Delegate│───►│  View   │
└─────────┘    └─────────┘    └─────────┘
```

**区别**：
- Qt 将 Controller 的功能分散到 View 和 Delegate 中
- Delegate 负责渲染和编辑
- View 处理用户交互
- 更简化，适合 GUI 数据展示

---

### 1.2 核心类

**面试问题**：Model/View 架构中有哪些核心类？

**答案**：

```cpp
// Model 基类
// QAbstractItemModel    - 所有模型的基类
// QAbstractListModel    - 列表模型基类
// QAbstractTableModel   - 表格模型基类
// QStandardItemModel    - 通用标准模型
// QStringListModel      - 字符串列表模型
// QSortFilterProxyModel - 排序/过滤代理模型
// QSqlTableModel        - SQL 表模型
// QFileSystemModel      - 文件系统模型

// View 类
// QListView             - 列表视图
// QTableView            - 表格视图
// QTreeView             - 树形视图
// QColumnView           - 列视图
// QHeaderView           - 表头视图

// Delegate 类
// QAbstractItemDelegate - 委托基类
// QStyledItemDelegate   - 样式化委托（推荐）
// QItemDelegate         - 传统委托

// 便捷类（集成 Model + View）
// QListWidget           - 列表控件
// QTableWidget          - 表格控件
// QTreeWidget           - 树形控件
```

---

### 1.3 QModelIndex

**面试问题**：QModelIndex 是什么？如何使用它？

**答案**：

```cpp
#include <QModelIndex>
#include <QAbstractItemModel>

void modelIndexDemo(QAbstractItemModel* model) {
    // QModelIndex 是模型中数据项的"地址"

    // 创建索引
    QModelIndex index = model->index(0, 0);  // 第 0 行，第 0 列

    // 带父索引的创建（树形结构）
    QModelIndex parentIndex = model->index(0, 0);
    QModelIndex childIndex = model->index(0, 0, parentIndex);

    // 检查有效性
    if (index.isValid()) {
        // 获取数据
        QVariant data = model->data(index, Qt::DisplayRole);
        QVariant editData = model->data(index, Qt::EditRole);
        QVariant decoration = model->data(index, Qt::DecorationRole);

        // 获取位置信息
        int row = index.row();
        int column = index.column();
        QModelIndex parent = index.parent();

        // 获取所属模型
        const QAbstractItemModel* m = index.model();
    }

    // 无效索引
    QModelIndex invalid;  // 默认构造是无效的
    Q_ASSERT(!invalid.isValid());

    // 兄弟索引
    QModelIndex sibling = index.sibling(1, 0);  // 同列的第 1 行
    QModelIndex siblingRow = index.siblingAtRow(2);  // 同列的第 2 行
    QModelIndex siblingCol = index.siblingAtColumn(1);  // 同行的第 1 列
}

// 注意：QModelIndex 是临时的，不要长期存储
// 如需持久化索引，使用 QPersistentModelIndex
void persistentIndexDemo(QAbstractItemModel* model) {
    QModelIndex tempIndex = model->index(0, 0);

    // 持久化索引，模型变化时自动更新
    QPersistentModelIndex persistentIndex(tempIndex);

    // 即使模型增删行，persistentIndex 仍然指向正确的数据
    model->insertRow(0);  // 插入新行

    // persistentIndex 自动更新
    qDebug() << "New row:" << persistentIndex.row();  // 现在是 1
}
```

---

## 2. 自定义模型

### 2.1 继承 QAbstractListModel

**面试问题**：如何实现一个自定义列表模型？

**答案**：

```cpp
#include <QAbstractListModel>

struct Contact {
    QString name;
    QString phone;
    QString email;
};

class ContactListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PhoneRole,
        EmailRole
    };

    explicit ContactListModel(QObject* parent = nullptr)
        : QAbstractListModel(parent) {}

    // 必须实现：行数
    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;  // 列表模型没有子项
        return m_contacts.size();
    }

    // 必须实现：数据
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_contacts.size())
            return QVariant();

        const Contact& contact = m_contacts[index.row()];

        switch (role) {
        case Qt::DisplayRole:
        case NameRole:
            return contact.name;
        case PhoneRole:
            return contact.phone;
        case EmailRole:
            return contact.email;
        default:
            return QVariant();
        }
    }

    // 可选：角色名（用于 QML）
    QHash<int, QByteArray> roleNames() const override {
        return {
            {NameRole, "name"},
            {PhoneRole, "phone"},
            {EmailRole, "email"}
        };
    }

    // 可选：设置数据
    bool setData(const QModelIndex& index, const QVariant& value, int role) override {
        if (!index.isValid() || index.row() >= m_contacts.size())
            return false;

        Contact& contact = m_contacts[index.row()];

        switch (role) {
        case NameRole:
            contact.name = value.toString();
            break;
        case PhoneRole:
            contact.phone = value.toString();
            break;
        case EmailRole:
            contact.email = value.toString();
            break;
        default:
            return false;
        }

        emit dataChanged(index, index, {role});
        return true;
    }

    // 可选：标志
    Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    // 自定义方法
    void addContact(const Contact& contact) {
        beginInsertRows(QModelIndex(), m_contacts.size(), m_contacts.size());
        m_contacts.append(contact);
        endInsertRows();
    }

    void removeContact(int row) {
        if (row < 0 || row >= m_contacts.size()) return;

        beginRemoveRows(QModelIndex(), row, row);
        m_contacts.removeAt(row);
        endRemoveRows();
    }

    void setContacts(const QList<Contact>& contacts) {
        beginResetModel();
        m_contacts = contacts;
        endResetModel();
    }

private:
    QList<Contact> m_contacts;
};
```

---

### 2.2 继承 QAbstractTableModel

**面试问题**：如何实现一个自定义表格模型？

**答案**：

```cpp
#include <QAbstractTableModel>

class SpreadsheetModel : public QAbstractTableModel {
    Q_OBJECT

public:
    SpreadsheetModel(int rows, int cols, QObject* parent = nullptr)
        : QAbstractTableModel(parent)
        , m_rowCount(rows)
        , m_colCount(cols)
    {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;
        return m_rowCount;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;
        return m_colCount;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid()) return QVariant();

        QPair<int, int> key(index.row(), index.column());

        switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return m_data.value(key, "");
        case Qt::TextAlignmentRole:
            return int(Qt::AlignCenter);
        case Qt::BackgroundRole:
            if (index.row() % 2 == 0)
                return QColor(240, 240, 240);
            break;
        }

        return QVariant();
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role) override {
        if (!index.isValid() || role != Qt::EditRole)
            return false;

        QPair<int, int> key(index.row(), index.column());
        m_data[key] = value.toString();

        emit dataChanged(index, index, {role});
        return true;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role != Qt::DisplayRole) return QVariant();

        if (orientation == Qt::Horizontal) {
            // A, B, C, ... Z, AA, AB, ...
            return columnToLetter(section);
        } else {
            return section + 1;
        }
    }

    // 插入行
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override {
        if (parent.isValid()) return false;

        beginInsertRows(parent, row, row + count - 1);
        m_rowCount += count;
        // 需要移动现有数据...
        endInsertRows();
        return true;
    }

    // 删除行
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override {
        if (parent.isValid()) return false;

        beginRemoveRows(parent, row, row + count - 1);
        m_rowCount -= count;
        // 需要移动现有数据并删除...
        endRemoveRows();
        return true;
    }

private:
    QString columnToLetter(int col) const {
        QString result;
        while (col >= 0) {
            result.prepend(QChar('A' + col % 26));
            col = col / 26 - 1;
        }
        return result;
    }

    int m_rowCount;
    int m_colCount;
    QHash<QPair<int, int>, QString> m_data;
};
```

---

### 2.3 继承 QAbstractItemModel（树形）

**面试问题**：如何实现一个树形模型？

**答案**：

```cpp
#include <QAbstractItemModel>

struct TreeItem {
    QString name;
    QVariant data;
    TreeItem* parent = nullptr;
    QList<TreeItem*> children;

    ~TreeItem() {
        qDeleteAll(children);
    }

    int row() const {
        if (parent) {
            return parent->children.indexOf(const_cast<TreeItem*>(this));
        }
        return 0;
    }
};

class TreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit TreeModel(QObject* parent = nullptr)
        : QAbstractItemModel(parent)
        , m_rootItem(new TreeItem{"Root", QVariant()})
    {}

    ~TreeModel() {
        delete m_rootItem;
    }

    // 必须实现
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        TreeItem* parentItem = getItem(parent);
        TreeItem* childItem = parentItem->children.value(row);

        if (childItem)
            return createIndex(row, column, childItem);
        return QModelIndex();
    }

    QModelIndex parent(const QModelIndex& index) const override {
        if (!index.isValid())
            return QModelIndex();

        TreeItem* childItem = getItem(index);
        TreeItem* parentItem = childItem->parent;

        if (parentItem == m_rootItem)
            return QModelIndex();

        return createIndex(parentItem->row(), 0, parentItem);
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        TreeItem* parentItem = getItem(parent);
        return parentItem->children.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 2;  // name 和 data 两列
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid())
            return QVariant();

        TreeItem* item = getItem(index);

        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return index.column() == 0 ? item->name : item->data;
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            return section == 0 ? "Name" : "Value";
        }
        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    // 添加节点
    void addItem(const QString& name, const QVariant& data, const QModelIndex& parentIndex) {
        TreeItem* parent = getItem(parentIndex);

        beginInsertRows(parentIndex, parent->children.size(), parent->children.size());

        TreeItem* item = new TreeItem{name, data, parent};
        parent->children.append(item);

        endInsertRows();
    }

private:
    TreeItem* getItem(const QModelIndex& index) const {
        if (index.isValid()) {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_rootItem;
    }

    TreeItem* m_rootItem;
};
```

---

## 3. 代理模型

### 3.1 QSortFilterProxyModel

**面试问题**：如何实现数据的排序和过滤？

**答案**：

```cpp
#include <QSortFilterProxyModel>

// 方式 1：直接使用
void basicProxyUsage() {
    QStandardItemModel* sourceModel = new QStandardItemModel();
    // 填充数据...

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(sourceModel);

    // 启用排序
    proxyModel->setSortRole(Qt::DisplayRole);
    proxyModel->sort(0, Qt::AscendingOrder);

    // 设置过滤
    proxyModel->setFilterKeyColumn(0);  // 过滤第 0 列
    proxyModel->setFilterRegularExpression("^A.*");  // 以 A 开头
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // 连接到视图
    QTableView* view = new QTableView();
    view->setModel(proxyModel);
    view->setSortingEnabled(true);
}

// 方式 2：自定义代理模型
class CustomFilterModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit CustomFilterModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent) {}

    void setMinAge(int age) {
        m_minAge = age;
        invalidateFilter();  // 触发重新过滤
    }

    void setMaxAge(int age) {
        m_maxAge = age;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
        // 获取源数据
        QModelIndex ageIndex = sourceModel()->index(sourceRow, 1, sourceParent);
        int age = sourceModel()->data(ageIndex).toInt();

        // 年龄过滤
        if (age < m_minAge || age > m_maxAge)
            return false;

        // 再应用文本过滤
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    }

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override {
        // 自定义排序逻辑
        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);

        // 数字列按数值排序
        if (left.column() == 1) {
            return leftData.toInt() < rightData.toInt();
        }

        // 其他列按字符串排序
        return leftData.toString() < rightData.toString();
    }

private:
    int m_minAge = 0;
    int m_maxAge = 200;
};
```

---

### 3.2 多级代理

**面试问题**：可以串联多个代理模型吗？

**答案**：

```cpp
void chainedProxyDemo() {
    QStandardItemModel* sourceModel = new QStandardItemModel();

    // 第一级代理：过滤
    QSortFilterProxyModel* filterProxy = new QSortFilterProxyModel();
    filterProxy->setSourceModel(sourceModel);
    filterProxy->setFilterKeyColumn(0);

    // 第二级代理：排序
    QSortFilterProxyModel* sortProxy = new QSortFilterProxyModel();
    sortProxy->setSourceModel(filterProxy);

    // 自定义装饰代理
    class DecoratorProxy : public QIdentityProxyModel {
    public:
        QVariant data(const QModelIndex& index, int role) const override {
            if (role == Qt::BackgroundRole && index.row() % 2 == 0) {
                return QColor(240, 240, 255);
            }
            return QIdentityProxyModel::data(index, role);
        }
    };

    // 第三级代理：装饰
    DecoratorProxy* decoratorProxy = new DecoratorProxy();
    decoratorProxy->setSourceModel(sortProxy);

    // 视图使用最终代理
    QTableView* view = new QTableView();
    view->setModel(decoratorProxy);

    // 索引转换
    QModelIndex proxyIndex = view->currentIndex();

    // 代理索引 → 源索引
    QModelIndex filterIndex = decoratorProxy->mapToSource(proxyIndex);
    QModelIndex sourceIndex = filterProxy->mapToSource(
        sortProxy->mapToSource(filterIndex));

    // 源索引 → 代理索引
    QModelIndex newProxyIndex = decoratorProxy->mapFromSource(
        sortProxy->mapFromSource(
            filterProxy->mapFromSource(sourceIndex)));
}
```

---

## 4. 委托（Delegate）

### 4.1 自定义委托

**面试问题**：如何自定义单元格的渲染和编辑？

**答案**：

```cpp
#include <QStyledItemDelegate>
#include <QPainter>
#include <QSpinBox>
#include <QComboBox>

class CustomDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit CustomDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    // 自定义绘制
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override {
        // 处理特殊列
        if (index.column() == 2) {
            // 绘制进度条
            int progress = index.data().toInt();

            QStyleOptionProgressBar progressBarOption;
            progressBarOption.rect = option.rect;
            progressBarOption.minimum = 0;
            progressBarOption.maximum = 100;
            progressBarOption.progress = progress;
            progressBarOption.text = QString("%1%").arg(progress);
            progressBarOption.textVisible = true;

            QApplication::style()->drawControl(
                QStyle::CE_ProgressBar, &progressBarOption, painter);
            return;
        }

        if (index.column() == 3) {
            // 绘制星级评分
            int rating = index.data().toInt();

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            for (int i = 0; i < 5; ++i) {
                QRect starRect = option.rect;
                starRect.setWidth(16);
                starRect.translate(i * 18, 0);

                if (i < rating) {
                    painter->setBrush(Qt::yellow);
                } else {
                    painter->setBrush(Qt::gray);
                }

                drawStar(painter, starRect);
            }

            painter->restore();
            return;
        }

        // 默认绘制
        QStyledItemDelegate::paint(painter, option, index);
    }

    // 创建编辑器
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override {
        if (index.column() == 1) {
            // 年龄列：使用 SpinBox
            QSpinBox* spinBox = new QSpinBox(parent);
            spinBox->setMinimum(0);
            spinBox->setMaximum(150);
            return spinBox;
        }

        if (index.column() == 4) {
            // 状态列：使用 ComboBox
            QComboBox* comboBox = new QComboBox(parent);
            comboBox->addItems({"Active", "Inactive", "Pending"});
            return comboBox;
        }

        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    // 设置编辑器数据
    void setEditorData(QWidget* editor, const QModelIndex& index) const override {
        if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor)) {
            spinBox->setValue(index.data().toInt());
            return;
        }

        if (QComboBox* comboBox = qobject_cast<QComboBox*>(editor)) {
            comboBox->setCurrentText(index.data().toString());
            return;
        }

        QStyledItemDelegate::setEditorData(editor, index);
    }

    // 从编辑器获取数据
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override {
        if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor)) {
            model->setData(index, spinBox->value());
            return;
        }

        if (QComboBox* comboBox = qobject_cast<QComboBox*>(editor)) {
            model->setData(index, comboBox->currentText());
            return;
        }

        QStyledItemDelegate::setModelData(editor, model, index);
    }

    // 编辑器大小
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override {
        if (index.column() == 3) {
            return QSize(100, 24);  // 星级列固定大小
        }
        return QStyledItemDelegate::sizeHint(option, index);
    }

private:
    void drawStar(QPainter* painter, const QRect& rect) const {
        // 绘制五角星
        QPolygonF star;
        // ... 星形顶点计算
        painter->drawPolygon(star);
    }
};

// 使用
void setupDelegate(QTableView* view) {
    CustomDelegate* delegate = new CustomDelegate(view);
    view->setItemDelegate(delegate);

    // 或只为特定列设置委托
    view->setItemDelegateForColumn(1, new SpinBoxDelegate(view));
    view->setItemDelegateForColumn(4, new ComboBoxDelegate(view));
}
```

---

### 4.2 编辑触发模式

**面试问题**：如何控制单元格进入编辑模式？

**答案**：

```cpp
void editTriggersDemo(QTableView* view) {
    // 编辑触发模式
    view->setEditTriggers(
        QAbstractItemView::DoubleClicked |    // 双击编辑
        QAbstractItemView::SelectedClicked |  // 选中后单击编辑
        QAbstractItemView::EditKeyPressed |   // 按 F2 编辑
        QAbstractItemView::AnyKeyPressed      // 任意键开始输入
    );

    // 禁用编辑
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 仅双击编辑
    view->setEditTriggers(QAbstractItemView::DoubleClicked);

    // 程序化触发编辑
    QModelIndex index = view->model()->index(0, 0);
    view->edit(index);

    // 关闭当前编辑器
    view->closePersistentEditor(index);

    // 持久化编辑器（始终显示）
    view->openPersistentEditor(index);
}
```

---

## 5. 视图功能

### 5.1 选择模型

**面试问题**：如何处理视图中的选择？

**答案**：

```cpp
#include <QItemSelectionModel>

void selectionDemo(QTableView* view) {
    // 获取选择模型
    QItemSelectionModel* selectionModel = view->selectionModel();

    // 选择模式
    view->setSelectionMode(QAbstractItemView::SingleSelection);     // 单选
    view->setSelectionMode(QAbstractItemView::MultiSelection);      // 多选（点击切换）
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);   // 扩展选择（Ctrl/Shift）
    view->setSelectionMode(QAbstractItemView::ContiguousSelection); // 连续选择
    view->setSelectionMode(QAbstractItemView::NoSelection);         // 禁止选择

    // 选择行为
    view->setSelectionBehavior(QAbstractItemView::SelectItems);   // 选择单元格
    view->setSelectionBehavior(QAbstractItemView::SelectRows);    // 选择整行
    view->setSelectionBehavior(QAbstractItemView::SelectColumns); // 选择整列

    // 获取选中项
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
    QModelIndexList selectedRows = selectionModel->selectedRows();
    QModelIndexList selectedColumns = selectionModel->selectedColumns();

    // 当前项（焦点项）
    QModelIndex currentIndex = selectionModel->currentIndex();

    // 程序化选择
    QModelIndex index = view->model()->index(0, 0);
    selectionModel->select(index, QItemSelectionModel::Select);
    selectionModel->select(index, QItemSelectionModel::Deselect);
    selectionModel->select(index, QItemSelectionModel::Toggle);
    selectionModel->select(index, QItemSelectionModel::ClearAndSelect);

    // 选择范围
    QModelIndex topLeft = view->model()->index(0, 0);
    QModelIndex bottomRight = view->model()->index(5, 3);
    QItemSelection selection(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);

    // 设置当前项
    selectionModel->setCurrentIndex(index, QItemSelectionModel::Select);

    // 清除选择
    selectionModel->clearSelection();

    // 监听选择变化
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged,
                     [](const QItemSelection& selected, const QItemSelection& deselected) {
        qDebug() << "Selected:" << selected.indexes().size() << "items";
    });

    QObject::connect(selectionModel, &QItemSelectionModel::currentChanged,
                     [](const QModelIndex& current, const QModelIndex& previous) {
        qDebug() << "Current changed from" << previous << "to" << current;
    });
}
```

---

### 5.2 拖放支持

**面试问题**：如何在 Model/View 中实现拖放？

**答案**：

```cpp
// 在模型中支持拖放
class DragDropModel : public QAbstractListModel {
    Q_OBJECT

public:
    // 支持的拖放操作
    Qt::DropActions supportedDropActions() const override {
        return Qt::MoveAction | Qt::CopyAction;
    }

    // 拖拽时的 MIME 类型
    QStringList mimeTypes() const override {
        return {"application/x-mydata"};
    }

    // 创建拖拽数据
    QMimeData* mimeData(const QModelIndexList& indexes) const override {
        QMimeData* mimeData = new QMimeData();
        QByteArray encodedData;

        QDataStream stream(&encodedData, QIODevice::WriteOnly);

        for (const QModelIndex& index : indexes) {
            if (index.isValid()) {
                QString text = data(index, Qt::DisplayRole).toString();
                stream << text;
            }
        }

        mimeData->setData("application/x-mydata", encodedData);
        return mimeData;
    }

    // 处理放下
    bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                      int row, int column, const QModelIndex& parent) override {
        if (action == Qt::IgnoreAction)
            return true;

        if (!data->hasFormat("application/x-mydata"))
            return false;

        QByteArray encodedData = data->data("application/x-mydata");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int targetRow = row;
        if (row == -1) {
            targetRow = parent.isValid() ? parent.row() : rowCount();
        }

        while (!stream.atEnd()) {
            QString text;
            stream >> text;

            beginInsertRows(QModelIndex(), targetRow, targetRow);
            m_data.insert(targetRow, text);
            endInsertRows();

            ++targetRow;
        }

        return true;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override {
        Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

        if (index.isValid()) {
            return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        }

        return defaultFlags | Qt::ItemIsDropEnabled;
    }

private:
    QStringList m_data;
};

// 视图设置
void setupDragDrop(QListView* view) {
    view->setDragEnabled(true);
    view->setAcceptDrops(true);
    view->setDropIndicatorShown(true);
    view->setDragDropMode(QAbstractItemView::InternalMove);  // 内部移动
    // QAbstractItemView::DragDrop       // 完整拖放
    // QAbstractItemView::DragOnly       // 仅拖
    // QAbstractItemView::DropOnly       // 仅放

    view->setDefaultDropAction(Qt::MoveAction);
}
```

---

## 6. 常见面试题

### Q1: 为什么要使用 Model/View 而不是直接操作控件？

**答案**：

```cpp
// 不使用 Model/View 的问题
void badApproach() {
    QListWidget* list = new QListWidget();

    // 数据和显示混在一起
    list->addItem("Item 1");
    list->addItem("Item 2");

    // 要更改显示方式，必须重写控件
    // 数据逻辑与 UI 耦合
    // 难以共享数据到多个视图
    // 难以实现复杂的排序/过滤
}

// 使用 Model/View 的优势
void goodApproach() {
    QStringListModel* model = new QStringListModel();
    model->setStringList({"Item 1", "Item 2"});

    // 同一模型可用于多个视图
    QListView* listView = new QListView();
    listView->setModel(model);

    QComboBox* comboBox = new QComboBox();
    comboBox->setModel(model);

    // 模型变化自动反映到所有视图
    // 可轻松添加排序/过滤
    // 数据逻辑与显示分离
}
```

---

### Q2: beginInsertRows/endInsertRows 必须配对使用吗？

**答案**：

```cpp
// 必须配对使用，否则视图不会正确更新

// 正确做法
void insertCorrectly() {
    beginInsertRows(QModelIndex(), 0, 0);
    m_data.insert(0, newItem);
    endInsertRows();  // 通知视图更新
}

// 错误：忘记调用 end
void insertWrong() {
    beginInsertRows(QModelIndex(), 0, 0);
    m_data.insert(0, newItem);
    // 视图不会更新！
}

// 错误：先修改数据再调用 begin
void insertWrongOrder() {
    m_data.insert(0, newItem);  // 数据已变
    beginInsertRows(QModelIndex(), 0, 0);
    endInsertRows();
    // 可能导致崩溃或显示错误
}

// 批量操作使用 reset
void batchUpdate() {
    beginResetModel();
    m_data = newDataList;  // 大量数据变化
    endResetModel();
    // 但这会丢失选择状态
}
```

---

### Q3: QStandardItemModel 和自定义模型哪个更好？

**答案**：

| 场景 | 推荐 | 原因 |
|------|------|------|
| 原型/小数据 | QStandardItemModel | 快速实现 |
| 大数据集 | 自定义模型 | 按需加载，性能好 |
| 复杂数据结构 | 自定义模型 | 直接使用业务对象 |
| 简单列表/表格 | QStandardItemModel | 够用 |
| 需要 QML 集成 | 自定义模型 | 自定义 roleNames |

```cpp
// QStandardItemModel：每项都是 QStandardItem 对象
// 内存开销大，但使用方便

// 自定义模型：只存储必要数据
// 通过 index/data 方法按需访问
```

---

### Q4: 如何处理大数据量的模型？

**答案**：

```cpp
// 1. 延迟加载（Lazy Loading）
class LazyTreeModel : public QAbstractItemModel {
    // 只在需要时加载子节点
    bool canFetchMore(const QModelIndex& parent) const override {
        TreeItem* item = getItem(parent);
        return item->hasMoreChildren && item->children.isEmpty();
    }

    void fetchMore(const QModelIndex& parent) override {
        TreeItem* item = getItem(parent);
        // 加载子节点...
        beginInsertRows(parent, 0, newChildren.size() - 1);
        item->children = loadChildren(item);
        endInsertRows();
    }
};

// 2. 虚拟滚动（由视图自动处理）
// QTableView 只渲染可见行

// 3. 分页
class PagedModel : public QAbstractTableModel {
public:
    void setPage(int page) {
        beginResetModel();
        m_currentPage = page;
        loadPageData(page);
        endResetModel();
    }
};

// 4. 使用代理模型过滤
// 减少显示的数据量
```

---

## 总结

Qt Model/View 架构的核心要点：

1. **分离关注点** - Model 管理数据，View 显示数据，Delegate 定制渲染
2. **QModelIndex** - 数据项的地址，使用 row、column、parent 定位
3. **自定义模型** - 继承 QAbstractItemModel 及其子类
4. **代理模型** - QSortFilterProxyModel 实现排序和过滤
5. **委托** - QStyledItemDelegate 定制单元格渲染和编辑
6. **选择模型** - QItemSelectionModel 处理选择逻辑

掌握这些概念可以构建灵活、高效的数据驱动 UI。
