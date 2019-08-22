#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(UIOAMANAGER_LIB)
#  define UIOAMANAGER_EXPORT Q_DECL_EXPORT
# else
#  define UIOAMANAGER_EXPORT Q_DECL_IMPORT
# endif
#else
# define UIOAMANAGER_EXPORT
#endif
