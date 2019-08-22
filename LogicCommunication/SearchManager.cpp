#include "SearchManager.h"
#include <iostream>
#include <sstream>
#include "../Platform/NavigationManager.h"
#include "../Platform/Platform.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "Communication.h"
#include "../QtUtil/Utils/Log.h"

using namespace QTalk;

SearchManager::SearchManager(Communication *pComm) :
        _pComm(pComm) {

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void SearchManager::GetSearchResult(SearchInfoEvent &e) {

    std::string searchUrl = NavigationManager::instance().getSearchUrl();
    std::string userId = Platform::instance().getSelfXmppId();

    cJSON *gObj = cJSON_CreateObject();
    cJSON_AddNumberToObject(gObj, "start", e.start);
    cJSON_AddNumberToObject(gObj, "length", e.length);
    cJSON_AddStringToObject(gObj, "key", e.key.c_str());
    cJSON_AddStringToObject(gObj, "qtalkId", userId.c_str());
    cJSON_AddNumberToObject(gObj, "action", e.action);
    std::string postData = QTalk::JSON::cJSON_to_string(gObj);
    cJSON_Delete(gObj);

    auto callback = [&e](int code, const std::string &responseData) {

        cJSON* response = cJSON_Parse(responseData.data());
        if(nullptr == response)
        {
            error_log("search error retData:{0}", responseData);
            return;
        }

        if (code == 200) {

            cJSON_bool ret = JSON::cJSON_SafeGetBoolValue(response, "ret");
            if(ret)
            {
                cJSON* data = cJSON_GetObjectItem(response, "data");
                cJSON *item = nullptr;
                //
                cJSON_ArrayForEach(item, data) {

                    cJSON* info = cJSON_GetObjectItem(item, "info");
                    if(nullptr == info) continue;

                    Search::StSearchResult tmpRet = Search::StSearchResult();
                    tmpRet.resultType = JSON::cJSON_SafeGetIntValue(item, "resultType");
                    tmpRet.default_portrait = JSON::cJSON_SafeGetStringValue(item, "defaultportrait");
//                    tmpRet.groupId = JSON::cJSON_SafeGetStringValue(item, "groupId");
                    tmpRet.groupLabel = JSON::cJSON_SafeGetStringValue(item, "groupLabel");
                    tmpRet.hasMore = JSON::cJSON_SafeGetBoolValue(item, "hasMore");

                    if(tmpRet.resultType & Search::EM_ACTION_USER)
                    {
                        cJSON *iitem = nullptr;
                        cJSON_ArrayForEach(iitem, info) {
                            Search::StUserItem tmpItem;
                            tmpItem.xmppId = JSON::cJSON_SafeGetStringValue(iitem, "uri");
                            tmpItem.name = JSON::cJSON_SafeGetStringValue(iitem, "name");
                            tmpItem.tips = JSON::cJSON_SafeGetStringValue(iitem, "label");
                            tmpItem.icon = JSON::cJSON_SafeGetStringValue(iitem, "icon");
                            tmpItem.structure = JSON::cJSON_SafeGetStringValue(iitem, "content");

                            tmpRet._users.push_back(tmpItem);
                        }
                    }
                    else if((tmpRet.resultType & Search::EM_ACTION_MUC)
                        || (tmpRet.resultType & Search::EM_ACTION_COMMON_MUC))
                    {
                        cJSON *iitem = nullptr;
                        cJSON_ArrayForEach(iitem, info) {
                            Search::StGroupItem tmpItem;

                            cJSON* hits = cJSON_GetObjectItem(iitem, "hit");
                            tmpItem.type = JSON::cJSON_SafeGetIntValue(iitem, "todoType");
                            tmpItem.xmppId = JSON::cJSON_SafeGetStringValue(iitem, "uri");
                            tmpItem.name = JSON::cJSON_SafeGetStringValue(iitem, "label");
                            tmpItem.icon = JSON::cJSON_SafeGetStringValue(iitem, "icon");

                            if(nullptr != hits)
                            {
                                cJSON *iiitem = nullptr;
                                cJSON_ArrayForEach(iiitem, hits) {
                                    tmpItem._hits.emplace_back(iiitem->valuestring);
                                }
                            }

                            tmpRet._groups.push_back(tmpItem);
                        }
                    }

                    e.searchRet[tmpRet.resultType] = tmpRet;
                }
            }
        } else {
            std::string errmsg = JSON::cJSON_SafeGetStringValue(response, "errmsg");
            error_log("search error errmsg:{0}", errmsg);
        }

        cJSON_Delete(response);
    };

    if (_pComm) {
        QTalk::HttpRequest req(searchUrl, RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
    }
}
