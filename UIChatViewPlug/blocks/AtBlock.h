//
// Created by QITMAC000260 on 2018/11/28.
//

#ifndef QTALK_V2_ATBLOCK_H
#define QTALK_V2_ATBLOCK_H

#include <QObject>
#include <QTextObjectInterface>
#include <QPainter>
#include "../../UICom/StyleDefine.h"

enum {
    atPropertyText = QTextFormat::UserProperty + 1,
    atPropertyFont,
    atPropertyXmppId,
    atPropertyName
};


// 用于显示@消息的块
class AtBlock : public QObject, public QTextObjectInterface
{
	Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
public:
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                                 const QTextFormat &format) override {
		Q_UNUSED(posInDocument);
		Q_UNUSED(doc);

        QString text = format.property(atPropertyText).toString();
        QFont font = format.property(atPropertyFont).value<QFont>();
        QFontMetricsF f(font);
        return {f.width(text), f.height()};
    }

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                            int posInDocument, const QTextFormat &format) override {
        QString text = format.property(atPropertyText).toString();
        painter->save();
        painter->setPen(QTalk::StyleDefine::instance().getAtBlockFontColor());
        painter->drawText(QRect(rect.x(), rect.y() + 3, rect.width(), rect.height()), Qt::AlignBottom | Qt::AlignLeft,text);
        painter->restore();
    }
};


#endif //QTALK_V2_ATBLOCK_H
