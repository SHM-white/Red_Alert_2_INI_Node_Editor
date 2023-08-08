#pragma once

#include "graphicscontrol_global.h"


class GRAPHICSCONTROL_EXPORT GraphicsControl
{
public:
    GraphicsControl(QGraphicsView* view);    
    void Rander(IBF_Setting& setting);
private:
    QGraphicsView* _view;
};
