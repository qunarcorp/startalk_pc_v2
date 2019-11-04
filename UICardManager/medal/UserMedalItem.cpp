//
// Created by QITMAC000260 on 2019/10/21.
//

#include "UserMedalItem.h"
#include <QPainter>
#include <QFile>
#include "../../Platform/Platform.h"
#include "../../UICom/qimage/qimage.h"

// --------------
UserMedalItem::UserMedalItem(QWidget *parent)
        :QFrame(parent), _isDetail(false)
{

}

void UserMedalItem::setUserMedal(const QTalk::Entity::ImMedalList &medalInfo, int status, bool isDetail) {
    _isDetail = isDetail;
    _medalId = medalInfo.medalId;

    QString tip = QString::fromStdString(medalInfo.medalName);
    if(_isDetail)
    {
        _imagePath = QTalk::getMedalPath(medalInfo.bigLightIcon).data();
    }
    else
    {
        if((status & 0x02) > 0) {
            _imagePath = QTalk::getMedalPath(medalInfo.bigLightIcon).data();
            _text = tr("佩戴中");
        }
        else if((status & 0x01) > 0) {
            _imagePath = QTalk::getMedalPath(medalInfo.bigLightIcon).data();
            _text = tr("已解锁");
        }
        else {
            _imagePath = QTalk::getMedalPath(medalInfo.bigLockIcon).data();
            _text = QString::fromStdString(medalInfo.medalName);
        }
    }

    if(!QFile::exists(_imagePath))
        _imagePath = ":/CardManager/image1/MadelDefault.png";

    this->setToolTip(tip);

    if(_isDetail)
        this->setFixedSize(130, 130);
    else
        this->setFixedSize(110, 170);

}

void UserMedalItem::paintEvent(QPaintEvent * e)
{
    QPixmap pixmap = QTalk::qimage::instance().loadPixmap(_imagePath, false);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);

    auto rect = this->contentsRect();
    int maxW = rect.width(), maxH = rect.height();
    if(!_isDetail)
        maxH -= 40;
    double w = pixmap.width(), h = pixmap.height();
    double factor = 1.0;
    double factor1 = maxW / w;
    double factor2 = maxH / h;
    if (factor1 < 1.0 || factor2 < 1.0) {
        factor = qMin(factor1, factor2);
    }
    w = factor * w;
    h = factor * h;
    QRect imgRect((maxW - w) / 2 + 10, (maxH - h) / 2, w, h);
    painter.drawPixmap(imgRect, pixmap);

    //
    if(!_isDetail)
    {
        QFont font;
        font.setPixelSize(14);
        painter.setPen(QColor(153,153,153));
        painter.setFont(font);
        int fw = QFontMetricsF(font).width(_text) + 1;
        int fh = QFontMetricsF(font).height() + 1;
        QRect countRect((this->width() - fw) / 2, 110, fw, fh);
        painter.drawText(countRect, Qt::AlignHCenter, _text);
    }

    return QFrame::paintEvent(e);
}

void UserMedalItem::setComingSoon() {
    _imagePath = ":/CardManager/image1/ComingSoon.png";
    _text = tr("敬请期待");
    this->setFixedSize(110, 170);
}