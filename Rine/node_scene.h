#include <QGraphicsScene>
#include <QWheelEvent>
#include <QRectF>
#include <QLine>
#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QList>

#ifndef NODE_SCENE_H
#define NODE_SCENE_H
class node_scene:public QGraphicsScene{
public:
    node_scene(QWidget *parent = nullptr);
    void drawBackground(QPainter * painter, const QRectF &rect);
private:
    QColor light_color=QColor("#2f2f2f");
    QColor dark_color=QColor("#292929");
    QPen pen_light=QPen(this->light_color);
    QPen pen_dark=QPen(this->dark_color);
    int scene_width=64000,scene_height=64000;
    int left,right,top,bottom;
    int gridSize=20;
    int first_left,first_top;
    QList<QLine> lines_light,lines_dark;
};

#endif // NODE_SCENE_H
