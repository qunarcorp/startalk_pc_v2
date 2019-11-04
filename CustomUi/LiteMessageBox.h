//
// Created by QITMAC000260 on 2019-01-27.
//

#ifndef QTALK_V2_LITEMESSAGEBOX_H
#define QTALK_V2_LITEMESSAGEBOX_H

#include <QFrame>
#include "customui_global.h"

/**
* @description: LiteMessageBox
* @author: cc
* @create: 2019-01-27 22:25
**/
class CUSTOMUISHARED_EXPORT LiteMessageBox : public QFrame{
public:
    LiteMessageBox(int type, const QString& message, QWidget* base);

    ~LiteMessageBox() override;

public:
    enum
    {
        EM_TYPE_INVALID,
        EM_TYPE_SUCCESS,
        EM_TYPE_FAILED,
    };

public:
    static void success(const QString& message, int duration = 3000, QWidget* = nullptr);
    static void failed(const QString& message, int duration = 3000, QWidget* = nullptr);
};


#endif //QTALK_V2_LITEMESSAGEBOX_H
