//
// Created by lihaibin on 2019-07-03.
//

#ifndef QTALK_V2_QUICKREPLYWND_H
#define QTALK_V2_QUICKREPLYWND_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "../CustomUi/UShadowWnd.h"
#include <QListWidget>
#include <vector>

class QuickReplyWnd : public UShadowDialog{
Q_OBJECT
public:
    explicit QuickReplyWnd(QWidget* parent = nullptr);
    ~QuickReplyWnd() override;

public:
    void addTagItem();
    void addContentItem(int id);

Q_SIGNALS:
    void sendQuickReply(std::string);

private slots:
    void singleclicked(QListWidgetItem* item);
    void doubleclicked(QListWidgetItem* item);

private:
    QListWidget *_tagListWidget;
    QListWidget *_contentListWidget;
};

#endif //QTALK_V2_QUICKREPLYWND_H
