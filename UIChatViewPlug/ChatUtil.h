//
// Created by QITMAC000260 on 2019-07-15.
//
#include <QObject>
#include <QPixmap>

namespace QTalk {

    namespace Image {
        void scaImageSize(qreal &width, qreal &height);
        void scaImageSizeByPath(const QString &imagePath, qreal &width, qreal &height);
    }
}