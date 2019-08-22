#include "MultifunctionFrm.h"
#include "../WebService/WebService.h"
#include "../Platform/Platform.h"
#include "../Platform/NavigationManager.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"

MultifunctionFrm::MultifunctionFrm(QWidget *parent) :
    QFrame(parent)
{
}

MultifunctionFrm::~MultifunctionFrm()
{
}

void MultifunctionFrm::openQiangDan() {
    std::string strUrl = NavigationManager::instance().getQcGrabOrder();
    MapCookie cookies;
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
