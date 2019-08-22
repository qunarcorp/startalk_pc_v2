#include "UIGroupManager.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QFile>
#include <QFileInfo>
#include <QStandardItemModel>
#include <thread>
#include <QDateTime>
#include <QSplitter>
#include <QJsonDocument>
#include <QApplication>
#include "../CustomUi/QtMessageBox.h"

#include "../UICom/uicom.h"
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/utils.h"
#include "SearchWgt.h"
#include "ChoseMemberWgt.h"
#include "TreeItemDelegate.h"
#include "../CustomUi/HeadPhotoLab.h"
#include "../Platform/dbPlatForm.h"
#include "BatchAddMemberWnd.h"
#include "../include/Line.h"
#include "../QtUtil/Entity/JID.h"

#define DEM_TIP_MESSAGE "已选择了 %1 个联系人"

UIGroupManager::UIGroupManager()
        : UShadowDialog(nullptr, true)
        , _pSearchWgt(nullptr)
        , _pLstWgt(nullptr)
        , _plstModel(nullptr)
        , _pTipLabel(nullptr)
        , _pTitleLabel(nullptr)
        , _pGroupNameEdit(nullptr)
        , _pGroupNaemFrm(nullptr)
        , _pTreeWgt(nullptr)
        , _pTreeModel(nullptr)
        , _batchAddMemberBtn(nullptr)
        , _batchAddMemberWnd(nullptr)
        , _type(EM_TYPE_INVALID)
        , _pItemDelegate(nullptr)
        , _pRecentItem(nullptr){

    initUi();
    //
    _pMsgManager = new GroupManagerMsgManager;
    _pMsgListener = new GroupMsgListener(this);

    connect(this, &UIGroupManager::initDataFinish, this, &UIGroupManager::updateUi, Qt::QueuedConnection);
    // init data
    initData();

}

UIGroupManager::~UIGroupManager() = default;

//
void UIGroupManager::onCreatGroup(const QString &memberId) {
    resetUi(memberId);
    _pTitleLabel->setText("创建群组");
    _type = EM_TYPE_CREATGROUP;
    _pGroupNaemFrm->setVisible(true);
    _pTreeWgt->setVisible(true);
    _batchAddMemberBtn->setVisible(true);
    //
    std::shared_ptr<QTalk::Entity::ImUserInfo> info = dbPlatForm::instance().getUserInfo(memberId.toStdString());
    if (info) {
        QString headrSrc = QString(QTalk::GetHeadPathByUrl(info->HeaderSrc).c_str());
        if (QFileInfo(headrSrc).isDir() || !QFile::exists(headrSrc)) {
#ifdef _STARTALK
            headrSrc = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
			headrSrc = ":/QTalk/image1/headPortrait.png";
#endif
        }
        addGroupMember(memberId, headrSrc);
        //
        QWidget *wgt = UICom::getInstance()->getAcltiveMainWnd();
        showCenter(true, wgt);
    }
}

//
void UIGroupManager::onAddGroupMember(const QString &groupId) {
    resetUi();
    _pTitleLabel->setText("加人进群");
    _type = EM_TYPE_ADDGROUPMEMBER;
    _strGroupId = groupId;
    _pGroupNaemFrm->setVisible(false);
    _pTreeWgt->setVisible(true);
    _batchAddMemberBtn->setVisible(true);
    QWidget *wgt = UICom::getInstance()->getAcltiveMainWnd();
    showCenter(true, wgt);
}

//
void UIGroupManager::initUi() {
    setFixedSize(650, 570);
    setObjectName("UIGroupManager");
    //
    auto *titleFrm = new QFrame();
    titleFrm->setFixedHeight(50);
    titleFrm->setObjectName("titleFrm");
    auto *titleLay = new QHBoxLayout(titleFrm);
    titleLay->setContentsMargins(20, 0, 20, 0);
    _pTitleLabel = new QLabel();
    _pTitleLabel->setAlignment(Qt::AlignVCenter);
    _pTitleLabel->setObjectName("TitleLabel");
    titleLay->addWidget(_pTitleLabel);
    titleLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    setMoverAble(true, titleFrm);
    //
    _pSearchWgt = new SearchWgt(this);
    // tree
    _pTreeModel = new QStandardItemModel(this);
    _pTreeWgt = new QTreeView(this);
    _pTreeWgt->setFrameShape(QFrame::NoFrame);
    _pTreeWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pTreeWgt->setModel(_pTreeModel);
    _pTreeWgt->header()->setVisible(false);
    _pTreeWgt->setFrameShape(QFrame::NoFrame);
    _pTreeWgt->setSortingEnabled(true);
    _pTreeWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pTreeWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pItemDelegate = new TreeItemDelegate(this);
    _pTreeWgt->setItemDelegate(_pItemDelegate);
    _pTreeWgt->setSelectionMode(QAbstractItemView::NoSelection);
    _pTreeWgt->setRootIsDecorated(false);
    _pTreeWgt->setExpandsOnDoubleClick(false);
    // search view
    _pSearchView = new QListView(this);
    _pSearchSrcModel = new QStandardItemModel(this);
    _pSearchModel = new SearchItemSortModel(this);
    _pSearchItemDelegate = new SearchItemDelegate(this);
    _pSearchModel->setSourceModel(_pSearchSrcModel);
    _pSearchView->setModel(_pSearchModel);
    _pSearchView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pSearchView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pSearchView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pSearchView->setItemDelegate(_pSearchItemDelegate);
    _pSearchView->setFrameShape(QFrame::NoFrame);
    // group
    auto *item = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    //
    QFrame *leftFrm = new QFrame(this);
    auto *leftLayout = new QVBoxLayout(leftFrm);
    leftLayout->setMargin(0);
    leftLayout->addWidget(_pSearchWgt);
    leftLayout->addWidget(_pTreeWgt);
    leftLayout->addWidget(_pSearchView);
    leftLayout->setStretch(0, 0);
    //
    _pSearchView->setVisible(false);
    //
    QFrame *rightFrm = new QFrame(this);
    auto *rightLayout = new QVBoxLayout(rightFrm);
    auto *btnlayout = new QHBoxLayout;
    btnlayout->setContentsMargins(12, 10, 18, 10);
    btnlayout->setSpacing(15);
    _pTipLabel = new QLabel(QString(DEM_TIP_MESSAGE).arg(0), this);
    _pTipLabel->setObjectName("TipLabel");
    _pTipLabel->setContentsMargins(6, 6, 6, 10);

    auto *listItemDelegate = new ChoseItemDelegate(this);
    _pLstWgt = new QListView(this);
    _pLstWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _plstModel = new QStandardItemModel(this);
    _pLstWgt->setModel(_plstModel);
    _pLstWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pLstWgt->setItemDelegate(listItemDelegate);
    _pLstWgt->setFrameShape(QFrame::NoFrame);
    // group name
    _pGroupNaemFrm = new QFrame();
    _pGroupNaemFrm->setObjectName("groupNaemFrm");
    auto *groupNameLay = new QHBoxLayout(_pGroupNaemFrm);
    groupNameLay->setContentsMargins(20, 20, 5, 20);
    groupNameLay->setSpacing(10);
    auto *groupNameLab = new QLabel("群名称", this);
    groupNameLab->setObjectName("GroupNameLabel");
    _pGroupNameEdit = new QLineEdit(this);
    _pGroupNameEdit->setObjectName("GroupNameEdit");
    _pGroupNameEdit->setPlaceholderText("取个群名方便后续搜索");
    _pGroupNameEdit->setFixedHeight(30);
    groupNameLay->addWidget(groupNameLab);
    groupNameLay->addWidget(_pGroupNameEdit);
    //
    auto *cancelBtn = new QPushButton("取消", this);
    auto *okBtn = new QPushButton("确定", this);
    _batchAddMemberBtn = new QPushButton("高级", this);
    cancelBtn->setObjectName("cancelButton");
    _batchAddMemberBtn->setObjectName("cancelButton");
    okBtn->setObjectName("makeSureButton");
    cancelBtn->setFixedSize(72, 32);
    _batchAddMemberBtn->setFixedSize(72, 32);
    okBtn->setFixedSize(72, 32);
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    _batchAddMemberBtn->setFocusPolicy(Qt::NoFocus);
    okBtn->setFocusPolicy(Qt::NoFocus);

    btnlayout->addWidget(_batchAddMemberBtn);
    btnlayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    btnlayout->addWidget(cancelBtn);
    btnlayout->addWidget(okBtn);
    rightLayout->addWidget(_pTipLabel);
    rightLayout->addWidget(_pLstWgt);
    rightLayout->addWidget(_pGroupNaemFrm);
    //
    auto *splitter = new QSplitter;
    splitter->setContentsMargins(20, 10, 10, 10);
    splitter->setObjectName("groupManagerSplitter");
    splitter->setHandleWidth(1);
    splitter->addWidget(leftFrm);
    splitter->addWidget(rightFrm);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    //
    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("mainFrm");
    auto* mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(6);
    mainLay->setSpacing(0);
    mainLay->addWidget(splitter, 1);
    mainLay->addWidget(new Line, 0);
    mainLay->addLayout(btnlayout, 0);
    //
    auto *layout = new QVBoxLayout(_pCenternWgt);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(titleFrm, 0);
    layout->addWidget(mainFrm);

    _batchAddMemberWnd = new BatchAddMemberWnd(this);

    connect(cancelBtn, &QPushButton::clicked, [this]() {
        this->setVisible(false);
    });

    connect(splitter, &QSplitter::splitterMoved, [this](int pos, int index) {
        qreal maxW = _pTreeWgt->width();
    });

    connect(okBtn, &QPushButton::clicked, [this]() {

        switch (_type) {
            case EM_TYPE_ADDGROUPMEMBER: {
                if (!_mapLstWgtItem.empty()) {
                    sendAddGroupMemberMessage(_strGroupId, _mapLstWgtItem.keys());
                }
                break;
            }
            case EM_TYPE_CREATGROUP:
                creatGroup();
                break;
            default:
                break;
        }

        this->setVisible(false);
    });

    connect(_pSearchItemDelegate, &SearchItemDelegate::itemChanged, [this](QModelIndex index) {

        QString memberId = index.data(EM_SEARCH_DATATYPE_XMPPID).toString();
        QString headSrc = index.data(EM_SEARCH_DATATYPE_ICONPATH).toString();
        bool check = index.data(EM_SEARCH_DATATYPE_CHECKSTATE).toBool();
        std::string id = memberId.toStdString();

        if(!id.empty())
        {
            if(check)
                addGroupMember(memberId, headSrc);
            else
                removeGroupMeber(memberId);
        }
    });

    connect(_pSearchItemDelegate, &SearchItemDelegate::sgItemDbClicked, [this](QModelIndex index) {
        QString id = index.data(EM_SEARCH_DATATYPE_XMPPID).toString();
        QString name = index.data(EM_SEARCH_DATATYPE_TEXT).toString();
        onItemDoubleClick(id, name);
    });

    connect(_pItemDelegate, &TreeItemDelegate::sgItemDbClicked, [this](QModelIndex index) {
        int type = index.data(EM_STAFF_DATATYPE_ROW_TYPE).toInt();
        if(EM_ROW_TYPE_ITEM == type)
        {
            QString id = index.data(EM_STAFF_DATATYPE_XMPPID).toString();
            QString name = index.data(EM_STAFF_DATATYPE_TEXT).toString();
            onItemDoubleClick(id, name);
        }
    });

    connect(_pSearchWgt, &SearchWgt::textChanged, [this](const QString& text) {
        _pSearchModel->setFilterRegExp(text.toLower());
        //
        bool isEmpty = text.isEmpty();
        _pSearchView->setVisible(!isEmpty);
        _pTreeWgt->setVisible(isEmpty);
    });

    connect(_batchAddMemberBtn, &QPushButton::clicked, [this](){
        QPoint pos = this->geometry().topRight();
        _batchAddMemberWnd->clear();
        _batchAddMemberWnd->move(pos.x() + 10, pos.y());
        _batchAddMemberWnd->showModel();
    });
    connect(_batchAddMemberWnd, &BatchAddMemberWnd::sgBatchAddGroupMember, this, &UIGroupManager::onBatchAddMember);

    connect(listItemDelegate, &ChoseItemDelegate::removeItem, [this](const QString& id){
        removeGroupMeber(id);
    });


    connect(_pItemDelegate, &TreeItemDelegate::itemChanged, [this](QModelIndex index) {

        int rowType = index.data(EM_STAFF_DATATYPE_ROW_TYPE).toInt();

        if(rowType == EM_ROW_TYPE_ITEM)
        {
            QString memberId = index.data(EM_STAFF_DATATYPE_XMPPID).toString();
            QString headSrc = index.data(EM_STAFF_DATATYPE_ICONPATH).toString();
            bool check = index.data(EM_DATATYPE_CHECKSTATE).toBool();
            std::string id = memberId.toStdString();
            if(!id.empty())
            {
                if(check)
                    addGroupMember(memberId, headSrc);
                else
                    removeGroupMeber(memberId);
            }
        }
        else
        {
            bool extend = _pTreeModel->data(index, EM_STAFF_DATATYPE_EXTEND).toBool();
            _pTreeWgt->setExpanded(index, extend);
        }
    });
}

//
void UIGroupManager::addGroupMember(const QString &memberId, const QString &headSrc)
{
    if (_mapLstWgtItem.contains(memberId)) return;

    auto *item = new QStandardItem();
    item->setData(memberId, EM_ITEM_DATA_XMPPID);

    QString name = _mapUserName[memberId];
    std::string head = headSrc.toStdString();
    item->setData(headSrc, EM_ITEM_DATA_HEAD);
    item->setData(name, EM_ITEM_DATA_NAME);

    _plstModel->appendRow(item);
    _mapLstWgtItem[memberId] = item;
    _pTipLabel->setText(QString(DEM_TIP_MESSAGE).arg(_mapLstWgtItem.size()));
    //
    std::string id = memberId.toStdString();
    if(!id.empty())
    {
        const auto &mapItems = _mapItems[id];
        for(const auto& it : mapItems)
        {
            auto *tmpItem = it.second;
            if(EM_STRUCTURE == it.first)
                tmpItem->setData(true, EM_SEARCH_DATATYPE_CHECKSTATE);
            else
                tmpItem->setData(true, EM_DATATYPE_CHECKSTATE);
        }
    }
}

//
void UIGroupManager::removeGroupMeber(const QString &memberId) {
    auto itFind = _mapLstWgtItem.find(memberId);
    if (itFind != _mapLstWgtItem.end()) {
        _plstModel->removeRow((*itFind)->row());
        _mapLstWgtItem.erase(itFind);
        _pTipLabel->setText(QString(DEM_TIP_MESSAGE).arg(_mapLstWgtItem.size()));
    }
    //
    std::string id = memberId.toStdString();
    if(!id.empty())
    {
        const auto &mapItems = _mapItems[id];
        for(const auto& it : mapItems)
        {
            auto *tmpItem = it.second;
            if(EM_STRUCTURE == it.first)
                tmpItem->setData(false, EM_SEARCH_DATATYPE_CHECKSTATE);
            else
                tmpItem->setData(false, EM_DATATYPE_CHECKSTATE);
        }
    }
}

void UIGroupManager::resetUi(const QString &memberId) {
    initRecentSessionData();

    _pGroupNameEdit->setText("");
    _pSearchWgt->clearText();
    //
    _plstModel->clear();
    _mapLstWgtItem.clear();
    _pTipLabel->setText(QString(DEM_TIP_MESSAGE).arg(_mapLstWgtItem.size()));
    //
    for(const auto& mapItem : _mapItems)
    {
        for(const auto& item : mapItem.second)
        {
            auto *tmpItem = item.second;
            if(item.first == EM_STRUCTURE)
                tmpItem->setData(false, EM_SEARCH_DATATYPE_CHECKSTATE);
            else
                tmpItem->setData(false, EM_DATATYPE_CHECKSTATE);
        }
    }
    //
    std::string id = memberId.toStdString();
    if(!id.empty() && _structure.find(id) != _structure.end())
    {
        auto info =_structure[id];
        addGroupMember(memberId,
                QTalk::GetHeadPathByUrl(info->HeaderSrc).data());
    }
    //
    _pRecentItem->setData(true, EM_STAFF_DATATYPE_EXTEND);
    _pFriendItem->setData(false, EM_STAFF_DATATYPE_EXTEND);
    _pStarItem->setData(false, EM_STAFF_DATATYPE_EXTEND);
    _pTreeWgt->setExpanded(_pRecentItem->index(), true);
    _pTreeWgt->setExpanded(_pFriendItem->index(), false);
    _pTreeWgt->setExpanded(_pStarItem->index(), false);
    _pTreeWgt->scrollTo(_pTreeModel->index(0, 0), QAbstractItemView::PositionAtTop);
}

void UIGroupManager::sendAddGroupMemberMessage(const QString &groupId, const QList<QString> &members, bool showTip) {
    if (nullptr == _pMsgManager)
        return;

    if(showTip && members.size() > 50)
    {
        int ret = QtMessageBox::question(this, "提示", QString("选择的群成员已超过50人，是否继续？"));
        if(ret == QtMessageBox::EM_BUTTON_NO)
            return;
    }

    std::vector<std::string> member;
    for(const QString &id : members) {
        member.push_back(id.toStdString());
    }
    _mapGroupMembers.remove(groupId);
    _pMsgManager->addGroupMember(member, groupId.toStdString());
}

void UIGroupManager::creatGroup() {
    if (nullptr == _pMsgManager)
        return;

    if(_mapLstWgtItem.size() > 50)
    {
        int ret = QtMessageBox::question(this, "提示", QString("选择的群成员已超过50人，是否继续？"));
        if(ret == QtMessageBox::EM_BUTTON_NO)
        {
            return;
        }
    }

    char uuid[36];
    QTalk::utils::generateUUID(uuid);
    QString groupId = QString("%1@conference.%2").arg(uuid).arg(Platform::instance().getSelfDomain().c_str());
    groupId.replace("-", "");
    groupId = groupId.toLower();
    _mapGroupMembers[groupId] = _mapLstWgtItem.keys();
    QString groupName = _pGroupNameEdit->text();
    if(groupName.isEmpty())
    {
        auto members = _mapGroupMembers[groupId];
        QString selfId = Platform::instance().getSelfXmppId().data();
        if(!members.contains(selfId))
            members.push_front(selfId);

        QFontMetricsF nameF(_pGroupNameEdit->font());
        for(const auto& mem : members)
        {
            groupName.append(_mapUserName[mem]);
            if(nameF.width(groupName) > 200)
            {
                groupName.append("..");
                break;
            }
            else
                groupName.append(",");
        }
        if(groupName.right(1) == ",")
            groupName.chop(1);
    }
    //
    _pMsgManager->creatGroup(groupId.toStdString(), groupName.toStdString());
}

void UIGroupManager::onCreatGroupRet(bool ret, const std::string &groupId) {
    QString gId = QString::fromStdString(groupId);
    if (ret && _mapGroupMembers.contains(gId)) {
        sendAddGroupMemberMessage(gId, _mapGroupMembers.value(gId), false);
    }
}

/**
 *
 * @param ids
 */
void UIGroupManager::onBatchAddMember(const QString &ids)
{
    QString tmpIds = ids;
    tmpIds.replace("\n", "");
    tmpIds.replace(" ", "");

    QStringList lstItems = tmpIds.split(";");
    QString domain = QString::fromStdString(Platform::instance().getSelfDomain());
    std::vector<std::string> arMembers;
    for(const auto& item : lstItems)
    {
        if(item.isEmpty()) continue;

        QString id = item
                .section("@", 0, 0)
                .append("@")
                .append(domain);
        std::string t = id.toStdString();
        arMembers.push_back(id.toStdString());

    }
    auto infos = dbPlatForm::instance().getGroupMemberInfo(arMembers);
    for(const auto& info : infos)
    {
        QString icon = QString::fromStdString(info.headerSrc);
        if(!icon.isEmpty())
        {
            icon = QString::fromStdString(QTalk::GetHeadPathByUrl(info.headerSrc));
        }
        if(icon.isEmpty() || !QFile::exists(icon) || QFileInfo(icon).isDir())
        {
#ifdef _STARTALK
            icon = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
            icon = ":/QTalk/image1/headPortrait.png";
#endif
        }

        addGroupMember(QString::fromStdString(info.xmppId), icon);
    }
}

QStandardItem* UIGroupManager::creatGroupItem(QStandardItem* item, const std::string& groupId)
{
    auto* subTitleItem = new QStandardItem;
    subTitleItem->setData(EM_ROW_TYPE_SUBTITLE, EM_STAFF_DATATYPE_ROW_TYPE);
    subTitleItem->setData(groupId.data(), EM_STAFF_DATATYPE_TEXT);
    subTitleItem->setData(groupId.data(), EM_STAFF_DATATYPE_XMPPID);
    subTitleItem->setData(false, EM_STAFF_DATATYPE_EXTEND);

    if(_groupInfos.find(groupId) != _groupInfos.end())
    {
        const QTalk::Entity::ImGroupInfo& info = _groupInfos[groupId];

#ifdef _STARTALK
        QString defaultGroupHead = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
        QString defaultGroupHead = ":/QTalk/image1/defaultGroupHead.png";
#endif
        QString iconPath = info.HeaderSrc.empty() ? defaultGroupHead :
                           QString::fromStdString(QTalk::GetHeadPathByUrl(info.HeaderSrc));

        if (!QFile::exists(iconPath) || QFileInfo(iconPath).isDir())
        {
            iconPath = defaultGroupHead;
        }

        subTitleItem->setData(info.Name.data(), EM_STAFF_DATATYPE_TEXT);
        subTitleItem->setData(iconPath, EM_STAFF_DATATYPE_ICONPATH);
    }

    item->appendRow(subTitleItem);

    return subTitleItem;
}


QStandardItem* UIGroupManager::creatItem(QStandardItem *item, const std::string &xmppId) {

    auto* mainItem = new QStandardItem;
    mainItem->setData(EM_ROW_TYPE_ITEM, EM_STAFF_DATATYPE_ROW_TYPE);
    mainItem->setData(xmppId.data(), EM_STAFF_DATATYPE_TEXT);
    mainItem->setData(xmppId.data(), EM_STAFF_DATATYPE_XMPPID);
    mainItem->setData(false, EM_DATATYPE_CHECKSTATE);

    std::shared_ptr<QTalk::Entity::ImUserInfo> userinfo;
    if(_structure.find(xmppId) == _structure.end() || nullptr == _structure[xmppId])
    {
        userinfo = std::make_shared<QTalk::Entity::ImUserInfo>();
        userinfo->XmppId = xmppId;
        if(_pMsgManager)
            _pMsgManager->getUserInfo(userinfo);

        if(userinfo)
        {
            _mapUserName[xmppId.data()] = QTalk::getUserName(userinfo).data();
            _structure[xmppId] = userinfo;
        }
    }
    else
        userinfo = _structure[xmppId];

    if(nullptr != userinfo)
    {
        QString defaultPath;
#ifdef _STARTALK
        defaultPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
        defaultPath = ":/QTalk/image1/headPortrait.png";
#endif
        QString iconPath = userinfo->HeaderSrc.empty() ? defaultPath :
                           QString::fromStdString(QTalk::GetHeadPathByUrl(userinfo->HeaderSrc));

        if (!QFile::exists(iconPath) || QFileInfo(iconPath).isDir())
        {
            iconPath = defaultPath;
        }

        std::string name = _mapUserName[xmppId.data()].toStdString();
        mainItem->setData(_mapUserName[xmppId.data()], EM_STAFF_DATATYPE_TEXT);
        mainItem->setData(iconPath, EM_STAFF_DATATYPE_ICONPATH);
    }
    item->appendRow(mainItem);

    return mainItem;
}


/**
 *
 */
void UIGroupManager::initGroupMembers()
{
    // title
    auto* titleItem = new QStandardItem;
    titleItem->setData("从群组选择", EM_STAFF_DATATYPE_TEXT);
    titleItem->setData(":/GroupManager/image1/groupList.png", EM_STAFF_DATATYPE_ICONPATH);
    titleItem->setData(EM_ROW_TYPE_TITLE, EM_STAFF_DATATYPE_ROW_TYPE);
    titleItem->setData(false, EM_STAFF_DATATYPE_EXTEND);
    _pTreeModel->appendRow(titleItem);

    // init all group
    for(const auto& group : _groupmemebers)
    {
        //
        std::string groupId = group.first;
        QStandardItem* groupItem = creatGroupItem(titleItem, groupId);
        //
        for(const auto& member : group.second)
        {
            _mapItems[member][EM_GROUPMEMEBER] = creatItem(groupItem, member);
        }
    }
}

void UIGroupManager::onRecvFriends(const std::vector<QTalk::Entity::IMFriendList> &friends)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _friends = friends;
    flags = (flags << 1);
    if(EM_ALL == flags)
            emit initDataFinish();
}

//
void UIGroupManager::initData()
{
    auto func = [this]() {
#ifdef _MACOS
        pthread_setname_np("UIGroupManager::getStructure");
#endif
        if (_pMsgManager) {
            {
                std::lock_guard<QTalk::util::spin_mutex> lock(sm);
                // 组织架构
                std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> structure;
                _pMsgManager->getStructure(structure);

                std::for_each(structure.begin(), structure.end(), [this](std::shared_ptr<QTalk::Entity::ImUserInfo> info) {
                    if(info)
                    {
                        _structure[info->XmppId] = info;
                        std::string name = QTalk::getUserNameNoMask(info);
                        _mapUserName[info->XmppId.data()] = name.data();
                    }
                });

                flags = (flags << 1);

                // 所有群以及群成员
                std::map<std::string, std::set<std::string>> mapGroupMembers;
                _pMsgManager->getGroupMembers(mapGroupMembers);
                if(!mapGroupMembers.empty())
                    _groupmemebers = mapGroupMembers;

                flags = (flags << 1);
                if(flags == EM_ALL )
                        emit initDataFinish();
            }
        }
    };
    //
    std::thread t(func);
    t.detach();
}

/**
 *
 * @param friends
 */
void UIGroupManager::onRecvGroupList(const std::vector<QTalk::Entity::ImGroupInfo>& groupInfos)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    for(const auto& info : groupInfos)
    {
        _groupInfos[info.GroupId] = info;
    }
    flags = (flags << 1);
    if(EM_ALL == flags)
            emit initDataFinish();
}

void UIGroupManager::updateUi()
{
    //
//    initGroupMembers();
    //
    initFriends();
    QApplication::processEvents();
    //
    initStarUser();
    QApplication::processEvents();
    //
    initRecentSession();
    QApplication::processEvents();
    //
    initStructure();
}

void UIGroupManager::initFriends()
{
    _pFriendItem = new QStandardItem;
    _pFriendItem->setData("从好友选择", EM_STAFF_DATATYPE_TEXT);
    _pFriendItem->setData(":/GroupManager/image1/friendList.png", EM_STAFF_DATATYPE_ICONPATH);
    _pFriendItem->setData(EM_ROW_TYPE_TITLE, EM_STAFF_DATATYPE_ROW_TYPE);
    _pFriendItem->setData(false, EM_STAFF_DATATYPE_EXTEND);
    //
    _pTreeModel->appendRow(_pFriendItem);
    //
    for(const auto& frie : _friends)
    {
        _mapItems[frie.XmppId][EM_FRIEND] = creatItem(_pFriendItem, frie.XmppId);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void UIGroupManager::updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs) {

    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    _arStarContact.clear();
    _arTopUsers.clear();

    auto it = arConfigs.begin();
    for (; it != arConfigs.end(); it++) {
        std::string subKey = it->ConfigSubKey;
        if (it->ConfigKey == "kStarContact") {
            _arStarContact.insert(subKey);
        }
        else if(it->ConfigKey == "kStickJidDic")
        {
            QString xmppId = QString::fromStdString(it->ConfigSubKey).section("<>", 1, 1);

            QJsonDocument jsonDocument = QJsonDocument::fromJson(it->ConfigValue.data());
            if (!jsonDocument.isNull()) {
                QJsonObject jsonObject = jsonDocument.object();
                int type = jsonObject.value("chatType").toInt();

                if(0 == type)
                    _arTopUsers.insert(xmppId.toStdString());
            }
        }

    }

    if(flags < EM_ALL)
    {
        flags = (flags << 1);
        if(flags == EM_ALL)
                emit initDataFinish();
    }
    else
    {
//        emit ;
    }
}


void UIGroupManager::updateUserConfig(const std::map<std::string, std::string> &deleteData,
                                      const std::vector<QTalk::Entity::ImConfig> &arImConfig) {

    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    for(const auto& it : deleteData)
    {
        if (it.second == "kStarContact") {
            if(_arStarContact.find(it.first) != _arStarContact.end())
                _arStarContact.erase(it.first);
        }
        else if(it.second == "kStickJidDic")
        {
            std::string xmppId = QString::fromStdString(it.first).section("<>", 1, 1).toStdString();
            if(_arTopUsers.find(xmppId) != _arTopUsers.end())
                _arTopUsers.erase(xmppId);
        }
    }
}

void UIGroupManager::initStarUser()
{
    _pStarItem = new QStandardItem;
    _pStarItem->setData("从星标联系人选择", EM_STAFF_DATATYPE_TEXT);
    _pStarItem->setData(":/GroupManager/image1/star.png", EM_STAFF_DATATYPE_ICONPATH);
    _pStarItem->setData(EM_ROW_TYPE_TITLE, EM_STAFF_DATATYPE_ROW_TYPE);
    _pStarItem->setData(false, EM_STAFF_DATATYPE_EXTEND);
    _pTreeModel->appendRow(_pStarItem);
    //
    for(const auto& star : _arStarContact)
    {
        _mapItems[star][EM_STAR] = creatItem(_pStarItem, star);
    }
}

void UIGroupManager::initRecentSession()
{
    _pRecentItem = new QStandardItem;
    _pRecentItem->setData("从最近联系人选择", EM_STAFF_DATATYPE_TEXT);
    _pRecentItem->setData(":/GroupManager/image1/Recent.png", EM_STAFF_DATATYPE_ICONPATH);
    _pRecentItem->setData(EM_ROW_TYPE_TITLE, EM_STAFF_DATATYPE_ROW_TYPE);
    _pRecentItem->setData(false, EM_STAFF_DATATYPE_EXTEND);
    //
    _pTreeModel->appendRow(_pRecentItem);
}

void UIGroupManager::initRecentSessionData()
{
    if(_pRecentItem)
    {
        static std::vector<QStandardItem*> recentItems;

        for(auto *item : recentItems)
        {
            // 移除缓冲数据
            if(item)
            {
                std::string id = item->data(EM_STAFF_DATATYPE_XMPPID).toString().toStdString();
                if(!id.empty() && _mapItems.find(id) != _mapItems.end())
                {
                    _mapItems[id].erase(EM_RECENT);
                }

                _pTreeModel->removeRow(item->row(), _pRecentItem->index());
            }
        }
        recentItems.clear();
        //
        if(_pMsgManager)
        {
            std::vector<QTalk::StShareSession> ss;
            _pMsgManager->getRecentSession(ss);

            std::set<std::string> recent = _arTopUsers;

            for(const auto& s : ss)
            {
                if(s.chatType == QTalk::Enum::TwoPersonChat && recent.find(s.xmppId) == recent.end())
                    recent.insert(s.xmppId);
            }
            //
            int r = 0;
            for(const auto& id : recent)
            {
                auto* item = creatItem(_pRecentItem, id);
                _mapItems[id][EM_RECENT] = item;
                recentItems.push_back(item);
            }
        }
    }
}

/**
 *
 */
void UIGroupManager::initStructure()
{
    static int index = 0;

    for(const auto& it : _structure)
    {
        if(++index == 10)
        {
            QApplication::processEvents();
            index = 0;
        }
        //
        const auto& info = it.second;
        if(info)
        {
            std::string xmppId = info->XmppId;

            auto* mainItem = new QStandardItem;
            mainItem->setData(xmppId.data(), EM_SEARCH_DATATYPE_XMPPID);
            mainItem->setData(QTalk::Entity::JID(xmppId.data()).username().data(),
                    EM_SEARCH_DATATYPE_USERID);
            mainItem->setData(false, EM_SEARCH_DATATYPE_CHECKSTATE);
            mainItem->setData(info->SearchIndex.data(), EM_SEARCH_DATATYPE_INDEX);

            QString defaultPath;
#ifdef _STARTALK
            defaultPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
            defaultPath = ":/QTalk/image1/headPortrait.png";
#endif
            QString iconPath = info->HeaderSrc.empty() ? defaultPath :
                               QString::fromStdString(QTalk::GetHeadPathByUrl(info->HeaderSrc));

            if (!QFile::exists(iconPath) || QFileInfo(iconPath).isDir())
            {
                iconPath = defaultPath;
            }

            mainItem->setData(_mapUserName[xmppId.data()], EM_SEARCH_DATATYPE_TEXT);
            mainItem->setData(iconPath, EM_SEARCH_DATATYPE_ICONPATH);
            mainItem->setData(info->DescInfo.data(), EM_SEARCH_DATATYPE_STAFF);
            mainItem->setData(info->DescInfo.data(), Qt::ToolTipRole);

            _mapItems[xmppId][EM_STRUCTURE] = mainItem;
            _pSearchSrcModel->appendRow(mainItem);
        }
    }
}

/**
 *
 * @param memberId
 */
void UIGroupManager::onItemDoubleClick(const QString& memberId, const QString& name)
{
    if(EM_TYPE_ADDGROUPMEMBER == _type)
    {
        int ret =QtMessageBox::question(this, "提示", QString("确认仅邀请<b>%1</b>进群？").arg(name));
        if(ret == QtMessageBox::EM_BUTTON_YES)
        {
            _mapLstWgtItem.clear();
            sendAddGroupMemberMessage(_strGroupId, QList<QString>() << memberId);
            this->setVisible(false);
        }
    }
}


void UIGroupManager::gotIncrementUser(const std::vector<QTalk::Entity::ImUserInfo> &arUserInfo,
                      const std::vector<std::string> &arDeletes)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    // delete
    for(const auto& id : arDeletes)
    {
        if(_structure.find(id) != _structure.end())
            _structure.erase(id);
        //
        if(_mapItems.find(id) != _mapItems.end() && _mapItems[id].find(EM_STRUCTURE) != _mapItems[id].end())
        {
            auto* item = _mapItems[id][EM_STRUCTURE];
            _pSearchSrcModel->removeRow(item->row());
            //
            _mapItems[id].erase(EM_STRUCTURE);
            if(_mapItems[id].empty())
                _mapItems.erase(id);
        }
    }
    // update -> delete first
    for(const auto& it : arUserInfo)
    {
        std::string id = it.XmppId;

        if(_structure.find(id) != _structure.end())
            _structure.erase(id);
        //
        if(_mapItems.find(id) != _mapItems.end() && _mapItems[id].find(EM_STRUCTURE) != _mapItems[id].end())
        {
            auto* item = _mapItems[id][EM_STRUCTURE];
            _pSearchSrcModel->removeRow(item->row());
            //
            _mapItems[id].erase(EM_STRUCTURE);
            if(_mapItems[id].empty())
                _mapItems.erase(id);
        }
    }
    // update -> add
    for(const auto& it : arUserInfo)
    {
        std::string xmppId = it.XmppId;

        _structure[xmppId] = std::make_shared<QTalk::Entity::ImUserInfo>(it);
        std::string name = QTalk::getUserNameNoMask(_structure[xmppId]);
        _mapUserName[xmppId.data()] = name.data();

        auto* mainItem = new QStandardItem;
        mainItem->setData(xmppId.data(), EM_SEARCH_DATATYPE_XMPPID);
        mainItem->setData(QTalk::Entity::JID(xmppId.data()).username().data(),
                          EM_SEARCH_DATATYPE_USERID);
        mainItem->setData(false, EM_SEARCH_DATATYPE_CHECKSTATE);
        mainItem->setData(it.SearchIndex.data(), EM_SEARCH_DATATYPE_INDEX);

        QString defaultPath;
#ifdef _STARTALK
        defaultPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
        defaultPath = ":/QTalk/image1/headPortrait.png";
#endif
        QString iconPath = it.HeaderSrc.empty() ? defaultPath :
                           QString::fromStdString(QTalk::GetHeadPathByUrl(it.HeaderSrc));

        if (!QFile::exists(iconPath) || QFileInfo(iconPath).isDir())
        {
            iconPath = defaultPath;
        }

        mainItem->setData(_mapUserName[xmppId.data()], EM_SEARCH_DATATYPE_TEXT);
        mainItem->setData(iconPath, EM_SEARCH_DATATYPE_ICONPATH);
        mainItem->setData(it.DescInfo.data(), EM_SEARCH_DATATYPE_STAFF);
        mainItem->setData(it.DescInfo.data(), Qt::ToolTipRole);

        _mapItems[xmppId][EM_STRUCTURE] = mainItem;
        _pSearchSrcModel->appendRow(mainItem);
    }

}