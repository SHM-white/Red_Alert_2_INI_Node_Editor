#pragma once
#include "GraphicsControl.h"
#include "Node_List.h"

GraphicsControl::GraphicsControl(QGraphicsView* view) 
{
	m_view = view;
}

void GraphicsControl::Render(std::map<QString, const std::map<QString, QString>> lists)
{
	auto scene = new QGraphicsScene();
	int count = 0;
	for (const auto& i : lists) {
		auto list = new Node_List(i.first, i.second);
		auto listItem = (*list)();
		listItem->setPos((NodeSize.width() + 20) * count, 0);
		listItem->setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
		scene->addItem(listItem);
		++count;
	}
	m_view->setScene(scene);
	m_view->show();
}


