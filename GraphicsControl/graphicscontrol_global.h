#pragma once

#include <QtCore/qglobal.h>
#include "qgraphicsitem.h"
#include <qwidget.h>
#include "qgraphicsview.h"
#include <INIBrowser/IBFront.h>
#include <vector>

#ifndef BUILD_STATIC
# if defined(GRAPHICSCONTROL_LIB)
#  define GRAPHICSCONTROL_EXPORT Q_DECL_EXPORT
# else
#  define GRAPHICSCONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define GRAPHICSCONTROL_EXPORT
#endif
