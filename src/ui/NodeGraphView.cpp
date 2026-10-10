#include "NodeGraphView.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <cmath>

NodeGraphView::NodeGraphView(QWidget* parent): QGraphicsView(parent) {
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setCacheMode(QGraphicsView::CacheBackground);
    setFrameShape(QFrame::NoFrame);
}

void NodeGraphView::resetZoom() {
    resetTransform();
    if (scene() && !scene()->items().empty()) fitInView(scene()->itemsBoundingRect().adjusted(-55, -55, 55, 55), Qt::KeepAspectRatio);
}

void NodeGraphView::setCanvasColors(const QColor& background, const QColor& dots) {
    background_ = background; dots_ = dots; resetCachedContent(); viewport()->update();
}

void NodeGraphView::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() == 0) { QGraphicsView::wheelEvent(event); return; }
    const double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0/1.12;
    const double zoom = transform().m11() * factor;
    if (zoom >= 0.15 && zoom <= 3.0) scale(factor, factor);
    event->accept();
}

void NodeGraphView::drawBackground(QPainter* painter, const QRectF& exposed) {
    painter->fillRect(exposed, background_);
    painter->setPen(QPen(dots_, 1));
    constexpr int spacing = 28;
    const int left = static_cast<int>(std::floor(exposed.left()/spacing))*spacing;
    const int top = static_cast<int>(std::floor(exposed.top()/spacing))*spacing;
    // Draw only visible dots. Never accumulate persistent grid geometry.
    for (int x = left; x <= exposed.right(); x += spacing)
        for (int y = top; y <= exposed.bottom(); y += spacing)
            painter->drawPoint(x, y);
}
