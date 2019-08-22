#include "CustomMenu.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>

CustomMenu::CustomMenu(QWidget *parent) :
    UShadowDialog(parent, true)
{
    Qt::WindowFlags flags = this->windowFlags();
    this->setWindowFlags(flags);
    init();
    this->setFixedSize(94, 84);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.22
  */
void CustomMenu::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    qDebug() << "mousePressEvent pos" <<  pos;
    QRect rect = this->realContentsRect();
    qDebug() << "mousePressEvent rect" <<  rect;
    if (!this->realContentsRect().contains(pos))
    {
        this->close();
    }
    UShadowDialog::mousePressEvent(event);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.22
  */
void CustomMenu::focusOutEvent(QFocusEvent *event)
{
    this->close();
    UShadowDialog::focusOutEvent(event);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.22
  */
void CustomMenu::wheelEvent(QWheelEvent *event)
{
    this->close();
    UShadowDialog::wheelEvent(event);
}

void CustomMenu::init()
{
    initLayout();
}

void CustomMenu::initMenu()
{

}

void CustomMenu::initLayout()
{
    QHBoxLayout *glay = new QHBoxLayout(_pCenternWgt);
    glay->setMargin(0);
    glay->setSpacing(0);
    _mainFrm = new QFrame(this);
    _mainFrm->setObjectName("menuMainFrm");
    glay->addWidget(_mainFrm);
    QString strStyle = QString("QFrame#menuMainFrm"
                       "{"
                        "background:rgba(255,255,255,1);"
                       "border-radius:6px;"
                       "}");
    _mainFrm->setStyleSheet(strStyle);
//    _mainFrm->setFixedSize(84, 74);

//    QGraphicsDropShadowEffect *defaultShadow = new QGraphicsDropShadowEffect();
//    defaultShadow->setBlurRadius(10.0);
//    defaultShadow->setColor(QColor(0, 0, 0, 160));
//    defaultShadow->setOffset(0, 0);
//    _mainFrm->setGraphicsEffect(defaultShadow);
    QHBoxLayout *mainFrmlay = new QHBoxLayout;
    mainFrmlay->setMargin(0);
    mainFrmlay->setSpacing(0);
    glay->addLayout(mainFrmlay);
//    _menu = new QMenu;
//    _menu->addAction(tr("另存为"));
//    _menu->addAction(tr("收藏"));
//    mainFrmlay->addWidget(_menu);
}
