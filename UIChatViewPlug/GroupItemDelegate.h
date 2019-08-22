//
// Created by QITMAC000260 on 2018/11/19.
//

#ifndef QTALK_V2_GROUPITEMDELEGATE_H
#define QTALK_V2_GROUPITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>

enum itemDataType
{
    EM_ITEMDATA_TYPE_USERID = Qt::UserRole + 1,
    EM_ITEMDATA_TYPE_XMPPID,
    EM_ITEMDATA_TYPE_USERNAME,
    EM_ITEMDATA_TYPE_MASKNAME,
    EM_ITEMDATA_TYPE_ISONLINE,
    EM_ITEMDATA_TYPE_USERHEAD,
    EM_ITEMDATA_TYPE_HEADPATH,
    EM_ITEMDATA_TYPE_SEARCHKEY,
    EM_ITEMDATA_TYPE_USERTYPE,
};


class GroupMemberSortModel : public QSortFilterProxyModel
{
public:
    explicit GroupMemberSortModel(QObject* parent = nullptr)
            : QSortFilterProxyModel(parent)
    {

    }

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override {
        if (!source_left.isValid() || !source_right.isValid())
            return false;

        int leftRole = source_left.data(EM_ITEMDATA_TYPE_USERTYPE).toInt();
        int rightRole = source_right.data(EM_ITEMDATA_TYPE_USERTYPE).toInt();
        bool leftIsOnline = source_left.data(EM_ITEMDATA_TYPE_ISONLINE).toBool();
        bool rightIsOnline = source_right.data(EM_ITEMDATA_TYPE_ISONLINE).toBool();

        if(leftRole != rightRole)
            return leftRole < rightRole;
        else
            return leftIsOnline > rightIsOnline;
    }
    //
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

        if(index.isValid())
        {
            if(filterRegExp().isEmpty())
                return true;
            else
            {
                QString userId = index.data(EM_ITEMDATA_TYPE_USERID).toString().toLower();
                bool ret = index.data(EM_ITEMDATA_TYPE_USERNAME).toString().toLower().contains(filterRegExp()) ||
                           index.data(EM_ITEMDATA_TYPE_MASKNAME).toString().toLower().contains(filterRegExp()) ||
                           index.data(EM_ITEMDATA_TYPE_SEARCHKEY).toString().toLower().contains(filterRegExp()) ||
                           userId.contains(filterRegExp());
                return ret;
            }
        }
        return false;
    }
};

class GroupItemDelegate : public QStyledItemDelegate{

public:
    explicit GroupItemDelegate(QWidget *parent);
    ~GroupItemDelegate() override;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    QWidget* _parentWgt;

    QMap<QString, QPixmap*> mapPixmap;
};


#endif //QTALK_V2_GROUPITEMDELEGATE_H
