//
// Created by QITMAC000260 on 2018/11/17.
//

#include "PicItem.h"
#include <QPainter>
#include <cmath>
#include <iostream>
#include <QMovie>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>

PicItem::PicItem(int &scaleVal)
        : _proportion(1), _pMovie(nullptr), _scaleVal(scaleVal) {

}

PicItem::~PicItem() {
    if (nullptr != _pMovie) {
        _pMovie->stop();
        delete _pMovie;
    }
}

QRectF PicItem::boundingRect() const {
    if (!_pixmap.isNull()) {

        qreal width = _pixmap.width() * _proportion;
        qreal height = _pixmap.height() * _proportion;
        return {-width / 2, -height / 2, width, height};

    }

    return {};
}

void PicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (!_pixmap.isNull() && nullptr != _pMovie) {
        QMutexLocker locker(&_mutex);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        qreal width = _pixmap.width() * _proportion;
        qreal height = _pixmap.height() * _proportion;

        if(_pMovie->isValid())
        {
            painter->drawPixmap(static_cast<int>(-width / 2), static_cast<int>(-height / 2), static_cast<int>(width),
                                static_cast<int>(height), _pMovie->currentPixmap());
        } else {
            painter->drawPixmap(static_cast<int>(-width / 2), static_cast<int>(-height / 2), static_cast<int>(width),
                                static_cast<int>(height), _pixmap
                                );
        }


    }
}

// 设置新图片
void PicItem::setPixmap(QPixmap &pixmap, const QString &path, qreal proportion) {
    QMutexLocker locker(&_mutex);
    if (nullptr != _pMovie) {
        _pMovie->stop();
        delete _pMovie;
    }
    _pMovie = new QMovie(path);
    _pMovie->start();
    QMovie::connect(_pMovie, &QMovie::frameChanged, [this](int) {
        this->update();
    });
    _pixmap = pixmap;
    _proportion = proportion;
}

// 滚轮事件
void PicItem::wheelEvent(QGraphicsSceneWheelEvent *e) {
    if (e->delta() > 0)
        _scaleVal++;
    else
        _scaleVal--;
    //
    onScaleChange(_scaleVal, QPoint(0, 0));
    //
    e->accept();
    //QGraphicsItem::wheelEvent(e);
}

// 图片缩放
void PicItem::onScaleChange(int scaleVal, const QPoint &p) {

    if (scaleVal > maxScale) {
        _scaleVal = maxScale;
        return;
    }
    if (scaleVal < -maxScale) {
        _scaleVal = -maxScale;
        return;
    }

    qreal multiple;
    if (scaleVal > 0)
        multiple = pow(1.1, scaleVal);
    else
        multiple = pow(1 / 1.1, -scaleVal);

    setTransformOriginPoint(p);
    setScale(multiple);
}
