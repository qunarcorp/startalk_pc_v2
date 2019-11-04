#include "DataBasePlug.h"
#include "SessionListDao.h"
#include "MessageDao.h"
#include "UserDao.h"
#include "GroupDao.h"
#include "GroupMemberDao.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Utils/utils.h"
#include "ConfigDao.h"
#include "UserSupplementDao.h"
#include "DbConfig.h"
#include "../Platform/Platform.h"
#include "FriendListDao.h"
#include "../entity/IM_Session.h"
#include "../include/perfcounter.h"
#include "CacheDataDao.h"
#include "QuickReplyDao.h"
#include "MedalListDao.h"
#include "UserMedalDao.h"
#include "TriggerConfig.h"

DataBasePlug::DataBasePlug() :
        _dataBass(nullptr), _dbPool("db thread pool") {
}

DataBasePlug::~DataBasePlug()
{
#ifndef _LINUX
    if(_dataBass)
        delete _dataBass;
#endif
}

/**
  * @函数名
  * @功能描述 打开数据库
  * @参数
  * @date 2018.9.21
  */
bool DataBasePlug::OpenDB(const std::string &dbPath, std::string& errorMsg) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, dbPath, & errorMsg]() {

        try {
            if (!_dataBass) {
                _dataBass = new qtalk::sqlite::database(dbPath);
                _dataBass->exec("PRAGMA journal_mode=WAL;");
            }
            CreatTables();
            //
            modifyDbByVersion();

            ret = true;
        } catch (std::exception &exception) {
            ret = false;
            errorMsg = exception.what();
            error_log("OpenDB failed! {0}", exception.what());
        }
    });
    //
    func.get();

    if (ret) {
        //
        // 初始化各种版本号列表
        initConfigVersions();
    }

    return ret;
}

/**
 * 清除数据
 */
void DataBasePlug::ClearDBData() {
    DbConfig dbConfig(_dataBass);
    if (_dataBass->tableExists("DB_Config") && !dbConfig.clearData()) {
        error_log("DB_Config  清理表失败");
    }
    SessionListDao userDao(_dataBass);
    if (_dataBass->tableExists("IM_SessionList") && !userDao.clearData()) {
        error_log("IM_SessionList  清理表失败");
    }
    MessageDao msgDao(_dataBass);
    if (_dataBass->tableExists("IM_Message") && !msgDao.clearData()) {
        error_log("IM_Message  清理表失败");
    }
    UserDao usrDao(_dataBass);
    if (_dataBass->tableExists("IM_User") && !usrDao.clearData()) {
        error_log("IM_User  清理表失败");
    }
    FriendListDao friendDao(_dataBass);
    if (_dataBass->tableExists("IM_Friend_List") && !friendDao.clearData()) {
        error_log("IM_Friend_List  清理表失败");
    }
    GroupDao grpDao(_dataBass);
    if (_dataBass->tableExists("IM_Group") && !grpDao.clearData()) {
        error_log("IM_Group  清理表失败");
    }
    GroupMemberDao grpMemer(_dataBass);
    if (_dataBass->tableExists("IM_Group_Member") && !grpMemer.clearData()) {
        error_log("IM_Group_Member  清理表失败");
    }

    ConfigDao confDao(_dataBass);
    if (_dataBass->tableExists("IM_Config") && !confDao.clearData()) {
        error_log("IM_Config  清理表失败");
    }
    UserSupplementDao userSupDao(_dataBass);
    if (_dataBass->tableExists("IM_UserSupplement") && !userSupDao.clearData()) {
        error_log("IM_UserSupplement  清理表失败");
    }

    CacheDataDao cacheDataDao(_dataBass);
    if (!cacheDataDao.clearData()) {
        error_log("IM_Cache_Data  清理表失败");
    }

    QuickReplyDao quickReplyDao(_dataBass);
    if (!quickReplyDao.clearData()) {
        error_log("QuickReplyDao  清理表失败");
    }
    // Medal
    MedalListDao medalListDao(_dataBass);
    if (!medalListDao.clearData()) {
        error_log("IM_Medal_List  清理表失败");
    }
    UserMedalDao medalDao(_dataBass);
    if (!medalDao.clearData()) {
        error_log("IM_User_Status_Medal 清理表失败");
    }
}

/**
  * @函数名
  * @功能描述 插入会话信息
  * @参数
  * @date 2018.9.21
  */
bool DataBasePlug::insertSessionInfo(const QTalk::Entity::ImSessionInfo &imSessionInfo) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, imSessionInfo]() {
        SessionListDao dao(_dataBass);
        ret = dao.insertSessionInfo(imSessionInfo);
    });
    func.get();
    return ret;
}


bool DataBasePlug::bulkDeleteSessions(const std::vector<std::string> &peerIds) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, peerIds]() {
        perf_counter("bulkDeleteSessions size is {0}", peerIds.size());
        SessionListDao dao(_dataBass);
        ret = dao.bulkDeleteSession(peerIds);
        if (ret) {
            dao.bulkremoveSessionMessage(peerIds);
        }
    });

    func.get();
    return ret;
}

/**
 *
 * @param sessionList
 * @return
 */
bool DataBasePlug::bulkInsertSessionInfo(const std::vector<QTalk::Entity::ImSessionInfo> &sessionList) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, sessionList]() {
        perf_counter("bulkInsertSessionInfo size is {0}", sessionList.size());
        SessionListDao dao(_dataBass);
        ret = dao.bulkInsertSessionInfo(sessionList);
    });
    func.get();
    return ret;
}

/**
  * @函数名
  * @功能描述 获取所有会话信息
  * @参数
  * @date 2018.9.21
  */
std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > DataBasePlug::QueryImSessionInfos() {
    std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > ret;
    auto func = _dbPool.enqueue([this, &ret]() {
        perf_counter("QueryImSessionInfos");
        SessionListDao dao(_dataBass);
        ret = dao.QueryImSessionInfos();
    });
    func.get();

    return ret;
}

/**
 * 获取最新的session
 * @return
 */
std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > DataBasePlug::reloadSession() {
    std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > ret;
    auto func = _dbPool.enqueue([this, &ret]() {
        perf_counter("reloadSession");
        SessionListDao dao(_dataBass);
        ret = dao.reloadSession();
    });
    func.get();

    return ret;
}

/**
  * @函数名
  * @功能描述 插入会话信息
  * @参数
  * @date 2018.9.25
  */
bool DataBasePlug::insertMessageInfo(const QTalk::Entity::ImMessageInfo &imMessageInfo) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, imMessageInfo]() {
        perf_counter("insertMessageInfo {1}:{0}", imMessageInfo.Content, imMessageInfo.MsgId);
        std::vector<QTalk::Entity::ImMessageInfo> msgList;
        msgList.push_back(imMessageInfo);

        MessageDao dao(_dataBass);
        std::map<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo> mapSessions;
        ret = dao.bulkInsertMessageInfo(msgList, &mapSessions);

        if (ret) {
            dao.bulkUpdateSessionList(&mapSessions);
        }
    });
    func.get();
    return ret;
}

bool DataBasePlug::bulkInsertMessageInfo(const std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, msgList]() {

        perf_counter("bulkInsertMessageInfo {0}", msgList.size());

        MessageDao dao(_dataBass);
        std::map<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo> map;
        ret = dao.bulkInsertMessageInfo(msgList, &map);

        if (ret) {
            dao.bulkUpdateSessionList(&map);
        }
    });
    func.get();
    return ret;
}

long long DataBasePlug::getMaxTimeStampByChatType(QTalk::Enum::ChatType chatType) {
    long long ret = 0;
    auto func = _dbPool.enqueue([this, &ret, chatType]() {
        MessageDao dao(_dataBass);
        ret = dao.getMaxTimeStampByChatType(chatType);
    });
    func.get();
    return ret;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/28
  */
bool DataBasePlug::getUserMessage(const long long &time, const std::string &userName,const std::string &realJid,
                                  std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, time, userName,realJid, &msgList]() {
        MessageDao dao(_dataBass);
        ret = dao.getUserMessage(time, userName,realJid, msgList);
    });
    func.get();
    return ret;
}

/**
  * @函数名   getUserVersion
  * @功能描述 获取组织架构版本号
  * @参数     version 版本号
  * @date     2018/09/29
  */
bool DataBasePlug::getUserVersion(int &version) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &version]() {
        UserDao dao(_dataBass);
        ret = dao.getUserVersion(version);
    });
    func.get();
    return ret;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
bool DataBasePlug::insertGroupInfo(const QTalk::Entity::ImGroupInfo &userInfo) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userInfo]() {
        GroupDao dao(_dataBass);
        ret = dao.insertGroupInfo(userInfo);
    });
    func.get();
    return ret;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
bool DataBasePlug::bulkInsertGroupInfo(const std::vector<QTalk::Entity::ImGroupInfo> &userInfos) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userInfos]() {
        GroupDao dao(_dataBass);
        ret = dao.bulkInsertGroupInfo(userInfos);
    });
    func.get();
    return ret;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
bool DataBasePlug::getGroupMainVersion(long long &lastUpdateTime) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &lastUpdateTime]() {
        GroupDao dao(_dataBass);
        ret = dao.getGroupLastUpdateTime(lastUpdateTime);
    });
    func.get();
    return ret;
}

bool DataBasePlug::setGroupMainVersion(long long mainVersion) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, mainVersion]() {
        GroupDao dao(_dataBass);
        ret = dao.setGroupMainVersion(mainVersion);
    });
    func.get();
    return ret;
}

/**
  * @函数名   updateGroupCard
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
bool DataBasePlug::updateGroupCard(const std::vector<QTalk::Entity::ImGroupInfo> &groups) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, groups]() {
        GroupDao dao(_dataBass);
        ret = dao.updateGroupCard(groups);
    });
    func.get();
    return ret;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.30
  */
std::shared_ptr<QTalk::Entity::ImGroupInfo> DataBasePlug::getGroupInfoByXmppId(const std::string &xmppids) {
    std::shared_ptr<QTalk::Entity::ImGroupInfo> ret;
    auto func = _dbPool.enqueue([this, &ret, xmppids]() {
        GroupDao dao(_dataBass);
        ret = dao.getGroupInfoByXmppId(xmppids);
    });
    func.get();
    return ret;
}

/**
  * @函数名   getGroupInfoById
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/03
  */
bool DataBasePlug::getGroupMemberById(const std::string &groupId, std::vector<QTalk::StUserCard> &member,
                                      std::map<std::string, QUInt8> &userRole) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, groupId, &member, &userRole]() {
        GroupMemberDao dao(_dataBass);
        ret = dao.getGroupMemberById(groupId, member, userRole);
    });
    func.get();
    return ret;
}

/**
  * @函数名   getGroupMemberInfo
  * @功能描述 获取群成员信息
  * @参数
  * @author   cc
  * @date     2018/10/08
  */
bool
DataBasePlug::getGroupMemberInfo(const std::vector<std::string> &members, std::vector<QTalk::StUserCard> &userInfos) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, members, &userInfos]() {
        UserDao dao(_dataBass);
        ret = dao.getUserCardInfos(members, userInfos);
    });
    func.get();
    return ret;
}


bool DataBasePlug::getGroupMemberInfo(std::map<std::string, QTalk::StUserCard> &userInfos)
{
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &userInfos]() {
        UserDao dao(_dataBass);
        ret = dao.getUserCardInfos(userInfos);
    });
    func.get();
    return ret;
}

/**
  * @函数名   bulkInsertGroupMember
  * @功能描述 批量插入群成员
  * @参数
     bool
  * @author   cc
  * @date     2018/10/11
  */
bool DataBasePlug::bulkInsertGroupMember(const std::string &groupId, const std::map<std::string, QUInt8> &member) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, groupId, member]() {
         GroupMemberDao dao(_dataBass);
        ret = dao.bulkInsertGroupMember(groupId, member);
    });
    func.get();
    return ret;
}

/**
  * @函数名   getGroupTopic
  * @功能描述 获取群公告
  * @参数
  * @author   cc
  * @date     2018/10/12
  */
bool DataBasePlug::getGroupTopic(const std::string &groupId, std::string &groupTopic) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, groupId, &groupTopic]() {
        GroupDao dao(_dataBass);
        ret = dao.getGroupTopic(groupId, groupTopic);
    });
    func.get();
    return ret;
}

/**
  * @函数名   updateMState
  * @功能描述 服务器已接收消息
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool DataBasePlug::updateMState(const std::string &messageId, const QInt64 &time) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, messageId, time]() {
        perf_counter("updateMState");
        MessageDao dao(_dataBass);
        ret = dao.updateMState(messageId, time);
    });
    func.get();
    return ret;
}

/**
  * @函数名   updateReadMask
  * @功能描述 更新消息阅读状态
  * @参数
     bool
  * @author   cc
  * @date     2018/10/25
  */
bool DataBasePlug::updateReadMask(const std::map<std::string, QInt32> &readMasks) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, readMasks]() {
        auto start = std::chrono::system_clock::now();
        MessageDao dao(_dataBass);
        ret = dao.updateReadMask(readMasks);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> elapsed_milliseconds = end - start;
        info_log("updateReadMask count :{0}", elapsed_milliseconds.count());
    });
    func.get();
    return ret;
}

bool DataBasePlug::updateReadMask(const std::map<std::string, QInt64> &readMasks) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, readMasks]() {
        perf_counter("updateReadMask");
        MessageDao dao(_dataBass);
        ret = dao.updateReadMask(readMasks);
    });
    func.get();
    return ret;
}

/**
  * @函数名   
  * @功能描述 
  * @参数IM_SessionList
     void
  * @author   cc
  * @date     2018/09/30
  */
bool DataBasePlug::insertUserInfo(const QTalk::Entity::ImUserInfo &userInfo) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userInfo]() {
        UserDao dao(_dataBass);
        ret = dao.insertUserInfo(userInfo);
    });
    func.get();
    return ret;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
bool DataBasePlug::setUserCardInfo(const std::vector<QTalk::StUserCard> &userInfos) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userInfos]() {
        UserDao dao(_dataBass);
        ret = dao.setUserCardInfo(userInfos);
    });
    func.get();
    return ret;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
bool DataBasePlug::bulkInsertUserInfo(const std::vector<QTalk::Entity::ImUserInfo> &userInfos) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userInfos]() {
        UserDao dao(_dataBass);
        ret = dao.bulkInsertUserInfo(userInfos);
    });
    func.get();
    return ret;
}

std::shared_ptr<QTalk::Entity::ImUserInfo> DataBasePlug::getUserInfoByXmppId(const std::string &xmppid) {
    std::shared_ptr<QTalk::Entity::ImUserInfo> ret = nullptr;
    auto func = _dbPool.enqueue([this, &ret, xmppid]() {
        UserDao dao(_dataBass);
        ret = dao.getUserInfoByXmppId(xmppid);
    });
    func.get();
    return ret;
}

/**
  * @函数名
  * @功能描述 检查创建各表单
  * @参数
  * @date 2018.9.21
  */
void DataBasePlug::CreatTables() {
    DbConfig dbConfig(_dataBass);
    if (!_dataBass->tableExists("DB_Config") && !dbConfig.creatTable()) {
        error_log("DB_Config  表创建失败");
        dbConfig.creatTable();
    }
    SessionListDao userDao(_dataBass);
    if (!_dataBass->tableExists("IM_SessionList") && !userDao.creatTable()) {
        // "IM_SessionList 表创建失败";
        error_log("IM_SessionList  表创建失败");
    }
    MessageDao msgDao(_dataBass);
    if (!_dataBass->tableExists("IM_Message") && !msgDao.creatTable()) {
        // "IM_Message 表创建失败";
        error_log("IM_Message  表创建失败");
    }
    UserDao usrDao(_dataBass);
    if (!_dataBass->tableExists("IM_User") && !usrDao.creatTable()) {
        // "IM_User 表创建失败";
        error_log("IM_User  表创建失败");
    }
    FriendListDao friendDao(_dataBass);
    if (!_dataBass->tableExists("IM_Friend_List") && !friendDao.creatTable()) {
        // "IM_Friend_List 表创建失败";
        error_log("IM_Friend_List  表创建失败");
    }
    GroupDao grpDao(_dataBass);
    if (!_dataBass->tableExists("IM_Group") && !grpDao.creatTable()) {
        // "IM_Group 表创建失败";
        error_log("IM_Group  表创建失败");
    }
    GroupMemberDao grpMemer(_dataBass);
    if (!_dataBass->tableExists("IM_Group_Member") && !grpMemer.creatTable()) {
        error_log("IM_Group_Member  表创建失败");
    }

    ConfigDao confDao(_dataBass);
    if (!_dataBass->tableExists("IM_Config") && !confDao.creatTable()) {
        error_log("IM_Config  表创建失败");
    }
    UserSupplementDao userSupDao(_dataBass);
    if (!_dataBass->tableExists("IM_UserSupplement") && !userSupDao.creatTable()) {
        error_log("IM_UserSupplement  表创建失败");
    }
    CacheDataDao cacheDataDao(_dataBass);
    if (!_dataBass->tableExists("IM_Cache_Data") && !cacheDataDao.creatTable()) {
        error_log("IM_Cache_Data  表创建失败");
    }
    QuickReplyDao quickReplyDao(_dataBass);
    if ((!_dataBass->tableExists("IM_QUICK_REPLY_GROUP") || !_dataBass->tableExists("IM_QUICK_REPLY_CONTENT")) && !quickReplyDao.creatTable()) {
        error_log("IM_QUICK_REPLY_GROUP  表创建失败");
    }

    TriggerConfig triggerConfig(_dataBass);
    if(!triggerConfig.createUnreadInserttrigger() || !triggerConfig.createUnreadUpdateTrigger()){
        error_log("TriggerConfig  创建触发器失败");
    }
    // Medal
    MedalListDao medalListDao(_dataBass);
    if (!_dataBass->tableExists("IM_Medal_List") && !medalListDao.creatTable()) {
        error_log("IM_Medal_List  表创建失败");
    }
    UserMedalDao medalDao(_dataBass);
    if (!_dataBass->tableExists("IM_User_Status_Medal") && !medalDao.creatTable()) {
        error_log("IM_User_Status_Medal 表创建失败");
    }
}

//
void DataBasePlug::modifyDbByVersion() {
    int dbVersion = 0;
    DbConfig dbConfig(_dataBass);
    dbConfig.getDbVersion(dbVersion);
    // todo
    if (dbVersion <= 100001) {
        GroupDao groupDao(_dataBass);
        groupDao.deleteAllGroup();
    }
    if (dbVersion <= 100002) {
        MessageDao msgdao(_dataBass);
        msgdao.creatIndex();
    }

    if (dbVersion <= 100003) {
        UserDao userDao(_dataBass);
        userDao.addColumn_03();
    }

    if(dbVersion == 100005)
    {
        UserDao userDao(_dataBass);
        //
        userDao.modDefaultValue_05();
        // 删除历史数据 重新拉取
        userDao.clearData();
    }
    else if (dbVersion <= 100004) {
        // user表 增加字段 sex isRobot visible
        UserDao userDao(_dataBass);
        userDao.addColumn_04();
        // 删除历史数据 重新拉取
        userDao.clearData();
    }

    if(dbVersion <= 100007){
        //处理一次session未读
        SessionListDao sessionListDao(_dataBass);
        sessionListDao.updateUnreadCount();
        sessionListDao.updateRealJidForFixBug();

    }

    if(dbVersion <= 100008){
        //处理一次session未读
        SessionListDao sessionListDao(_dataBass);
        sessionListDao.fixMessageType();
        //
        MessageDao messageDao(_dataBass);
        messageDao.fixMessageType();
    }
    if(dbVersion <= 100009)
    {
        CacheDataDao dao(_dataBass);
        dao.clear_data_01();
    }
    //
    dbConfig.setDbVersion(Platform::instance().getDbVersion());
}

/**
  * @函数名   
  * @功能描述 配置表相关
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool DataBasePlug::insertConfig(const std::string &key, const std::string &subKey, const std::string &val) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, key, subKey, val]() {
        ConfigDao confDao(_dataBass);
        ret = confDao.insertConfig(key, subKey, val);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getConfig(const std::string &key, const std::string &subKey, std::string &val) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, key, subKey, &val]() {
        ConfigDao confDao(_dataBass);
        ret = confDao.getConfig(key, subKey, val);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getConfig(const std::string &key, std::map<std::string, std::string> &mapConf) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, key, &mapConf]() {
        ConfigDao confDao(_dataBass);
        ret = confDao.getConfig(key, mapConf);
    });
    func.get();
    return ret;
}

bool DataBasePlug::insertOrUpdateUserMood(const std::string &userId, const std::string &userMood, const int &version) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userId, userMood, version]() {
        UserSupplementDao dao(_dataBass);
        ret = dao.insertOrUpdateUserMood(userId, userMood, version);
    });
    func.get();
    return ret;
}

bool DataBasePlug::insertOrUpdateUserPhoneNo(const std::string &userId, const std::string &phoneNo) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userId, phoneNo]() {
        UserSupplementDao dao(_dataBass);
        ret = dao.insertOrUpdateUserPhoneNo(userId, phoneNo);
    });
    func.get();
    return ret;
}

bool DataBasePlug::insertOrUpdateUserSuppl(std::shared_ptr<QTalk::Entity::ImUserSupplement> imUserSup) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &imUserSup]() {
        UserSupplementDao dao(_dataBass);
        ret = dao.insertOrUpdateUserSuppl(imUserSup);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &structure]() {
        UserDao dao(_dataBass);
        ret = dao.getStructure(structure);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getUnreadedMessages(const std::string &messageId, std::vector<std::string> &msgIds) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, messageId, &msgIds]() {
        MessageDao dao(_dataBass);
        ret = dao.getUnreadedMessages(messageId, msgIds);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getGroupMessageLastUpdateTime(const std::string &messageId, QInt64 &time) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, messageId, &time]() {
        MessageDao dao(_dataBass);
        ret = dao.getGroupMessageLastUpdateTime(messageId, time);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getGroupUnreadedCount(const std::map<std::string, QInt64> &readMasks,
                                         std::map<std::string, int> &unreadedCount) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, readMasks, &unreadedCount]() {
        perf_counter("getGroupUnreadedCount");
        MessageDao dao(_dataBass);
        ret = dao.getGroupUnreadedCount(readMasks, unreadedCount);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getMessageByMessageId(const std::string &messageId, QTalk::Entity::ImMessageInfo &imMessageInfo) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, messageId, &imMessageInfo]() {
        MessageDao dao(_dataBass);
        ret = dao.getMessageByMessageId(messageId, imMessageInfo);
    });
    func.get();
    return ret;
}

bool DataBasePlug::updateRevokeMessage(const std::string &messageId) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, messageId]() {
        MessageDao dao(_dataBass);
        ret = dao.updateRevokeMessage(messageId);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getConfigVersion(int &version) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, & version]() {
        ConfigDao dao(_dataBass);
        ret = dao.getConfigVersion(version);
    });
    func.get();
    return ret;
}

bool DataBasePlug::bulkInsertConfig(const std::vector<QTalk::Entity::ImConfig> &config) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, config]() {
        ConfigDao dao(_dataBass);
        ret = dao.bulkInsertConfig(config);
    });
    func.get();
    return ret;
}

bool DataBasePlug::bulkRemoveConfig(const std::map<std::string, std::string> &mapConf) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, mapConf]() {
        ConfigDao dao(_dataBass);
        ret = dao.bulkRemoveConfig(mapConf);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getAllConfig(std::vector<QTalk::Entity::ImConfig> &configs) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &configs]() {
        ConfigDao dao(_dataBass);
        ret = dao.getAllConfig(configs);
    });
    func.get();
    return ret;
}

bool DataBasePlug::bulkDeleteGroup(const std::vector<std::string> &groupIds) {

    if (groupIds.empty()) {
        return true;
    }
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, groupIds]() {
        //
        GroupDao groupdao(_dataBass);
        groupdao.bulkDeleteGroup(groupIds);
        //
        GroupMemberDao gMemberDao(_dataBass);
        gMemberDao.bulkDeleteGroupMember(groupIds);
        //
        MessageDao msgDao(_dataBass);
        msgDao.bulkDeleteMessage(groupIds);
        //
        SessionListDao sessionDao(_dataBass);
        sessionDao.bulkDeleteSession(groupIds);

        ret = true;
    });
    func.get();
    return ret;
}

//
bool DataBasePlug::bulkInsertFriends(const std::vector<QTalk::Entity::IMFriendList> &friends) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, friends]() {
        FriendListDao dao(_dataBass);
        ret = dao.bulkInsertFriends(friends);
    });
    func.get();
    return ret;
}

bool DataBasePlug::insertFriend(QTalk::Entity::IMFriendList imfriend) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, imfriend]() {
        FriendListDao dao(_dataBass);
        ret = dao.insertFriend(imfriend);
    });
    func.get();
    return ret;
}

bool DataBasePlug::deleteAllFriends() {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret]() {
        FriendListDao dao(_dataBass);
        ret = dao.deleteAllFriends();
    });
    func.get();
    return ret;
}

bool DataBasePlug::deleteFriendByXmppId(const std::string &xmppId) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, xmppId]() {
        FriendListDao dao(_dataBass);
        ret = dao.deleteFriendByXmppId(xmppId);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getAllFriends(std::vector<QTalk::Entity::IMFriendList> &friends) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &friends]() {
        FriendListDao dao(_dataBass);
        ret = dao.getAllFriends(friends);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getAllGroup(std::vector<QTalk::Entity::ImGroupInfo> &groups) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &groups]() {
        GroupDao dao(_dataBass);
        ret = dao.getAllGroup(groups);
    });
    func.get();
    return ret;
}

bool DataBasePlug::deleteMessageByMessageId(const std::string &messageId) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, messageId]() {
        MessageDao dao(_dataBass);
        ret = dao.deleteMessageByMessageId(messageId);
    });
    func.get();
    return ret;
}

bool DataBasePlug::bulkDeleteUserInfo(const std::vector<std::string> &userIds) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, userIds]() {
        UserDao dao(_dataBass);
        ret = dao.bulkDeleteUserInfo(userIds);
    });
    func.get();
    return ret;
}

bool DataBasePlug::getStructureCount(const std::string &strName, int &count) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, strName, &count]() {
        UserDao dao(_dataBass);
        ret = dao.getStructureCount(strName, count);
    });
    func.get();
    return ret;
}


bool DataBasePlug::getStructureMember(const std::string &strName, std::vector<std::string> &arMember) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, strName, &arMember]() {
        UserDao dao(_dataBass);
        ret = dao.getStructureMember(strName, arMember);
    });
    func.get();
    return ret;
}

/**
 *
 */
bool DataBasePlug::getGroupCardById(std::shared_ptr<QTalk::Entity::ImGroupInfo> &group) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, &group]() {
        GroupDao dao(_dataBass);
        ret = dao.getGroupCardById(group);
    });
    func.get();
    return ret;
}

/**
 * 初始化Session
 * @return
 */
//bool DataBasePlug::initSessionList() {
//
//    bool ret = false;
//    auto func = _dbPool.enqueue([this, &ret]() {
//        // 删除原session
//        SessionListDao dao(_dataBass);
//        ret = dao.deleteAllSession();
//        // 生成新的session
//        if (ret) {
//            ret = dao.initSession();
//        }
//    });
//    func.get();
//    return ret;
//
//
//}

void DataBasePlug::getRecentSession(std::vector<QTalk::StShareSession> &sessions)
{
    auto func = _dbPool.enqueue([this, &sessions]() {
        SessionListDao dao(_dataBass);
        dao.getRecentSession(sessions);
    });
    func.get();
}

void DataBasePlug::geContactsSession(std::vector<QTalk::StShareSession> &sessions)
{
    auto func = _dbPool.enqueue([this, &sessions]() {
        UserDao dao(_dataBass);
        dao.geContactsSession(sessions);
    });
    func.get();
}

bool DataBasePlug::initConfigVersions() {

    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret]() {
        DbConfig dbConfig(_dataBass);
        ret = dbConfig.initVersions();
    });
    func.get();
    return ret;

}

bool DataBasePlug::insertUserId(std::string value) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, value]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.insertUserId(value);
    });
    func.get();
    return ret;
}

bool DataBasePlug::insertHotLine(std::string value) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, value]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.insertHotLine(value);
    });
    func.get();
    return ret;
}

void DataBasePlug::getHotLines(std::string &hotLines) {
    auto func = _dbPool.enqueue([this, &hotLines]() {
        CacheDataDao cacheDataDao(_dataBass);
        cacheDataDao.getHotLines(hotLines);
    });
    func.get();
}

bool DataBasePlug::isHotlineMerchant(const std::string xmppid) {
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, xmppid]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.isHotlineMerchant(xmppid);
    });
    func.get();
    return ret;
}

std::string DataBasePlug::getGroupReadMarkTime(){
    std::string ret = "0";
    auto func = _dbPool.enqueue([this, &ret]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.getGroupReadMarkTime();
    });
    func.get();
    return ret;
}
bool DataBasePlug::updateGroupReadMarkTime(std::string time){
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, time]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.updateGroupReadMarkTime(time);
    });
    func.get();
    return ret;
}

std::string DataBasePlug::getLoginBeforeGroupReadMarkTime(){
    std::string ret = "0";
    auto func = _dbPool.enqueue([this, &ret]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.getLoginBeforeGroupReadMarkTime();
    });
    func.get();
    return ret;
}

bool DataBasePlug::saveLoginBeforeGroupReadMarkTime(const std::string &time){
    bool ret = false;
    auto func = _dbPool.enqueue([this, &ret, time]() {
        CacheDataDao cacheDataDao(_dataBass);
        ret = cacheDataDao.saveLoginBeforeGroupReadMarkTime(time);
    });
    func.get();
    return ret;
}


void DataBasePlug::getBeforeImageMessage(const std::string &messageId,
                                         std::vector<std::pair<std::string, std::string>> &msgs) {
    auto func = _dbPool.enqueue([this, &msgs, messageId]() {
        MessageDao dao(_dataBass);
        dao.getBeforeImageMessage(messageId, msgs);
    });
    func.get();
}

void DataBasePlug::getNextImageMessage(const std::string &messageId,
                                       std::vector<std::pair<std::string, std::string>> &msgs) {
    auto func = _dbPool.enqueue([this, &msgs, messageId]() {
        MessageDao dao(_dataBass);
        dao.getNextImageMessage(messageId, msgs);
    });
    func.get();
}

/**
 *
 * @param users
 */
void DataBasePlug::getCareUsers(std::set<std::string> &users)
{
    auto func = _dbPool.enqueue([this, &users]() {
        GroupMemberDao dao(_dataBass);
        dao.getCareUsers(users);
    });
    func.get();
}

void DataBasePlug::getGroupCardMaxVersion(long long &version)
{
    GroupDao dao(_dataBass);
    dao.getGroupCardMaxVersion(version);
}

/**
 *
 * @param members
 */
void DataBasePlug::getAllGroupMembers(std::map<std::string, std::set<std::string>> &members)
{
    GroupMemberDao dao(_dataBass);
    dao.getAllGroupMembers(members);
}

/**
 * 快捷回复
 * @param data
 */
void DataBasePlug::batchInsertQuickReply(const std::string &data) {
    QuickReplyDao dao(_dataBass);
    dao.batchInsertQuickReply(data);
}

void DataBasePlug::getQuickReplyVersion(QInt64 version[]) {
    QuickReplyDao dao(_dataBass);
    dao.getQuickReplyVersion(version);
}

void DataBasePlug::getQuickGroups(std::vector<QTalk::Entity::ImQRgroup> &groups) {
    QuickReplyDao dao(_dataBass);
    dao.getQuickGroups(groups);
}

void DataBasePlug::getQuickContentByGroup(std::vector<QTalk::Entity::IMQRContent>& contents,int id){
    QuickReplyDao dao(_dataBass);
    dao.getQuickContentByGroup(contents,id);
}

void DataBasePlug::getLocalMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                   std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    MessageDao dao(_dataBass);
    dao.getLocalMessage(time, userId, realJid, msgList);
}

void DataBasePlug::getFileMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                  std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    MessageDao dao(_dataBass);
    dao.getFileMessage(time, userId, realJid, msgList);
}

void DataBasePlug::getImageMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                   std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    MessageDao dao(_dataBass);
    dao.getImageMessage(time, userId, realJid, msgList);
}

void DataBasePlug::getSearchMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                    const std::string &searchKey, std::vector<QTalk::Entity::ImMessageInfo> &msgList) {

    MessageDao dao(_dataBass);
    dao.getSearchMessage(time, userId, realJid, searchKey, msgList);
}

void DataBasePlug::getAfterMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                   std::vector<QTalk::Entity::ImMessageInfo> &msgList) {

    MessageDao dao(_dataBass);
    dao.getAfterMessage(time, userId, realJid, msgList);

}

// 批量更新单人消息阅读状态
void DataBasePlug::updateMessageReadFlags(const std::map<std::string, int> &readFlags) {
    MessageDao dao(_dataBass);
    dao.updateMessageReadFlags(readFlags);
}

void DataBasePlug::updateMessageExtendInfo(const std::string &msgId, const std::string &info) {
    auto func = _dbPool.enqueue([this, &msgId, info]() {
        MessageDao dao(_dataBass);
        dao.updateMessageExtendInfo(msgId, info);
    });
    func.get();
}

//
void DataBasePlug::insertMedalList(const std::vector<QTalk::Entity::ImMedalList> &medals) {
    auto func = _dbPool.enqueue([this, medals]() {
        MedalListDao dao(_dataBass);
        dao.insertMedalList(medals);
    });
    func.get();
}

void DataBasePlug::insertMedals(const std::vector<QTalk::Entity::ImUserStatusMedal> &medals) {
    auto func = _dbPool.enqueue([this, medals]() {
        UserMedalDao dao(_dataBass);
        dao.insertMedals(medals);
    });
    func.get();
}

void DataBasePlug::getUserMedal(const std::string &xmppId, std::set<QTalk::StUserMedal> &stMedal) {
    UserMedalDao dao(_dataBass);
    dao.getUserMedal(xmppId, stMedal);
}

void DataBasePlug::getMedalList(std::vector<QTalk::Entity::ImMedalList> &medals) {
    MedalListDao dao(_dataBass);
    dao.getMedalList(medals);
}

//
void DataBasePlug::getMedalUsers(int medalId, std::vector<QTalk::StMedalUser> &metalUsers)
{
    UserMedalDao dao(_dataBass);
    dao.getMedalUsers(medalId, metalUsers);
}

void DataBasePlug::modifyUserMedalStatus(const std::string& userId, int medalId, int status) {
    auto func = _dbPool.enqueue([this, userId, medalId, status]() {
        UserMedalDao dao(_dataBass);
        dao.modifyUserMedalStatus(userId, medalId, status);
    });
    func.get();
}
