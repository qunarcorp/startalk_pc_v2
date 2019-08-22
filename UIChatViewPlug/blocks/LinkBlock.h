//
// Created by cc on 19-2-19.
//

#ifndef QTALK_V2_LINKBLOCK_H
#define QTALK_V2_LINKBLOCK_H

#include <QObject>
#include <QTextObjectInterface>
#include <QPainter>
#include "../../UICom/qimage/qimage.h"

enum {
    linkPropertyLink = QTextFormat::UserProperty + 1,
    linkPropertyFont,
    linkPropertyWidth
};

//
class LinkBlock : public QObject, public QTextObjectInterface
{
	Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
public:
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                         const QTextFormat &format) override {
        Q_UNUSED(posInDocument);
        Q_UNUSED(doc);

        qreal width = format.property(linkPropertyWidth).toDouble();
        QString link = format.property(linkPropertyLink).toString();
        QFont font = format.property(linkPropertyFont).value<QFont>();

        QFontMetricsF f(font);
        int lineNum = (f.width(link) / width) + 1;
        qreal height = (f.height() + 2) * lineNum;
        return QSize(width, height);
    }

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                    int posInDocument, const QTextFormat &format) override {

        QString link = format.property(linkPropertyLink).toString();
        QFont font = format.property(linkPropertyFont).value<QFont>();
        painter->save();
        painter->setFont(font);
        painter->setPen(QColor("#20bcd2"));
        painter->drawText(QRect(rect.x(), rect.y() + 3, rect.width() + 10, rect.height() + 20) , link);
        painter->restore();
    }
};

#endif //QTALK_V2_LINKBLOCK_H
