#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(UIGROUPMANAGER_LIB)
#  define UIGROUPMANAGER_EXPORT Q_DECL_EXPORT
# else
#  define UIGROUPMANAGER_EXPORT Q_DECL_IMPORT
# endif
#else
# define UIGROUPMANAGER_EXPORT
#endif
