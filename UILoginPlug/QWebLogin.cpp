//
// Created by lihaibin on 2019-04-04.
//
#include "QWebLogin.h"
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include "../Platform/Platform.h"
#include <assert.h>

#define QCHAT_LOGIN_URL ""

QWebLogin* QWebLogin::_pQWebLogin = nullptr;
QWebLogin::QWebLogin(LoginPanel* loginPanel)
        : QWidget(nullptr)
        , webView(nullptr)
        , _pLoginPanel(loginPanel){

    //
    webView = new WebView(this);

    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("WebLogin");
    auto * mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(webView);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 1);

    auto * lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(mainFrm);


    resize(500, 400);

    connect(webView, &WebView::sgCookieAdded, [this](const QNetworkCookie &cookie){
        if(qvtStr.empty() && getQvtFromCookie(cookie)){
            _pQWebLogin->close();
            _pLoginPanel->getTokenByQVT(qvtStr, false);
        }
    });

}

bool QWebLogin::getQvtFromCookie(const QNetworkCookie &cookie){
    std::string name = cookie.name().toStdString();
    std::string value = cookie.value().toStdString();
    info_log("cookie:" + name + "-" + value);
    if("_q" == name){
        qcookie = value;
    } else if("_v" == name){
        vcookie = value;
    } else if("_t" == name){
        tcookie = value;
    }
    if(!qcookie.empty() && !vcookie.empty() && !tcookie.empty()){
        cJSON* obj = cJSON_CreateObject();
        cJSON* data = cJSON_AddObjectToObject(obj,"data");
        cJSON_AddStringToObject(data, "qcookie", qcookie.c_str());
        cJSON_AddStringToObject(data, "vcookie", vcookie.c_str());
        cJSON_AddStringToObject(data, "tcookie", tcookie.c_str());
        qvtStr = QTalk::JSON::cJSON_to_string(obj);
//        Platform::instance().setQvt(qvtStr);
        cJSON_Delete(obj);
        return true;
    } else{
        return false;
    }
}

void QWebLogin::load(LoginPanel* loginPanel){

    assert(QCHAT_LOGIN_URL != "");

    if(_pQWebLogin != nullptr){
        delete _pQWebLogin;
    }
    _pQWebLogin = new QWebLogin(loginPanel);

    QWebEngineHttpRequest req;
    QUrl url(QCHAT_LOGIN_URL);
    req.setUrl(url);

    _pQWebLogin->webView->clearCookieAndCache();

    _pQWebLogin->webView->startReq(req);
    _pQWebLogin->activateWindow();
    _pQWebLogin->show();
}

QWebLogin::~QWebLogin() = default;