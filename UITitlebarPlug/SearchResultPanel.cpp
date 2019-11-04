#include "SearchResultPanel.h"

#include <QButtonGroup>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include "../Message/ChatMessage.h"
#include "../QtUtil/Utils/Log.h"
#include "SearchThread.h"
#include "../Platform/Platform.h"
#include "../include/Line.h"

using namespace QTalk::Search;

SearchResultPanel::SearchResultPanel(TitlebarMsgManager *messageManager, TitlebarMsgListener *messageListener,
                                     QWidget *parent) :
        _pMessageManager(messageManager),
        _pMessageListener(messageListener),
        _pSearchView(nullptr),
        _searchThread(nullptr),
        isGetMore(false),
        UShadowDialog(parent, false, false) {
    initPanel();
#ifndef _WINDOWS
    Qt::WindowFlags flags = this->windowFlags()
                            | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
#endif

    _reqType = 0;
    _searchStart = 0;
    _searchLength = 3;
}

SearchResultPanel::~SearchResultPanel() {

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void SearchResultPanel::initPanel() {
    this->setFixedWidth(270);
    auto *gLay = new QVBoxLayout(_pCenternWgt);
    gLay->setMargin(0);
    auto *mainFrm = new QFrame(this);
    mainFrm->setObjectName("SearchResultPanel");
    gLay->addWidget(mainFrm);
    auto *mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);

    auto *btmsFrm = new QFrame(this);
    mainLay->addWidget(btmsFrm);

    _tabGroup = new UCButtonGroup(this);
    _allBtn = new UCButton(tr("全部"),this);
    _contactBtn = new UCButton(tr("联系人"), this);
    _groupChatBtn = new UCButton(tr("群聊"), this);
    _chatRecordBtn = new UCButton(tr("聊天记录"), this);
    _fileBtn = new UCButton(tr("文件"), this);
    _chatRecordBtn->setVisible(false);
    _fileBtn->setVisible(false);
    _chatRecordBtn->setMinimumWidth(60);
    //
    _tabGroup->addButton(_allBtn, REQ_TYPE_ALL);
    _tabGroup->addButton(_contactBtn, REQ_TYPE_USER);
    _tabGroup->addButton(_groupChatBtn, REQ_TYPE_GROUP);
    _tabGroup->addButton(_chatRecordBtn, REQ_TYPE_HISTORY);
    _tabGroup->addButton(_fileBtn, REQ_TYPE_FILE);

    auto *tabLay = new QHBoxLayout(btmsFrm);
    tabLay->addWidget(_allBtn);
    tabLay->addWidget(_contactBtn);
    tabLay->addWidget(_groupChatBtn);
    tabLay->addWidget(_chatRecordBtn);
    tabLay->addWidget(_fileBtn);

    _pSearchView = new SearchView(this);
    _pWnLabel = new QLabel(tr("搜索联系人、群聊、共同群聊，\n 支持关键词、ID搜索"));
    _pWnLabel->setObjectName("SearchTipWnd");
    _pWnLabel->setContentsMargins(0, 80, 0, 0);
    _pWnLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mainLay->addWidget(new Line);
    mainLay->addWidget(_pSearchView);
    mainLay->addWidget(_pWnLabel);
    _pSearchView->setVisible(false);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 0);
    mainLay->setStretch(2, 1);
    mainLay->setStretch(3, 1);

    _allBtn->setCheckState(true);

    connect(_tabGroup, SIGNAL(clicked(int)), this, SLOT(onFunSelect(int)));
    connect(_pSearchView, SIGNAL(sgSwitchFun(int)), this, SLOT(onFunSelect(int)));
    connect(_pSearchView, SIGNAL(sgOpenNewSession(StSessionInfo)),
            this, SIGNAL(sgOpenNewSession(StSessionInfo)));
    _searchThread = new SearchThread(this);
    connect(_searchThread, &SearchThread::sgSearchStart, this, &SearchResultPanel::onSearchStart);

    connect(this, &SearchResultPanel::sgSelectUp, _pSearchView, &SearchView::selectUp);
    connect(this, &SearchResultPanel::sgSelectDown, _pSearchView, &SearchView::selectDown);
    connect(this, &SearchResultPanel::sgSelectItem, _pSearchView, &SearchView::selectItem);
    connect(_pSearchView, &SearchView::sgGetMore, this, &SearchResultPanel::onGetMore);
    //
    qRegisterMetaType<SearchResult>("SearchResult");
    connect(this, &SearchResultPanel::sgGotSearchResult,
            this, &SearchResultPanel::onGotSearchResult, Qt::QueuedConnection);

//    _pWnLabel->setStyleSheet("font-size:14px; color:rgba(191,191,191,1);");
#ifdef _MACOS
    macAdjustWindows();
#endif
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SearchResultPanel::closeSearch() {
    if (_pSearchView) {
        _pSearchView->clear();
        _pWnLabel->setVisible(true);
        _pSearchView->setVisible(false);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void SearchResultPanel::onFunSelect(const int &index) {

    _tabGroup->setCheck(index);
    _reqType = index;
    onSearchStart(_keywords);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SearchResultPanel::onSearchStart(const QString &keywords) {

    isGetMore = false;
    _keywords = keywords;
    if (_keywords.isEmpty() || _keywords.size() == 1) {
        closeSearch();
        return;
    }

    if (!this->isVisible())
            emit sgOpenSearch(true);

    switch(_reqType)
    {
        case REQ_TYPE_ALL:
            _searchStart = 0;
            _searchLength = 3;
            break;
        case REQ_TYPE_USER:
            _searchStart = 0;
            _searchLength = 30;
            break;
        case REQ_TYPE_GROUP:
            _searchStart = 0;
            _searchLength = 10;
            break;
        case REQ_TYPE_HISTORY:
            _searchStart = 0;
            _searchLength = 10;
            break;
        case REQ_TYPE_FILE:
            _searchStart = 0;
            _searchLength = 10;
            break;
        default:
            break;
    }

    switch(_reqType)
    {
        case REQ_TYPE_ALL:
            onSearchAll();
            break;
        case REQ_TYPE_USER:
            onSearchContact();
            break;
        case REQ_TYPE_GROUP:
            onSearchGroup();
            break;
        case REQ_TYPE_HISTORY:
            onSearchHistory();
            break;
        case REQ_TYPE_FILE:
            onSearchFile();
            break;
        default:
            break;
    }

    emit sgSetEditFocus();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SearchResultPanel::onSearchAll() {
    if (_pMessageManager) {
        //
        std::thread([this](){
            SearchInfoEvent searchInfo;
            searchInfo.start = _searchStart;
            searchInfo.length = _searchLength;
            searchInfo.key = _keywords.toStdString();
            _action = searchInfo.action = EM_ACTION_USER | EM_ACTION_MUC | EM_ACTION_COMMON_MUC /*| EM_ACTION_HS_SINGLE | EM_ACTION_HS_MUC | EM_ACTION_HS_FILE*/;
            _pMessageManager->sendSearch(searchInfo);

            emit sgGotSearchResult(searchInfo.key.data(), searchInfo.searchRet);
        }).detach();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SearchResultPanel::onSearchContact() {
    if (_pMessageManager) {
        std::thread([this](){
            SearchInfoEvent searchInfo;
            searchInfo.start = _searchStart;
            searchInfo.length = _searchLength;
            searchInfo.key = _keywords.toStdString();
            _action = searchInfo.action = EM_ACTION_USER;
            _pMessageManager->sendSearch(searchInfo);

            emit sgGotSearchResult(searchInfo.key.data(), searchInfo.searchRet);
        }).detach();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SearchResultPanel::onSearchGroup() {
    if (_pMessageManager) {

        std::thread([this](){
            SearchInfoEvent searchInfo;
            searchInfo.start = _searchStart;
            searchInfo.length = _searchLength;
            searchInfo.key = _keywords.toStdString();
            _action = searchInfo.action = EM_ACTION_MUC | EM_ACTION_COMMON_MUC;
            _pMessageManager->sendSearch(searchInfo);
            emit sgGotSearchResult(searchInfo.key.data(), searchInfo.searchRet);
        }).detach();
    }
}

void SearchResultPanel::onSearchHistory() {
    if (_pMessageManager) {
        //
        std::thread([this](){
            SearchInfoEvent searchInfo;
            searchInfo.start = _searchStart;
            searchInfo.length = _searchLength;
            searchInfo.key = _keywords.toStdString();
            _action = searchInfo.action = EM_ACTION_HS_SINGLE | EM_ACTION_HS_MUC;
            _pMessageManager->sendSearch(searchInfo);

            emit sgGotSearchResult(searchInfo.key.data(), searchInfo.searchRet);
        }).detach();
    }
}

void SearchResultPanel::onSearchFile() {
    if (_pMessageManager) {
        //
        std::thread([this](){
            SearchInfoEvent searchInfo;
            searchInfo.start = _searchStart;
            searchInfo.length = _searchLength;
            searchInfo.key = _keywords.toStdString();
            _action = searchInfo.action = EM_ACTION_HS_FILE;
            _pMessageManager->sendSearch(searchInfo);

            emit sgGotSearchResult(searchInfo.key.data(), searchInfo.searchRet);
        }).detach();
    }
}

/**
 *
 * @param req
 */
void SearchResultPanel::addSearchReq(const QString &req) {
    _searchThread->addSearchReq(req);
}

/**
 *
 * @param key
 * @param ret
 */
void SearchResultPanel::onGotSearchResult(const QString& key, const SearchResult& ret)
{
    if(nullptr == _pSearchView)
        return;
    //
    bool getmore = isGetMore;
    if(!getmore)
        _pSearchView->clear();
    //
    if (!_pSearchView->isVisible()) {
        _pSearchView->setVisible(true);
        _pWnLabel->setVisible(false);
    }
    //
    if(_keywords != key)
        return;
    //
    if(getmore)
        _pSearchView->removeMoreBar();
    //
    for(const auto & it : ret)
    {
        if((_action & it.first) == 0)
            continue;

        _pSearchView->addSearchResult(it.second, _reqType, getmore);
    }

    if(REQ_TYPE_ALL == _reqType)
    {
        _pSearchView->addOpenWithIdItem(_keywords);
    }
}

void SearchResultPanel::onGetMore(int req)
{
    if(_reqType == req)
    {
        isGetMore = true;
        _tabGroup->setCheck(req);

        switch(_reqType)
        {
            case REQ_TYPE_USER:
            case REQ_TYPE_GROUP:
            case REQ_TYPE_HISTORY:
            case REQ_TYPE_FILE:
                _searchStart = _searchLength;
                _searchLength += 10;
                break;
            case REQ_TYPE_ALL:
            default:
                break;
        }

        if (!this->isVisible())
                emit sgOpenSearch(true);

        switch(_reqType)
        {
            case REQ_TYPE_USER:
                onSearchContact();
                break;
            case REQ_TYPE_GROUP:
                onSearchGroup();
                break;
            case REQ_TYPE_HISTORY:
                onSearchHistory();
                break;
            case REQ_TYPE_FILE:
                onSearchFile();
                break;
            default:
                break;
        }

        emit sgSetEditFocus();
    }
}
