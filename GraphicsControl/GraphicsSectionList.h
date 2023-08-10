#pragma once
#include <qgraphicsitem.h>
#include "qgraphicsview.h"
#include "qevent.h"
#include "qgraphicssceneevent.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene"
#include "qapplication.h"
class GraphicsSectionList :
    public QGraphicsItemGroup
{
public:
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        // 在此处添加禁用部分区域鼠标事件的逻辑
        if (isInDisabledArea(event->scenePos()))
        {
            QGraphicsItem* item = nullptr;
            for (QGraphicsItem* child : childItems())
            {
                if (child->type() == QGraphicsTextItem::Type && child->contains(event->pos()))
                {
                    item = child;
                    break;
                }
            }

            if (item && item->type() == QGraphicsTextItem::Type)
            {
                QGraphicsTextItem* textItem = static_cast<QGraphicsTextItem*>(item);
                QMouseEvent mouseEvent(QEvent::MouseButtonPress, event->pos(), event->screenPos(),
                    event->button(), event->buttons(), event->modifiers());
                QApplication::sendEvent(textItem, &mouseEvent);  // 发送自定义鼠标事件给文本项
                return;
            }

            event->ignore();  // 忽略鼠标事件
            return;
        }

        // 执行默认的鼠标事件处理逻辑
        QGraphicsItemGroup::mousePressEvent(event);
    }

private:
    bool isInDisabledArea(const QPointF& pos)
    {
        // 在此处添加判断鼠标位置是否位于禁用区域的逻辑
        // 返回 true 表示位于禁用区域
        // 返回 false 表示不在禁用区域
       /* QRectF disabledRect(100, 100, 200, 200); */ // 禁用区域的矩形范围
        //return disabledRect.contains(pos);
        return true;
    }
};

