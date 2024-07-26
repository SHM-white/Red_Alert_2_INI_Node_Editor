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

QSize ViewContent::nodeSize() const
{
    return m_nodeSize;
}

void ViewContent::setNodeSize(const QSize& newNodeSize)
{
    if (m_nodeSize == newNodeSize)
        return;
    m_nodeSize = newNodeSize;
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
    for (const auto& content : lists) {

        auto title = std::make_shared<GraphicsControls::Node_Title>(content.title(), Qt::black);
        std::vector<std::shared_ptr<GraphicsControls::Node>> nodes;
        //Add title and nodes to nodelist
        for (const auto& item : content.content()) {
            auto node = std::make_shared<GraphicsControls::Node>(item.first, Qt::blue, QRectF(0, 0, content.nodeSize().width(), content.nodeSize().height()));
            nodes.push_back(node);
        }
        auto nodeList = std::make_shared<GraphicsControls::Node_List>(title, nodes);
        m_nodeLists.push_back(nodeList);
    }

}

void GraphicsControl::Render() {
    if (m_view == nullptr) {
        return;
    }
    if (m_scene == nullptr) {
        m_scene = new QGraphicsScene();
    }
    m_nodeLists.clear();
    m_scene->clear();

    
    m_view->setScene(m_scene);
}
   
GraphicsControls::Node_Title::Node_Title(QString title, QColor color)
{
    m_title = title;
    m_color = color;
}

GraphicsControls::Node_Title::Node_Title()
{
    m_title = QString("Untitled");
    m_color = QColor();
}

bool GraphicsControls::Node_Title::Init()
{
    return false;
}

QString GraphicsControls::Node_Title::title() const
{
    return m_title;
}

QColor GraphicsControls::Node_Title::color() const
{
    return m_color;
}

GraphicsControls::Node::Node(QString name, QColor color, QRectF rect)
{
    m_name = name;
    m_color = color;
    m_rect = rect;
}

GraphicsControls::Node::Node()
{
    m_name = QString("Untitled");
    m_color = QColor();
    m_rect = QRectF();
}

bool GraphicsControls::Node::Init()
{
    return false;
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
{
    m_title = title;
    m_nodes = nodes;
}

bool GraphicsControls::Node_List::Init()
{
    return false;
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

std::shared_ptr<Node> GraphicsControls::Node_List::findParentNode(const QString& childNodeName) const
{
    for (const auto& node : m_nodes) {
        if (node->name() == childNodeName) {
            return node;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Node>> GraphicsControls::Node_List::findChildNodes(const QString& parentNodeName) const
{
    std::vector<std::shared_ptr<Node>> children;
    for (const auto& node : m_nodes) {
        if (node->name() == parentNodeName) {
            children.push_back(node);
        }
    }
    return children;
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
