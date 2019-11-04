//
// Created by QITMAC000260 on 2019/10/21.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef QTALK_V2_USERMEDALITEM_H
#define QTALK_V2_USERMEDALITEM_H

#include <QFrame>
#include "../../entity/im_medal_list.h"
#include "../../include/CommonStrcut.h"


/**
* @description: UserMedalItem
* @author: cc
* @create: 2019-10-21 19:56
**/
class UserMedalItem : public QFrame {
    Q_OBJECT
public:
    explicit UserMedalItem(QWidget* parent = nullptr);

public:
    void setUserMedal(const QTalk::Entity::ImMedalList& medalInfo,
                      int status,
                      bool isDetail);

    void setComingSoon();

protected:
    void paintEvent(QPaintEvent * e) override;

public:
    int _medalId = -1;

private:
    QString _imagePath;
    QString _text;
    bool    _isDetail;
};

#endif //QTALK_V2_USERMEDALITEM_H
