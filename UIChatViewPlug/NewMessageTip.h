//
// Created by cc on 19-1-9.
//

#ifndef QTALK_V2_NEWMESSAGETIP_H
#define QTALK_V2_NEWMESSAGETIP_H

#include <QFrame>
#include <QLabel>

class NewMessageTip : public QFrame
{
public:
    NewMessageTip(QWidget* parent);
    ~NewMessageTip();

public:
    void onNewMessage();
    void onResetWnd();
	void onResize();

protected:
    virtual void mousePressEvent(QMouseEvent* e);

private:
    int _newMessageCount;

private:
    QLabel* _pLabel;

private:
    QWidget* _parentWgt;
};


#endif //QTALK_V2_NEWMESSAGETIP_H
