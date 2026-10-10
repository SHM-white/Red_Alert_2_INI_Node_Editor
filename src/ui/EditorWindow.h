#pragma once

#include "IniDocument.h"
#include <QMainWindow>
#include <QColor>
#include <QString>
#include <functional>
#include <unordered_map>
#include <vector>

class QCloseEvent;
class QComboBox;
class QGraphicsScene;
class QItemSelection;
class QLabel;
class QLineEdit;
class QListView;
class QSortFilterProxyModel;
class QSplitter;
class QStandardItemModel;
class QTableWidget;
class QTimer;
class QUndoStack;
class NodeGraphView;

class EditorWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit EditorWindow(QWidget* parent = nullptr);
    void restoreSnapshot(const std::string& bytes);
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    struct Palette {
        QString background, panel, panelAlt, text, muted, border, accent, canvas, dot;
    };
    IniDocument document_;
    QString filePath_;
    QString theme_;
    int selectedSection_ = -1;
    QLineEdit* search_ = nullptr;
    QListView* sections_ = nullptr;
    QStandardItemModel* sectionModel_ = nullptr;
    QSortFilterProxyModel* filterModel_ = nullptr;
    QComboBox* categoryFilter_ = nullptr;
    QGraphicsScene* graphScene_ = nullptr;
    NodeGraphView* graph_ = nullptr;
    QTableWidget* properties_ = nullptr;
    QLabel* sectionInfo_ = nullptr;
    QLabel* fileInfo_ = nullptr;
    QLabel* countInfo_ = nullptr;
    QUndoStack* undo_ = nullptr;
    QTimer* filterTimer_ = nullptr;
    std::unordered_map<std::string, int> sectionByName_;
    std::unordered_map<std::string, std::vector<int>> incoming_;
    std::unordered_map<std::string, QString> objectKinds_;

    void createUi();
    void createMenus();
    void refreshDocument(const QString& preferredName = {});
    void buildIndex();
    void selectSection(int sectionIndex, bool syncList = true);
    void updateGraph();
    void updateProperties();
    void updateTitle();
    void setTheme(const QString& name);
    [[nodiscard]] Palette palette() const;
    [[nodiscard]] QString categoryFor(int index) const;
    [[nodiscard]] QColor colorFor(const QString& category) const;
    [[nodiscard]] QString decode(const std::string& bytes) const;
    [[nodiscard]] std::string encode(const QString& string) const;
    [[nodiscard]] bool maybeSave();
    void applyEdit(const QString& label, const std::function<bool(IniDocument&)>& change, const QString& focus = {});
    void newFile();
    void openFile();
    bool openPath(const QString& path);
    bool saveFile(bool saveAs = false);
    void addSection();
    void renameSection();
    void deleteSection();
    void addProperty();
    void deleteProperty();
};
