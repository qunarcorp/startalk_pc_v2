//
// Created by cc on 18-12-24.
//

#include "SessionBtn.h"
#include <QPaintEvent>
#include <QPainter>
#include "../Platform/Platform.h"
#include "../Platform/dbPlatForm.h"
#include <QEvent>
#ifdef _MACOS
#include <QtMacExtras>
#endif

SessionBtn::SessionBtn(QWidget *parnet)
    : QToolButton(parnet), _unreadCount(0)
{
    //setUnreadCount(dbPlatForm::instance().getAllUnreadCount());
//    connect(this, &SessionBtn::clicked, this, &SessionBtn::setUnreadCount);
}

SessionBtn::~SessionBtn()
{

}

/**
 * 设置未度数
 * @param count
 */
void SessionBtn::setUnreadCount(int count)
{
   _unreadCount = count;
#ifdef _MACOS
   if(count > 0)
       QtMac::setBadgeLabelText(QString::number(count));
   else
       QtMac::setBadgeLabelText("");
#endif
   update();
}

/**
 *
 * @param e
 */
void SessionBtn::paintEvent(QPaintEvent *e) {

    QRect rect =  e->rect();
    QPainter painter(this);
    bool isClicked = this->isChecked();
//    isClicked |= _isPressed;
	painter.setRenderHints(QPainter::Antialiasing, true);
	painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
	QString iconPath = QString(isClicked ?
	        ":/title/image1/sessionBtn_checked_%1.png" :
	        ":/title/image1/sessionBtn_normal_%1.png").arg(AppSetting::instance().getThemeMode());
    QPixmap pixmap(iconPath);
    painter.drawPixmap(0, 0, 30, 30, pixmap);

    if(_unreadCount <= 0)
    {

    }
    else
    {
        QPen pen(QColor(255, 108, 86));
        painter.setPen(pen);
        painter.setBrush(QBrush(QColor(255, 75, 62)));
        QFont font(this->font());
        font.setPixelSize(10);
        QFontMetricsF f(font);
        int unWidth = qMax((int)f.width(QString::number(_unreadCount)) + 8, 20);
        QRect unreadRect(rect.x() + 14, rect.y(), unWidth, 14);
        painter.drawRoundRect(unreadRect, 80, 80);
        //
        QPen textpen(Qt::white);
        painter.setFont(font);
        painter.setPen(textpen);
        painter.drawText(unreadRect,  Qt::AlignCenter, QString::number(_unreadCount));
    }
}

bool SessionBtn::event(QEvent *e) {

    if(e->type() == QEvent::MouseButtonPress)
    {
        _isPressed = true;
        update();
    }
    else if(e->type() == QEvent::Leave)
    {
        if(_isPressed)
            _isPressed = false;
        update();
    }

    return QToolButton::event(e);
}
