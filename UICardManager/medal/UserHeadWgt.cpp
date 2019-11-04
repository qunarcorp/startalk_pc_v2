//
// Created by QITMAC000260 on 2019/10/22.
//

#include "UserHeadWgt.h"
#include "../../Platform/Platform.h"
#include "../../UICom/qimage/qimage.h"
#include <QFileInfo>
#include <QPainter>
#include <QRegExp>
#include <QDebug>

UserHeadWgt::UserHeadWgt(QString name, const QString &headSrc,
        bool onlyShowText,
        QWidget *parent)
    :QFrame(parent), _name(std::move(name)), _onlyShowText(onlyShowText)
{
    if(!onlyShowText)
    {
        QString tmp = QTalk::GetHeadPathByUrl(headSrc.toStdString()).data();
        QFileInfo info(tmp);
        if(info.exists() && info.isFile())
            _headSrc = tmp;

        this->setToolTip(_name);
        QRegExp regExp(".+([^\\x00-\\xff])[a-z|A-Z]*");
        if ((regExp.indexIn(_name , 0)) != -1)
        {
            _name = regExp.cap(1);
        }
        else
            _name = name[0];
    }
}

//
void UserHeadWgt::paintEvent(QPaintEvent *e) {

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    QRect contentRect = this->contentsRect();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(244,244,244));
    painter.drawEllipse(contentRect);
    if(_onlyShowText)
    {
        QFont f;
        f.setPixelSize(12);
//        f.setBold(true);
        painter.setFont(f);
        painter.setPen(QColor(102,102,102));
        painter.drawText(contentRect, Qt::AlignCenter, _name);
    }
    else {
        if(_headSrc.isEmpty())
        {
#ifdef _STARTALK
            _headSrc = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
            _headSrc = ":/QTalk/image1/headPortrait.png";
#endif
        }
//    else
        QPainterPath path;
        path.addEllipse(contentRect);
        painter.setClipPath(path);
        auto image = QTalk::qimage::instance().loadPixmap(_headSrc, false);
        painter.drawPixmap(contentRect, image);
    }

    QFrame::paintEvent(e);
}

//
void UserHeadWgt::mousePressEvent(QMouseEvent *e) {
    emit clicked();
    QWidget::mousePressEvent(e);
}
