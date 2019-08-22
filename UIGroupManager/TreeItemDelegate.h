//
// Created by cc on 18-11-13.
//

#ifndef QTALK_V2_TREEITEMDELEGATE_H
#define QTALK_V2_TREEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QEvent>
#include <QSortFilterProxyModel>
#include <QDebug>

enum SearchItemRoles
{
    EM_STAFF_DATATYPE_ROW_TYPE = Qt::UserRole + 1,
	EM_STAFF_DATATYPE_TEXT,
	EM_STAFF_DATATYPE_ICONPATH,
	EM_STAFF_DATATYPE_XMPPID,
	EM_DATATYPE_CHECKSTATE,
    EM_STAFF_DATATYPE_EXTEND,
};

enum rowType{
    EM_ROW_INVALID = 0,
    EM_ROW_TYPE_TITLE,
    EM_ROW_TYPE_SUBTITLE,
    EM_ROW_TYPE_ITEM
};

class TreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TreeItemDelegate(QWidget *parent);
    ~TreeItemDelegate() override;

signals:
    void itemChanged(const QModelIndex&);
    void sgItemDbClicked(const QModelIndex&);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    QWidget * _parentWgt;
};


#endif //QTALK_V2_TREEITEMDELEGATE_H
