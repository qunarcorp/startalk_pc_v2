#include <memory>

#include "AddressBookPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <thread>
#include <QDebug>
#include <QHeaderView>
#include <QApplication>
#include <QFileInfo>
#include "ListItemView.h"
#include "MessageManager.h"
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Utils/utils.h"
#include "../Platform/dbPlatForm.h"

AddressBookPanel::AddressBookPanel(QWidget *parent)
        : QWidget(parent), _mainSplitter(nullptr), _pEmptyLabel(nullptr), _pUserCard(nullptr), _pstStrcuture(nullptr) {
    initUi();
    _pMsgManager = new AddressBookMsgManager;
    _pMsgListener = new AddressBookListener(this);
    //
    connect(this, &AddressBookPanel::updateUiSignal, this, &AddressBookPanel::updateUi);
    initStaff();
    updateStaffUi();
    getStructure();
}

AddressBookPanel::~AddressBookPanel() {

}

//
void AddressBookPanel::initUi() {

    setObjectName("AddressBookPanel");
    _mainSplitter = new QSplitter(this);
    // left frame
    QFrame *leftFrame = new QFrame(_mainSplitter);
    leftFrame->setObjectName("AddressBookLeftFrm");
    leftFrame->setMinimumWidth(260);
    _leftLay = new QVBoxLayout(leftFrame);
    _leftLay->setContentsMargins(0, 10, 0, 0);
    _leftLay->setSpacing(0);

    _mapNavItems[EM_ITEM_TYPE_START] = new NavigationItem(EM_ITEM_TYPE_START, this);
    _mapNavItems[EM_ITEM_TYPE_FRIENDLIST] = new NavigationItem(EM_ITEM_TYPE_FRIENDLIST, this);
    _mapNavItems[EM_ITEM_TYPE_GROUPLIST] = new NavigationItem(EM_ITEM_TYPE_GROUPLIST, this);
    _mapNavItems[EM_ITEM_TYPE_STAFF] = new NavigationItem(EM_ITEM_TYPE_STAFF, this);
//	_mapNavItems[EM_ITEM_TYPE_SUBSCRIPTION] = new NavigationItem(EM_ITEM_TYPE_SUBSCRIPTION, this);
    _mapNavItems[EM_ITEM_TYPE_BLACKLIST] = new NavigationItem(EM_ITEM_TYPE_BLACKLIST, this);

    _pStaffView = new QTreeView(this);
    _pStaffView->setObjectName("StaffView");
    _pStaffDelegate = new StaffDelegate(this);
    _pStaffView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _pStaffView->header()->setVisible(false);

    _pStaffView->setItemDelegate(_pStaffDelegate);
    _pStaffView->setFrameShape(QFrame::NoFrame);

    _mapItemWidgets[EM_ITEM_TYPE_START] = new ListItemView(this, leftFrame);
    _mapItemWidgets[EM_ITEM_TYPE_FRIENDLIST] = new ListItemView(this, leftFrame);
    _mapItemWidgets[EM_ITEM_TYPE_GROUPLIST] = new ListItemView(this, leftFrame);
    _mapItemWidgets[EM_ITEM_TYPE_BLACKLIST] = new ListItemView(this, leftFrame);
    _mapItemWidgets[EM_ITEM_TYPE_STAFF] = _pStaffView;

    dealNavItem(_leftLay);
    _leftLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding));
    // right frame
    QFrame *rightFrm = new QFrame(_mainSplitter);
    rightFrm->setObjectName("AddressBookRightFrm");
    _rightLay = new QStackedLayout(rightFrm);
    _pEmptyLabel = new QLabel(this);
    _pEmptyLabel->setObjectName("EmptyLabel");
    _pEmptyLabel->setPixmap(QPixmap(QString(":/addressbook/image1/Artboard_%1.png").arg(AppSetting::instance().getThemeMode())));
    _pEmptyLabel->setAlignment(Qt::AlignCenter);
    _rightLay->addWidget(_pEmptyLabel);
    // main layout
    _mainSplitter->addWidget(leftFrame);
    _mainSplitter->addWidget(rightFrm);
    _mainSplitter->setHandleWidth(1);
    _mainSplitter->setStretchFactor(1, 1);
    _mainSplitter->setCollapsible(0, false);
    _mainSplitter->setCollapsible(1, false);
    auto *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_mainSplitter);

    connect(_pStaffDelegate, &StaffDelegate::itemClicked, this, &AddressBookPanel::onStaffItemClicked);

}

/**
 *
 * @param sto
 * @param index
 * @return
 */
QString getStrName(QStringList sto, int index) {
    QString ret = "";
    if (index <= 0 || index >= sto.size())
        return ret;

    for (int i = 1; i <= index; i++) {
        ret += "/" + sto[i];
    }
    return ret;
}

/**
 * 初始化组织架构
 */
void AddressBookPanel::initStaff() {
    if (nullptr == _pMsgManager) return;
    //
    _pStaffStructure = new StaffStructure(this);
    _rightLay->addWidget(_pStaffStructure);
    _pStaffModel = new QStandardItemModel;
    connect(_pStaffStructure, &StaffStructure::creatGroupSignal, this, &AddressBookPanel::creatGroup);
//    connect(this, &AddressBookPanel::updateStaffUiSinal, this, &AddressBookPanel::updateStaffUi);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/12/22
  */
void AddressBookPanel::updateStaffUi() {
    _pStaffView->setModel(_pStaffModel);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void AddressBookPanel::onNavItemClicked(QUInt8 type)
{
    static QUInt8 old_type = EM_ITEM_TYPE_EMPTY;

    if(type == old_type)
    {
        auto item = _mapNavItems[type];
        _mapNavItems[type]->setSelectState(!item->getItemWgt()->isVisible());
    }
    else
    {
        old_type = type;

        for(const auto item : _mapNavItems) {
            bool isSelectItem = item->getItemType() == type;
            auto *lstView = qobject_cast<ListItemView *>(item->getItemWgt());
            if (isSelectItem) {
                if (lstView)
                {
#ifdef _STARTALK
                    switch (type)
                {
                    case EM_ITEM_TYPE_START:
                        _pEmptyLabel->setPixmap(QPixmap(":/addressbook/image1/star_default.png"));
                        break;
                    case EM_ITEM_TYPE_FRIENDLIST:
                        _pEmptyLabel->setPixmap(QPixmap(":/addressbook/image1/friend_default.png"));
                        break;
                    case EM_ITEM_TYPE_GROUPLIST:
                        _pEmptyLabel->setPixmap(QPixmap(":/addressbook/image1/group_default.png"));
                        break;
                    case EM_ITEM_TYPE_BLACKLIST:
                        _pEmptyLabel->setPixmap(QPixmap(":/addressbook/image1/balck_list_default.png"));
                        break;
                    default:
                        break;
                }
#endif
                    lstView->resetHeight(this->height(), getFixedHeight());
                }
                else if (item->getItemWgt() == _pStaffView)
                {
                    emit sgOpeartor("组织架构");
#ifdef _STARTALK
                    _pEmptyLabel->setPixmap(QPixmap(":/addressbook/image1/staff_default.png"));
#endif
                    _pStaffView->setFixedHeight(this->height() - getFixedHeight());
                }

#ifndef _STARTALK
                _pEmptyLabel->setPixmap(QPixmap(QString(":/addressbook/image1/Artboard_%1.png").arg(AppSetting::instance().getThemeMode())));
#endif

            } else {
                if (lstView)
                    lstView->clearSelection();

            }
            item->setSelectState(isSelectItem);
        }
    }
    _rightLay->setCurrentWidget(_pEmptyLabel);
}


/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void AddressBookPanel::updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs) {
    QVector<std::string> oldStarContact = _arStarContact;
    QVector<std::string> oldBlackList = _arBlackList;

    _arStarContact.clear();
    _arBlackList.clear();
    _mapMaskNames.clear();

    auto it = arConfigs.begin();
    for (; it != arConfigs.end(); it++) {
        std::string subKey = it->ConfigSubKey;
        if (it->ConfigKey == "kStarContact") {
            if (oldStarContact.contains(subKey))
                oldStarContact.removeOne(subKey);
            //
            _arStarContact.push_back(subKey);
        } else if (it->ConfigKey == "kBlackList") {
            if (oldBlackList.contains(subKey))
                oldBlackList.removeOne(subKey);
            //
            _arBlackList.push_back(subKey);
        } else if (it->ConfigKey == "kMarkupNames") {
            _mapMaskNames[subKey] = it->ConfigValue;
        }
    }
    //
    addItemByType(EM_ITEM_TYPE_START, _arStarContact);
    addItemByType(EM_ITEM_TYPE_BLACKLIST, _arBlackList);

    removeItemByType(EM_ITEM_TYPE_START, oldStarContact);
    removeItemByType(EM_ITEM_TYPE_BLACKLIST, oldBlackList);
}

void AddressBookPanel::updateUserConfig(const std::map<std::string, std::string> &deleteData,
                                        const std::vector<QTalk::Entity::ImConfig>& arImConfig) {

    {
        QVector<std::string> delStarContact;
        QVector<std::string> delBlackList;
        for(const auto& it : deleteData)
        {
            if (it.second == "kStarContact") {
                delStarContact.push_back(it.first);
            } else if (it.second == "kBlackList") {
                delBlackList.push_back(it.first);
            } else if (it.second == "kMarkupNames") {
                if (_mapMaskNames.contains(it.first))
                    _mapMaskNames.remove(it.first);
            }
        }
        if(!delStarContact.empty())
            removeItemByType(EM_ITEM_TYPE_START, delStarContact);
        if(delBlackList.empty())
            removeItemByType(EM_ITEM_TYPE_BLACKLIST, delBlackList);
    }

    //
    {

        QVector<std::string> addStarContact;
        QVector<std::string> addBlackList;
        for(const auto& it : arImConfig)
        {
            if (it.ConfigKey == "kStarContact") {
                addStarContact.push_back(it.ConfigSubKey);
            } else if (it.ConfigKey == "kBlackList") {
                addBlackList.push_back(it.ConfigSubKey);
            } else if (it.ConfigKey == "kMarkupNames") {
                _mapMaskNames[it.ConfigSubKey] = it.ConfigValue;
            }
        }
        if(!addStarContact.empty())
            addItemByType(EM_ITEM_TYPE_START, addStarContact);
        if(addBlackList.empty())
            addItemByType(EM_ITEM_TYPE_BLACKLIST, addBlackList);
    }
}


/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void AddressBookPanel::dealNavItem(QVBoxLayout *layout) {
    for(NavigationItem *item : _mapNavItems)
    {
        if (nullptr == item) continue;

        layout->addWidget(item);
        QWidget *wgt = _mapItemWidgets.value(item->getItemType());
        if (wgt) {
            wgt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            item->registerWgt(wgt);
            layout->addWidget(wgt);
            wgt->setVisible(false);
        }

        connect(item, &NavigationItem::itemClicked, this, &AddressBookPanel::onNavItemClicked);
    }
}

//
void AddressBookPanel::addItemByType(QUInt8 type, const QVector<std::string> &users) {
    auto func = [this, type, users]() {
#ifdef _MACOS
        pthread_setname_np("AddressBookPanel::addItemByType");
#endif
        QMutexLocker locker(&_mutex);
        ListItemView *listview = (ListItemView *) _mapItemWidgets.value(type);

        if (nullptr != listview) {
            for (const std::string& id : users) {
                std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo = dbPlatForm::instance().getUserInfo(id);

                if(nullptr == userInfo)
                {
                    userInfo = std::make_shared<QTalk::Entity::ImUserInfo>();
                    userInfo->XmppId = id;
                    _pMsgManager->getUserInfo(userInfo);
                }

                if (nullptr != userInfo) {

#ifdef _STARTALK
                    QString defaltHead = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
                    QString defaltHead = ":/QTalk/image1/headPortrait.png";
#endif
                    QString iconPath = userInfo->HeaderSrc.empty() ? defaltHead :
                                       QString::fromStdString(QTalk::GetHeadPathByUrl(userInfo->HeaderSrc));
                    listview->addItem(QString::fromStdString(id), EM_TYPE_USER, iconPath,
                                      QString::fromStdString(QTalk::getUserName(userInfo)));
                }
            }
        }
    };

    std::thread t(func);
    t.detach();
}

/**
 *
 * @param type
 * @param users
 */
void AddressBookPanel::removeItemByType(QUInt8 type, const QVector<std::string> &users) {
    auto func = [this, type, users]() {
        QMutexLocker locker(&_mutex);
        ListItemView *listview = (ListItemView *) _mapItemWidgets.value(type);
        if (nullptr != listview) {
            for (std::string id : users) {
                listview->removeItem(QString::fromStdString(id));
            }
        }
    };

    std::thread t(func);
    t.detach();
}

/**
 *
 * @param friends
 */
void AddressBookPanel::onRecvFriends(const std::vector<QTalk::Entity::IMFriendList> &friends) {
    for (const QTalk::Entity::IMFriendList &imfriend : friends) {
        _arFriends.push_back(imfriend.XmppId);
    }

    addItemByType(EM_ITEM_TYPE_FRIENDLIST, _arFriends);
}

/**
 *
 * @param groups
 */
void AddressBookPanel::onRecvGroups(const std::vector<QTalk::Entity::ImGroupInfo> &groups) {
    //_arGroups.fromStdVector(groups);
    std::thread t([this, groups]() {
#ifdef _MACOS
        pthread_setname_np("AddressBookPanel::onRecvGroups");
#endif
        QMutexLocker locker(&_mutex);

        ListItemView *blkListview = (ListItemView *) _mapItemWidgets.value(EM_ITEM_TYPE_GROUPLIST);

        if (nullptr == blkListview) return;

        for (const QTalk::Entity::ImGroupInfo &group : groups) {
            _arGroups.push_back(group);
#ifdef _STARTALK
            QString defaultGroupHead = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
            QString defaultGroupHead = ":/QTalk/image1/defaultGroupHead.png";
#endif
            QString iconPath = group.HeaderSrc.empty() ? defaultGroupHead :
                               QString::fromStdString(QTalk::GetHeadPathByUrl(group.HeaderSrc));
            if (!QFile::exists(iconPath) || QFileInfo(iconPath).isDir())
            {
                iconPath = defaultGroupHead;
            }
            blkListview->addItem(QString::fromStdString(group.GroupId), EM_TYPE_GROUP, iconPath,
                                 QString::fromStdString(group.Name));
        }
    });

    t.detach();
}

/**
 *
 * @return
 */
int AddressBookPanel::getFixedHeight() {
    return _mapNavItems.size() * 50 + 10;
}

/**
 *
 * @param id
 * @param type
 */
void AddressBookPanel::onListItemClicked(const QString &id, const QUInt8 &type) {
    // type
    switch (type) {
        case EM_TYPE_USER: {
            showUserCard(id);
            break;
        }
        case EM_TYPE_GROUP:
        {
            StSessionInfo stSession(QTalk::Enum::GroupChat, id, "");
            emit sgSwitchCurFun(0);
            emit sgOpenNewSession(stSession);

            break;
        }
        default:break;
    }
}

/**
 *
 * @param id 用户xmppid
 */
void AddressBookPanel::showUserCard(const QString &id) {
    if (_pMsgManager) {
        _imuserSup = std::make_shared<QTalk::Entity::ImUserSupplement>();
        _userInfo = std::make_shared<QTalk::Entity::ImUserInfo>();
        _userInfo->XmppId = id.toStdString();
        _imuserSup->XmppId = id.toStdString();
        _pMsgManager->getUserCard(_imuserSup, _userInfo);
        if (nullptr == _imuserSup)
            return;

        if (nullptr == _pUserCard) {
            _pUserCard = new UserCard(this);
        }
        _rightLay->addWidget(_pUserCard);
        _pUserCard->showUserCard(_imuserSup, _userInfo);

        int flags = _arStarContact.contains(_imuserSup->XmppId);
        flags |= _arBlackList.contains(_imuserSup->XmppId) << 1;
        flags |= _arFriends.contains(_imuserSup->XmppId) << 2;
        _pUserCard->setFlags(flags);
        if (_mapMaskNames.contains(_imuserSup->XmppId)) {
            _pUserCard->setMaskName(QString::fromStdString(_mapMaskNames[_imuserSup->XmppId]));
        }
        _rightLay->setCurrentWidget(_pUserCard);
    }
}

/**
 * 显示群
 * @param id 群id
 */
void AddressBookPanel::showGroupCard(const QString &id) {

}

/**
 *
 * @param userId
 */
void AddressBookPanel::getPhoneNo(const std::string &userId) {
    std::thread t([this, userId]() {
#ifdef _MACOS
        pthread_setname_np("AddressBookPanel::getPhoneNo");
#endif
        QMutexLocker locker(&_mutex);
        if (nullptr != _pMsgManager) {
            std::string phoneNo;
            _pMsgManager->getUserPhoneNo(userId, phoneNo);

            if (!phoneNo.empty()) {
                emit gotPhoneNo(userId, phoneNo);
            }
        }

    });
    t.detach();
}

/**
 * 星标联系人
 * @param userId
 */
void AddressBookPanel::starUser(const std::string &userId) {
    bool isStar = _arStarContact.contains(userId);
    QString val = QString::number(isStar ? 0 : 1);
    if (_pMsgManager) {
        _pMsgManager->setUserSetting(isStar, "kStarContact", userId, val.toStdString());
    }
    _rightLay->setCurrentWidget(_pEmptyLabel);
}

void AddressBookPanel::addBlackList(const std::string &userId) {
    bool isBlack = _arBlackList.contains(userId);
    QString val = QString::number(isBlack ? 0 : 1);
    if (_pMsgManager) {
        _pMsgManager->setUserSetting(isBlack, "kBlackList", userId, val.toStdString());
    }
    _rightLay->setCurrentWidget(_pEmptyLabel);
}

/**
 * 建群
 * @param structure
 * @param count
 */
void AddressBookPanel::creatGroup(const QString &structure, const QString &groupName) {
    // 获取群成员
    std::vector<std::string> arMembers;
    _pMsgManager->getStructureMembers(structure.toStdString(), arMembers);
    if (!arMembers.empty()) {
        // 发送建群请求
        QString groupId = QString("%1@conference.%2").arg(QTalk::utils::getMessageId().c_str()).
                arg(Platform::instance().getSelfDomain().c_str());
        groupId = groupId.replace("-", "");
        mapGroupMembers[groupId.toStdString()] = arMembers;
        //
        QString realName = groupName;
        if (realName.isEmpty()) {
            realName = structure.mid(1);
        }
        _pMsgManager->creatGroup(groupId.toStdString(), realName.toStdString());
    }
}

///**
//  * @函数名   sysTreeData
//  * @功能描述 同步数据
//  * @参数
//     void
//  * @author   cc
//  * @date     2018/12/23
//  */
//void AddressBookPanel::sysTreeData(void *model) {
//    _pStaffModel = (QStandardItemModel *) model;
//}

void AddressBookPanel::onCreatGroupRet(const std::string &groupId) {
    auto it = mapGroupMembers.find(groupId);
    if (it != mapGroupMembers.end()) {
        _pMsgManager->addGroupMember(*it, groupId);
        mapGroupMembers.erase(it);
    }
}

/**
 *
 */
void AddressBookPanel::jumpToUserStructre(const QString &useId) {
    //_mapItemWidgets[EM_ITEM_TYPE_STAFF];
#ifdef _STARTALK
    if (_mapNavItems.contains(EM_ITEM_TYPE_STAFF) && _mapNavItems[EM_ITEM_TYPE_STAFF]) {
        onNavItemClicked(EM_ITEM_TYPE_STAFF);
        _mapNavItems[EM_ITEM_TYPE_STAFF]->setSelectState(true);
    }
    //
    if (_mapItemWidgets.contains(EM_ITEM_TYPE_STAFF)) {
        QWidget *wgt = _mapItemWidgets[EM_ITEM_TYPE_STAFF];
        if (nullptr != wgt && wgt == _pStaffView && _pStaffModel) {
            _pShowItem = nullptr;
            _pStaffView->collapseAll();
            //
            int rowCount = _pStaffModel->rowCount();
            for (int row = 0; row < rowCount; ++row) {
                QStandardItem *tmpItem = _pStaffModel->item(row);
                getShowItem(tmpItem, useId);
                if (nullptr != _pShowItem) {
                    break;
                }
            }
            if (nullptr != _pShowItem) {
                parentShow(_pShowItem);
                _pStaffView->scrollTo(_pShowItem->index(), QAbstractItemView::PositionAtTop);
                onStaffItemClicked(_pShowItem->index());
                _pStaffView->setCurrentIndex(_pShowItem->index());
            }
        }
    }
#endif
}

/**
 *
 * @param parentItem
 * @param userId
 */
void AddressBookPanel::getShowItem(QStandardItem *parentItem, const QString &userId) {
    if (nullptr != _pShowItem) {
        return;
    }

    int rowCount = parentItem->rowCount();
    for (int row = 0; row < rowCount; row++) {
        QStandardItem *tmpItem = parentItem->child(row, 0);
        if (nullptr == tmpItem) continue;
        //
        if (tmpItem->rowCount() > 0) {
            getShowItem(tmpItem, userId);
        } else {
            QString xmppId = tmpItem->data(EM_STAFF_DATATYPE_XMPPID).toString();

            if (xmppId == userId) {
                _pShowItem = tmpItem;
                break;
            }
        }
    }
}

void AddressBookPanel::parentShow(QStandardItem *item) {
    QStandardItem *tmpItem(item);
    QList<QStandardItem *> aritems;
    while (tmpItem->parent() != nullptr) {
        tmpItem = tmpItem->parent();
        aritems.push_front(tmpItem);
    }
            foreach(QStandardItem *i, aritems) {
            _pStaffView->expand(i->index());
        }
}

void AddressBookPanel::onStaffItemClicked(const QModelIndex &index) {
    QString xmppid = index.data(EM_STAFF_DATATYPE_XMPPID).toString();
    bool hasChild = index.data(EM_STAFF_DATATYPE_HASCHILD).toBool();
    if (hasChild) {
        QString structureName = index.data(EM_STAFF_DATATYPE_STRUCTURE).toString();
        QString text = index.data(EM_STAFF_DATATYPE_TEXT).toString();

        if (_pMsgManager) {
            int count = 0;
            _pMsgManager->getStructureCount(QString("%1/%2").arg(structureName, text).toStdString(), count);
            _pStaffStructure->setData(structureName, text, count);
            _rightLay->setCurrentWidget(_pStaffStructure);
        }
    } else {
        showUserCard(xmppid);
    }
}

/**
 *
 */
void AddressBookPanel::onJumpToStructre(const QString &strureName)
{
#ifdef _STARTALK
    if (_mapNavItems.contains(EM_ITEM_TYPE_STAFF) && _mapNavItems[EM_ITEM_TYPE_STAFF]) {
        onNavItemClicked(EM_ITEM_TYPE_STAFF);
        _mapNavItems[EM_ITEM_TYPE_STAFF]->setSelectState(true);
    }
    //
    if (_mapItemWidgets.contains(EM_ITEM_TYPE_STAFF)) {
        QWidget *wgt = _mapItemWidgets[EM_ITEM_TYPE_STAFF];
        if (nullptr != wgt && wgt == _pStaffView && _pStaffModel) {
            _pShowItem = nullptr;
            _pStaffView->collapseAll();
            //
            int rowCount = _pStaffModel->rowCount();
            for (int row = 0; row < rowCount; ++row) {
                QStandardItem *tmpItem = _pStaffModel->item(row);

                getShowItemByStructre(tmpItem, strureName);
                if (nullptr != _pShowItem) {
                    break;
                }
            }
            if (nullptr != _pShowItem) {
                parentShow(_pShowItem);
                _pStaffView->scrollTo(_pShowItem->index(), QAbstractItemView::PositionAtTop);
                onStaffItemClicked(_pShowItem->index());
                _pStaffView->setCurrentIndex(_pShowItem->index());
            }
        }
    }
#endif
}

void AddressBookPanel::getShowItemByStructre(QStandardItem* parentItem, const QString& structre)
{
    if (nullptr != _pShowItem) {
        return;
    }

    QString structureName = parentItem->data(EM_STAFF_DATATYPE_STRUCTURE).toString();
    QString text = parentItem->data(EM_STAFF_DATATYPE_TEXT).toString();
    QString tmpStructure = structureName.isEmpty() ?
                           QString("/%1").arg(text) :
                           QString("%1/%2").arg(structureName, text);

    if(tmpStructure == structre)
    {
        _pShowItem = parentItem;
        return;
    }

    //
    int rowCount = parentItem->rowCount();
    for (int row = 0; row < rowCount; row++) {
        QStandardItem *tmpItem = parentItem->child(row, 0);
        if (nullptr == tmpItem) continue;
        //
        bool hasChild = tmpItem->data(EM_STAFF_DATATYPE_HASCHILD).toBool();

        if (hasChild)
        {
            getShowItemByStructre(tmpItem, structre);
        }
    }
}

void AddressBookPanel::onDestroyGroupRet(const std::string &groupId)
{
    ListItemView *blkListview = (ListItemView *) _mapItemWidgets.value(EM_ITEM_TYPE_GROUPLIST);
    if (nullptr == blkListview) return;
    blkListview->removeItem(QString::fromStdString(groupId));
}

void AddressBookPanel::getStructure() {

    auto func = [this]() {
#ifdef _MACOS
        pthread_setname_np("UIGroupManager::getStructure");
#endif
        if (_pMsgManager) {
            {
                std::lock_guard<QTalk::util::spin_mutex> lock(sm);
                std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> structure;
                _pMsgManager->getStructure(structure);

                _pstStrcuture = new StStrcuture;

                std::for_each(structure.begin(), structure.end(), [this](std::shared_ptr<QTalk::Entity::ImUserInfo> info) {

                    if(info->isVisible)
                    {
                        StStrcuture *tmp = _pstStrcuture;
                        QStringList sto = QString::fromStdString(info->DescInfo).split("/");

                        for (int i = 0; i < sto.size(); i++)
                        {
                            const QString &o = sto.at(i);

                            if (o.isEmpty()) continue;

                            if (nullptr == tmp->mapChild[o])
                                tmp->mapChild[o] = new StStrcuture;

                            tmp = tmp->mapChild[o];
                            tmp->curStrcutureName = getStrName(sto, i);
                            if (i == sto.size() - 1) {
                                tmp->oUsers.push_back(info);
                            }
                        }
                    }
                });
            }

            emit updateUiSignal();
        }
    };
    //
    std::thread t(func);
    t.detach();
}

void AddressBookPanel::updateUi() {

    if (nullptr != _pstStrcuture) {

        if(_pstStrcuture->mapChild.empty())
        {
            _pStaffView->setVisible(false);
            _mapNavItems[EM_ITEM_TYPE_STAFF]->setVisible(false);
        }
        //
        auto it = _pstStrcuture->mapChild.begin();
        while (it != _pstStrcuture->mapChild.end()) {

            auto *item = new QStandardItem();
            item->setData(it->first, EM_STAFF_DATATYPE_TEXT);
            item->setData(":/GroupManager/image1/structure.png", EM_STAFF_DATATYPE_ICONPATH);
            item->setData(_pstStrcuture->curStrcutureName, EM_STAFF_DATATYPE_STRUCTURE);
            item->setData(true, EM_STAFF_DATATYPE_HASCHILD);

            _pStaffModel->appendRow(item);
            //
            it->second->item = item;

            creatChildItem(it->second, item);
            //
            it++;
        }

//        _pTreeWgt->setColumnWidth(EM_COLUMNTYPE_COMTENT, 230);
//        _pTreeWgt->setColumnWidth(EM_COLUMNTYPE_CHECKBOX, 30);
    }
//    emit setTreeDataFinised();
}

//
void AddressBookPanel::creatChildItem(const StStrcuture *os, QStandardItem *parentItem) {

    static int index = 0;
    if(++index == 10)
    {
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        index = 0;
    }

    if (os) {
        auto it = os->mapChild.begin();
        while (it != os->mapChild.end()) {

            auto *item = new QStandardItem();
            item->setData(it->first, EM_STAFF_DATATYPE_TEXT);
            item->setData(":/GroupManager/image1/structure.png", EM_STAFF_DATATYPE_ICONPATH);
            item->setData(true, EM_STAFF_DATATYPE_HASCHILD);
            item->setData(os->curStrcutureName, EM_STAFF_DATATYPE_STRUCTURE);
            //
            parentItem->appendRow(item);

            it->second->item = item;
            //
            creatChildItem(it->second, item);
            //
            it++;
        }

        auto itu = os->oUsers.begin();
        while (itu != os->oUsers.end()) {

            QStandardItem *item = new QStandardItem(QString((*itu)->Name.c_str()));
            QString headrSrc = QString(QTalk::GetHeadPathByUrl((*itu)->HeaderSrc).c_str());
            if (QFileInfo(headrSrc).isDir() || !QFile::exists(headrSrc)) {
#ifdef _STARTALK
                headrSrc = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
                headrSrc = ":/QTalk/image1/headPortrait.png";
#endif
            }
            item->setData(QString::fromStdString((*itu)->Name), EM_STAFF_DATATYPE_TEXT);
            item->setData(headrSrc, EM_STAFF_DATATYPE_ICONPATH);
            item->setData(false, EM_STAFF_DATATYPE_HASCHILD);
            item->setData(QString((*itu)->XmppId.c_str()), EM_STAFF_DATATYPE_XMPPID);
            item->setData(QString((*itu)->SearchIndex.c_str()), EM_DATATYPE_SEARCHKEY);
            auto *checkItem = new QStandardItem();
            checkItem->setData(false, EM_DATATYPE_CHECKSTATE);

            parentItem->appendRow(QList<QStandardItem *>() << item << checkItem);

            _staffItems[(*itu)->XmppId] = item;

            itu++;
        }
    }
}


void AddressBookPanel::gotIncrementUser(const std::vector<QTalk::Entity::ImUserInfo> &arUserInfo,
                                      const std::vector<std::string> &arDeletes)
{
    // todo 先全部刷 不考虑增量
//    // add
//    for(const auto& it : arUserInfo)
//    {
//        std::string xmppId = it.XmppId;
//
//        QStringList sto = QString::fromStdString(it.DescInfo).split("/");
//
//        if(nullptr == _pstStrcuture) break;
//
//        StStrcuture *tmp = _pstStrcuture;
//        for (int i = 0; i < sto.size(); i++)
//        {
//            const QString &o = sto.at(i);
//
//            if (o.isEmpty()) continue;
//
//            if (nullptr == tmp->mapChild[o])
//            {
//                tmp->mapChild[o] = new StStrcuture;
//                tmp->mapChild[o]->curStrcutureName = getStrName(sto, i);
//                //
//                auto *item = new QStandardItem();
//                item->setData(o, EM_STAFF_DATATYPE_TEXT);
//                item->setData(":/GroupManager/image1/structure.png", EM_STAFF_DATATYPE_ICONPATH);
//                item->setData(true, EM_STAFF_DATATYPE_HASCHILD);
//                item->setData(getStrName(sto, i), EM_STAFF_DATATYPE_STRUCTURE);
//                //
//                if(tmp->item)
//                    tmp->item->appendRow(item);
//                else
//                    _pStaffModel->appendRow(item);
//
//                tmp->mapChild[o]->item = item;
//            }
//
//            tmp = tmp->mapChild[o];
//
//            if (i == sto.size() - 1) {
//                auto info = std::make_shared<QTalk::Entity::ImUserInfo>(it);
//                tmp->oUsers.push_back(info);
//                //
//                QStandardItem *item = new QStandardItem(QString(info->Name.c_str()));
//                QString headrSrc = QString(QTalk::GetHeadPathByUrl(info->HeaderSrc).c_str());
//                if (QFileInfo(headrSrc).isDir() || !QFile::exists(headrSrc)) {
//#ifdef _STARTALK
//                    headrSrc = ":/QTalk/image1/StarTalk_defaultHead.png";
//#else
//                    headrSrc = ":/QTalk/image1/headPortrait.png";
//#endif
//                }
//                item->setData(QString::fromStdString(info->Name), EM_STAFF_DATATYPE_TEXT);
//                item->setData(headrSrc, EM_STAFF_DATATYPE_ICONPATH);
//                item->setData(false, EM_STAFF_DATATYPE_HASCHILD);
//                item->setData(QString(info->XmppId.c_str()), EM_STAFF_DATATYPE_XMPPID);
//                item->setData(QString(info->SearchIndex.c_str()), EM_DATATYPE_SEARCHKEY);
//                auto *checkItem = new QStandardItem();
//                checkItem->setData(false, EM_DATATYPE_CHECKSTATE);
//
//                if(tmp->item)
//                    tmp->item->appendRow(QList<QStandardItem *>() << item << checkItem);
//                else
//                    _pStaffModel->appendRow(QList<QStandardItem *>() << item << checkItem);
//
//                _staffItems[info->XmppId] = item;
//            }
//        }
//    }
//    // delete
//    for(const auto& id : arDeletes)
//    {
//        //
//        if(_staffItems.find(id) != _staffItems.end())
//        {
//            auto* item = _staffItems[id];
//            _pStaffModel->removeRow(item->row(), item->parent()->index());
//            //
//            _staffItems.erase(id);
//        }
//    }
    // 清楚数据
    delete _pstStrcuture;
    _pstStrcuture = nullptr;
    _pStaffModel->clear();
    // 重新刷新
    getStructure();
}