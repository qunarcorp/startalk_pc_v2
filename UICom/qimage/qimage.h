//
// Created by cc on 19-1-16.
//

#ifndef QTALK_V2_QIMAGE_H
#define QTALK_V2_QIMAGE_H

#include <QObject>
#include <QMap>
#include <QPixmap>
#include <QMutexLocker>
#include "../uicom_global.h"

namespace QTalk {

    class UICOMSHARED_EXPORT qimage {

	private:
		static qimage* _qimage;

	public:
		static qimage& instance();

    public:
        qimage();
        ~qimage();

    public:
        QPixmap loadPixmap(const QString& srcPath, bool save, bool scaled = false, int width = 0, int height = 0);
        QPixmap loadCirclePixmap(const QString &srcPath, const int& radius, bool isGrey = false);
        QPixmap generateGreyPixmap(const QPixmap &src);
        QString  getRealImageSuffix(const QString &filePath);
        QString getRealImagePath(const QString& filePath);
        QPixmap scaledPixmap(const QPixmap& src, int width, int height = 0, bool model = false);
        QPixmap generatePixmap(const QPixmap &src, const int &radius);
        int dpi();

    private:
        QMap<QString, QPixmap> _allPixmap;

    private:
        QMutex _mutex;
        int    _dpi;
    };
}


#endif //QTALK_V2_QIMAGE_H
