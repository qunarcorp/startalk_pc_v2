//
// Created by QITMAC000260 on 2019-02-22.
//

#include "AudioVideo.h"
#include <QVBoxLayout>
#include <QFile>
#include <QNetworkCookie>
#include <mutex>
#include <QUrlQuery>
#include <assert.h>
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/Log.h"
#include <QTimer>

#define start2TalkUrl ""
#define groupTalkUrl ""

AudioVideo* AudioVideo::_pAudioVideo = nullptr;
AudioVideo::AudioVideo()
        : QWidget(nullptr)
        , _webView(nullptr)
        , isGroupVideo(false){

    //
    _webView = new WebView(this);

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
            std::string strUrl = url.toString().toStdString();

            //status=hangup
            if(url.hasQuery())
            {
                QUrlQuery query(url);
                QString status = query.queryItemValue("status");
                if(status == "hangup")
                {
                    info_log("hangup");
                    this->close();
                }

//                bool ret = query.queryItemValue("fullscreen").toStdString() == "true";
//                if(ret)
//                    this->showFullScreen();
//                else
//                    this->showNormal();
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
            _webView->excuteJs("qtReady()");
        }
    });

    connect(_webView, &WebView::closeVideo, [this](){
        this->close();
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
void AudioVideo::start2Talk(const std::string &caller, const std::string &callee)
{
    if(nullptr != _pAudioVideo)
    {
        delete _pAudioVideo;
    }
    _pAudioVideo = new AudioVideo;
    _pAudioVideo->isGroupVideo = false;
    std::string userId = Platform::instance().getSelfUserId();
    std::string ckey = Platform::instance().getClientAuthKey();

    QWebEngineHttpRequest req;
    assert(start2TalkUrl != "");
    QUrl url(start2TalkUrl);
    req.setUrl(url);
    _pAudioVideo->_webView->setCookie(QNetworkCookie("_caller", caller.data()), url);
    _pAudioVideo->_webView->setCookie(QNetworkCookie("_callee", callee.data()), url);
    _pAudioVideo->_webView->setCookie(QNetworkCookie("_calltime", QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8()), url);
    _pAudioVideo->_webView->setCookie(QNetworkCookie("u", userId.data()), url);
    _pAudioVideo->_webView->setCookie(QNetworkCookie("q_ckey", ckey.data()), url);

    _pAudioVideo->_webView->startReq(req);
    _pAudioVideo->activateWindow();
    _pAudioVideo->show();
}

void AudioVideo::startGroupTalk()
{
    assert(groupTalkUrl != "");
    QUrl url(groupTalkUrl);
    if (nullptr == _pAudioVideo)
    {
        _pAudioVideo = new AudioVideo;
    }

    if(nullptr != _pAudioVideo)
    {
        if(_pAudioVideo->isVisible())
            _pAudioVideo->setVisible(false);

        std::string strCkey = Platform::instance().getClientAuthKey();
        QString agent = QString("startalk/%1/(pc:%2)").arg(Platform::instance().getGlobalVersion().data())
#if defined(_WINDOWS )
                        .arg("windows");
#elif defined(_LINUX)
                        .arg("linux");
#else
                .arg("macos");
#endif
        _pAudioVideo->_webView->clearHistory();
        _pAudioVideo->isGroupVideo = true;
        QWebEngineHttpRequest req;
        req.setUrl(url);
        _pAudioVideo->_webView->setAgent(agent);
        _pAudioVideo->_webView->startReq(req);
        _pAudioVideo->resize(1000, 750);
        _pAudioVideo->showNormal();
        _pAudioVideo->activateWindow();
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
            _webView->excuteJs("closePeerConn()");
        else
            _webView->excuteJs("this.appController.call_.hangup()");
    }
    QWidget::closeEvent(e);
}

