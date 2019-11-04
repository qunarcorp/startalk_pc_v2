//
// Created by QITMAC000260 on 2019-05-22.
//

#include "EmoticonView.h"
#include <QFileInfo>
#include <QPainter>
#include "../UICom/qimage/qimage.h"
#include "../UICom/StyleDefine.h"

EmoticonView::EmoticonView(QWidget* parent)
    :QFrame(parent), _mov(nullptr), _width(0)
{
    setFixedSize(120, 120);
    setFrameShape(QFrame::NoFrame);
    Qt::WindowFlags flags =  Qt::WindowDoesNotAcceptFocus | Qt::WindowContextHelpButtonHint | Qt::FramelessWindowHint
                             | Qt::WindowFullscreenButtonHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint |
                             Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint;

#if defined(_LINUX)
    flags |= Qt::Window;
#else
    flags |= Qt::Tool;
#endif
    setWindowFlags(flags);
    // 设置不抢主窗口焦点
    setAttribute(Qt::WA_X11DoNotAcceptFocus, true);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

EmoticonView::~EmoticonView() = default;

void EmoticonView::paintEvent(QPaintEvent *e)
{
    const int dpi = QTalk::qimage::instance().dpi();

    QPixmap pix;
    if(nullptr != _mov)
    {
        pix = _mov->currentPixmap();
        if(!pix.isNull())
            pix = QTalk::qimage::instance().scaledPixmap(pix, _width * dpi, _width * dpi);
    }
    if(pix.isNull())
        pix = QTalk::qimage::instance().loadPixmap(_imagePath, false, true, _width * dpi, _width * dpi);

    int w = pix.width() / dpi;
    int h = pix.height() / dpi;

    QPainter painter(this);
    QPainterPath path;
    QRect rect(0, 0, this->width(), this->height());
    path.addRoundedRect(rect, 6, 6);
    painter.fillPath(path, QTalk::StyleDefine::instance().getNavNormalColor());
	QPen pen;
	pen.setColor(QColor(181, 181, 181));
	pen.setWidthF(1.5);
    painter.setPen(pen);
    painter.drawRoundedRect(rect, 6, 6);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap((width() - w) / 2, (height() - h) / 2, w, h, pix);

    QFrame::paintEvent(e);
}

void EmoticonView::setImagePath(const QString &imgPath)
{
    _imagePath = imgPath;
    QString suffix = QTalk::qimage::instance().getRealImageSuffix(_imagePath);
    if(suffix.toUpper() == "GIF")
    {
        if(nullptr == _mov)
            _mov = new QMovie;
        else
            _mov->stop();

        _mov->setFileName(imgPath);
        _mov->setSpeed(80);
        _mov->start();

        connect(_mov, &QMovie::frameChanged, [this](int){
            update();
        });
    }
    else
    {
        if(nullptr != _mov)
        {
            _mov->stop();
            delete _mov;
            _mov = nullptr;
        }
    }

    QPixmap img = QTalk::qimage::instance().loadPixmap(_imagePath, true);
    _width = qMin((int)img.height(), (int)img.width());
    _width = qMin(100, _width);

    update();
}
