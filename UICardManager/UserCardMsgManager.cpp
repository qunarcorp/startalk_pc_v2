//
// Created by cc on 18-11-6.
//

#include "UserCardMsgManager.h"
#include "../EventBus/EventBus.h"
#include "CardManager.h"
#include "../QtUtil/Utils/Log.h"
#include "../Message/GroupMessage.h"
#include "../Message/ChatMessage.h"

UserCardMsgManager::UserCardMsgManager() {

}

UserCardMsgManager::~UserCardMsgManager() {

}

void UserCardMsgManager::getUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement> &imUserSup,
                                     std::shared_ptr<QTalk::Entity::ImUserInfo>& userInfo) {
    UserCardSupple e(imUserSup, userInfo);
    EventBus::FireEvent(e);
}

void UserCardMsgManager::getUserPhoneNo(const std::string &userId, std::string &phoneNo) {
    UserPhoneNo e;
    e.userId = userId;
    EventBus::FireEvent(e, false);
    phoneNo = e.phoneNo;
}

/**
 * 更新配置
 * @param isSetting 需要注意 直接传原bool值就可以
 * @param key
 * @param subKey
 * @param val
 */
void UserCardMsgManager::setUserSetting(bool isSetting, const std::string &key, const std::string &subKey,
                                        const std::string &val) {

    info_log("setting userconfig -> issetting:{0}, key:{1}, subKey:{2}, val:{3}", isSetting, key, subKey, val);

    UserSettingMsg e;
    e.operatorType = isSetting ? UserSettingMsg::EM_OPERATOR_CANCEL : UserSettingMsg::EM_OPERATOR_SET;
    e.key = key;
    e.subKey = subKey;
    e.value = val;
    EventBus::FireEvent(e);
}

/**
 * 获取群卡片信息
 * @param groupInfo
 */
void UserCardMsgManager::getGroupCard(std::shared_ptr<QTalk::Entity::ImGroupInfo> &groupInfo) {
    GetGroupInfoMessage e(groupInfo);
    EventBus::FireEvent(e);
}

/**
 * 获取最新群成员列表
 * @param groupId
 */
void UserCardMsgManager::getGroupMembers(const std::string &groupId) {
    GetGroupMessage e;
    e.groupId = groupId;
    EventBus::FireEvent(e);
}

void UserCardMsgManager::updateGroupInfo(std::shared_ptr<QTalk::StGroupInfo> groupinfo) {
    UpdateGroupInfoMsg e;
    e.groupinfo = groupinfo;
    EventBus::FireEvent(e);
}

/**
 *
 * @param groupi
 */
void UserCardMsgManager::quitGroup(const std::string &groupId) {
    QuitGroupMsg e;
    e.groupId = groupId;
    EventBus::FireEvent(e);
}

/**
 *
 * @param groupId
 */
void UserCardMsgManager::destroyGroupMsg(const std::string &groupId) {
    DestroyGroupMsg e;
    e.groupId = groupId;
    EventBus::FireEvent(e);
}

void UserCardMsgManager::updateMood(const std::string &mood)
{
    UpdateMoodEvt e(mood);
    EventBus::FireEvent(e);
}

std::string UserCardMsgManager::getSourceImage(const std::string& netPath)
{
    SourceNetImage e;
    e.netFilePath = netPath;
    EventBus::FireEvent(e);
    return e.localFilePath;
}

void UserCardMsgManager::getUserMedal(const std::string& xmppId, std::set<QTalk::StUserMedal>& medal) {
    UserMedalEvt e(xmppId, medal);
    EventBus::FireEvent(e);
}

void UserCardMsgManager::getMedalUser(int medalId, std::vector<QTalk::StMedalUser> &metalUsers) {
    GetMedalUserEvt e;
    e.medalId = medalId;
    EventBus::FireEvent(e);
    metalUsers = e.metalUsers;
}

bool UserCardMsgManager::modifyUserMedal(int medalId, bool wear) {
    ModifyUserMedalStatusEvt e;
    e.medalId = medalId;
    e.isWear = wear;
    EventBus::FireEvent(e);
    return e.result;
}

/***/
UserCardMessageListener::UserCardMessageListener(CardManager *mainPanel)
        : _pMainPanel(mainPanel) {
	EventBus::AddHandler<AllFriends>(*this);
	EventBus::AddHandler<UpdateUserConfigMsg>(*this);
	EventBus::AddHandler<UpdateGroupMember>(*this);
	EventBus::AddHandler<GroupMemberMessage>(*this);
	EventBus::AddHandler<IncrementConfig>(*this);
}

UserCardMessageListener::~UserCardMessageListener() {

}

void UserCardMessageListener::onEvent(UpdateUserConfigMsg &e) {
    if (_pMainPanel) {
        _pMainPanel->updateUserConfig(e.arConfigs);
    }
}

void UserCardMessageListener::onEvent(AllFriends &e) {
    if (e.getCanceled()) return;

    if (_pMainPanel) {
        _pMainPanel->onRecvFriends(e.friends);
    }
}

/**
 *
 * @param e
 */
void UserCardMessageListener::onEvent(UpdateGroupMember &e) {
    if (e.getCanceled()) return;

    if (_pMainPanel) {
        //_pMainPanel->onRecvGroupMember(e.groupId, e.userCards);
    }
}

/**
 *
 * @param e
 */
void UserCardMessageListener::onEvent(GroupMemberMessage &e) {
    if (e.getCanceled()) return;

    if (_pMainPanel) {
        _pMainPanel->onRecvGroupMember(e.groupId, e.members, e.userRole);
    }
}

void UserCardMessageListener::onEvent(IncrementConfig &e) {
    if (e.getCanceled()) return;

    if (_pMainPanel) {
        _pMainPanel->updateUserConfig(e.deleteData, e.arImConfig);
    }
}

