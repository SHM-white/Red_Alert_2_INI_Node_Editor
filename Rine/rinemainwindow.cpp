#include "rinemainwindow.h"
#include "ui_rinemainwindow.h"
#include "node_scene.h"
#include "node_view.h"
#include <QFile>
#include <QPainter>
#include <QRectF>
#include <QGraphicsView>
#include "GraphicsControl.h"

#define DEBUG

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
    std::map<QString, const std::map<QString, QString>> lists;
    std::map<QString, QString> items1, items2;
    items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test21" }, QString{ "test22" }));
    items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test31" }, QString{ "test32" }));
    items1.insert(items1.begin(), std::pair< QString, QString>(QString{ "test41" }, QString{ "test42" }));
    items2.insert(items2.begin(), std::pair< QString, QString>(QString{ "test51" }, QString{ "test52" }));
    lists.insert(lists.begin(), std::pair<QString, const std::map<QString, QString>>(QString{ "test1" }, items1));
    lists.insert(lists.begin(), std::pair<QString, const std::map<QString, QString>>(QString{ "test2" }, items2));
    control->Render(lists);
#endif // DEBUG

};
RineMainWindow::~RineMainWindow()
{
    delete ui;
};

