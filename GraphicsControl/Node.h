#pragma once
#include "graphicscontrol_global.h"
#include "GraphicsControl.h"

class GRAPHICSCONTROL_EXPORT Node
{
public:
	Node(const QString key, const QString value);
	QString m_key;
	QString m_value;
	bool hasChild();
	bool hasParent();
	Node* GetParent();
	Node* GetChild();
	QGraphicsItemGroup* operator()(void) const;
private:
	friend class GraphicsControl;
	std::shared_ptr<QGraphicsItemGroup> m_item;
	bool m_hasParent;
	bool m_hasChild;
	Node* m_pParent;
	Node* m_pChild;
};

