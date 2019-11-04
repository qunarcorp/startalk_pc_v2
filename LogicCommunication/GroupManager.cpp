#include "GroupManager.h"
#include <iostream>
#include <sstream>
#include "../Platform/NavigationManager.h"
#include "../Platform/Platform.h"
#include "../interface/logic/IDatabasePlug.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../QtUtil/Utils/utils.h"
#include "MessageManager.h"
#include "Communication.h"
#include "../QtUtil/Utils/Log.h"
#include "UserConfig.h"

using namespace QTalk;

GroupManager::GroupManager(Communication *pComm)
        : _pComm(pComm) {

}


GroupManager::~GroupManager() {

}

/**
  * @函数名   getUserGroupInfo
  * @功能描述 拉取群资料
  * @参数
     bool
  * @author   cc
  * @date     2018/09/30
  */
bool GroupManager::getUserGroupInfo(MapGroupCard &mapGroups) {
    // {"u":"dan.liu", "d" : "ejabhost1", "t" : 1537521131996}

    long long originVersion = 0;
    LogicManager::instance()->getDatabase()->getGroupMainVersion(originVersion);

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/muc/get_increment_mucs.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *user = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "u", user);
    cJSON *domain = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "d", domain);
    cJSON *ltime = cJSON_CreateNumber(originVersion);
    cJSON_AddItemToObject(jsonObject, "t", ltime);

    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    std::string strUrl = url.str();
    std::vector<Entity::ImGroupInfo> groups;
    std::vector<std::string> deleteGroups;
    bool restSts = false;
    long long mainVersion = 0;
    auto callback = [strUrl, &mapGroups, &restSts, &groups, &deleteGroups, &mainVersion](int code,
                                                                                         string responseData) {
        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            cJSON_bool ret = JSON::cJSON_SafeGetBoolValue(data, "ret");
            if (ret) {

                std::string vs = JSON::cJSON_SafeGetStringValue(data, "version");
                mainVersion = std::stoll(vs);
                cJSON *msgList = cJSON_GetObjectItem(data, "data");
                int size = cJSON_GetArraySize(msgList);

                for (int i = 0; i < size; i++) {
                    Entity::ImGroupInfo group;
                    cJSON *item = cJSON_GetArrayItem(msgList, i);

                    std::string groupid = JSON::cJSON_SafeGetStringValue(item, "M", "");
                    std::string localdomain = JSON::cJSON_SafeGetStringValue(item, "D");
                    group.GroupId = groupid + "@" + localdomain;

                    int flag = JSON::cJSON_SafeGetIntValue(item, "F");
                    if (flag) {
                        group.LastUpdateTime = 0;
                        mapGroups[localdomain].push_back(group);
                        groups.push_back(group);
                    } else {
                        deleteGroups.push_back(group.GroupId);
                    }
                }
                restSts = true;
                cJSON_Delete(data);

                return;
            }
            cJSON_Delete(data);
        } else {
            warn_log("请求失败  url: {0}", strUrl);
        }
    };

    if (_pComm) {
        HttpRequest req(strUrl, RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
        if (restSts && !groups.empty()) {
            restSts = LogicManager::instance()->getDatabase()->bulkInsertGroupInfo(groups);
        }
        if (restSts && !deleteGroups.empty()) {
            restSts = LogicManager::instance()->getDatabase()->bulkDeleteGroup(deleteGroups);
            // all top
            std::map<std::string, std::string> allTop;
            LogicManager::instance()->getDatabase()->getConfig("kStickJidDic", allTop);
			// 删除置顶群
 			for (const auto& groupId : deleteGroups)
 			{
                std::string uid = QTalk::Entity::UID(groupId).toStdString();
                if(allTop.find(uid) != allTop.end())
                {
                    _pComm->_pUserConfig->updateUserSetting(UserSettingMsg::EM_OPERATOR_CANCEL,
                                                            "kStickJidDic",
                                                            uid,
                                                            "{\"topType\":0,\"chatType\":1}");
                }
 			}
        }

        if (restSts && mainVersion != originVersion) {
            restSts = LogicManager::instance()->getDatabase()->setGroupMainVersion(mainVersion);
        }

    }

    return restSts;
}

/**
  * @函数名   getGroupCard
  * @功能描述 获取群名片
  * @参数
     bool
  * @author   cc
  * @date     2018/09/30
  */
bool GroupManager::getGroupCard(const MapGroupCard &groups) {
    //[{"mucs":[{"version":2, "muc_name" : "de07fb3c853c4ba68f402431ae65d5de@conference.ejabhost1"}], "domain" : "ejabhost1"}]

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/muc/get_muc_vcard.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    cJSON *objs = cJSON_CreateArray();
    auto itObj = groups.cbegin();
    for (; itObj != groups.cend(); itObj++) {
        cJSON *obj = cJSON_CreateObject();

        cJSON *domain = cJSON_CreateString(itObj->first.c_str());
        cJSON_AddItemToObject(obj, "domain", domain);

        cJSON *group = cJSON_CreateArray();
        for (const auto & itG : itObj->second) {
            cJSON *g = cJSON_CreateObject();

            cJSON *muc_name = cJSON_CreateString(itG.GroupId.c_str());
            cJSON_AddItemToObject(g, "muc_name", muc_name);

            cJSON *version = cJSON_CreateNumber(0);
            cJSON_AddItemToObject(g, "version", version);

            cJSON_AddItemToArray(group, g);
        }

        cJSON_AddItemToObject(obj, "mucs", group);
        cJSON_AddItemToArray(objs, obj);
    }
    std::string postData = QTalk::JSON::cJSON_to_string(objs);
    cJSON_Delete(objs);

    bool retSts = false;
    std::string strUrl = url.str();
    std::vector<Entity::ImGroupInfo> arGroups;
    auto callback = [strUrl, &retSts, &arGroups](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());
            if (data == nullptr) {
                warn_log("parsing json error.{0}", strUrl);
                return;
            }

            cJSON_bool ret = QTalk::JSON::cJSON_SafeGetBoolValue(data, "ret");
            if (ret) {
                cJSON *jsonGroups = cJSON_GetObjectItem(data, "data");
                int size = cJSON_GetArraySize(jsonGroups);

                for (int i = 0; i < size; i++) {
                    cJSON *item = cJSON_GetArrayItem(jsonGroups, i);

                    cJSON *mucs = cJSON_GetObjectItem(item, "mucs");
                    int gSize = cJSON_GetArraySize(mucs);

                    for (int j = 0; j < gSize; j++) {
                        Entity::ImGroupInfo group;
                        cJSON *gobj = cJSON_GetArrayItem(mucs, j);

                        group.GroupId = JSON::cJSON_SafeGetStringValue(gobj, "MN");
                        group.Name = JSON::cJSON_SafeGetStringValue(gobj, "SN");
                        group.Introduce = JSON::cJSON_SafeGetStringValue(gobj, "MD");
                        group.Topic = JSON::cJSON_SafeGetStringValue(gobj, "MT");
                        group.HeaderSrc = JSON::cJSON_SafeGetStringValue(gobj, "MP");
                        if(!group.HeaderSrc.empty() && group.HeaderSrc.find("http") != 0){
                            group.HeaderSrc = NavigationManager::instance().getFileHttpHost() + "/" +  group.HeaderSrc;
                        }
                        group.LastUpdateTime = std::strtoll(JSON::cJSON_SafeGetStringValue(gobj, "UT"), nullptr, 0);
                        arGroups.push_back(group);
                    }
                }

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
            retSts = LogicManager::instance()->getDatabase()->updateGroupCard(arGroups);
        }
    }

    return retSts;
}

/**
 * 更新群资料
 */
void GroupManager::upateGroupInfo(const std::vector<QTalk::StGroupInfo> &groupInfos) {
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/muc/set_muc_vcard.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    cJSON *objs = cJSON_CreateArray();
    auto itObj = groupInfos.cbegin();
    for (; itObj != groupInfos.cend(); itObj++) {
        cJSON *obj = cJSON_CreateObject();

        cJSON *desc = cJSON_CreateString(itObj->desc.c_str());
        cJSON_AddItemToObject(obj, "desc", desc);

        cJSON *muc_name = cJSON_CreateString(itObj->groupId.c_str());
        cJSON_AddItemToObject(obj, "muc_name", muc_name);

        cJSON *nick = cJSON_CreateString(itObj->name.c_str());
        cJSON_AddItemToObject(obj, "nick", nick);
        cJSON *title = cJSON_CreateString(itObj->title.c_str());
        cJSON_AddItemToObject(obj, "title", title);

        cJSON_AddItemToArray(objs, obj);
    }
    std::string postData = QTalk::JSON::cJSON_to_string(objs);
    cJSON_Delete(objs);
    //
    std::string strUrl = url.str();
    std::vector<Entity::ImGroupInfo> groups;
    auto callback = [strUrl, &groups](int code, const string &data) {

        if (code == 200) {

            cJSON *resData = cJSON_Parse(data.c_str());
            if (nullptr == resData) {
                error_log("upateGroupInfo json error {0}", data.data());
                return;
            }
            int ret = cJSON_GetObjectItem(resData, "ret")->valueint;
            if (ret) {
                cJSON *data = cJSON_GetObjectItem(resData, "data");
                int size = cJSON_GetArraySize(data);
                for (int i = 0; i < size; i++) {
                    Entity::ImGroupInfo groupInfo;
                    cJSON *item = cJSON_GetArrayItem(data, i);
                    groupInfo.GroupId = cJSON_GetObjectItem(item, "muc_name")->valuestring;
                    groupInfo.Name = cJSON_GetObjectItem(item, "show_name")->valuestring;
                    groupInfo.Introduce = cJSON_GetObjectItem(item, "muc_desc")->valuestring;
                    groupInfo.Topic = cJSON_GetObjectItem(item, "muc_title")->valuestring;

                    groups.push_back(groupInfo);
                }
            } else {
                if (cJSON_HasObjectItem(resData, "errmsg")) {
                    char *msg = cJSON_GetObjectItem(resData, "errmsg")->valuestring;
                    info_log(msg);
                }
            }
            cJSON_Delete(resData);
            info_log("update group info success {0}", data);
        } else {
            info_log("请求失败  url:{0}", strUrl);
            info_log("update group info error");
        }
    };

    if (_pComm) {
        HttpRequest req(strUrl, RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
        if (!groups.empty()) {
            // 更新ui
            if (_pComm && _pComm->_pMsgManager) {
                auto it = groups.begin();
                for (; it != groups.end(); it++) {
                    std::shared_ptr<QTalk::StGroupInfo> info(new QTalk::StGroupInfo);
                    info->groupId = it->GroupId;
                    info->name = it->Name;
                    info->desc = it->Introduce;
                    info->title = it->Topic;
                    _pComm->_pMsgManager->onUpdateGroupInfo(info);
                }
            }
//            更新群资料需要调接口 否则时间戳不能及时更新
            MapGroupCard params;
            for(const auto& it : groups)
            {
                QTalk::Entity::JID jid(it.GroupId.data());
                params[jid.domainname()].push_back(it);
            }
            getGroupCard(params);
//            LogicManager::instance()->GetDatabase()->updateGroupCard(groups);
        }
    }
}

/**
 * 增量获取群卡片
 */
void GroupManager::getUserIncrementMucVcard()
{
    try {
        //
        long long maxGroupCardVersion = 0;
        LogicManager::instance()->getDatabase()->getGroupCardMaxVersion(maxGroupCardVersion);
        //
        std::ostringstream url;
        url << NavigationManager::instance().getHttpHost()
            << "/muc/get_user_increment_muc_vcard.qunar"
            << "?v=" << Platform::instance().getClientVersion()
            << "&p=" << Platform::instance().getPlatformStr()
            << "&u=" << Platform::instance().getSelfUserId()
            << "&k=" << Platform::instance().getServerAuthKey()
            << "&d=" << Platform::instance().getSelfDomain();

        cJSON *obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "userid", Platform::instance().getSelfUserId().data());
        cJSON_AddStringToObject(obj, "lastupdtime", std::to_string(maxGroupCardVersion).data());
        std::string postData = JSON::cJSON_to_string(obj);
        cJSON_Delete(obj);
        //
        bool retSts = false;
        std::string strUrl = url.str();
        std::vector<Entity::ImGroupInfo> arGroups;
        auto callback = [strUrl, &retSts, &arGroups](int code, const std::string &responseData) {

            if (code == 200) {
                cJSON *data = cJSON_Parse(responseData.c_str());
                if (data == nullptr) {
                    warn_log("parsing json error.{0}", strUrl);
                    return;
                }

                cJSON_bool ret = QTalk::JSON::cJSON_SafeGetBoolValue(data, "ret");
                if (ret) {
                    cJSON *jsonGroups = cJSON_GetObjectItem(data, "data");
                    int size = cJSON_GetArraySize(jsonGroups);

                    for (int i = 0; i < size; i++) {
                        cJSON *item = cJSON_GetArrayItem(jsonGroups, i);

                        Entity::ImGroupInfo group;
                        group.GroupId = JSON::cJSON_SafeGetStringValue(item, "MN");
                        group.Name = JSON::cJSON_SafeGetStringValue(item, "SN");
                        group.Introduce = JSON::cJSON_SafeGetStringValue(item, "MD");
                        group.Topic = JSON::cJSON_SafeGetStringValue(item, "MT");
                        group.HeaderSrc = JSON::cJSON_SafeGetStringValue(item, "MP");
                        if(!group.HeaderSrc.empty() && group.HeaderSrc.find("http") != 0){
                            group.HeaderSrc = NavigationManager::instance().getFileHttpHost() + "/" +  group.HeaderSrc;
                        }
                        group.LastUpdateTime = std::strtoll(JSON::cJSON_SafeGetStringValue(item, "UT"), nullptr, 0);
                        arGroups.push_back(group);
                    }

                    retSts = true;
                }
                cJSON_Delete(data);
            } else {
                info_log("请求失败  url:{0}", strUrl);
            }

        };

        if (_pComm) {
            HttpRequest req(strUrl, RequestMethod::POST);
            req.header["Content-Type"] = "application/json;";
            req.body = postData;
            _pComm->addHttpRequest(req, callback);
            if (retSts) {
                LogicManager::instance()->getDatabase()->updateGroupCard(arGroups);
            }
        }
    }
    catch (const std::exception& e)
    {
        error_log("exception {0}", e.what());
    }
}
