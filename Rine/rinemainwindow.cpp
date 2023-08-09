#include "rinemainwindow.h"
#include "ui_rinemainwindow.h"
#include "node_scene.h"
#include "node_view.h"
#include <QFile>
#include <QPainter>
#include <QRectF>
#include <QGraphicsView>
#include "GraphicsControl.h"


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
    control.m_scene = scene;
    ui->graphicsView->setScene(scene);

    //拖动
    //ui->graphicsView->setDragMode(ScrollHandDrag);
};
RineMainWindow::~RineMainWindow()
{
    delete ui;
};

