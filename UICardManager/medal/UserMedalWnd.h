//
// Created by QITMAC000260 on 2019/10/21.
//

#ifndef QTALK_V2_USERMEDALWND_H
#define QTALK_V2_USERMEDALWND_H

#include "../../CustomUi/UShadowWnd.h"
#include <vector>
#include <set>
#include <QTableWidget>
#include <QPainter>
#include "UserMedalItem.h"

/**
* @description: UserMedalWnd
* @author: cc
* @create: 2019-10-21 11:25
**/

struct StMedalInfo {
    QTalk::Entity::ImMedalList im_medal;
    int medal_status = 0;

    bool operator<(const StMedalInfo& other) const
    {
        if(this->im_medal.medalId == other.im_medal.medalId)
            return false;

        if(this->medal_status == other.medal_status)
            return this->im_medal.medalId < other.im_medal.medalId;
        else
            return this->medal_status > other.medal_status;
    }
};

class UserMedalTip : public QFrame {
    Q_OBJECT
public:
    explicit UserMedalTip(int count, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent * e) override;

private:
    int _user_medal_size = 0;
};


class UserMedalWnd : public QFrame {
    Q_OBJECT
public:
    explicit UserMedalWnd(const QString& userName,
            const QString& headPath,
            const std::set<QTalk::StUserMedal>& user_medals,
            QWidget* parent);
    ~UserMedalWnd() override;

public:
    inline QWidget* getMoveWgt() { return _topFrm;};

public:
    void init_medals(const std::set<QTalk::StUserMedal>& user_medals);

Q_SIGNALS:
    void sgShowMedalDetail(int);

private:
    QTableWidget* _pMainWgt;
    QFrame* _topFrm;
};


#endif //QTALK_V2_USERMEDALWND_H
