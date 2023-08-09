#include "Node.h"

bool Node::hasChild()
{
    return false;
}

bool Node::hasParent()
{
    return false;
}

Node* Node::GetParent()
{
    return nullptr;
}

Node* Node::GetChild()
{
    return nullptr;
}

QGraphicsItemGroup* Node::operator()(void)
{
    auto* item_str = new QGraphicsLineItem;
    item_str->setLine

    return &item;
}

