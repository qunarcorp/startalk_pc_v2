#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(_WINDOWS)
#  define UIADDRESSBOOK_EXPORT Q_DECL_EXPORT
# else
#  define UIADDRESSBOOK_EXPORT Q_DECL_IMPORT
# endif
#else
# define UIADDRESSBOOK_EXPORT
#endif
