//
// Created by cc on 18-11-12.
//

#ifndef QTALK_V2_CHOSEMEMBERWGT_H
#define QTALK_V2_CHOSEMEMBERWGT_H

#include <QStyledItemDelegate>

enum {
    EM_ITEM_DATA_XMPPID = Qt::UserRole + 1,
    EM_ITEM_DATA_HEAD,
    EM_ITEM_DATA_NAME,
};

enum {
    ITEM_COLUMN_CONTENT,
    ITEM_COLUMN_CLOSEBTN
};

class ChoseItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChoseItemDelegate(QWidget *parent);

signals:
    void removeItem(const QString&);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif //QTALK_V2_CHOSEMEMBERWGT_H
