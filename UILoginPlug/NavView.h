//
// Created by cc on 2018-12-26.
//
#ifndef QTALK_V2_NAVVIEW_H
#define QTALK_V2_NAVVIEW_H

#include <QFrame>
#include <QListView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include "../CustomUi/LinkButton.h"
#include "AddNavWnd.h"

struct StNav {
    QString name;
    QString domain;
    QString url;
    bool    debug;
};

enum
{
    ITEM_DATA_NAME = Qt::UserRole + 1,
    ITEM_DATA_CHECKED,
    ITEM_DATA_Link,
};

/**
 *
 */
class NavItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    NavItemDelegate();
    ~NavItemDelegate() override;

Q_SIGNALS:
    void itemClicked(const QString&);
    void showDetail(const QString&);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

/**
 *
 */
class NavMainView : public QFrame
{
    Q_OBJECT

public:
    explicit NavMainView(const StNav& nav, QWidget* parnet = nullptr);
    ~NavMainView() override;

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

Q_SIGNALS:
    void sgBack();
    void deleteSignal(const QString&);
    void navAddrChanged(const QString&, const QString&); // name, addr
    void navDebugChanged(const QString&, bool); // name, debug

private:
    QLabel*    _pQRCodeLabel;
    QLineEdit* _pNameEdit;
    QLineEdit* _pHostEdit;
    QTextEdit* _pAddressEdit;
    QPushButton* _pDeleteBtn;

public:
    StNav        _nav;
};


/**
 *
 */
class NavView : public QFrame
{
    Q_OBJECT
public:
    NavView(QMap<QString, StNav>& mapNav, QString& defalutNav, QWidget* wgt = nullptr);
    ~NavView() override;

public:
    bool checkName(const QString& name);
    void onItemClicked(const QString& name);
    void onShowDetail(const QString& name);

Q_SIGNALS:
    void addItemSignal(const StNav&);
    void saveConfSignal();
    void addNavSinal(const QString &name, const QString &navAddr, const bool &isDebug);
    void sgClose();

private:
    void addItem(const StNav& stNav);
    void onSelectChange(const QString& name);
    void onDeleteItem(const QString& name);
    void onNavAddrChanged(const QString& name, const QString& addr);
    void onNavDebugChanged(const QString& name, bool debug);
    void onBack();

private:
    QListView*  _itemView;
    QStandardItemModel* _itemModel;
    NavItemDelegate*    _itemDelegate;
    QPushButton*        _addBtn;
    QStackedWidget*     _itemStackWgt;

    AddNavWnd*          _pAddNavWnd;
    QFrame* _mainFrm;

private:
    QMap<QString, NavMainView*> _mapMainView;
    QMap<QString, QStandardItem*> _mapListItem;

private:
    QString& _defaultName;
    QMap<QString, StNav>& _mapNav;
};


#endif //QTALK_V2_NAVVIEW_H
