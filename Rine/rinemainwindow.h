#ifndef RINEMAINWINDOW_H
#define RINEMAINWINDOW_H

#include "include.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RineMainWindow; }
QT_END_NAMESPACE

class RineMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RineMainWindow(QWidget *parent = nullptr);
    ~RineMainWindow();

private:
    Ui::RineMainWindow *ui;
    std::shared_ptr<GraphicsControl> control;
};
#endif // RINEMAINWINDOW_H
