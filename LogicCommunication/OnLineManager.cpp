#include "OnLineManager.h"
#include <iostream>
#include <sstream>
#include "../Platform/NavigationManager.h"
#include "../Platform/Platform.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "Communication.h"
#include "../QtUtil/Utils/Log.h"
//#include "../QtUtil/Enum/im_enum.h"

using namespace QTalk;

OnLineManager::OnLineManager(Communication *pComm) :
        _pComm(pComm) {

}

/**
  * @函数名
  * @功能描述 根据参数中用户是否在线
  * @参数
  * @date 2018.10.12
  */
bool OnLineManager::OnGetOnLineUser(const std::set<std::string> &users, bool sendRet) {

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/domain/get_user_status.qunar"
        << "?v=" << Platform::instance().getClientVersion()
        << "&p=" << Platform::instance().getPlatformStr()
        << "&u=" << Platform::instance().getSelfUserId()
        << "&k=" << Platform::instance().getServerAuthKey()
        << "&d=" << Platform::instance().getSelfDomain();

    std::string postData;
    {
        cJSON *gObj = cJSON_CreateObject();

        cJSON *userAry = cJSON_CreateArray();
        for (const std::string &user : users) {
            cJSON *userObj = cJSON_CreateString(user.c_str());
            cJSON_AddItemToArray(userAry, userObj);
        }
        cJSON_AddItemToObject(gObj, "users", userAry);

        postData = QTalk::JSON::cJSON_to_string(gObj);

        cJSON_Delete(gObj);

    }

    std::map<std::string, std::string> userStatus;

    bool retSts = false;
    auto callback = [users, &retSts, &userStatus](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON *data = cJSON_Parse(responseData.c_str());

            if (data == nullptr) {
                error_log("json paring error"); return;
            }

            int ret = cJSON_GetObjectItem(data, "ret")->valueint;
            if (ret) {
                cJSON *dataObj = cJSON_GetObjectItem(data, "data");
                if (dataObj) {
                    cJSON *userStatusAry = cJSON_GetObjectItem(dataObj, "ul");
                    int size = cJSON_GetArraySize(userStatusAry);

                    for (int i = 0; i < size; i++) {
                        cJSON *item = cJSON_GetArrayItem(userStatusAry, i);

                        std::string struser = cJSON_GetObjectItem(item, "u")->valuestring;
                        std::string strstatus = cJSON_GetObjectItem(item, "o")->valuestring;
                        userStatus[struser] = strstatus;
                    }
                }
                cJSON_Delete(data);
                info_log("got user states, count:{0}", userStatus.size());
                retSts = true;
                return;
            }
            cJSON_Delete(data);
        } else {
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(url.str(), RequestMethod::POST);
        req.body = postData;
        req.header["Content-Type"] = "application/json;";
        _pComm->addHttpRequest(req, callback);
        if (retSts && _pComm && _pComm->_pMsgManager) {

            if (sendRet && !userStatus.empty())
                _pComm->_pMsgManager->sendGotUsersStatus(userStatus);

            Platform::instance().loadOnlineData(userStatus);
        }
    }

    return false;
}
