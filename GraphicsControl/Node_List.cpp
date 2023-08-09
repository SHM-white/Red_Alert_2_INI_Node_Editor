#pragma once

#include "Node_List.h"

Node_List::Node_List(QString section, const std::map<QString, QString>& key_value):m_section{section}
{
	item = std::make_shared<QGraphicsItemGroup>();
	for (const auto& i:key_value) {
		m_nodes.push_back(std::make_shared<Node>(i.first, i.second));
	}
}

QGraphicsItemGroup* Node_List::operator()(void) const
{
	auto* itemTitle = new QGraphicsTextItem(m_section);
	itemTitle->setPos(0, 0);
	item->addToGroup(itemTitle);
	//todo:add effect to title to display section
	for (int i = 0; i < m_nodes.size();++i) {
		auto* itemNode = (*m_nodes[i])();
		auto* itemRect = new QGraphicsRectItem();
		itemRect->setRect(0, NodeSize.height() * (i + 1), NodeSize.width(), NodeSize.height());
		itemNode->setPos(0, NodeSize.height() * (i + 1));
		item->addToGroup(itemNode);
		item->addToGroup(itemRect);
	}
	return item.get();
}

Node* Node_List::operator[](size_t index)
{
	if (index < 0 || index >= m_nodes.size()) {
		throw std::logic_error("Out of range! At Node_List::operator[]");
	}
	return m_nodes[index].get();
}
