#pragma once

#include "graphicscontrol_global.h"
#include "qgraphicsitem.h"
#include <qwidget.h>
#include "qgraphicsview.h"

class GRAPHICSCONTROL_EXPORT GraphicsControl
{
public:
    GraphicsControl(QGraphicsView* view);
private:
    QGraphicsView* _view;
};
