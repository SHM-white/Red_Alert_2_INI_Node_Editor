#include "GraphicsControl.h"
using namespace GraphicsControls;

ViewContent::ViewContent(const std::map<QString, QString>& content, const QString& title)
{
    m_content = content;
    m_title = title;
}

std::map<QString, QString> ViewContent::content() const
{
    return m_content;
}

void ViewContent::setContent(const std::map<QString, QString>& newContent)
{
    if (m_content == newContent)
        return;
    m_content = newContent;
}

QString ViewContent::title() const
{
    return m_title;
}

void ViewContent::setTitle(const QString& newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
}

GraphicsControl::GraphicsControl(QGraphicsView* view) : m_view(view) {
    m_nodeLists.clear();
}

GraphicsControl::GraphicsControl()
{
    m_view = nullptr;
    m_scene = nullptr;
    m_nodeLists.clear();
}

void GraphicsControl::SetView(QGraphicsView* view)
{
    if (view == nullptr) {
        return;
    }
    m_view = view;
}

void GraphicsControl::Init(const std::vector<ViewContent>& lists)
{
    int xOffset = 0;
    int yOffset = 0;
    if (m_view == nullptr) {
        return; // 确保视图已设置
    }
    if (m_scene == nullptr) {
        m_scene = new QGraphicsScene();
        m_view->setScene(m_scene);
    }
    // 清空现有的节点列表
    m_nodeLists.clear();
    // 根据传入的ViewContent列表初始化节点列表
    for (const auto& content : lists) {
        auto title = std::make_shared<GraphicsControls::Node_Title>(content.title(), NodeTitleColor);
        title->Init();
        title->setRect(QRectF(xOffset, 0, NodeSize.width(), NodeSize.height()));
        yOffset += NodeSize.height();
        std::vector<std::shared_ptr<GraphicsControls::Node>> nodes;
        for (const auto& item : content.content()) {
            nodes.push_back(std::make_shared<GraphicsControls::Node>(item.first, item.second, NodeColor, QRectF(xOffset, yOffset, NodeSize.width(), NodeSize.height())));
            yOffset += NodeSize.height();
        }
        auto nodeList = std::make_shared<GraphicsControls::Node_List>(title, nodes);
        nodeList->setRect(QRectF(xOffset, 0, NodeSize.width(), yOffset));
        if (nodeList->Init()) {
            m_nodeLists.push_back(nodeList);
            m_scene->addItem(nodeList.get());
        }
        xOffset += NodeSize.width();
        yOffset = 0;
    }
    createConnections();
}

void GraphicsControl::createConnections()
{
    for (const auto& nodeList : m_nodeLists) {
        for (const auto& nodelist2 : m_nodeLists) {
            if (nodeList.get() == nodelist2.get()) {
                continue;
            }
            for (const auto& node : nodeList->nodes()) {
                if (node->name() == nodelist2->title()->title()) {
                    nodelist2->add_connection(std::make_shared<Connection>(node.get(), nodelist2->title().get()));
                }
            }
        }
    }
}

void GraphicsControl::Render() {
    if (m_view == nullptr) {
        return;
    }
    if (m_scene == nullptr) {
        m_scene = new QGraphicsScene();
    }
        
    for (const auto& nodeList : m_nodeLists) {
        m_scene->addItem(nodeList.get());
        for (const auto& connection : nodeList->connections()) {
            m_scene->addItem(connection.get());
        }
    }
    m_view->setScene(m_scene);
}

GraphicsControls::Node_Title::Node_Title(QString title, QColor color)
    : m_title(title), m_color(color)
{
    Init();
}

GraphicsControls::Node_Title::Node_Title()
{
    m_title = QString("Untitled");
    m_color = QColor();
    Init();
}

bool GraphicsControls::Node_Title::Init()
{
    return true;
}

QString GraphicsControls::Node_Title::title() const
{
    return m_title;
}

QColor GraphicsControls::Node_Title::color() const
{
    return m_color;
}

GraphicsControls::Node::Node(QString name, QString value, QColor color, QRectF rect)
    : m_name(name), m_value(value), m_color(color), m_rect(rect)
{
    Init();
}

GraphicsControls::Node::Node()
{
    m_name = QString("Untitled");
    m_color = QColor();
    m_rect = QRectF();
    Init();
}

bool GraphicsControls::Node::Init()
{
    return true;
}

QString GraphicsControls::Node::name() const
{
    return m_name;
}

QColor GraphicsControls::Node::color() const
{
    return m_color;
}

QRectF GraphicsControls::Node::rect() const
{
    return m_rect;
}

Node_List::Node_List(std::shared_ptr<Node_Title> title, std::vector<std::shared_ptr<Node>> nodes)
    : m_title(title), m_nodes(nodes)
{
    Init();
}

bool GraphicsControls::Node_List::Init()
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
    return true;
}

std::shared_ptr<GraphicsControls::Node_Title> GraphicsControls::Node_List::title() const
{
    return m_title;
}

std::vector<std::shared_ptr<GraphicsControls::Node>> GraphicsControls::Node_List::nodes() const
{
    return m_nodes;
}

void GraphicsControls::Node_List::add_node(std::shared_ptr<Node> node)
{
    m_nodes.push_back(node);
}

void GraphicsControls::Node_List::remove_node(int index)
{
    m_nodes.erase(m_nodes.begin() + index);
}

void Node::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

void Node::setColor(const QColor &newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;
    emit colorChanged();
}

void Node::setRect(const QRectF &newRect)
{
    if (m_rect == newRect)
        return;
    m_rect = newRect;
    setPos(newRect.topLeft());
    emit rectChanged();
}

QString Node::value() const
{
    return m_value;
}

void Node::setValue(const QString &newValue)
{
    if (m_value == newValue)
        return;
    m_value = newValue;
    emit valueChanged();
}

void Node_List::setTitle(const std::shared_ptr<Node_Title> &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();
}

void Node_List::setNodes(const std::vector<std::shared_ptr<Node> > &newNodes)
{
    if (m_nodes == newNodes)
        return;
    m_nodes = newNodes;
    emit nodesChanged();
}

QRectF Node_List::rect() const
{
    return m_rect;
}

void Node_List::setRect(const QRectF &newRect)
{
    if (m_rect == newRect)
        return;
    m_rect = newRect;
    setPos(newRect.topLeft());
    emit rectChanged();
}

void Node_Title::setTitle(const QString &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();
}

void Node_Title::setColor(const QColor &newColor)
{
    if (m_color == newColor)
        return;
    m_color = newColor;
    emit colorChanged();
}
QRectF GraphicsControls::Node_List::boundingRect() const
{
    return m_rect;
}
void Node_List::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) 
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 绘制背景
    painter->setBrush(NodeListColor);
    painter->drawRect(boundingRect());

    // 绘制标题
    if (m_title) {
        m_title->paint(painter, option, widget);
    }

    // 绘制节点
    for (const auto& node : m_nodes) {
        node->paint(painter, option, widget);
    }
}

std::vector<std::shared_ptr<Connection>> GraphicsControls::Node_List::connections() const
{
    return m_connections;
}

void GraphicsControls::Node_List::add_connection(std::shared_ptr<Connection> connection)
{
    m_connections.push_back(connection);
}

QVariant Node_List::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        for (const auto& connection : m_connections) {
            connection->updatePosition();
        }
    }
    return QGraphicsObject::itemChange(change, value);
}

QRectF GraphicsControls::Node::boundingRect() const
{
    return m_rect;
}
void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) 
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 绘制背景
    painter->setBrush(m_color);
    painter->drawRect(boundingRect());

    // 绘制名称
    painter->setPen(Qt::black);
    painter->drawText(QRectF(m_rect.left(), m_rect.top() + NodeSize.height() / 4, NodeSize.width() / 2, NodeSize.height() / 2), Qt::AlignLeft | Qt::AlignVCenter, m_name);

    // 绘制值
    painter->drawText(QRectF(m_rect.left() + NodeSize.width() / 2, m_rect.top() + NodeSize.height() / 4, NodeSize.width() / 2, NodeSize.height() / 2), Qt::AlignLeft | Qt::AlignVCenter, m_value);
}

QRectF GraphicsControls::Node_Title::boundingRect() const
{
    return m_rect;
}
void Node_Title::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) 
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 绘制背景
    painter->setBrush(m_color);
    painter->drawRect(boundingRect());

    // 绘制文本
    painter->setPen(Qt::black);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_title);
}


QRectF Node_Title::rect() const
{
    return m_rect;
}

void Node_Title::setRect(const QRectF &newRect)
{
    if (m_rect == newRect)
        return;
    m_rect = newRect;
    setPos(newRect.topLeft());
    emit rectChanged();
}
Connection::Connection(QGraphicsObject* startItem, QGraphicsObject* endItem, QColor color)
    : m_startItem(startItem), m_endItem(endItem), m_color(color)
{
    connect(startItem, &QGraphicsObject::yChanged, this, &Connection::updatePosition);
    connect(endItem, &QGraphicsObject::yChanged, this, &Connection::updatePosition);
    updatePosition();
}

QRectF Connection::boundingRect() const
{
    return QRectF(m_startItem->scenePos(), m_endItem->scenePos()).normalized().adjusted(-2, -2, 2, 2);
}

void Connection::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(m_startItem->scenePos(), m_endItem->scenePos());
    qDebug() << m_startItem->scenePos() << "->" << m_endItem->scenePos();
}

void Connection::updatePosition()
{
    prepareGeometryChange();
    update();
}

void GraphicsControls::Node_List::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_dragging) {
        QPointF delta = event->scenePos() - m_dragStartPos;
        setPos(scenePos() + delta);
        m_dragStartPos = event->scenePos();
        prepareGeometryChange(); // 确保几何形状变化通知
        update(); // 确保重绘
    }
}

void GraphicsControls::Node_List::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_dragging == false) {
            m_dragging = true;
            m_dragStartPos = event->scenePos();
        }
    }
}

void GraphicsControls::Node_List::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}