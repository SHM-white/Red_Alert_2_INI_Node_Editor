#pragma once

#include "graphicscontrol_global.h"
#include "Node.h"
#include "GraphicsSectionList.h"
#include "GraphicsControl.h"


class GRAPHICSCONTROL_EXPORT Node_List
{
public:
	Node_List(QString section,const std::map<QString, QString>& key_value);
	QGraphicsItemGroup* operator()(void) const;//get group
	Node* operator[](size_t index);
	int size();
private:
	friend class GraphicsControl;
	std::vector<std::shared_ptr<Node>> m_nodes;
	QString m_section;
	std::shared_ptr<GraphicsSectionList> item;
};

