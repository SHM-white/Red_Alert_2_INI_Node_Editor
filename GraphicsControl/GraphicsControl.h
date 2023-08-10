#pragma once

#include "graphicscontrol_global.h"
class GRAPHICSCONTROL_EXPORT Node_List;
class GRAPHICSCONTROL_EXPORT Node;

const QSize NodeSize{100, 20};

class GRAPHICSCONTROL_EXPORT GraphicsControl
{
public:
    GraphicsControl(QGraphicsView* view);//get pointer to QGraphicsView control
    void Render(std::map<QString,const std::map<QString,QString>> lists);//与后台对接
    std::map<std::shared_ptr<Node>, std::shared_ptr<Node_List>> GetRelatedItems(std::map<QString, const std::map<QString, QString>> lists);
    QGraphicsScene* m_scene;
private:
    QGraphicsView* m_view;
    std::map<std::shared_ptr<Node>, std::shared_ptr<Node_List>> m_relatedItems;
    bool static HasRelation(std::shared_ptr<Node> node, std::shared_ptr<Node_List> nodeList);
};
