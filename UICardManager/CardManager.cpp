#include <memory>
#include "CardManager.h"
#include "UserCard.h"
#include "GroupCard.h"
#include <QFile>
#include "UserCardMsgManager.h"
#include "../UICom/uicom.h"
#include "../Platform/Platform.h"

CardManager::CardManager()
        : _pUserCard(nullptr), _groupCard(nullptr) {
    _pMsgManager = new UserCardMsgManager;
    _pMsgListener = new UserCardMessageListener(this);
    connect(this, &CardManager::showUserCardSignal, this, &CardManager::showUserCardSlot);
    connect(this, &CardManager::showGroupCardSignal, this, &CardManager::showGroupCardSlot);
    qRegisterMetaType<std::map<std::string, QTalk::StUserCard> >("std::map<std::string, QTalk::StUserCard>");
    qRegisterMetaType<std::map<std::string, QUInt8> >("std::map<std::string, QUInt8>");
    connect(this, &CardManager::sgUpdateGroupMember, this, &CardManager::updateGroupMember);
}

CardManager::~CardManager() = default;

void CardManager::shwoUserCard(const QString &userId) {
    _pUserCard = new user_card(this);
    connect(_pUserCard, &user_card::sgJumpToStructre, [this](const QString &structreName) {
        emit sgSwitchCurFun(1);
        emit sgJumpToStructre(structreName);
        _pUserCard->setVisible(false);
    });
    std::thread([this, userId]() {
#ifdef _MACOS
        pthread_setname_np("CardManager::shwoUserCard");
#endif
        {
            std::lock_guard<QTalk::util::spin_mutex> lock(sm);
            if (_pMsgManager) {
                _imuserSup = std::make_shared<QTalk::Entity::ImUserSupplement>();
                _userInfo = std::make_shared<QTalk::Entity::ImUserInfo>();
                _userInfo->XmppId = userId.toStdString();
                _imuserSup->XmppId = userId.toStdString();
                _pMsgManager->getUserCard(_imuserSup, _userInfo);
            }
        }
        //
        _user_medal.clear();
        if(_pMsgManager)
            _pMsgManager->getUserMedal(userId.toStdString(), _user_medal);

        if (nullptr != _imuserSup && _pMsgManager) {
            emit showUserCardSignal();
        }

    }).detach();
}

/**
 *
 * @param groupId
 */
void CardManager::showGroupCard(const QString &groupId) {
    _groupCard = new GroupCard(this);
    if (_pMsgManager) {
        //
        _imGroupSup = std::make_shared<QTalk::Entity::ImGroupInfo>();
        _imGroupSup->GroupId = groupId.toStdString();
        _pMsgManager->getGroupMembers(groupId.toStdString());
        _pMsgManager->getGroupCard(_imGroupSup);
        if (_imGroupSup) {
            emit showGroupCardSignal();
        }
    }
}

/**
 *
 */
void CardManager::showUserCardSlot() {

    if (nullptr != _pUserCard) {
        std::string usrId = _imuserSup->XmppId;

        std::lock_guard<QTalk::util::spin_mutex> lock(sm);
        int flags = _arStarContact.contains(usrId);
        flags |= _arBlackList.contains(usrId) << 1;
        flags |= _arFriends.contains(usrId) << 2;
        _pUserCard->setFlags(flags);
        _pUserCard->showUserCard(_imuserSup, _userInfo);
        if (_mapMaskNames.contains(usrId)) {
            _pUserCard->setMaskName(QString::fromStdString(_mapMaskNames[usrId]));
        }
        //
        _pUserCard->showMedal(_user_medal);
        //
        _pUserCard->adjustSize();
        _pUserCard->showCenter(false, nullptr);
		_pUserCard->resize(_pUserCard->width() + 10, _pUserCard->height());
        _pUserCard->activateWindow();
    }
}

void CardManager::getPhoneNo(const std::string &userId) {
    std::thread t([this, userId]() {
#ifdef _MACOS
        pthread_setname_np("CardManager::getPhoneNo");
#endif
        std::string phoneNo;
        if (nullptr != _pMsgManager) {
            std::lock_guard<QTalk::util::spin_mutex> lock(sm);
            _pMsgManager->getUserPhoneNo(userId, phoneNo);
        }

        if (!phoneNo.empty()) {
            emit gotPhoneNo(userId, phoneNo);
        }

    });
    t.detach();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/12/16
  */
void CardManager::updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs) {

    std::lock_guard<QTalk::util::spin_mutex> lock(sm);

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
}

void CardManager::updateUserConfig(const std::map<std::string, std::string> &deleteData,
                                   const std::vector<QTalk::Entity::ImConfig>& arImConfig)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    for(const auto& it : deleteData)
    {
        if (it.second == "kStarContact") {
            if (_arStarContact.contains(it.first))
                _arStarContact.removeOne(it.first);
        } else if (it.second == "kBlackList") {
            if (_arBlackList.contains(it.first))
                _arBlackList.removeOne(it.first);
        } else if (it.second == "kMarkupNames") {
            if (_mapMaskNames.contains(it.first))
                _mapMaskNames.remove(it.first);
        }
    }

    for(const auto& it : arImConfig)
    {
        if (it.ConfigKey == "kStarContact") {
            if (!_arStarContact.contains(it.ConfigSubKey))
                _arStarContact.push_back(it.ConfigSubKey);
        } else if (it.ConfigKey == "kBlackList") {
            if (!_arBlackList.contains(it.ConfigSubKey))
                _arBlackList.push_back(it.ConfigSubKey);
        } else if (it.ConfigKey == "kMarkupNames") {
            _mapMaskNames[it.ConfigSubKey] = it.ConfigValue;
        }
    }
}

/**
 *
 * @param friends
 */
void CardManager::onRecvFriends(const std::vector<QTalk::Entity::IMFriendList> &friends) {
    _arFriends.clear();

    for (QTalk::Entity::IMFriendList imfriend : friends) {
        _arFriends.push_back(imfriend.XmppId);
    }
}

/**
 * 星标联系人
 * @param userId
 */
void CardManager::starUser(const std::string &userId) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    bool isStar = _arStarContact.contains(userId);

    std::thread([this, userId, isStar](){
        QString val = QString::number(isStar ? 0 : 1);
        if (_pMsgManager) {
            _pMsgManager->setUserSetting(isStar, "kStarContact", userId, val.toStdString());
        }
    }).detach();
}

/**
 * 加入黑名单
 */
void CardManager::addBlackList(const std::string &userId) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    bool isBlack = _arBlackList.contains(userId);
    std::thread([this, isBlack, userId](){
        QString val = QString::number(isBlack ? 0 : 1);
        if (_pMsgManager) {
            _pMsgManager->setUserSetting(isBlack, "kBlackList", userId, val.toStdString());
        }
    }).detach();
}

/**
 *
 */
void CardManager::showGroupCardSlot() {
    if (nullptr != _groupCard && nullptr != _imGroupSup) {
        _groupCard->setData(_imGroupSup);
        _groupCard->adjustSize();
        _groupCard->showCenter(false, nullptr);
        _groupCard->activateWindow();
    }
}

/**
 *
 * @param groupId
 * @param userCards
 */
void CardManager::onRecvGroupMember(const std::string &groupId,
                                     const std::map<std::string, QTalk::StUserCard> &userCards,
                                     const std::map<std::string, QUInt8> &userRole) {
    if (nullptr != _imGroupSup && _imGroupSup->GroupId == groupId) {
        emit sgUpdateGroupMember(userCards, userRole);
    }
}

/**
 *
 */
void CardManager::updateGroupMember(std::map<std::string, QTalk::StUserCard> userCards, std::map<std::string, QUInt8> userRole) {
    if (_groupCard) {
        _groupCard->showGroupMember(userCards, userRole);
    }
}

void CardManager::updateGroupInfo(const QString &groupId, const QString &groupName, const QString &topic,
                                  const QString &desc) {
    if (_pMsgManager) {
        std::shared_ptr<QTalk::StGroupInfo> groupinfo(new QTalk::StGroupInfo);
        groupinfo->groupId = groupId.toStdString();
        groupinfo->name = groupName.toStdString();
        groupinfo->desc = desc.toStdString();
        groupinfo->title = topic.toStdString();
        _pMsgManager->updateGroupInfo(groupinfo);
    }
}

/**
 *
 * @param groupi
 */
void CardManager::quitGroup(const QString &groupId) {
    if (_pMsgManager) {
        _pMsgManager->quitGroup(groupId.toStdString());
    }
}

/**
 *
 * @param groupId
 */
void CardManager::destroyGroup(const QString &groupId) {
    if (_pMsgManager) {
        _pMsgManager->destroyGroupMsg(groupId.toStdString());
    }
}

void CardManager::setUserMaskName(const std::string &userId, const std::string &maskName)
{
    std::thread([this, userId, maskName](){
        if (_pMsgManager) {
            _pMsgManager->setUserSetting(false, "kMarkupNames", userId, maskName);
        }
    }).detach();
}

void CardManager::setUserMood(const std::string &mood) {

    if (_pMsgManager) {
        _pMsgManager->updateMood(mood);
    }
}

std::string CardManager::getSourceHead(const std::string& headLink)
{
    if (_pMsgManager) {
        return _pMsgManager->getSourceImage(headLink);
    }
    return "";
}