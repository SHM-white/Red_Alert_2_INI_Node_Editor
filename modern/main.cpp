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
        QTimer::singleShot(0, &app, [&app, &window] {
            const bool passed = window.smokeTestUi();
            if (passed && qEnvironmentVariableIsSet("RA2_UI_PREVIEW_PATH")) {
                window.resize(1400, 840);
                window.show();
                app.processEvents();
                const QString path = qEnvironmentVariable("RA2_UI_PREVIEW_PATH");
                if (!window.grab().save(path)) qWarning("Could not save UI preview");
                window.hide();
            }
            if (!passed) qCritical("UI filtering / flat theme smoke test failed");
            app.exit(passed ? 0 : 2);
        });
    } else {
        window.show();
    }
    return app.exec();
}
