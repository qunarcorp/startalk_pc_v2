//
// Created by QITMAC000260 on 2019-04-16.
//

#include "ScanMainFrm.h"

ScanMainFrm::ScanMainFrm(QWidget* parnet)
    : QFrame(parnet), animation(nullptr)
{
    _pTimer = new QTimer(this);
    _pTimer->setInterval(2000);

    label = new PixmapLabel(this);
    label->setFixedSize(260, 4);
    label->setVisible(false);

    connect(_pTimer, &QTimer::timeout, [this](){
        animation->start();
    });
}

ScanMainFrm::~ScanMainFrm() = default;

void ScanMainFrm::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    //
    int w = this->width();
    int h = this->height();
    int cw = w - 100;
    int ch = h - 100;
    p.setCompositionMode( QPainter::CompositionMode_Clear );
    p.fillRect(50, 50, cw, ch, Qt::SolidPattern );

    p.setCompositionMode( QPainter::CompositionMode_SourceOver );
    QBrush b1(QColor(0, 202, 190));
    p.fillRect(47, 47, 25, 5, b1);
    p.fillRect(47, 47, 5, 25, b1);

    p.fillRect(w - 73, 47, 25, 5, b1);
    p.fillRect(w - 52, 47, 5, 25, b1);

    p.fillRect(47, h - 52, 25, 5, b1);
    p.fillRect(47, h - 73, 5, 25, b1);

    p.fillRect(w - 73, h - 52, 25, 5, b1);
    p.fillRect(w - 52, h - 73, 5, 25, b1);

    //
//    QRect rect = contentsRect();
//    static short index = 0;
//    const int count = 30;
//    QRect pixRect(50, rect.y() + 50 + ch / count * index, cw, 5);
//    p.drawPixmap(pixRect, QPixmap(":/chatview/image1/scan.png"));
//    if(index++ >= count)
//        index = 0;
    //
    QFrame::paintEvent(e);
}

void ScanMainFrm::start()
{
    if(nullptr == animation)
    {
        adjustSize();
        animation = new QPropertyAnimation(label, "pos");
        animation->setDuration(3000);
        animation->setStartValue(QPoint(50, 50));
        animation->setEndValue(QPoint(50, height() - 60));
        animation->setEasingCurve(QEasingCurve::Linear);
    }

    if(!_pTimer->isActive())
        _pTimer->start();

    label->setVisible(true);
    animation->start();
}

void ScanMainFrm::stop() {
    if(_pTimer && _pTimer->isActive())
        _pTimer->stop();

    if(label)
        label->setVisible(false);
    if(animation)
        animation->stop();
}
