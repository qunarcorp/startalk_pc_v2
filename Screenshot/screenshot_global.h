#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SCREENSHOT_LIB)
#  define SCREENSHOT_EXPORT Q_DECL_EXPORT
# else
#  define SCREENSHOT_EXPORT Q_DECL_IMPORT
# endif
#else
# define SCREENSHOT_EXPORT
#endif
