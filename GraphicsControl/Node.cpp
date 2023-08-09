﻿#pragma once

#include "Node.h"
#include "qgraphicsitem.h"

Node::Node(const QString key, const QString value)
{
    m_key = key;
    m_value = value;
    m_item = std::make_shared<QGraphicsItemGroup>();
}

bool Node::hasChild()
{
    return m_hasChild;
}

bool Node::hasParent()
{
    return m_hasParent;
}

Node* Node::GetParent()
{
    if (m_hasParent) {
        return m_pParent;
    }
    else
    {
        return nullptr;
    }
}

Node* Node::GetChild()
{
    if(m_hasChild){
        return m_pChild;
    }
    else
    {
        return nullptr;
    }
}

QGraphicsItemGroup* Node::operator()(void) const
{
    auto item_key = std::make_shared<QGraphicsTextItem>(m_key);
    item_key->setPos(0, 0);
    m_item->addToGroup(item_key.get());

    auto item_value = std::make_shared<QGraphicsTextItem>(m_value);
    item_value->setPos((int)(NodeSize.width() / 2), 0);
    m_item->addToGroup(item_value.get());
    return m_item.get();
}

