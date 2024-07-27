#ifndef GRAPHICSCONTROL_GLOBAL_H
#define GRAPHICSCONTROL_GLOBAL_H

#include <QtCore/qglobal.h>
#include <qdebug.h>
#include <QWidget>
#include <map>
#include <QString>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QPolygon>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QObject>
#include <QRectF>
#include <memory>
#include <vector>
#include <QDrag>


#if defined(GRAPHICSCONTROL_LIB)
#  define GRAPHICSCONTROL_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHICSCONTROL_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPHICSCONTROL_GLOBAL_H
