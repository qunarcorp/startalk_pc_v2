//
// Created by QITMAC000260 on 2019-01-27.
//

#include <QtCore/QEvent>
#include "UShadowBorder.h"
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include "UShadowWnd.h"
#include "../QtUtil/Utils/Log.h"

#define DEM_STYLESHEET_1 "background: qlineargradient(x1:%1, y1:%2, x2:%3, y2:%4,stop:0 rgba(181,181,181,0.3), stop:0.5 rgba(181,181,181,0.1),stop:1 rgba(255,255,255,0));"
#define DEM_STYLESHEET_2 "background: qradialgradient(spread:pad, cx:%1, cy:%2, radius:1, fx:%3, fy:%4,stop:0 rgba(181,181,181,0.3), stop:0.5 rgba(181,181,181,0.1),stop:1 rgba(255,255,255,0));"

UShadowBorder::UShadowBorder(int borderType, bool isRidus, UShadowDialog* parent)
    : QFrame(parent)
    , _type(borderType)
    , _pControlWgt(parent)
    , _borderPressed(false)
{
    Q_INIT_RESOURCE(ushadowdialog);
    setMinimumSize(DEM_FIXSIZE, DEM_FIXSIZE);
    setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QString styleSheet;
    switch (borderType)
    {
        case EM_LEFT: 
        {
            setFixedWidth(DEM_FIXSIZE);
            break;
        }
        case EM_LEFTTOP: 
        {
            setFixedSize(DEM_FIXSIZE, DEM_FIXSIZE);
            break;
        }
        case EM_TOP: 
        {
            setFixedHeight(DEM_FIXSIZE);
            break;
        }
        case EM_RIGHTTOP: 
        {
            setFixedSize(DEM_FIXSIZE, DEM_FIXSIZE);
            break;
        }
        case EM_RIGHT: 
        {
            setFixedWidth(DEM_FIXSIZE);
            break;
        }
        case EM_RIGHTBOTTOM: 
        {
            setFixedSize(DEM_FIXSIZE, DEM_FIXSIZE);
            break;
        }
        case EM_BOTTOM: 
        {
            setFixedHeight(DEM_FIXSIZE);
            break;
        }
        case EM_LEFTBOTTOM: 
        {
//            styleSheet = QString(DEM_STYLESHEET_2).arg(1).arg(0).arg(1).arg(0);
            setFixedSize(DEM_FIXSIZE, DEM_FIXSIZE);
            break;
        }
        default:break;
    }

    setStyleSheet(styleSheet);
}

UShadowBorder::~UShadowBorder()
= default;

bool UShadowBorder::event(QEvent *e) {

    if(e->type() == QEvent::Enter)
    {
        switch (_type)
        {
            case EM_LEFT:
            case EM_RIGHT:
                setCursor(Qt::SizeHorCursor);
                break;
            case EM_TOP:
            case EM_BOTTOM:
                setCursor(Qt::SizeVerCursor);
                break;
            case EM_RIGHTBOTTOM:
                setCursor(Qt::SizeFDiagCursor);
                break;
            default:
                break;
        }
    }
    else if(e->type() == QEvent::Leave)
    {
        setCursor(Qt::ArrowCursor);
    }
    return QFrame::event(e);
}

void UShadowBorder::mousePressEvent(QMouseEvent *e) {
    QPoint pos = QCursor::pos();
    _borderPressed = true;
    _pControlWgt->setResizing(true);
    _resizeStartPos = QCursor::pos();
    QWidget::mousePressEvent(e);
}

void UShadowBorder::mouseReleaseEvent(QMouseEvent *e) {
    _borderPressed = false;
    _pControlWgt->setResizing(false);
    QWidget::mouseReleaseEvent(e);
}

void UShadowBorder::mouseMoveEvent(QMouseEvent *e) {
    //    // 伸缩
    if(_borderPressed)
    {
        QPoint pos = QCursor::pos();
        int x = pos.x() - _resizeStartPos.x();
        int y = pos.y() - _resizeStartPos.y();

        //if(abs(x) > 10 || abs(y) > 10)
        {
            QRect geo = _pControlWgt->geometry();
            switch (_type)
            {
                case EM_LEFT:
                {
                    _resizeStartPos = pos;
                    _pControlWgt->setGeometry(geo.x() + x, geo.y(), geo.width() - x, geo.height());
                    break;
                }
                case EM_TOP:
                {
                    _resizeStartPos = pos;
                    _pControlWgt->setGeometry(geo.x(), geo.y() + y, geo.width(), geo.height() - y);
                    break;
                }
                case EM_RIGHT:
                {
                    _resizeStartPos = pos;
                    _pControlWgt->setGeometry(geo.x(), geo.y(), geo.width() + x, geo.height());
                    break;
                }
                case EM_RIGHTBOTTOM:
                {
                    _resizeStartPos = pos;
                    _pControlWgt->setGeometry(geo.x(), geo.y(), geo.width() + x, geo.height() + y);
                    break;
                }
                case EM_BOTTOM:
                {
                    _resizeStartPos = pos;
                    _pControlWgt->setGeometry(geo.x(), geo.y(), geo.width(), geo.height() + y);
                    break;
                }
                default:
                    break;
            }
        }
    }
    QWidget::mouseMoveEvent(e);
}

void UShadowBorder::paintEvent(QPaintEvent *e) {

    QPainter painter(this);
    QRect rect = contentsRect();
    painter.setPen(Qt::NoPen);
    // painter.fillRect(rect, QColor(255,255,255,1));

    QFrame::paintEvent(e);
}
