#include "MessageManager.h"
#include "../EventBus/EventBus.h"
#include "AddressBookPanel.h"
#include "../QtUtil/Utils/Log.h"


AddressBookMsgManager::AddressBookMsgManager() {
}


AddressBookMsgManager::~AddressBookMsgManager() {
}

/**
 *
 * @param imUserSup
 */
void AddressBookMsgManager::getUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement> &imUserSup,
                                        std::shared_ptr<QTalk::Entity::ImUserInfo>& userInfo) {
    UserCardSupple e(imUserSup, userInfo);
    EventBus::FireEvent(e);
}

/**
 *
 * @param userId 用户id
 * @param phoneNo 返回值
 */
void AddressBookMsgManager::getUserPhoneNo(const std::string &userId, std::string &phoneNo) {
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
void AddressBookMsgManager::setUserSetting(bool isSetting, const std::string &key, const std::string &subKey,
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
 * 获取本地组织架构
 * @param structure
 */
void AddressBookMsgManager::getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure) {
    StructureMessage e(*this, structure);
    e.structure = structure;
    EventBus::FireEvent(e);
}

/**
 * 获取此组织架构下人数
 * @param structName
 * @param count
 */
void AddressBookMsgManager::getStructureCount(const std::string &structName, int &count) {
    GetStructureCount e(count);
    e.structName = structName;
    EventBus::FireEvent(e);
}

/**
 * 获取组织架构群成员
 */
void AddressBookMsgManager::getStructureMembers(const std::string &structName, std::vector<std::string> &members) {
    GetStructureMember e(members);
    e.structName = structName;
    EventBus::FireEvent(e);
}

/**
 * eventbus 创建群请求
 */
void AddressBookMsgManager::creatGroup(const std::string &groupId, const std::string &groupName) {
    CreatGroup e(*this, groupId);
    e.groupName = groupName;
    EventBus::FireEvent(e);
}

/**
 * 拉人进群
 */
void AddressBookMsgManager::addGroupMember(const std::vector<std::string> &members, const std::string &groupId) {
    AddGroupMember e(*this, members, groupId);
    EventBus::FireEvent(e);
}

void AddressBookMsgManager::getUserInfo(std::shared_ptr<QTalk::Entity::ImUserInfo>& info)
{
    UserCardInfo e(info);
    EventBus::FireEvent(e);
}

/***************************/
AddressBookListener::AddressBookListener(AddressBookPanel *mainPanel)
        : _mainPanel(mainPanel) {
	EventBus::AddHandler<UpdateUserConfigMsg>(*this);
	EventBus::AddHandler<AllFriends>(*this);
	EventBus::AddHandler<AllGroupList>(*this);
	EventBus::AddHandler<CreatGroupRet>(*this);
	EventBus::AddHandler<DestroyGroupRet>(*this);
	EventBus::AddHandler<IncrementConfig>(*this);
	EventBus::AddHandler<IncrementUser>(*this);
}

AddressBookListener::~AddressBookListener() {

}

void AddressBookListener::onEvent(UpdateUserConfigMsg &e) {
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->updateUserConfig(e.arConfigs);
    }
}

void AddressBookListener::onEvent(AllFriends &e) {
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->onRecvFriends(e.friends);
    }
}

void AddressBookListener::onEvent(AllGroupList &e) {
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->onRecvGroups(e.groups);
    }
}

void AddressBookListener::onEvent(CreatGroupRet &e) {
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->onCreatGroupRet(e.groupId);
    }
}

void AddressBookListener::onEvent(DestroyGroupRet &e)
{
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->onDestroyGroupRet(e.groupId);
    }
}

void AddressBookListener::onEvent(IncrementConfig &e)
{
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->updateUserConfig(e.deleteData, e.arImConfig);
    }
}

void AddressBookListener::onEvent(IncrementUser &e)
{
    if (e.getCanceled()) return;

    if (_mainPanel) {
        _mainPanel->gotIncrementUser(e.arUserInfo, e.arDeletes);
    }
}
