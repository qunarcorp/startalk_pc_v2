//
// Created by cc on 18-12-17.
//

#ifndef QTALK_V2_LISTITEMVIEW_H
#define QTALK_V2_LISTITEMVIEW_H


#include <QFrame>
#include <QListView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QMutexLocker>
#include "../include/CommonDefine.h"

enum {
    EM_DATA_TYPE_ICON = Qt::UserRole + 1,
    EM_DATA_TYPE_TEXT,
    EM_DATA_TYPE_ID,
    EM_DATA_TYPE_TYPE,
};

enum {
    EM_TYPE_USER,
    EM_TYPE_GROUP,
};

class ListItemDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
    explicit ListItemDelegate(QObject *parent = nullptr);
    ~ListItemDelegate() override;

signals:
    void itemClicked(const QString&, const QUInt8);

protected:
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;
};

class AddressBookPanel;
class ListItemView : public QFrame
{
Q_OBJECT
public:
    ListItemView(AddressBookPanel* _panel, QWidget* parent);
    ~ListItemView();

public:
    void addItem(const QString& id, const QUInt8& type, const QString& icon, const QString& name);
    void removeItem(const QString& id);

signals:
    //void addItemSignal(const QString&, const QString&, const QString&);
    //void removeItemSignal(const QString& id);

public:
    void resetHeight(int maxH, int FixedH);
    void clearSelection();

private:
    QListView          *_pLstView;
    QStandardItemModel *_pModel;
    ListItemDelegate   *_pDelegate;

private:
    QMap<QString, QStandardItem*> _items;

private:
    QMutex              _mutex;

private:
    AddressBookPanel*  _mainPanel;

};


#endif //QTALK_V2_LISTITEMVIEW_H
