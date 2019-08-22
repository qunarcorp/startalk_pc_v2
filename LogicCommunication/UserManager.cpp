#include "UserManager.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "../LogicManager/LogicManager.h"
#include "../Platform/NavigationManager.h"
#include "../Platform/Platform.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "Communication.h"
#include "../QtUtil/Utils/Log.h"
#include "../Platform/dbPlatForm.h"

using namespace QTalk;

UserManager::UserManager(Communication *pComm)
        : _pComm(pComm) {

}


UserManager::~UserManager() {

}

/**
  * @函数名   getStructure
  * @功能描述 获取组织架构
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
bool UserManager::getNewStructure(bool sendEvt) {
    //{"version":1905}

    // 获取组织架构版本号
    int maxVersion = 0;
    LogicManager::instance()->getDatabase()->getUserVersion(maxVersion);
    //
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/update/getUpdateUsers.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *version = cJSON_CreateNumber(maxVersion);
    cJSON_AddItemToObject(jsonObject, "version", version);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    bool retSts = false;
    std::vector<Entity::ImUserInfo> arUserInfo;
    std::vector<std::string> arDeletes;
    auto callback = [postData, &retSts, &arUserInfo, &arDeletes](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON *resData = cJSON_Parse(responseData.c_str());

            if (resData == nullptr) {
                error_log("json paring error"); return;
                return;
            }

            cJSON_bool ret = QTalk::JSON::cJSON_SafeGetBoolValue(resData, "ret");

            if (ret) {
                cJSON *data = cJSON_GetObjectItem(resData, "data");

                int version = QTalk::JSON::cJSON_SafeGetIntValue(data, "version");
                std::string domain = Platform::instance().getSelfDomain();
                // update
                cJSON *update = cJSON_GetObjectItem(data, "update");
                int size = cJSON_GetArraySize(update);

                for (int i = 0; i < size; i++)
                {
                    Entity::ImUserInfo user;
                    cJSON *item = cJSON_GetArrayItem(update, i);

                    user.UserId = QTalk::JSON::cJSON_SafeGetStringValue(item, "U");
                    if(!user.UserId.empty() && user.UserId.find('@') != -1){
                        user.XmppId = user.UserId;
                    } else{
                        user.XmppId = user.UserId + "@" + domain;
                    }
                    user.Name = JSON::cJSON_SafeGetStringValue(item, "N");
                    user.DescInfo = JSON::cJSON_SafeGetStringValue(item, "D");
                    user.IncrementVersion = version;
                    user.SearchIndex = JSON::cJSON_SafeGetStringValue(item, "pinyin");
                    user.Gender = JSON::cJSON_SafeGetIntValue(item, "sex");
                    user.userType = JSON::cJSON_SafeGetStringValue(item, "uType");
                    user.isVisible = JSON::cJSON_SafeGetBoolValue(item, "visibleFlag", true);

                    arUserInfo.push_back(user);
                }
                // delete
                cJSON *deleteu = cJSON_GetObjectItem(data, "delete");
                size = cJSON_GetArraySize(deleteu);
                for (int i = 0; i < size; i++) {
                    cJSON *item = cJSON_GetArrayItem(deleteu, i);
                    std::string userId = JSON::cJSON_SafeGetStringValue(item, "U");
                    if (!userId.empty()) {
                        userId += "@" + domain;
                        arDeletes.push_back(userId);
                    }
                }
                retSts = true;
            }
            cJSON_Delete(resData);
        }
    };


    if (_pComm) {
        HttpRequest req(url.str(), RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {

            LogicManager::instance()->getDatabase()->bulkInsertUserInfo(arUserInfo);
            LogicManager::instance()->getDatabase()->bulkDeleteUserInfo(arDeletes);
            //
            if(sendEvt && _pComm && _pComm->_pMsgManager)
            {
                _pComm->_pMsgManager->gotIncrementUser(arUserInfo, arDeletes);
            }
        }
    }

    return retSts;
}

bool UserManager::getOldStructure() {
    //{"version":1905, "domain":"ejabhost1"}
    // 获取组织架构版本号
    int maxVersion = 0;
    LogicManager::instance()->getDatabase()->getUserVersion(maxVersion);
    //
    std::ostringstream url;
    url << NavigationManager::instance().getApiUrl()
        << "/get_increment_users"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *version = cJSON_CreateNumber(maxVersion);
    cJSON_AddItemToObject(jsonObject, "version", version);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "domain", domain);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    bool retSts = false;
    std::vector<Entity::ImUserInfo> arUserInfo;
    auto callback = [postData, &retSts, &arUserInfo](int code, std::string responseData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            bool ret = (bool)JSON::cJSON_SafeGetBoolValue(data, "ret");
            if (ret) {
                cJSON *msgList = cJSON_GetObjectItem(data, "data");
                int size = cJSON_GetArraySize(msgList);

                for (int i = 0; i < size; i++) {
                    Entity::ImUserInfo user;
                    cJSON *item = cJSON_GetArrayItem(msgList, i);

                    user.UserId = cJSON_GetObjectItem(item, "U")->valuestring;
                    user.XmppId = user.UserId + "@" + cJSON_GetObjectItem(item, "Domain")->valuestring;
                    user.Name = cJSON_GetObjectItem(item, "N")->valuestring;
                    user.DescInfo = cJSON_GetObjectItem(item, "D")->valuestring;
                    user.IncrementVersion = cJSON_GetObjectItem(item, "V")->valueint;
                    user.SearchIndex = cJSON_GetObjectItem(item, "F")->valuestring;
                    user.SearchIndex += cJSON_GetObjectItem(item, "S")->valuestring;
                    arUserInfo.push_back(user);
                }

                cJSON_Delete(data);

                retSts = true;
            }
        }
    };


    if (_pComm) {
        HttpRequest req(url.str(), RequestMethod::POST);
        //        req.header["Content-Type"] = "application/json";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {
            retSts = LogicManager::instance()->getDatabase()->bulkInsertUserInfo(arUserInfo);
        }
    }

    return retSts;
}

/**
  * @函数名   getUserCard获取用户列表
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
bool UserManager::getUserCard(const UserCardParam &param, std::vector<StUserCard> &arUserInfo) {
    //
    //[{"domain": "ejabhost1", "users" : [{"user": "chaocc.wang", "version" : 0}]}]

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/domain/get_vcard_info.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    cJSON *objs = cJSON_CreateArray();

    auto itObj = param.cbegin();
    for (; itObj != param.cend(); itObj++) {
        cJSON *obj = cJSON_CreateObject();

        cJSON_AddStringToObject(obj, "domain", itObj->first.c_str());

        cJSON *users = cJSON_CreateArray();
        for (const auto &itUsers : itObj->second) {
            cJSON *user = cJSON_CreateObject();

            cJSON_AddStringToObject(user, "user", itUsers.first.c_str());
            cJSON_AddNumberToObject(user, "version", itUsers.second);

            cJSON_AddItemToArray(users, user);
        }

        cJSON_AddItemToObject(obj, "users", users);
        cJSON_AddItemToArray(objs, obj);
    }

    std::string postData = QTalk::JSON::cJSON_to_string(objs);

    cJSON_Delete(objs);

    std::string strUrl = url.str();
    info_log("向服务器请求用户数据 url:{0} Json:{1}", strUrl.c_str(), postData);

    bool retSts = false;
    auto callback = [postData, strUrl, &retSts, &arUserInfo](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            cJSON_bool ret = JSON::cJSON_SafeGetBoolValue(data, "ret");
            if (ret) {
                cJSON *msgList = cJSON_GetObjectItem(data, "data");
                int size = cJSON_GetArraySize(msgList);

                for (int i = 0; i < size; i++) {
                    cJSON *item = cJSON_GetArrayItem(msgList, i);

                    std::string strdomain = JSON::cJSON_SafeGetStringValue(item, "domain");

                    cJSON *users = cJSON_GetObjectItem(item, "users");
                    int userSize = cJSON_GetArraySize(users);

                    for (int j = 0; j < userSize; j++) {
                        StUserCard stUserCard;
                        cJSON *user = cJSON_GetArrayItem(users, j);

                        stUserCard.xmppId = JSON::cJSON_SafeGetStringValue(user, "username");
                        stUserCard.xmppId += "@" + strdomain;
                        stUserCard.headerSrc = JSON::cJSON_SafeGetStringValue(user, "imageurl");
                        if(!stUserCard.headerSrc.empty() && stUserCard.headerSrc.find("http") != 0){
                            stUserCard.headerSrc = NavigationManager::instance().getFileHttpHost() + "/" +  stUserCard.headerSrc;
                        }
                        //先取webname 在取nickname
                        std::string webName = JSON::cJSON_SafeGetStringValue(user, "webname");
                        if(!webName.empty()){
                            stUserCard.nickName = webName;
                        } else{
                            stUserCard.nickName = JSON::cJSON_SafeGetStringValue(user, "nickname");
                        }
                        stUserCard.mood = JSON::cJSON_SafeGetStringValue(user, "mood");
                        std::string gender = JSON::cJSON_SafeGetStringValue(user, "gender");
                        stUserCard.gender = std::strtol(gender.data(), nullptr, 0);
                        std::string v = JSON::cJSON_SafeGetStringValue(user, "V");
                        stUserCard.version = std::strtol(v.data(), nullptr, 0);

                        arUserInfo.push_back(stUserCard);
                    }
                }
                info_log("从服务器请求到用户数据 个数:{0}", arUserInfo.size());
                retSts = true;
            }
            cJSON_Delete(data);
        } else {
            warn_log("请求失败  url:{0}", strUrl);
        }
    };

    if (_pComm) {
        HttpRequest req(strUrl, RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {
            retSts = LogicManager::instance()->getDatabase()->setUserCardInfo(arUserInfo);
            if (_pComm && _pComm->_pMsgManager) {
                _pComm->_pMsgManager->sendGotUserCard(arUserInfo);
            }
        }
    }

    return retSts;
}

void UserManager::getUserFullInfo(std::shared_ptr<QTalk::Entity::ImUserSupplement> &imUserSup,
                                  std::shared_ptr<QTalk::Entity::ImUserInfo>& userInfo) {
    //
    info_log("请求名片 userId:{0}", imUserSup->XmppId);
    // 强制从服务器获取最新卡片
    QTalk::Entity::JID jid(userInfo->XmppId.c_str());
    UserCardParam param;
    std::vector<StUserCard> arUserInfo;
    param[jid.domainname()][jid.username()] = 0;
    getUserCard(param, arUserInfo);
    if(arUserInfo.size() == 1)
    {
        auto info = arUserInfo[0];
        userInfo = dbPlatForm::instance().getUserInfo(userInfo->XmppId);
        if(nullptr == userInfo)
        {
            userInfo = std::make_shared<QTalk::Entity::ImUserInfo>();
            userInfo->XmppId = jid.barename();
        }

        userInfo->Name = info.userName;
        userInfo->HeaderSrc = info.headerSrc;
        userInfo->NickName = info.nickName;
        userInfo->Gender = info.gender;
        userInfo->Mood = info.mood;
    }
    // head
    _pComm->downloadUserHeadByStUserCard(arUserInfo);
    // mood
//    if (!getUserMood(&jid, imUserSup->UserMood, imUserSup->UserMoodVersion)) {
//        imUserSup = nullptr;
//        return;
//    }
    // leader
    if (!getUserSupplement(&jid, imUserSup)) {
        // 不存在的员工
    }

    if(imUserSup->MailAddr.empty())
        imUserSup->MailAddr = jid.username() + "@" + NavigationManager::instance().getMailSuffix();

    // 刷新显示mood
    std::string mood = userInfo->Mood;
    std::string id = userInfo->XmppId;
    std::thread([this, id, mood](){
        if(_pComm && _pComm->_pMsgManager)
        {
            _pComm->_pMsgManager->updateMoodRet(id, mood);
        }
    }).detach();
}

// get user mood
bool UserManager::getUserMood(QTalk::Entity::JID *jid, std::string &mood, int &version) {

    info_log("请求mood userId:{0}", jid->username());

    std::ostringstream url;
    url << NavigationManager::instance().getApiUrl()
        << "/get_user_profile"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    cJSON *objs = cJSON_CreateArray();
    cJSON *obj = cJSON_CreateObject();
    cJSON *domain = cJSON_CreateString(jid->domainname().c_str());
    cJSON_AddItemToObject(obj, "domain", domain);
    cJSON *user = cJSON_CreateString(jid->username().c_str());
    cJSON_AddItemToObject(obj, "user", user);
    cJSON *v = cJSON_CreateNumber(version);
    cJSON_AddItemToObject(obj, "version", v);
    cJSON_AddItemToArray(objs, obj);

    std::string postData = QTalk::JSON::cJSON_to_string(objs);
    cJSON_Delete(objs);

    bool retSts = false;
    auto callback = [postData, jid, &retSts, &mood, &version](int code, std::string responsData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responsData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(data, "ret")->valueint;
            if (ret) {
                cJSON *msgList = cJSON_GetObjectItem(data, "data");

                int size = cJSON_GetArraySize(msgList);
                if (size == 1) {
                    cJSON *item = cJSON_GetArrayItem(msgList, 0);
                    mood = cJSON_GetObjectItem(item, "M")->valuestring;
                    version = atoi(cJSON_GetObjectItem(item, "V")->valuestring);

                }
                retSts = true;
                info_log("请求mood success userId:{0} mood:{1}", jid->username(), mood);
            }

            cJSON_Delete(data);
        } else {
            info_log("请求mood error userId:{0} msg: {1}", jid->username(), responsData);
        }
    };

    if (_pComm) {
        HttpRequest req(url.str(), RequestMethod::POST);
        req.header["Content-Type"] = "application/json";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {
            retSts = LogicManager::instance()->getDatabase()->insertOrUpdateUserMood(jid->barename(), mood, version);
        }
    }

    return retSts;
}

// leader and userno
bool
UserManager::getUserSupplement(QTalk::Entity::JID *jid, std::shared_ptr<QTalk::Entity::ImUserSupplement> &imUserSup) {

    info_log("请求leader userId: {0}", jid->username());

    std::ostringstream url;
    url << NavigationManager::instance().getLeaderUrl();

    cJSON *obj = cJSON_CreateObject();
    cJSON *platform = cJSON_CreateString(Platform::instance().getPlatformStr().c_str());
    cJSON_AddItemToObject(obj, "platform", platform);
    cJSON *qtalk_id = cJSON_CreateString(jid->username().c_str());
    cJSON_AddItemToObject(obj, "qtalk_id", qtalk_id);
    cJSON *user_id = cJSON_CreateString(jid->username().c_str());
    cJSON_AddItemToObject(obj, "user_id", user_id);
    cJSON *ckey = cJSON_CreateString(Platform::instance().getClientAuthKey().c_str());
    cJSON_AddItemToObject(obj, "ckey", ckey);

    std::string postData = QTalk::JSON::cJSON_to_string(obj);

    std::cout << Platform::instance().getClientAuthKey() << std::endl;

    cJSON_Delete(obj);
    bool retSts = false;
    auto callback = [postData, jid, &retSts, &imUserSup](int code, std::string responsData) {
        if (code == 200) {
            cJSON *data = cJSON_Parse(responsData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            int errcde = JSON::cJSON_SafeGetIntValue(data, "errcode");
            if (errcde == 0) {
                cJSON *msg = cJSON_GetObjectItem(data, "data");
                imUserSup->LeaderName = JSON::cJSON_SafeGetStringValue(msg, "leader");
                imUserSup->LeaderId = JSON::cJSON_SafeGetStringValue(msg, "qtalk_id");
                imUserSup->MailAddr = jid->username() + "@" + NavigationManager::instance().getMailSuffix();
                imUserSup->UserNo = JSON::cJSON_SafeGetStringValue(msg, "sn");

                info_log("请求mood success userId:{0}", jid->username());
                retSts = true;

            } else {
                std::string errMsg = JSON::cJSON_SafeGetStringValue(data, "msg");
                info_log("请求leader error userId:{0} msg: {1}", jid->username().c_str(), errMsg);
            }

            cJSON_Delete(data);
        } else {
            //LOG_INFO
            //<< "请求leader error userId:" << jid->username().c_str() << "msg: " << responsData;
        }
    };

    if (_pComm) {
        HttpRequest req(url.str(), RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {
            retSts = LogicManager::instance()->getDatabase()->insertOrUpdateUserSuppl(imUserSup);
        }
    }

    return retSts;
}

// get phoneno
bool UserManager::getPhoneNo(const std::string &userId, std::string &phoneNo) {

    info_log("请求PhoneNo userId: {0}", userId);

    QTalk::Entity::JID *jid = new QTalk::Entity::JID(userId.c_str());

    std::ostringstream url;
    url << NavigationManager::instance().getPhoneNumAddr();

    std::string self_id = Platform::instance().getSelfUserId();

    cJSON *obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "platform", Platform::instance().getPlatformStr().c_str());
    cJSON_AddStringToObject(obj, "qtalk_id", jid->username().c_str());
    cJSON_AddStringToObject(obj, "user_id", self_id.data());
    cJSON_AddStringToObject(obj, "ckey", Platform::instance().getClientAuthKey().c_str());

    std::string postData = QTalk::JSON::cJSON_to_string(obj);

    cJSON_Delete(obj);

    bool ret = false;
    std::string errMsg;
    auto callback = [this, postData, jid, &ret, &phoneNo, &errMsg](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            int errcode = JSON::cJSON_SafeGetIntValue(data, "errcode");
            if (errcode == 0) {
                cJSON *msg = cJSON_GetObjectItem(data, "data");
                phoneNo = JSON::cJSON_SafeGetStringValue(msg, "phone");
                ret = true;
            } else {
                errMsg = JSON::cJSON_SafeGetStringValue(data, "msg");
                info_log("getPhoneNo error userId:{0} msg: {1}", jid->username(), errMsg);
            }

            cJSON_Delete(data);
        } else {
            //LOG_INFO << "请求PhoneNo error userId:" << jid->username().c_str() << "msg: " << responseData;
            return;
        }
    };

    if (_pComm) {
		HttpRequest req(url.str(), RequestMethod::POST);
		req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (ret) {
            ret = LogicManager::instance()->getDatabase()->insertOrUpdateUserPhoneNo(jid->barename(), phoneNo);
        }
        else
            phoneNo = errMsg;
    }

    return ret;
}

/**
 * 修改头像接口
 * @param headPath
 */
bool UserManager::changeUserHead(const std::string &headurl)
{

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/profile/set_profile.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();


    cJSON *obj = cJSON_CreateObject();
    cJSON *objs = cJSON_CreateArray();
    cJSON_AddStringToObject(obj, "user", Platform::instance().getSelfUserId().data());
    cJSON_AddStringToObject(obj, "domain", Platform::instance().getSelfDomain().data());
    cJSON_AddStringToObject(obj, "url", headurl.data());
    cJSON_AddItemToArray(objs, obj);
    std::string postData = QTalk::JSON::cJSON_to_string(objs);
    cJSON_Delete(obj);

    bool retSts = false;
    StUserCard userCard = StUserCard();
    auto callback = [postData, headurl, &retSts, &userCard](int code, std::string responsData) {

        if (code == 200) {
            cJSON *dataObj = cJSON_Parse(responsData.c_str());

            if (dataObj == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = QTalk::JSON::cJSON_SafeGetBoolValue(dataObj, "ret");
            if (ret) {
                cJSON *data = cJSON_GetObjectItem(dataObj, "data");

                int size = cJSON_GetArraySize(data);

                for(int i = 0; i < size; i++)
                {
                    cJSON* item = cJSON_GetArrayItem(data, i);
                    userCard.xmppId = QTalk::JSON::cJSON_SafeGetStringValue(item, "user");
                    userCard.xmppId += QTalk::JSON::cJSON_SafeGetStringValue(item, "domain");
                    userCard.version = std::stoi(QTalk::JSON::cJSON_SafeGetStringValue(item, "version"));
                    userCard.headerSrc = QTalk::JSON::cJSON_SafeGetStringValue(item, "url");
                    if(!userCard.headerSrc.empty() && userCard.headerSrc.find("http") != 0){
                        userCard.headerSrc = NavigationManager::instance().getFileHttpHost() + "/" +  userCard.headerSrc;
                    }
                    userCard.mood = QTalk::JSON::cJSON_SafeGetStringValue(item, "mood");
                }
                retSts = true;
            }
            else
            {
                error_log("UpdateMood error {0}", QTalk::JSON::cJSON_SafeGetStringValue(dataObj, "errmsg"));
            }

            cJSON_Delete(dataObj);

            // 旧接口
//            cJSON* data = cJSON_GetObjectItem(dataObj, "data");
//            int size = cJSON_GetArraySize(data);
//
//            for(int i = 0; i < size; i++)
//            {
//                cJSON* item = cJSON_GetArrayItem(data, i);
//
//                userCard.xmppId = QTalk::JSON::cJSON_SafeGetStringValue(item, "user");
//                userCard.xmppId += "@";
//                userCard.xmppId += QTalk::JSON::cJSON_SafeGetStringValue(item, "domain");
//                userCard.version = atoi(QTalk::JSON::cJSON_SafeGetStringValue(item, "version"));
//                userCard.headerSrc = headurl;
//                retSts = true;
//                info_log("changeUserHead success head:{0} ", userCard.headerSrc);
//            }
        }
    };

    if (_pComm) {
        HttpRequest req(url.str(), RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {
            std::vector<StUserCard> arUserInfos;
            arUserInfos.push_back(userCard);
            retSts = LogicManager::instance()->getDatabase()->setUserCardInfo(arUserInfos);
        }
    }

    return retSts;
}

/**
 *
 * @param mood
 * @return
 */
void UserManager::UpdateMood(const std::string &mood)
{
    QTalk::Entity::JID selfId(Platform::instance().getSelfXmppId().data());

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/profile/set_profile.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << selfId.username()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << selfId.domainname();

    //
    cJSON *obj = cJSON_CreateObject();
    cJSON *objs = cJSON_CreateArray();
    cJSON_AddStringToObject(obj, "user", selfId.username().data());
    cJSON_AddStringToObject(obj, "domain", selfId.domainname().data());
    cJSON_AddStringToObject(obj, "mood", mood.data());
    cJSON_AddItemToArray(objs, obj);
    std::string postData = QTalk::JSON::cJSON_to_string(objs);
    cJSON_Delete(obj);

    bool retSts = false;
    std::vector<StUserCard> arUserInfos;
    auto callback = [postData, &retSts, &arUserInfos](int code, const std::string& responsData) {

        if (code == 200) {
            cJSON *dataObj = cJSON_Parse(responsData.c_str());

            if (dataObj == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = QTalk::JSON::cJSON_SafeGetBoolValue(dataObj, "ret");
            if (ret) {
                cJSON *data = cJSON_GetObjectItem(dataObj, "data");

                int size = cJSON_GetArraySize(data);

                for(int i = 0; i < size; i++)
                {
                    cJSON* item = cJSON_GetArrayItem(data, i);

                    StUserCard userCard;
                    userCard.xmppId = QTalk::JSON::cJSON_SafeGetStringValue(item, "user");
                    userCard.xmppId += QTalk::JSON::cJSON_SafeGetStringValue(item, "domain");
                    userCard.version = std::stoi(QTalk::JSON::cJSON_SafeGetStringValue(item, "version"));
                    userCard.headerSrc = QTalk::JSON::cJSON_SafeGetStringValue(item, "url");
                    userCard.mood = QTalk::JSON::cJSON_SafeGetStringValue(item, "mood");
                    arUserInfos.push_back(userCard);
                }
                retSts = true;
            }
            else
            {
                error_log("UpdateMood error {0}", QTalk::JSON::cJSON_SafeGetStringValue(dataObj, "errmsg"));
            }

            cJSON_Delete(dataObj);
        }
    };

    if (_pComm) {
        HttpRequest req(url.str(), RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);

        if (retSts) {

            LogicManager::instance()->getDatabase()->setUserCardInfo(arUserInfos);
            //
            if(_pComm && _pComm->_pMsgManager)
            {
                _pComm->_pMsgManager->updateMoodRet(selfId.barename(), mood);
            }
        }
    }
}
