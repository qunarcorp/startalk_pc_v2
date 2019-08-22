#include "SearchEdit.h"
#include "../QtUtil/Utils/Log.h"

#include <QMouseEvent>
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QTimer>

SearchEdit::SearchEdit(QWidget *parent) :
    QLineEdit(parent),
    _isEditing(false)
{
    setObjectName("SearchEdit_");

    setFocusPolicy(Qt::ClickFocus);
}

SearchEdit::~SearchEdit()
{
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void SearchEdit::mousePressEvent(QMouseEvent *event)
{

    if (!_isEditing)
    {
        _isEditing = true;
        emit sgIsOpenSearch(_isEditing);
    }

    QTimer::singleShot(50, [this](){
        QApplication::setActiveWindow(this);
        this->setFocus();
    });
    QLineEdit::mousePressEvent(event);
}

/**
 *
 */
void SearchEdit::keyPressEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_Up)
    {
        emit sgSelectUp();
        e->accept();
    }
    else if(e->key() == Qt::Key_Down)
    {
        emit sgSelectDown();
        e->accept();
    }
    else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        emit sgSelectItem();
    }
    else if(e->key() == Qt::Key_Escape)
    {
        emit sgKeyEsc();
        e->accept();
    }

    QLineEdit::keyPressEvent(e);
}
