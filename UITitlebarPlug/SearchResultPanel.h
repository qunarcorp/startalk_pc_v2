#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include "UCButton.h"
#include "../CustomUi/UShadowWnd.h"
#include "MessageManager.h"
#include "../UICom/UIEntity.h"
#include "SearchView.h"

class SearchThread;

class SearchResultPanel : public UShadowDialog {
    Q_OBJECT
public:
    explicit SearchResultPanel(TitlebarMsgManager *messageManager,
                               TitlebarMsgListener *messageListener,
                               QWidget *parent = nullptr);
    ~SearchResultPanel() override;

public:
    void initPanel();
    void closeSearch();

public slots:
    void onFunSelect(const int &index);
    void onSearchStart(const QString &keywords);
    void onSearchAll();
    void onSearchContact();
    void onSearchGroup();
    void addSearchReq(const QString &req);
    void onGotSearchResult(const QString& key, const SearchResult& ret);
    void onGetMore(int req);

signals:
    void sgOpenNewSession(const StSessionInfo &into);
    void sgOpenSearch(bool);
    void sgSetEditFocus();
    void sgSelectUp();
    void sgSelectDown();
    void sgSelectItem();
    void sgGotSearchResult(const QString& key, const SearchResult& ret);

private:
    SearchView* _pSearchView;
    UCButton  *_allBtn = nullptr;        // 全部
    UCButton  *_contactBtn = nullptr;    // 联系人
    UCButton  *_groupChatBtn = nullptr;  // 群聊
    UCButton  *_chatRecordBtn = nullptr; // 聊天记录
    UCButtonGroup *_tabGroup = nullptr;
    QLabel       *_pWnLabel = nullptr;

    QString _keywords; // 搜索关键字
    int _reqType;
    int _searchStart;
    int _searchLength;
    int _action;
    bool isGetMore;

    TitlebarMsgManager *_pMessageManager;
    TitlebarMsgListener *_pMessageListener;

private:
    SearchThread *_searchThread;

};

#endif // SEARCHPANEL_H
