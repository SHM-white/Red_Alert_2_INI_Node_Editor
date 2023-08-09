#pragma once
#include "graphicscontrol_global.h"
class Node
{
public:
	std::string key;
	std::string value;
	bool hasChild();
	bool hasParent();
	Node* GetParent();
	Node* GetChild();
	QGraphicsItemGroup* operator()(void);
private:
	QGraphicsItemGroup item;
	bool m_hasParent;
	bool m_hasChild;
	Node* m_pParent;
	Node* m_pChild;
};

