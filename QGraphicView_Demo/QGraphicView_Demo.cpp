#include "QGraphicView_Demo.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qgraphicsview.h"
#include "GraphicsControl"
#include 

QGraphicView_Demo::QGraphicView_Demo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    ui.graphicsView->show();
}

QGraphicView_Demo::~QGraphicView_Demo()
{}

