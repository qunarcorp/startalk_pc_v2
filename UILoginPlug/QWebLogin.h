//
// Created by lihaibin on 2019-04-04.
//


#ifndef QTALK_V2_QWEBLOGIN_H
#define QTALK_V2_QWEBLOGIN_H


#include <string>
#include "../WebService/WebView.h"
#include "../WebService/webservice_global.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "LoginPanel.h"

class QWebLogin : public QWidget{
public:
    QWebLogin(LoginPanel* loginPanel);
    ~QWebLogin() override;

public:
    static void load(LoginPanel* _pLoginPanel);

private:
    bool getQvtFromCookie(const QNetworkCookie &cookie);

private:
    LoginPanel* _pLoginPanel;
    WebView* webView;
    std::string qcookie;
    std::string vcookie;
    std::string tcookie;
    std::string qvtStr;
    static QWebLogin* _pQWebLogin;
};

#endif //QTALK_V2_QWEBLOGIN_H