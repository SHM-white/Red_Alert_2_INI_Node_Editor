#ifndef RINEMAINWINDOW_H
#define RINEMAINWINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QMouseEvent>
#include "GraphicsControl.h"


QT_BEGIN_NAMESPACE
namespace Ui { class RineMainWindow; }
QT_END_NAMESPACE

class RineMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RineMainWindow(QWidget *parent = nullptr);
    ~RineMainWindow();
    //void wheelEvent(QWheelEvent *event);
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    //void mouseDoubleClickEvent(QMouseEvent *event);
private:
    Ui::RineMainWindow *ui;
    std::shared_ptr<GraphicsControl> control;
};
#endif // RINEMAINWINDOW_H
