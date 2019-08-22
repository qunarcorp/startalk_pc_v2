//
// Created by QITMAC000260 on 2019-01-27.
//

#ifndef QTALK_V2_QTMESSAGEBOX_H
#define QTALK_V2_QTMESSAGEBOX_H

#include "UShadowWnd.h"
#include "customui_global.h"

/**
* @description: MessageBox
* @author: cc
* @create: 2019-01-27 19:40
**/
class CUSTOMUISHARED_EXPORT QtMessageBox : public UShadowDialog {
public:
    QtMessageBox(QWidget* parent, int type, const QString& message, const QString& subMessage, int buttons);
    ~QtMessageBox() override;

public:
    enum
    {
        EM_LEVEL_INVALID,
        EM_LEVEL_SUCCESS,
        EM_LEVEL_INFO,
        EM_LEVEL_QUESTION,
        EM_LEVEL_WARNING,
        EM_LEVEL_ERROR,
    };

    enum
    {
        EM_BUTTON_INVALID = 0,
        EM_BUTTON_YES = 1,
        EM_BUTTON_NO = 2,
        EM_BUTTON_DELETE = 4
    };

public:
    static int information(QWidget* parent, const QString& message, const QString& subMessage, int buttons = EM_BUTTON_YES);
    static int success(QWidget* parent, const QString& message, const QString& subMessage, int buttons = EM_BUTTON_YES);
    static int question(QWidget* parent, const QString& message, const QString& subMessage, int buttons = EM_BUTTON_YES | EM_BUTTON_NO);
    static int warning(QWidget* parent, const QString& message, const QString& subMessage, int buttons = EM_BUTTON_YES);
    static int error(QWidget* parent, const QString& message, const QString& subMessage, int buttons = EM_BUTTON_YES);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private:
    int _retButton;
    QEventLoop* _evtLoop;

private:
    QLabel *mainMessageLabel;
    QLabel *subMessageLabel;
};


#endif //QTALK_V2_QTMESSAGEBOX_H
