//
// Created by QITMAC000260 on 2018/11/26.
//

#ifndef QTALK_V2_ATMESSAGEVIEW_H
#define QTALK_V2_ATMESSAGEVIEW_H

#include <QListView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QList>
#include "../CustomUi/UShadowWnd.h"
#include "../include/CommonStrcut.h"
#include <QMutexLocker>

class AtSortModel : public QSortFilterProxyModel
{
public:
    AtSortModel(QObject *parent = nullptr);
    ~AtSortModel();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

public:
    static int filterCount;
};


class AtItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
    explicit AtItemDelegate(QObject *parent = nullptr);
    ~AtItemDelegate() override;

signals:
	void itemClicked(const QString&, const QString&);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
	bool editorEvent(QEvent *e, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index);
};

class InputWgt;
class AtMessageView : public QFrame
{
	Q_OBJECT
public:
    explicit AtMessageView(InputWgt *inputWgt);
    ~AtMessageView();

public:
    void addItem(const QString& icon, const QString& xmppId, const QString& name, const QString& searchIndex);
    void updateGroupMemberInfo(const std::vector<QTalk::StUserCard>& member);
    void deleteItem(const QString& memberId);
    void updateFilter();

    bool eventFilter(QObject* o, QEvent* e) override;
protected:
//    virtual void focusOutEvent(QFocusEvent *e);
//    virtual void mousePressEvent(QMouseEvent *e);

    void confirmAtUser(const QString& xmppId, const QString& name);

private:
    QListView             *_atView;
    QStandardItemModel    *_atSrcModel;
    AtSortModel           *_atModel;
    AtItemDelegate        *_atItemDelegate;
    QMap<QString, QStandardItem*> _items;

private:
    InputWgt              *_pInputWgt;
    int                   _pos;
    int                   _count;

    QMutex _mutex;
};


#endif //QTALK_V2_ATMESSAGEVIEW_H
