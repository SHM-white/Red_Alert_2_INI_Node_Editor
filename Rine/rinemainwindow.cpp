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
};

