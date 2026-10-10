#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("RA2 Node Studio");
    QApplication::setOrganizationName("SHM-white");
    app.setStyle(QStyleFactory::create("Fusion"));
    MainWindow window;
    window.show();
    return app.exec();
}
