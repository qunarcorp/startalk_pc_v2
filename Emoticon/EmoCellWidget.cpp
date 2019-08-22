#include <utility>
#include <QPainter>
#include "EmoCellWidget.h"
#include "../UICom/qimage/qimage.h"
#include <QPixmap>
#include <QFileInfo>

EmoCellWidget::EmoCellWidget(QString emoPath, QString pkgId, QString shortCut, QWidget *parent)
	: QLabel(parent), _strPkgId(std::move(pkgId)),
	_strEmoShortCut(std::move(shortCut)),
	_imagePath(std::move(emoPath))
{
	setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    QFileInfo fileInfo(_imagePath);
    if(fileInfo.exists()) {
        QPixmap img = QTalk::qimage::instance().loadPixmap(_imagePath, true);
        _width = qMin((int)img.height(), (int)img.width());
        _width = qMin(45, _width);
    }
}

EmoCellWidget::~EmoCellWidget()
{

}

bool EmoCellWidget::event(QEvent *event) {
	if(event->type() == QEvent::Leave)
	{
//        emit sgHidePreviewWnd();
	}
	else if(event->type() == QEvent::ToolTip)
	{
        emit sgPreviewImage(_imagePath);
	}
	return QLabel::event(event);
}

void EmoCellWidget::paintEvent(QPaintEvent *e) {

    const int dpi = QTalk::qimage::instance().dpi();

    QPixmap pix = QTalk::qimage::instance().loadPixmap(_imagePath, false, true, _width * dpi, _width * dpi);

    int w = pix.width() / dpi;
    int h = pix.height() / dpi;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap((width() - w) / 2, (height() - h) / 2, w, h, pix);
    QLabel::paintEvent(e);
}
