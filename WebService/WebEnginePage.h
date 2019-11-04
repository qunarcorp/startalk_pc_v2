//
// Created by QITMAC000260 on 2019-02-18.
//

#ifndef QTALK_V2_WEBENGINEPAGE_H
#define QTALK_V2_WEBENGINEPAGE_H

#include <QWebEnginePage>
#include <QWebEngineFullScreenRequest>
#include <QtWebEngineCore>

/**
* @description: WebEnginePage
* @author: cc
* @create: 2019-02-18 20:26
**/
class WebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit WebEnginePage(QObject *parent = nullptr);

    ~WebEnginePage() override;

Q_SIGNALS:
    void sgFullScreen();

private :
    void onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);

    void onFullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);

    //void onQuotaRequested(QWebEngineQuotaRequest quotaRequest);

    void onAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator);

//    void registerProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);

//    void proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator, const QString &proxyHost);

//    void renderProcessTerminated(RenderProcessTerminationStatus terminationStatus, int exitCode);

protected:
    bool certificateError(const QWebEngineCertificateError &certificateError) override;
    QWebEnginePage * createWindow(QWebEnginePage :: WebWindowType ) override ;

};


#endif //QTALK_V2_WEBENGINEPAGE_H
