#include "GraphView.h"
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QSet>
#include <algorithm>
#include <cmath>

namespace {
constexpr qreal cardWidth = 292.0;
class Card final : public QGraphicsItem {
public:
    Card(const IniDocument& doc, int section, QColor color, QColor text, QColor surface, bool focused)
        : section_(section), name_(doc.sections()[section].name),
          category_(doc.category(section)), color_(color), text_(text), surface_(surface), focused_(focused) {
        for (const auto& e : doc.sections()[section].entries) {
            if (rows_.size() >= 8) break;
            rows_.append(qMakePair(e.key, e.value));
        }
        total_ = doc.sections()[section].entries.size();
        height_ = 69 + rows_.size() * 24 + (total_ > rows_.size() ? 21 : 0);
        setData(0, section_);
        setToolTip(QString("[%1]\n%2 entries — double click to inspect").arg(name_).arg(total_));
    }
    QRectF boundingRect() const override { return QRectF(0, 0, cardWidth, height_); }
    qreal height() const { return height_; }
    void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override {
        // Flat, squared VS-style section card. Category is an accent, never a full-color header.
        const QRectF outer = boundingRect().adjusted(0.5, 0.5, -0.5, -0.5);
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setPen(QPen(focused_ ? color_ : text_.darker(175), focused_ ? 2.0 : 1.0));
        p->setBrush(surface_);
        p->drawRect(outer);
        p->setPen(Qt::NoPen);
        p->setBrush(color_);
        p->drawRect(QRectF(2, 2, 5, height_ - 4));

        QFont titleFont = p->font();
        titleFont.setBold(true);
        titleFont.setPointSize(10);
        p->setFont(titleFont);
        p->setPen(text_);
        const QFontMetrics titleMetrics(titleFont);
        p->drawText(QRectF(16, 8, cardWidth - 28, 22), Qt::AlignLeft | Qt::AlignVCenter,
                    titleMetrics.elidedText(name_, Qt::ElideRight, int(cardWidth - 34)));
        QFont small = titleFont;
        small.setBold(false);
        small.setPointSize(9);
        p->setFont(small);
        p->setPen(color_);
        p->drawText(QRectF(16, 31, cardWidth - 30, 18), Qt::AlignLeft | Qt::AlignVCenter,
                    category_ + "  |  " + QString::number(total_) + " properties");
        p->setPen(QPen(text_.darker(165), 1.0));
        p->drawLine(QPointF(15, 54.5), QPointF(cardWidth - 9, 54.5));

        const QFontMetrics fm(small);
        for (int i = 0; i < rows_.size(); ++i) {
            const qreal y = 61 + i * 24;
            p->setPen(color_);
            p->drawText(QRectF(16, y, 110, 19), Qt::AlignVCenter,
                        fm.elidedText(rows_[i].first, Qt::ElideRight, 110));
            p->setPen(text_);
            p->drawText(QRectF(137, y, 140, 19), Qt::AlignVCenter,
                        fm.elidedText(rows_[i].second, Qt::ElideRight, 139));
        }
        if (total_ > rows_.size()) {
            p->setPen(text_);
            p->drawText(QRectF(15, height_ - 25, cardWidth - 25, 20),
                        Qt::AlignCenter, QString("+ %1 more properties").arg(total_ - rows_.size()));
        }
    }

private:
    int section_ = -1, total_ = 0;
    QString name_, category_;
    QColor color_, text_, surface_;
    bool focused_ = false;
    qreal height_ = 90;
    QVector<QPair<QString, QString>> rows_;
};
}

GraphView::GraphView(QWidget* parent) : QGraphicsView(parent), scene_(this) {
    setScene(&scene_);
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setTransformationAnchor(AnchorUnderMouse);
    setFrameShape(QFrame::NoFrame);
    setBackgroundBrush(background_);
    setCacheMode(CacheBackground);
    scene_.setItemIndexMethod(QGraphicsScene::NoIndex); // bounded, frequently rebuilt neighborhood
}

void GraphView::setColors(QHash<QString, QColor> colors, QColor background, QColor text, QColor cardBackground) {
    colors_ = std::move(colors);
    background_ = background;
    text_ = text;
    dots_ = background_.lightness() < 128 ? background_.lighter(145) : background_.darker(110);
    cardBackground_ = cardBackground;
    setBackgroundBrush(background_);
    resetCachedContent();
    scene_.update();
}

void GraphView::setDepthLimits(int parentDepth, int childDepth) {
    parentDepth_ = std::clamp(parentDepth, 0, 5);
    childDepth_ = std::clamp(childDepth, 0, 5);
}

void GraphView::display(const IniDocument& doc, int section) {
    scene_.clear();
    neighborhood_ = buildNeighborhood(doc, section, parentDepth_, childDepth_);
    if (section < 0 || section >= doc.sections().size()) {
        auto* note = scene_.addText("打开 INI 后，从左侧选择对象以查看引用关系");
        note->setDefaultTextColor(text_);
        scene_.setSceneRect(-280, -100, 560, 200);
        return;
    }

    // Scene geometry is built only for the bounded subgraph, never for the whole INI.
    // BFS depth determines the column; rows in a column are vertically centered.
    QHash<int, Card*> parentCards, childCards;
    const auto addCard = [&](int id, qreal x, qreal y, bool isFocus) {
        auto* item = new Card(doc, id, colors_.value(doc.category(id), QColor("#9aa4b2")),
                              text_, cardBackground_, isFocus);
        scene_.addItem(item);
        item->setPos(x, y);
        return item;
    };
    Card* focus = addCard(section, 0, 0, true);
    constexpr qreal columnGap = 445.0;
    constexpr qreal rowGap = 22.0;
    const auto placeColumns = [&](const QVector<GraphNeighborhood::Node>& nodes,
                                  int side, int depthLimit, QHash<int, Card*>& cards) {
        for (int depth = 1; depth <= depthLimit; ++depth) {
            QVector<int> ids;
            qreal height = 0;
            for (const auto& node : nodes) {
                if (node.depth != depth) continue;
                ids.append(node.section);
                const int count = doc.sections()[node.section].entries.size();
                height += 69 + std::min(count, 8) * 24 + (count > 8 ? 21 : 0);
            }
            if (ids.isEmpty()) continue;
            height += (ids.size() - 1) * rowGap;
            qreal y = focus->height() / 2 - height / 2;
            const qreal x = side * columnGap * depth;
            for (const int id : ids) {
                Card* card = addCard(id, x, y, false);
                cards.insert(id, card);
                y += card->height() + rowGap;
            }
        }
    };
    placeColumns(neighborhood_.parents, -1, parentDepth_, parentCards);
    placeColumns(neighborhood_.children, 1, childDepth_, childCards);

    const auto drawEdge = [&](Card* from, Card* to, int fromSection) {
        if (!from || !to) return;
        const QPointF start(from->pos().x() + cardWidth, from->pos().y() + from->height() / 2);
        const QPointF finish(to->pos().x(), to->pos().y() + to->height() / 2);
        QPainterPath curve(start);
        curve.cubicTo(start.x() + 60, start.y(), finish.x() - 60, finish.y(),
                      finish.x(), finish.y());
        auto* line = scene_.addPath(curve, QPen(
            colors_.value(doc.category(fromSection), QColor("#8794a2")), 1.65));
        line->setZValue(-2);
    };
    for (const auto& node : neighborhood_.parents) {
        drawEdge(parentCards.value(node.section, nullptr),
                 node.nextToFocus == section ? focus : parentCards.value(node.nextToFocus, nullptr),
                 node.section);
    }
    for (const auto& node : neighborhood_.children) {
        drawEdge(node.nextToFocus == section ? focus : childCards.value(node.nextToFocus, nullptr),
                 childCards.value(node.section, nullptr), node.nextToFocus);
    }

    const QRectF bounds = scene_.itemsBoundingRect().adjusted(-100, -80, 100, 80);
    scene_.setSceneRect(bounds);
    resetTransform();
    scale(0.92, 0.92);
    if (parentCards.size() + childCards.size() + 1 <= 2)
        centerOn(scene_.itemsBoundingRect().center());
    else centerOn(cardWidth / 2, focus->height() / 2);
}

void GraphView::fitGraph() {
    if (scene_.items().isEmpty()) return;
    fitInView(scene_.itemsBoundingRect().adjusted(-40, -40, 40, 40), Qt::KeepAspectRatio);
}

void GraphView::drawBackground(QPainter* painter, const QRectF& exposed) {
    painter->fillRect(exposed, background_);
    painter->setPen(QPen(dots_, 1));
    constexpr int spacing = 28;
    const int left = int(std::floor(exposed.left() / spacing)) * spacing;
    const int top = int(std::floor(exposed.top() / spacing)) * spacing;
    for (int x = left; x < exposed.right(); x += spacing)
        for (int y = top; y < exposed.bottom(); y += spacing)
            painter->drawPoint(x, y);
}


void GraphView::wheelEvent(QWheelEvent* event) {
    const qreal current = transform().m11();
    const qreal factor = event->angleDelta().y() > 0 ? 1.14 : 1.0 / 1.14;
    if (current * factor >= 0.25 && current * factor <= 2.4) scale(factor, factor);
    event->accept();
}
void GraphView::mouseDoubleClickEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());
    if (item && item->data(0).isValid()) {
        emit sectionActivated(item->data(0).toInt());
        event->accept();
        return;
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}
