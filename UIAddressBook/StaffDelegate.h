//
// Created by cc on 18-12-20.
//
#ifndef QTALK_V2_STAFFDELEGATE_H
#define QTALK_V2_STAFFDELEGATE_H


#include <QStyledItemDelegate>

enum SearchItemRoles
{
	EM_STAFF_DATATYPE_TEXT = Qt::UserRole + 1,
	EM_STAFF_DATATYPE_ICONPATH,
	EM_STAFF_DATATYPE_XMPPID,
	EM_DATATYPE_SEARCHKEY,
	EM_DATATYPE_CHECKSTATE,
	EM_STAFF_DATATYPE_HASCHILD,
	EM_STAFF_DATATYPE_STRUCTURE
};

class StaffDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StaffDelegate(QWidget* parent);
    ~StaffDelegate();

Q_SIGNALS:
    void itemClicked(const QModelIndex& );

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

};


#endif //QTALK_V2_STAFFDELEGATE_H
