//
// Created by cc on 19-1-9.
//

#include "NewMessageTip.h"
#include <QHBoxLayout>
#include <QListWidget>
#include "../CustomUi/HeadPhotoLab.h"

#define TIP_MODEL QStringLiteral("%1 条新消息")

NewMessageTip::NewMessageTip(QWidget *parent)
    : QFrame(parent), _parentWgt(parent), _newMessageCount(0)
{
    this->setFixedHeight(40);
    this->setMinimumWidth(120);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setObjectName("NewMessageTip");
    //
    HeadPhotoLab* picLabel = new HeadPhotoLab;
    picLabel->setParent(this);
    picLabel->setHead(":/chatview/image1/go.png", 10, false, false, true);
    picLabel->setFixedWidth(20);
    _pLabel = new QLabel(QString(TIP_MODEL).arg(_newMessageCount));
    _pLabel->setObjectName("NewMessageTipLabel");
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->addWidget(picLabel);
    lay->addWidget(_pLabel);
}

NewMessageTip::~NewMessageTip() {

}

/**
 *
 */
void NewMessageTip::onNewMessage() {
    _newMessageCount++;
    if(!this->isVisible())
    {
       this->setVisible(true);
        int w = (_parentWgt->width() - this->width()) / 2;
        this->move(w, _parentWgt->height() - 50);
    }
    //
    _pLabel->setText(QString(TIP_MODEL).arg(_newMessageCount));
}

/*
 *
 */
void NewMessageTip::mousePressEvent(QMouseEvent *e) {
    onResetWnd();
    QWidget::mousePressEvent(e);
}

/**
 *
 */
void NewMessageTip::onResetWnd() {
    this->setVisible(false);
    _newMessageCount = 0;
    _pLabel->setText(QString(TIP_MODEL).arg(_newMessageCount));
    QListWidget* parentWgt = qobject_cast<QListWidget*>(_parentWgt);
    if(parentWgt)
    {
        parentWgt->scrollToBottom();
    }
}

void NewMessageTip::onResize()
{
	if (this->isVisible())
	{
		int w = (_parentWgt->width() - this->width()) / 2;
		this->move(w, _parentWgt->height() - 50);
	}
}
