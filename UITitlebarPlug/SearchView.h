//
// Created by QITMAC000260 on 2019-07-08.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_SEARCHVIEW_H
#define QTALK_V2_SEARCHVIEW_H

#include <QFrame>
#include <QListView>
#include <QStandardItemModel>
#include "../UICom/UIEntity.h"
#include "SearchItemDelegate.h"


/**
* @description: SearchView
* @author: cc
* @create: 2019-07-08 16:33
**/
class SearchView : public QListView{
    Q_OBJECT
public:
    explicit SearchView(QWidget* parent = nullptr);
    ~SearchView() override;

signals:
    void sgOpenNewSession(const StSessionInfo &into);
    void sgGetMore(int );
    void sgSwitchFun(int );

public:
    void selectUp();
    void selectDown();
    void selectItem();
    void removeMoreBar();
    void addOpenWithIdItem(const QString& keyId);

public:
    void addSearchResult(const QTalk::Search::StSearchResult& ret, int reqType, bool isGetMore);
    void onHoverIndexChanged(const QModelIndex&);

public:
    void clear();

private:
    void onOpenNewSession(int type, const QString &xmppid, const QString &name, const QString &icon);

private:
    QStandardItemModel *_srcModel;
    SearchItemDelegate* _pItemDelegate;
};


#endif //QTALK_V2_SEARCHVIEW_H
