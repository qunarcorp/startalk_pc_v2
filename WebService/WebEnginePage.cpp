//
// Created by QITMAC000260 on 2019-02-18.
//

#include "WebEnginePage.h"
#include <QMessageBox>
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

inline QString questionForFeature(QWebEnginePage::Feature feature)
{
    switch (feature) {
//        case QWebEnginePage::Geolocation:
//            return (tr("允许 %1 访问您的位置信息？"));
//        case QWebEnginePage::MediaAudioCapture:
//            return (tr("允许 %1 访问您的麦克风？"));
//        case QWebEnginePage::MediaVideoCapture:
//            return (tr("允许 %1 访问您的摄像头？"));
//        case QWebEnginePage::MediaAudioVideoCapture:
//            return (tr("允许 %1 访问您的麦克风和网络摄像头？"));
//        case QWebEnginePage::MouseLock:
//            return (tr("允许 %1 锁定鼠标光标？"));
//        case QWebEnginePage::DesktopVideoCapture:
//            return (tr("允许 %1 捕获桌面的视频？"));
//        case QWebEnginePage::DesktopAudioVideoCapture:
//            return (tr("允许 %1 捕获桌面的音频和视频？"));
        case QWebEnginePage::Notifications:
            return QString();
    }
    return QString();
}

void WebEnginePage::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
//    QString title = tr("权限申请");
//    QString question = questionForFeature(feature).arg(securityOrigin.host());
//    if (!question.isEmpty() && QMessageBox::question(view()->window(), title, question) == QMessageBox::Yes)
//        setFeaturePermission(securityOrigin, feature, PermissionGrantedByUser);
//    else
//        setFeaturePermission(securityOrigin, feature, PermissionDeniedByUser);
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