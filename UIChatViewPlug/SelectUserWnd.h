//
// Created by QITMAC000260 on 2019-04-12.
//

#ifndef QTALK_V2_SELECTUSERWND_H
#define QTALK_V2_SELECTUSERWND_H

#include "../CustomUi/UShadowWnd.h"
#include "../CustomUi/SearchEdit.hpp"
#include "../include/CommonStrcut.h"
#include "../entity/UID.h"
#include <QListView>
#include <QStandardItemModel>
#include <set>
#include <QStyledItemDelegate>
#include <QMutexLocker>
#include <QSortFilterProxyModel>
#include "../include/STLazyQueue.h"

/**
* @description: SelectUserWnd
* @author: cc
* @create: 2019-04-12 10:09
**/
enum data_type{
    EM_DATATYPE_CHATTYPE = Qt::UserRole + 1,
    EM_DATATYPE_XMPPID,
    EM_DATATYPE_REALJID,
    EM_DATATYPE_NAME,
    EM_DATATYPE_HEAD,
    EM_DATATYPE_CHECK,
    EM_DATATYPE_SEARCHKEY
};

class SelectUserWndSort : public QSortFilterProxyModel
{
public:
    explicit SelectUserWndSort(QObject *parent = nullptr);
    ~SelectUserWndSort() override = default;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override ;
};

class SelectUserWndDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SelectUserWndDelegate(QWidget *parent);
    ~SelectUserWndDelegate() override = default;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void itemCheckChanged(const QModelIndex& index);
    void itemDbClicked(const QModelIndex& index);

private:
    QFont _nameFont;
};

class SelectUserWnd : public UShadowDialog {
    Q_OBJECT
public:
    explicit SelectUserWnd(QWidget* parent = nullptr);
    ~SelectUserWnd() override;

public:
    void reset();
    void setSessions(int type, const std::vector<QTalk::StShareSession>& sessions);

private:
    void initUi();
    void updateUi();
    void onItemCheckChanged(const QModelIndex& index);
    void onItemDbClicked(const QModelIndex& index);

private slots:
    void onBtnGroupClicked(int);

protected:
    void closeEvent(QCloseEvent* e) override ;

signals:
    void sgUpdateUi();

private:
    Search_Edit* _pSearchEdit;
    QListView*   _pListView;
    SelectUserWndSort*  _pSortModel{};
    QStandardItemModel* _pModel;

    QButtonGroup* _pBtnGroup;

public:
    QEventLoop* _loop;
    std::map<QTalk::Entity::UID, int> _selectedIds;

public:
    int _type{};
    enum {
        EM_TYPE_RECENT,
        EM_TYPE_CONTACTS,
        EM_TYPE_GROUPCHAT,
    };

private:
    QMutex _mutex;
    std::vector<QTalk::StShareSession> recentSessions;
    std::vector<QTalk::StShareSession> contactsSessions;
    std::vector<QTalk::StShareSession> groupSessions;

private:
    STLazyQueue<std::string> *_searchQueue{};
};


#endif //QTALK_V2_SELECTUSERWND_H
