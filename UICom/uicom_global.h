#ifndef UICOM_GLOBAL_H
#define UICOM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UICOM_LIBRARY)
#  define UICOMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define UICOMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UICOM_GLOBAL_H
