#pragma once

#include <QString>
#include <QtGlobal>

#if defined(Q_OS_LINUX)
#include <QFile>
#endif

// Work around WSLg/XWayland window decorations rendering some CJK titlebar
// glyphs incorrectly. Qt sends the correct UTF8_STRING _NET_WM_NAME, so this
// must NOT change text encoding, file contents, or the dialog's body text.
namespace PlatformWindowTitle {

inline bool isWsl() {
#if defined(Q_OS_LINUX)
    if (qEnvironmentVariableIsSet("WSL_DISTRO_NAME") ||
        qEnvironmentVariableIsSet("WSL_INTEROP"))
        return true;

    // Also handle Linux GUI launches that do not inherit WSL environment vars.
    QFile kernelRelease("/proc/sys/kernel/osrelease");
    if (kernelRelease.open(QIODevice::ReadOnly)) {
        const QString release = QString::fromLatin1(kernelRelease.readAll());
        return release.contains("microsoft", Qt::CaseInsensitive);
    }
#endif
    return false;
}

inline QString dialogTitle(const QString& localizedTitle,
                           const QString& wslAsciiTitle,
                           bool wsl = isWsl()) {
    return wsl ? wslAsciiTitle : localizedTitle;
}

} // namespace PlatformWindowTitle
