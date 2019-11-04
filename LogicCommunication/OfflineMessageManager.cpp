#include "OfflineMessageManager.h"
#include "../Platform/Platform.h"
#include "../Platform/NavigationManager.h"
#include "../entity/im_message.h"
#include "../entity/IM_Session.h"
#include "../LogicManager/LogicManager.h"
#include "../interface/logic/IDatabasePlug.h"
#include "Communication.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

vector<string> split(const string &strtem, char a) {
    vector<string> strvec;
    string::size_type pos1, pos2;
    pos2 = strtem.find(a);
    pos1 = 0;
    while (string::npos != pos2) {
        strvec.push_back(strtem.substr(pos1, pos2 - pos1));
        pos1 = pos2 + 1;
        pos2 = strtem.find(a, pos1);
    }
    strvec.push_back(strtem.substr(pos1));
    return strvec;
}

OfflineMessageManager::OfflineMessageManager(Communication *comm)
        : _pComm(comm) {
}

bool OfflineMessageManager::updateChatOfflineMessage() {
    int pageCount = 500;
    int retryCount = 3;
    bool complete = false;
    std::string errMsg;
    QInt64 timeStamp = getTimeStamp(QTalk::Enum::TwoPersonChat);

    int index = 0;

    while (true)
    {
        std::vector<QTalk::Entity::ImMessageInfo> msgList;
        std::vector<QTalk::Entity::ImSessionInfo> sessionList;
        getOfflineChatMessageJson(timeStamp, pageCount, complete, errMsg, msgList, sessionList);

        if (complete) {
            if (!msgList.empty()) {
                LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);

                QTalk::Entity::ImMessageInfo lastMsgInfo = msgList.back();
                timeStamp = lastMsgInfo.LastUpdateTime;
                retryCount = 3;
            } else {
                // 获取消息完成
                break;
            }
        } else {
            // 获取消息失败重试
            if (retryCount < 0) {
                error_log("拉取历史消息失败");
                break;
            } else {
                retryCount--;
                warn_log("拉取历史消息失败 开始倒数第{0}次重试", retryCount);
            }
        }

        if(_pComm && _pComm->_pMsgManager)
        {
            std::string msg = SFormat("getting user message: {0}", ++index);
            _pComm->_pMsgManager->sendLoginProcessMessage(msg);
        }
    }

    // 成功才更新时间戳
    return retryCount >= 0;
}

void OfflineMessageManager::updateChatMasks()
{

    using namespace QTalk;

    QInt64 timeStamp = getTimeStamp(QTalk::Enum::TwoPersonChat);
    //
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/qtapi/getreadflag.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string strUrl = url.str();


    cJSON *jsonObject = cJSON_CreateObject();
    cJSON_AddNumberToObject(jsonObject, "time", timeStamp);
    cJSON_AddStringToObject(jsonObject, "domain", Platform::instance().getSelfDomain().data());
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    
//            {
//                "ret": true,
//                        "errcode": 0,
//                        "errmsg": null,
//                        "data": [
//                {
//                    "readflag": 1,
//                            "msgid": "qtalk-corp-service-63b13c45-bd54-4e94-a348-a4964a83eeb2"
//                }
//                ]
//            }
    bool _ret = false;
    std::map<std::string, int> readFlags;
    auto callBack = [this, &_ret, &readFlags](int code, const std::string &responseData) {

        warn_log("{0}  {1}", code, responseData);
        if (code == 200) {
            cJSON *resData = cJSON_Parse(responseData.c_str());

            if (resData == nullptr) {
                error_log("json paring error"); return;
            }
            //
            cJSON_bool ret = QTalk::JSON::cJSON_SafeGetBoolValue(resData, "ret");
            if(ret)
            {
                cJSON* data = cJSON_GetObjectItem(resData, "data");

                //
                cJSON* tempMsg = nullptr;
                cJSON_ArrayForEach(tempMsg, data){
                    if(tempMsg)
                    {
                        auto flag = JSON::cJSON_SafeGetIntValue(tempMsg, "readflag");
                        std::string msgId = JSON::cJSON_SafeGetStringValue(tempMsg, "msgid");
                        readFlags[msgId] = flag;
                    }
                }
            }
            //
            _ret = true;
            cJSON_Delete(resData);
        }
        else
        {

        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", strUrl, postData);
        _pComm->addHttpRequest(req, callBack);
        //
        if(_ret) {
            // 更新阅读状态
            LogicManager::instance()->getDatabase()->updateMessageReadFlags(readFlags);
            warn_log("-- got user message and read flag -> update timestamp");
            // 更新时间戳
            LogicManager::instance()->getDatabase()->insertConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_TWOPERSONCHAT,
                                                                  std::to_string(0));
        }
    }
}


bool OfflineMessageManager::updateGroupOfflineMessage() {

    int pageCount = 500;
    int retryCount = 3;
    bool complete = false;
    std::string errMsg;
    QInt64 timeStamp = getTimeStamp(QTalk::Enum::GroupChat);

    int index = 0;

    while (true) {
        std::vector<QTalk::Entity::ImMessageInfo> msgList;
        std::vector<QTalk::Entity::ImSessionInfo> sessionList;
        getOfflineGroupMessageJson(timeStamp, pageCount, complete, errMsg, msgList, sessionList);
        if (complete) {
            if (!msgList.empty()) {
                LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);
                QTalk::Entity::ImMessageInfo lastMsgInfo = msgList.back();
                timeStamp = lastMsgInfo.LastUpdateTime;
                retryCount = 3;
            } else {
                // 获取消息完成
                break;
            }
        } else {
            // 获取消息失败重试
            if (retryCount < 0) {
                error_log("拉取历史消息失败");
                break;
            } else {
                retryCount--;
                warn_log("拉取历史消息失败 开始倒数第{0}次重试", retryCount);
            }
        }

        if(_pComm && _pComm->_pMsgManager)
        {
            std::string msg = SFormat("getting group message: {0}", ++index);
            _pComm->_pMsgManager->sendLoginProcessMessage(msg);
        }
    }
    // 成功才更新时间戳
    if(retryCount >= 0) {
        warn_log("-- got group message -> update timestamp");
        LogicManager::instance()->getDatabase()->insertConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_GROUPCHAT,
                                                              std::to_string(0));
    }

    return retryCount >= 0;
}

bool OfflineMessageManager::updateNoticeOfflineMessage() {

    int pageCount = 500;
    int retryCount = 3;
    bool complete = false;
    std::string errMsg;
    QInt64 timeStamp = getTimeStamp(QTalk::Enum::System);

    int index = 0;
    while (true) {
        std::vector<QTalk::Entity::ImMessageInfo> msgList;
        std::vector<QTalk::Entity::ImSessionInfo> sessionList;
        getOfflineNoticeMessageJson(timeStamp, pageCount, complete, errMsg, msgList, sessionList);
        if (complete) {
            if (!msgList.empty()) {
                LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);
                //LogicManager::instance()->GetDatabase()->bulkInsertSessionInfo(sessionList);
                QTalk::Entity::ImMessageInfo lastMsgInfo = msgList.back();
                timeStamp = lastMsgInfo.LastUpdateTime;
                retryCount = 3;
            } else {
                // 获取消息完成
                break;
            }
        } else {
            // 获取消息失败重试
            if (retryCount < 0) {
                error_log("拉取历史消息失败");
                break;
            } else {
                retryCount--;
                warn_log("拉取历史消息失败 开始倒数第{0}次重试", retryCount);
            }
        }

        if(_pComm && _pComm->_pMsgManager)
        {
            std::string msg = SFormat("getting system message: {0}", ++index);
            _pComm->_pMsgManager->sendLoginProcessMessage(msg);
        }
    }
    // 成功才更新时间戳
    if(retryCount >= 0) {
        warn_log("-- got system message -> update timestamp");
        LogicManager::instance()->getDatabase()->insertConfig(DEM_MESSAGE_MAXTIMESTAMP, DEM_SYSTEM, std::to_string(0));
    }
    return retryCount >= 0;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/26
  */
QInt64 OfflineMessageManager::getTimeStamp(QTalk::Enum::ChatType chatType) {
    QInt64 timeStamp = 0;

    std::string subKey;
    switch (chatType) {
        case QTalk::Enum::TwoPersonChat:
        case QTalk::Enum::Consult:
        case QTalk::Enum::ConsultServer:
        case QTalk::Enum::Robot:
        case QTalk::Enum::Collection:
            subKey = DEM_TWOPERSONCHAT;
            break;
        case QTalk::Enum::GroupChat:
            subKey = DEM_GROUPCHAT;
            break;
        case QTalk::Enum::System:
            subKey = DEM_SYSTEM;
            break;
        default:
            break;
    }

    std::string strTime;
    if (LogicManager::instance()->getDatabase()->getConfig(DEM_MESSAGE_MAXTIMESTAMP, subKey, strTime)) {
        timeStamp = atoll(strTime.c_str());
    }
    info_log("getconfig time {0}", timeStamp);
//    if (timeStamp <= 0) {
//        timeStamp = LogicManager::instance()->GetDatabase()->getMaxTimeStampByChatType(chatType);
//    }
//    info_log("getMaxTimeStampByChatType time{0}", timeStamp);
    if (timeStamp <= 0) {
        time_t now = time(0);
        timeStamp = (now - Platform::instance().getServerDiffTime() - 3600 * 48) * 1000;
    }
    info_log("get now time{0}", timeStamp);

    return timeStamp;
}

void OfflineMessageManager::updateGroupMasks() {
    try {
        std::map<std::string, QInt64> readMarkList;
        getGroupReadMark(readMarkList);

        bool ret = LogicManager::instance()->getDatabase()->updateReadMask(readMarkList);
        if (ret) {
            //登录后阅读指针更新成功将时间至0
            LogicManager::instance()->getDatabase()->saveLoginBeforeGroupReadMarkTime("0");
        }
    }
    catch (std::exception &e) {
        error_log("updateGroupMasks exception {0}", e.what());
        throw e.what();
    }
}

std::string OfflineMessageManager::safeGetJsonStringValue(const cJSON *cjson, const char *const key) {
    cJSON *item = cJSON_GetObjectItem(cjson, key);
    if (item) {
        return item->valuestring;
    }
    return "";
}

int OfflineMessageManager::safeGetJsonIntValue(const cJSON *cjson, const char *const key) {
    cJSON *item = cJSON_GetObjectItem(cjson, key);
    if (item) {
        return item->valueint;
    }
    return -1;
}

QInt64 OfflineMessageManager::safeGetJsonLongValue(const cJSON *cjson, const char *const key) {
    cJSON *item = cJSON_GetObjectItem(cjson, key);
    if (item) {
        return item->valuedouble;
    }
    return -1;
}


void OfflineMessageManager::getOfflineChatMessageJson(long long chatTimestamp, int count, bool &complete,
                                                      std::string &errMsg,
                                                      std::vector<QTalk::Entity::ImMessageInfo> &outMsgList,
                                                      std::vector<QTalk::Entity::ImSessionInfo> &outSessionList) {

    info_log("开始获取单人消息");

    std::string httpHost = NavigationManager::instance().getJavaHost();
    std::string method = "/qtapi/gethistory.qunar";
    std::string params = "server=" + Platform::instance().getSelfDomain()
                         + "&c=qtalk&u=" + Platform::instance().getSelfUserId()
                         + "&p=" + Platform::instance().getPlatformStr()
                         + "&v=" + Platform::instance().getClientVersion();

    std::string selfJid = Platform::instance().getSelfUserId();
    std::string url = httpHost + method + "?" + params;
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *user = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "user", user);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "host", host);
    cJSON *time = cJSON_CreateNumber(chatTimestamp);
    cJSON_AddItemToObject(jsonObject, "time", time);
    cJSON *num = cJSON_CreateNumber(count);
    cJSON_AddItemToObject(jsonObject, "num", num);
    cJSON *f = cJSON_CreateString("t");
    cJSON_AddItemToObject(jsonObject, "f", f);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    //
    auto callback = [this, chatTimestamp, count, selfJid, &complete, &errMsg, &outMsgList, &outSessionList]
            (int code, std::string responeseData) {
        warn_log("{0}  {1}", code, responeseData);
        std::map<std::string, QTalk::Entity::ImSessionInfo> sessionMap;
        if (code == 200) {

            cJSON *data = cJSON_Parse(responeseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(data, "ret")->valueint;
            if (ret) {

                cJSON *msgList = cJSON_GetObjectItem(data, "data");
                int size = cJSON_GetArraySize(msgList);
                complete = true;
                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(msgList, index);
                    std::string from = safeGetJsonStringValue(item, "from");
                    std::string fromDomain = safeGetJsonStringValue(item, "from_host");
                    std::string fromJid = from + "@" + fromDomain;
                    std::string to = safeGetJsonStringValue(item, "to");
                    std::string toDomain = safeGetJsonStringValue(item, "to_host");
                    std::string toJid = to + "@" + toDomain;
                    int readFlag = safeGetJsonIntValue(item, "read_flag");
                    std::string xmppId;
                    std::string realJid;
                    int chatType = QTalk::Enum::TwoPersonChat;
                    if (cJSON_HasObjectItem(item, "body") && cJSON_HasObjectItem(item, "message")) {
                        cJSON *message = cJSON_GetObjectItem(item, "message");
                        cJSON *body = cJSON_GetObjectItem(item, "body");

                        std::string type = safeGetJsonStringValue(message, "type");
                        std::string msgId = safeGetJsonStringValue(body, "id");
                        if (msgId.empty()) {
                            continue;
                        }
                        std::string chatId = safeGetJsonStringValue(message, "qchatid");
                        if (chatId.empty()) {
                            chatId = safeGetJsonStringValue(message, "qchatid");
                        }
                        if (chatId.empty()) {
                            chatId = "4";
                        }
                        bool isConsult = false;
                        QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                        std::string msg = safeGetJsonStringValue(body, "content");
                        std::string channelInfo = safeGetJsonStringValue(message, "channelid");
                        int platform = safeGetJsonIntValue(body, "maType");
                        int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                        std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                        //之前的老逻辑 去掉
//                        if (type == "note") {
//                            msgType = -11;
//                        } else
                        if (type == "consult") {
                            isConsult = true;
                        } else if (type != "chat" && type != "revoke" && type != "subscription") {
                            continue;
                        }
                        if (type == "subscription") {
                            chatType = QTalk::Enum::Robot;
                        }
                        int direction = 0;
                        if (fromJid == (selfJid + "@" + fromDomain)) {
                            if (isConsult) {
                                xmppId = toJid;
                                if (chatId == "4") {
                                    realJid = toJid;
                                    chatType = QTalk::Enum::Consult;
                                } else {
                                    std::string realTo = safeGetJsonStringValue(message, "realto");
                                    realJid = split(realTo, '/').front();
                                    chatType = QTalk::Enum::ConsultServer;
                                }
                            } else {
                                xmppId = toJid;
                                realJid = toJid;
                            }
                            direction = 1;
                            if (msgType == 1003 || msgType == 1004 || msgType == 1002 || msgType == 1001) {
                                continue;
                            }
                        } else {
                            direction = 0;
                            if (isConsult) {
                                xmppId = fromJid;
                                if (chatId == "4") {
                                    std::string realfrom = safeGetJsonStringValue(message, "realfrom");
                                    realJid = split(realfrom, '/').front();
                                    chatType = QTalk::Enum::ConsultServer;
                                } else {
                                    realJid = fromJid;
                                    chatType = QTalk::Enum::Consult;
                                }
                            } else {
                                xmppId = fromJid;
                                realJid = fromJid;
                            }
                            if (msgType == 1004 || msgType == 1003 || msgType == 1002 || msgType == 1001) {
                                continue;
                            }
                        }

                        QTalk::Entity::ImMessageInfo msgInfo;
                        msgInfo.MsgId = msgId;
                        msgInfo.XmppId = xmppId;
                        msgInfo.RealJid = realJid;
                        msgInfo.ChatType = chatType;
                        msgInfo.Direction = direction;
                        msgInfo.From = fromJid;
                        msgInfo.To = toJid;
                        msgInfo.Content = msg;
                        msgInfo.ExtendedInfo = extendInfo;
                        msgInfo.Platform = platform;
                        msgInfo.Type = msgType;
                        msgInfo.State = 1; // 从服务器拉回来的消息 -> 消息已送达服务器
                        msgInfo.ReadedTag = readFlag;
                        msgInfo.LastUpdateTime = msec_times;
                        outMsgList.push_back(msgInfo);

                        std::string sessionKey = xmppId + "-" + realJid;
                        auto iter = sessionMap.find(sessionKey);

                        if (sessionMap.end() != iter) {
                            QTalk::Entity::ImSessionInfo sessionInfo = iter->second;
                            if (msec_times > sessionInfo.LastUpdateTime) {
                                sessionInfo.LastMessageId = msgId;
                                sessionInfo.LastUpdateTime = msec_times;
                                sessionMap[sessionKey] = sessionInfo;
                            }

                        } else {
                            QTalk::Entity::ImSessionInfo sessionInfo;
                            sessionInfo.XmppId = xmppId;
                            sessionInfo.RealJid = realJid;
                            sessionInfo.ChatType = chatType;
                            sessionInfo.LastMessageId = msgId;
                            sessionInfo.UnreadCount = 0;
                            sessionInfo.LastUpdateTime = msec_times;
                            sessionInfo.MessageState = 0;
                            sessionMap.insert(
                                    map<std::string, QTalk::Entity::ImSessionInfo>::value_type(sessionKey,
                                                                                               sessionInfo));
//                        outSessionList->push_back(sessionInfo);
                        }
                    }
                }
                //组装会话列表
                auto iter = sessionMap.begin();
                for (; iter != sessionMap.end(); ++iter) {
                    QTalk::Entity::ImSessionInfo sessionInfo = iter->second;
                    outSessionList.push_back(sessionInfo);
                }
            } else {
                complete = false;
                errMsg = cJSON_GetObjectItem(data, "errmsg")->valuestring;
            }
            cJSON_Delete(data);
        } else {
            complete = false;
            errMsg = "请求失败";
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(url, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", url, postData);
        _pComm->addHttpRequest(req, callback);
    }
}

void OfflineMessageManager::getGroupReadMark(std::map<std::string, QInt64> &readMarkList) {

    std::string httpHost = NavigationManager::instance().getJavaHost();
    std::string method = "/qtapi/get_muc_readmark1.qunar";
    std::string params = "server=" + Platform::instance().getSelfDomain()
                         + "&c=qtalk&u=" + Platform::instance().getSelfUserId()
                         + "&p=" + Platform::instance().getPlatformStr()
                         + "&v=" + Platform::instance().getClientVersion();

    std::string timeStamp = LogicManager::instance()->getDatabase()->getLoginBeforeGroupReadMarkTime();
    timeStamp = std::to_string(std::strtoll(timeStamp.data(), nullptr, 0));
    //
    std::string selfJid = Platform::instance().getSelfUserId() + "@" + Platform::instance().getSelfDomain();
    std::string url = httpHost + method + "?" + params;
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *user = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "user", user);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "host", host);
    cJSON *time = cJSON_CreateString(timeStamp.c_str());
    cJSON_AddItemToObject(jsonObject, "time", time);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    std::map<std::string, QTalk::Entity::ImSessionInfo> sessionMap;


    auto callback = [this, httpHost, &readMarkList](int code, string responseData) {
        warn_log("{0}  {1}", code, responseData);
        if (code == 200) {
            cJSON *requestData = cJSON_Parse(responseData.c_str());

            if (requestData == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(requestData, "ret")->valueint;
            if (ret) {
                cJSON *data = cJSON_GetObjectItem(requestData, "data");
                int size = cJSON_GetArraySize(data);
                std::map<std::string, QInt64> readMark;
                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(data, index);
                    std::string mucName = safeGetJsonStringValue(item, "muc_name");
                    std::string domain = safeGetJsonStringValue(item, "domain");
                    QInt64 date = atoll(safeGetJsonStringValue(item, "date").c_str());
                    std::string groupId = mucName + "@" + domain;
                    readMarkList.insert(std::map<std::string, QInt64>::value_type(groupId, date));
                }
            } else {
                std::ostringstream os("error code is :");
                os << ret
                   << ", url is :"
                   << httpHost;
                throw std::logic_error(os.str());
            }

            cJSON_Delete(requestData);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(url, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", url, postData);
        _pComm->addHttpRequest(req, callback);
    }
}

void OfflineMessageManager::getOfflineGroupMessageJson(long long chatTimestamp, int count, bool &complete,
                                                       std::string &errMsg,
                                                       std::vector<QTalk::Entity::ImMessageInfo> &outMsgList,
                                                       std::vector<QTalk::Entity::ImSessionInfo> &outSessionList) {
    try {
        std::string httpHost = NavigationManager::instance().getJavaHost();
        std::string method = "/qtapi/getmuchistory.qunar";
        std::string params = "server=" + Platform::instance().getSelfDomain()
                             + "&c=qtalk&u=" + Platform::instance().getSelfUserId()
                             //                       + "&k=" + Platform::GetPlatform()->getServerAuthKey()
                             + "&p=" + Platform::instance().getPlatformStr()
                             + "&v=" + Platform::instance().getClientVersion();

        std::string selfJid = Platform::instance().getSelfUserId() + "@" + Platform::instance().getSelfDomain();

        std::string httpUrl = httpHost + method + "?" + params;
        cJSON *jsonObject = cJSON_CreateObject();
        cJSON *user = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
        cJSON_AddItemToObject(jsonObject, "user", user);
        cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
        cJSON_AddItemToObject(jsonObject, "domain", domain);
        cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
        cJSON_AddItemToObject(jsonObject, "host", host);
        cJSON *time = cJSON_CreateNumber(chatTimestamp);
        cJSON_AddItemToObject(jsonObject, "time", time);
        cJSON *num = cJSON_CreateNumber(count);
        cJSON_AddItemToObject(jsonObject, "num", num);
        cJSON *f = cJSON_CreateString("t");
        cJSON_AddItemToObject(jsonObject, "f", f);
        std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
        cJSON_Delete(jsonObject);

        auto callback = [this, chatTimestamp, count, selfJid, &complete, &errMsg, &outMsgList, &outSessionList]
                (int code, string responseData) {
            warn_log("{0}  {1}", code, responseData);
            std::map<std::string, QTalk::Entity::ImSessionInfo> sessionMap;
            if (code == 200) {

                cJSON *data = cJSON_Parse(responseData.c_str());

                if (data == nullptr) {
                    error_log("json paring error"); return;
                }

                int ret = cJSON_GetObjectItem(data, "ret")->valueint;
                if (ret) {
                    complete = true;
                    cJSON *msgList = cJSON_GetObjectItem(data, "data");
                    int size = cJSON_GetArraySize(msgList);
                    for (int index = 0; index < size; index++) {
                        cJSON *item = cJSON_GetArrayItem(msgList, index);
                        std::string nickName = safeGetJsonStringValue(item, "nick");
                        int chatType = QTalk::Enum::GroupChat;
                        cJSON *message = cJSON_GetObjectItem(item, "message");
                        std::string xmppId = safeGetJsonStringValue(message, "to");
                        if (xmppId.empty()) {
                            continue;
                        }

                        const std::string &realJid = xmppId;
                        if (cJSON_HasObjectItem(item, "body")) {
                            cJSON *body = cJSON_GetObjectItem(item, "body");
                            std::string msgId = safeGetJsonStringValue(body, "id");
                            if (msgId.empty()) {
                                continue;
                            }
                            QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                            std::string msg = safeGetJsonStringValue(body, "content");
                            int platform = safeGetJsonIntValue(body, "maType");
//                    int msgType = safeGetJsonIntValue(body,"msgType");
                            int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                            std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                            std::string realFrom = safeGetJsonStringValue(message, "sendjid");
                            int direction = 0;
                            int readState = 0;
                            if (realFrom == selfJid) {
                                direction = 1;
                                readState = 1;//已发送
                            } else {
                                direction = 0;
                                readState = 0;//未读
                            }

                            QTalk::Entity::ImMessageInfo msgInfo;
                            msgInfo.MsgId = msgId;
                            msgInfo.XmppId = xmppId;
                            msgInfo.RealJid = realJid;
                            msgInfo.ChatType = chatType;
                            msgInfo.Direction = direction;
                            msgInfo.From = realFrom;
                            msgInfo.To = "";
                            msgInfo.Content = msg;
                            msgInfo.ExtendedInfo = extendInfo;
                            msgInfo.Platform = platform;
                            msgInfo.Type = msgType;
                            msgInfo.State = 1;
                            msgInfo.ReadedTag = readState;
                            msgInfo.LastUpdateTime = msec_times;
                            outMsgList.push_back(msgInfo);

                            std::string sessionKey = xmppId + "-" + realJid;
                            auto iter = sessionMap.find(
                                    sessionKey);
                            if (sessionMap.end() != iter) {
                                QTalk::Entity::ImSessionInfo sessionInfo = iter->second;
                                if (msec_times > sessionInfo.LastUpdateTime) {
                                    sessionInfo.LastMessageId = msgId;
                                    sessionInfo.LastUpdateTime = msec_times;
                                    sessionMap[sessionKey] = sessionInfo;
                                }
                            } else {
                                QTalk::Entity::ImSessionInfo sessionInfo;
                                sessionInfo.XmppId = xmppId;
                                sessionInfo.RealJid = realJid;
                                sessionInfo.ChatType = chatType;
                                sessionInfo.LastMessageId = msgId;
                                sessionInfo.UnreadCount = 0;
                                sessionInfo.LastUpdateTime = msec_times;
                                sessionInfo.MessageState = 0;
                                sessionMap.insert(map<std::string, QTalk::Entity::ImSessionInfo>::value_type(sessionKey,
                                                                                                             sessionInfo));
//                        outSessionList->push_back(sessionInfo);
                            }
                        }
                    }
                    //组装会话列表
                    auto iter = sessionMap.begin();
                    for (; iter != sessionMap.end(); ++iter) {
                        QTalk::Entity::ImSessionInfo sessionInfo = iter->second;
                        outSessionList.push_back(sessionInfo);
                    }
                } else {
                    complete = false;
                    errMsg = cJSON_GetObjectItem(data, "errmsg")->valuestring;
                    throw std::logic_error(errMsg);
                }

                cJSON_Delete(data);
            } else {
                complete = false;
                errMsg = "请求失败";
            }

        };

        if (_pComm) {
            QTalk::HttpRequest req(httpUrl, QTalk::RequestMethod::POST);
            req.header["Content-Type"] = "application/json;";
            req.body = postData;
            warn_log("{0} \n {1}", httpUrl, postData);
            _pComm->addHttpRequest(req, callback);
        }

    } catch (std::exception &e) {
        std::string what = e.what();

        warn_log(what);
    }
}

void OfflineMessageManager::getOfflineNoticeMessageJson(long long noticeTimestamp, int count, bool &complete,
                                                        std::string &errMsg,
                                                        std::vector<QTalk::Entity::ImMessageInfo> &outMsgList,
                                                        std::vector<QTalk::Entity::ImSessionInfo> &outSessionList) {

    std::string httpHost = NavigationManager::instance().getJavaHost();
    std::string method = "/qtapi/get_system_history.qunar";
    std::string params = "server=" + Platform::instance().getSelfDomain()
                         + "&c=qtalk&u=" + Platform::instance().getSelfUserId()
                         //                       + "&k=" + Platform::GetPlatform()->getServerAuthKey()
                         + "&p=" + Platform::instance().getPlatformStr()
                         + "&v=" + Platform::instance().getClientVersion();

    std::string selfJid = Platform::instance().getSelfUserId() + "@" + Platform::instance().getSelfDomain();
    std::string url = httpHost + method + "?" + params;
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *user = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "user", user);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "host", host);
    cJSON *time = cJSON_CreateNumber(noticeTimestamp);
    cJSON_AddItemToObject(jsonObject, "time", time);
    cJSON *num = cJSON_CreateNumber(count);
    cJSON_AddItemToObject(jsonObject, "num", num);
    cJSON *f = cJSON_CreateString("t");
    cJSON_AddItemToObject(jsonObject, "f", f);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    auto callback = [this, noticeTimestamp, count, selfJid, &complete, &errMsg, &outMsgList, &outSessionList]
            (int code, string responseData) {
        warn_log("{0}  {1}", code, responseData);
        std::map<std::string, QTalk::Entity::ImSessionInfo> sessionMap;
        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(data, "ret")->valueint;
            if (ret) {
                complete = true;
                cJSON *msgList = cJSON_GetObjectItem(data, "data");
//            std::cout << "msgList json" << cJSON_Print(msgList) << std::endl;
                int size = cJSON_GetArraySize(msgList);
                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(msgList, index);
                    std::string nickName = safeGetJsonStringValue(item, "nick");
                    int chatType = QTalk::Enum::System;
                    cJSON *message = cJSON_GetObjectItem(item, "message");
                    std::string xmppId = "SystemMessage@" + Platform::instance().getSelfDomain();
                    std::string type = safeGetJsonStringValue(message, "type");
                    const std::string &realJid = xmppId;
                    if (cJSON_HasObjectItem(item, "body")) {
                        cJSON *body = cJSON_GetObjectItem(item, "body");
                        std::string msgId = safeGetJsonStringValue(body, "id");
                        if (msgId.empty()) {
                            continue;
                        }
                        QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                        std::string msg = safeGetJsonStringValue(body, "content");
                        int platform = safeGetJsonIntValue(body, "maType");
//                    int msgType = safeGetJsonIntValue(body,"msgType");
                        int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                        std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                        std::string realFrom = safeGetJsonStringValue(message, "sendjid");

                        QTalk::Entity::ImMessageInfo msgInfo;
                        msgInfo.MsgId = msgId;
                        msgInfo.XmppId = xmppId;
                        msgInfo.RealJid = realJid;
                        msgInfo.ChatType = chatType;
                        msgInfo.Direction = 0;
                        msgInfo.From = realFrom;
                        msgInfo.To = "";
                        msgInfo.Content = msg;
                        msgInfo.ExtendedInfo = extendInfo;
                        msgInfo.Platform = platform;
                        msgInfo.Type = msgType;
                        msgInfo.State = 1;
                        msgInfo.ReadedTag = 0;
                        msgInfo.LastUpdateTime = msec_times;
                        outMsgList.push_back(msgInfo);

                        std::string sessionKey = xmppId + "-" + realJid;
                        auto iter = sessionMap.find(sessionKey);
                        if (sessionMap.end() != iter) {
                            QTalk::Entity::ImSessionInfo sessionInfo = iter->second;
                            sessionInfo.LastMessageId = msgId;
                            sessionInfo.LastUpdateTime = msec_times;
                        } else {
                            QTalk::Entity::ImSessionInfo sessionInfo;
                            sessionInfo.XmppId = xmppId;
                            sessionInfo.RealJid = realJid;
                            sessionInfo.ChatType = chatType;
                            sessionInfo.LastMessageId = msgId;
                            sessionInfo.UnreadCount = 0;
                            sessionInfo.LastUpdateTime = msec_times;
                            sessionInfo.MessageState = 0;
                            sessionMap.insert(
                                    map<std::string, QTalk::Entity::ImSessionInfo>::value_type(sessionKey,
                                                                                               sessionInfo));
                            outSessionList.push_back(sessionInfo);
                        }

                    }
                }
            } else {
                complete = false;
                errMsg = cJSON_GetObjectItem(data, "errmsg")->valuestring;
            }

            cJSON_Delete(data);
        } else {
            complete = false;
            errMsg = "请求失败";
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(url, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", url, postData);
        _pComm->addHttpRequest(req, callback);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/13
  */
void OfflineMessageManager::getUserMessage(const QInt64 &time, const std::string &userId) {
    //
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/qtapi/getmsgs.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string strUrl = url.str();

    QTalk::Entity::JID jid(&userId);
    cJSON *jsonObject = cJSON_CreateObject();

    cJSON *from = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "from", from);
    cJSON *fhost = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "fhost", fhost);
    cJSON *to = cJSON_CreateString(jid.username().c_str());
    cJSON_AddItemToObject(jsonObject, "to", to);
    cJSON *thost = cJSON_CreateString(jid.domainname().c_str());
    cJSON_AddItemToObject(jsonObject, "thost", thost);
    cJSON *direction = cJSON_CreateString("0");
    cJSON_AddItemToObject(jsonObject, "direction", direction);
    cJSON *timet = cJSON_CreateNumber(time);
    cJSON_AddItemToObject(jsonObject, "time", timet);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    cJSON *num = cJSON_CreateNumber(20);
    cJSON_AddItemToObject(jsonObject, "num", num);
    cJSON *f = cJSON_CreateString("t");
    cJSON_AddItemToObject(jsonObject, "f", f);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    //
    std::vector<QTalk::Entity::ImMessageInfo> msgList;
    auto callBack = [this, time, userId, &msgList](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON *resData = cJSON_Parse(responseData.c_str());

            if (resData == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(resData, "ret")->valueint;
            if (ret) {
                std::string selfJid = Platform::instance().getSelfUserId();

                cJSON *data = cJSON_GetObjectItem(resData, "data");
                int size = cJSON_GetArraySize(data);

                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(data, index);
                    std::string from = safeGetJsonStringValue(item, "from");
                    std::string fromDomain = safeGetJsonStringValue(item, "from_host");
                    std::string fromJid = from + "@" + fromDomain;
                    std::string to = safeGetJsonStringValue(item, "to");
                    std::string toDomain = safeGetJsonStringValue(item, "to_host");
                    std::string toJid = to + "@" + toDomain;
                    int readFlag = safeGetJsonIntValue(item, "read_flag");
                    std::string xmppId;
                    std::string realJid;
                    QUInt8 chatType = QTalk::Enum::TwoPersonChat;
                    if (cJSON_HasObjectItem(item, "body") && cJSON_HasObjectItem(item, "message")) {
                        cJSON *message = cJSON_GetObjectItem(item, "message");
                        cJSON *body = cJSON_GetObjectItem(item, "body");

                        std::string type = safeGetJsonStringValue(message, "type");
                        std::string msgId = safeGetJsonStringValue(body, "id");
                        //
                        if (msgId.empty()) continue;

                        std::string chatId = safeGetJsonStringValue(message, "qchatid");
                        if (chatId.empty()) {
                            chatId = safeGetJsonStringValue(body, "qchatid");
                        }
                        if (chatId.empty()) {
                            chatId = "4";
                        }
                        bool isConsult = false;
                        QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                        std::string msg = safeGetJsonStringValue(body, "content");
                        std::string channelInfo = safeGetJsonStringValue(message, "channelid");
                        int platform = safeGetJsonIntValue(body, "maType");
                        int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                        std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                        if (type == "note") {
                            msgType = -11;
                        } else if (type == "consult") {
                            isConsult = true;
                        } else if (type != "chat" && type != "revoke" && type != "subscription") {
                            continue;
                        }
                        if (type == "subscription") {
                            chatType = QTalk::Enum::Robot;
                        }
                        int direction = 0;
                        if (fromJid == (selfJid + "@" + fromDomain)) {
                            if (isConsult) {
                                xmppId = toJid;
                                if (chatId == "4") {
                                    realJid = toJid;
                                    chatType = QTalk::Enum::Consult;
                                } else {
                                    std::string realTo = safeGetJsonStringValue(message, "realto");
                                    realJid = split(realTo, '/').front();
                                    chatType = QTalk::Enum::ConsultServer;
                                }
                            } else {
                                xmppId = toJid;
                                realJid = toJid;
                            }
                            direction = 1;
                            if (msgType == 1003 || msgType == 1004 || msgType == 1002 || msgType == 1001) {
                                continue;
                            }
                        } else {
                            direction = 0;
                            if (isConsult) {
                                xmppId = fromJid;
                                if (chatId == "4") {
                                    std::string realfrom = safeGetJsonStringValue(message, "realfrom");
                                    realJid = split(realfrom, '/').front();
                                    chatType = QTalk::Enum::ConsultServer;
                                } else {
                                    realJid = fromJid;
                                    chatType = QTalk::Enum::Consult;
                                }
                            } else {
                                xmppId = fromJid;
                                realJid = fromJid;
                            }
                            if (msgType == 1004 || msgType == 1003 || msgType == 1002 || msgType == 1001) {
                                continue;
                            }
                        }

                        QTalk::Entity::ImMessageInfo msgInfo;
                        msgInfo.MsgId = msgId;
                        msgInfo.XmppId = xmppId;
                        msgInfo.RealJid = realJid;
                        msgInfo.ChatType = chatType;
                        msgInfo.Direction = direction;
                        msgInfo.From = fromJid;
                        msgInfo.To = toJid;
                        msgInfo.Content = msg;
                        msgInfo.ExtendedInfo = extendInfo;
                        msgInfo.Platform = platform;
                        msgInfo.Type = msgType;
                        msgInfo.State = 1; // 从服务器拉回来的消息 -> 消息已送达服务器
                        msgInfo.ReadedTag = readFlag;
                        msgInfo.LastUpdateTime = msec_times;
                        msgList.push_back(msgInfo);
                    }
                }
            }
            cJSON_Delete(resData);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", strUrl, postData);
        _pComm->addHttpRequest(req, callBack);
        if (msgList.size() > 0) {
            LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);
        }
    }
}

/**
 * 获取客服测 consult会话上翻历史消息
 * @param time
 * @param userId
 * @param realJid
 */
void OfflineMessageManager::getConsultServerMessage(const QInt64 &time, const std::string &userId,
                                                    const std::string &realJid) {
    if(realJid.empty()){
        return;
    }
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/qtapi/getconsultmsgs.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string strUrl = url.str();

    QTalk::Entity::JID jid(&userId);
    QTalk::Entity::JID relId(&realJid);
    cJSON *jsonObject = cJSON_CreateObject();

    cJSON *from = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "from", from);
    cJSON *fhost = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "fhost", fhost);
    cJSON *to = cJSON_CreateString(relId.username().c_str());
    cJSON_AddItemToObject(jsonObject, "to", to);
    cJSON *virtualid = cJSON_CreateString(jid.username().c_str());
    cJSON_AddItemToObject(jsonObject, "virtual", virtualid);
    cJSON *thost = cJSON_CreateString(jid.domainname().c_str());
    cJSON_AddItemToObject(jsonObject, "thost", thost);
    cJSON *direction = cJSON_CreateString("0");
    cJSON_AddItemToObject(jsonObject, "direction", direction);
    cJSON *timet = cJSON_CreateNumber(time);
    cJSON_AddItemToObject(jsonObject, "time", timet);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    cJSON *num = cJSON_CreateNumber(20);
    cJSON_AddItemToObject(jsonObject, "num", num);
    cJSON *f = cJSON_CreateString("t");
    cJSON_AddItemToObject(jsonObject, "f", f);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    //
    std::vector<QTalk::Entity::ImMessageInfo> msgList;
    auto callBack = [this, time, userId, &msgList](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON *resData = cJSON_Parse(responseData.c_str());

            if (resData == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(resData, "ret")->valueint;
            if (ret) {
                std::string selfJid = Platform::instance().getSelfUserId();

                cJSON *data = cJSON_GetObjectItem(resData, "data");
                int size = cJSON_GetArraySize(data);

                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(data, index);
                    std::string from = safeGetJsonStringValue(item, "from");
                    std::string fromDomain = safeGetJsonStringValue(item, "from_host");
                    std::string fromJid = from + "@" + fromDomain;
                    std::string to = safeGetJsonStringValue(item, "to");
                    std::string toDomain = safeGetJsonStringValue(item, "to_host");
                    std::string toJid = to + "@" + toDomain;
                    int readFlag = safeGetJsonIntValue(item, "read_flag");
                    std::string xmppId;
                    std::string realJid;
                    QUInt8 chatType = QTalk::Enum::TwoPersonChat;
                    if (cJSON_HasObjectItem(item, "body") && cJSON_HasObjectItem(item, "message")) {
                        cJSON *message = cJSON_GetObjectItem(item, "message");
                        cJSON *body = cJSON_GetObjectItem(item, "body");

                        std::string type = safeGetJsonStringValue(message, "type");
                        std::string msgId = safeGetJsonStringValue(body, "id");
                        //
                        if (msgId.empty()) continue;

                        std::string chatId = safeGetJsonStringValue(message, "qchatid");
                        if (chatId.empty()) {
                            chatId = safeGetJsonStringValue(body, "qchatid");
                        }
                        QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                        std::string msg = safeGetJsonStringValue(body, "content");
                        std::string channelInfo = safeGetJsonStringValue(message, "channelid");
                        int platform = safeGetJsonIntValue(body, "maType");
                        int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                        std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                        int direction = 0;
                        if (fromJid == (selfJid + "@" + fromDomain)) {
                            xmppId = toJid;
                            if (chatId == "4") {
                                realJid = toJid;
                                chatType = QTalk::Enum::Consult;
                            } else {
                                std::string realTo = safeGetJsonStringValue(message, "realto");
                                realJid = split(realTo, '/').front();
                                chatType = QTalk::Enum::ConsultServer;
                            }
                            direction = 1;
                            if (msgType == 1003 || msgType == 1004 || msgType == 1002 || msgType == 1001) {
                                continue;
                            }
                        } else {
                            direction = 0;
                            xmppId = fromJid;
                            if (chatId == "4") {
                                std::string realfrom = safeGetJsonStringValue(message, "realfrom");
                                realJid = split(realfrom, '/').front();
                                chatType = QTalk::Enum::ConsultServer;
                            } else {
                                realJid = fromJid;
                                chatType = QTalk::Enum::Consult;
                            }
                            if (msgType == 1004 || msgType == 1003 || msgType == 1002 || msgType == 1001) {
                                continue;
                            }
                        }

                        QTalk::Entity::ImMessageInfo msgInfo;
                        msgInfo.MsgId = msgId;
                        msgInfo.XmppId = xmppId;
                        msgInfo.RealJid = realJid;
                        msgInfo.ChatType = chatType;
                        msgInfo.Direction = direction;
                        msgInfo.From = fromJid;
                        msgInfo.To = toJid;
                        msgInfo.Content = msg;
                        msgInfo.ExtendedInfo = extendInfo;
                        msgInfo.Platform = platform;
                        msgInfo.Type = msgType;
                        msgInfo.State = 1; // 从服务器拉回来的消息 -> 消息已送达服务器
                        msgInfo.ReadedTag = readFlag;
                        msgInfo.LastUpdateTime = msec_times;
                        msgList.push_back(msgInfo);
                    }
                }
            }
            cJSON_Delete(resData);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", strUrl, postData);
        _pComm->addHttpRequest(req, callBack);
        if (msgList.size() > 0) {
            LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);
        }
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/13
  */
void OfflineMessageManager::getGroupMessage(const QInt64 &time, const std::string &userId) {
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/qtapi/getmucmsgs.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string strUrl = url.str();

    QTalk::Entity::JID jid(&userId);
    cJSON *jsonObject = cJSON_CreateObject();

    cJSON *muc = cJSON_CreateString(jid.username().c_str());
    cJSON_AddItemToObject(jsonObject, "muc", muc);
    cJSON *direction = cJSON_CreateString("0");
    cJSON_AddItemToObject(jsonObject, "direction", direction);
    cJSON *num = cJSON_CreateNumber(20);
    cJSON_AddItemToObject(jsonObject, "num", num);
    cJSON *timet = cJSON_CreateNumber(time);
    cJSON_AddItemToObject(jsonObject, "time", timet);
    cJSON *domain = cJSON_CreateString(jid.domainname().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    //
    std::vector<QTalk::Entity::ImMessageInfo> msgList;
    auto callBack = [this, time, userId, &msgList](int code, const std::string &responseData) {
        if (code == 200) {

            cJSON *resdata = cJSON_Parse(responseData.c_str());

            if (resdata == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(resdata, "ret")->valueint;
            if (ret) {
                std::string selfJid = Platform::instance().getSelfXmppId();

                cJSON *data = cJSON_GetObjectItem(resdata, "data");
                int size = cJSON_GetArraySize(data);
                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(data, index);
                    std::string nickName = safeGetJsonStringValue(item, "nick");
                    int chatType = QTalk::Enum::GroupChat;
                    cJSON *message = cJSON_GetObjectItem(item, "message");
                    std::string xmppId = safeGetJsonStringValue(message, "to");
                    if (xmppId.empty()) {
                        continue;
                    }

                    const std::string &realJid = xmppId;
                    if (cJSON_HasObjectItem(item, "body")) {
                        cJSON *body = cJSON_GetObjectItem(item, "body");
                        std::string msgId = safeGetJsonStringValue(body, "id");
                        if (msgId.empty()) {
                            continue;
                        }
                        QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                        std::string msg = safeGetJsonStringValue(body, "content");
                        int platform = safeGetJsonIntValue(body, "maType");
                        //                    int msgType = safeGetJsonIntValue(body,"msgType");
                        int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                        std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                        std::string realFrom = safeGetJsonStringValue(message, "sendjid");
                        int direction = 0;
                        int readState = 0;
                        if (realFrom == selfJid) {
                            direction = 1;
                            readState = 1;//已发送
                        } else {
                            direction = 0;
                            readState = 3;//未读
                        }

                        QTalk::Entity::ImMessageInfo msgInfo;
                        msgInfo.MsgId = msgId;
                        msgInfo.XmppId = xmppId;
                        msgInfo.RealJid = realJid;
                        msgInfo.ChatType = chatType;
                        msgInfo.Direction = direction;
                        msgInfo.From = realFrom;
                        msgInfo.To = "";
                        msgInfo.Content = msg;
                        msgInfo.ExtendedInfo = extendInfo;
                        msgInfo.Platform = platform;
                        msgInfo.Type = msgType;
                        msgInfo.State = 1;
                        msgInfo.ReadedTag = readState;
                        msgInfo.LastUpdateTime = msec_times;
                        msgList.push_back(msgInfo);

                    }
                }

            }
            cJSON_Delete(resdata);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", strUrl, postData);
        _pComm->addHttpRequest(req, callBack);
        if (msgList.size() > 0) {
            LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);
        }
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/12/13
  */
void OfflineMessageManager::getSystemMessage(const QInt64 &time, const std::string &userId) {
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/qtapi/get_system_msgs.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string strUrl = url.str();

    QTalk::Entity::JID jid(&userId);
    cJSON *jsonObject = cJSON_CreateObject();

    cJSON *from = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "from", from);
    cJSON *fhost = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "fhost", fhost);
    cJSON *to = cJSON_CreateString(jid.username().c_str());
    cJSON_AddItemToObject(jsonObject, "to", to);
    cJSON *thost = cJSON_CreateString(jid.domainname().c_str());
    cJSON_AddItemToObject(jsonObject, "thost", thost);
    cJSON *direction = cJSON_CreateString("0");
    cJSON_AddItemToObject(jsonObject, "direction", direction);
    cJSON *timet = cJSON_CreateNumber(time);
    cJSON_AddItemToObject(jsonObject, "time", timet);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    cJSON *num = cJSON_CreateNumber(20);
    cJSON_AddItemToObject(jsonObject, "num", num);
    cJSON *f = cJSON_CreateString("t");
    cJSON_AddItemToObject(jsonObject, "f", f);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    //
    std::vector<QTalk::Entity::ImMessageInfo> msgList;
    auto callBack = [this, time, userId, &msgList](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON *resData = cJSON_Parse(responseData.c_str());

            if (resData == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(resData, "ret")->valueint;
            if (ret) {
                std::string selfJid = Platform::instance().getSelfUserId();

                cJSON *data = cJSON_GetObjectItem(resData, "data");
                int size = cJSON_GetArraySize(data);
                for (int index = 0; index < size; index++) {
                    cJSON *item = cJSON_GetArrayItem(data, index);
                    int chatType = QTalk::Enum::System;
                    cJSON *message = cJSON_GetObjectItem(item, "message");
                    std::string xmppId = userId;
                    std::string type = safeGetJsonStringValue(message, "type");
                    const std::string &realJid = xmppId;
                    if (cJSON_HasObjectItem(item, "body")) {
                        cJSON *body = cJSON_GetObjectItem(item, "body");
                        std::string msgId = safeGetJsonStringValue(body, "id");
                        if (msgId.empty()) {
                            continue;
                        }
                        QInt64 msec_times = atoll(safeGetJsonStringValue(message, "msec_times").c_str());
                        std::string msg = safeGetJsonStringValue(body, "content");
                        int platform = safeGetJsonIntValue(body, "maType");
                        int msgType = atoi(safeGetJsonStringValue(body, "msgType").c_str());
                        std::string extendInfo = safeGetJsonStringValue(body, "extendInfo");
                        std::string realFrom = safeGetJsonStringValue(message, "sendjid");

                        QTalk::Entity::ImMessageInfo msgInfo;
                        msgInfo.MsgId = msgId;
                        msgInfo.XmppId = xmppId;
                        msgInfo.RealJid = realJid;
                        msgInfo.ChatType = chatType;
                        msgInfo.Direction = 0;
                        msgInfo.From = realFrom;
                        msgInfo.To = "";
                        msgInfo.Content = msg;
                        msgInfo.ExtendedInfo = extendInfo;
                        msgInfo.Platform = platform;
                        msgInfo.Type = msgType;
                        msgInfo.State = 1;
                        msgInfo.ReadedTag = 3;
                        msgInfo.LastUpdateTime = msec_times;
                        msgList.push_back(msgInfo);
                    }
                }
            }

            cJSON_Delete(resData);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        warn_log("{0} \n {1}", strUrl, postData);
        _pComm->addHttpRequest(req, callBack);
        if (msgList.size() > 0) {
            LogicManager::instance()->getDatabase()->bulkInsertMessageInfo(msgList);
        }
    }
}
