//
// Created by lihaibin on 2019-06-27.
//

#ifndef QTALK_V2_SENDPRODUCTWND_H
#define QTALK_V2_SENDPRODUCTWND_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QFrame>
#include <QTextEdit>
#include "../CustomUi/UShadowWnd.h"

class SendProductWnd : public UShadowDialog {
    Q_OBJECT
public:
    explicit SendProductWnd(QWidget* parent = nullptr);
    ~SendProductWnd() override;

public:
    void clear();

Q_SIGNALS:
    void sendJsonSig(const QString&);

private:
    QTextEdit* _pEidt;

};

#endif //QTALK_V2_SENDPRODUCTWND_H
