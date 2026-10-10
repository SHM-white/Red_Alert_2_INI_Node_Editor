#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QHash>
#include <QListView>
#include <QPlainTextEdit>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTabWidget>
#include <QUndoStack>
#include <functional>
#include "IniDocument.h"
#include "GraphView.h"
#include "ThemeStore.h"
#include <QLabel>

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    void createUi();
    void applyTheme();
    void refreshList(const QString& focus = {});
    void selectSection(int section);
    void refreshDetails();
    bool maybeSave();
    bool applySourceIfNeeded();
    void openPath(const QString& path);
    bool save(bool as = false);
    void mutation(const QString& label, const std::function<bool()>& action, const QString& focus = {});
    void addSection();
    void renameSection();
    void deleteSection();
    void addKey();
    void deleteKey();
    void setCategoryColor();
    void editTheme();
    void importTheme();
    void exportTheme();
    void deleteTheme();
    void reloadThemeChoices(const QString& id = {});
    void updateCategoryCount();
    void syncSelectionToFilter();
    void titleUpdate();
    QHash<QString, QColor> colors() const;

    IniDocument document_;
    QSettings settings_{"SHM-white", "RA2NodeStudio"};
    ThemeStore themes_{settings_};
    QUndoStack history_{this};
    GraphView* graph_ = nullptr;
    QStandardItemModel* model_ = nullptr;
    QSortFilterProxyModel* proxy_ = nullptr;
    QListView* list_ = nullptr;
    QComboBox* categoryFilter_ = nullptr;
    QLabel* countInfo_ = nullptr;
    QLabel* fileInfo_ = nullptr;
    QLabel* sectionInfo_ = nullptr;
    QTableWidget* properties_ = nullptr;
    QPlainTextEdit* source_ = nullptr;
    QTabWidget* views_ = nullptr;
    QComboBox* theme_ = nullptr;
    QComboBox* encoding_ = nullptr;
    int current_ = -1;
};
