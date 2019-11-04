//
// Created by QITMAC000260 on 2019-01-18.
//

#ifndef QTALK_V2_DBPLATFORM_H
#define QTALK_V2_DBPLATFORM_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <memory>
#include "../entity/im_user.h"
#include "../entity/im_group.h"
#include "../entity/IM_Session.h"
#include "../entity/im_medal_list.h"
#include "platform_global.h"
#include "../include/CommonStrcut.h"
#include "../include/Spinlock.h"

/**
* @description: ${description}
* @author: cc
* @create: 2019-01-18 16:15
**/
class PLATFORMSHARED_EXPORT dbPlatForm {

public:
    dbPlatForm() = default;

public:
//    int getAtCount(const std::string& id);
//    int getAllUnreadCount();
    bool isHotLine(const std::string& jid);
    void setHotLines(const std::set<std::string>& hotlines);

public:
    void setMaskNames(const std::map<std::string, std::string>& masks);
    std::string getMaskName(const std::string& xmppId);
    void getAllGroup(std::vector<QTalk::Entity::ImGroupInfo>& groups);

    void setMedals(const std::vector<QTalk::Entity::ImMedalList> &medals);
    QTalk::Entity::ImMedalList getMedal(const int & id);
    void getAllMedals(std::vector<QTalk::Entity::ImMedalList> &medals);

private:
    static dbPlatForm* _platform;

public:
    static dbPlatForm& instance();

public:
    std::shared_ptr<QTalk::Entity::ImUserInfo> getUserInfo(const std::string &xmppId, const bool &readDb = false);
    std::shared_ptr<QTalk::Entity::ImGroupInfo> getGroupInfo(const std::string &xmppId, const bool &readDb = false);
    std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > QueryImSessionInfos();
    std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > reloadSession();
    std::vector<QTalk::StUserCard> getGroupMemberInfo(const std::vector<std::string>& arMembers);

private:
    std::map<std::string, std::shared_ptr<QTalk::Entity::ImUserInfo> > _userInfoMap;
    std::map<std::string, std::shared_ptr<QTalk::Entity::ImGroupInfo> > _groupInfoMap;
    std::set<std::string> _hotLines;

private:
    std::vector<QTalk::Entity::ImMedalList> _medals;

private:
    QTalk::util::spin_mutex sm;
    std::map<std::string, std::string> mask_names;
};


#endif //QTALK_V2_DBPLATFORM_H
