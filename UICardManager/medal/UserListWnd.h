//
// Created by QITMAC000260 on 2019/10/23.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_USERLISTWND_H
#define QTALK_V2_USERLISTWND_H

#include <QFrame>
#include <QListView>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QLabel>
#include "../../include/CommonStrcut.h"

enum {
    em_user_xmppId = Qt::UserRole + 1,
    em_user_name,
    em_user_head
};

/**
* @description: UserListWnd
* @author: cc
* @create: 2019-10-23 18:04
**/
class UserItemDelegate : public QStyledItemDelegate
{
public:
    explicit UserItemDelegate(QWidget* parent);

protected:
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};


class UserListWnd : public QFrame {
    Q_OBJECT
public:
    explicit UserListWnd(QWidget* parent = nullptr);

public:
    void showUserList(const std::vector<QTalk::StMedalUser>& userList);
    inline QWidget* getMoveWgt() { return _topFrm;};

Q_SIGNALS:
    void sgShowBack();

protected:
    QListView* _userView;
    QStandardItemModel* _model;

    QLabel* _pTitleLabel = nullptr;
    QFrame *_topFrm = nullptr;
};

#endif //QTALK_V2_USERLISTWND_H
