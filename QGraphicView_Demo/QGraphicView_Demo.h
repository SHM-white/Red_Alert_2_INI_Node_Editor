#pragma once

#include <QtWidgets/QWidget>
#include "ui_QGraphicView_Demo.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qgraphicsview.h"

class QGraphicView_Demo : public QWidget
{
    Q_OBJECT

public:
    QGraphicView_Demo(QWidget *parent = nullptr);
    ~QGraphicView_Demo();

private:
    Ui::QGraphicView_DemoClass ui;
};

class GraphicsItems {
public:
    GraphicsItems() {
        group = new QGraphicsItemGroup();
        QGraphicsRectItem* item2 = new QGraphicsRectItem();
        item2->setRect(100, 20, 60, 20);
        group->addToGroup(item2);

        QGraphicsRectItem* item3 = new QGraphicsRectItem();
        item3->setRect(100, 100, 60, 20);
        group->addToGroup(item3);
    }
    QGraphicsItemGroup* group;
    QGraphicsItemGroup* operator()() {
        return group;
    }
};