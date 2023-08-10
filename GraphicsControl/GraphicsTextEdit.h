#pragma once
#include <qgraphicsitem.h>
class GraphicsTextEdit :
    public QGraphicsTextItem
{
public:
    explicit GraphicsTextEdit(const QString& text, QGraphicsItem* parent = nullptr) : QGraphicsTextItem(text, parent) 
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setEnabled(true);
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
        
    }
    ~GraphicsTextEdit() { QGraphicsTextItem::~QGraphicsTextItem(); }
protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    //void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

};

