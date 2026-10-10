#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTimer>
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("RA2 Node Studio");
    QApplication::setOrganizationName("SHM-white");
    app.setStyle(QStyleFactory::create("Fusion"));
    MainWindow window;
    if (app.arguments().contains("--smoke-test")) {
        // Exercise QMainWindow, proxies, graph, and theme setup on a headless Qt platform.
        QTimer::singleShot(0, &app, &QCoreApplication::quit);
    } else {
        window.show();
    }
    return app.exec();
}
