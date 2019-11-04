//
// Created by QITMAC000260 on 2019-02-22.
//

#include "AudioVideo.h"
#include <QVBoxLayout>
#include <QFile>
#include <QNetworkCookie>
#include <mutex>
#include <QUrlQuery>
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/Log.h"
#include "WebJsObj.h"
#include <QMessageBox>
#include <QCloseEvent>
#include "../Platform/dbPlatForm.h"
#include "../Platform/NavigationManager.h"
#include <QTimer>

std::function<void(const std::string&, const std::string&)> _sendMsgFunc;

AudioVideo::AudioVideo()
        : QWidget(nullptr)
        , _webView(nullptr)
        , isGroupVideo(false){

    //
    _webView = new WebView(this);
    obj = new WebJsObj(this);
    _webView->setObj(obj);
    setAttribute(Qt::WA_QuitOnClose, false);
    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("WebServiceMainFrm");
    auto * mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(_webView);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 1);

    auto * lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    //setMoverAble(true);
    loadQss();

    resize(768, 512);
    setAttribute(Qt::WA_QuitOnClose, false);
    connect(_webView->getWebView(), &QWebEngineView::urlChanged, [this](const QUrl& url){

        if(url.isValid())
        {
            QString strUrl = url.toString();
            //status=hangup
            auto querys = strUrl.section("?", 1, 1);
            QUrlQuery query(querys);
            if(!query.isEmpty())
            {
                QString status = query.queryItemValue("status");
                if(status == "hangup")
                {
                    info_log("hangup");
                    this->close();
                }
            }
        }

    });

    connect(_webView, &WebView::sgFullScreen, [this](){
        if(this->isFullScreen())
            this->showNormal();
        else
            this->showFullScreen();
    });

    connect(_webView, &WebView::sgLoadFinished, [this](){
        if(isGroupVideo)
        {
//            _webView->excuteJs("qtReady()");
        } else {

            if(_isCaller)
            {
                QString json = "{\"type\":\"create\"}";
                emit obj->sgSendSignal(json, _conversationId);
            }

            if(!_peerId.isEmpty())
            {
                auto info = dbPlatForm::instance().getUserInfo(_peerId.toStdString());
                if(info)
                {
                    QString name = QTalk::getUserName(info).data();
                    QString js = QString("sgsetUserInfo('%1','%2','%3');").arg(_peerId, name, info->HeaderSrc.data());
                    _webView->excuteJs(js);
                }
            }

            QString js;
            if(_isCaller)
            {
                if(_isVideo)
                    js = QString("startVideo('%1', '%2')").arg(_json).arg(_conversationId);
                else
                    js = QString("startAudio('%1', '%2')").arg(_json).arg(_conversationId);
            }
            else
            {
                if(_isVideo)
                    js = QString("receiveRtc('%1', '%2')").arg(_json).arg(_conversationId);
                else
                    js = QString("receiveRtcAudio('%1', '%2')").arg(_json).arg(_conversationId);
            }

            _webView->excuteJs(js);
        }
    });

    connect(_webView, &WebView::closeVideo, [this](){
        this->close();
    });

    connect(obj, &WebJsObj::sgCloseRtcWindow, [this](const QString& id, long long occupied_time){
        if(id == _conversationId)
        {
            emit sgClose2AudioVideo(occupied_time);
        }
    });
}

AudioVideo::~AudioVideo() = default;

/**
 *
 */
void AudioVideo::loadQss()
{
    Q_INIT_RESOURCE(WebService);
    QString styleSheetPath = QString(":/web/style1/WebService.qss");
    if (QFile::exists(styleSheetPath)) {
        QFile file(styleSheetPath);
        if (file.open(QFile::ReadOnly)) {
            QString qss = QString::fromUtf8(file.readAll()); //以utf-8形式读取文件
            this->setStyleSheet(qss.toUtf8());
            file.close();
        }
    }
}

/**
 *
 */
void AudioVideo::closeWeb()
{
    if(_webView)
    {
        delete _webView;
        _webView = nullptr;
    }
}

void AudioVideo::closeEvent(QCloseEvent *e)
{
    if(_webView)
    {
        if(isGroupVideo)
        {
            _webView->excuteJs("closePeerConn()");
            emit closeAudioVideo();
        }
        else
        {
            QString payload= "{\"type\":\"close\"}";
            emit obj->sgSendSignal(payload, _conversationId);
//            emit closeAudioVideo();
            _webView->excuteJs(QString("receiveSignal('%1')").arg(payload));
            e->ignore();
        }
    }

    QWidget::closeEvent(e);
}



AudioVideoManager::~AudioVideoManager() {
    for(auto *t : _2Talks)
        delete t;

    for(auto *t : _groupTalks)
        delete t;
}

QString get2TalkKey(const QString &peerId)
{
    QString key;
    QString self = Platform::instance().getSelfUserId().data();
    if(self < peerId)
        key.append(self).append(peerId);
    else
        key.append(peerId).append(self);

    return key;
}

QString get2TalkKey(const QString &caller, const QString& callee)
{
    QString key;
    if(caller < callee)
        key.append(caller).append(callee);
    else
        key.append(callee).append(caller);

    return key;
}

void AudioVideoManager::start2Talk_old(const QString& json, const std::string& peerId, bool isVideo, bool isCall) {

    if(isBusy())
    {
        if(isCall)
            QMessageBox::information(nullptr, tr("提醒"), tr("当前正在通话中..."));
        else
            emit sgSendSignal("{\"type\":\"busy\"}", peerId.data());
        return;
    }

    QString key = get2TalkKey(QString(peerId.data()).section("@", 0, 0));

//    if(_2Talks.contains(key) && _2Talks[key])
//    {
//        _2Talks[key]->activateWindow();
//        _2Talks[key]->show();
//        return;
//    }

    auto * pAudioVideo = new AudioVideo;
    _2Talks[key] = pAudioVideo;
    pAudioVideo->isGroupVideo = false;
    pAudioVideo->_json = json;
    pAudioVideo->_isCaller = isCall;
    pAudioVideo->_isVideo = isVideo;
    pAudioVideo->_peerId = peerId.data();
    pAudioVideo->_conversationId = QString::number(QDateTime::currentMSecsSinceEpoch());
    std::string userId = Platform::instance().getSelfUserId();
    QWebEngineHttpRequest req;
    QString strUrl = QString("%1/single?ver=new&plat=0").arg(NavigationManager::instance().getVideoUrl().data());
    QUrl url(strUrl);
    req.setUrl(url);
    pAudioVideo->_webView->startReq(req);
    pAudioVideo->activateWindow();
    pAudioVideo->show();

    connect(pAudioVideo, &AudioVideo::sgClose2AudioVideo, [this, key, pAudioVideo, peerId](long long occupied_time){
        if(_2Talks.contains(key))
        {
            _2Talks.remove(key);
            if(pAudioVideo->_isCaller && occupied_time >= 0)
                emit sgClose2AudioVideo(QString(peerId.data()), pAudioVideo->_isVideo, occupied_time);
            pAudioVideo->deleteLater();
        }
    });

    connect(pAudioVideo->obj, &WebJsObj::sgSendSignal, [this, pAudioVideo, peerId](const QString& json, const QString& id){
        if(id == pAudioVideo->_conversationId)
            emit sgSendSignal(json, peerId.data());
    });
}

void AudioVideoManager::onRecvWebRtcCommand(const QString &peerId, const QString &cmd) {

    QString key = get2TalkKey(peerId.section("@", 0, 0));

    if(_2Talks.contains(key) && _2Talks[key])
    {
//        emit pAudioVideo->sgRecvCommand(peerId, cmd);
        QString Json = cmd;
        QString strJs = QString("receiveSignal('%1')").arg(Json.replace("&quot;", "\""));
        _2Talks[key]->_webView->excuteJs(strJs);
    }
    
}


/**
 * // invalid method
 */
void AudioVideoManager::start2Talk(const std::string &caller, const std::string &callee)
{
    QString key = get2TalkKey(caller.data(), callee.data());

    if(_2Talks.contains(key) && _2Talks[key])
    {
        _2Talks[key]->activateWindow();
        _2Talks[key]->show();
        return;
    }

    auto * pAudioVideo = new AudioVideo;
    _2Talks[key] = pAudioVideo;
    pAudioVideo->isGroupVideo = false;
    std::string userId = Platform::instance().getSelfUserId();
    std::string ckey = Platform::instance().getClientAuthKey();

    QWebEngineHttpRequest req;
    // invalid method
    QUrl url("");
    req.setUrl(url);
    pAudioVideo->_webView->setCookie(QNetworkCookie("_caller", caller.data()), url);
    pAudioVideo->_webView->setCookie(QNetworkCookie("_callee", callee.data()), url);
    pAudioVideo->_webView->setCookie(QNetworkCookie("_calltime", QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8()), url);
    pAudioVideo->_webView->setCookie(QNetworkCookie("u", userId.data()), url);
    pAudioVideo->_webView->setCookie(QNetworkCookie("q_ckey", ckey.data()), url);

    pAudioVideo->_webView->startReq(req);
    pAudioVideo->activateWindow();
    pAudioVideo->show();

    connect(pAudioVideo, &AudioVideo::sgClose2AudioVideo, [this, key, pAudioVideo](long long){
        _2Talks.remove(key);
        pAudioVideo->deleteLater();
    });
}

void AudioVideoManager::startGroupTalk(const QString& id, const QString &name)
{
    if(isBusy())
    {
        QMessageBox::information(nullptr, tr("提醒"), tr("当前正在通话中..."));
        return;
    }

//    if(_groupTalks.contains(id) && _groupTalks[id])
//    {
//        _groupTalks[id]->activateWindow();
//        _groupTalks[id]->show();
//        return;
//    }

    auto * pAudioVideo = new AudioVideo;
    _groupTalks[id] = pAudioVideo;

    std::string strCkey = Platform::instance().getClientAuthKey();
    QString agent = QString("startalk/%1/(pc:%2)").arg(Platform::instance().getGlobalVersion().data())
#if defined(_WINDOWS )
                    .arg("windows");
#elif defined(_LINUX)
                    .arg("linux");
#else
            .arg("macos");
#endif
    pAudioVideo->_webView->clearHistory();
    pAudioVideo->isGroupVideo = true;
    QWebEngineHttpRequest req;

    QString url = QString("%4/conference#/login?userId=%1&roomId=%2&topic=%3")
            .arg(Platform::instance().getSelfXmppId().data())
            .arg(id)
            .arg(QUrl(name.toUtf8()).toEncoded().data())
            .arg(NavigationManager::instance().getVideoUrl().data());
    qInfo() << url;
    req.setUrl(QUrl(url));

    std::string ckey = Platform::instance().getClientAuthKey();
    if(ckey.empty())
    {
        QMessageBox::information(nullptr, tr("警告"), tr("获取群信息失败, 请重试!"));
        return;
    }
    qInfo() << "AudioVideoManager cookie ckey" << ckey.data();
    QNetworkCookie ckeyCookie("q_ckey", QByteArray(ckey.data(), ckey.size()));
//    ckeyCookie.setPath("/");
    pAudioVideo->_webView->setCookie(ckeyCookie, url);
//    pAudioVideo->_webView->setCookie(QNetworkCookie("userId", Platform::instance().getSelfXmppId().data()), url);
//    pAudioVideo->_webView->setCookie(QNetworkCookie("nick", QUrl(Platform::instance().getSelfName().data()).toEncoded()), url);
//    pAudioVideo->_webView->setCookie(QNetworkCookie("roomId", id.toUtf8()), url);
//    pAudioVideo->_webView->setCookie(QNetworkCookie("startTime", QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8()), url);
//    pAudioVideo->_webView->setCookie(QNetworkCookie("topic", QUrl(name.toUtf8()).toEncoded()), url);
    pAudioVideo->_webView->setAgent(agent);
    pAudioVideo->_webView->startReq(req);
    pAudioVideo->resize(1000, 750);
    pAudioVideo->showNormal();
    pAudioVideo->activateWindow();
    
    connect(pAudioVideo, &AudioVideo::closeAudioVideo, [this, id, pAudioVideo](){
        _groupTalks.remove(id);
        pAudioVideo->deleteLater();
    });
}

bool AudioVideoManager::isCall(const std::string& peerId) {
    QString key = get2TalkKey(QString(peerId.data()).section("@", 0, 0));
    if(_2Talks.contains(key))
        return _2Talks[key]->_isCaller;
    else
        return false;
}

/**
 *
 * @param peerId
 */
void AudioVideoManager::onCloseAudioVideo(const std::string &peerId) {

    QString key = get2TalkKey(QString(peerId.data()).section("@", 0, 0));
    if(_2Talks.contains(key))
    {
        emit _2Talks[key]->sgClose2AudioVideo(0);
    }
    else if(_groupTalks.contains(key))
    {
        emit _2Talks[key]->closeAudioVideo();
    }
}

bool AudioVideoManager::isVideo(const std::string &peerId) {
    QString key = get2TalkKey(QString(peerId.data()).section("@", 0, 0));
    if(_2Talks.contains(key))
        return _2Talks[key]->_isVideo;
    else
        return false;
}

//
bool AudioVideoManager::isBusy() {
    return !_2Talks.empty() || !_groupTalks.empty();
}