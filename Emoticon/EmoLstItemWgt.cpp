#include <utility>

#include <utility>

#include <utility>

#include "EmoLstItemWgt.h"
#include "../UICom/StyleDefine.h"
#include <QPainter>
#include <QPixmap>
#include <QMenu>

EmoLstItemWgt::EmoLstItemWgt(QString  pkgId, QString  iconPath, QString  emoName)
	:QFrame(), _pgkId(std::move(pkgId)), _icon(std::move(iconPath)), _name(std::move(emoName)), _isChecked(false)
{

}

EmoLstItemWgt::~EmoLstItemWgt()
{

}

QString EmoLstItemWgt::getPkgId()
{
	return _pgkId;
}

QString EmoLstItemWgt::getIconPath()
{
	return _icon;
}

void EmoLstItemWgt::setCheckState(bool check)
{
	if (_isChecked == check) return;
	_isChecked = check;
	update();
}

void EmoLstItemWgt::paintEvent(QPaintEvent *e)
{
	//
	QPainter painter;
	painter.begin(this);
	QPixmap pic(_icon);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
	painter.drawPixmap(QRect(12, 10, 30, 30), pic);

	//
	QFont font;
	font.setBold(_isChecked);
    font.setPixelSize(13);

	QPen pen;
	if (_isChecked)
	{
		//
		pen.setColor(QTalk::StyleDefine::instance().getNavNameFontColor());
		//painter.setBrush(QBrush(checkCor));
		//painter.drawRect(QRect(0, 0, 5, height()));
	}
	else
		pen.setColor(QTalk::StyleDefine::instance().getNavContentFontColor());
	painter.setFont(font);
	painter.setPen(pen);
	//
	QFontMetricsF f(font);
    painter.drawText(QRect(54, 18, f.width(_name) + 5, height()), _name);
	painter.end();
	//
	return QFrame::paintEvent(e);
}

