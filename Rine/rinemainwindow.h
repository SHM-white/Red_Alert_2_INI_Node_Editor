#ifndef RINEMAINWINDOW_H
#define RINEMAINWINDOW_H

#include "include.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RineMainWindow; }
QT_END_NAMESPACE

class RineMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RineMainWindow(QWidget* parent = nullptr);
    ~RineMainWindow();

public slots:

    bool loadINI(const std::string& path);
    bool saveINI(const std::string& path);

private slots:
    void on_action_ini_E_triggered();

    void on_action_Q_triggered();

    void on_actionini_I_triggered();

    void on_action_O_triggered();

    void on_action_N_triggered();

    void on_action_S_triggered();

    void on_action_L_triggered();

    void updateAll();

    void updateOnlyView();

    void updateSectionList();

    void on_lineEdit_section_search_textChanged(const QString &arg1);

    void on_listWidget_sections_currentRowChanged(int currentRow);

    void on_listWidget_sections_itemDoubleClicked(QListWidgetItem *item);

private:
    bool AnalyzeINI();
    Ui::RineMainWindow* ui;
    mINI::INIStructure m_ini;
    std::string m_iniPath;
    std::unique_ptr<mINI::INIFile> m_iniFile;
    std::vector<std::string> m_operations;//TODO: Create operatoins class and save operatoins in this vector to be able to undo/redo operations
    GraphicsControl *m_graphicsControl;
    std::vector<ViewContent> m_viewContents;
    QString m_sectionFilter;

};

#endif // RINEMAINWINDOW_H
