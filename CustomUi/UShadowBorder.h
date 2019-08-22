//
// Created by QITMAC000260 on 2019-01-27.
//

#ifndef QTALK_V2_USHADOWBORDER_H
#define QTALK_V2_USHADOWBORDER_H

#include <QLabel>

enum
{
    EM_LEFT,
    EM_LEFTTOP,
    EM_TOP,
    EM_RIGHTTOP,
    EM_RIGHT,
    EM_RIGHTBOTTOM,
    EM_BOTTOM,
    EM_LEFTBOTTOM,
};
#define DEM_FIXSIZE 15
/**
* @description: 阴影画面border
* @author: cc
* @create: 2019-01-27 15:27
**/
class UShadowDialog;
class UShadowBorder : public QFrame {
public:
    UShadowBorder(int borderType, bool isRidus, UShadowDialog* parent);
    ~UShadowBorder() override;

protected:
    bool event(QEvent* e) override ;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent* e) override ;

private:
    int _type;

private:
    UShadowDialog* _pControlWgt;

    bool  _borderPressed;   //
    QPoint _resizeStartPos; //
};


#endif //QTALK_V2_USHADOWBORDER_H
