#include <utility>
#include <iostream>
#include "Communication.h"
#include "../entity/IM_Session.h"
#include "../LogicManager/LogicManager.h"
#include "../QtUtil/Entity/JID.h"
#include "../QtUtil/Enum/im_enum.h"
#include "FileHelper.h"
#include "OfflineMessageManager.h"
#include "../Platform/Platform.h"
#include "UserManager.h"
#include "GroupManager.h"
#include "../QtUtil/Utils/utils.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../Message/GroupMessage.h"
#include "../QtUtil/Utils/Log.h"
#include "OnLineManager.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../include/CommonStrcut.h"
#include "SearchManager.h"
#include "UserConfig.h"
#include "HotLinesConfig.h"
#include "../Platform/NavigationManager.h"
#include "../Platform/dbPlatForm.h"
#include "../include/perfcounter.h"
#include <time.h>
#include "../interface/logic/ILogicBase.h"

#if defined(_MACOS)
    #define DEM_CLIENT_TYPE ClientTypeMac
    #include <unistd.h>
#elif defined(_LINUX)
    #define DEM_CLIENT_TYPE ClientTypeLinux
    #include <unistd.h>
#else
    #include<windows.h>
    #define DEM_CLIENT_TYPE ClientTypePC
#endif
//

#define QTALK_2_0_CONFIG "QTALK_2_0_CONFIG"

#define LOGIN_TYPE_NEW_PASSWORD "newpassword"

using namespace QTalk;
using namespace QTalk::JSON;

Communication::Communication()
        : _threadPoolCount(3),
          _port(0) {
    _pMsgManager = new CommMsgManager;
    _pMsgListener = new CommMsgListener(this);
    _pFileHelper = new FileHelper(this);
    _pUserManager = new UserManager(this);
    _pUserGroupManager = new GroupManager(this);
    _pOnLineManager = new OnLineManager(this);
    _pSearchManager = new SearchManager(this);
    _pOfflineMessageManager = new OfflineMessageManager(this);
    _pUserConfig = new UserConfig(this);
    _pHotLinesConfig = new HotLinesConfig(this);
    for (int i = 0; i < _threadPoolCount; ++i) {
        _httpPool.push_back(new ThreadPool(SFormat("Communication's http pool {0}", i).c_str()));
    }


    std::function<int(STLazyQueue<std::pair<std::string, UserCardMapParam>> *)> usercardFun
            = [this](STLazyQueue<std::pair<std::string, UserCardMapParam>> *queue) -> int {
                int runningCount = 0;
                if (queue != nullptr && !queue->empty()) {
                    std::map<std::string, UserCardMapParam> params;
                    while (!queue->empty()) {
                        auto tmpParam = queue->front();
                        const std::string &groupId = tmpParam.first;
                        auto tmp = tmpParam.second;
                        for (const auto &item : tmp) {
                            params[groupId][item.first] = item.second;
                        }
                        queue->pop();
                        runningCount++;
                    }
                    // 插库
                    if (_pUserManager) {

                        for (const auto &it : params) {
                            std::vector<QTalk::StUserCard> arUserInfo;
                            _pUserManager->getUserCard(it.second, arUserInfo);
                            // 下载头像
                            downloadUserHeadByStUserCard(arUserInfo);
                            if (_pMsgManager) {
                                _pMsgManager->updateGroupMemberInfo(it.first, arUserInfo);
                            }
                        }
                    }
                }
                return runningCount;
            };


    userCardQueue = new STLazyQueue<std::pair<std::string, UserCardMapParam>>(50, usercardFun);
    //
//    _pUpdaterManager = new UpdaterManager(this);
}


Communication::~Communication() {
    if (nullptr != _pMsgManager) {
        delete _pMsgManager;
        _pMsgManager = nullptr;
    }
    if (nullptr != _pMsgListener) {
        delete _pMsgListener;
        _pMsgListener = nullptr;
    }
    if (nullptr != _pFileHelper) {
        delete _pFileHelper;
        _pFileHelper = nullptr;
    }
    if (_pUserManager) {
        delete _pUserManager;
        _pUserManager = nullptr;
    }
    if (_pOnLineManager) {
        delete _pOnLineManager;
        _pOnLineManager = nullptr;
    }
    if (nullptr != _pOfflineMessageManager) {
        delete _pOfflineMessageManager;
        _pOfflineMessageManager = nullptr;
    }
}

/**
 * 登录处理
 * @param userName
 * @param password
 * @return
 */
bool Communication::OnLogin(const std::string& userName, const std::string& password)
{
    // 设置当前登录的域名
    Platform::instance().setSelfUserId(userName);
    // 下载公钥
    if(!_pFileHelper->DownloadPubKey()) return false;

    const std::string host = NavigationManager::instance().getXmppHost();
    const int port = NavigationManager::instance().getProbufPort();
    std::string domain = NavigationManager::instance().getDomain();

    info_log("login at host:{0} domain:{1} port: {2}", host, domain, port);

    if (port == 0 || domain.empty() || host.empty()) {
        //
        warn_log("nav info error (port == 0 || domain.empty())");
        if (_pMsgManager) {
            _pMsgManager->sendLoginErrMessage("获取服务器地址失败!");
        }
        return false;
    }

    //增加新登录逻辑
    std::string loginType = NavigationManager::instance().getLoginType();
    if(!loginType.empty() && LOGIN_TYPE_NEW_PASSWORD == loginType){
        std::map<std::string,std::string> map;
        getNewLoginToken(userName,password,map);
        std::string u = map["u"];
        std::string p = map["t"];
        if(!u.empty() && !p.empty()){
            char uuid[36];
            utils::generateUUID(uuid);
            cJSON *nauth = cJSON_CreateObject();
            cJSON *gObj = cJSON_CreateObject();
            cJSON_AddStringToObject(gObj, "u", userName.c_str());
            cJSON_AddStringToObject(gObj, "p", p.c_str());
            cJSON_AddStringToObject(gObj, "mk", uuid);
            cJSON_AddItemToObject(nauth,"nauth",gObj);
            std::string pp = QTalk::JSON::cJSON_to_string(nauth);
            cJSON_Delete(nauth);
            AsyncConnect(u + "@" + domain, pp, host, port);
        } else {
            if (_pMsgManager) {
                _pMsgManager->sendLoginErrMessage("获取token失败!");
            }
            return false;
        }
    } else{
        std::string loginName = userName + "@" + domain;

#ifndef _QCHAT
        AsyncConnect(loginName, password, host, port);
#else
        std::string plaint = LogicManager::instance()->getLogicBase()->chatRsaEncrypt(password);
        std::string qvt = getQchatQvt(userName, plaint);
        if(qvt.empty()){
            AsyncConnect(loginName, password, host, port);
        } else{
            std::map<std::string,std::string> map;
            getQchatTokenByQVT(qvt,map);
            std::string token = map["password"];
            if(token.empty()){
                AsyncConnect(loginName, password, host, port);
            } else{
                AsyncConnect(loginName, token, host, port);
            }
        }
#endif
    }
    return true;
}

//
void Communication::AsyncConnect(const std::string &userName, const std::string &password, const std::string &host,
                                 int port) {
    info_log("start login: user:{0}, password length:{1}, host:{2}, port:{3}", userName, password.length(), host, port);
    _userName = userName;
    _password = password;
    _host = host;
    _port = port;

    tryConneteToServer();
}

void Communication::tryConneteToServer()
{
    bool isNewLogin = (LOGIN_TYPE_NEW_PASSWORD == NavigationManager::instance().getLoginType());
    LogicManager::instance()->getLogicBase()->tryConnectToServer(_userName, _password, _host, _port,isNewLogin);
}

/**
 * qchat断线重连 需要根据qvt重新获取token
 */
void Communication::tryConneteToServerByQVT()
{
    std::string qvt = Platform::instance().getQvt();
    std::map<std::string,std::string> userMap;
    getQchatTokenByQVT(qvt,userMap);
    LogicManager::instance()->getLogicBase()->tryConnectToServer(_userName, userMap["password"], _host, _port, false);
}

/**
 * 获取导航信息
 * @param navAddr
 */
bool Communication::getNavInfo(const std::string &navAddr, QTalk::StNav &nav) {
    std::string url = navAddr;
    if (navAddr.find('?') == std::string::npos)
        url += "?p=pc";
    else
        url += "&p=pc";

    bool ret = false;
    auto func = [url, &ret, &nav](int code, const std::string &resData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(resData.data());
            if (nullptr == data) {
                throw std::logic_error("nav data parse error");
            }

            nav.version = cJSON_SafeGetLonglongValue(data, "version", 0);
            // base address
            cJSON *baseAddress = cJSON_GetObjectItem(data, "baseaddess");
            nav.xmppHost = cJSON_SafeGetStringValue(baseAddress, "xmpp");
            nav.domain = cJSON_SafeGetStringValue(baseAddress, "domain");
            nav.protobufPcPort = cJSON_SafeGetIntValue(baseAddress, "protobufPcPort");
            nav.apiurl = cJSON_SafeGetStringValue(baseAddress, "apiurl");
            nav.javaurl = cJSON_SafeGetStringValue(baseAddress, "javaurl");
            nav.httpurl = cJSON_SafeGetStringValue(baseAddress, "httpurl");
            nav.pubkey = cJSON_SafeGetStringValue(baseAddress, "pubkey");
            nav.fileurl = cJSON_SafeGetStringValue(baseAddress, "fileurl");
            nav.mobileurl = cJSON_SafeGetStringValue(baseAddress, "mobileurl");
            nav.leaderUrl = cJSON_SafeGetStringValue(baseAddress, "leaderurl");
            nav.shareUrl = cJSON_SafeGetStringValue(baseAddress, "shareurl");
            nav.videoUrl = cJSON_SafeGetStringValue(baseAddress, "videourl");

            //imcofig
            if (cJSON_HasObjectItem(data, "imConfig")) {
                cJSON *imcofig = cJSON_GetObjectItem(data, "imConfig");
                nav.rsaEncodeType = cJSON_SafeGetIntValue(imcofig, "RsaEncodeType");
                nav.uploadLog = cJSON_SafeGetStringValue(imcofig, "uploadLog");
                nav.mailSuffix = cJSON_SafeGetStringValue(imcofig, "mail");
                nav.foundConfigUrl = cJSON_SafeGetStringValue(imcofig, "foundConfigUrl");
            }

            // ops
            if (cJSON_HasObjectItem(data, "ops")) {
                cJSON *ops = cJSON_GetObjectItem(data, "ops");
                nav.opsHost = cJSON_SafeGetStringValue(ops, "host");
            }

            // ability
            if (cJSON_HasObjectItem(data, "ability")) {
                cJSON *ability = cJSON_GetObjectItem(data, "ability");
                nav.qCloudHost = cJSON_SafeGetStringValue(ability, "qCloudHost");
                nav.searchUrl = cJSON_SafeGetStringValue(ability, "new_searchurl");
                nav.showmsgstat = cJSON_SafeGetBoolValue(ability,"showmsgstat", false);
                nav.qcGrabOrder =  cJSON_SafeGetStringValue(ability, "qcGrabOrder");
            }
            //qcadmin
            if(cJSON_HasObjectItem(data,"qcadmin")){
                cJSON *qcadmin = cJSON_GetObjectItem(data, "qcadmin");
                nav.qcadminHost = cJSON_SafeGetStringValue(qcadmin, "host");
            }
            //Login
            if(cJSON_HasObjectItem(data,"Login")){
                cJSON *login = cJSON_GetObjectItem(data, "Login");
                if(cJSON_HasObjectItem(login,"loginType")){
                    nav.loginType = cJSON_SafeGetStringValue(login, "loginType");
                }
            }
            ret = true;

            cJSON_Delete(data);
        }
    };

    try {
        QTalk::HttpRequest req(url);
        addHttpRequest(req, func);
    }
    catch (const std::exception &e) {
        error_log(e.what());
    }

    return ret;
}

/**
 *
 * @param nav
 */
void Communication::setLoginNav(const QTalk::StNav &nav) {
    NavigationManager::instance().setVersion(nav.version);
    NavigationManager::instance().setXmppHost(nav.xmppHost);
    NavigationManager::instance().setDomain(nav.domain);
    Platform::instance().setSelfDomain(nav.domain);
    NavigationManager::instance().setProbufPort(nav.protobufPcPort);
    NavigationManager::instance().setApiHost(nav.apiurl);
    NavigationManager::instance().setJavaHost(nav.javaurl);
    NavigationManager::instance().setHttpHost(nav.httpurl);
    NavigationManager::instance().setPubKey(nav.pubkey);
    NavigationManager::instance().setFileHttpHost(nav.fileurl);
    NavigationManager::instance().setPhoneNumAddr(nav.mobileurl);
    NavigationManager::instance().setOpsApiHost(nav.opsHost);
    NavigationManager::instance().setQCHost(nav.qCloudHost);
    NavigationManager::instance().setQcadminHost(nav.qcadminHost);
    NavigationManager::instance().setRsaEncodeType(nav.rsaEncodeType);
    NavigationManager::instance().setSearchUrl(nav.searchUrl);
    NavigationManager::instance().setUploadLog(nav.uploadLog);
    NavigationManager::instance().setMailSuffix(nav.mailSuffix);
    NavigationManager::instance().setLeaderUrl(nav.leaderUrl);
    NavigationManager::instance().setShareUrl(nav.shareUrl);
    NavigationManager::instance().setFoundConfigUrl(nav.foundConfigUrl);
    NavigationManager::instance().setShowmsgstat(nav.showmsgstat);
    NavigationManager::instance().setQcGrabOrder(nav.qcGrabOrder);
    NavigationManager::instance().setLoginType(nav.loginType);
    // video
    NavigationManager::instance().setVideoUrl(nav.videoUrl);
}

/**
  * @函数名
  * @功能描述 登陆成功后同步各服务器数据 同步完成后发送同步信息完成event
  * @参数
  * @date 2018.9.28
  */
void Communication::synSeverData() {

    try {
        if(nullptr == _pMsgManager)
            return;
        // 发发送一次心跳
        LogicManager::instance()->getLogicBase()->sendHeartbeat();
//        sendHeartbeat();
        // 获取组织架构
        info_log("获取组织架构");
        _pMsgManager->sendLoginProcessMessage("getting user information");
        if (_pUserManager && _pMsgManager) {
            bool ret = _pUserManager->getNewStructure();
            _pMsgManager->sendGotStructure(ret);
        }
        // 获取群信息
        _pMsgManager->sendLoginProcessMessage("getting group information");
        if (_pUserGroupManager && _pMsgManager) {
            info_log("获取群信息");
            MapGroupCard groups;
            if (_pUserGroupManager->getUserGroupInfo(groups)) {
//                _pUserGroupManager->getGroupCard(groups);
                _pUserGroupManager->getUserIncrementMucVcard();
                _pMsgManager->sendGotGroupCard();
            }
        }
        // 获取单人配置
        info_log("获取单人配置");
        _pMsgManager->sendLoginProcessMessage("initializing configuration");
        _pUserConfig->getUserConfigFromServer(false);
        // 初始化配置
        info_log("初始化配置");
        std::string strPlatform = Platform::instance().getPlatformStr();
        std::string userConfig;
        LogicManager::instance()->getDatabase()->getConfig(QTALK_2_0_CONFIG, strPlatform, userConfig);
        AppSetting::instance().initAppSetting(userConfig);
        // 获取好友列表
        info_log("获取好友列表");
        getFriendList();
        // 获取单人离线消息
        info_log("获取单人消息");
        _pMsgManager->sendLoginProcessMessage("getting user message");
        bool isok = _pOfflineMessageManager->updateChatOfflineMessage();
        //
        if(!isok)
        {
            _pMsgManager->sendGetHistoryError();
            return;
        }
        _pMsgManager->sendLoginProcessMessage("updating message read mask");
        _pOfflineMessageManager->updateChatMasks();

        // 获取群离线消息
        info_log("获取群离线消息");
        _pMsgManager->sendLoginProcessMessage("getting group message");
        isok = _pOfflineMessageManager->updateGroupOfflineMessage();
        if(!isok)
        {
            _pMsgManager->sendGetHistoryError();
            return;
        }
        //
        info_log("更新群阅读状态");
        _pOfflineMessageManager->updateGroupMasks();
        // 获取通知消息
        info_log("获取通知消息");
        _pMsgManager->sendLoginProcessMessage("getting notice message");
        isok = _pOfflineMessageManager->updateNoticeOfflineMessage();
        if(!isok)
        {
            _pMsgManager->sendGetHistoryError();
            return;
        }
        //获取热线账号
        info_log("获取热线账号信息");
        _pHotLinesConfig->getVirtualUserRole();
#ifdef _QCHAT
        info_log("获取坐席状态");
        _pHotLinesConfig->getServiceSeat();
        info_log("获取快捷回复");
        _pHotLinesConfig->updateQuickReply();
#endif
        // 根据离线消息生成sessionList
        //LogicManager::instance()->GetDatabase()->QueryImSessionInfos();
        // 获取最新版本号
//        initAppNetVersion();
        // 增量拉取勋章信息
        getMedalList();
        getUserMedal();

        //
        _pMsgManager->sendLoginProcessMessage("getting user state");
        if (_pMsgManager) {
            _pMsgManager->sendSynOfflineSuccees();
            // 同步完离线后 开启在线信息查询定时器
            synUsersUserStatus();
        }
        _pMsgManager->sendLoginProcessMessage("login success");
    }
    catch (const std::exception &e) {
        error_log(e.what());
        throw std::runtime_error(e.what());
    }

}


void Communication::synUsersUserStatus() {

    std::thread([this]() {
        if (_pOnLineManager) {
#ifdef _MACOS
            pthread_setname_np("synUsersUserStatus thread");
#endif
            perf_counter("syn users status");

            std::set<std::string> users;
            LogicManager::instance()->getDatabase()->getCareUsers(users);

            info_log("will get user states count:{0}", users.size());

            std::set<std::string> tmpUsers;
            for(const auto& u : users)
            {
                tmpUsers.insert(u);

                if(tmpUsers.size() >= 300)
                {
                    if(_pOnLineManager)
                        _pOnLineManager->OnGetOnLineUser(tmpUsers);
                    tmpUsers.clear();
                }
            }

            if(_pOnLineManager && !tmpUsers.empty())
                _pOnLineManager->OnGetOnLineUser(tmpUsers);

            if(_pMsgManager)
                _pMsgManager->sendOnlineUpdate();
        }

    }).detach();

}

/**
  * @函数名   getUserHistoryMessage
  * @功能描述 获取对应人小于对应时间的历史消息
  * @参数     time 时间 userName 人
     void
  * @author   cc
  * @date     2018/09/28
  */
void Communication::getUserHistoryMessage(const QInt64 &time, const QUInt8 &chatType, const std::string &userId,
                                          const std::string &realJid,
                                          std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    bool ret = LogicManager::instance()->getDatabase()->getUserMessage(time, userId, realJid, msgList);
    if (!ret || msgList.empty()) {
        switch (chatType) {
            case QTalk::Enum::TwoPersonChat:
            case QTalk::Enum::Consult:
                _pOfflineMessageManager->getUserMessage(time, userId);
                break;
            case QTalk::Enum::GroupChat:
                _pOfflineMessageManager->getGroupMessage(time, userId);
                // 群消息需要更新一下群阅读状态
                //_pOfflineMessageManager->updateGroupMasks();
                break;
            case QTalk::Enum::System:
                _pOfflineMessageManager->getSystemMessage(time, userId);
                break;
            case QTalk::Enum::ConsultServer:
                _pOfflineMessageManager->getConsultServerMessage(time,userId,realJid);
                break;
            default:
                break;
        }
        LogicManager::instance()->getDatabase()->getUserMessage(time, userId, realJid, msgList);
    }
}



/**
  * @函数名   onCreateGroupCompleted
  * @功能描述 收到创建群通知
  * @参数
  * @author   may
  * @date     2018/11/10
  */
void Communication::onCreateGroupComplete(const std::string &groupId, bool ret) {

    if (ret && _pUserGroupManager) {
        QTalk::StGroupInfo groupInfo;
        groupInfo.desc = "";
        groupInfo.groupId = groupId;
        if (_mapGroupIdName.find(groupId) != _mapGroupIdName.end() && !_mapGroupIdName[groupId].empty()) {
            groupInfo.name = _mapGroupIdName[groupId];
            _mapGroupIdName.erase(groupId);
        } else
            groupInfo.name = "新建群聊...";

        groupInfo.title = "";

        QTalk::Entity::ImGroupInfo im_gInfo;
        im_gInfo.GroupId = groupInfo.groupId;
        LogicManager::instance()->getDatabase()->insertGroupInfo(im_gInfo);

        std::vector<QTalk::StGroupInfo> arInfos;
        arInfos.push_back(groupInfo);
        _pUserGroupManager->upateGroupInfo(arInfos);
    }
}

/**
  * @函数名   onRecvGroupMembers
  * @功能描述 收到群成员信息
  * @参数
  * @author   cc
  * @date     2018/10/08
  */
void Communication::onRecvGroupMembers(const std::string &groupId, const std::map<std::string, QUInt8>& mapUserRole) {

    std::map<std::string, QTalk::StUserCard> mapStUsers;
    // 存储数据库
    if (!mapUserRole.empty())
        LogicManager::instance()->getDatabase()->bulkInsertGroupMember(groupId, mapUserRole);
    else
        return;

    for(const auto& item : mapUserRole)
    {
        auto card = QTalk::StUserCard();
        card.xmppId = item.first;
        mapStUsers[item.first] = card;
    }

    LogicManager::instance()->getDatabase()->getGroupMemberInfo(mapStUsers);
    //
    if (!mapStUsers.empty()) {
        // 下载头像
//            downloadUserHeadByStUserCard(arStUsers);
        if (_pMsgManager) {
            GroupMemberMessage e;
            e.groupId = groupId;
            e.userRole = mapUserRole;
            e.members = mapStUsers;
            _pMsgManager->gotGroupMember(e);
        }
        // 获取群成员名片
        std::set<std::string> users;
        UserCardMapParam param;
        for (auto &itm : mapStUsers) {
            const std::string &strXmppId = itm.first;
            auto pos = strXmppId.find('@');
            if (pos != -1) {

                users.insert(strXmppId);

                std::string userId = strXmppId.substr(0, pos);
                std::string domain = strXmppId.substr(pos + 1);
                param[domain][userId] = itm.second.version;
            }
        }
        userCardQueue->push(std::pair<std::string, UserCardMapParam>(groupId, param));
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/26
  */
void Communication::downloadUserHeadByStUserCard(const std::vector<QTalk::StUserCard> &arUserInfo) {
    std::vector<std::string> urls;
    for (const QTalk::StUserCard &user : arUserInfo) {
        if (user.headerSrc.empty()) continue;
        urls.push_back(user.headerSrc);
    }
    if (_pFileHelper) {
        if (urls.empty()) {
            return;
        }
        _pFileHelper->batchDownloadHead(urls);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void Communication::batchUpdateHead(const std::vector<std::string> &arXmppids) {
    if (arXmppids.empty()) return;

    if (_pUserManager) {
        std::map<std::string, std::map<std::string, int>> params;

        for (const auto &arXmppid : arXmppids) {
            auto pos = arXmppid.find('@');
            if (pos != -1) {
                std::string user = arXmppid.substr(0, pos);
                std::string domain = arXmppid.substr(pos + 1);

                params[domain][user] = 0;
            }
        }

        std::thread([this, params]() {
            std::vector<QTalk::StUserCard> arUserInfo;
            _pUserManager->getUserCard(params, arUserInfo);

            downloadUserHeadByStUserCard(arUserInfo);
            if (_pMsgManager) {
                _pMsgManager->sendDownloadHeadSuccess();
            }
        }).detach();
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/03
  */
void Communication::getGroupMemberById(const std::string &groupId) {

    // 获取群公告
    std::string groupTopic;
    LogicManager::instance()->getDatabase()->getGroupTopic(groupId, groupTopic);
    info_log("群公告 群Id:{0} 群公告:{1}", groupId, groupTopic);
    if (_pMsgManager && !groupTopic.empty()) {
        _pMsgManager->gotGroupTopic(groupId, groupTopic);
    }

    info_log("请求群成员 群Id:{0}", groupId);
    //
    // 获取本地群成员
//    std::vector<QTalk::StUserCard> arGroupMembers;
//    std::map<std::string, QUInt8> userRole;
//    LogicManager::instance()->GetDatabase()->getGroupMemberById(groupId, arGroupMembers, userRole);
//    if (!arGroupMembers.empty() && _pMsgManager) {
//        GroupMemberMessage e(*_pMsgManager);
//        e.groupId = groupId;
//        e.userRole = userRole;
//        e.members = arGroupMembers;
//        _pMsgManager->gotGroupMember(e);
//    }

    // 从网络获取数据
    LogicManager::instance()->getLogicBase()->getGroupMemberById(groupId);
}


/**
 * bind成功后处理
 * @param evt
 */
void Communication::dealBindMsg() {

    if(_pMsgManager)
        _pMsgManager->sendLoginProcessMessage("opening database");

    std::string path = Platform::instance().getAppdataRoamingUserPath();
    path += "/qtalk.db";
    _pFileHelper->creatDir(path);

    std::string errorMsg;
    if (!LogicManager::instance()->getDatabase()->OpenDB(path, errorMsg)) {
        throw std::runtime_error(errorMsg);
    }
    //将自己的id插入cache_data表 触发器使用
    LogicManager::instance()->getDatabase()->insertUserId(Platform::instance().getSelfXmppId());

    //设置群阅读指针时间
    std::string last_rmt = LogicManager::instance()->getDatabase()->getLoginBeforeGroupReadMarkTime();
    if (last_rmt == "0") {
        std::string current_rmt = LogicManager::instance()->getDatabase()->getGroupReadMarkTime();
        LogicManager::instance()->getDatabase()->saveLoginBeforeGroupReadMarkTime(current_rmt);
    }
    //
    updateTimeStamp();
    //
    if (_pMsgManager) {
        _pMsgManager->sendDataBaseOpen();
    }
}

/**
  * @函数名   inviteGroupMembers
  * @功能描述 邀请群成员
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
void Communication::inviteGroupMembers(std::vector<std::string> &users, const std::string &groupId) {
    LogicManager::instance()->getLogicBase()->inviteGroupMembers(users, groupId);
}

/**
  * @函数名   createGroup
  * @功能描述 创建群
  * @参数
  * @author   may
  * @date     2018/11/10
  */
void Communication::createGroup(const std::string &groupId, const std::string &groupName) {
    _mapGroupIdName[groupId] = groupName;
    LogicManager::instance()->getLogicBase()->createGroup(groupId);
}

/**
  * @函数名   getNetEmoticon
  * @功能描述 获取网络
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void Communication::getNetEmoticon(GetNetEmoticon &e) {

    std::ostringstream url;
    url << NavigationManager::instance().getFileHttpHost()
    << "/file/v1/emo/d/e/config?"
#ifdef _QCHAT
    << "p=qchat";
#else
    << "p=qtalk";
#endif // _QCHAT

    auto callback = [this, &e](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON *msgList = cJSON_Parse(responseData.c_str());

            if (msgList == nullptr) {
                error_log("json paring error");
                return;
            }

            ArStNetEmoticon netEm0os;
            cJSON* item = nullptr;
            cJSON_ArrayForEach(item, msgList) {
                std::shared_ptr<StNetEmoticon> emo(new StNetEmoticon);
                emo->pkgid = JSON::cJSON_SafeGetStringValue(item, "pkgid");
                emo->emoName = JSON::cJSON_SafeGetStringValue(item, "name");
                emo->emoFile = JSON::cJSON_SafeGetStringValue(item, "file");
                emo->desc = JSON::cJSON_SafeGetStringValue(item, "desc");
                emo->iconPath = JSON::cJSON_SafeGetStringValue(item, "thumb");
                emo->filesize = JSON::cJSON_SafeGetIntValue(item, "file_size");
                emo->md5 = JSON::cJSON_SafeGetStringValue(item, "md5");

                netEm0os.push_back(emo);
            }

            cJSON_Delete(msgList);
            e.arEmoInfo = netEm0os;
        } else {

        }
    };

    //
    QTalk::HttpRequest req(url.str());
    addHttpRequest(req, callback);

    if (_pFileHelper) {
        auto it = e.arEmoInfo.begin();
        for (; it != e.arEmoInfo.end(); it++) {
            std::shared_ptr<StNetEmoticon> emo = *it;
            emo->iconPath = _pFileHelper->downloadEmoticonIcon(emo->iconPath, emo->pkgid);
        }
    }
}

void Communication::getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure) {
    LogicManager::instance()->getDatabase()->getStructure(structure);
}

void Communication::onInviteGroupMembers(const std::string &groupId) {
    getGroupMemberById(groupId);
}

// 发送http 请求
void
Communication::addHttpRequest(const QTalk::HttpRequest &req, const std::function<void(int, const std::string &)>& callback) {

    try {

        if(Platform::instance().isMainThread())
            warn_log("main thread http request {0}", req.url);

        int currentThread = 0;
        if (!req.url.empty() && req.url.size() > 4 && req.url.substr(0, 4) == "http") {
            std::string url = req.url;
            std::size_t hash = std::hash<std::string>{}(url);
            currentThread = static_cast<int>(hash % _threadPoolCount);

            auto http = _httpPool[currentThread]->enqueue([this, req, callback, currentThread]() {
                info_log("在第{1}个http坑 开始请求: {0}", req.url, currentThread);

                perf_counter("addHttpRequest {0}", req.url);

                QTalk::QtHttpRequest request(req.url.c_str());
                //method
                request.setRequestMethod((RequestMethod)req.method);
                // header
                auto itr = req.header.begin();
                for (; itr != req.header.end(); itr++) {
                    info_log("请求header:{0} = {1}", itr->first, itr->second);
                    request.addRequestHeader(itr->first.c_str(), itr->second.c_str());
                }
                std::string requestHeaders = std::string("q_ckey=") + Platform::instance().getClientAuthKey();
                request.addRequestHeader("Cookie", requestHeaders.c_str());
                // body
                if ((RequestMethod)req.method == QTalk::RequestMethod::POST) {
                    info_log("请求body:{0}", req.body);
                    request.appendPostData(req.body.c_str(), req.body.length());
                }
                // form
                if (!req.formFile.empty()) {
                    request.addFromFile(req.formFile);
                }
                // process callback
                if (req.addProcessCallback) {
                    // 参数 总下载量 当前下载量 总上传量 当前上传量 速度 剩余时间
                    std::function<void(StProcessParam)> processCallback;
                    processCallback = [this, req](StProcessParam param) {
                        if (_pMsgManager) {
                            _pMsgManager->updateFileProcess(param.key, param.dt, param.dn,
                                                            param.ut, param.un, param.speed, param.leftTime);
                        }
                    };
                    request.setProcessCallback(req.processCallbackKey, processCallback);
                }

                // start
                request.startSynchronous();

                info_log("请求结果: code: {0}", request.getResponseCode());


                if ((RequestMethod)req.method == QTalk::RequestMethod::POST) {
                    info_log("请求结果: data: {0}", *request.getResponseData());
                }

                // callback
                callback(request.getResponseCode(), *request.getResponseData());

                if (request.getResponseCode() != 200) {
                    error_log("请求失败:{3} -> {0} \n params{2} \n {1}", req.url, *request.getResponseData(), req.body, request.getResponseCode());
                }
            });

            // 等待http请求结果
            http.get();
            //
            info_log("请求返回: {0}", req.url);
        } else {
            error_log("invalid url {0}", req.url);
            callback(-1, "");
        }
    }
    catch (const std::exception& e)
    {
        error_log("http exception, {0}", e.what());
    }
}

// 获取session and 个人配置
void Communication::getSessionData() {

    if (_pMsgManager) {
        // 最新个人配置
        updateUserConfigFromDb();
        // 好友列表
        std::vector<QTalk::Entity::IMFriendList> friends;
        LogicManager::instance()->getDatabase()->getAllFriends(friends);
        _pMsgManager->sendGotFriends(friends);
        // 群组列表
        std::vector<QTalk::Entity::ImGroupInfo> groups;
        LogicManager::instance()->getDatabase()->getAllGroup(groups);
        _pMsgManager->sendGotGroupList(groups);
    }
}

/**
 * 更新个人配置
 */
void Communication::updateUserConfigFromDb() {

    std::vector<QTalk::Entity::ImConfig> arConfig;
    LogicManager::instance()->getDatabase()->getAllConfig(arConfig);
    // maskNames
//    std::map<std::string, std::string> mapMaskNames;
//    for(const auto & config: arConfig)
//    {
//        if(config.ConfigKey == "kMarkupNames")
//            mapMaskNames[config.ConfigSubKey] = config.ConfigValue;
//    }
//    dbPlatForm::instance().setMaskNames(mapMaskNames);
    std::map<std::string, std::string> mapConf;
    LogicManager::instance()->getDatabase()->getConfig("kMarkupNames", mapConf);
    dbPlatForm::instance().setMaskNames(mapConf);
    //
    if (!arConfig.empty() && _pMsgManager) {
        _pMsgManager->updateUserConfigs(arConfig);
    }
}

//
void Communication::getFriendList() {
    LogicManager::instance()->getLogicBase()->getFriendList();
}

void Communication::onRecvFriendList(const std::vector<QTalk::Entity::IMFriendList> &friends) {
    LogicManager::instance()->getDatabase()->deleteAllFriends();
    if (!friends.empty()) {
        LogicManager::instance()->getDatabase()->bulkInsertFriends(friends);
    }
}

/**
 *
 * @param keyName
 * @param count
 */
void Communication::getStructureCount(const std::string &keyName, int &count) {
    LogicManager::instance()->getDatabase()->getStructureCount(keyName, count);
}

/**
 *
 * @param keyName
 * @param arMember
 */
void Communication::getStructureMember(const std::string &keyName, std::vector<std::string> &arMember) {
    LogicManager::instance()->getDatabase()->getStructureMember(keyName, arMember);
}

/**
 * 下载收藏表情
 * @param arDownloads
 */
void Communication::downloadCollection(const std::vector<std::string> &arDownloads) {
    auto it = arDownloads.cbegin();
    for (; it != arDownloads.cend(); it++) {
        _pFileHelper->getLocalImgFilePath(*it, "/emoticon/collection/");
    }
}

/**
 * 动态获取oa部分 ui组件
 */
bool Communication::geiOaUiData(std::vector<QTalk::StOAUIData> &arOAUIData) {
    bool ret = false;

    std::ostringstream url;
    url << NavigationManager::instance().getFoundConfigUrl();

    cJSON *gObj = cJSON_CreateObject();
    cJSON_AddStringToObject(gObj, "qtalk", Platform::instance().getSelfUserId().c_str());
    cJSON_AddStringToObject(gObj, "platform", "PC");
    cJSON_AddNumberToObject(gObj, "version", Platform::instance().getClientNumVerison());
    std::string postData = QTalk::JSON::cJSON_to_string(gObj);
    cJSON_Delete(gObj);

    //
    std::vector<std::string> arIcons;
    //
    auto callback = [&arOAUIData, &ret, &arIcons](int code, const std::string &responseData) {
        if (code == 200) {

            cJSON *retDta = cJSON_Parse(responseData.c_str());

            if (nullptr == retDta) {
                error_log("json 解析失败");
                return;
            }

            int errCode = JSON::cJSON_SafeGetIntValue(retDta, "errorCode");
            //
            if (errCode == 0) {
                cJSON *data = cJSON_GetObjectItem(retDta, "data");
                int groupSize = cJSON_GetArraySize(data);

                int i = 0;
                for (; i < groupSize; i++) {
                    QTalk::StOAUIData stOAData;
                    cJSON *groupItem = cJSON_GetArrayItem(data, i);
                    stOAData.groupId = JSON::cJSON_SafeGetIntValue(groupItem, "groupId");
                    stOAData.groupName = JSON::cJSON_SafeGetStringValue(groupItem, "groupName");
                    stOAData.groupIcon = JSON::cJSON_SafeGetStringValue(groupItem, "groupIcon");
                    //
                    arIcons.push_back(stOAData.groupIcon);
                    //
                    cJSON *memberItems = cJSON_GetObjectItem(groupItem, "members");
                    int memberSize = cJSON_GetArraySize(memberItems);
                    int j = 0;
                    for (; j < memberSize; j++) {
                        cJSON *memItem = cJSON_GetArrayItem(memberItems, j);
                        QTalk::StOAUIData::StMember member;
                        member.memberId = JSON::cJSON_SafeGetIntValue(memItem, "memberId");
                        member.memberName = JSON::cJSON_SafeGetStringValue(memItem, "memberName");
                        member.memberAction = JSON::cJSON_SafeGetStringValue(memItem, "memberAction");
                        member.memberIcon = JSON::cJSON_SafeGetStringValue(memItem, "memberIcon");
                        //
                        arIcons.push_back(member.memberIcon);
                        stOAData.members.push_back(member);
                    }

                    arOAUIData.push_back(stOAData);
                }

                ret = true;
            } else {
                if (cJSON_HasObjectItem(retDta, "errorMsg"))
                    error_log(cJSON_GetObjectItem(retDta, "errorMsg")->valuestring);
            }
            cJSON_Delete(retDta);
        } else {
            warn_log("获取oa数据 请求失败");
        }
    };

    QTalk::HttpRequest req(url.str(), QTalk::RequestMethod::POST);
    req.header["Content-Type"] = "application/json;";
    req.body = postData;
    addHttpRequest(req, callback);

    if (ret) {
        auto it = arIcons.cbegin();
        for (; it != arIcons.cend(); it++) {
            _pFileHelper->getLocalImgFilePath(*it, "/oaIcon/");
        }
    }

    return ret;
}

std::string Communication::getQchatQvt(const std::string &userName, const std::string &password) {
    std::string qvtStr;
    std::ostringstream url;
    url << NavigationManager::instance().getApiUrl()
        << "/get_power";

    std::string type = "username";
    regex regPhone("^1\\d{10}$");
    smatch smatchPhone;
    if(regex_match(userName,smatchPhone,regPhone)){
        type = "mobile";
    }
    regex regEmail("^[\\w-]+(\\.[\\w-]+)*@[\\w-]+(\\.[\\w-]+)+$");
    smatch smatchEmail;
    if(regex_match(userName,smatchEmail,regEmail)){
        type = "email";
    }
    std::string pwd = QTalk::utils::UrlEncode(password);
    std::string postData = "strid=" + userName + "&password=" + pwd + "&type=" + type + "&terminalType=01&osType=12&port=5222&loginsource=1";
    auto callback = [&qvtStr](int code, const std::string &responseData) {
        if (code == 200) {

            cJSON *retDta = cJSON_Parse(responseData.c_str());

            if (nullptr == retDta) {
                error_log("json 解析失败");
                cJSON_Delete(retDta);
                return;
            }

            int errCode = cJSON_GetObjectItem(retDta, "errcode")->valueint;
            if(errCode == 200){
                cJSON *array = cJSON_GetObjectItem(retDta,"data");
                cJSON *QVTJson = cJSON_GetArrayItem(array,0);
                std::string qcookie = cJSON_GetObjectItem(QVTJson,"qcookie")->valuestring;
                std::string vcookie = cJSON_GetObjectItem(QVTJson,"vcookie")->valuestring;
                std::string tcookie = cJSON_GetObjectItem(QVTJson,"tcookie")->valuestring;
                cJSON_Delete(retDta);

                cJSON* obj = cJSON_CreateObject();
                cJSON* data = cJSON_AddObjectToObject(obj,"data");
                cJSON_AddStringToObject(data, "qcookie", qcookie.c_str());
                cJSON_AddStringToObject(data, "vcookie", vcookie.c_str());
                cJSON_AddStringToObject(data, "tcookie", tcookie.c_str());
                qvtStr = QTalk::JSON::cJSON_to_string(obj);
                Platform::instance().setQvt(qvtStr);
                cJSON_Delete(obj);
            }
        }
    };


    QTalk::HttpRequest req(url.str(),QTalk::RequestMethod::POST);
    req.header["Content-Type"] = "application/x-www-form-urlencoded;";
    req.body = postData;
    addHttpRequest(req, callback);
    if(!qvtStr.empty())
        _pFileHelper->writeQvtToFile(qvtStr);
    return qvtStr;

}

void Communication::getQchatTokenByQVT(const std::string &qvt,std::map<std::string,std::string> &userMap) {
}

void Communication::getNewLoginToken(const std::string u, const std::string p,std::map<std::string,std::string> &map) {
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/nck/qtlogin.qunar";
    std::string plaint = LogicManager::instance()->getLogicBase()->normalRsaEncrypt(p);
    char uuid[36];
    utils::generateUUID(uuid);
    cJSON *gObj = cJSON_CreateObject();
    cJSON_AddStringToObject(gObj, "u", u.c_str());
    cJSON_AddStringToObject(gObj, "p", plaint.c_str());
    cJSON_AddStringToObject(gObj, "h", NavigationManager::instance().getDomain().c_str());
    cJSON_AddStringToObject(gObj, "mk", uuid);
    std::string postData = QTalk::JSON::cJSON_to_string(gObj);
    cJSON_Delete(gObj);

    auto callback = [&map](int code, const std::string &responseData) {
        if (code == 200) {

            cJSON *retDta = cJSON_Parse(responseData.c_str());
            if (nullptr == retDta) {
                error_log("json 解析失败");
                cJSON_Delete(retDta);
                return;
            }
            int errCode = cJSON_GetObjectItem(retDta, "errcode")->valueint;
            if(errCode == 0){
                cJSON *data = cJSON_GetObjectItem(retDta,"data");
                std::string u = cJSON_GetObjectItem(data,"u")->valuestring;
                std::string t = cJSON_GetObjectItem(data,"t")->valuestring;
                map["u"] = u;
                map["t"] = t;
            } else{
                error_log(cJSON_GetObjectItem(retDta, "errmsg")->valuestring);
            }
            cJSON_Delete(retDta);
        }
    };

    QTalk::HttpRequest req(url.str(), QTalk::RequestMethod::POST);
    req.header["Content-Type"] = "application/json;";
    req.body = postData;

    addHttpRequest(req, callback);
}

/**
 *
 * @param e
 */
void Communication::getGroupCardInfo(std::shared_ptr<QTalk::Entity::ImGroupInfo> &group) {
    if (_pUserGroupManager) {
        std::string groupId = group->GroupId;
        // 获取最新
        QTalk::Entity::JID jid(groupId.c_str());
        std::string domian = jid.domainname();
        MapGroupCard mapGroupCard;
        mapGroupCard[domian].push_back(*group);
        bool ret = _pUserGroupManager->getGroupCard(mapGroupCard);
        if (ret) {
            LogicManager::instance()->getDatabase()->getGroupCardById(group);

            if (!group->HeaderSrc.empty()) {
                std::string localHead = QTalk::GetHeadPathByUrl(group->HeaderSrc);
                if (!_pFileHelper->fileExist(localHead)) {
                    _pFileHelper->downloadFile(group->HeaderSrc, localHead, false);
                }
            }

            if (_pMsgManager) {
                std::shared_ptr<QTalk::StGroupInfo> info(new QTalk::StGroupInfo);
                info->groupId = groupId;
                info->name = group->Name;
                info->headSrc = group->HeaderSrc;
                info->title = group->Topic;
                info->desc = group->Introduce;

                _pMsgManager->onUpdateGroupInfo(info);
            }
        }
    }
}

// 设置群管理员
void Communication::setGroupAdmin(const std::string& groupId, const std::string& nickName,
        const std::string &memberJid, bool isAdmin) {

    LogicManager::instance()->getLogicBase()->setGroupAdmin(groupId, nickName, memberJid, isAdmin);
}

void Communication::removeGroupMember(const std::string& groupId,
                                      const std::string& nickName,
                                      const std::string& memberJid) {

    LogicManager::instance()->getLogicBase()->removeGroupMember(groupId, nickName, memberJid);
}

/**
 * 退出群
 * @param groupId
 */
void Communication::quitGroup(const std::string &groupId) {
    //
    LogicManager::instance()->getLogicBase()->quitGroup(groupId);
    // 移除置顶
    _pUserConfig->updateUserSetting(UserSettingMsg::EM_OPERATOR_CANCEL,
                                    "kStickJidDic",
                                    QTalk::Entity::UID(groupId).toStdString(),
                                    "{\"topType\":0,\"chatType\":1}");
}

/**
 * 销毁群处理
 * @param groupId
 */
void Communication::destroyGroup(const std::string &groupId) {
    LogicManager::instance()->getLogicBase()->destroyGroup(groupId);
}

void Communication::reportDump(const std::string& dumpPath)
{
	auto fun = [this, dumpPath](const std::string &url, const std::string &) {
		if (!url.empty()) {

			{
				// 发消息
				std::ostringstream msgCont;
				msgCont << "qtalk pc2.0 崩溃信息 \n\n"
					<< "登录用户: " << Platform::instance().getSelfXmppId()
					<< " " << Platform::instance().getSelfName() << "\n"
					<< "版本号: " << Platform::instance().getClientVersion() << "\n"
					<< "使用平台: " << Platform::instance().getPlatformStr() << "\n"
					<< "OS 信息: " << Platform::instance().getOSInfo() << "\n"
					<< "dump文件: " << url;
				//
				std::string error;
				bool isSuccess = LogicManager::instance()->getLogicBase()->sendReportMessage(msgCont.str(), error);
			}
		};
	};

	if (_pFileHelper)
		_pFileHelper->uploadLogFile(dumpPath, fun);
}

/**
 *
 * @param logPath
 */
void Communication::reportLog(const std::string &desc, const std::string &logPath) {
    if (_pFileHelper) {
        auto fun = [this, desc, logPath](const std::string &url, const std::string &) {
            // 发送消息
            if (!url.empty()) {

                {
                    // 发消息
                    std::ostringstream msgCont;
                    msgCont << "qtalk pc2.0 日志反馈 \n\n"
                            << "登录用户: " << Platform::instance().getSelfXmppId()
                            << " " << Platform::instance().getSelfName() <<"\n"
                            << "版本号: " << Platform::instance().getClientVersion() << "\n"
                            << "编译时间: " << Platform::instance().get_build_date_time() << "\n"
                            << "使用平台: " << Platform::instance().getPlatformStr() << "\n"
                            << "OS 信息: " << Platform::instance().getOSInfo() << "\n"
                            << "描述信息: " << desc << " \n"
                            << "日志文件: " << url;
                    //
                    std::string error;
                    bool isSuccess = LogicManager::instance()->getLogicBase()->sendReportMessage(msgCont.str(), error);
#if !defined(_STARTALK) and !defined(_QCHAT)
                    info_log("send message :{0}", isSuccess);
#endif
                }
                {
                    // 发邮件
                    std::string strSub = string("QTalk 2.0 反馈日志 ") + Platform::instance().getSelfXmppId();
                    std::ostringstream strBody;
                    bool isHtml = true;
                    strBody << "登录用户: " << Platform::instance().getSelfXmppId()
                            << " " << Platform::instance().getSelfName() <<"<br/>"
                            << "版本号: " << Platform::instance().getClientVersion() << "<br/>"
                            << "使用平台: " << Platform::instance().getPlatformStr() << "<br/>"
                            << "OS 信息: " << Platform::instance().getOSInfo() << "<br/>"
                            << "描述信息: " << desc << " <br/>"
                            << "日志文件: " << url;
                    std::string error;
                    bool isSuccess = LogicManager::instance()->getLogicBase()->sendReportMail(strSub, strBody.str(), true, error);
                    bool isNotify = (desc != "@@#@@");
                    if (_pMsgManager && isNotify) {
                        _pMsgManager->sendLogReportRet(isSuccess, isSuccess ? "日志反馈成功" : error);
                    }
                }
//#endif
            }
        };
        if(_pFileHelper)
            _pFileHelper->uploadLogFile(logPath, fun);
    }
}

/**
 * 
 */
void Communication::saveUserConfig() {
    if (_pUserConfig) {
        std::string val = AppSetting::instance().saveAppSetting();
        //_pUserConfig->updateUserSettting(UserSettingMsg::EM_OPERATOR_SET, QTALK_2_0_CONFIG, Platform::instance().getPlatformStr(), val);
        LogicManager::instance()->getDatabase()->insertConfig(QTALK_2_0_CONFIG,
                                                              Platform::instance().getPlatformStr(), val);
    }
}

void Communication::clearSystemCache() {
    // 清理数据库
    LogicManager::instance()->getDatabase()->ClearDBData();
    // 清理配置文件
    if (_pFileHelper) {
        _pFileHelper->clearConfig();
    }
}

void Communication::removeSession(const string &peerId) {
    std::vector<std::string> peers;
    peers.push_back(peerId);
    LogicManager::instance()->getDatabase()->bulkDeleteSessions(peers);
}

/**
 *
 * @param head
 */
void Communication::changeUserHead(const string &head) {
    std::thread([this, head]() {
#ifdef _MACOS
        pthread_setname_np("communication changeUserHead thread");
#endif
        if (_pFileHelper && _pUserManager) {
            std::string headUrl = _pFileHelper->getNetImgFilePath(head);
			if (headUrl.empty()) //todo return error message
				return;

            bool ret = _pUserManager->changeUserHead(headUrl);
            std::string localHead;
            if (ret) {
                localHead = _pFileHelper->getLocalImgFilePath(headUrl, "/image/headphoto/");
            }
            if (_pMsgManager) {
                _pMsgManager->changeHeadRetMessage(ret, localHead);
            }
        }

    }).detach();
}

/**
 *
 * @param loginTime
 * @param logoutTime
 */
void Communication::sendUserOnlineState(const QInt64 &loginTime, const QInt64 &logoutTime, const std::string &ip) {
    std::thread([this, loginTime, logoutTime, ip]() {
#ifdef _MACOS
        pthread_setname_np("communication sendUserOnlineState thread");
#endif
        std::ostringstream url;
        url << NavigationManager::instance().getHttpHost()
            << "/statics/pc_statics.qunar"
            << "?v=" << Platform::instance().getClientVersion()
            << "&p=" << Platform::instance().getPlatformStr()
            << "&u=" << Platform::instance().getSelfUserId()
            << "&k=" << Platform::instance().getServerAuthKey()
            << "&d=" << Platform::instance().getSelfDomain();

        cJSON *obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "username", Platform::instance().getSelfUserId().data());
        cJSON_AddStringToObject(obj, "host", Platform::instance().getSelfDomain().data());
        cJSON_AddStringToObject(obj, "resource", Platform::instance().getSelfResource().data());
        cJSON_AddStringToObject(obj, "platform", Platform::instance().getPlatformStr().data());
        cJSON_AddStringToObject(obj, "login_time", std::to_string(loginTime).data());
        cJSON_AddStringToObject(obj, "logout_at", std::to_string(logoutTime).data());
        cJSON_AddStringToObject(obj, "ip", ip.data());
        std::string postData = QTalk::JSON::cJSON_to_string(obj);
        cJSON_Delete(obj);

        auto callback = [](int code, std::string responsData) {

            if (code == 200) {
                info_log("sendUserOnlineState success {0}", responsData);
            } else {
                error_log("sendUserOnlineState error {0}", responsData);
            }
        };

        HttpRequest req(url.str(), RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        addHttpRequest(req, callback);
    }).detach();
}

/**
 *
 * @param ip
 * @param desc
 * @param reportTime
 */
void Communication::sendOperatorStatistics(const std::string &ip, const std::vector<QTalk::StActLog> &operators) {
    std::thread([this, operators, ip]() {
#ifdef _MACOS
        pthread_setname_np("communication sendOperatorStatistics thread");
#endif
        std::string uploadLog = NavigationManager::instance().getUploadLog();
        std::vector<QTalk::StActLog> logs(operators);
        do {
            cJSON *obj = cJSON_CreateObject();
            // user
            cJSON *user = cJSON_CreateObject();
            cJSON_AddStringToObject(user, "uid", Platform::instance().getSelfUserId().data());
            cJSON_AddStringToObject(user, "domain", Platform::instance().getSelfDomain().data());
            cJSON_AddStringToObject(user, "nav", Platform::instance().getLoginNav().data());
            cJSON_addJsonObject(obj, "user", user);
            // device
            cJSON *device = cJSON_CreateObject();
#if defined(_ATALK)
            cJSON_AddStringToObject(device, "plat", "atalk");
#elif defined(_STARTALK)
            cJSON_AddStringToObject(device, "plat", "starTalk");
#else
            cJSON_AddStringToObject(device, "plat", "qtalk");
#endif
            cJSON_AddStringToObject(device, "os", Platform::instance().getPlatformStr().data());
            cJSON_AddNumberToObject(device, "versionCode", Platform::instance().getClientNumVerison());
            cJSON_AddStringToObject(device, "versionName", Platform::instance().getGlobalVersion().data());

            cJSON_AddStringToObject(device, "osModel", Platform::instance().getOSInfo().data());
            cJSON_AddStringToObject(device, "osBrand", Platform::instance().getOSProductType().data());
            cJSON_AddStringToObject(device, "osVersion", Platform::instance().getOSVersion().data());
            cJSON_addJsonObject(obj, "device", device);
            cJSON *infos = cJSON_CreateArray();

            int i = 0;
            while (i++ <= 100 && !logs.empty()) {
                const auto &log = logs.back();
                cJSON *info = cJSON_CreateObject();
                cJSON_AddNumberToObject(info, "costTime", 0);
                cJSON_AddStringToObject(info, "describtion", log.desc.data());
                cJSON_AddBoolToObject(info, "isMainThread", true);
                cJSON_AddStringToObject(info, "reportTime", std::to_string(log.operatorTime).data());
                cJSON_AddStringToObject(info, "sql", "[]");
                cJSON_AddStringToObject(info, "subType", "click");
                cJSON_AddNumberToObject(info, "threadId", 1);
                cJSON_AddStringToObject(info, "threadName", "main");
                cJSON_AddStringToObject(info, "type", "ACT");

                logs.pop_back();

                cJSON_AddItemToArray(infos, info);
            }
            cJSON_addJsonObject(obj, "infos", infos);

            std::string postData = QTalk::JSON::cJSON_to_string(obj);
            cJSON_Delete(obj);

            auto callback = [](int code, std::string responsData) {

                if (code == 200) {
                    info_log("sendOperatorStatistics success {0}", responsData);
                } else {
                    error_log("sendOperatorStatistics error {0}", responsData);
                }
            };

            HttpRequest req(uploadLog, RequestMethod::POST);
            req.header["Content-Type"] = "application/json;";
            req.body = postData;
            addHttpRequest(req, callback);

        } while (!logs.empty());

    }).detach();
}

/**
 *
 * @param info
 */
void Communication::getUserCard(std::shared_ptr<QTalk::Entity::ImUserInfo> &info) {
    if (info && _pUserManager) {
        QTalk::Entity::JID jid(info->XmppId.data());
        UserCardParam params;
        params[jid.domainname()][jid.username()] = 0;

        std::vector<QTalk::StUserCard> userCards;
        _pUserManager->getUserCard(params, userCards);

        if (userCards.size() == 1) {
            auto card = userCards[0];
            info->Name = card.userName;
            info->HeaderSrc = card.headerSrc;
            info->NickName = card.nickName;
            //
            LogicManager::instance()->getDatabase()->setUserCardInfo(userCards);
        }
    }
}

/**
 * 最近session
 * @param sessions
 */
void Communication::getRecntSession(std::vector<QTalk::StShareSession> &sessions)
{
    try {
        LogicManager::instance()->getDatabase()->getRecentSession(sessions);
    }
    catch (const std::exception& e)
    {
        error_log(e.what());
    }

}

/**
 *
 * @param sessions
 */
void Communication::geContactsSession(std::vector<QTalk::StShareSession> &sessions)
{
    try {
        LogicManager::instance()->getDatabase()->geContactsSession(sessions);
    }
    catch (const std::exception& e)
    {
        error_log(e.what());
    }
}

/**
 *
 */
void Communication::updateTimeStamp()
{
    try {
        QInt64 timeStamp = 0;
        std::string configTimeStamp;
        //
        LogicManager::instance()->getDatabase()->getConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_TWOPERSONCHAT, configTimeStamp);
        if(configTimeStamp.empty())
            timeStamp = 0;
        else
            timeStamp = std::stoll(configTimeStamp);

        if(timeStamp == 0)
        {
            timeStamp = LogicManager::instance()->getDatabase()->getMaxTimeStampByChatType(QTalk::Enum::TwoPersonChat);
            LogicManager::instance()->getDatabase()->insertConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_TWOPERSONCHAT,
                                                                  std::to_string(timeStamp));
            warn_log("{0} insert new timestamp {1} str:{2}", DEM_TWOPERSONCHAT, timeStamp, configTimeStamp);
        }
        else
            warn_log("{0} has old timestamp {1} str:{2}", DEM_TWOPERSONCHAT, timeStamp, configTimeStamp);

        //
        configTimeStamp = "";
        LogicManager::instance()->getDatabase()->getConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_GROUPCHAT, configTimeStamp);
        if(configTimeStamp.empty())
            timeStamp = 0;
        else
            timeStamp = std::stoll(configTimeStamp);

        if(timeStamp == 0)
        {
            timeStamp = LogicManager::instance()->getDatabase()->getMaxTimeStampByChatType(QTalk::Enum::GroupChat);
            LogicManager::instance()->getDatabase()->insertConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_GROUPCHAT,
                                                                  std::to_string(timeStamp));
            warn_log("{0} insert new timestamp {1} str:{2}", DEM_GROUPCHAT, timeStamp, configTimeStamp);
        }
        else
            warn_log("{0} has old timestamp {1} str:{2}", DEM_GROUPCHAT, timeStamp, configTimeStamp);

        //
        configTimeStamp = "";
        LogicManager::instance()->getDatabase()->getConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_SYSTEM, configTimeStamp);
        if(configTimeStamp.empty())
            timeStamp = 0;
        else
            timeStamp = std::stoll(configTimeStamp);

        if(timeStamp == 0)
        {
            timeStamp = LogicManager::instance()->getDatabase()->getMaxTimeStampByChatType(QTalk::Enum::System);
            LogicManager::instance()->getDatabase()->insertConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_SYSTEM,
                                                                  std::to_string(timeStamp));
            warn_log("{0} insert new timestamp {1} str:{2}", DEM_SYSTEM, timeStamp, configTimeStamp);
        }
        else
            warn_log("{0} has old timestamp {1} str:{2}", DEM_SYSTEM, timeStamp, configTimeStamp);

    }
    catch (const std::exception &e) {
        error_log("exception {0}", e.what());
    }
}

/**
 *
 * @param status
 */


void Communication::setServiceSeat(const int sid, const int seat) {
    if(_pHotLinesConfig){
        _pHotLinesConfig->setServiceSeat(sid,seat);
    }
}

void Communication::serverCloseSession(const std::string& username, const std::string &seatname,
                                       const std::string& virtualname) {
    if(_pHotLinesConfig){
        _pHotLinesConfig->serverCloseSession(username, seatname, virtualname);
    }
}

void Communication::getSeatList(const QTalk::Entity::UID uid) {
    if(_pHotLinesConfig){
        _pHotLinesConfig->getTransferSeatsList(uid);
    }
}

void Communication::sendProduct(const std::string username, const std::string virtualname,const std::string product,const std::string type) {
    if(_pHotLinesConfig){
        _pHotLinesConfig->sendProduct(username,virtualname,product,type);
    }
}

void Communication::sessionTransfer(const QTalk::Entity::UID uid, const std::string newCser,
                                    const std::string reason) {
    if(_pHotLinesConfig){
        _pHotLinesConfig->transferCsr(uid,newCser,reason);
    }
}

void Communication::sendWechat(const QTalk::Entity::UID uid) {
    if(_pHotLinesConfig){
        _pHotLinesConfig->sendWechat(uid);
    }
}

/**
 *
 * @param groupId
 * @param memberId
 * @param affiliation
 */
void Communication::onUserJoinGroup(const std::string &groupId, const std::string &memberId, int affiliation) {
    std::shared_ptr<QTalk::StGroupMember> member(new QTalk::StGroupMember);
    member->groupId = groupId;
    //
    std::string jid = memberId;
    // 自己被拉入群 拉取群资料
    if (jid == Platform::instance().getSelfXmppId()) {
        MapGroupCard mapGroups;
        _pUserGroupManager->getUserGroupInfo(mapGroups);
        _pUserGroupManager->getGroupCard(mapGroups);
        if (_pMsgManager) {
            std::shared_ptr<QTalk::Entity::ImGroupInfo> group(new QTalk::Entity::ImGroupInfo);
            group->GroupId = groupId;
            if (LogicManager::instance()->getDatabase()->getGroupCardById(group)) {
                std::shared_ptr<QTalk::StGroupInfo> info(new QTalk::StGroupInfo);
                info->groupId = groupId;
                info->name = group->Name;
                info->headSrc = group->HeaderSrc;
                info->title = group->Topic;
                info->desc = group->Introduce;
                _pMsgManager->onUpdateGroupInfo(info);
            }
        }
    }

    member->memberJid = jid;
    std::map<std::string, QUInt8> members;
    members[jid] = static_cast<unsigned char>(affiliation);
    LogicManager::instance()->getDatabase()->bulkInsertGroupMember(groupId, members);
    std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo = LogicManager::instance()->getDatabase()->getUserInfoByXmppId(
            jid);
    if (userInfo && !userInfo->Name.empty()) {
        member->nick = userInfo->Name;
    }
    member->domain = QTalk::Entity::JID(memberId.data()).domainname();
    member->affiliation = affiliation;
    _pMsgManager->onGroupJoinMember(member);

}


void Communication::onStaffChanged()
{
    if (_pUserManager) {
        _pUserManager->getNewStructure(true);
    }
}

//
std::string Communication::checkUpdater(int ver) {
    return "";
}

//
void Communication::getMedalList()
{
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/medal/medalList.qunar";
    std::string strUrl = url.str();
    //
    std::string strVer;
    LogicManager::instance()->getDatabase()->getConfig("MEDAL_LIST", "VERSION", strVer);
    auto version = atoi(strVer.data());
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "version", version);
    std::string postData = cJSON_to_string(obj);
    cJSON_Delete(obj);
    //
    std::vector<QTalk::Entity::ImMedalList> medalList;
    std::set<std::string> imgs;
    int newVersion = 0;
    auto call_back = [this, &medalList, &newVersion, &imgs](int code, const std::string& resData){

        if(resData.empty())
            return;
        cJSON* json = cJSON_Parse(resData.data());
        if(nullptr == json)
        {
            error_log("json Parse error {0}", resData);
            return;
        }
        if(code == 200) {
            //
            cJSON_bool ret = JSON::cJSON_SafeGetBoolValue(json, "ret");
            if(ret)
            {
                cJSON* data = cJSON_GetObjectItem(json, "data");
                newVersion = JSON::cJSON_SafeGetIntValue(data, "version");
                //
                cJSON* list = cJSON_GetObjectItem(data, "medalList");
                cJSON* temp = nullptr;
                cJSON_ArrayForEach(temp, list){
                    QTalk::Entity::ImMedalList medal;
                    medal.medalId = JSON::cJSON_SafeGetIntValue(temp, "id");
                    medal.medalName = JSON::cJSON_SafeGetStringValue(temp, "medalName");
                    medal.obtainCondition = JSON::cJSON_SafeGetStringValue(temp, "obtainCondition");
                    medal.status = JSON::cJSON_SafeGetIntValue(temp, "status");

                    cJSON* icon = cJSON_GetObjectItem(temp, "icon");
                    medal.smallIcon = JSON::cJSON_SafeGetStringValue(icon, "small");
                    medal.bigLightIcon = JSON::cJSON_SafeGetStringValue(icon, "bigLight");
//                    medal.bigGrayIcon = JSON::cJSON_SafeGetStringValue(icon, "bigGray");
                    medal.bigLockIcon = JSON::cJSON_SafeGetStringValue(icon, "bigLock");

                    medalList.push_back(medal);

                    imgs.insert(medal.smallIcon);
                    imgs.insert(medal.bigLightIcon);
                    imgs.insert(medal.bigLockIcon);
                };
            }
            else
            {
                std::string errorMsg = JSON::cJSON_SafeGetStringValue(json, "errmsg");
                error_log("getMedalList error {0}", errorMsg);
            }
        }
        else
        {

        }
        cJSON_Delete(json);
    };

    QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
    req.header["Content-Type"] = "application/json;";
    req.body = postData;
    addHttpRequest(req, call_back);

    if(newVersion >= version && !medalList.empty())
    {
        try {
            //
            LogicManager::instance()->getDatabase()->insertMedalList(medalList);
            //
            LogicManager::instance()->getDatabase()->insertConfig("MEDAL_LIST", "VERSION", std::to_string(newVersion));
        }
        catch (const std::exception& e)
        {
            error_log("update medal list error {0}", e.what());
        }
    }
    //
    std::vector<QTalk::Entity::ImMedalList> medals;
    LogicManager::instance()->getDatabase()->getMedalList(medals);
    if(!medals.empty())
        dbPlatForm::instance().setMedals(medals);

    if(!imgs.empty() && _pFileHelper)
    {
        for(const auto& img : imgs)
            _pFileHelper->getLocalImgFilePath(img, "/image/medal/", false);
    }
}

void Communication::getUserMedal(bool presence) {
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/medal/userMedalList.qunar";
    std::string strUrl = url.str();
    //
    std::string strVer;
    LogicManager::instance()->getDatabase()->getConfig("USER_MEDAL", "VERSION", strVer);
    auto version = atoi(strVer.data());
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "version", version);
//    cJSON_AddStringToObject(obj, "userId", Platform::instance().getSelfUserId().data());
//    cJSON_AddStringToObject(obj, "host", Platform::instance().getSelfDomain().data());
    std::string postData = cJSON_to_string(obj);
    cJSON_Delete(obj);
    //
    std::vector<QTalk::Entity::ImUserStatusMedal> userMedals;
    int newVersion = 0;
    auto call_back = [this, &userMedals, &newVersion](int code, const std::string& resData){

        if(resData.empty())
            return;
        cJSON* json = cJSON_Parse(resData.data());
        if(nullptr == json)
        {
            error_log("json Parse error {0}", resData);
            return;
        }
        if(code == 200) {
            //
            cJSON_bool ret = JSON::cJSON_SafeGetBoolValue(json, "ret");
            if(ret)
            {
                cJSON* data = cJSON_GetObjectItem(json, "data");
                newVersion = JSON::cJSON_SafeGetIntValue(data, "version");
                //
                cJSON* list = cJSON_GetObjectItem(data, "userMedals");
                cJSON* temp = nullptr;
                cJSON_ArrayForEach(temp, list){
                    QTalk::Entity::ImUserStatusMedal medal;
                    medal.medalId = JSON::cJSON_SafeGetIntValue(temp, "medalId");
                    medal.userId = JSON::cJSON_SafeGetStringValue(temp, "userId");
                    medal.host = JSON::cJSON_SafeGetStringValue(temp, "host");
                    medal.medalStatus = JSON::cJSON_SafeGetIntValue(temp, "medalStatus");
                    medal.mappingVersion = JSON::cJSON_SafeGetIntValue(temp, "mappingVersion");
                    medal.updateTime = JSON::cJSON_SafeGetLonglongValue(temp, "updateTime");
                    userMedals.push_back(medal);
                };
            }
            else
            {
                std::string errorMsg = JSON::cJSON_SafeGetStringValue(json, "errmsg");
                error_log("getMedalList error {0}", errorMsg);
            }
        }
        else
        {

        }
        cJSON_Delete(json);
    };

    QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
    req.header["Content-Type"] = "application/json;";
    req.body = postData;
    addHttpRequest(req, call_back);

    if(newVersion > version && !userMedals.empty())
    {
        try {
            //
            LogicManager::instance()->getDatabase()->insertMedals(userMedals);
            //
            LogicManager::instance()->getDatabase()->insertConfig("USER_MEDAL", "VERSION", std::to_string(newVersion));

            if(presence && _pMsgManager)
            {
                _pMsgManager->onUserMadelChanged(userMedals);
            }
        }
        catch (const std::exception& e)
        {
            error_log("update medal list error {0}", e.what());
        }
    }
}

void Communication::getMedalUser(int medalId, std::vector<QTalk::StMedalUser> &metalUsers) {
    try {
        LogicManager::instance()->getDatabase()->getMedalUsers(medalId, metalUsers);
    }
    catch (const std::exception& e)
    {
        error_log("getMedalUser error", e.what());
    }
}

bool Communication::modifyUserMedalStatus(int medalId, bool wear) {
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/medal/userMedalStatusModify.qunar";
    std::string strUrl = url.str();
    //
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "medalStatus", wear ? 3 : 1);
    cJSON_AddStringToObject(obj, "userId", Platform::instance().getSelfUserId().data());
    cJSON_AddStringToObject(obj, "host", Platform::instance().getSelfDomain().data());
    cJSON_AddNumberToObject(obj, "medalId", medalId);
    std::string postData = cJSON_to_string(obj);
    cJSON_Delete(obj);
    //
    bool ret = false;
    auto call_back = [this, &ret](int code, const std::string& resData){

        if(resData.empty())
            return;
        cJSON* json = cJSON_Parse(resData.data());
        if(nullptr == json)
        {
            error_log("json Parse error {0}", resData);
            return;
        }
        if(code == 200) {
            //
            ret = JSON::cJSON_SafeGetBoolValue(json, "ret");
            if(!ret)
            {
                std::string errorMsg = JSON::cJSON_SafeGetStringValue(json, "errmsg");
                error_log("userMedalStatusModify error {0}", errorMsg);
            }
        }
        else
        {

        }
        cJSON_Delete(json);
    };

    QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
    req.header["Content-Type"] = "application/json;";
    req.body = postData;
    addHttpRequest(req, call_back);

    if(ret)
        LogicManager::instance()->getDatabase()->modifyUserMedalStatus(Platform::instance().getSelfXmppId(), medalId, wear ? 3 : 1);
    return ret;
}
