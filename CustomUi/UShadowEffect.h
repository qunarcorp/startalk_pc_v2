#ifndef USHADOWEFFECT_H
#define USHADOWEFFECT_H

#include <QGraphicsDropShadowEffect>
#include <QObject>
#include "customui_global.h"
class CUSTOMUISHARED_EXPORT UShadowEffect : public QGraphicsEffect
{
    Q_OBJECT
public:
    explicit  UShadowEffect(QObject *parent = Q_NULLPTR);
    void draw(QPainter* painter);
    QRectF boundingRectFor(const QRectF& rect) const;
    inline void setDistance(qreal distance)
    {
        _distance = distance;
        updateBoundingRect();
    }
    inline qreal distance() const
    {
        return _distance;
    }
    inline void setBlurRadius(qreal blurRadius)
    {
        _blurRadius = blurRadius;
        updateBoundingRect();
    }
    inline qreal blurRadius() const
    {
        return _blurRadius;
    }
    inline void setColor(const QColor& color)
    {
        _color = color;
    }
    inline QColor color() const
    {
        return _color;
    }
private:
    qreal  _distance;
    qreal  _blurRadius;
    QColor _color;

};

#endif // USHADOWEFFECT_H
