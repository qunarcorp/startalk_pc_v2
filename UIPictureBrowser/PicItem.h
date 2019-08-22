//
// Created by QITMAC000260 on 2018/11/17.
//

#ifndef QTALK_V2_PICITEM_H
#define QTALK_V2_PICITEM_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>
#include <QPointF>
#include <QDragEnterEvent>
#include <QGraphicsSceneWheelEvent>
#include <QMutexLocker>


class PicItem : public QGraphicsItem {
public:
    explicit PicItem(int &scaleVal);

    ~PicItem() override;

protected:
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void wheelEvent(QGraphicsSceneWheelEvent *e) override;

public:
    void setPixmap(QPixmap &pixmap, const QString &path, qreal);

    void onScaleChange(int scaleVal, const QPoint &p);

protected:
    QPixmap _pixmap;
    QMovie *_pMovie;
    qreal _proportion;

private:
    QMutex _mutex;
    int &_scaleVal;
    const int maxScale = 50;
};


#endif //QTALK_V2_PICITEM_H
