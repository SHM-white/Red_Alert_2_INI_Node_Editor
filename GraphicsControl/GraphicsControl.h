#pragma once

#include "graphicscontrol_global.h"


class GRAPHICSCONTROL_EXPORT GraphicsControl
{
public:
    GraphicsControl(QGraphicsView* view);
    //void Render(IBF_Setting& setting);//与后台对接
private:
    QGraphicsView* m_view;
};
