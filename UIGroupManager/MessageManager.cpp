//
// Created by cc on 18-11-11.
//

#include "MessageManager.h"
#include "../Message/GroupMessage.h"
#include "../EventBus/EventBus.h"
#include "UIGroupManager.h"
#include "../QtUtil/Utils/Log.h"
#include "../Message/ChatMessage.h"
#include "../Message/UserMessage.h"

GroupManagerMsgManager::GroupManagerMsgManager()
{

}

GroupManagerMsgManager::~GroupManagerMsgManager()
{

}

void GroupManagerMsgManager::getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>>& structure)
{
    StructureMessage e(*this, structure);
    e.structure = structure;
    EventBus::FireEvent(e);
}

void GroupManagerMsgManager::addGroupMember(const std::vector<std::string>& members, const std::string& groupId)
{
    AddGroupMember e(*this, members, groupId);
    EventBus::FireEvent(e);
}

void GroupManagerMsgManager::creatGroup(const std::string &groupId, const std::string& groupName)
{
    CreatGroup e(*this, groupId);
	e.groupName = groupName;
    EventBus::FireEvent(e);
}

void GroupManagerMsgManager::getGroupMembers(std::map<std::string, std::set<std::string>> &memebers)
{
    AllGroupMemberEvt e(memebers);
    EventBus::FireEvent(e);
}

void GroupManagerMsgManager::getUserInfo(std::shared_ptr<QTalk::Entity::ImUserInfo>& info)
{
    UserCardInfo e(info);
    EventBus::FireEvent(e);
}

void GroupManagerMsgManager::getRecentSession(std::vector<QTalk::StShareSession> &ss)
{
    RecentSessionEvt e(ss);
    EventBus::FireEvent(e);
}

//
GroupMsgListener::GroupMsgListener(UIGroupManager *manager)
    :_pManager(manager)
{
	EventBus::AddHandler<CreatGroupRet>(*this);
	EventBus::AddHandler<DestroyGroupRet>(*this);
	EventBus::AddHandler<AllFriends>(*this);
	EventBus::AddHandler<AllGroupList>(*this);
	EventBus::AddHandler<UpdateUserConfigMsg>(*this);
	EventBus::AddHandler<IncrementConfig>(*this);
	EventBus::AddHandler<IncrementUser>(*this);
}

GroupMsgListener::~GroupMsgListener()
{

}

void GroupMsgListener::onEvent(CreatGroupRet &e)
{
    if(e.getCanceled()) return;

    if(_pManager)
    {
        _pManager->onCreatGroupRet(e.ret, e.groupId);
    }
}

void GroupMsgListener::onEvent(DestroyGroupRet &e)
{

}

void GroupMsgListener::onEvent(AllFriends &e) {
    if (e.getCanceled()) return;

    if (_pManager) {
        _pManager->onRecvFriends(e.friends);
    }
}

void GroupMsgListener::onEvent(AllGroupList &e) {

    if(e.getCanceled()) return;

    if(_pManager)
        _pManager->onRecvGroupList(e.groups);
}


void GroupMsgListener::onEvent(UpdateUserConfigMsg &e) {
    if (e.getCanceled()) return;

    if (_pManager) {
        _pManager->updateUserConfig(e.arConfigs);
    }
}

void GroupMsgListener::onEvent(IncrementConfig &e) {
    if (e.getCanceled()) return;

    if (_pManager) {
        _pManager->updateUserConfig(e.deleteData, e.arImConfig);
    }
}

void GroupMsgListener::onEvent(IncrementUser &e) {
    if (e.getCanceled()) return;

    if (_pManager) {
        _pManager->gotIncrementUser(e.arUserInfo, e.arDeletes);
    }
}