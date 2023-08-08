#pragma once

#include "graphicscontrol_global.h"
#include "Node.h"

class Node_List
{
public:
	std::vector<Node> m_nodes;
	std::string m_title;
	QGraphicsItemGroup* item;

};

