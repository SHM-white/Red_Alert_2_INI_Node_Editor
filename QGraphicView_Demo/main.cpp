#include "QGraphicView_Demo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicView_Demo w;
    w.show();
    return a.exec();
}
