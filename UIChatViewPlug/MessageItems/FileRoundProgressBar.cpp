#include "FileRoundProgressBar.h"
#include "../../UICom/StyleDefine.h"
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>

FileRoundProgressBar::FileRoundProgressBar(QWidget *parent) :
    QFrame(parent, Qt::FramelessWindowHint),
    _currentValue(0)
{
    init();
    setMouseTracking(true);
}

FileRoundProgressBar::~FileRoundProgressBar()
{

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.19
  */
void FileRoundProgressBar::init()
{
//       setFixedSize(30, 30);
       setAutoFillBackground(false);
       QPalette pal = palette();
       pal.setColor(QPalette::Background, QColor(0xFF,0xFF,0xFF,0xFF));
       setPalette(pal);
       _barCloseColor = QColor(133, 133, 133);
}


/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.22
  */
void FileRoundProgressBar::setCurValue(const int &curValue)
{
    if (_currentValue != curValue)
    {
        _currentValue = curValue;
        update();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.21
  */
void FileRoundProgressBar::mousePressEvent(QMouseEvent *event)
{
    if (_colsePressRect.contains(event->pos()))
    {
        _barCloseColor = QColor(0, 195, 188);
    }
    else
    {
        _barCloseColor = QColor(133, 133, 133);
    }
    this->update();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.21
  */
void FileRoundProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (_colsePressRect.contains(event->pos()))
    {
        _barCloseColor = QColor(0, 195, 188);
    }
    else
    {
        _barCloseColor = QColor(133, 133, 133);
    }
    emit sgCloseDownLoadingClicked();
    this->update();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.21
  */
void FileRoundProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    if (_colsePressRect.contains(event->pos()))
    {
        _barCloseColor = QColor(0, 195, 188);
    }
    else
    {
        _barCloseColor = QColor(133, 133, 133);
    }
    this->update();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.19
  */
void FileRoundProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    int barRadius = qMin(width() / 2, height() / 2 );
    int barWidth = 3;
    int barMargin = 2;
    QRectF outRect = QRectF(barMargin, barMargin, 2*barRadius - 2*barMargin,  2*barRadius - 2*barMargin);
    QRectF inRect = QRectF(barMargin + barWidth, barMargin + barWidth, 2*(barRadius - barWidth) - 2*barMargin, 2*(barRadius - barWidth) - 2*barMargin);
//    _closePressPath.addEllipse(inRect.adjusted(2, 2, -2, -2));

    _colsePressRect = inRect.adjusted(barWidth, barWidth, -barWidth, -barWidth);
//    _closePressPlg = _closePressPath.toFillPolygon();
//    qDebug() << "close  " << _closePressPlg.isClosed();

    double degrees = _currentValue * double(360.0 / 100.0);

    QPainter painter(this);
    painter.setPen(QColor(0,0,0));
    painter.drawPath(_closePressPath);

    painter.save();//保存移动坐标前painter设置

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.setBrush(QTalk::StyleDefine::instance().getFileProcessBarLine());
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(outRect);

    QPainterPath dataPath;
    dataPath.setFillRule(Qt::WindingFill);

    dataPath.moveTo(outRect.center());
    dataPath.arcTo(outRect, 90, -degrees);
    dataPath.lineTo(outRect.center());
    painter.setBrush(QColor(0,195,188));
    painter.setPen(Qt::NoPen);
    painter.drawPath(dataPath);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QTalk::StyleDefine::instance().getFileProcessBarBackground());
    painter.drawEllipse(inRect);

    // 绘制关闭叉叉
//    painter.setPen(QPen(_barCloseColor, 2));
//    int sp = 6;
//    QPointF topLeft = QPointF(inRect.topLeft().x() + sp, inRect.topLeft().y() + sp);
//    QPointF bottomRight = QPointF(inRect.bottomRight().x() - sp, inRect.bottomRight().y() - sp);
//    QPointF topRight = QPointF(inRect.topRight().x() - sp, inRect.topRight().y() + sp);
//    QPointF bottomLeft = QPointF(inRect.bottomLeft().x() + sp, inRect.bottomLeft().y() - sp);
//    painter.drawLine(topLeft, bottomRight);
//    painter.drawLine(topRight, bottomLeft);

    painter.restore();//还原移动坐标前painter设置

//    QPixmap pixmap;
//    pixmap.load("D:/qttest/build-FileRoundProgressBar-Desktop_Qt_5_9_6_MSVC2017_64bit-Debug/debug/close_normal.png");
//    if (pixmap.isNull())
//    {
//        qDebug() << "close_normal.png";
//    }
//    painter.drawPixmap(QRect(0, 0, 30, 30), pixmap);
}
