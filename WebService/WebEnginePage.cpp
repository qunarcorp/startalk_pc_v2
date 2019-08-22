//
// Created by QITMAC000260 on 2019-02-18.
//

#include "WebEnginePage.h"

#include "../QtUtil/Utils/Log.h"

WebEnginePage::WebEnginePage(QObject *parent)
        : QWebEnginePage(parent) {
    connect(this, &WebEnginePage::featurePermissionRequested, this, &WebEnginePage::onFeaturePermissionRequested);

//    connect(this, &WebEnginePage::quotaRequested, this, &WebEnginePage::onQuotaRequested);

    connect(this, &WebEnginePage::fullScreenRequested, this, &WebEnginePage::onFullScreenRequested);

//    connect(this, &WebEnginePage::authenticationRequired, this, &WebEnginePage::onAuthenticationRequired);


//    void fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest);
//    void quotaRequested(QWebEngineQuotaRequest quotaRequest);
//    void registerProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);

}

WebEnginePage::~WebEnginePage() = default;

void WebEnginePage::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
    if (feature == QWebEnginePage::MediaAudioCapture
        || feature == QWebEnginePage::MediaVideoCapture
        || feature == QWebEnginePage::MediaAudioVideoCapture)
        setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
    else
        setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
}

//void WebEnginePage::onQuotaRequested(QWebEngineQuotaRequest quotaRequest) {
//
//    info_log("onQuotaRequested");
//
//}

//void WebEnginePage::registerProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request) {
//
//}

void WebEnginePage::onFullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest) {
    fullScreenRequest.accept();
    emit sgFullScreen();
    info_log("onFullScreenRequested");
}

bool WebEnginePage::certificateError(const QWebEngineCertificateError &certificateError) {
    return true;
}

QWebEnginePage *WebEnginePage::createWindow(QWebEnginePage::WebWindowType type) {
    // todo 解决 _blank 问题
    return QWebEnginePage::createWindow( type);
}


//void WebEnginePage::onAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator) {
//    info_log("onAuthenticationRequired");
//}

//void WebEnginePage::proxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *authenticator,
//                                                const QString &proxyHost) {
//
//}
//
//void WebEnginePage::renderProcessTerminated(RenderProcessTerminationStatus terminationStatus, int exitCode) {
//
//}