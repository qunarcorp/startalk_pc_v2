#include "SnipScreenTool.h"
#include "FullScreenWidget.h"
#include <QToolButton>
#include "QBoxLayout"
#include "QPushButton"
#include "QLabel"
#include "ActionCommand.h"
#include <qdebug.h>
#include <QApplication>
#include <QClipboard>
#include "floatTextEdit.h"
#include <QApplication>
#include <QDesktopWidget>
#include "SnipScreenToolBar.h"

SnipScreenTool* SnipScreenTool::pInstance = NULL;
SnipScreenTool* SnipScreenTool::getInstance()
{
    // 仅限ui调用，不用锁了
    if (NULL == pInstance)
    {
        pInstance = new SnipScreenTool(NULL);
    }
    return pInstance;
}


SnipScreenTool::SnipScreenTool(QObject *parent)
    : FullScreenWidget()
{
	auto remove = Qt::WindowTitleHint;
	auto add = Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint|Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint;
	setAttribute(Qt::WA_AlwaysShowToolTips, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	overrideWindowFlags( (Qt::WindowFlags)((windowFlags() & ~remove) | add));


    m_Actioning = NULL;
    _beEdit = false;
    m_currentType = drawNotSet;

    m_toolbar = new SnipScreenToolBar(this);
    _fontSize = 12;
    pEdit = new floatTextEdit(this);
    connect(m_toolbar,&SnipScreenToolBar::sgColorChange,this,&SnipScreenTool::onPenColorChanged);
    connect(m_toolbar,&SnipScreenToolBar::sgPenSizeChange,this,&SnipScreenTool::onPenSizeChanged);
    connect(m_toolbar,&SnipScreenToolBar::sgToolSelect,this,&SnipScreenTool::onToolBarTypeChanged);

    connect(m_toolbar,&SnipScreenToolBar::sgOk,[this]( ){
        onSaveContext();
        // 写入剪切板
        if(m_actionStatck.empty())
        {
            QApplication::clipboard()->setImage( selectPixmap().toImage()); 
        }else{
            QApplication::clipboard()->setImage( m_actionStatck.top()->CurrentPipxmap().toImage()); 
            //清栈
            while(!m_actionStatck.empty())
                m_actionStatck.pop();
        }            

		      

        hide(); 
        emit sgFinish(mConversionId);
    });
    connect(m_toolbar,&SnipScreenToolBar::sgCancle,[this](){
        //取消截图
        hide(); 
        emit sgCancel(mConversionId);
    });

    connect(m_toolbar,&SnipScreenToolBar::sgUndo,this,&SnipScreenTool::onUndo);
}

// 组件初始化，归到初始状态
SnipScreenTool& SnipScreenTool::Init()
{
    FullScreenWidget::reset();
    pEdit->InitWidget();
    pEdit->setVisible(false);
    m_Actioning = NULL;
    _beEdit = false;
    m_currentType = drawNotSet;
    while(!m_actionStatck.empty())m_actionStatck.pop();
    m_toolbar->setVisible(false);
    m_toolbar->initToolBar();
    return *this;
}

SnipScreenTool::~SnipScreenTool()
{
}

// 显示右键菜单
void SnipScreenTool::showPictureEditContextMenu(const QPoint& pos)
{
    _beEdit = true;
}

// 显示编辑框
void SnipScreenTool::showPictureEditBar(const QRect& rec)
{
	int nRatio = this->devicePixelRatio();
    QRect rect = getSelectedRect();
	QPoint ptRight = rect.bottomRight() / nRatio;
	QPoint ptxd = mapToGlobal(ptRight);
    int nToolbarBottom =  rect.bottomRight().y() / nRatio + 5;
    int screemBottom = QApplication::desktop()->availableGeometry(QCursor::pos()).bottom();
    bool b = nToolbarBottom + 80 > screemBottom;

    if (b)
    {
        nToolbarBottom = nToolbarBottom - m_toolbar->getHeight();
        m_toolbar->setDirect(0);
    }
    else
    {
        m_toolbar->setDirect(1);
    }


	//这地方需要判断屏幕的边界
    QDesktopWidget *deskTop = QApplication::desktop();
    QRect rectdesk = deskTop->availableGeometry(QCursor::pos());

    if (rectdesk.contains(ptxd))
    {
        if (ptxd.x() - rectdesk.left() > m_toolbar->width())
        {
            //这地方判断一下toolbar是在截图框的下面，还是上面
            if (rectdesk.bottom() - rect.bottom() / nRatio > m_toolbar->height())
                m_toolbar->move(rect.bottomRight().x() / nRatio - m_toolbar->width(), rect.bottomRight().y() / nRatio + 5);
            else
            {
                //这里还需要判断top的问题
                if (rect.top() / nRatio - rectdesk.top() > m_toolbar->height())
                    m_toolbar->move(rect.bottomRight().x() / nRatio - m_toolbar->width(), rect.top() / nRatio - 40);
                else
                    m_toolbar->move(rect.bottomRight().x() / nRatio - m_toolbar->width(), rect.bottom() / nRatio - 40);
            }
        }
        else
        {

            if (rectdesk.bottom() - rect.bottom() / nRatio > m_toolbar->height())
                m_toolbar->move(rect.x() / nRatio , nToolbarBottom);
            else
            {
                //这里还需要判断top的问题
                if (rect.top() / nRatio - rectdesk.top() > m_toolbar->height())
                    m_toolbar->move(rect.x() / nRatio , rect.top() / nRatio - 40);
                else
                    m_toolbar->move(rect.x() / nRatio , rect.bottom() / nRatio - 40);
            }
        }
    }
    else
    {

    }

    //m_toolbar->move(rect.bottomRight().x()/nRatio - m_toolbar->width(),nToolbarBottom);
    m_toolbar->show();
	m_toolbarrect = m_toolbar->geometry();
}

void SnipScreenTool::mousePressEvent(QMouseEvent * event)
{
	

    if (event->button() == Qt::MouseButton::LeftButton && m_currentType == drawNotSet && !_beEdit)
    {
        return FullScreenWidget::mousePressEvent( event );
    }
    // 按下即创建命令
    if (m_Actioning == nullptr)
    {
        m_Actioning = new ActionCommand(_brushColor,_fontSize,_burshSize);
		m_Actioning->setdeviceRadio(this->devicePixelRatio());
        m_Actioning->Type((ActionType)m_currentType);
        !m_actionStatck.empty()
            ? m_Actioning->LastPixmap(m_actionStatck.top()->CurrentPipxmap())
            : m_Actioning->LastPixmap(selectPixmap()); // 设置背景图片

        m_Actioning->setTargetRect(getSelectedRect()); // 设置绘制区域框
    }

    m_Actioning->mousePressEvent(event); // 坐绘制准备
    update();
}

void SnipScreenTool::mouseMoveEvent(QMouseEvent * event)
{
    // 没有点过编辑按钮
    if (!_beEdit && m_currentType == drawNotSet )
    {
        return FullScreenWidget::mouseMoveEvent(event);
    }

    if (m_Actioning!=NULL) 
    {
        m_Actioning->mouseMoveEvent(event);
        update();
    }
    event->accept();
}


void SnipScreenTool::mouseReleaseEvent(QMouseEvent * event )
{
	if (event->button() == Qt::RightButton)
	{
		 if(FullScreenWidget::JietuCancel( event))
		 {
			 emit m_toolbar->sgCancle();
		 }
	}

    if (event->button() == Qt::MouseButton::LeftButton )
    {
        // 没有点过编辑按钮
        if (!_beEdit && m_currentType == drawNotSet)
        {
           return  FullScreenWidget::mouseReleaseEvent(event);
        }

        // m_Actioning->mouseReleaseEvent 会返回是否是有效绘制
        if (m_Actioning!=NULL && m_Actioning->mouseReleaseEvent(event,this))
        {
            // 有效的前图压栈
            m_actionStatck.push(m_Actioning);
            m_Actioning = NULL;
            update();
        } 
        else
        {
           // m_Actioning = NULL;
        }
    }
}

void SnipScreenTool::mouseDoubleClickEvent(QMouseEvent * event)
{
	if(FullScreenWidget::JietuCancel(event))
	{
		emit m_toolbar->sgOk();
	}
	return FullScreenWidget::mouseDoubleClickEvent(event);
}


void SnipScreenTool::onForcus(QFocusEvent *eve){
    setCursor(Qt::ArrowCursor);
}

void SnipScreenTool::paintEvent(QPaintEvent *e)
{
    FullScreenWidget::paintEvent(e); // 间接会调用到 SnipScreenTool::drawSelectedPixmap

    if (m_Actioning != NULL)
    {
        m_Actioning->paintEvent(e,this,getPainter(),this);
    }
    
}

void SnipScreenTool::drawSelectedPixmap()
{
    if (m_actionStatck.size()==0)
    {
        return FullScreenWidget::drawSelectedPixmap();
    }

    // 画新图
    ActionCommand* pActionOnTopofStack = m_actionStatck.top();
    QPainter* p = getPainter();
	int nRatio = this->devicePixelRatio();
    QRect rect =  pActionOnTopofStack->TargetRect();
 	rect.setX(rect.x()/nRatio);
 	rect.setY(rect.y() / nRatio);
	if (nRatio != 1)
	{
		rect.setWidth((rect.width() - rect.x()) / nRatio);
		rect.setHeight((rect.height() - rect.y()) / nRatio);
	}
 
    p->drawRect(rect); //画选中的矩形框
    if(rect.width() > 0 && rect.height() > 0){
        p->drawPixmap(rect,pActionOnTopofStack->CurrentPipxmap()); //画选中区域的图片
    }
    draw8ControlPoint(rect);
}

void SnipScreenTool::onPenSizeChanged(int size)
{
    _burshSize = size;
}

void SnipScreenTool::onPenColorChanged(const QString& color)
{
    _brushColor = QColor(color);
    if(m_Actioning)
        m_Actioning->setColor(color);
}

void SnipScreenTool::onToolBarTypeChanged(unsigned char type)
{
    m_currentType = drawNotSet;
    switch (type)
    {
        case SnipScreenToolBar::TOOLTYPE::SNIP_TOOLBAR_RECT:
        case SnipScreenToolBar::TOOLTYPE::SNIP_TOOLBAR_RING:
        case SnipScreenToolBar::TOOLTYPE::SNIP_TOOLBAR_PEN:
        case SnipScreenToolBar::TOOLTYPE::SNIP_TOOLBAR_TEXT:
        case SnipScreenToolBar::TOOLTYPE::SNIP_TOOLBAR_ARROW:
            m_currentType = type;
            break;
        default:
            break;
    }

    onSaveContext();

    FullScreenWidget::LockSelectRange();
    _beEdit = true;
}

void SnipScreenTool::onSaveContext()
{
    if (m_Actioning!=NULL && m_Actioning->mouseReleaseEvent(NULL,this,true))
    {
        // 有效的前图压栈
        m_actionStatck.push(m_Actioning);
        m_Actioning = NULL;
        update();
    } 
}

/**
 * 
 */
void SnipScreenTool::setToolbarStyle(const QByteArray& qss)
{
	m_toolbar->setStyleSheet(qss);
}

void SnipScreenTool::onUndo()
{
    onSaveContext();

    if(!m_actionStatck.empty())
    {
         m_actionStatck.pop();
         update();
    }        
}

void SnipScreenTool::keyPressEvent(QKeyEvent *keyEvent)
{
    if (NULL!=keyEvent && keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Z)
    {
        onUndo();
    }
	else if (keyEvent->key() == Qt::Key_Escape) {
		initFullScreenWidget();
		hide();
		emit sgCancel(mConversionId);
	}
    return FullScreenWidget::keyPressEvent(keyEvent);

}

void SnipScreenTool::JietuFinshed()
{
	emit sgFinish(mConversionId);
}

void SnipScreenTool::JietuCancel()
{
	emit sgCancel(mConversionId);
}


