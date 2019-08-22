//
// Created by QITMAC000260 on 2019-07-08.
//

#include "SearchView.h"
#include "../Platform/Platform.h"
#include <QScrollBar>
#include <QMouseEvent>

using namespace QTalk::Search;

SearchView::SearchView(QWidget* parent)
    : QListView(parent)
{
    setObjectName("SearchView");
    this->verticalScrollBar()->setVisible(false);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->verticalScrollBar()->setSingleStep(10);
    _srcModel = new QStandardItemModel(this);
    this->setModel(_srcModel);
    _pItemDelegate = new SearchItemDelegate(this);
    this->setItemDelegate(_pItemDelegate);
    this->setFrameShape(QFrame::NoFrame);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(_pItemDelegate, &SearchItemDelegate::sgOpenNewSession,
            this, &SearchView::onOpenNewSession, Qt::QueuedConnection);
    connect(_pItemDelegate, &SearchItemDelegate::sgGetMore, this, &SearchView::sgGetMore);
    connect(_pItemDelegate, &SearchItemDelegate::sgSwitchFun, this, &SearchView::sgSwitchFun);
}

SearchView::~SearchView() = default;

void SearchView::onOpenNewSession(int type, const QString& xmppid, const QString& name, const QString& icon)
{
    StSessionInfo sessionInfo(type, xmppid, name);
    sessionInfo.headPhoto = icon;
    emit sgOpenNewSession(sessionInfo);
}

void SearchView::selectUp() {
    auto curIndex = currentIndex();
    if(curIndex.row() > 0)
        setCurrentIndex(_srcModel->index(curIndex.row() - 1, 0));
}

void SearchView::selectDown()
{
    auto curIndex = currentIndex();
    if(curIndex.row() < _srcModel->rowCount() - 1)
        setCurrentIndex(_srcModel->index(curIndex.row() + 1, 0));
}

void SearchView::selectItem() {
    auto index = currentIndex();
    SearchItemType itemType = (SearchItemType)index.data(EM_TYPE_TYPE).toInt();
    switch (itemType)
    {
        case EM_ITEM_TYPE_ITEM:
        {
            //
            int type = index.data(EM_ITEMROLE_ITEM_TYPE).toInt();

            if(QTalk::Search::EM_ACTION_USER == type)
            {
                int chatType = QTalk::Enum::TwoPersonChat;
                QString xmppId = index.data(EM_ITEMROLE_XMPPID).toString();
                QString icon = index.data(EM_ITEMROLE_ICON).toString();
                QString name = index.data(EM_ITEMROLE_NAME).toString();
                onOpenNewSession(chatType, xmppId, name, icon);
            }
            else if(QTalk::Search::EM_ACTION_MUC == type ||
                    QTalk::Search::EM_ACTION_COMMON_MUC == type)
            {
                int chatType = QTalk::Enum::GroupChat;
                QString xmppId = index.data(EM_ITEMROLE_XMPPID).toString();
                QString icon = index.data(EM_ITEMROLE_ICON).toString();
                QString name = index.data(EM_ITEMROLE_NAME).toString();
                onOpenNewSession(chatType, xmppId, name, icon);
            }
            else
            {

            }
            break;
        }
        case EM_ITEM_TYPE_SHOW_MORE:
        {
            int req = index.data(EM_TITLEROLE_REQ_TYPE).toInt();
            emit sgGetMore(req);
            break;
        }
        case EM_ITEM_TYPE_TITLE:
        default:
            break;
    }
}

void SearchView::clear()
{
    if(_srcModel)
        _srcModel->clear();
}

void addUserItem(QStandardItemModel *model, const std::vector<StUserItem>& users)
{
    for(const auto& it : users)
    {
        auto* pItem = new QStandardItem;
        pItem->setData(EM_ITEM_TYPE_ITEM, EM_TYPE_TYPE);
        pItem->setData(EM_ACTION_USER, EM_ITEMROLE_ITEM_TYPE);
        pItem->setData(QTalk::GetHeadPathByUrl(it.icon).data(), EM_ITEMROLE_ICON);
        pItem->setData(it.name.data(), EM_ITEMROLE_NAME);
        pItem->setData(it.structure.data(), EM_ITEMROLE_SUB_MESSAGE);
        pItem->setData(it.xmppId.data(), EM_ITEMROLE_XMPPID);
        pItem->setData(it.tips.data(), Qt::ToolTipRole);

        model->appendRow(pItem);
    }
}

void addGroupItem(QStandardItemModel *model, const std::vector<StGroupItem>& groups)
{
    for(const auto& it : groups)
    {
        auto* pItem = new QStandardItem;
        pItem->setData(EM_ITEM_TYPE_ITEM, EM_TYPE_TYPE);
        pItem->setData(it.type, EM_ITEMROLE_ITEM_TYPE);
        pItem->setData(QTalk::GetHeadPathByUrl(it.icon).data(), EM_ITEMROLE_ICON);
        pItem->setData(it.name.data(), EM_ITEMROLE_NAME);
        if(!it._hits.empty())
        {
            QString subMessage = "包含: ";
            for(const auto& hit : it._hits)
            {
                subMessage.append(QString("%1 ").arg(hit.data()));
            }
            pItem->setData(subMessage, EM_ITEMROLE_SUB_MESSAGE);
        }
        pItem->setData(it.xmppId.data(), EM_ITEMROLE_XMPPID);
        pItem->setData(it.name.data(), Qt::ToolTipRole);

        model->appendRow(pItem);
    }
}

/**
 *
 * @param ret
 */
void SearchView::addSearchResult(const QTalk::Search::StSearchResult& ret, int reqType, bool isGetMore)
{
    if(!isGetMore)
    {
        auto* titleItem = new QStandardItem;
        titleItem->setData(EM_ITEM_TYPE_TITLE, EM_TYPE_TYPE);
        titleItem->setData(ret.resultType, EM_TITLEROLE_TYPE);
        titleItem->setData(ret.groupLabel.data(), EM_TITLEROLE_NAME);
        titleItem->setData(ret.hasMore, EM_TITLEROLE_HASMORE);
        titleItem->setData(reqType, EM_TITLEROLE_REQ_TYPE);
        //
        _srcModel->appendRow(titleItem);
        if(_srcModel->rowCount() == 1)
            setCurrentIndex(titleItem->index());
    }

    if(ret.resultType & EM_ACTION_USER)
    {
        addUserItem(_srcModel, ret._users);
    }
    else if((ret.resultType & EM_ACTION_MUC) ||
        (ret.resultType & EM_ACTION_COMMON_MUC))
    {
        addGroupItem(_srcModel, ret._groups);
    }

    if(ret.hasMore && reqType != REQ_TYPE_ALL)
    {
        auto* moreItem = new QStandardItem;
        moreItem->setData(EM_ITEM_TYPE_SHOW_MORE, EM_TYPE_TYPE);
        moreItem->setData(reqType, EM_TITLEROLE_REQ_TYPE);
        _srcModel->appendRow(moreItem);
    }
}

//
void SearchView::addOpenWithIdItem(const QString& keyId) {

    auto* titleItem = new QStandardItem;
    titleItem->setData(EM_ITEM_TYPE_TITLE, EM_TYPE_TYPE);
//    titleItem->setData(REQ_TYPE_ALL, EM_TITLEROLE_TYPE);
    titleItem->setData(QString("打开ID为[ %1 ]的会话").arg(keyId), EM_TITLEROLE_NAME);
    titleItem->setData(false, EM_TITLEROLE_HASMORE);
//    titleItem->setData(REQ_TYPE_ALL, EM_TITLEROLE_REQ_TYPE);
    //
    _srcModel->appendRow(titleItem);
    if(_srcModel->rowCount() == 1)
        setCurrentIndex(titleItem->index());
    //
    QString id = keyId;
    id = id.trimmed().toLower();
    if(!id.contains("@"))
    {
        id += QString("@%1").arg(Platform::instance().getSelfDomain().data());
    }

    auto* pItem = new QStandardItem;
    pItem->setData(EM_ITEM_TYPE_ITEM, EM_TYPE_TYPE);
    pItem->setData(EM_ACTION_USER, EM_ITEMROLE_ITEM_TYPE);
    pItem->setData("", EM_ITEMROLE_ICON);
    pItem->setData(keyId, EM_ITEMROLE_NAME);
    pItem->setData("/", EM_ITEMROLE_SUB_MESSAGE);
    pItem->setData(id, EM_ITEMROLE_XMPPID);
    pItem->setData("打开离职员工或者跨域用户会话", Qt::ToolTipRole);

    _srcModel->appendRow(pItem);
}

/**
 *
 */
void SearchView::removeMoreBar()
{
    int row = _srcModel->rowCount();
    while (row != 0)
    {
        QModelIndex index = _srcModel->index(--row, 0);
        if(index.isValid())
        {
            int type = index.data(EM_TYPE_TYPE).toInt();
            if(EM_ITEM_TYPE_SHOW_MORE == type)
            {
                _srcModel->removeRow(row);
                break;
            }
        }
    }
}