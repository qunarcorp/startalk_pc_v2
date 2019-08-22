//
// Created by QITMAC000260 on 2019-06-20.
//

#ifndef QTALK_V2_SEARCHVIEW_H
#define QTALK_V2_SEARCHVIEW_H


#include <QListView>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>

enum
{
    EM_SEARCH_DATATYPE_TEXT = Qt::UserRole + 1,
    EM_SEARCH_DATATYPE_ICONPATH,
    EM_SEARCH_DATATYPE_XMPPID,
    EM_SEARCH_DATATYPE_USERID,
    EM_SEARCH_DATATYPE_INDEX,
    EM_SEARCH_DATATYPE_CHECKSTATE,
    EM_SEARCH_DATATYPE_STAFF,
};

class SearchItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SearchItemDelegate(QWidget *parent);
    ~SearchItemDelegate() override;

signals:
    void itemChanged(const QModelIndex&);
    void sgItemDbClicked(const QModelIndex&);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};


class SearchItemSortModel : public QSortFilterProxyModel
{
public:
    explicit SearchItemSortModel(QObject* parent = nullptr);

protected:
    //
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif //QTALK_V2_SEARCHVIEW_H
