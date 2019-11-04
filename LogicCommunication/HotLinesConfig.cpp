//
// Created by lihaibin on 2019-02-26.
//

#include "HotLinesConfig.h"
#include "Communication.h"
#include "../Platform/NavigationManager.h"
#include "../Platform/Platform.h"
#include "../Platform/dbPlatForm.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include <iostream>

HotLinesConfig::HotLinesConfig(Communication *comm)
        : _pComm(comm) {

}

HotLinesConfig::~HotLinesConfig()
= default;

void HotLinesConfig::getVirtualUserRole(){
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/admin/outer/qtalk/getHotlineList";
    std::string strUrl = url.str();

    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "username", Platform::instance().getSelfName().data());
    cJSON_AddStringToObject(obj, "host", Platform::instance().getSelfDomain().data());
    std::string postDta = QTalk::JSON::cJSON_to_string(obj);
    cJSON_Delete(obj);
    //
    std::set<std::string> hotlines;
    auto callback = [this, &hotlines](int code, const std::string &responseData) {
        if (code == 200) {
            cJSON* json = cJSON_Parse(responseData.c_str());
            if(nullptr == json)
                error_log("error json {0}", responseData);

            cJSON_bool ret = QTalk::JSON::cJSON_SafeGetBoolValue(json, "ret");
            if(ret)
            {
                cJSON* data = cJSON_GetObjectItem(json, "data");
                cJSON* allhotlines = cJSON_GetObjectItem(data, "allhotlines");


                cJSON* item = nullptr;
                cJSON_ArrayForEach(item, allhotlines) {
                    if(item && cJSON_IsString(item))
                        hotlines.insert(item->valuestring);
                }
            }
        }
    };
    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postDta;
        _pComm->addHttpRequest(req, callback);

        if(!hotlines.empty())
            dbPlatForm::instance().setHotLines(hotlines);
    }
}

void HotLinesConfig::getServiceSeat() {
    std::ostringstream url;
    url << NavigationManager::instance().getQcadminHost()
        << "/api/seat/getSeatSeStatusWithSid.qunar?qName="
        << Platform::instance().getSelfUserId();
    std::string strUrl = url.str();

    //
    auto callback = [this](int code, const std::string &responseData) {

        if (code == 200) {
            Platform::instance().setSeats(responseData.c_str());
        }
    };
    std::string qvt = Platform::instance().getQvt();
    if(qvt.empty()){
        return;
    }
    cJSON *qvtJson = cJSON_GetObjectItem(cJSON_Parse(qvt.data()),"data");
    std::string qcookie = cJSON_GetObjectItem(qvtJson,"qcookie")->valuestring;
    std::string vcookie = cJSON_GetObjectItem(qvtJson,"vcookie")->valuestring;
    std::string tcookie = cJSON_GetObjectItem(qvtJson,"tcookie")->valuestring;
    cJSON_Delete(qvtJson);
    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::GET);
        std::string requestHeaders = std::string("_q=") + qcookie + ";_v=" + vcookie + ";_t=" + tcookie;
        req.header["Content-Type"] = "application/json;";
        req.header["Cookie"] = requestHeaders;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::setServiceSeat(int sid, int state) {
    std::ostringstream url;
    url << NavigationManager::instance().getQcadminHost()
        << "/api/seat/upSeatSeStatusWithSid.qunar?qName="
        << Platform::instance().getSelfUserId()
        << "&st="
        << state
        << "&sid="
        << sid;
    std::string strUrl = url.str();

    //
    auto callback = [this](int code, const std::string &responseData) {

        if (code == 200) {

        }
    };
    std::string qvt = Platform::instance().getQvt();
    if(qvt.empty()){
        return;
    }
    cJSON *qvtJson = cJSON_GetObjectItem(cJSON_Parse(qvt.data()),"data");
    std::string qcookie = cJSON_GetObjectItem(qvtJson,"qcookie")->valuestring;
    std::string vcookie = cJSON_GetObjectItem(qvtJson,"vcookie")->valuestring;
    std::string tcookie = cJSON_GetObjectItem(qvtJson,"tcookie")->valuestring;
    cJSON_Delete(qvtJson);
    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::GET);
        std::string requestHeaders = std::string("_q=") + qcookie + ";_v=" + vcookie + ";_t=" + tcookie;
        req.header["Content-Type"] = "application/json;";
        req.header["Cookie"] = requestHeaders;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::serverCloseSession(const std::string username, const std::string seatname,
                                        const std::string virtualname) {
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/admin/api/seat/closeSession.qunar?userName="
        << username
        << "&seatName="
        << seatname
        << "&virtualname="
        << virtualname;
    std::string strUrl = url.str();

    //
    auto callback = [this](int code, const std::string &responseData) {

        if (code == 200) {

        }
    };
    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::GET);
        std::string requestHeaders = std::string("q_ckey=") + Platform::instance().getClientAuthKey();
        req.header["Content-Type"] = "application/json;";
        req.header["Cookie"] = requestHeaders;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::getTransferSeatsList(const QTalk::Entity::UID uid) {
    QTalk::Entity::JID jid(uid.usrId().c_str());
    std::string shopJId = jid.username();
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/admin/seat/getSeatList.json?shopJid="
        << shopJId
        << "&currentCsrName="
        << Platform::instance().getSelfUserId()
        << "&domain="
        << Platform::instance().getSelfDomain();
    std::string strUrl = url.str();

    //
    auto callback = [this,uid](int code, const std::string &responseData) {

        if (code == 200) {
            cJSON* json = cJSON_Parse(responseData.c_str());
            int ret = cJSON_GetObjectItem(json,"ret")->valueint;
            if(ret){
                std::vector<QTalk::Entity::ImTransfer> transfers;
                cJSON* data = cJSON_GetObjectItem(json,"data");
                int size = cJSON_GetArraySize(data);
                for(int i = 0;i<size;i++){
                    cJSON* item = cJSON_GetArrayItem(data,i);
                    std::string webName = QTalk::JSON::cJSON_SafeGetStringValue(item,"webName");
                    std::string nickName = QTalk::JSON::cJSON_SafeGetStringValue(item,"nickName");
                    QTalk::Entity::ImTransfer transfer;
                    transfer.nickName = webName.empty() ? nickName : webName;
                    cJSON* qunarName = cJSON_GetObjectItem(item,"qunarName");
                    std::string node = QTalk::JSON::cJSON_SafeGetStringValue(qunarName,"node");
                    std::string d = QTalk::JSON::cJSON_SafeGetStringValue(qunarName,"domain");
                    transfer.newCsrName = node;
                    transfers.push_back(transfer);
                }
                if(_pComm && _pComm->_pMsgManager){

                    _pComm->_pMsgManager->setSeatList(uid,transfers);
                }
            }
            cJSON_Delete(json);
        }
    };
    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::GET);
        std::string requestHeaders = std::string("q_ckey=") + Platform::instance().getClientAuthKey();
        req.header["Content-Type"] = "application/json;";
        req.header["Cookie"] = requestHeaders;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::transferCsr(const QTalk::Entity::UID uid,
                                 const std::string newCsrName, const std::string reason) {
    std::string shopJId = uid.usrId();
    std::string customerName = uid.realId();
    std::ostringstream url;
    url << NavigationManager::instance().getJavaHost()
        << "/admin/seat/transformCsr.json?shopJid="
        << shopJId
        << "&currentCsrName="
        << Platform::instance().getSelfUserId()
        << "&domain="
        << Platform::instance().getSelfDomain()
        << "&customerName="
        << customerName
        << "&newCsrName="
        << newCsrName
        << "&reason="
        << reason;
    std::string strUrl = url.str();

    //
    auto callback = [this](int code, const std::string &responseData) {

        if (code == 200) {

        }
    };
    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::GET);
        std::string requestHeaders = std::string("q_ckey=") + Platform::instance().getClientAuthKey();
        req.header["Content-Type"] = "application/json;";
        req.header["Cookie"] = requestHeaders;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::updateQuickReply() {
    QInt64 version[2];
    LogicManager::instance()->getDatabase()->getQuickReplyVersion(version);
    QInt64 gversion = version[0];
    QInt64 cversion = version[1];
    // url
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/quickreply/quickreplylist.qunar";

    std::string strUrl = url.str();

    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *username = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(jsonObject, "username", username);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(jsonObject, "host", host);
    cJSON *versiong = cJSON_CreateNumber(gversion);
    cJSON_AddItemToObject(jsonObject, "groupver", versiong);
    cJSON *versionc = cJSON_CreateNumber(cversion);
    cJSON_AddItemToObject(jsonObject, "contentver", versionc);

    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);
    //
    auto callback = [this](int code, const std::string &responseData) {
        if (code == 200) {
            LogicManager::instance()->getDatabase()->batchInsertQuickReply(responseData);
        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::sendProduct(const std::string username, const std::string virtualname,
                                 const std::string product,const std::string type) {
    // url
    std::ostringstream url;
    url << NavigationManager::instance().getQcadminHost()
        << "/api/pdt/sendNoteByJson.qunar";

    std::string strUrl = url.str();

//    cJSON * prod = cJSON_Parse(product.c_str());

    cJSON *jsonObject = cJSON_CreateObject();

    cJSON *args = cJSON_CreateObject();

    cJSON *typeJson = cJSON_CreateString(type.c_str());
    cJSON_AddItemToObject(args, "type", typeJson);
    cJSON *seatQName = cJSON_CreateString(Platform::instance().getSelfUserId().c_str());
    cJSON_AddItemToObject(args, "seatQName", seatQName);
    cJSON *host = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(args, "userHost", host);
    cJSON *userQName = cJSON_CreateString(username.c_str());
    cJSON_AddItemToObject(args, "userQName", userQName);
    cJSON *virtualId = cJSON_CreateString(virtualname.c_str());
    cJSON_AddItemToObject(args, "virtualId", virtualId);
    cJSON *seatHost = cJSON_CreateString(Platform::instance().getSelfDomain().c_str());
    cJSON_AddItemToObject(args, "seatHost", seatHost);

    cJSON_AddItemToObject(jsonObject, "noteArgs", args);
    cJSON *productVo = cJSON_CreateString(product.c_str());
    cJSON_AddItemToObject(jsonObject,"productVO",productVo);

    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    info_log("sendProduct:" + postData);

    auto callback = [this](int code, const std::string &responseData) {
        if (code == 200) {

        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::sendWechat(const QTalk::Entity::UID uid) {
    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/qchat/sendWe.qunar";

    std::string strUrl = url.str();

    cJSON *jsonObject = cJSON_CreateObject();
    cJSON *from = cJSON_CreateString(Platform::instance().getSelfXmppId().c_str());
    cJSON_AddItemToObject(jsonObject,"from",from);
    cJSON *to = cJSON_CreateString(uid.usrId().c_str());
    cJSON_AddItemToObject(jsonObject,"to",to);
    cJSON *realJid = cJSON_CreateString(uid.realId().c_str());
    cJSON_AddItemToObject(jsonObject,"realJid", realJid);
    cJSON *chatType = cJSON_CreateString("5");
    cJSON_AddItemToObject(jsonObject,"chatType",chatType);
    std::string postData = QTalk::JSON::cJSON_to_string(jsonObject);
    cJSON_Delete(jsonObject);

    auto callback = [this](int code, const std::string &responseData) {
        if (code == 200) {

        }
    };

    if (_pComm) {
        QTalk::HttpRequest req(strUrl, QTalk::RequestMethod::POST);
        req.header["Content-Type"] = "application/json;";
        req.body = postData;
        _pComm->addHttpRequest(req, callback);
    }
}

void HotLinesConfig::getHotLineMessageList(const std::string &xmppId) {

    QTalk::Entity::JID jid(xmppId.data());

    std::ostringstream url;
    url << NavigationManager::instance().getHttpHost()
        << "/robot/qtalk_robot/sendtips"
        << "?rexian_id=" << jid.username()
        << "&m_from=" << jid.barename()
        << "&m_to=" << Platform::instance().getSelfXmppId();

    auto callback = [this](int code, const std::string &responseData) {
        if (code == 200) {

        }
    };

    std::string strUrl = url.str();
    if (_pComm) {
        QTalk::HttpRequest req(strUrl);
        _pComm->addHttpRequest(req, callback);
    }
}
