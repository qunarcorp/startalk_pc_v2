#include <utility>

#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "communication_global.h"
#include "MessageManager.h"
#include <thread>
#include <regex>
#include "../include/Spinlock.h"
#include "../include/STLazyQueue.h"
#include "../interface/logic/ILogicObject.h"
#include "../QtUtil/Entity/JID.h"
#include "../QtUtil/lib/http/QtHttpRequest.h"
#include "../include/CommonStrcut.h"

typedef std::map<std::string, std::map<std::string, int>> UserCardMapParam;


class FileHelper;
class UserManager;
class GroupManager;
class OnLineManager;
class SearchManager;
class OfflineMessageManager;
class UserConfig;
class HotLinesConfig;
class LOGICMANAGER_EXPORT Communication : public ILogicObject {
public:
    Communication();
    ~Communication() override;

public:
    bool OnLogin(const std::string& userName, const std::string& password);
    void
    AsyncConnect(const std::string &userName, const std::string &password, const std::string &host, int port);

    void tryConneteToServer();
    void tryConneteToServerByQVT();

public:
    void addHttpRequest(const QTalk::HttpRequest &req, const std::function<void(int, const std::string &)>& callback);

public:
    void dealBindMsg();
    //
    void updateTimeStamp();
    // 获取导航信息
    bool getNavInfo(const std::string &navAddr, QTalk::StNav &nav);
    //
    void setLoginNav(const QTalk::StNav &nav);
    // 登陆后同步服务器
    void synSeverData();
    // 获取用户的状态
    void synUsersUserStatus();
    // 获取某人的历史消息
    void getUserHistoryMessage(const QInt64 &time, const QUInt8 &chatType, const std::string &userId,
                               const std::string &realJid,
                               std::vector<QTalk::Entity::ImMessageInfo> &msgList);
    //批量获取头像信息
    void batchUpdateHead(const std::vector<std::string> &arXmppids);
    //创建群组
    void createGroup(const std::string &groupId, const std::string &groupName);
    //邀请用户入群
    void inviteGroupMembers(std::vector<std::string> &users, const std::string &groupId);
    // 获取群成员
    void getGroupMemberById(const std::string &groupId);
    //
    void getNetEmoticon(GetNetEmoticon &e);

    void downloadUserHeadByStUserCard(const std::vector<QTalk::StUserCard> &arUserInfo);
    //
    void getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure);

    // 获取会话信息
    void getSessionData();
    //
    void updateUserConfigFromDb();
    //
    void getStructureCount(const std::string &keyName, int &count);
    //
    void getStructureMember(const std::string &keyName, std::vector<std::string> &arMember);
    //
    void downloadCollection(const std::vector<std::string> &arDownloads);
    // 动态获取oa部分 ui组件
    bool geiOaUiData(std::vector<QTalk::StOAUIData> &arOAUIData);

    // 向服务器同步在离线时长
    void sendUserOnlineState(const QInt64& loginTime, const QInt64& logoutTime, const std::string& ip);

    // 发送点击统计
    void sendOperatorStatistics(const std::string& ip, const std::vector<QTalk::StActLog>& operators);

    //
    void reportLog(const std::string &desc, const std::string &logPath);
    void reportDump(const std::string &dmp_path);

    void saveUserConfig();

    void clearSystemCache();

    void getUserCard(std::shared_ptr<QTalk::Entity::ImUserInfo>& info);
    // 移除会话
    void removeSession(const string &peerId);

public: // 群组相关

    //
    void getGroupCardInfo(std::shared_ptr<QTalk::Entity::ImGroupInfo> &e);

    // 设置群管理员
    void setGroupAdmin(const std::string& groupId, const std::string &nickName,
            const std::string &memberJid, bool isAdmin);

    // 移除群成员
    void removeGroupMember(const std::string& groupId,
            const std::string& nickName,
            const std::string& memberJid);

    // 退出群
    void quitGroup(const std::string &groupId);
    // 销毁群
    void destroyGroup(const std::string &groupId);
    // 修改头像
    void changeUserHead(const std::string &headPath);

public:
    //
    void onRecvGroupMembers(const std::string &groupId, const std::map<std::string, QUInt8>& mapUserRole);
    //
    void onCreateGroupComplete(const std::string &groupId, bool ret);
    //
    void onInviteGroupMembers(const std::string &groupId);
    //
    void onRecvFriendList(const std::vector<QTalk::Entity::IMFriendList> &friends);
    //
    void onUserJoinGroup(const std::string& groupId, const std::string& memberId, int affiliation);
    //
    void onStaffChanged();

private:
    // 获取好友列表
    void getFriendList();

public:
    void setServiceSeat(int sid, int seat);
    void serverCloseSession(const std::string &username, const std::string &seatname, const std::string &virtualname);
    void sendProduct(const std::string username, const std::string virtualname,const std::string product,const std::string type);
    void getSeatList(const QTalk::Entity::UID uid);
    void sessionTransfer(const QTalk::Entity::UID uid, const std::string newCser,const std::string reason);
    void sendWechat(const QTalk::Entity::UID uid);

//qchat 登录相关
public:
    std::string getQchatQvt(const std::string &userName, const std::string &password);
    std::map<std::string,std::string> getQchatTokenByQVT(const std::string &qvt);

public:
    // 最近聊天
    void getRecntSession(std::vector<QTalk::StShareSession> &sessions);
    // 联系人
    void geContactsSession(std::vector<QTalk::StShareSession> &sessions);

public:
    CommMsgManager *_pMsgManager;
    FileHelper *_pFileHelper;
    UserManager *_pUserManager;
    GroupManager *_pUserGroupManager;
    OnLineManager *_pOnLineManager;
    SearchManager *_pSearchManager;
    UserConfig *_pUserConfig;
    HotLinesConfig *_pHostLinesConfig;

    OfflineMessageManager *_pOfflineMessageManager;


private:
    CommMsgListener *_pMsgListener;

    QTalk::util::spin_mutex sm;

private:
    std::map<std::string, std::string> _mapGroupIdName;

private:
    std::string _userName;
    std::string _password;
    std::string _host;
    int _port;

private:
    const int _threadPoolCount;
    ThreadPool _socketPool;         // 线程 根据功能判断
    STLazyQueue<std::pair<std::string, UserCardMapParam>> *userCardQueue;
    std::vector<ThreadPool *> _httpPool;
};

#endif // _COMMUNICATION_H_

