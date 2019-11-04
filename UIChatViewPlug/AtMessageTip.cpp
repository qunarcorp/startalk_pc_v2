//
// Created by cc on 19-1-9.
//

#include "AtMessageTip.h"
#include "ChatMainWgt.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPointer>
#include <QtWidgets/QGraphicsDropShadowEffect>

AtMessageTip::AtMessageTip(ChatMainWgt *chatMainWgt)
    : QFrame(chatMainWgt), _pChatMainWgt(chatMainWgt)
{
    this->setObjectName("AtMessageTip");
    _pNameLabel = new QLabel;
    _pAtLabel = new QLabel;
    _pNameLabel->setObjectName("AtMessageTipNameLabel");
    _pAtLabel->setObjectName("AtMessageTipAtLabel");
    auto * layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->addWidget(_pNameLabel);
    layout->addWidget(_pAtLabel);
}

AtMessageTip::~AtMessageTip() {

}

/**
 * 增加At
 * @param info
 */
void AtMessageTip::addAt(const StShowAtInfo &info)
{
    //
    _arAtItems.append(info);
    showAtInfo();
    //
    if(!this->isVisible() && _pChatMainWgt && !_arAtItems.empty())
    {
        // 显示
        this->setVisible(true);
        // 移动
        
    }
}

/**
 *
 * @param e
 */
void AtMessageTip::mousePressEvent(QMouseEvent *e)
{
    StShowAtInfo info = _arAtItems.last();
    _arAtItems.pop_back();
    //
    if(info.atItem)
        _pChatMainWgt->scrollToItem(info.atItem, QAbstractItemView::PositionAtBottom);

	if (!_arAtItems.empty())
	{
        showAtInfo();
	}
    else
        this->setVisible(false);
    //
    QWidget::mousePressEvent(e);
}

/**
 *
 * @param info
 */
void AtMessageTip::showAtInfo()
{
    if(!_arAtItems.empty() )
    {
        StShowAtInfo info = _arAtItems.last();
        _pNameLabel->setText(info.name);

        if(info.isAtAll)
            _pAtLabel->setText(tr("@全体成员"));
        else
            _pAtLabel->setText(tr("@了我"));

        QFont font = this->font();
        font.setPixelSize(14);
        QFontMetricsF f(font);
        qreal w = f.width(info.name) + f.width(_pAtLabel->text());
        this->setFixedWidth(w + 20);
        this->move(_pChatMainWgt->width() - this->width(), 10);
    }
}
