#include "rinemainwindow.h"
#include "ui_rinemainwindow.h"
#include "include.h"

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

    //GraphicsView背景
    node_scene *scene=new node_scene(this);
    control = std::make_shared<GraphicsControl>(ui->graphicsView);
    control->m_scene = scene;
    ui->graphicsView->setScene(scene);

    //拖动
    //ui->graphicsView->setDragMode(ScrollHandDrag);
#ifdef DEBUG

#endif // DEBUG

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


void RineMainWindow::on_action_Q_triggered()
{
    qApp->quit();
}


void RineMainWindow::on_actionini_I_triggered()
{

}


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
}


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


void RineMainWindow::on_action_L_triggered()
{

}

