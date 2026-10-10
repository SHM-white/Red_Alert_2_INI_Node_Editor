#include "MainWindow.h"
#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QScrollArea>
#include <algorithm>
#include <QFile>
#include <QJsonDocument>
#include <QItemSelectionModel>
#include <QToolButton>
#include <QRegularExpressionValidator>
#include <QFrame>
#include <QTemporaryDir>
#include <QApplication>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QHeaderView>
#include <QMenu>
#include <QTextDocument>
#include <QFileDialog>
#include <QFontDatabase>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QUndoCommand>
#include <QTimer>
#include <QSet>

namespace {
constexpr int kCategoryRole = Qt::UserRole + 2;
constexpr int kIndexRole = Qt::UserRole + 1;
class CategoryProxy final : public QSortFilterProxyModel {
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;
    void setCategory(const QString& newCategory) {
        category_ = newCategory;
        invalidateFilter();
    }
protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override {
        if (!QSortFilterProxyModel::filterAcceptsRow(row, parent)) return false;
        return category_.isEmpty()
            || sourceModel()->data(sourceModel()->index(row, 0, parent), kCategoryRole).toString() == category_;
    }
private:
    QString category_;
};

class HistoryEntry : public QUndoCommand {
public:
    HistoryEntry(IniDocument* doc, QStringList before, QStringList after,
                 std::function<void()> notify, QString label)
        : doc_(doc), before_(std::move(before)), after_(std::move(after)),
          notify_(std::move(notify)) { setText(label); }
    void undo() override { doc_->setLines(before_); notify_(); }
    void redo() override { doc_->setLines(after_); notify_(); }
private:
    IniDocument* doc_;
    QStringList before_, after_;
    std::function<void()> notify_;
};

QLabel* makeTitle(const QString& text) {
    auto* label = new QLabel(text);
    label->setObjectName("sectionTitle");
    return label;
}
}

MainWindow::MainWindow() {
    setWindowTitle("RA2 Node Studio");
    resize(1440, 900);
    setMinimumSize(880, 550);
    history_.setUndoLimit(30);
    createUi();
    reloadThemeChoices(settings_.value("themes/selected", QString()).toString());
    encoding_->setCurrentIndex(settings_.value("encoding", 0).toInt());
    applyTheme();
    connect(&history_, &QUndoStack::cleanChanged, this, [this] { titleUpdate(); });
    refreshList();
    statusBar()->showMessage("就绪 · Ctrl+O 打开 rules.ini / art.ini");
    const auto savedGeometry = settings_.value("window/geometry").toByteArray();
    if (!savedGeometry.isEmpty()) restoreGeometry(savedGeometry);
}

void MainWindow::createUi() {
    auto* file = menuBar()->addMenu("文件(&F)");
    auto* edit = menuBar()->addMenu("编辑(&E)");
    auto* view = menuBar()->addMenu("视图(&V)");
    auto* themesMenu = menuBar()->addMenu("主题(&T)");
    auto* help = menuBar()->addMenu("帮助(&H)");
    auto* bar = addToolBar("快捷操作");
    bar->setMovable(false);

    auto add = [&](QMenu* menu, const QString& text, const QKeySequence& shortcut, auto fn) {
        QAction* action = menu->addAction(text);
        if (!shortcut.isEmpty()) action->setShortcut(shortcut);
        connect(action, &QAction::triggered, this, fn);
        return action;
    };
    const auto newFile = add(file, "新建", QKeySequence::New, [this] {
        if (!maybeSave()) return;
        document_.clear(); history_.clear(); history_.setClean(); current_ = -1;
        refreshList(); titleUpdate();
    });
    const auto openFile = add(file, "打开 INI...", QKeySequence::Open, [this] {
        const QString path = QFileDialog::getOpenFileName(
            this, "打开 INI", QDir::homePath(), "INI / map (*.ini *.map *.mpr);;所有文件 (*)");
        if (!path.isEmpty()) openPath(path);
    });
    const auto saveFile = add(file, "保存", QKeySequence::Save, [this] { save(); });
    add(file, "另存为...", QKeySequence::SaveAs, [this] { save(true); });
    file->addSeparator();
    add(file, "退出", QKeySequence::Quit, [this] { close(); });
    bar->addAction(newFile);
    bar->addAction(openFile);
    bar->addAction(saveFile);
    bar->addSeparator();

    auto* undo = history_.createUndoAction(this, "撤销");
    undo->setShortcut(QKeySequence::Undo);
    auto* redo = history_.createRedoAction(this, "重做");
    redo->setShortcut(QKeySequence::Redo);
    edit->addAction(undo); edit->addAction(redo);
    bar->addAction(undo); bar->addAction(redo);
    edit->addSeparator();
    add(edit, "新增节...", QKeySequence("Ctrl+Shift+N"), [this] { addSection(); });
    add(edit, "重命名节...", QKeySequence("F2"), [this] { renameSection(); });
    add(edit, "删除节", QKeySequence("Ctrl+Shift+D"), [this] { deleteSection(); });
    edit->addSeparator();
    add(edit, "新增属性...", QKeySequence("Ctrl+Insert"), [this] { addKey(); });
    add(edit, "删除选中属性", QKeySequence("Ctrl+Delete"), [this] { deleteKey(); });

    add(themesMenu, "编辑当前主题...", {}, [this] { editTheme(); });
    add(themesMenu, "自定义选中节点类型颜色...", {}, [this] { setCategoryColor(); });
    themesMenu->addSeparator();
    add(themesMenu, "导入主题 JSON...", {}, [this] { importTheme(); });
    add(themesMenu, "导出当前主题 JSON...", {}, [this] { exportTheme(); });
    add(themesMenu, "删除当前自定义主题...", {}, [this] { deleteTheme(); });
    add(view, "自定义节点类型颜色...", {}, [this] { setCategoryColor(); });
    add(help, "关于", {}, [this] {
        QMessageBox::about(this, "RA2 Node Studio",
            "基于 Qt 6 的红警 2 INI 节点编辑器。\n\n"
            "左侧筛选节，中间探索关系图或编辑源码，右侧修改当前节属性。\n"
            "每套主题都有独立的 UI 颜色和节点颜色。"
            "节点类别来自规则列表及字段特征，并非完整的游戏语义识别。");
    });

    bar->addSeparator();
    bar->addWidget(new QLabel(" 主题 "));
    theme_ = new QComboBox(this);
    theme_->setMinimumWidth(145);
    theme_->setToolTip("切换完整主题：背景、控件、节点分类配色同时更新");
    bar->addWidget(theme_);
    auto* themeEdit = new QPushButton("编辑主题", bar);
    bar->addWidget(themeEdit);
    connect(themeEdit, &QPushButton::clicked, this, [this] { editTheme(); });
    bar->addWidget(new QLabel("   编码 "));
    encoding_ = new QComboBox(this);
    encoding_->addItems({"UTF-8", "系统本地编码 (ANSI)"});
    encoding_->setToolTip("打开文件时采用的编码，简体中文 Windows 通常对应 GBK");
    bar->addWidget(encoding_);
    connect(theme_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        if (theme_->currentIndex() < 0) return;
        settings_.setValue("themes/selected", theme_->currentData().toString());
        applyTheme();
    });
    connect(encoding_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        settings_.setValue("encoding", index);
    });

    auto* host = new QWidget(this);
    host->setObjectName("centralHost");
    auto* page = new QVBoxLayout(host);
    page->setContentsMargins(12, 10, 12, 12);
    page->setSpacing(10);
    auto* heading = new QHBoxLayout;
    auto* brand = new QLabel("RA2  /  INI NODE STUDIO", host);
    brand->setObjectName("brand");
    heading->addWidget(brand);
    heading->addSpacing(18);
    fileInfo_ = new QLabel("未打开文件", host);
    fileInfo_->setObjectName("subtle");
    fileInfo_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    heading->addWidget(fileInfo_, 1);
    page->addLayout(heading);

    auto* splitter = new QSplitter(Qt::Horizontal, host);
    splitter->setHandleWidth(9);
    splitter->setChildrenCollapsible(false);
    page->addWidget(splitter, 1);
    setCentralWidget(host);

    auto* sidebar = new QWidget(splitter);
    sidebar->setObjectName("pane");
    sidebar->setAttribute(Qt::WA_StyledBackground, true);
    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(12, 14, 12, 12);
    sideLayout->setSpacing(9);
    sideLayout->addWidget(makeTitle("SECTION EXPLORER  /  节浏览器"));
    auto* search = new QLineEdit(sidebar);
    search->setPlaceholderText("搜索节名…   Ctrl+F");
    search->setClearButtonEnabled(true);
    sideLayout->addWidget(search);
    categoryFilter_ = new QComboBox(sidebar);
    categoryFilter_->setObjectName("categoryFilter");
    categoryFilter_->addItem("全部对象类型", "");
    for (const QString& category : ThemeStore::nodeKeys())
        categoryFilter_->addItem(category, category);
    sideLayout->addWidget(categoryFilter_);
    model_ = new QStandardItemModel(this);
    auto* categoryProxy = new CategoryProxy(this);
    proxy_ = categoryProxy;
    proxy_->setSourceModel(model_);
    proxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy_->setFilterKeyColumn(0);
    list_ = new QListView(sidebar);
    list_->setObjectName("sectionList");
    list_->setModel(proxy_);
    list_->setUniformItemSizes(true);
    list_->setAlternatingRowColors(true);
    list_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    list_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    sideLayout->addWidget(list_, 1);
    countInfo_ = new QLabel(sidebar);
    countInfo_->setObjectName("subtle");
    sideLayout->addWidget(countInfo_);
    auto* secBtns = new QHBoxLayout;
    auto* plus = new QPushButton("+ 新增", sidebar);
    auto* rename = new QPushButton("重命名", sidebar);
    auto* minus = new QPushButton("删除", sidebar);
    secBtns->addWidget(plus); secBtns->addWidget(rename); secBtns->addWidget(minus);
    sideLayout->addLayout(secBtns);
    connect(plus, &QPushButton::clicked, this, [this] { addSection(); });
    connect(minus, &QPushButton::clicked, this, [this] { deleteSection(); });
    connect(rename, &QPushButton::clicked, this, [this] { renameSection(); });
    auto* searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(120);
    connect(search, &QLineEdit::textChanged, searchTimer, QOverload<>::of(&QTimer::start));
    connect(searchTimer, &QTimer::timeout, this, [this, search] {
        proxy_->setFilterFixedString(search->text());
        updateCategoryCount();
        syncSelectionToFilter();
    });
    connect(categoryFilter_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        [this, categoryProxy](int) {
            categoryProxy->setCategory(categoryFilter_->currentData().toString());
            updateCategoryCount();
            syncSelectionToFilter();
        });
    add(view, "查找节", QKeySequence::Find, [search] { search->setFocus(); search->selectAll(); });
    connect(list_->selectionModel(), &QItemSelectionModel::currentChanged, this,
        [this](const QModelIndex& row) {
            if (!row.isValid()) return;
            const QModelIndex source = proxy_->mapToSource(row);
            const int id = source.data(kIndexRole).toInt();
            if (source.isValid() && id != current_) selectSection(id);
        });

    auto* center = new QWidget(splitter);
    auto* centerLayout = new QVBoxLayout(center);
    centerLayout->setContentsMargins(4, 5, 4, 5);
    centerLayout->setSpacing(9);
    auto* graphHeader = new QHBoxLayout;
    graphHeader->addWidget(makeTitle("RELATION GRAPH  /  节点关系"));
    graphHeader->addStretch();
    auto* fit = new QPushButton("适应画布", center);
    graphHeader->addWidget(fit);
    centerLayout->addLayout(graphHeader);

    auto* depthControls = new QHBoxLayout;
    depthControls->setSpacing(7);
    auto makeDepthSpin = [this, center, depthControls](const QString& text,
                                                      const QString& key,
                                                      const QString& name) {
        auto* label = new QLabel(text, center);
        depthControls->addWidget(label);
        auto* spin = new QSpinBox(center);
        spin->setObjectName(name);
        spin->setRange(0, 5);
        spin->setValue(std::clamp(settings_.value(key, 1).toInt(), 0, 5));
        spin->setToolTip("0 = 隐藏此方向；1 = 直接关系；2–5 = 逐层追溯/展开。"
                         "每层最多展示 14 个节点，每个方向最多 64 个。");
        spin->setFixedWidth(65);
        spin->setAlignment(Qt::AlignCenter);
        depthControls->addWidget(spin);
        return spin;
    };
    parentDepth_ = makeDepthSpin("父节点最大深度", "graph/parentDepth", "parentDepth");
    childDepth_ = makeDepthSpin("子节点最大深度", "graph/childDepth", "childDepth");
    depthControls->addStretch();
    centerLayout->addLayout(depthControls);
    views_ = new QTabWidget(center);
    views_->setObjectName("workspaceTabs");
    graph_ = new GraphView(views_);
    graph_->setObjectName("graphCanvas");
    graph_->setDepthLimits(parentDepth_->value(), childDepth_->value());
    const auto changeDepth = [this] {
        settings_.setValue("graph/parentDepth", parentDepth_->value());
        settings_.setValue("graph/childDepth", childDepth_->value());
        graph_->setDepthLimits(parentDepth_->value(), childDepth_->value());
        graph_->display(document_, current_);
    };
    connect(parentDepth_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [changeDepth](int) { changeDepth(); });
    connect(childDepth_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [changeDepth](int) { changeDepth(); });
    views_->addTab(graph_, "关系图");
    auto* sourcePage = new QWidget(views_);
    auto* sourceLayout = new QVBoxLayout(sourcePage);
    sourceLayout->setContentsMargins(9, 9, 9, 9);
    auto* sourceControls = new QHBoxLayout;
    sourceControls->addWidget(new QLabel("原始 INI 文本（保留注释及顺序）"));
    sourceControls->addStretch();
    auto* applySource = new QPushButton("应用源码修改", sourcePage);
    sourceControls->addWidget(applySource);
    sourceLayout->addLayout(sourceControls);
    source_ = new QPlainTextEdit(sourcePage);
    source_->setLineWrapMode(QPlainTextEdit::NoWrap);
    source_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    sourceLayout->addWidget(source_);
    views_->addTab(sourcePage, "源码");
    centerLayout->addWidget(views_, 1);
    auto* graphHint = new QLabel("滚轮缩放 · 拖动画布 · 双击节点跳转 · 每层最多 14 个、每侧累计最多 64 个节点");
    graphHint->setObjectName("subtle");
    graphHint->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(graphHint);
    connect(fit, &QPushButton::clicked, graph_, &GraphView::fitGraph);
    connect(views_, &QTabWidget::currentChanged, this, [this, previous = 0](int index) mutable {
        if (previous == 1 && index != 1) applySourceIfNeeded();
        if (index == 1) {
            QSignalBlocker block(source_);
            source_->setPlainText(document_.lines().join("\n"));
            source_->document()->setModified(false);
        }
        previous = index;
    });
    connect(source_, &QPlainTextEdit::textChanged, this, [this] { titleUpdate(); });
    connect(applySource, &QPushButton::clicked, this, [this] { applySourceIfNeeded(); });
    connect(graph_, &GraphView::sectionActivated, this, [this](int id) { selectSection(id); });

    auto* inspector = new QWidget(splitter);
    inspector->setObjectName("pane");
    inspector->setAttribute(Qt::WA_StyledBackground, true);
    auto* inspectorLayout = new QVBoxLayout(inspector);
    inspectorLayout->setContentsMargins(12, 14, 12, 12);
    inspectorLayout->setSpacing(9);
    inspectorLayout->addWidget(makeTitle("INSPECTOR  /  属性检查器"));
    sectionInfo_ = new QLabel("选择一个节查看属性", inspector);
    sectionInfo_->setObjectName("inspectorTitle");
    sectionInfo_->setWordWrap(true);
    inspectorLayout->addWidget(sectionInfo_);
    properties_ = new QTableWidget(inspector);
    properties_->setColumnCount(2);
    properties_->setHorizontalHeaderLabels({"键 / KEY", "值 / VALUE"});
    properties_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    properties_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    properties_->verticalHeader()->setVisible(false);
    properties_->setAlternatingRowColors(true);
    properties_->setSelectionBehavior(QAbstractItemView::SelectRows);
    properties_->setSelectionMode(QAbstractItemView::SingleSelection);
    properties_->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    inspectorLayout->addWidget(properties_, 1);
    auto* propertyButtons = new QHBoxLayout;
    auto* addProp = new QPushButton("+ 新增属性", inspector);
    auto* removeProp = new QPushButton("删除属性", inspector);
    propertyButtons->addWidget(addProp); propertyButtons->addWidget(removeProp);
    inspectorLayout->addLayout(propertyButtons);
    auto* hint = new QLabel("双击表格编辑；更改可以撤销、保存。", inspector);
    hint->setObjectName("subtle");
    inspectorLayout->addWidget(hint);
    connect(addProp, &QPushButton::clicked, this, [this] { addKey(); });
    connect(removeProp, &QPushButton::clicked, this, [this] { deleteKey(); });
    connect(properties_, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        if (current_ < 0 || !item) return;
        const QString oldKey = properties_->item(item->row(), 0)->data(Qt::UserRole).toString();
        const QString newValue = item->text();
        if (item->column() == 0)
            mutation("重命名属性 " + oldKey, [this, oldKey, newValue] {
                return document_.renameKey(current_, oldKey, newValue);
            });
        else
            mutation("编辑属性 " + oldKey, [this, oldKey, newValue] {
                return document_.setKey(current_, oldKey, newValue);
            });
    });

    splitter->addWidget(sidebar);
    splitter->addWidget(center);
    splitter->addWidget(inspector);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);
    splitter->setSizes({270, 810, 370});
}

QHash<QString, QColor> MainWindow::colors() const {
    return themes_.byId(theme_->currentData().toString()).nodes;
}

void MainWindow::reloadThemeChoices(const QString& requested) {
    const QString previous = requested.isEmpty()
        ? (theme_->currentIndex() >= 0 ? theme_->currentData().toString() : QString())
        : requested;
    QSignalBlocker block(theme_);
    theme_->clear();
    for (const auto& t : themes_.all()) theme_->addItem(t.name, t.id);
    int index = theme_->findData(previous);
    if (index < 0) index = settings_.value("theme", 0).toInt();
    if (index < 0 || index >= theme_->count()) index = 0;
    theme_->setCurrentIndex(index);
    settings_.setValue("themes/selected", theme_->currentData().toString());
    applyTheme();
}

void MainWindow::updateCategoryCount() {
    if (countInfo_) countInfo_->setText(
        QString("当前显示 %1 / %2 个节").arg(proxy_->rowCount()).arg(model_->rowCount()));
}

// Keep graph, inspector, and list synchronized when a filter hides the current section.
void MainWindow::syncSelectionToFilter() {
    if (!list_ || !model_ || !proxy_) return;
    const QModelIndex currentSource = current_ < 0 ? QModelIndex() : model_->index(current_, 0);
    const QModelIndex currentVisible = proxy_->mapFromSource(currentSource);
    if (currentVisible.isValid()) {
        if (list_->currentIndex() != currentVisible) {
            QSignalBlocker block(list_->selectionModel());
            list_->setCurrentIndex(currentVisible);
        }
        return;
    }
    const QModelIndex first = proxy_->index(0, 0);
    const QModelIndex source = first.isValid() ? proxy_->mapToSource(first) : QModelIndex();
    const int replacement = source.isValid() ? source.data(kIndexRole).toInt() : -1;
    if (replacement != current_) selectSection(replacement);
    else if (replacement < 0) {
        QSignalBlocker block(list_->selectionModel());
        list_->selectionModel()->clear();
    }
}

void MainWindow::applyTheme() {
    if (!theme_ || !graph_ || theme_->currentIndex() < 0) return;
    const ThemeProfile current = themes_.byId(theme_->currentData().toString());
    const auto v = [&current](const QString& key) { return current.ui.value(key).name(); };
    const QString accentText = current.ui.value("accent").lightness() > 145 ? "#141923" : "#ffffff";
    // VS-style flat square-edged visual language. A single QSS controls all widgets,
    // including dialogs and scroll-area viewports, instead of mixing native rounding.
    const QString css = QString(R"(
      QWidget { color:%4; font-family:'Segoe UI','Microsoft YaHei UI',sans-serif; font-size:13px; }
      QMainWindow, QWidget#centralHost, QDialog, QMessageBox { background:%1; }
      QMenuBar, QToolBar, QStatusBar { background:%2; color:%4; }
      QWidget#pane { background:%2; border:1px solid %6; border-radius:0px; }
      QLabel#brand { color:%7; font-weight:700; font-size:17px; }
      QLabel#subtle { color:%5; font-size:11px; }
      QLabel#sectionTitle { color:%5; font-size:11px; font-weight:700; }
      QLabel#inspectorTitle { color:%4; font-size:16px; font-weight:700; padding:3px 0; }
      QLineEdit, QComboBox, QSpinBox, QListView, QTableWidget, QPlainTextEdit {
        background:%3; color:%4; border:1px solid %6; border-radius:0px;
        selection-background-color:%7; selection-color:%8;
      }
      QLineEdit, QComboBox, QSpinBox { padding:5px 7px; min-height:24px; }
      QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QPlainTextEdit:focus,
      QListView:focus, QTableWidget:focus { border:1px solid %7; }
      QListView { outline:0; alternate-background-color:%2; }
      QListView::item { min-height:26px; padding-left:8px; border-radius:0px; }
      QListView::item:hover { background:%2; }
      QListView::item:selected, QTableWidget::item:selected { background:%7; color:%8; }
      QTableWidget { gridline-color:%6; alternate-background-color:%2; }
      QHeaderView { background:%2; }
      QHeaderView::section { background:%2; color:%4; border:0; border-right:1px solid %6;
        border-bottom:1px solid %6; padding:6px; }
      QPushButton, QToolButton {
        background:%3; color:%4; border:1px solid %6; border-radius:0px;
        padding:5px 12px; min-height:23px; font-weight:400;
      }
      QPushButton:hover, QToolButton:hover { background:%7; color:%8; border-color:%7; }
      QPushButton:pressed, QToolButton:pressed { background:%6; color:%4; }
      QPushButton:disabled, QToolButton:disabled { color:%5; background:%2; }
      QPushButton:focus, QToolButton:focus { border:1px solid %7; }
      QComboBox::drop-down { border-left:1px solid %6; width:22px; background:%2; }
      QSpinBox::up-button, QSpinBox::down-button { background:%2; border-left:1px solid %6; width:17px; }
      QSpinBox::up-button:hover, QSpinBox::down-button:hover { background:%7; }
      QComboBox QAbstractItemView { background:%2; color:%4; border:1px solid %6;
        border-radius:0px; selection-background-color:%7; selection-color:%8; }
      QTabWidget::pane { background:%2; border:1px solid %6; border-radius:0px; }
      QTabBar::tab { background:%2; color:%5; border:1px solid %6; border-radius:0px;
        border-bottom:0; padding:7px 18px; margin-right:0px; }
      QTabBar::tab:selected { background:%3; color:%4; border-top:2px solid %7; padding-top:6px; }
      QTabBar::tab:hover:!selected { background:%3; }
      QToolBar { border:0; border-bottom:1px solid %6; spacing:5px; padding:4px 7px; }
      QToolBar::separator { background:%6; width:1px; margin:4px; }
      QMenu { background:%2; color:%4; border:1px solid %6; border-radius:0px; padding:3px; }
      QMenu::item { padding:6px 25px; border-radius:0px; }
      QMenu::item:selected { background:%7; color:%8; }
      QMenuBar::item { background:transparent; padding:5px 10px; border-radius:0px; }
      QMenuBar::item:selected { background:%3; }
      QScrollBar:vertical { background:%2; width:12px; border:0; }
      QScrollBar::handle:vertical { background:%6; min-height:26px; border-radius:0px; }
      QScrollBar:horizontal { background:%2; height:12px; border:0; }
      QScrollBar::handle:horizontal { background:%6; min-width:26px; border-radius:0px; }
      QScrollBar::add-line, QScrollBar::sub-line { width:0px; height:0px; }
      QSplitter::handle { background:%6; }
      QGroupBox { background:%2; border:1px solid %6; border-radius:0px;
        margin-top:14px; padding-top:8px; font-weight:600; }
      QGroupBox::title { subcontrol-origin:margin; left:9px; color:%4; padding:0 5px; }
      QScrollArea, QScrollArea > QWidget, QScrollArea > QWidget > QWidget {
        background:%1; border:0; border-radius:0px;
      }
      QDialogButtonBox { background:%1; }
      QToolTip { background:%2; color:%4; border:1px solid %6; }
    )").arg(v("background"), v("panel"), v("surface"), v("text"),
           v("muted"), v("border"), v("accent"), accentText);
    qApp->setStyleSheet(css);
    // Theme changes recolor nodes without jumping the user's graph viewport.
    const bool hasGraph = current_ >= 0 && current_ < document_.sections().size();
    const QTransform transform = graph_->transform();
    const QPointF center = graph_->mapToScene(graph_->viewport()->rect().center());
    graph_->setColors(current.nodes, current.ui.value("canvas"),
                      current.ui.value("text"), current.ui.value("panel"));
    graph_->display(document_, current_);
    if (hasGraph) {
        graph_->setTransform(transform);
        graph_->centerOn(center);
    }
    if (model_) {
        for (int i = 0; i < document_.sections().size(); ++i) {
            auto* item = model_->item(i);
            if (item) item->setForeground(current.nodes.value(document_.category(i)));
        }
    }
}

void MainWindow::editTheme() {
    if (theme_->currentIndex() < 0) return;
    ThemeProfile draft = themes_.byId(theme_->currentData().toString());
    const bool editingCustom = !draft.builtIn;
    QDialog dialog(this);
    dialog.setWindowTitle(editingCustom ? "编辑自定义主题" : "从预设创建自定义主题");
    dialog.resize(520, 690);
    dialog.setMinimumSize(440, 460);
    auto* root = new QVBoxLayout(&dialog);
    root->setContentsMargins(14, 12, 14, 12);
    root->setSpacing(10);
    auto* name = new QLineEdit(editingCustom ? draft.name : draft.name + " 自定义", &dialog);
    auto* description = new QLabel("一套主题同时控制界面色板与所有节点类型颜色。预设不可覆盖，编辑会生成独立副本。");
    description->setWordWrap(true);
    root->addWidget(description);
    auto* header = new QFormLayout;
    header->addRow("主题名称", name);
    root->addLayout(header);

    auto* scroll = new QScrollArea(&dialog);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* content = new QWidget(scroll);
    auto* palette = new QVBoxLayout(content);
    palette->setContentsMargins(2, 5, 9, 5);
    palette->setSpacing(10);
    auto addColorSection = [&](const QString& heading, const QStringList& keys,
                               QHash<QString, QColor>* target, const QHash<QString, QString>& labels) {
        auto* group = new QGroupBox(heading, content);
        auto* form = new QFormLayout(group);
        form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        form->setFormAlignment(Qt::AlignTop);
        form->setHorizontalSpacing(16);
        form->setVerticalSpacing(5);
        for (const QString& key : keys) {
            auto* control = new QWidget(group);
            auto* row = new QHBoxLayout(control);
            row->setContentsMargins(0, 1, 0, 1);
            row->setSpacing(7);
            auto* swatch = new QFrame(control);
            swatch->setFixedSize(34, 25);
            swatch->setFrameShape(QFrame::Box);
            auto* input = new QLineEdit(control);
            input->setMaxLength(7);
            input->setFixedWidth(104);
            input->setToolTip("直接输入 #RRGGBB，或点击右侧按钮选择颜色");
            auto* pick = new QPushButton("选择…", control);
            pick->setMinimumWidth(72);
            auto redraw = [target, key, swatch, input] {
                const QColor color = target->value(key);
                swatch->setStyleSheet(QString("QFrame{background:%1;border:1px solid #858c96;"
                                               "border-radius:0px;}").arg(color.name()));
                input->setText(color.name(QColor::HexRgb).toUpper());
            };
            redraw();
            connect(input, &QLineEdit::editingFinished, &dialog,
                    [input, target, key, redraw] {
                const QString hex = input->text().trimmed();
                static const QRegularExpression format("^#[0-9a-fA-F]{6}$");
                if (format.match(hex).hasMatch()) {
                    (*target)[key] = QColor(hex);
                } else {
                    input->setToolTip("无效的颜色格式，应为 #RRGGBB");
                }
                redraw();
            });
            connect(pick, &QPushButton::clicked, &dialog, [&, key, target, redraw] {
                const QColor selected = QColorDialog::getColor(
                    target->value(key), &dialog, "选择 " + labels.value(key, key));
                if (selected.isValid()) {
                    (*target)[key] = selected;
                    redraw();
                }
            });
            row->addWidget(swatch);
            row->addWidget(input);
            row->addWidget(pick);
            row->addStretch(1);
            form->addRow(labels.value(key, key), control);
        }
        palette->addWidget(group);
    };
    const QHash<QString, QString> uiLabels{
        {"background","主窗口背景"}, {"panel","侧栏 / 检查器底色"},
        {"surface","控件和输入框底色"}, {"text","主要文字"}, {"muted","次要文字"},
        {"border","边框"}, {"accent","强调色 / 选中"}, {"canvas","节点画布背景"}
    };
    const QHash<QString, QString> categoryLabels{
        {"Infantry","步兵"}, {"Vehicle","载具"}, {"Aircraft","航空器"},
        {"Building","建筑"}, {"Weapon","武器"}, {"Warhead","弹头"},
        {"Projectile","弹体"}, {"Animation","动画"}, {"Superweapon","超级武器"},
        {"Registry","类型索引"}, {"Other","其他"}
    };
    addColorSection("界面 / UI COLORS", ThemeStore::uiKeys(), &draft.ui, uiLabels);
    addColorSection("节点分类 / NODE COLORS", ThemeStore::nodeKeys(), &draft.nodes, categoryLabels);
    palette->addStretch();
    scroll->setWidget(content);
    root->addWidget(scroll, 1);
    auto* actions = new QDialogButtonBox(&dialog);
    actions->addButton("保存主题", QDialogButtonBox::AcceptRole);
    actions->addButton("取消", QDialogButtonBox::RejectRole);
    connect(actions, &QDialogButtonBox::accepted, &dialog, [&] {
        if (name->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "无效名称", "主题名称不能为空");
            return;
        }
        dialog.accept();
    });
    connect(actions, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    root->addWidget(actions);
    // Screenshot probe used by the offscreen regression job; normal users never see it.
    if (qEnvironmentVariableIsSet("RA2_THEME_PREVIEW_PATH")) {
        const QString imagePath = qEnvironmentVariable("RA2_THEME_PREVIEW_PATH");
        QTimer::singleShot(0, &dialog, [&dialog, imagePath] {
            dialog.show();
            qApp->processEvents();
            if (!dialog.grab().save(imagePath)) qWarning("Could not capture theme editor preview");
            dialog.reject();
        });
    }
    if (dialog.exec() != QDialog::Accepted) return;
    draft.name = name->text().trimmed();
    if (!editingCustom) draft.id.clear();
    const QString id = draft.id;
    if (!themes_.saveCustom(draft)) {
        QMessageBox::warning(this, "保存失败", "主题无法保存"); return;
    }
    reloadThemeChoices(id.isEmpty() ? themes_.all().last().id : id);
}

void MainWindow::importTheme() {
    const QString path = QFileDialog::getOpenFileName(this, "导入主题", QDir::homePath(), "JSON theme (*.json)");
    if (path.isEmpty()) return;
    QString id, error;
    if (!themes_.importFile(path, &id, &error)) {
        QMessageBox::warning(this, "导入失败", error);
        return;
    }
    reloadThemeChoices(id);
}
void MainWindow::exportTheme() {
    const QString path = QFileDialog::getSaveFileName(this, "导出主题",
        QDir::homePath() + "/ra2-node-theme.json", "JSON theme (*.json)");
    if (path.isEmpty()) return;
    QString error;
    if (!themes_.exportFile(theme_->currentData().toString(), path, &error))
        QMessageBox::warning(this, "导出失败", error);
}
void MainWindow::deleteTheme() {
    const QString id = theme_->currentData().toString();
    if (themes_.byId(id).builtIn) {
        QMessageBox::information(this, "内置主题", "内置主题不能删除，编辑它会自动生成自定义副本。");
        return;
    }
    if (QMessageBox::question(this, "删除主题", "仅删除这套自定义配色，不会影响 INI 文件，确定吗？")
        != QMessageBox::Yes) return;
    if (themes_.deleteCustom(id)) reloadThemeChoices("night");
}

void MainWindow::refreshList(const QString& focus) {
    QString desired = focus;
    if (desired.isEmpty() && current_ >= 0 && current_ < document_.sections().size())
        desired = document_.sections()[current_].name;
    // Allow model reset/data signals to reach the proxy and QListView.
    model_->clear();
    const auto palette = colors();
    for (int i = 0; i < document_.sections().size(); ++i) {
        const auto& section = document_.sections()[i];
        auto* item = new QStandardItem(section.name);
        item->setData(i, kIndexRole);
        item->setData(document_.category(i), kCategoryRole);
        item->setForeground(palette.value(document_.category(i), QColor("#9aa5b3")));
        item->setToolTip(document_.category(i) + " · " + QString::number(section.entries.size()) + " entries");
        model_->appendRow(item);
    }
    updateCategoryCount();
    current_ = document_.indexOf(desired);
    if (current_ < 0 && !document_.sections().isEmpty()) current_ = 0;
    syncSelectionToFilter();
    // If the visible selection did not change, refresh the property inspector anyway
    // because the underlying document contents may have changed.
    refreshDetails();
    titleUpdate();
}

void MainWindow::selectSection(int section) {
    current_ = section >= 0 && section < document_.sections().size() ? section : -1;
    if (current_ >= 0) {
        const QModelIndex original = model_->index(current_, 0);
        const QModelIndex visible = proxy_->mapFromSource(original);
        if (visible.isValid()) {
            QSignalBlocker blocker(list_);
            list_->setCurrentIndex(visible);
            list_->scrollTo(visible);
        }
    } else {
        QSignalBlocker block(list_->selectionModel());
        list_->selectionModel()->clear();
    }
    refreshDetails();
}
void MainWindow::refreshDetails() {
    if (!properties_ || !graph_) return;
    QSignalBlocker block(properties_);
    properties_->setRowCount(0);
    if (current_ >= 0 && current_ < document_.sections().size()) {
        const auto& entries = document_.sections()[current_].entries;
        properties_->setRowCount(entries.size());
        for (int i = 0; i < entries.size(); ++i) {
            auto* key = new QTableWidgetItem(entries[i].key);
            key->setData(Qt::UserRole, entries[i].key);
            auto* val = new QTableWidgetItem(entries[i].value);
            properties_->setItem(i, 0, key);
            properties_->setItem(i, 1, val);
        }
        properties_->resizeColumnToContents(0);
    }
    if (sectionInfo_) sectionInfo_->setText(current_ < 0 ? "选择一个节查看属性" :
        document_.sections()[current_].name + "  ·  " + document_.category(current_));
    graph_->display(document_, current_);
    int count = 0;
    for (const auto& s : document_.sections()) count += s.entries.size();
    statusBar()->showMessage(QString("%1 sections · %2 properties · %3")
        .arg(document_.sections().size()).arg(count)
        .arg(current_ < 0 ? "No selection" : document_.sections()[current_].name));
}
void MainWindow::titleUpdate() {
    if (fileInfo_) fileInfo_->setText(document_.path().isEmpty() ? "未打开文件" : document_.path());
    setWindowTitle(QString("RA2 Node Studio — %1%2")
        .arg(document_.path().isEmpty() ? "Untitled" : document_.path())
        .arg(history_.isClean() && !(source_ && source_->document()->isModified()) ? "" : " *"));
}
bool MainWindow::applySourceIfNeeded() {
    if (!source_ || !source_->document()->isModified()) return true;
    const QString text = source_->toPlainText();
    const QStringList next = text.isEmpty() ? QStringList{} :
        text.split(QRegularExpression("\r\n|\n|\r"), Qt::KeepEmptyParts);
    if (next == document_.lines()) {
        source_->document()->setModified(false);
        return true;
    }
    const QString oldName = current_ >= 0 && current_ < document_.sections().size()
        ? document_.sections()[current_].name : QString();
    const QStringList before = document_.lines();
    document_.setLines(next);
    const QStringList after = document_.lines();
    history_.push(new HistoryEntry(&document_, before, after,
        [this, oldName] { refreshList(oldName); }, "编辑原始文本"));
    source_->document()->setModified(false);
    refreshList(oldName);
    return true;
}
bool MainWindow::maybeSave() {
    if (views_->currentIndex() == 1) applySourceIfNeeded();
    if (history_.isClean()) return true;
    const auto result = QMessageBox::warning(this, "未保存更改",
        "当前文件存在未保存的修改，是否保存？",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);
    if (result == QMessageBox::Cancel) return false;
    if (result == QMessageBox::Save) return save();
    return true;
}
void MainWindow::openPath(const QString& path) {
    if (!maybeSave()) return;
    QString error;
    const auto selectedEncoding = encoding_->currentIndex() == 1 ?
        IniDocument::Encoding::System : IniDocument::Encoding::Utf8;
    IniDocument next;
    if (!next.load(path, selectedEncoding, &error)) {
        QMessageBox::critical(this, "读取失败", error);
        return;
    }
    document_ = std::move(next);
    history_.clear(); history_.setClean(); current_ = -1;
    refreshList();
    settings_.setValue("recentFile", path);
}
bool MainWindow::save(bool as) {
    if (views_->currentIndex() == 1) applySourceIfNeeded();
    QString output = document_.path();
    if (as || output.isEmpty()) {
        output = QFileDialog::getSaveFileName(this, "保存 INI", output.isEmpty() ?
            QDir::homePath() + "/rules.ini" : output,
            "INI files (*.ini);;All files (*)");
    }
    if (output.isEmpty()) return false;
    QString error;
    if (!document_.save(output, &error)) {
        QMessageBox::critical(this, "保存失败", error);
        return false;
    }
    history_.setClean(); titleUpdate();
    statusBar()->showMessage("已安全保存: " + output, 4500);
    return true;
}
void MainWindow::mutation(const QString& label, const std::function<bool()>& action, const QString& focus) {
    if (views_->currentIndex() == 1) applySourceIfNeeded();
    const QString oldName = current_ >= 0 && current_ < document_.sections().size()
        ? document_.sections()[current_].name : QString();
    const QStringList before = document_.lines();
    if (!action()) {
        QMessageBox::warning(this, "操作未完成", "名称冲突、目标不存在或名称包含无效字符。");
        refreshDetails();
        return;
    }
    const QStringList after = document_.lines();
    if (before == after) return;
    const QString preferred = focus.isEmpty() ? oldName : focus;
    history_.push(new HistoryEntry(&document_, before, after,
        [this, preferred] { refreshList(preferred); }, label));
}
void MainWindow::addSection() {
    bool ok = false;
    QString name = QInputDialog::getText(this, "新建节", "节名（不含方括号）", QLineEdit::Normal, "", &ok);
    if (ok && !name.trimmed().isEmpty())
        mutation("新增节 " + name, [this, name] { return document_.addSection(name); }, name.trimmed());
}
void MainWindow::renameSection() {
    if (current_ < 0) return;
    const QString old = document_.sections()[current_].name;
    bool ok = false;
    const QString name = QInputDialog::getText(this, "重命名节", "新名称", QLineEdit::Normal, old, &ok);
    if (ok && name != old)
        mutation("重命名节 " + old, [this, name] { return document_.renameSection(current_, name); }, name.trimmed());
}
void MainWindow::deleteSection() {
    if (current_ < 0) return;
    const QString name = document_.sections()[current_].name;
    if (QMessageBox::question(this, "确认删除", "删除整个节 [" + name + "] 及其中的属性？") != QMessageBox::Yes) return;
    mutation("删除节 " + name, [this] { return document_.deleteSection(current_); });
}
void MainWindow::addKey() {
    if (current_ < 0) { QMessageBox::information(this, "提示", "请先选中一个节"); return; }
    bool ok = false;
    const QString key = QInputDialog::getText(this, "新增属性", "键名", QLineEdit::Normal, "", &ok);
    if (!ok || key.trimmed().isEmpty()) return;
    // Avoid using the upsert operation to silently overwrite an existing value.
    for (const auto& entry : document_.sections()[current_].entries)
        if (entry.key.compare(key.trimmed(), Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "属性已存在", "请选择不同的键名"); return;
        }
    const QString value = QInputDialog::getText(this, "属性值", key + " =", QLineEdit::Normal, "", &ok);
    if (ok) mutation("新增属性 " + key, [this, key, value] { return document_.setKey(current_, key, value); });
}
void MainWindow::deleteKey() {
    if (current_ < 0 || properties_->currentRow() < 0) return;
    const QString key = properties_->item(properties_->currentRow(), 0)->data(Qt::UserRole).toString();
    mutation("删除属性 " + key, [this, key] { return document_.removeKey(current_, key); });
}
void MainWindow::setCategoryColor() {
    if (current_ < 0) return;
    const QString category = document_.category(current_);
    ThemeProfile draft = themes_.byId(theme_->currentData().toString());
    const QColor chosen = QColorDialog::getColor(
        draft.nodes.value(category), this, "配置当前主题的 " + category + " 节点颜色");
    if (!chosen.isValid()) return;
    draft.nodes.insert(category, chosen);
    if (draft.builtIn) {
        draft.name += " 自定义";
        draft.id.clear();
    }
    const QString id = draft.id;
    if (themes_.saveCustom(draft))
        reloadThemeChoices(id.isEmpty() ? themes_.all().last().id : id);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        settings_.setValue("window/geometry", saveGeometry());
        event->accept();
    } else event->ignore();
}


bool MainWindow::smokeTestUi() {
    auto check = [](bool good, const char* stage) {
        if (!good) qWarning("UI smoke failed at: %s", stage);
        return good;
    };
    QTemporaryDir temp;
    if (!check(temp.isValid(), "temporary directory")) return false;
    const QString path = temp.filePath("selection-smoke.ini");
    QFile sample(path);
    if (!check(sample.open(QIODevice::WriteOnly), "create sample file")) return false;
    if (!check(sample.write("[Unit]\nPrimary=W1\n[W1]\nDamage=5\nROF=20\n") > 0, "write sample")) return false;
    sample.close();
    openPath(path);
    if (!check(document_.sections().size() == 2, "parse two sections")) return false;
    if (!check(current_ >= 0, "initial current selection")) return false;
    if (!check(properties_->rowCount() == 1, "initial Unit has one property")) return false;
    const int emptyFilter = categoryFilter_->findData("Projectile");
    if (!check(emptyFilter >= 0, "Projectile selector exists")) return false;
    categoryFilter_->setCurrentIndex(emptyFilter);
    if (!check(proxy_->rowCount() == 0, "Projectile filter empty")) return false;
    if (!check(current_ == -1, "hidden selection cleared")) return false;
    if (!check(properties_->rowCount() == 0, "inspector cleared when filter empty")) return false;
    if (!check(sectionInfo_->text().contains("选择一个节"), "inspector title cleared")) return false;
    const int weaponFilter = categoryFilter_->findData("Weapon");
    if (!check(weaponFilter >= 0, "Weapon selector exists")) return false;
    categoryFilter_->setCurrentIndex(weaponFilter);
    if (!check(proxy_->rowCount() == 1, "Weapon filter one match")) return false;
    if (!check(current_ == document_.indexOf("W1"), "visible Weapon selected")) return false;
    if (!check(properties_->rowCount() == 2, "Weapon two properties")) return false;
    const int light = theme_->findData("light");
    if (!check(light >= 0, "light theme exists")) return false;
    theme_->setCurrentIndex(light);
    if (!check(qApp->styleSheet().contains("border-radius:0px"), "square style applied")) return false;
    if (!check(!qApp->styleSheet().contains("border-radius:12px"), "no rounded style rules")) return false;
    // Verify controls trigger actual BFS model rebuild, separately per direction.
    categoryFilter_->setCurrentIndex(0);
    document_.setLines({"[Grand]", "ref=Unit", "[Unit]", "Primary=W1",
                        "[W1]", "Damage=5", "ROF=20", "Next=End",
                        "[End]", "Value=2"});
    refreshList("W1");
    if (!check(current_ == document_.indexOf("W1"), "depth test selection")) return false;
    parentDepth_->setValue(2);
    childDepth_->setValue(1);
    const auto& expanded = graph_->visibleNeighborhood();
    if (!check(expanded.parents.size() == 2 && expanded.children.size() == 1,
               "independent parent 2 child 1 expansion")) return false;
    parentDepth_->setValue(0);
    if (!check(graph_->visibleNeighborhood().parents.isEmpty()
               && graph_->visibleNeighborhood().children.size() == 1,
               "zero parent hides parents, preserves children")) return false;
    childDepth_->setValue(0);
    if (!check(graph_->visibleNeighborhood().children.isEmpty(), "zero child hides children")) return false;
    parentDepth_->setValue(1);
    childDepth_->setValue(1);
    if (qEnvironmentVariableIsSet("RA2_THEME_PREVIEW_PATH")) {
        editTheme();
        if (!check(QFile::exists(qEnvironmentVariable("RA2_THEME_PREVIEW_PATH")),
                   "theme dialog preview rendered")) return false;
    }
    return true;
}
