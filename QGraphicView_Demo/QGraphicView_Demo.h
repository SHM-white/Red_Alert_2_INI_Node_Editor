#pragma once

#include <QtWidgets/QWidget>
#include "ui_QGraphicView_Demo.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem.h"
#include "qgraphicsview.h"
#include "GraphicsControl.h"

class QGraphicView_Demo : public QWidget
{
    Q_OBJECT

public:
    QGraphicView_Demo(QWidget *parent = nullptr);
    ~QGraphicView_Demo();
public slots:

    void on_pushButton_clicked();

private:
    Ui::QGraphicView_DemoClass ui;
    GraphicsControl m_control;
};
