#include "NavigationItem.h"
#include "OAManagerPanel.h"
#include "../CustomUi/NavigationItem.h"

#include "../UICom/qimage/qimage.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include "../UICom/StyleDefine.h"

using namespace QTalk;
OANavigationItem::OANavigationItem(const int& id, QString name, QString icon, OAManagerPanel *mainPanel)
	: QFrame(mainPanel), _id(id),
	_mainPanel(mainPanel),
	_selected(false),
	_itemWgt(nullptr),
	_name(std::move(name)),
	_icon(std::move(icon))
{
    setFixedHeight(50);
	setObjectName("NavigationItem");
}

OANavigationItem::~OANavigationItem()
= default;

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/15
  */
void OANavigationItem::registerWgt(QWidget * wgt)
{
	_itemWgt = wgt;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/15
  */
void OANavigationItem::setSelectState(bool selected)
{
	_selected = selected;
	update();
}

void OANavigationItem::mousePressEvent(QMouseEvent *e)
{
	//setSelectState(true);
	emit itemClicked(_id);
	QFrame::mousePressEvent(e);
}

void OANavigationItem::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);

	painter.fillRect(e->rect(), QBrush(_selected ? StyleDefine::instance().getAdrSelectColor() : StyleDefine::instance().getAdrNormalColor()));
	QPixmap pixmap = QTalk::qimage::instance().loadCirclePixmap(_icon, 14 * qimage::instance().dpi());
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap( 20, 12, 24, 24, pixmap);
    //
    painter.setPen(QPen(StyleDefine::instance().getAdrNameFontColor()));
    painter.drawText(55, 0, width() - 55, height(), Qt::AlignVCenter, _name);
	QFrame::paintEvent(e);
}
