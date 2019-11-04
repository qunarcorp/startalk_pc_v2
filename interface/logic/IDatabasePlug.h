#ifndef IDATABASEPLUG_H
#define IDATABASEPLUG_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "ILogicObject.h"
#include "../../entity/IM_Session.h"
#include "../../entity/im_message.h"
#include "../../entity/im_user.h"
#include "../../entity/im_group.h"
#include "../../QtUtil/Enum/im_enum.h"
#include "../../entity/im_userSupplement.h"
#include "../../entity/im_config.h"
#include "../../entity/im_friend_list.h"
#include "../../include/CommonStrcut.h"
#include "../../entity/im_qr_group.h"
#include "../../entity/im_qr_content.h"
#include "../../entity/im_medal_list.h"
#include "../../entity/im_user_status_medal.h"

/**
 * 数据库操作接口
 */
class IDatabasePlug : public ILogicObject {
    // 数据库操作
public:
    virtual bool OpenDB(const std::string &dbPath, std::string& errorMsg) = 0;
    virtual void ClearDBData() = 0;

    // session
public:
    // add session
    virtual bool insertSessionInfo(const QTalk::Entity::ImSessionInfo &imSessionInfo) = 0;
    virtual bool bulkInsertSessionInfo(const std::vector<QTalk::Entity::ImSessionInfo> &sessionList) = 0;
    // delete session
    virtual bool bulkDeleteSessions(const std::vector<std::string> &peerIds) = 0;
    // recent session contains user and group
    virtual void getRecentSession(std::vector<QTalk::StShareSession> &sessions) = 0;
    // recent session contains user only
    virtual void geContactsSession(std::vector<QTalk::StShareSession> &sessions) = 0;
    // make and get session
    virtual std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > QueryImSessionInfos() = 0;
    // get session
    virtual std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > reloadSession() = 0;

    // message
public:
    // add message
    virtual bool insertMessageInfo(const QTalk::Entity::ImMessageInfo &imMessageInfo) = 0;
    virtual bool bulkInsertMessageInfo(const std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;
    // delete message by message id
    virtual bool deleteMessageByMessageId(const std::string &meesageId) = 0;
    // get max timestamp by chat type
    virtual long long getMaxTimeStampByChatType(QTalk::Enum::ChatType chatType) = 0;
    // get message by id
    virtual bool getUserMessage(const long long &time, const std::string &userName,const std::string &realJid,
                                std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;
    // update message read flag
    virtual void updateMessageReadFlags(const std::map<std::string, int>& msgFlags) = 0;
    virtual bool updateMState(const std::string &messageId, const QInt64 &time) = 0;
    virtual bool updateReadMask(const std::map<std::string, QInt32> &readMasks) = 0;
    virtual bool updateReadMask(const std::map<std::string, QInt64> &readMasks) = 0;
    //
    virtual void updateMessageExtendInfo(const std::string& msgId, const std::string& info) = 0;

    // local message
    virtual void getLocalMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                 std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;

    virtual void getFileMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;

    virtual void getImageMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                 std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;

    virtual void getSearchMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                  const std::string& searchKey, std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;

    virtual void getAfterMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                 std::vector<QTalk::Entity::ImMessageInfo> &msgList) = 0;
    // get image or "photo-text" message
    virtual void getBeforeImageMessage(const std::string& messageId,
                                       std::vector<std::pair<std::string, std::string>> & msgs) = 0;

    virtual void getNextImageMessage(const std::string& messageId,
                                     std::vector<std::pair<std::string, std::string>> & msgs) = 0;
    // 获取xmppid所有未读消息 消息id
    virtual bool getUnreadedMessages(const std::string &messageId, std::vector<std::string> &msgIds) = 0;
    //
    virtual bool getGroupMessageLastUpdateTime(const std::string &messageId, QInt64 &time) = 0;

    virtual bool getGroupUnreadedCount(const std::map<std::string, QInt64> &readMasks,
                                       std::map<std::string, int> &unreadedCount) = 0;

    virtual bool getMessageByMessageId(const std::string &messageId, QTalk::Entity::ImMessageInfo &imMessageInfo) = 0;

    //
    virtual bool updateRevokeMessage(const std::string &messageId) = 0;

    // user
public:
    // add
    virtual bool insertUserInfo(const QTalk::Entity::ImUserInfo &userInfo) = 0;
    virtual bool bulkInsertUserInfo(const std::vector<QTalk::Entity::ImUserInfo> &userInfos) = 0;
    virtual bool setUserCardInfo(const std::vector<QTalk::StUserCard> &userInfos) = 0;
    // delete
    virtual bool bulkDeleteUserInfo(const std::vector<std::string> &userIds) = 0;
    // get count by structure name
    virtual bool getStructureCount(const std::string &strName, int &count) = 0;
    // get users by structure name
    virtual bool getStructureMember(const std::string &strName, std::vector<std::string> &arMember) = 0;
    // get user operator version
    virtual bool getUserVersion(int &version) = 0;
    // get user info by id
    virtual std::shared_ptr<QTalk::Entity::ImUserInfo> getUserInfoByXmppId(const std::string &xmppids) = 0;
    // update user mood
    virtual bool insertOrUpdateUserMood(const std::string &userId, const std::string &userMood, const int &version) = 0;
    // update user phone no
    virtual bool insertOrUpdateUserPhoneNo(const std::string &userId, const std::string &phoneNo) = 0;
    //
    virtual bool insertOrUpdateUserSuppl(std::shared_ptr<QTalk::Entity::ImUserSupplement> imUserSup) = 0;
    //
    virtual bool getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure) = 0;

    // group
public:
    // add
    virtual bool insertGroupInfo(const QTalk::Entity::ImGroupInfo &userInfo) = 0;
    virtual bool bulkInsertGroupInfo(const std::vector<QTalk::Entity::ImGroupInfo> &userInfos) = 0;
    // get group config version
    virtual bool getGroupMainVersion(long long &version) = 0;
    virtual bool setGroupMainVersion(long long version) = 0;
    // get group card max version
    virtual void getGroupCardMaxVersion(long long &version) = 0;
    // get group info by xmppId
    virtual std::shared_ptr<QTalk::Entity::ImGroupInfo> getGroupInfoByXmppId(const std::string &xmppids) = 0;
    // update group card
    virtual bool updateGroupCard(const std::vector<QTalk::Entity::ImGroupInfo> &groups) = 0;
    //
    virtual bool
    getGroupMemberInfo(std::map<std::string, QTalk::StUserCard> &userInfos) = 0;
    // get group topic
    virtual bool getGroupTopic(const std::string &groupId, std::string &groupTopic) = 0;
    // 批量删除群
    virtual bool bulkDeleteGroup(const std::vector<std::string> &groupIds) = 0;
    // all group
    virtual bool getAllGroup(std::vector<QTalk::Entity::ImGroupInfo> &groups) = 0;

    virtual bool getGroupCardById(std::shared_ptr<QTalk::Entity::ImGroupInfo> &group) = 0;

    // group member
public:
    //
    virtual bool bulkInsertGroupMember(const std::string &groupId, const std::map<std::string, QUInt8> &member) = 0;
    // get group card
    virtual bool getGroupMemberById(const std::string &groupId, std::vector<QTalk::StUserCard> &member,
                                    std::map<std::string, QUInt8> &userRole) = 0;
    virtual bool
    getGroupMemberInfo(const std::vector<std::string> &members, std::vector<QTalk::StUserCard> &userInfos) = 0;
    //
    virtual void getAllGroupMembers(std::map<std::string, std::set<std::string>> &members) = 0;

    // config
public:
    virtual bool getConfigVersion(int &version) = 0;

    virtual bool bulkInsertConfig(const std::vector<QTalk::Entity::ImConfig> &config) = 0;

    virtual bool bulkRemoveConfig(const std::map<std::string, std::string> &mapConf) = 0;

    virtual bool insertConfig(const std::string &key, const std::string &subKey, const std::string &val) = 0;

    virtual bool getConfig(const std::string &key, const std::string &subKey, std::string &val) = 0;

    virtual bool getConfig(const std::string &key, std::map<std::string, std::string> &mapConf) = 0;

    virtual bool getAllConfig(std::vector<QTalk::Entity::ImConfig> &configs) = 0;

    // friend
public:
    virtual bool bulkInsertFriends(const std::vector<QTalk::Entity::IMFriendList> &friends) = 0;
    virtual bool insertFriend(QTalk::Entity::IMFriendList imfriend) = 0;
    virtual bool deleteAllFriends() = 0;
    virtual bool deleteFriendByXmppId(const std::string &xmppId) = 0;
    virtual bool getAllFriends(std::vector<QTalk::Entity::IMFriendList> &friends) = 0;

    //Im_Cache_Data
public:
    virtual bool insertUserId(std::string value) = 0;
    virtual bool insertHotLine(std::string value) = 0;
    virtual void getHotLines(std::string &hotLines) = 0;
    virtual bool isHotlineMerchant(const std::string xmppid) = 0;

    virtual std::string getGroupReadMarkTime() = 0;
    virtual bool updateGroupReadMarkTime(const std::string time) = 0;

    virtual std::string getLoginBeforeGroupReadMarkTime() = 0;
    virtual bool saveLoginBeforeGroupReadMarkTime(const std::string& time) = 0;

public:
    virtual void getCareUsers(std::set<std::string>& users) = 0;
    virtual void batchInsertQuickReply(const std::string &data) = 0;
    virtual void getQuickReplyVersion(QInt64 version[]) = 0;
    virtual void getQuickGroups(std::vector<QTalk::Entity::ImQRgroup>& groups) = 0;
    virtual void getQuickContentByGroup(std::vector<QTalk::Entity::IMQRContent>& contents,int id) = 0;

public:
    virtual void insertMedalList(const std::vector<QTalk::Entity::ImMedalList>& medals) = 0;
    virtual void insertMedals(const std::vector<QTalk::Entity::ImUserStatusMedal>& medals) = 0;
    virtual void getUserMedal(const std::string& xmppId, std::set<QTalk::StUserMedal>& stMedal) = 0;
    virtual void getMedalList(std::vector<QTalk::Entity::ImMedalList>& medals) = 0;
    virtual void getMedalUsers(int medalId, std::vector<QTalk::StMedalUser>& metalUsers) = 0;
    virtual void modifyUserMedalStatus(const std::string& userId, int medalId, int status) = 0;
};

#endif // IDATABASEPLUG_H
