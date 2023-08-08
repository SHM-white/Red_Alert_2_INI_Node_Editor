#include "QGraphicView_Demo.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qgraphicsview.h"

QGraphicView_Demo::QGraphicView_Demo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    QGraphicsScene* Scene=new QGraphicsScene();
    QGraphicsRectItem* item1 = new QGraphicsRectItem();
    item1->setRect(20, 20, 20, 20);
    item1->setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    Scene->addItem(item1);
    GraphicsItems* item2 = new GraphicsItems();
    item2->group->setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    Scene->addItem(item2->group);
    ui.graphicsView->setScene(Scene);
    ui.graphicsView->show();
}

QGraphicView_Demo::~QGraphicView_Demo()
{}

