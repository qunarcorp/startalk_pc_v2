#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef SEARCHITEMDELEGATE_H
#define SEARCHITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "../include/CommonStrcut.h"

enum SearchItemRoles
{
    EM_TYPE_TYPE        = Qt::UserRole + 1, //item类型 对应存 SearchItemType
    EM_ITEMROLE_ITEM_TYPE, // QTalk::Search::Action
    EM_ITEMROLE_ICON,
    EM_ITEMROLE_NAME,
    EM_ITEMROLE_SUB_MESSAGE,
    EM_ITEMROLE_XMPPID,

    EM_TITLEROLE_TYPE,
    EM_TITLEROLE_NAME,
    EM_TITLEROLE_HASMORE,
    EM_TITLEROLE_REQ_TYPE,
    EM_TITLEROLE_HOVER,
};

enum SearchItemType
{
    EM_ITEM_TYPE_TITLE,
    EM_ITEM_TYPE_ITEM,
    EM_ITEM_TYPE_SHOW_MORE
};

enum {
    REQ_TYPE_ALL,
    REQ_TYPE_USER,
    REQ_TYPE_GROUP,
    REQ_TYPE_HISTORY,
};

class SearchItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
    explicit SearchItemDelegate(QAbstractItemView* parent);
	~SearchItemDelegate() override;

public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

Q_SIGNALS:
    void sgOpenNewSession(int, const QString&, const QString&, const QString&);
    void sgGetMore(int);
    void sgSwitchFun(int);

private:
    QFont titleNameFont;
    QFont titleBtnFont;

    QFont itemNameFont;
    QFont itemSubFont;

    QFont moreFont;

private:
    int hover_index_row = 0;
    bool press = false;
};

#endif // SEARCHITEMDELEGATE_H
