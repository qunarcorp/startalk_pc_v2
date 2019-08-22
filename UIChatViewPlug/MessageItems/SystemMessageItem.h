//
// Created by lihaibin on 2019-06-12.
//

#ifndef QTALK_V2_SYSTEMMESSAGEITEM_H
#define QTALK_V2_SYSTEMMESSAGEITEM_H

#include <QWebEngineView>
#include "MessageItemBase.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"

class SystemMessageItem : public MessageItemBase
{
Q_OBJECT
public:
    explicit SystemMessageItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                               QWidget *parent = nullptr);

    ~SystemMessageItem() override;

public:
    QSize itemWdtSize() override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;

private:
    QLabel *titleLabel;
    QLabel *contentLabel;
    QLabel *promptLabel;

    QSize _headPixSize;
    QMargins _mainMargin;
    QMargins _leftMargin;
    QMargins _rightMargin;
    QSize _contentSize;
    int _mainSpacing;

    int _leftSpacing;
    int _rightSpacing;
    int _nameLabHeight;

    void init();
    void initLayout();
    void initSendLayout();
    void initReceiveLayout();
    void initContentLayout();
    void loadUrl(const QTalk::Entity::ImMessageInfo& msgInfo);
};

#endif //QTALK_V2_SYSTEMMESSAGEITEM_H
