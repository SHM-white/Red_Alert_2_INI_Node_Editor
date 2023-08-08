#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(GRAPHICSCONTROL_LIB)
#  define GRAPHICSCONTROL_EXPORT Q_DECL_EXPORT
# else
#  define GRAPHICSCONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define GRAPHICSCONTROL_EXPORT
#endif
