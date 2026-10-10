#include "EditorWindow.h"
#include "NodeGraphView.h"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontMetrics>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainterPath>
#include <QPushButton>
#include <QSaveFile>
#include <QScrollBar>
#include <QSettings>
#include <QSignalBlocker>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTableWidget>
#include <QTimer>
#include <QToolBar>
#include <QUndoCommand>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_set>
#include <utility>

namespace {
constexpr int kCategoryRole = Qt::UserRole + 2;
constexpr int kIndexRole = Qt::UserRole + 1;
constexpr int kGraphLimit = 14;

class CategoryProxy final : public QSortFilterProxyModel {
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;
    QString category;
protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override {
        if (!QSortFilterProxyModel::filterAcceptsRow(row, parent)) return false;
        if (category.isEmpty()) return true;
        const QModelIndex index = sourceModel()->index(row, 0, parent);
        return sourceModel()->data(index, kCategoryRole).toString() == category;
    }
};

class SnapshotCommand final : public QUndoCommand {
public:
    SnapshotCommand(EditorWindow* editor, std::string before, std::string after, const QString& label)
        : QUndoCommand(label), editor_(editor), before_(std::move(before)), after_(std::move(after)) {}
    void undo() override { editor_->restoreSnapshot(before_); }
    void redo() override {
        // The UI has already applied the initial mutation prior to QUndoStack::push().
        if (initial_) { initial_ = false; return; }
        editor_->restoreSnapshot(after_);
    }
private:
    EditorWindow* editor_;
    std::string before_, after_;
    bool initial_ = true;
};

QLabel* caption(const QString& text) {
    auto* label = new QLabel(text);
    label->setObjectName("panelCaption");
    return label;
}

QPushButton* smallButton(const QString& text) {
    auto* button = new QPushButton(text);
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(32);
    return button;
}

QString prettyCategory(const QString& value) {
    if (value == "Infantry") return QObject::tr("步兵");
    if (value == "Vehicle") return QObject::tr("载具");
    if (value == "Aircraft") return QObject::tr("航空器");
    if (value == "Building") return QObject::tr("建筑");
    if (value == "Weapon") return QObject::tr("武器");
    if (value == "Warhead") return QObject::tr("弹头");
    if (value == "Projectile") return QObject::tr("弹体");
    return QObject::tr("通用");
}
}

EditorWindow::EditorWindow(QWidget* parent) : QMainWindow(parent) {
    undo_ = new QUndoStack(this);
    undo_->setUndoLimit(20);
    createUi();
    createMenus();
    connect(undo_, &QUndoStack::cleanChanged, this, &EditorWindow::updateTitle);
    QSettings settings;
    setTheme(settings.value("appearance/theme", "midnight").toString());
    const auto geometry = settings.value("window/geometry").toByteArray();
    if (!geometry.isEmpty()) restoreGeometry(geometry);
    else resize(1480, 900);
    document_.parse("");
    refreshDocument();
    undo_->setClean();
    const auto args = QCoreApplication::arguments();
    if (args.size() > 1) openPath(args[1]);
}

void EditorWindow::createUi() {
    auto* host = new QWidget(this);
    auto* frame = new QVBoxLayout(host);
    frame->setContentsMargins(12, 10, 12, 10);
    frame->setSpacing(10);

    auto* top = new QHBoxLayout;
    auto* brand = new QLabel("RA2  /  INI NODE EDITOR");
    brand->setObjectName("brand");
    top->addWidget(brand);
    fileInfo_ = new QLabel(tr("未打开文件"));
    fileInfo_->setObjectName("subtle");
    fileInfo_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    top->addSpacing(20);
    top->addWidget(fileInfo_, 1);
    frame->addLayout(top);

    auto* splitter = new QSplitter(Qt::Horizontal, host);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(9);

    auto* left = new QWidget;
    auto* leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(10, 14, 10, 10);
    leftLayout->setSpacing(9);
    left->setObjectName("pane");
    leftLayout->addWidget(caption(tr("SECTION EXPLORER  /  节浏览器")));
    search_ = new QLineEdit;
    search_->setPlaceholderText(tr("搜索节名称...") );
    search_->setClearButtonEnabled(true);
    leftLayout->addWidget(search_);
    categoryFilter_ = new QComboBox;
    categoryFilter_->addItem(tr("全部对象类型"), "");
    for (const QString& kind : {"Infantry", "Vehicle", "Aircraft", "Building", "Weapon", "Warhead", "Projectile", "Other"})
        categoryFilter_->addItem(prettyCategory(kind), kind);
    leftLayout->addWidget(categoryFilter_);
    sections_ = new QListView;
    sections_->setUniformItemSizes(true);
    sections_->setAlternatingRowColors(true);
    sections_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    sections_->setSelectionMode(QAbstractItemView::SingleSelection);
    sections_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    sectionModel_ = new QStandardItemModel(this);
    auto* proxy = new CategoryProxy(this);
    filterModel_ = proxy;
    proxy->setSourceModel(sectionModel_);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setFilterKeyColumn(0);
    sections_->setModel(proxy);
    leftLayout->addWidget(sections_, 1);
    countInfo_ = new QLabel;
    countInfo_->setObjectName("subtle");
    leftLayout->addWidget(countInfo_);
    auto* sectionButtons = new QHBoxLayout;
    auto* newSection = smallButton(tr("+ 新增"));
    auto* rename = smallButton(tr("重命名"));
    auto* removeSection = smallButton(tr("删除"));
    sectionButtons->addWidget(newSection); sectionButtons->addWidget(rename); sectionButtons->addWidget(removeSection);
    leftLayout->addLayout(sectionButtons);
    connect(newSection, &QPushButton::clicked, this, &EditorWindow::addSection);
    connect(rename, &QPushButton::clicked, this, &EditorWindow::renameSection);
    connect(removeSection, &QPushButton::clicked, this, &EditorWindow::deleteSection);
    splitter->addWidget(left);

    auto* center = new QWidget;
    auto* centerLayout = new QVBoxLayout(center);
    centerLayout->setContentsMargins(4, 6, 4, 6);
    centerLayout->setSpacing(7);
    auto* graphTop = new QHBoxLayout;
    graphTop->addWidget(caption(tr("RELATION GRAPH  /  节点关系")));
    graphTop->addStretch();
    auto* resetZoom = smallButton(tr("适应画布"));
    graphTop->addWidget(resetZoom);
    centerLayout->addLayout(graphTop);
    graph_ = new NodeGraphView;
    graphScene_ = new QGraphicsScene(this);
    graph_->setScene(graphScene_);
    graph_->setMinimumWidth(300);
    centerLayout->addWidget(graph_, 1);
    auto* graphHint = new QLabel(tr("滚轮缩放 · 拖动画布 · 点击关联节点跳转 · 每侧最多显示 14 个邻居"));
    graphHint->setObjectName("subtle");
    graphHint->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(graphHint);
    connect(resetZoom, &QPushButton::clicked, graph_, &NodeGraphView::resetZoom);
    splitter->addWidget(center);

    auto* right = new QWidget;
    right->setObjectName("pane");
    auto* rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(10, 14, 10, 10);
    rightLayout->setSpacing(9);
    rightLayout->addWidget(caption(tr("INSPECTOR  /  属性检查器")));
    sectionInfo_ = new QLabel(tr("选择一个节查看属性"));
    sectionInfo_->setObjectName("sectionTitle");
    sectionInfo_->setWordWrap(true);
    rightLayout->addWidget(sectionInfo_);
    properties_ = new QTableWidget;
    properties_->setColumnCount(2);
    properties_->setHorizontalHeaderLabels({tr("键 / KEY"), tr("值 / VALUE")});
    properties_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    properties_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    properties_->verticalHeader()->hide();
    properties_->setAlternatingRowColors(true);
    properties_->setSelectionBehavior(QAbstractItemView::SelectRows);
    properties_->setSelectionMode(QAbstractItemView::SingleSelection);
    properties_->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    rightLayout->addWidget(properties_, 1);
    auto* propertyButtons = new QHBoxLayout;
    auto* newProperty = smallButton(tr("+ 新增属性"));
    auto* deleteProperty = smallButton(tr("删除属性"));
    propertyButtons->addWidget(newProperty); propertyButtons->addWidget(deleteProperty);
    rightLayout->addLayout(propertyButtons);
    rightLayout->addWidget(new QLabel(tr("双击单元格修改。更改可撤销，保存时保留未修改行的原始内容。")));
    connect(newProperty, &QPushButton::clicked, this, &EditorWindow::addProperty);
    connect(deleteProperty, &QPushButton::clicked, this, &EditorWindow::deleteProperty);
    splitter->addWidget(right);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);
    splitter->setSizes({270, 820, 380});
    frame->addWidget(splitter, 1);
    setCentralWidget(host);
    statusBar()->showMessage(tr("就绪 · Ctrl+O 打开 INI · Ctrl+S 保存"));

    filterTimer_ = new QTimer(this);
    filterTimer_->setSingleShot(true);
    filterTimer_->setInterval(100);
    connect(search_, &QLineEdit::textChanged, filterTimer_, qOverload<>(&QTimer::start));
    connect(filterTimer_, &QTimer::timeout, this, [this] {
        filterModel_->setFilterFixedString(search_->text());
        countInfo_->setText(tr("当前显示 %1 / %2 个节").arg(filterModel_->rowCount()).arg(sectionModel_->rowCount()));
    });
    connect(categoryFilter_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, proxy] {
        proxy->category = categoryFilter_->currentData().toString();
        proxy->invalidate();
        countInfo_->setText(tr("当前显示 %1 / %2 个节").arg(proxy->rowCount()).arg(sectionModel_->rowCount()));
    });
    connect(sections_->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex& current) {
                if (!current.isValid()) return;
                const int index = filterModel_->mapToSource(current).data(kIndexRole).toInt();
                if (index != selectedSection_) selectSection(index, false);
            });
    connect(graphScene_, &QGraphicsScene::selectionChanged, this, [this] {
        const auto selectedItems = graphScene_->selectedItems();
        if (!selectedItems.empty()) {
            const int index = selectedItems.first()->data(0).toInt();
            if (index != selectedSection_) selectSection(index);
        }
    });
    connect(properties_, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        const int section = selectedSection_, row = item->row(), column = item->column();
        if (section < 0 || section >= static_cast<int>(document_.sections().size()) ||
            row < 0 || row >= static_cast<int>(document_.sections()[section].entries.size())) return;
        const QString value = item->text();
        const QString current = decode(column == 0 ? document_.sections()[section].entries[row].key
                                                     : document_.sections()[section].entries[row].value);
        if (value == current) return;
        applyEdit(column == 0 ? tr("重命名属性") : tr("修改属性值"), [=, this](IniDocument& d) {
            return column == 0 ? d.renameKey(section, row, encode(value)) : d.setValue(section, row, encode(value));
        });
    });
}

void EditorWindow::createMenus() {
    QMenu* file = menuBar()->addMenu(tr("文件(&F)"));
    QAction* newAction = file->addAction(tr("新建 INI"), this, &EditorWindow::newFile, QKeySequence::New);
    QAction* openAction = file->addAction(tr("打开 INI..."), this, &EditorWindow::openFile, QKeySequence::Open);
    QAction* saveAction = file->addAction(tr("保存"), this, [this] { saveFile(false); }, QKeySequence::Save);
    file->addAction(tr("另存为..."), this, [this] { saveFile(true); }, QKeySequence::SaveAs);
    file->addSeparator();
    file->addAction(tr("退出"), this, &QWidget::close, QKeySequence::Quit);
    QMenu* edit = menuBar()->addMenu(tr("编辑(&E)"));
    auto* undoAction = undo_->createUndoAction(this, tr("撤销"));
    undoAction->setShortcut(QKeySequence::Undo);
    edit->addAction(undoAction);
    auto* redoAction = undo_->createRedoAction(this, tr("重做"));
    redoAction->setShortcut(QKeySequence::Redo);
    edit->addAction(redoAction);
    edit->addSeparator();
    edit->addAction(tr("新增节..."), this, &EditorWindow::addSection);
    edit->addAction(tr("新增属性..."), this, &EditorWindow::addProperty);
    QMenu* themes = menuBar()->addMenu(tr("主题(&T)"));
    for (const auto& entry : {std::pair{"midnight", tr("午夜蓝")}, {"dark", tr("深灰")}, {"light", tr("浅色")}}) {
        auto* action = themes->addAction(entry.second);
        action->setCheckable(true);
        connect(action, &QAction::triggered, this, [this, name = QString(entry.first)] { setTheme(name); });
    }
    auto* toolbar = addToolBar(tr("常用操作"));
    toolbar->setMovable(false);
    toolbar->addAction(newAction); toolbar->addAction(openAction); toolbar->addAction(saveAction);
    toolbar->addSeparator();
    toolbar->addAction(undo_->createUndoAction(this, tr("撤销")));
    toolbar->addAction(undo_->createRedoAction(this, tr("重做")));
}

EditorWindow::Palette EditorWindow::palette() const {
    if (theme_ == "light") return {"#f4f7fb", "#ffffff", "#eaf0f7", "#17233a", "#63748d", "#dbe3ee", "#3365da", "#f2f6fc", "#cfdaea"};
    if (theme_ == "dark") return {"#202124", "#2c2e33", "#34383e", "#eef0f3", "#afb4c0", "#484d55", "#8b9afe", "#202328", "#353b45"};
    return {"#0b1220", "#151f30", "#202c3f", "#e5efff", "#97acc7", "#34445c", "#60a5fa", "#0c1728", "#26354a"};
}

void EditorWindow::setTheme(const QString& name) {
    theme_ = (name == "light" || name == "dark") ? name : "midnight";
    QSettings().setValue("appearance/theme", theme_);
    const auto p = palette();
    // Token-based QSS: three palettes share layout and interactions.
    const QString qss = QString(R"(
        QWidget { background: %1; color: %4; font-family: 'Segoe UI', 'Microsoft YaHei UI', sans-serif; font-size: 13px; }
        QMainWindow, QMenuBar, QToolBar, QStatusBar { background: %1; }
        QWidget#pane { background: %2; border: 1px solid %6; border-radius: 11px; }
        QLabel#brand { color: %7; font-weight: 800; font-size: 18px; letter-spacing: 1px; }
        QLabel#panelCaption { color: %5; font-weight: 700; font-size: 11px; }
        QLabel#sectionTitle { color: %4; font-size: 18px; font-weight: 700; padding: 5px; }
        QLabel#subtle { color: %5; font-size: 11px; }
        QLineEdit, QComboBox, QListView, QTableWidget {
            background: %3; border: 1px solid %6; border-radius: 6px; padding: 6px; selection-background-color: %7;
        }
        QListView::item { min-height: 25px; padding-left: 8px; }
        QListView::item:selected, QTableWidget::item:selected { background: %7; color: white; }
        QHeaderView::section { background: %2; color: %5; padding: 9px; border: none; border-bottom: 1px solid %6; }
        QPushButton { background: %3; border: 1px solid %6; border-radius: 6px; padding: 6px 11px; font-weight: 600; }
        QPushButton:hover { background: %7; color: white; }
        QPushButton:pressed { background: %6; }
        QMenu { background: %2; border: 1px solid %6; }
        QMenu::item:selected { background: %7; }
        QToolBar { spacing: 5px; border: 0; padding: 5px; }
        QToolButton { border-radius: 5px; padding: 5px 10px; }
        QToolButton:hover { background: %3; }
        QSplitter::handle { background: transparent; }
        QScrollBar:vertical { width: 9px; background: %2; }
        QScrollBar::handle:vertical { background: %6; border-radius: 4px; min-height: 25px; }
    )").arg(p.background, p.panel, p.panelAlt, p.text, p.muted, p.border, p.accent);
    qApp->setStyleSheet(qss);
    graph_->setCanvasColors(QColor(p.canvas), QColor(p.dot));
    for (auto* action : menuBar()->actions()) {
        if (auto* menu = action->menu(); menu && action->text().startsWith(tr("主题"))) {
            for (auto* choice : menu->actions())
                choice->setChecked((theme_ == "midnight" && choice->text() == tr("午夜蓝")) ||
                                   (theme_ == "dark" && choice->text() == tr("深灰")) ||
                                   (theme_ == "light" && choice->text() == tr("浅色")));
        }
    }
    if (sectionModel_) {
        for (int i = 0; i < sectionModel_->rowCount(); ++i) {
            auto* item = sectionModel_->item(i);
            item->setForeground(colorFor(item->data(kCategoryRole).toString()));
        }
    }
    updateGraph();
}

QString EditorWindow::decode(const std::string& bytes) const {
    const QByteArray raw(bytes.data(), static_cast<qsizetype>(bytes.size()));
    return document_.utf8() ? QString::fromUtf8(raw) : QString::fromLocal8Bit(raw);
}

std::string EditorWindow::encode(const QString& str) const {
    const QByteArray bytes = document_.utf8() ? str.toUtf8() : str.toLocal8Bit();
    return std::string(bytes.constData(), static_cast<std::size_t>(bytes.size()));
}

QString EditorWindow::categoryFor(int index) const {
    if (index < 0 || index >= static_cast<int>(document_.sections().size())) return "Other";
    const auto& sec = document_.sections()[index];
    const auto it = objectKinds_.find(IniDocument::lower(sec.name));
    if (it != objectKinds_.end()) return it->second;
    const std::string name = IniDocument::lower(sec.name);
    if (name == "infantrytypes" || name == "vehicletypes" || name == "aircrafttypes" || name == "buildingtypes") return "Other";
    bool weapon = false;
    for (const auto& entry : sec.entries) {
        const auto key = IniDocument::lower(entry.key);
        if (key == "damage" || key == "rof" || key == "warhead" || key == "projectile") weapon = true;
    }
    return weapon ? "Weapon" : "Other";
}

QColor EditorWindow::colorFor(const QString& category) const {
    if (category == "Infantry") return QColor("#22b8a8");
    if (category == "Vehicle") return QColor("#e5a141");
    if (category == "Aircraft") return QColor("#6d9bff");
    if (category == "Building") return QColor("#b48af6");
    if (category == "Weapon") return QColor("#ed6b8a");
    if (category == "Warhead") return QColor("#ef7360");
    if (category == "Projectile") return QColor("#63b7d2");
    return QColor(theme_ == "light" ? "#6b7890" : "#9cabc4");
}

void EditorWindow::buildIndex() {
    sectionByName_.clear(); incoming_.clear(); objectKinds_.clear();
    const auto& list = document_.sections();
    for (int i = 0; i < static_cast<int>(list.size()); ++i)
        sectionByName_.try_emplace(IniDocument::lower(list[i].name), i);
    for (const auto& [group, kind] : {
         std::pair{"infantrytypes", "Infantry"}, {"vehicletypes", "Vehicle"},
         {"aircrafttypes", "Aircraft"}, {"buildingtypes", "Building"},
         {"warheads", "Warhead"}, {"projectiles", "Projectile"}}) {
        if (const auto iter = sectionByName_.find(group); iter != sectionByName_.end()) {
            for (const auto& item : list[iter->second].entries)
                objectKinds_[IniDocument::lower(item.value)] = kind;
        }
    }
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
        for (const auto& entry : list[i].entries) {
            const auto target = IniDocument::lower(entry.value);
            if (target != IniDocument::lower(list[i].name) && sectionByName_.contains(target))
                incoming_[target].push_back(i);
            const auto key = IniDocument::lower(entry.key);
            if (key == "warhead" && sectionByName_.contains(target)) objectKinds_.try_emplace(target, "Warhead");
            if (key == "projectile" && sectionByName_.contains(target)) objectKinds_.try_emplace(target, "Projectile");
        }
    }
}

void EditorWindow::refreshDocument(const QString& preferredName) {
    buildIndex();
    int index = selectedSection_;
    if (!preferredName.isEmpty()) {
        const auto it = sectionByName_.find(IniDocument::lower(encode(preferredName)));
        if (it != sectionByName_.end()) index = it->second;
    }
    {
        const QSignalBlocker blocker(sections_->selectionModel());
        sectionModel_->clear();
        for (int i = 0; i < static_cast<int>(document_.sections().size()); ++i) {
            const auto& section = document_.sections()[i];
            auto* item = new QStandardItem(decode(section.name));
            const QString category = categoryFor(i);
            item->setEditable(false);
            item->setData(i, kIndexRole);
            item->setData(category, kCategoryRole);
            item->setForeground(colorFor(category));
            item->setToolTip(prettyCategory(category) + tr(" · %1 个属性").arg(static_cast<qulonglong>(section.entries.size())));
            sectionModel_->appendRow(item);
        }
    }
    filterModel_->invalidate();
    countInfo_->setText(tr("当前显示 %1 / %2 个节").arg(filterModel_->rowCount()).arg(sectionModel_->rowCount()));
    if (index < 0 && !document_.sections().empty()) index = 0;
    if (index >= static_cast<int>(document_.sections().size())) index = static_cast<int>(document_.sections().size())-1;
    selectSection(index);
    updateTitle();
}

void EditorWindow::selectSection(int index, bool syncList) {
    if (index < -1 || index >= static_cast<int>(document_.sections().size())) return;
    selectedSection_ = index;
    if (syncList) {
        const QSignalBlocker blocker(sections_->selectionModel());
        if (index >= 0) {
            const auto src = sectionModel_->index(index, 0);
            const auto mapped = filterModel_->mapFromSource(src);
            if (mapped.isValid()) {
                sections_->setCurrentIndex(mapped);
                sections_->scrollTo(mapped, QAbstractItemView::EnsureVisible);
            } else sections_->clearSelection();
        } else sections_->clearSelection();
    }
    updateProperties();
    updateGraph();
}

void EditorWindow::updateProperties() {
    const QSignalBlocker blocker(properties_);
    properties_->clearContents();
    if (selectedSection_ < 0) {
        sectionInfo_->setText(tr("选择一个节查看属性")); properties_->setRowCount(0); return;
    }
    const auto& section = document_.sections()[selectedSection_];
    sectionInfo_->setText(decode(section.name) + "  ·  " + prettyCategory(categoryFor(selectedSection_)));
    properties_->setRowCount(static_cast<int>(section.entries.size()));
    for (int i = 0; i < static_cast<int>(section.entries.size()); ++i) {
        auto* key = new QTableWidgetItem(decode(section.entries[i].key));
        auto* value = new QTableWidgetItem(decode(section.entries[i].value));
        key->setToolTip(tr("双击重命名属性"));
        value->setToolTip(tr("双击编辑值"));
        properties_->setItem(i, 0, key);
        properties_->setItem(i, 1, value);
    }
}

void EditorWindow::updateGraph() {
    if (!graphScene_) return;
    const QSignalBlocker blocker(graphScene_);
    graphScene_->clear();
    if (selectedSection_ < 0 || selectedSection_ >= static_cast<int>(document_.sections().size())) {
        auto* note = graphScene_->addText(tr("打开 INI 文件并选择左侧的节\n查看对象之间的引用关系"));
        note->setDefaultTextColor(QColor(palette().muted));
        note->setPos(-130, -35);
        graphScene_->setSceneRect(-390, -260, 780, 520);
        graph_->resetTransform();
        return;
    }
    const auto& list = document_.sections();
    const int center = selectedSection_;
    std::vector<int> parents, children;
    std::unordered_set<int> parentSeen, childSeen;
    const auto key = IniDocument::lower(list[center].name);
    if (const auto i = incoming_.find(key); i != incoming_.end()) {
        for (int other : i->second) {
            if (other != center && parentSeen.insert(other).second) parents.push_back(other);
            if (parents.size() >= kGraphLimit) break;
        }
    }
    for (const auto& entry : list[center].entries) {
        if (const auto i = sectionByName_.find(IniDocument::lower(entry.value)); i != sectionByName_.end()) {
            const int other = i->second;
            if (other != center && childSeen.insert(other).second) children.push_back(other);
        }
        if (children.size() >= kGraphLimit) break;
    }
    const auto p = palette();
    constexpr qreal width = 220, height = 76, xGap = 315, yGap = 108;
    auto node = [&](int index, qreal x, qreal y, bool focused) {
        const auto tint = colorFor(categoryFor(index));
        const QRectF card(x - width/2, y - height/2, width, height);
        QPainterPath outline;
        outline.addRoundedRect(card, 12, 12);
        auto* graphic = graphScene_->addPath(outline, QPen(focused ? tint : QColor(p.border), focused ? 2.6 : 1),
                                              QColor(p.panel));
        graphic->setFlag(QGraphicsItem::ItemIsSelectable);
        graphic->setData(0, index);
        graphic->setZValue(2);
        auto* strip = new QGraphicsRectItem(card.left()+1, card.top()+10, 4, 55, graphic);
        strip->setPen(Qt::NoPen); strip->setBrush(tint); strip->setAcceptedMouseButtons(Qt::NoButton);
        QFont nameFont; nameFont.setPointSize(11); nameFont.setBold(true);
        const QString name = QFontMetrics(nameFont).elidedText(decode(list[index].name), Qt::ElideRight, 181);
        auto* title = new QGraphicsSimpleTextItem(name, graphic);
        title->setFont(nameFont); title->setBrush(QColor(p.text)); title->setPos(card.left()+18, card.top()+12);
        title->setAcceptedMouseButtons(Qt::NoButton);
        auto* subtitle = new QGraphicsSimpleTextItem(prettyCategory(categoryFor(index)) +
            tr("  ·  %1 属性").arg(static_cast<qulonglong>(list[index].entries.size())), graphic);
        QFont metaFont; metaFont.setPointSize(9);
        subtitle->setFont(metaFont); subtitle->setBrush(tint);
        subtitle->setPos(card.left()+18, card.top()+43);
        subtitle->setAcceptedMouseButtons(Qt::NoButton);
    };
    auto edge = [&](QPointF start, QPointF end, QColor tint) {
        QPainterPath curve(start);
        const qreal mid = (start.x() + end.x())/2;
        curve.cubicTo(mid, start.y(), mid, end.y(), end.x(), end.y());
        auto* item = graphScene_->addPath(curve, QPen(tint, 1.8));
        item->setZValue(-1);
    };
    const QColor wire = QColor(p.border);
    for (int i = 0; i < static_cast<int>(parents.size()); ++i) {
        const qreal y = (i - (static_cast<qreal>(parents.size())-1)/2)*yGap;
        edge(QPointF(-xGap+width/2, y), QPointF(-width/2, 0), wire);
        node(parents[i], -xGap, y, false);
    }
    for (int i = 0; i < static_cast<int>(children.size()); ++i) {
        const qreal y = (i - (static_cast<qreal>(children.size())-1)/2)*yGap;
        edge(QPointF(width/2, 0), QPointF(xGap-width/2, y), wire);
        node(children[i], xGap, y, false);
    }
    node(center, 0, 0, true);
    graphScene_->setSceneRect(graphScene_->itemsBoundingRect().adjusted(-80, -80, 80, 80));
    graph_->resetZoom();
    statusBar()->showMessage(tr("%1 · 入边 %2 · 出边 %3（单侧最多显示 %4）")
        .arg(decode(list[center].name)).arg(static_cast<qulonglong>(parents.size())).arg(static_cast<qulonglong>(children.size())).arg(kGraphLimit));
}

void EditorWindow::updateTitle() {
    const QString name = filePath_.isEmpty() ? tr("未命名.ini") : QFileInfo(filePath_).fileName();
    setWindowTitle(name + (undo_->isClean() ? "" : " *") + "  —  RA2 INI Node Editor");
    fileInfo_->setText(filePath_.isEmpty() ? tr("未保存 · 新文档") : filePath_);
    fileInfo_->setToolTip(filePath_);
    setWindowModified(!undo_->isClean());
}

void EditorWindow::restoreSnapshot(const std::string& bytes) {
    QString focus;
    if (selectedSection_ >= 0 && selectedSection_ < static_cast<int>(document_.sections().size()))
        focus = decode(document_.sections()[selectedSection_].name);
    document_.parse(bytes);
    refreshDocument(focus);
}

void EditorWindow::applyEdit(const QString& label, const std::function<bool(IniDocument&)>& change, const QString& focus) {
    const std::string before = document_.serialize();
    QString previous = focus;
    if (previous.isEmpty() && selectedSection_ >= 0 && selectedSection_ < static_cast<int>(document_.sections().size()))
        previous = decode(document_.sections()[selectedSection_].name);
    if (!change(document_)) {
        QMessageBox::warning(this, tr("操作未完成"), tr("名称可能重复、为空或包含非法字符。"));
        updateProperties();
        return;
    }
    const auto after = document_.serialize();
    if (before == after) return;
    refreshDocument(previous);
    undo_->push(new SnapshotCommand(this, before, after, label));
    updateTitle();
}

bool EditorWindow::maybeSave() {
    if (undo_->isClean()) return true;
    const auto answer = QMessageBox::warning(this, tr("未保存的更改"), tr("当前 INI 有未保存的更改，是否保存？"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if (answer == QMessageBox::Save) return saveFile();
    return answer == QMessageBox::Discard;
}

void EditorWindow::newFile() {
    if (!maybeSave()) return;
    filePath_.clear(); document_.parse(""); selectedSection_ = -1;
    undo_->clear(); undo_->setClean(); refreshDocument();
}

void EditorWindow::openFile() {
    if (!maybeSave()) return;
    const QString path = QFileDialog::getOpenFileName(this, tr("打开 RA2 INI 文件"), {}, tr("INI 文件 (*.ini);;全部文件 (*.*)"));
    if (!path.isEmpty()) openPath(path);
}

bool EditorWindow::openPath(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("无法打开"), file.errorString()); return false;
    }
    const auto bytes = file.readAll();
    document_.parse(std::string(bytes.constData(), static_cast<std::size_t>(bytes.size())));
    filePath_ = path; selectedSection_ = -1;
    search_->clear(); categoryFilter_->setCurrentIndex(0);
    undo_->clear(); undo_->setClean(); refreshDocument();
    statusBar()->showMessage(tr("已打开 %1 · %2 个节 · 原始字节安全保留")
        .arg(QFileInfo(path).fileName()).arg(static_cast<qulonglong>(document_.sections().size())), 5000);
    return true;
}

bool EditorWindow::saveFile(bool saveAs) {
    QString target = filePath_;
    if (saveAs || target.isEmpty()) {
        target = QFileDialog::getSaveFileName(this, tr("保存 INI 文件"), target, tr("INI 文件 (*.ini);;全部文件 (*.*)"));
        if (target.isEmpty()) return false;
        if (QFileInfo(target).suffix().isEmpty()) target += ".ini";
    }
    QSaveFile file(target);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("保存失败"), file.errorString()); return false;
    }
    const auto bytes = document_.serialize();
    if (file.write(bytes.data(), static_cast<qint64>(bytes.size())) != static_cast<qint64>(bytes.size()) || !file.commit()) {
        QMessageBox::critical(this, tr("保存失败"), file.errorString()); return false;
    }
    filePath_ = target;
    undo_->setClean(); updateTitle();
    statusBar()->showMessage(tr("已保存 %1").arg(target), 4000);
    return true;
}

void EditorWindow::addSection() {
    bool ok = false;
    const auto name = QInputDialog::getText(this, tr("添加节"), tr("Section 名称："), QLineEdit::Normal, {}, &ok);
    if (ok && !name.trimmed().isEmpty())
        applyEdit(tr("新增节"), [this, name](IniDocument& d) { return d.addSection(encode(name)); }, name.trimmed());
}

void EditorWindow::renameSection() {
    if (selectedSection_ < 0) return;
    bool ok = false;
    const auto name = QInputDialog::getText(this, tr("重命名节"), tr("Section 名称："), QLineEdit::Normal,
        decode(document_.sections()[selectedSection_].name), &ok);
    if (ok && !name.trimmed().isEmpty()) {
        const int index = selectedSection_;
        applyEdit(tr("重命名节"), [this, index, name](IniDocument& d) { return d.renameSection(index, encode(name)); }, name.trimmed());
    }
}

void EditorWindow::deleteSection() {
    if (selectedSection_ < 0) return;
    if (QMessageBox::question(this, tr("删除节"), tr("确认删除 [%1] 及其中全部属性？")
        .arg(decode(document_.sections()[selectedSection_].name))) != QMessageBox::Yes) return;
    const int index = selectedSection_;
    applyEdit(tr("删除节"), [index](IniDocument& d) { return d.removeSection(index); });
}

void EditorWindow::addProperty() {
    if (selectedSection_ < 0) return;
    bool ok = false;
    const auto key = QInputDialog::getText(this, tr("添加属性"), tr("属性名："), QLineEdit::Normal, {}, &ok);
    if (!ok || key.trimmed().isEmpty()) return;
    const auto value = QInputDialog::getText(this, tr("添加属性"), tr("属性值："), QLineEdit::Normal, {}, &ok);
    if (!ok) return;
    const int index = selectedSection_;
    applyEdit(tr("新增属性"), [this, index, key, value](IniDocument& d) {
        return d.addKey(index, encode(key), encode(value));
    });
}

void EditorWindow::deleteProperty() {
    if (selectedSection_ < 0 || properties_->currentRow() < 0) return;
    const int section = selectedSection_, row = properties_->currentRow();
    applyEdit(tr("删除属性"), [section, row](IniDocument& d) { return d.removeKey(section, row); });
}

void EditorWindow::closeEvent(QCloseEvent* event) {
    if (!maybeSave()) { event->ignore(); return; }
    QSettings().setValue("window/geometry", saveGeometry());
    event->accept();
}
