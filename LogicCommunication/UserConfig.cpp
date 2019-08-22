//
// Created by cc on 18-12-3.
//

#include "UserConfig.h"
#include <iostream>
#include "Communication.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../Platform/Platform.h"
#include "../Platform/dbPlatForm.h"
#include "../Platform/NavigationManager.h"
#include "../QtUtil/Utils/Log.h"

UserConfig::UserConfig(Communication *comm)
        : _pComm(comm) {

}

UserConfig::~UserConfig()
= default;

void UserConfig::getUserConfigFromServer(bool sendEvt) {
    //
    int configVersion = 0;
    LogicManager::instance()->getDatabase()->getConfigVersion(configVersion);
    info_log("get userconfig version:{0}", configVersion);
    // url
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/configuration/getincreclientconfig.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string strUrl = url.str();

    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *username = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "username", username);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "host", host);
    cJSON *version = cJSON_CreateNumber(configVersion);
    cJSON_AddItemToObject(jsonObject, "version", version);

    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    //
    auto callback = [this, sendEvt](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *jsonObj = cJSON_Parse(responseData.c_str());

            if (jsonObj == nullptr) {
                error_log("json paring error"); return;
            }
            updateDbByJson(jsonObj, sendEvt);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
    }
}

//
void UserConfig::updateUserSetting(QUInt8 operatorType, const std::string &key, const std::string &subKey,
                                   const std::string &val) {

    //
    int configVersion = 0;
    LogicManager::instance()->getDatabase()->getConfigVersion(configVersion);
    info_log("get userconfig version:{0}", configVersion);
    //
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/configuration/setclientconfig.qunar";
    std::string strUrl = url.str();
    //
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *username = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "username", username);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "host", host);
    cJSON *resource = cJSON_CreateString(Platform::instance().getSelfResource().c_str());
    cJSON_AddItemToObject(jsonObject, "resource", resource);
    cJSON *version = cJSON_CreateNumber(configVersion);
    cJSON_AddItemToObject(jsonObject, "version", version);
    cJSON *operate_plat = cJSON_CreateString(Platform::instance().getPlatformStr().c_str());
    cJSON_AddItemToObject(jsonObject, "operate_plat", operate_plat);
    cJSON *type = cJSON_CreateNumber(operatorType);
    cJSON_AddItemToObject(jsonObject, "type", type);
    cJSON *jsonkey = cJSON_CreateString(key.c_str());
    cJSON_AddItemToObject(jsonObject, "key", jsonkey);
    cJSON *jsonsubKey = cJSON_CreateString(subKey.c_str());
    cJSON_AddItemToObject(jsonObject, "subkey", jsonsubKey);
    cJSON *jsonVal = cJSON_CreateString(val.c_str());
    cJSON_AddItemToObject(jsonObject, "value", jsonVal);
    //
    std::string body = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    //
    auto callback = [this](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *jsonObj = cJSON_Parse(responseData.c_str());
            if (jsonObj == nullptr) {
                error_log("json paring error"); return;
            }
            updateDbByJson(jsonObj, true);

        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = body;

        _pComm->addHttpRequest(req, callback);
    }

}

void UserConfig::updateDbByJson(cJSON *jsObj, bool sendEvt) {
    std::map<std::string, std::string> deleteData;
    std::vector<QTalk::Entity::ImConfig> arImConfig;

    cJSON_bool ret = QTalk::JSON::cJSON_SafeGetBoolValue(jsObj, "ret");
    bool isMaskName = false;
    if (ret) 
	{
        cJSON *data = cJSON_GetObjectItem(jsObj, "data");
        int ver = QTalk::JSON::cJSON_SafeGetIntValue(data, "version");
        cJSON *configs = cJSON_GetObjectItem(data, "clientConfigInfos");
        int size = cJSON_GetArraySize(configs);
        for (int i = 0; i < size; i++) {
            cJSON *conf = cJSON_GetArrayItem(configs, i);
            std::string key = QTalk::JSON::cJSON_SafeGetStringValue(conf, "key");

            if("kMarkupNames" == key && !isMaskName)
                isMaskName = true;
            //
            cJSON *infos = cJSON_GetObjectItem(conf, "infos");
            int infoSize = cJSON_GetArraySize(infos);
            for (int j = 0; j < infoSize; j++) {
                cJSON *info = cJSON_GetArrayItem(infos, j);
                std::string subKey = QTalk::JSON::cJSON_SafeGetStringValue(info, "subkey");
                int isdel = QTalk::JSON::cJSON_SafeGetIntValue(info, "isdel");
                if (isdel) {
                    deleteData[subKey] = key;
                } else {
                    std::string configValue = QTalk::JSON::cJSON_SafeGetStringValue(info, "configinfo");
                    QTalk::Entity::ImConfig imconf;
                    imconf.ConfigKey = key;
                    imconf.ConfigSubKey = subKey;
                    imconf.ConfigValue = configValue;
                    imconf.Version = ver;

                    arImConfig.push_back(imconf);
                }
            }
        }
        if (ret)
		{
            //
			info_log("update db config-> delete:{0}, insert{1}", deleteData.size(), arImConfig.size());

            if (!deleteData.empty()) {
                LogicManager::instance()->getDatabase()->bulkRemoveConfig(deleteData);
            }
            if (!arImConfig.empty()) {
                LogicManager::instance()->getDatabase()->bulkInsertConfig(arImConfig);
            }
            //
            if(isMaskName)
            {
                std::map<std::string, std::string> mapConf;
                LogicManager::instance()->getDatabase()->getConfig("kMarkupNames", mapConf);
                dbPlatForm::instance().setMaskNames(mapConf);
            }

            //
            if(sendEvt && (!deleteData.empty() || !arImConfig.empty()) && _pComm && _pComm->_pMsgManager)
            {
                _pComm->_pMsgManager->incrementConfigs(deleteData, arImConfig);
            }
//            _pComm->updateUserConfigFromDb();
        }

        cJSON_Delete(jsObj);
        return;
    } else {
        std::string errmsg(cJSON_GetObjectItem(jsObj, "errmsg")->valuestring);
        error_log(errmsg);
        cJSON_Delete(jsObj);
        // todo
    }
}
