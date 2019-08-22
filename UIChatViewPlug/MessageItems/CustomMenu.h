#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <QDialog>
#include <QFrame>
#include <QMenu>
#include "../../CustomUi/UShadowWnd.h"


class CustomMenu : public UShadowDialog
{
    Q_OBJECT
public:
    explicit CustomMenu(QWidget *parent = 0);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void init();
    void initMenu();
    void initLayout();

private:
    QFrame * _mainFrm;
    QMenu * _menu;
    
};

#endif // CUSTOMMENU_H
