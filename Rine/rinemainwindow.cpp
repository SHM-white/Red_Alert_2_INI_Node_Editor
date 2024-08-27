#include "rinemainwindow.h"
#include "ui_rinemainwindow.h"
#include "include.h"
#include "Information.h"

RineMainWindow::RineMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RineMainWindow)
{
    ui->setupUi(this);
    QFile file(":/qss.qss");
    file.open(QFile::ReadOnly);

    //设置qss
    if(file.isOpen()){
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }

    ////GraphicsView背景
    //node_scene *scene=new node_scene(this);
    //control = std::make_shared<GraphicsControl>(ui->graphicsView);
    //control->m_scene = scene;
    //ui->graphicsView->setScene(scene);


    //Initialize GraphicsControl
    m_graphicsControl = new GraphicsControl(ui->graphicsView);
    updateAll();
    GraphicsControls::Settings::CaseSensitive = false;
};
RineMainWindow::~RineMainWindow()
{
    delete ui;
}
bool RineMainWindow::loadINI(const std::string& path)
{
    m_iniPath=path;
    m_iniFile = std::make_unique<mINI::INIFile>(path);
    if(m_iniFile->read(m_ini)){
        return true;
    }
    return false;
}

bool RineMainWindow::saveINI(const std::string& path)
{
    if (!path.empty()) {
        m_iniFile = std::make_unique<mINI::INIFile>(path);
        m_iniPath = path;
    }
    if (m_iniPath.empty()) {
        return false;
    }
    if (m_iniFile->write(m_ini)) {
        return true;
    }
    return false;
}

void RineMainWindow::on_action_ini_E_triggered()
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(m_iniPath.c_str()))) {
        QMessageBox::warning(this, "打开失败", QString::fromStdString("无法打开文件：" + m_iniPath));
    }
}

//Quit Application
void RineMainWindow::on_action_Q_triggered()
{
    qApp->quit();
}

//Open Information Window
void RineMainWindow::on_actionini_I_triggered()
{
    Information *info = new Information(this);
    info->show();
}

//Open File
void RineMainWindow::on_action_O_triggered()
{
    auto path = QFileDialog::getOpenFileName(this, "打开文件", "", "ini文件 (*.ini)");
    if (!path.isEmpty()) {
        if (loadINI(path.toStdString())) {
            QMessageBox::information(this, "打开成功", QString::fromStdString("打开文件：" + path.toStdString()));
        } else {
            QMessageBox::warning(this, "打开失败", QString::fromStdString("无法打开文件：" + path.toStdString()));
        }
    }
    updateAll();
}

//New File
void RineMainWindow::on_action_N_triggered()
{
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
    std::filesystem::path temp_file = temp_dir / "temp.ini";
    if (std::filesystem::exists(temp_file)) {
        std::filesystem::remove(temp_file);
    }
    std::ofstream out(temp_file.string());
    out.close();
    if (!loadINI(temp_file.string())) {
        QMessageBox::warning(this, "新建失败", "无法创建新文件，请检查临时目录权限");
        return;
    }

}

//Save File
void RineMainWindow::on_action_S_triggered()
{
    auto path = QFileDialog::getSaveFileName(this, "保存文件", "", "ini文件 (*.ini)");
    if (saveINI(path.toStdString())) {
        QMessageBox::information(this, "保存成功", QString::fromStdString("保存文件：" + m_iniPath));
    }
    else {
        QMessageBox::warning(this, "保存失败", QString::fromStdString("无法保存文件：" + path.toStdString()));
    }
}

//Load file(Why add an action instead of use Open?)
void RineMainWindow::on_action_L_triggered()
{
    QMessageBox::information(this, "提示", "建议使用打开文件");
    on_action_O_triggered();
}

void RineMainWindow::updateAll()
{
    AnalyzeINI();
    updateSectionList();
}

void RineMainWindow::updateOnlyView()
{
    if (ui->listWidget_sections->currentItem() == nullptr) {
        return;
    }
    ViewContent content;
    bool hasFound{ false };
    auto current_section = ui->listWidget_sections->currentItem()->text();
    for (auto& c : m_viewContents) {
        if (c.title() == current_section) {
            if (hasFound){throw std::runtime_error("Duplicate section found");}
            content = c;
            hasFound = true;
            break;
        }
    }
    //Find related sections
    auto results = Utils::RemoveRepeatedContents(Utils::GetRelatedContents(&content, m_viewContents));

    m_graphicsControl->Init(results);

    m_graphicsControl->Render();

}

void RineMainWindow::updateSectionList()
{
    //TODO:Update Section List
    //clear listview
    ui->listWidget_sections->clear();
    //if (!m_sectionFilter.isEmpty()) {
    for (auto& content : m_viewContents
        | std::views::filter([=](const ViewContent& content) { return content.title().contains(m_sectionFilter, Qt::CaseInsensitive); }))
    {
        ui->listWidget_sections->addItem(content.title());
    }
    //}
    //else {
    //    for (auto& content : m_viewContents) {
    //        ui->listWidget_sections->addItem(content.title());
    //    }
    //}
    ui->listWidget_sections->setCurrentRow(0);
}

bool RineMainWindow::AnalyzeINI()
{
    m_viewContents.clear();
    //TODO:Analyze INI
    for (auto& section : m_ini) {
        ViewContent content;
        content.setTitle(QString::fromStdString(section.first));
        std::map<QString, QString> items;
        for (auto& item : section.second) {
            items[QString::fromStdString(item.first)] = QString::fromStdString(item.second);
        }
        content.setContent(items);
        m_viewContents.push_back(content);
    }
    return true;
}


void RineMainWindow::on_lineEdit_section_search_textChanged(const QString &arg1)
{
    m_sectionFilter = arg1;
    updateSectionList();
}


void RineMainWindow::on_listWidget_sections_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow);
    updateOnlyView();
}


void RineMainWindow::on_listWidget_sections_itemDoubleClicked(QListWidgetItem *item)
{

}

