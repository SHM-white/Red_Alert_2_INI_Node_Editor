#pragma once

#include "graphicscontrol_global.h"

const QSize NodeSize{100, 20};

class GRAPHICSCONTROL_EXPORT GraphicsControl
{
public:
    GraphicsControl(QGraphicsView* view);//get pointer to QGraphicsView control
    void Render(std::map<QString,const std::map<QString,QString>> lists);//与后台对接
private:
    QGraphicsView* m_view;
};
