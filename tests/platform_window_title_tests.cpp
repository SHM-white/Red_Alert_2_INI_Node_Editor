#include "../modern/PlatformWindowTitle.h"
#include <QCoreApplication>
#include <QByteArray>
#include <QtGlobal>

static void check(bool ok, const char* msg) {
    if (!ok) qFatal("Window title test failed: %s", msg);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    const QString chinese = QStringLiteral("从预设创建自定义主题");
    const QString ascii = QStringLiteral("Create Custom Theme");
    check(PlatformWindowTitle::dialogTitle(chinese, ascii, true) == ascii,
          "WSL title must be ASCII");
    check(PlatformWindowTitle::dialogTitle(chinese, ascii, false) == chinese,
          "non-WSL title must stay Chinese");

#if defined(Q_OS_LINUX)
    const QByteArray old = qgetenv("WSL_DISTRO_NAME");
    const bool existed = qEnvironmentVariableIsSet("WSL_DISTRO_NAME");
    qputenv("WSL_DISTRO_NAME", "Debian");
    check(PlatformWindowTitle::isWsl(), "WSL environment detection");
    check(PlatformWindowTitle::dialogTitle(chinese, ascii) == ascii,
          "WSL default behavior");
    if (existed) qputenv("WSL_DISTRO_NAME", old);
    else qunsetenv("WSL_DISTRO_NAME");
#endif
    return 0;
}
