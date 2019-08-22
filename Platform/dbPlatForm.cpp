//
// Created by QITMAC000260 on 2019-01-18.
//

#include "dbPlatForm.h"
#include "../LogicManager/LogicManager.h"

dbPlatForm* dbPlatForm::_platform = nullptr;

dbPlatForm& dbPlatForm::instance() {
    if (nullptr == _platform)
    {
        static dbPlatForm platform;
        _platform = &platform;
    }
    return *_platform;
}

//int dbPlatForm::getAtCount(const std::string & id)
//{
//    int count = 0;
//    //LogicManager::instance()->GetDatabase()->getAtCount(id, count);
//    return count;
//}
//
//int dbPlatForm::getAllUnreadCount()
//{
//    int count = 0;
//    LogicManager::instance()->getDatabase()->getAllUnreadCount(count);
//    return count;
//}


std::shared_ptr<QTalk::Entity::ImUserInfo> dbPlatForm::getUserInfo(const std::string &xmppId, const bool &readDb) {
//    LogicManager::
    std::shared_ptr<QTalk::Entity::ImUserInfo> info = _userInfoMap[xmppId];
    if (info && !readDb) {
        return info;
    } else // 如果内存中没有则去数据库中查找
    {
        std::shared_ptr<QTalk::Entity::ImUserInfo> userinfo =
                LogicManager::instance()->getDatabase()->getUserInfoByXmppId(xmppId);
        _userInfoMap[xmppId] = userinfo;
        return userinfo;
    }
}


std::shared_ptr<QTalk::Entity::ImGroupInfo> dbPlatForm::getGroupInfo(const std::string &xmppId, const bool &readDb) {
    std::shared_ptr<QTalk::Entity::ImGroupInfo> info = _groupInfoMap[xmppId];
    if (info && !readDb) {
        return info;
    } else // 如果内存中没有则去数据库中查找
    {
        std::shared_ptr<QTalk::Entity::ImGroupInfo> groupinfo =
                LogicManager::instance()->getDatabase()->getGroupInfoByXmppId(xmppId);
        _groupInfoMap[xmppId] = groupinfo;
        return groupinfo;
    }
    return nullptr;
}

std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > dbPlatForm::QueryImSessionInfos() {
    return LogicManager::instance()->getDatabase()->QueryImSessionInfos();
}

/**
 *
 * @param groups
 */
void dbPlatForm::getAllGroup(std::vector<QTalk::Entity::ImGroupInfo> &groups)
{
    LogicManager::instance()->getDatabase()->getAllGroup(groups);
}

bool dbPlatForm::isHotLine(const std::string &jid) {
    return LogicManager::instance()->getDatabase()->isHotLine(jid);
}

std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > dbPlatForm::reloadSession() {
    return LogicManager::instance()->getDatabase()->reloadSession();
}

std::vector<QTalk::StUserCard> dbPlatForm::getGroupMemberInfo(const std::vector<std::string> &arMembers) {
    std::vector<QTalk::StUserCard> ret;
    if(!arMembers.empty())
    {
        LogicManager::instance()->getDatabase()->getGroupMemberInfo(arMembers, ret);
    }
    return ret;
}

/**
 *
 * @param masks
 */
void dbPlatForm::setMaskNames(const std::map<std::string, std::string> &masks)
{
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    mask_names = masks;
}

std::string dbPlatForm::getMaskName(const std::string& xmppId) {
    std::lock_guard<QTalk::util::spin_mutex> lock(sm);
    auto itFind = mask_names.find(xmppId);
    if(itFind != mask_names.end())
        return itFind->second;
    else
        return std::string();
}
