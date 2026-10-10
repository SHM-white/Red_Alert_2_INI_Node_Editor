#pragma once
#include <QGraphicsView>

class NodeGraphView final : public QGraphicsView {
public:
    explicit NodeGraphView(QWidget* parent = nullptr);
    void resetZoom();
    void setCanvasColors(const QColor& background, const QColor& dots);
protected:
    void wheelEvent(QWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& exposed) override;
private:
    QColor background_ = QColor("#101827");
    QColor dots_ = QColor("#293345");
};
