//
// Created by QITMAC000260 on 2019-02-19.
//

#ifndef QTALK_V2_IMAGEBLOCK_H
#define QTALK_V2_IMAGEBLOCK_H

#include <QObject>
#include <QTextObjectInterface>
#include <QPainter>
#include <QMovie>
#include <QFileInfo>
#include "../../UICom/qimage/qimage.h"
#include "../../QtUtil/Utils/Log.h"
#include "block_define.h"

//
class ImageBlock : public QObject, public QTextObjectInterface {
Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
public:
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                         const QTextFormat &format) override {
        Q_UNUSED(posInDocument);
        Q_UNUSED(doc);

        qreal width = format.property(imagePropertyWidth).toDouble();
        qreal height = format.property(imagePropertyHeight).toDouble();
        return {width, height};
    }

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                    int posInDocument, const QTextFormat &format) override {

        QString imagePath = format.property(imagePropertyPath).toString();
        qreal width = format.property(imagePropertyWidth).toDouble();
        qreal height = format.property(imagePropertyHeight).toDouble();
        painter->save();
        painter->setRenderHints(QPainter::Antialiasing, true);
        painter->setRenderHints(QPainter::SmoothPixmapTransform, true);


        QPixmap image = _image;
        if (!inited) {
            if (imagePath.isEmpty() || !QFile::exists(imagePath)) {
                //warn_log("load head failed, use default picture-> imagePath:{0}, realMessage:{0}", imagePath);

                imagePath = ":/chatview/image1/defaultImage.png";
                image = QTalk::qimage::instance().loadPixmap(imagePath, true, true, 80, 80);
            } else {

                image = QPixmap(imagePath);
                if (image.isNull()) {
                    imagePath = ":/chatview/image1/defaultImage.png";
                    image = QTalk::qimage::instance().loadPixmap(imagePath, true, true, 80, 80);
                } else {
                    image = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
            }
            inited = true;
            _image = image;
        }

        painter->drawPixmap(QRect(rect.x(), rect.y() + 3, width, height), image);
        painter->restore();
    }

private:
    bool inited = false;
    QPixmap _image;
};

#endif //QTALK_V2_IMAGEBLOCK_H
