#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(EMOTICON_LIB)
#  define EMOTICON_EXPORT Q_DECL_EXPORT
# else
#  define EMOTICON_EXPORT Q_DECL_IMPORT
# endif
#else
# define EMOTICON_EXPORT
#endif
