#pragma once
#include "GraphicsControl.h"
#include "Node_List.h"

GraphicsControl::GraphicsControl(QGraphicsView* view) 
{
	m_view = view;
	m_scene = nullptr;
}

void GraphicsControl::Render(std::map<QString, const std::map<QString, QString>> lists)
{
	if(m_scene==nullptr)
	{
		m_scene = new QGraphicsScene();
	}
	int count = 0;
	m_scene->clear();
	for (const auto& i : lists) {
		auto list = new Node_List(i.first, i.second);
		auto listItem = (*list)();
		listItem->setPos((NodeSize.width() + 20) * count, 0);
		listItem->setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
		m_scene->addItem(listItem);
		++count;
	}
	m_view->setScene(m_scene);
	m_view->show();
}

std::map<std::shared_ptr<Node>, std::shared_ptr<Node_List>> GraphicsControl::GetRelatedItems(std::map<QString, const std::map<QString, QString>> lists)
{
	return m_relatedItems;
}

bool GraphicsControl::HasRelation(std::shared_ptr<Node> node, std::shared_ptr<Node_List> nodeList)
{
	return node->m_key == nodeList->m_section;
}


