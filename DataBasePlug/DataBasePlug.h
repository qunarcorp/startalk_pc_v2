#ifndef DATABASEPLUG_H
#define DATABASEPLUG_H

#include "../interface/logic/IDatabasePlug.h"
#include "databaseplug_global.h"
#include "sqlite/database.h"
#include "../entity/im_userSupplement.h"
#include "../entity/im_config.h"
#include "../include/ThreadPool.h"
#include "../entity/im_qr_group.h"
#include "../entity/im_qr_content.h"

/**
 * database operator
 * tips: There is no need to use "_dbPool" for reading operations
 */
class DATABASEPLUGSHARED_EXPORT DataBasePlug : public IDatabasePlug {

public:
    DataBasePlug();
    ~DataBasePlug() override;

private:

    // IDatabasePlug interface
public:
    bool OpenDB(const std::string &dbPath, std::string& errorMsg) override ;
    // IDatabasePlug interface
    void ClearDBData() override ;

private:
    //
    void CreatTables();
    void modifyDbByVersion();
    bool initConfigVersions();

    // session info
public:
    // add session
    bool insertSessionInfo(const QTalk::Entity::ImSessionInfo &imSessionInfo) override ;
    bool bulkInsertSessionInfo(const std::vector<QTalk::Entity::ImSessionInfo> &sessionList) override ;
    // delete session
    bool bulkDeleteSessions(const std::vector<std::string> &peerIds) override ;
    // get session
    std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > QueryImSessionInfos() override ;
    std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > reloadSession() override ;
    //
    void getRecentSession(std::vector<QTalk::StShareSession> &sessions) override ;
    void geContactsSession(std::vector<QTalk::StShareSession> &sessions) override ;

public:
    long long getMaxTimeStampByChatType(QTalk::Enum::ChatType chatType) override;
    bool setUserCardInfo(const std::vector<QTalk::StUserCard> &userInfos) override;
    bool insertOrUpdateUserMood(const std::string &userId, const std::string &userMood, const int &version) ;
    bool insertOrUpdateUserSuppl(std::shared_ptr<QTalk::Entity::ImUserSupplement> imUserSup) ;
    bool insertOrUpdateUserPhoneNo(const std::string &userId, const std::string &phoneNo) override ;

public://user
    //
    bool getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure) override;
    // 获取组织架构版本号
    bool getUserVersion(int &version) override;

    bool insertUserInfo(const QTalk::Entity::ImUserInfo &userInfo) override;

    bool bulkInsertUserInfo(const std::vector<QTalk::Entity::ImUserInfo> &userInfos) override;

    bool bulkDeleteUserInfo(const std::vector<std::string> &userIds) override;

    bool getStructureCount(const std::string &strName, int &count) override;

    bool getStructureMember(const std::string &strName, std::vector<std::string> &arMember) override;

    std::shared_ptr<QTalk::Entity::ImUserInfo> getUserInfoByXmppId(const std::string &xmppid) override;

public: //message
    bool insertMessageInfo(const QTalk::Entity::ImMessageInfo &imMessageInfo) override;
    bool bulkInsertMessageInfo(const std::vector<QTalk::Entity::ImMessageInfo> &msgList) override;
    //
    bool deleteMessageByMessageId(const std::string &messageId) override ;

    bool getUnreadedMessages(const std::string &messageId, std::vector<std::string> &msgIds) override;

    bool getMessageByMessageId(const std::string &messageId, QTalk::Entity::ImMessageInfo &imMessageInfo) override;
    // 撤销消息处理
    bool updateRevokeMessage(const std::string &messageId) override;
    // 服务器已接收消息
    bool updateMState(const std::string &messageId, const QInt64 &time) override;
    // 更新消息阅读状态
    bool updateReadMask(const std::map<std::string, QInt32> &readMasks) override;

    bool updateReadMask(const std::map<std::string, QInt64> &readMasks) override;
    //
    bool getUserMessage(const long long &time, const std::string &userName,const std::string &realJid,
                        std::vector<QTalk::Entity::ImMessageInfo> &msgList) override;

    void updateMessageExtendInfo(const std::string& msgId, const std::string& info) override ;

    // get image or "photo-text"
    void getBeforeImageMessage(const std::string& messageId,
                                       std::vector<std::pair<std::string, std::string>> & msgs) override ;

    void getNextImageMessage(const std::string& messageId,
                                     std::vector<std::pair<std::string, std::string>> & msgs) override;
    // local message
    void getLocalMessage(const long long &time, const std::string &userId, const std::string &realJid,
                         std::vector<QTalk::Entity::ImMessageInfo> &msgList) override;

    void getFileMessage(const long long &time, const std::string &userId, const std::string &realJid,
                        std::vector<QTalk::Entity::ImMessageInfo> &msgList) override;

    void getImageMessage(const long long &time, const std::string &userId, const std::string &realJid,
                         std::vector<QTalk::Entity::ImMessageInfo> &msgList) override;

    void getSearchMessage(const long long &time, const std::string &userId, const std::string &realJid,
                          const std::string& searchKey, std::vector<QTalk::Entity::ImMessageInfo> &msgList) override ;

    void getAfterMessage(const long long &time, const std::string &userId, const std::string &realJid,
                         std::vector<QTalk::Entity::ImMessageInfo> &msgList) override ;

    void updateMessageReadFlags(const std::map<std::string, int>& readFlags) override;

public://group
    bool insertGroupInfo(const QTalk::Entity::ImGroupInfo &userInfo) override ;
    bool bulkInsertGroupInfo(const std::vector<QTalk::Entity::ImGroupInfo> &userInfos) override ;
    // get group config version
    bool getGroupMainVersion(long long &lastUpdateTime) override ;
    bool setGroupMainVersion(long long mainVersion) override ;
    // update groupcard
    bool updateGroupCard(const std::vector<QTalk::Entity::ImGroupInfo> &groups) override ;
    // delete
    bool bulkDeleteGroup(const std::vector<std::string> &groupIds) override ;
    // get group card info
    std::shared_ptr<QTalk::Entity::ImGroupInfo> getGroupInfoByXmppId(const std::string &xmppids) override ;
    // get group member by id
    bool getGroupMemberById(const std::string &groupId, std::vector<QTalk::StUserCard> &member,
                                    std::map<std::string, QUInt8> &userRole) override;
    //
    bool getGroupMemberInfo(const std::vector<std::string> &members, std::vector<QTalk::StUserCard> &userInfos) override;

    bool getGroupMemberInfo(std::map<std::string, QTalk::StUserCard> &userInfos) override;

    bool bulkInsertGroupMember(const std::string &groupId, const std::map<std::string, QUInt8> &member) override;

    bool getGroupTopic(const std::string &groupId, std::string &groupTopic) override;

    bool getAllGroup(std::vector<QTalk::Entity::ImGroupInfo> &groups) override;

    bool getGroupMessageLastUpdateTime(const std::string &messageId, QInt64 &time) override;

    bool getGroupUnreadedCount(const std::map<std::string, QInt64> &readMasks,
                                       std::map<std::string, int> &unreadedCount) override;

    bool getGroupCardById(std::shared_ptr<QTalk::Entity::ImGroupInfo> &group) override;

    void getGroupCardMaxVersion(long long &version) override ;

    void getAllGroupMembers(std::map<std::string, std::set<std::string>> &members) override ;

public://config
    bool getConfigVersion(int &version) override;

    bool bulkInsertConfig(const std::vector<QTalk::Entity::ImConfig> &config) override;

    bool bulkRemoveConfig(const std::map<std::string, std::string> &mapConf) override;

    bool insertConfig(const std::string &key, const std::string &subKey, const std::string &val) override;

    bool getConfig(const std::string &key, const std::string &subKey, std::string &val) override;

    bool getConfig(const std::string &key, std::map<std::string, std::string> &mapConf) override;

    bool getAllConfig(std::vector<QTalk::Entity::ImConfig> &configs) override;

public: // friends
    bool bulkInsertFriends(const std::vector<QTalk::Entity::IMFriendList> &friends) override;
    bool insertFriend(QTalk::Entity::IMFriendList imfriend) override;
    bool deleteAllFriends() override;
    bool deleteFriendByXmppId(const std::string &xmppId) override;
    bool getAllFriends(std::vector<QTalk::Entity::IMFriendList> &friends) override;

public://Im_Cache_Data
    bool insertUserId(std::string value) override;
    bool insertHotLine(std::string value) override;
    void getHotLines(std::string &hotLines) override;
    bool isHotlineMerchant(std::string xmppid) override;
    std::string getGroupReadMarkTime() override;
    bool updateGroupReadMarkTime(std::string time) override;

    std::string getLoginBeforeGroupReadMarkTime() override;
    bool saveLoginBeforeGroupReadMarkTime(const std::string &time) override;

public://快捷回复
    void batchInsertQuickReply(const std::string &data) override;
    void getQuickReplyVersion(QInt64 version[]) override;
    void getQuickGroups(std::vector<QTalk::Entity::ImQRgroup>& groups) override;
    void getQuickContentByGroup(std::vector<QTalk::Entity::IMQRContent>& contents,int id) override;

public:
    void getCareUsers(std::set<std::string>& users) override ;

public:
    void insertMedalList(const std::vector<QTalk::Entity::ImMedalList>& medals) override;
    void insertMedals(const std::vector<QTalk::Entity::ImUserStatusMedal>& medals) override ;
    void getUserMedal(const std::string& xmppId, std::set<QTalk::StUserMedal>& stMedal) override ;
    void getMedalList(std::vector<QTalk::Entity::ImMedalList>& medals) override ;
    void getMedalUsers(int medalId, std::vector<QTalk::StMedalUser>& metalUsers) override;
    void modifyUserMedalStatus(const std::string& userId, int medalId, int status) override ;

private:
    qtalk::sqlite::database *_dataBass;
    ThreadPool _dbPool;
};

#endif // DATABASEPLUG_H
