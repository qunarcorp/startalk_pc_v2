#include "EmoIconWgt.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "EmoticonMainWgt.h"
#include "../UICom/StyleDefine.h"

#define DEM_MAXICON_COUNT 6 

EmoIcon::EmoIcon(const QString& iconPath)
	:_icon(iconPath), _isChecked(false)
{
	setFixedSize(30, 30);
	setFrameShape(QFrame::NoFrame);
}

EmoIcon::~EmoIcon()
{

}

QString EmoIcon::getIconPath()
{
	return _icon;
}

void EmoIcon::setCheckState(bool check)
{
	if (_isChecked == check) return;
	_isChecked = check;
	update();
}

void EmoIcon::paintEvent(QPaintEvent *e)
{
	QPainter painter;
	painter.begin(this);
	painter.setPen(QPen(QColor(Qt::transparent)));
	if (_isChecked)
		painter.setBrush(QBrush(QTalk::StyleDefine::instance().getEmoSelelctIconColor()));
	else
		painter.setBrush(QBrush(QColor(Qt::transparent)));

	painter.drawRoundedRect(QRect(0, 0, 30, 30), 2, 2);

	QPixmap pic(_icon);
	painter.drawPixmap(QRect(4, 4, 22, 22), pic);


	painter.end();
	return QFrame::paintEvent(e);
}

void EmoIcon::mousePressEvent(QMouseEvent *e)
{
	emit clicked();
	return QFrame::mousePressEvent(e);
}

/***************************/
EmoIconRow::EmoIconRow(QWidget* parent)
	:QFrame(parent), _iconCount(0), _pFirstEmo(nullptr)
{
	_pLayout = new QHBoxLayout(this);
	_pLayout->setMargin(0);
	_pLayout->setSpacing(15);
	_pLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	setLayout(_pLayout);
}


EmoIconRow::~EmoIconRow()
{

}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
void EmoIconRow::addEmoIcon(EmoIcon* icon, const QString& id)
{
	if (_iconCount < DEM_MAXICON_COUNT)
	{
		if (_iconCount == 0)
		{
			_pFirstEmo = icon;
		}

		_mapEmoId[id] = icon;
		_pLayout->insertWidget(_iconCount, icon);
		_iconCount++;
	}
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/18
  */
int EmoIconRow::getCount()
{
	return _iconCount;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/28
  */
void EmoIconRow::removeIcon(const QString& pkgId)
{
	if (hasEmoIcon(pkgId))
	{
		EmoIcon *icon = getEmoIcon(pkgId);
		if (icon)
		{
			delete icon;
			icon = nullptr;
			_mapEmoId.remove(pkgId);
			_iconCount--;
		}
	}
}

void EmoIconRow::resetState()
{
	if (_pFirstEmo)
	{
		emit _pFirstEmo->clicked();
	}
}

//**************************************
EmoIconWgt::EmoIconWgt(QWidget *parent)
	: QStackedWidget(parent)
{
	
}

EmoIconWgt::~EmoIconWgt()
{

}

//
EmoIcon* EmoIconWgt::addEmoIcon(const QString& iconPath, const QString& id, const QString& name)
{
	int count = this->count();
	EmoIconRow* wgt = nullptr;
	if (count != 0)
	{
		wgt = (EmoIconRow*)this->widget(count - 1);
	}
	
	if (!wgt || wgt->getCount() >= DEM_MAXICON_COUNT)
	{
		wgt = new EmoIconRow(this);
		_arEmoIconRow.push_back(wgt);
		this->addWidget(wgt);
	}

	if (wgt)
	{
		EmoIcon* btn = new EmoIcon(iconPath);
		btn->setToolTip(name);
		btn->setFocusPolicy(Qt::NoFocus);
		wgt->addEmoIcon(btn, id);
		return btn;
	}

	return nullptr;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/28
  */
void EmoIconWgt::deleteEmoIcon(const QString& pkgId)
{
	auto itFind = std::find_if(_arEmoIconRow.begin(), _arEmoIconRow.end(), [pkgId](EmoIconRow *row)
	{
		return row->hasEmoIcon(pkgId);
	});

	if (itFind != _arEmoIconRow.end())
	{
		EmoIconRow *row = *itFind;
		row->removeIcon(pkgId);
		if (row->getCount() == 0)
		{
			_arEmoIconRow.erase(itFind);
			this->removeWidget(row);
			delete row;
		}
		if (this->count() > 0)
		{
			row = (EmoIconRow*)this->widget(0);
			if (row)
			{
				row->resetState();
			}
			setCurrentIndex(0);
		}
	}
}
