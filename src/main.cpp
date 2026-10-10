#include "ui/EditorWindow.h"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("SHM-white");
    QCoreApplication::setApplicationName("RA2 INI Node Editor");
    app.setStyle("Fusion");
    EditorWindow window;
    window.show();
    return app.exec();
}
