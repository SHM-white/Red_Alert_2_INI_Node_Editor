#pragma once
#include "QGraphicView_Demo.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qgraphicsview.h"
#include "GraphicsControl.h"
#include "qdialog.h"
#include "qmessagebox.h"

QGraphicView_Demo::QGraphicView_Demo(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    try
    {
        GraphicsControl control(ui.graphicsView);
        std::map<QString, const std::map<QString, QString>> lists;
        std::map<QString, QString> items;
        items.insert(items.begin(), std::pair< QString, QString>(QString{ "test1" }, QString{ "test2" }));
        lists.insert(lists.begin(), std::pair<QString, const std::map<QString, QString>>(QString{ "test" }, items));
        control.Render(lists);
        this->show();
    }
    catch (std::exception& ex) {
        QMessageBox::warning(this, "error", ex.what(), QMessageBox::Ok);
    }
    
}

QGraphicView_Demo::~QGraphicView_Demo()
{}

