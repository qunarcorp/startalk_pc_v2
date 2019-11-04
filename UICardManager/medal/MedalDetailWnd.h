//
// Created by QITMAC000260 on 2019/10/21.
//

#ifndef QTALK_V2_MEDALDETAILWND_H
#define QTALK_V2_MEDALDETAILWND_H

#include "../../CustomUi/UShadowWnd.h"
#include <QFrame>
#include "UserMedalItem.h"
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <QSet>
#include <QPushButton>
#include <QMutexLocker>
#include "UserHeadWgt.h"

/**
* @description: MedalDetail
* @author: cc
* @create: 2019-10-21 20:15
**/
class MedalTip : public QFrame {
public:
    explicit MedalTip(QWidget* parent)
        :QFrame(parent){
        _font.setPixelSize(14);
    }

public:
    void showTips(const QString& strTip)
    {
        _tips = strTip.split("\n");
        QFontMetricsF mf(_font);
        auto lineHeight = (mf.height() + 4);
        auto width = this->width() - 100;
        int rowCount = 0;
        for(const auto& tip : _tips)
        {
            int textWidth = mf.width(tip);
            int row = textWidth / width;
            if(textWidth % width > 0)
                row += 1;
            rowCount += row;
        }
        this->setFixedHeight(lineHeight * rowCount );
        update();
    }

protected:
    void paintEvent(QPaintEvent* e) override
    {
        QPainter painter(this);
        painter.setFont(_font);
        QFontMetricsF mf(_font);

        auto lineHeight = (mf.height() + 4);
        auto width = this->width() - 100;
        int rowIndex = 0;
        for(const auto& tip : _tips)
        {
            int textWidth = mf.width(tip);
            int row = textWidth / width;
            if(textWidth % width > 0)
                row += 1;

            painter.fillRect(42,  (lineHeight - 4) / 2 + rowIndex * lineHeight, 4, 4, QColor(0,202,190));
            QRect textRect(50, lineHeight * rowIndex, this->width() - 100, lineHeight * row);

            QTextOption option(Qt::AlignLeft | Qt::AlignTop);
            option.setWrapMode(QTextOption::WordWrap);
            painter.drawText(textRect, tip, option);
            rowIndex += row;
        }
        QFrame::paintEvent(e);
    }

private:
    QStringList _tips;
    QFont       _font;
};

class MedalDetailWnd : public QFrame{
    Q_OBJECT
public:
    explicit MedalDetailWnd(QWidget* parent = nullptr);
    ~MedalDetailWnd() override ;

public:
    void showMedalDetail(const QTalk::Entity::ImMedalList& medalInfo, bool isSelf, int status);
    inline QWidget* getMoveWgt() { return _topFrm;};
    void setMedalUsers(int id, const std::vector<QTalk::StMedalUser>& metalUsers);
    void onModifySuccess(int id, bool isWear);

Q_SIGNALS:
    void sgShowBack();
    void sgUpdateUsers();
    void sgShowUserList(const std::vector<QTalk::StMedalUser>&);
    void sgModifyStatus(int, bool);

private:
    void onUpdateUsers();

private:
    QLabel* _pTitleLabel = nullptr;
    QFrame *_topFrm = nullptr;
    UserMedalItem* _pImageView;
    MedalTip*      _pTipWgt;
    QPushButton* wearBtn;
    QPushButton* unloadBtn;
    QLabel*      wearTipLabel;
    QLabel*      usersCountLabel;
    UserHeadWgt* moreBtn;

private:
    QMutex _mutex;
    int _medalId = -1;
    std::vector<QTalk::StMedalUser> _metalUsers;

    QHBoxLayout * _userLay;
    QSet<UserHeadWgt*> _userHeadWnds;
};


#endif //QTALK_V2_MEDALDETAILWND_H
