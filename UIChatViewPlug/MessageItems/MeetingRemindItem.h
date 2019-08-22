//
// Created by lihaibin on 2019-06-20.
//

#ifndef QTALK_V2_MEETINGREMINDITEM_H
#define QTALK_V2_MEETINGREMINDITEM_H

#include <QWebEngineView>
#include "MessageItemBase.h"

class MeetingRemindItem : public MessageItemBase
{
Q_OBJECT
public:
    explicit MeetingRemindItem(const QTalk::Entity::ImMessageInfo &msgInfo,
                               QWidget *parent = nullptr);

    ~MeetingRemindItem() override;

public:
    QSize itemWdtSize() override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;

private:
    QLabel *contentLabel;
    QLabel *clickLabel;

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

#endif //QTALK_V2_MEETINGREMINDITEM_H
