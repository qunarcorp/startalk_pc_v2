//
// Created by lihaibin on 2019-07-17.
//
#include "QchatMoreFun.h"

QchatMoreFun::QchatMoreFun(QTalk::Entity::UID _uid,QTalk::Enum::ChatType _chatType)
        :uid(_uid),chatType(_chatType) {

}

QchatMoreFun::~QchatMoreFun() {

}

void QchatMoreFun::showSuggestProducts() {

    assert(PRODUCT_SUGGEST_URL != "");

    std::ostringstream url;
    url << PRODUCT_SUGGEST_URL
        << "?username="
        << Platform::instance().getSelfUserId()
        << "&rk="
        << Platform::instance().getClientAuthKey()
        << "&domain="
        << Platform::instance().getSelfDomain()
        << "&q_d="
        << Platform::instance().getSelfDomain();

    if(chatType == QTalk::Enum::ConsultServer){
        url << "&qchatid=5&type=consult"
            << "&user_id="
            << uid.usrId()
            << "&realfrom="
            << Platform::instance().getSelfXmppId()
            << "&realto="
            << uid.realId();
    } else if(chatType == QTalk::Enum::GroupChat){
        url << "&group_id="
            << uid.usrId()
            << "&type=groupchat";
    } else{
        url << "&user_id="
            << uid.usrId()
            << "&type=chat";
    }

    std::string strUrl = url.str();
    MapCookie cookies;
    cookies["ckey"] = QString::fromStdString(Platform::instance().getClientAuthKey());
    std::string qvt = Platform::instance().getQvt();
    if(!qvt.empty()){
        cJSON *qvtJson = cJSON_GetObjectItem(cJSON_Parse(qvt.data()),"data");
        std::string qcookie = cJSON_GetObjectItem(qvtJson,"qcookie")->valuestring;
        std::string vcookie = cJSON_GetObjectItem(qvtJson,"vcookie")->valuestring;
        std::string tcookie = cJSON_GetObjectItem(qvtJson,"tcookie")->valuestring;
        cJSON_Delete(qvtJson);
        cookies["_q"] = QString::fromStdString(qcookie);
        cookies["_v"] = QString::fromStdString(vcookie);
        cookies["_t"] = QString::fromStdString(tcookie);
    }
    WebService::loadUrl(QUrl(QString::fromStdString(strUrl)), false, cookies);
}

