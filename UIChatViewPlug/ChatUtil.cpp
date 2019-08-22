//
// Created by QITMAC000260 on 2019-07-15.
//
#include "ChatUtil.h"
#include "../UICom/qimage/qimage.h"

namespace QTalk
{
    namespace Image {

        void scaImageSize(qreal &width, qreal &height) {

            if(width == 0 || height == 0)
            {
                width = height = 200;
            }

            double factor = 1.0;
            const double maxsize = 200;
            double factor1 = maxsize / width;
            double factor2 = maxsize / height;
            if (factor1 < 1.0 || factor2 < 1.0) {
                factor = qMin(factor1, factor2);
            }

            width = factor * width;
            height = factor * height;
        }

        void scaImageSizeByPath(const QString &imagePath, qreal &width, qreal &height)
        {
            QPixmap pix = qimage::instance().loadPixmap(imagePath, false);
            width = pix.width();
            height = pix.height();

            scaImageSize(width, height);
        }

    }
}
