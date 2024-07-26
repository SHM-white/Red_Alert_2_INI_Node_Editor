#pragma once

#include "graphicscontrol_global.h"
#include <QObject>
#include <QString>
#include <QColor>
#include <QRectF>
#include <memory>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>

namespace GraphicsControls {
    class Node_List;
    class Node_Title;
    class Node;

    class GRAPHICSCONTROL_EXPORT Node_List : public QObject, public QGraphicsObject, public QGraphicsItemGroup
    {
        Q_OBJECT
    public:
        Node_List(std::shared_ptr<Node_Title> title, std::vector<std::shared_ptr<Node>> nodes);
        bool Init();
        Q_PROPERTY(std::shared_ptr<Node_Title> title READ title WRITE setTitle NOTIFY titleChanged FINAL)
        Q_PROPERTY(std::vector<std::shared_ptr<Node>> nodes READ nodes WRITE setNodes NOTIFY nodesChanged FINAL)
        Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged FINAL)
        std::shared_ptr<Node_Title> title() const;
        std::vector<std::shared_ptr<Node>> nodes() const;
        void add_node(std::shared_ptr<Node> node);
        void remove_node(int index);
        // 查找父节点和子节点
        std::shared_ptr<Node> findParentNode(const QString& childNodeName) const;
        std::vector<std::shared_ptr<Node>> findChildNodes(const QString& parentNodeName) const;
        void setTitle(const std::shared_ptr<Node_Title> &newTitle);
        void setNodes(const std::vector<std::shared_ptr<Node> > &newNodes);
        QRectF rect() const;
        void setRect(const QRectF &newRect);
    signals:
        void titleChanged();
        void nodesChanged();
        void rectChanged();
    private:
        std::shared_ptr<Node_Title> m_title;
        std::vector<std::shared_ptr<Node>> m_nodes;
        QRectF m_rect;
    };

    class GRAPHICSCONTROL_EXPORT Node_Title : public QObject, public QGraphicsObject, public QGraphicsItemGroup
    {
        Q_OBJECT
    public:
        Node_Title(QString title, QColor color);
        Node_Title();
        bool Init();
        Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
        QString title() const;
        QColor color() const;;
        void setTitle(const QString &newTitle);
        void setColor(const QColor &newColor);
    signals:
        void titleChanged();
        void colorChanged();
    private:
        QString m_title;
        QColor m_color;
    };

    class GRAPHICSCONTROL_EXPORT Node : public QObject, public QGraphicsObject, public QGraphicsItemGroup
    {
        Q_OBJECT
    public:
        Node(QString name, QColor color, QRectF rect);
        Node();
        bool Init();
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
        Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
        Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged FINAL)
        Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged FINAL)
        QString name() const;
        QColor color() const;
        QRectF rect() const;
        void setName(const QString &newName);
        void setColor(const QColor &newColor);
        void setRect(const QRectF &newRect);
        QString value() const;
        void setValue(const QString &newValue);
    signals:
        void nameChanged();
        void colorChanged();
        void rectChanged();
        void valueChanged();
    private:
        QString m_name;
        QColor m_color;
        QRectF m_rect;
        QString m_value;
    };
}

class GRAPHICSCONTROL_EXPORT ViewContent
{
public:
    ViewContent(const std::map<QString, QString>& content, const QString& title);
    ViewContent() = default;
    std::map<QString, QString> content() const;
    void setContent(const std::map<QString, QString>& newContent);
    QString title() const;
    void setTitle(const QString& newTitle);
    QSize nodeSize() const;
    void setNodeSize(const QSize& newNodeSize);
private:
    std::map<QString, QString> m_content;
    QString m_title;
    QSize m_nodeSize{ 200,20 };
};

class GRAPHICSCONTROL_EXPORT GraphicsControl
{
public:
    GraphicsControl(QGraphicsView* view); // get pointer to QGraphicsView control
    GraphicsControl();
    void SetView(QGraphicsView* view); // set pointer to QGraphicsView control
    void Render(); // 与后台对接
    void Init(const std::vector<ViewContent>& lists); // 初始化
    QGraphicsScene* m_scene;
private:
    QGraphicsView* m_view;
    std::vector<std::shared_ptr<GraphicsControls::Node_List>> m_nodeLists;
};

