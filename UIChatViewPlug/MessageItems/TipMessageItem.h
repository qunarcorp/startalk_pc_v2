//
// Created by cc on 18-11-9.
//

#ifndef QTALK_V2_TIPMESSAGEITEM_H
#define QTALK_V2_TIPMESSAGEITEM_H


#include "MessageItemBase.h"
#include <QLabel>
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"

class QLabel;
class TipMessageItem : public QFrame//: public MessageItemBase
{
	Q_OBJECT
public:
    TipMessageItem(QWidget *parent = Q_NULLPTR);
    ~TipMessageItem();

public:
    void setText(const QString& text);
    QString getText();

protected:
    void resizeEvent(QResizeEvent *e);

protected slots:
    void openURL(QString url);

private:
    QString _strText;
    QLabel* _pLabel;
};


#endif //QTALK_V2_TIPMESSAGEITEM_H
