#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHash>
#include <QColor>
#include "IniDocument.h"
#include "GraphNeighborhood.h"

class GraphView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphView(QWidget* parent = nullptr);
    void setColors(QHash<QString, QColor> categoryColors, QColor background, QColor text, QColor cardBackground);
    void display(const IniDocument& document, int sectionIndex);
    void fitGraph();
    void setDepthLimits(int parentDepth, int childDepth);
    int parentDepth() const { return parentDepth_; }
    int childDepth() const { return childDepth_; }
    const GraphNeighborhood& visibleNeighborhood() const { return neighborhood_; }
signals:
    void sectionActivated(int section);
protected:
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
private:
    QGraphicsScene scene_;
    GraphNeighborhood neighborhood_;
    int parentDepth_ = 1;
    int childDepth_ = 1;
    QHash<QString, QColor> colors_;
    QColor background_{"#151922"}, text_{"#f2f6fc"}, cardBackground_{"#222935"}, dots_{"#344253"};
};
