#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CARDMANAGER_LIB)
#  define CARDMANAGER_EXPORT Q_DECL_EXPORT
# else
#  define CARDMANAGER_EXPORT Q_DECL_IMPORT
# endif
#else
# define CARDMANAGER_EXPORT
#endif
