#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#ifdef _WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif
#include "UShadowWnd.h"
#include "../QtUtil/Utils/Log.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QtGui>
#include <QGraphicsDropShadowEffect>
#include "../UICom/uicom.h"

UShadowDialog::UShadowDialog(QWidget *parent, bool radius, bool hasBorder) :
    QDialog(parent),
	_evtRet(0),
#ifdef _MACOS
    _hasBorder(false),
#else
    _hasBorder(hasBorder),
#endif
    _moveAble(false),
    _isResizing(false),
    _movePressed(false),
    _pMoveContentWgt(nullptr)
{
    Qt::WindowFlags flags = Qt::Window | Qt::WindowContextHelpButtonHint | Qt::FramelessWindowHint
#ifdef _WINDOWS
                           | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowSystemMenuHint
#endif
                           | Qt::WindowFullscreenButtonHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint;
    this->setWindowFlags(flags);
    //setAttribute(Qt::WA_AlwaysShowToolTips, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
//    setAttribute(Qt::WA_MacNoShadow, true);
    setAttribute(Qt::WA_QuitOnClose, false);
    _pCenternWgt = new QWidget(this);
    auto * lay = new QHBoxLayout(this);
#ifdef _MACOS
    lay->setMargin(0);
#else
    lay->setMargin(2);
#endif
    lay->setSpacing(0);

    lay->addWidget(_pCenternWgt);
	//
	setSizeGripEnabled(true);

	setObjectName("UShadowWnd");
}

UShadowDialog::~UShadowDialog()
= default;

//
void UShadowDialog::mousePressEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();
    // 移动
    if(!_isResizing && _moveAble && _pMoveContentWgt)
    {
		if ((_pMoveContentWgt == this && realContentsRect().contains(pos)) ||
			(_pMoveContentWgt->geometry().contains(pos)))
		{
			_movePressed = true;
			_moveStartPos = pos;
		}
    }

    QWidget::mousePressEvent(e);
}

void UShadowDialog::mouseMoveEvent(QMouseEvent *e)
{
    QPoint pos = e->pos();

    // 移动
    if(_moveAble && _movePressed)
    {
        QPoint movePos = pos - _moveStartPos;
        this->move(mapToGlobal(movePos));
    }

    QWidget::mouseMoveEvent(e);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/13
  */
void UShadowDialog::closeEvent(QCloseEvent * e)
{
	emit closeSignal();
	e->accept();
}

void UShadowDialog::mouseReleaseEvent(QMouseEvent *e)
{
    // 移动
    if(_moveAble && _movePressed)
    {
        _movePressed = false;
        e->accept();
        return;
    }

    QWidget::mouseReleaseEvent(e);
}

/**
 *
 * @param e
 */
void UShadowDialog::keyPressEvent(QKeyEvent *e)
{
   if(e->key() == Qt::Key_Escape || e->key() == Qt::Key_Space)
   {
       this->close();
   }

#ifdef _MACOS
   if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_W)
   {
       this->setVisible(false);
   }
#endif
    QDialog::keyPressEvent(e);
}

bool UShadowDialog::resizable() const
{
	return false;
}

void UShadowDialog::setResizable(bool resizable)
{

}

int UShadowDialog::showModel() {

//    Qt::NonModal 不阻塞
//    Qt::WindowModal 阻塞父窗口，所有祖先窗口及其子窗口
//    Qt::ApplicationModal 阻塞整个应用程序

    this->setWindowModality(Qt::ApplicationModal);
	this->show();
	//_evtLoop = new QEventLoop(this);
	//_evtLoop->exec();

	return _evtRet;
}

int UShadowDialog::showCenter(bool model, QWidget* parent) {

    QDesktopWidget *deskTop = QApplication::desktop();
	QWidget *ActiveWnd = UICom::getInstance()->getAcltiveMainWnd();
    int curMonitor = deskTop->screenNumber(nullptr == parent ? ActiveWnd : parent);
    QRect deskRect = deskTop->screenGeometry(curMonitor);
    move((deskRect.width() - width()) / 2 + deskRect.x(), (deskRect.height() - height()) / 2 + deskRect.y()
#ifdef _MACOS
        - 50
#endif
    );

    if(model)
    {
        showModel();
    } else {
        show();
    }

    return _evtRet;
}

/**
 * 设置可移动
 * @param moveAble
 */
void UShadowDialog::setMoverAble(bool moveAble, QWidget *wgt) {

    _moveAble = moveAble;
    _pMoveContentWgt = wgt == nullptr ? this : wgt;
}

QRect UShadowDialog::realContentsRect()
{
    return _pCenternWgt->geometry();
}

void UShadowDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::WindowStateChange && _hasBorder)
    {
		auto sts = this->windowState();
        if((sts & Qt::WindowMaximized) || (sts & Qt::WindowFullScreen) || (sts & Qt::WindowNoState))
            repaint();
    }

    QWidget::changeEvent(event);
}


void UShadowDialog::paintEvent(QPaintEvent* e) {

    if(_hasBorder)
    {
        QPainter painter(this);
		painter.setPen(QColor(197, 197, 197));
		QRect rect = _pCenternWgt->contentsRect();
		QRect content( 1, 1, rect.width() + 1, rect.height() + 1 );
		painter.drawRoundedRect(content, 6, 6);
    }
    QWidget::paintEvent(e);
}

#ifdef _MACOS
#include <objc/objc-runtime.h>
void UShadowDialog::macAdjustWindows()
{
    setWindowFlags(this->windowFlags() | Qt::Tool);
//    WId windowObject = this->winId();
//    auto * nsviewObject = reinterpret_cast<objc_object *>(windowObject);
//    objc_object * nsWindowObject = objc_msgSend(nsviewObject, sel_registerName("window"));
//    int NSWindowCollectionBehaviorCanJoinAllSpaces = 1 << 0;
//    objc_msgSend(nsWindowObject, sel_registerName("setCollectionBehavior:"), NSWindowCollectionBehaviorCanJoinAllSpaces);
}

#endif