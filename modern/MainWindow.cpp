#include "MainWindow.h"
#include <QAction>
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
    theme_->setCurrentIndex(settings_.value("theme", 0).toInt());
    encoding_->setCurrentIndex(settings_.value("encoding", 0).toInt());
    applyTheme();
    connect(&history_, &QUndoStack::cleanChanged, this, [this] { titleUpdate(); });
    refreshList();
    statusBar()->showMessage("Ready · Open a rules.ini / art.ini to begin");
}

void MainWindow::createUi() {
    auto* file = menuBar()->addMenu("文件(&F)");
    auto* edit = menuBar()->addMenu("编辑(&E)");
    auto* view = menuBar()->addMenu("视图(&V)");
    auto* help = menuBar()->addMenu("帮助(&H)");
    auto* bar = addToolBar("快捷操作");
    bar->setMovable(false);

    auto add = [&](QMenu* menu, const QString& name, const QKeySequence& shortcut, auto fn) {
        QAction* action = menu->addAction(name);
        if (!shortcut.isEmpty()) action->setShortcut(shortcut);
        connect(action, &QAction::triggered, this, fn);
        return action;
    };
    QAction* newFile = add(file, "新建", QKeySequence::New, [this] {
        if (!maybeSave()) return;
        document_.clear(); history_.clear(); history_.setClean(); current_ = -1;
        refreshList(); titleUpdate();
    });
    QAction* openFile = add(file, "打开 INI...", QKeySequence::Open, [this] {
        const QString p = QFileDialog::getOpenFileName(this, "打开 INI", QDir::homePath(),
                                                       "INI files (*.ini *.map *.mpr);;All files (*)");
        if (!p.isEmpty()) openPath(p);
    });
    QAction* saveFile = add(file, "保存", QKeySequence::Save, [this] { save(); });
    QAction* saveAs = add(file, "另存为...", QKeySequence::SaveAs, [this] { save(true); });
    file->addSeparator();
    add(file, "退出", QKeySequence::Quit, [this] { close(); });
    bar->addAction(newFile); bar->addAction(openFile); bar->addAction(saveFile);
    bar->addSeparator();
    QAction* undo = history_.createUndoAction(this, "撤销");
    undo->setShortcut(QKeySequence::Undo);
    QAction* redo = history_.createRedoAction(this, "重做");
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
    add(view, "自定义当前节点类型颜色...", {}, [this] { setCategoryColor(); });
    add(help, "关于", {}, [this] {
        QMessageBox::about(this, "RA2 Node Studio",
            "基于 Qt 6 的红色警戒 2 INI 节点编辑器。\n\n"
            "在左侧选节，中央查看依赖关系，底部编辑键值；双击图中的节点可跳转。\n"
            "不同颜色代表不同对象类型。分类基于规则列表与字段特征推断，可能需要人工确认。\n\n"
            "旧版 Qt / ImGui 代码作为历史资料保留在仓库。");
    });

    bar->addSeparator();
    bar->addWidget(new QLabel(" 主题 "));
    theme_ = new QComboBox(this);
    theme_->addItems({"深空暗夜", "晨光浅色", "海洋青蓝", "战场红黑"});
    theme_->setToolTip("切换完整界面配色和节点默认颜色");
    bar->addWidget(theme_);
    bar->addWidget(new QLabel("   编码 "));
    encoding_ = new QComboBox(this);
    encoding_->addItems({"UTF-8", "系统本地编码 (ANSI)"});
    encoding_->setToolTip("打开文件时采用的编码；简体中文 Windows 通常可用 ANSI/GBK");
    bar->addWidget(encoding_);
    connect(theme_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int i) {
        settings_.setValue("theme", i);
        applyTheme();
    });
    connect(encoding_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int i) {
        settings_.setValue("encoding", i);
    });

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    setCentralWidget(splitter);

    auto* sidebar = new QWidget(splitter);
    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(12, 14, 12, 12);
    sideLayout->addWidget(makeTitle("节浏览器  /  Sections"));
    auto* search = new QLineEdit(sidebar);
    search->setPlaceholderText("搜索节名…   Ctrl+F");
    search->setClearButtonEnabled(true);
    sideLayout->addWidget(search);
    model_ = new QStandardItemModel(this);
    proxy_ = new QSortFilterProxyModel(this);
    proxy_->setSourceModel(model_);
    proxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy_->setFilterKeyColumn(0);
    list_ = new QListView(sidebar);
    list_->setModel(proxy_);
    list_->setUniformItemSizes(true);
    list_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    sideLayout->addWidget(list_, 1);
    auto* secBtns = new QHBoxLayout();
    auto* plus = new QPushButton("+ 节", sidebar);
    auto* minus = new QPushButton("− 节", sidebar);
    auto* rename = new QPushButton("重命名", sidebar);
    secBtns->addWidget(plus); secBtns->addWidget(minus); secBtns->addWidget(rename);
    sideLayout->addLayout(secBtns);
    connect(plus, &QPushButton::clicked, this, [this] { addSection(); });
    connect(minus, &QPushButton::clicked, this, [this] { deleteSection(); });
    connect(rename, &QPushButton::clicked, this, [this] { renameSection(); });
    auto* searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(130);
    connect(search, &QLineEdit::textChanged, searchTimer, QOverload<>::of(&QTimer::start));
    connect(searchTimer, &QTimer::timeout, this, [this, search] {
        proxy_->setFilterFixedString(search->text());
    });
    add(view, "查找节", QKeySequence::Find, [search] { search->setFocus(); search->selectAll(); });
    connect(list_, &QListView::clicked, this, [this](const QModelIndex& row) {
        const QModelIndex source = proxy_->mapToSource(row);
        if (source.isValid()) selectSection(source.data(Qt::UserRole + 1).toInt());
    });

    auto* vertical = new QSplitter(Qt::Vertical, splitter);
    vertical->setChildrenCollapsible(false);
    views_ = new QTabWidget(vertical);
    graph_ = new GraphView(views_);
    views_->addTab(graph_, " 关系图 · Graph ");
    auto* sourcePage = new QWidget(views_);
    auto* sourceLayout = new QVBoxLayout(sourcePage);
    sourceLayout->setContentsMargins(8, 8, 8, 8);
    auto* sourceControls = new QHBoxLayout();
    sourceControls->addWidget(new QLabel("原始 INI 文本（保留注释和原始节顺序）"));
    sourceControls->addStretch();
    auto* applySource = new QPushButton("应用源码修改", sourcePage);
    sourceControls->addWidget(applySource);
    sourceLayout->addLayout(sourceControls);
    source_ = new QPlainTextEdit(sourcePage);
    source_->setLineWrapMode(QPlainTextEdit::NoWrap);
    source_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    sourceLayout->addWidget(source_);
    views_->addTab(sourcePage, " 源码 · Source ");
    connect(views_, &QTabWidget::currentChanged, this, [this, previous = 0](int index) mutable {
        if (previous == 1 && index != 1) applySourceIfNeeded();
        if (index == 1) {
            QSignalBlocker block(source_);
            source_->setPlainText(document_.lines().join("\n"));
            source_->document()->setModified(false);
        }
        previous = index;
    });
    connect(applySource, &QPushButton::clicked, this, [this] { applySourceIfNeeded(); });
    connect(graph_, &GraphView::sectionActivated, this, [this](int id) { selectSection(id); });

    auto* editorPanel = new QWidget(vertical);
    auto* editorLayout = new QVBoxLayout(editorPanel);
    editorLayout->setContentsMargins(9, 8, 9, 8);
    auto* propsHead = new QHBoxLayout();
    propsHead->addWidget(makeTitle("属性编辑器  /  Properties"));
    propsHead->addStretch();
    auto* addProp = new QPushButton("+ 属性");
    auto* removeProp = new QPushButton("− 属性");
    propsHead->addWidget(addProp); propsHead->addWidget(removeProp);
    editorLayout->addLayout(propsHead);
    properties_ = new QTableWidget(editorPanel);
    properties_->setColumnCount(2);
    properties_->setHorizontalHeaderLabels({"键 / Key", "值 / Value"});
    properties_->horizontalHeader()->setStretchLastSection(true);
    properties_->verticalHeader()->setVisible(false);
    properties_->setAlternatingRowColors(true);
    properties_->setSelectionBehavior(QAbstractItemView::SelectRows);
    properties_->setSelectionMode(QAbstractItemView::SingleSelection);
    editorLayout->addWidget(properties_);
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
    splitter->addWidget(vertical);
    splitter->setSizes({270, 1170});
    vertical->setSizes({590, 245});
}

QHash<QString, QColor> MainWindow::colors() const {
    const bool light = theme_->currentIndex() == 1;
    const QHash<QString, QColor> basic{
        {"Infantry", QColor(light ? "#9bd4ae" : "#90d8a3")},
        {"Vehicle", QColor(light ? "#e4ba82" : "#efba73")},
        {"Aircraft", QColor(light ? "#91c2e5" : "#91c8ff")},
        {"Building", QColor(light ? "#c4b2df" : "#c7a1ef")},
        {"Weapon", QColor(light ? "#e99f9f" : "#f19496")},
        {"Warhead", QColor(light ? "#dfa4ca" : "#e99ed4")},
        {"Projectile", QColor(light ? "#9eaef0" : "#a2b8ff")},
        {"Animation", QColor(light ? "#90d2c7" : "#89decf")},
        {"Superweapon", QColor(light ? "#e3d28b" : "#f1d782")},
        {"Registry", QColor(light ? "#b1c4d4" : "#a5b2ce")},
        {"Other", QColor(light ? "#b9c1cb" : "#adb9ca")}
    };
    auto chosen = basic;
    const QString prefix = "color/" + QString::number(theme_->currentIndex()) + "/";
    for (auto it = chosen.begin(); it != chosen.end(); ++it) {
        const QColor color(settings_.value(prefix + it.key()).toString());
        if (color.isValid()) it.value() = color;
    }
    return chosen;
}

void MainWindow::applyTheme() {
    if (!theme_ || !graph_) return;
    const int t = theme_->currentIndex();
    const QString bg = t == 1 ? "#f2f5fa" : (t == 2 ? "#0e2431" : t == 3 ? "#1e171a" : "#151a22");
    const QString panel = t == 1 ? "#ffffff" : (t == 2 ? "#123141" : t == 3 ? "#2b2025" : "#222935");
    const QString text = t == 1 ? "#1e293b" : "#ebf1f9";
    const QString accent = t == 1 ? "#3976ce" : (t == 2 ? "#55b7cb" : t == 3 ? "#dd7171" : "#729dea");
    const QString border = t == 1 ? "#d7e0ec" : "#3b4656";
    qApp->setStyleSheet(
        QString("QMainWindow,QWidget{background:%1;color:%2;}"
                "QMenuBar,QMenu,QToolBar,QStatusBar{background:%3;color:%2;}"
                "QLineEdit,QListView,QTableWidget,QPlainTextEdit,QComboBox,QTabWidget::pane{"
                "background:%3;color:%2;border:1px solid %4;border-radius:6px;}"
                "QLineEdit{padding:7px;}"
                "QPushButton{background:%3;color:%2;border:1px solid %4;border-radius:6px;padding:7px 12px;}"
                "QPushButton:hover,QListView::item:selected,QTableWidget::item:selected{background:%5;color:white;}"
                "QTabBar::tab{padding:9px 15px;background:%3;color:%2;}"
                "QTabBar::tab:selected{border-bottom:2px solid %5;}"
                "QHeaderView::section{background:%3;color:%2;padding:6px;border:none;}"
                "QLabel#sectionTitle{font-size:14px;font-weight:700;}"
                "QSplitter::handle{background:%4;}")
            .arg(bg, text, panel, border, accent));
    graph_->setColors(colors(), QColor(bg), QColor("#f4f7ff"));
    graph_->display(document_, current_);
    if (model_) {
        for (int i = 0; i < document_.sections().size(); ++i) {
            auto* item = model_->item(i);
            if (item) item->setForeground(colors().value(document_.category(i)));
        }
    }
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
        item->setData(i, Qt::UserRole + 1);
        item->setForeground(palette.value(document_.category(i), QColor("#9aa5b3")));
        item->setToolTip(document_.category(i) + " · " + QString::number(section.entries.size()) + " entries");
        model_->appendRow(item);
    }
    current_ = document_.indexOf(desired);
    if (current_ < 0 && !document_.sections().isEmpty()) current_ = 0;
    selectSection(current_);
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
    } else list_->clearSelection();
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
    graph_->display(document_, current_);
    int count = 0;
    for (const auto& s : document_.sections()) count += s.entries.size();
    statusBar()->showMessage(QString("%1 sections · %2 properties · %3")
        .arg(document_.sections().size()).arg(count)
        .arg(current_ < 0 ? "No selection" : document_.sections()[current_].name));
}
void MainWindow::titleUpdate() {
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
    const QColor initial = colors().value(category);
    const QColor chosen = QColorDialog::getColor(initial, this, "配置 " + category + " 节点颜色");
    if (chosen.isValid()) {
        settings_.setValue("color/" + QString::number(theme_->currentIndex()) + "/" + category, chosen.name());
        applyTheme();
        refreshList();
    }
}
void MainWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) event->accept(); else event->ignore();
}
