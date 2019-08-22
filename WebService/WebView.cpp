//
// Created by QITMAC000260 on 2019-02-13.
//

#include "WebView.h"
#include <QDebug>
#include <QWebEngineSettings>
#include "WebEnginePage.h"
#include "WebJsObj.h"
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QFileDialog>
#include "../QtUtil/Utils/Log.h"
#include <QWebEngineHistory>

WebView::WebView(QWidget* parent)
    : QFrame(parent)
    , _pWebView(nullptr)
    , _pWebPage(nullptr)
{
    _pWebView = new QWebEngineView(this);
	_pWebCannel = new QWebChannel(this);
    _downloadWgt = new DownLoadWgt(this);
	_pWebView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    _pWebPage = new WebEnginePage(this);
    _pWebView->setPage(_pWebPage);
    auto * lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(_pWebView, 1);
    lay->addWidget(_downloadWgt, 0);

    _pWebView->setContextMenuPolicy(Qt::NoContextMenu);
    _downloadWgt->setVisible(false);

    _pWebView->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    _pWebPage->profile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    auto *obj = new WebJsObj(this);
    _pWebCannel->registerObject("client", obj);
    _pWebPage->setWebChannel(_pWebCannel);
    connect(obj, &WebJsObj::runScript, this, &WebView::excuteJs);

    connect(_pWebPage, &WebEnginePage::contentsSizeChanged, [this](const QSizeF &size){


    });

    connect(_pWebPage->profile(), &QWebEngineProfile::downloadRequested, this, &WebView::onDownLoadFile);
    connect(_pWebPage, &WebEnginePage::sgFullScreen, this, &WebView::sgFullScreen);
    connect(_pWebPage, &WebEnginePage::loadFinished, this, &WebView::sgLoadFinished);
    connect(_pWebPage, &WebEnginePage::loadFinished, [this](){
        qreal zoom = _pWebView->zoomFactor();
        if(abs(zoom - 0.8) > 0.00001)
            _pWebView->setZoomFactor(0.8);
    });
    auto cookieStore = _pWebPage->profile()->cookieStore();
    connect(cookieStore, &QWebEngineCookieStore::cookieAdded, this, &WebView::sgCookieAdded);
    connect(cookieStore, &QWebEngineCookieStore::cookieRemoved, this, &WebView::sgCookieRemoved);
    connect(obj, &WebJsObj::sgFullScreen, this, &WebView::sgFullScreen);
    connect(obj, &WebJsObj::closeGroupRoom, this, &WebView::closeVideo);

    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
}

WebView::~WebView() = default;

/**
 *
 * @param url
 */
void WebView::loadUrl(const QUrl &url)
{
    if(nullptr != _pWebView)
    {
        _pWebView->setUrl(url);
    }
}

void WebView::startReq(const QWebEngineHttpRequest& req)
{
    if(nullptr != _pWebView)
    {
        auto cookieStore = _pWebPage->profile()->cookieStore();
        cookieStore->loadAllCookies();
        _pWebView->load(req);
    }
}

void WebView::setCookie(const QNetworkCookie& cookie, const QUrl& hostUrl)
{
    if(_pWebView)
    {
        auto cookieStore = _pWebPage->profile()->cookieStore();
        cookieStore->setCookie(cookie, hostUrl);
    }
}

/**
 * 
 */
void WebView::setAgent(const QString & userAgent)
{
    _pWebPage->profile()->setHttpUserAgent(userAgent);
}

/**
 *
 */
void WebView::excuteJs(const QString &js)
{
    _pWebPage->runJavaScript(js);
    info_log(js.toStdString());
}

void WebView::onDownLoadFile(QWebEngineDownloadItem *download)
{
    QString path = QFileDialog::getSaveFileName(this, tr("另存为"), download->path());
    if (path.isEmpty())
        return;
    download->setPath(path);
    download->accept();

    _downloadWgt->addDownLoaded(download);
}

void WebView::clearHistory()
{
    _pWebView->history()->clear();
}

void WebView::clearCookieAndCache()
{
    if(_pWebView)
    {
        _pWebPage->profile()->clearHttpCache();
        auto cookieStore = _pWebPage->profile()->cookieStore();
        cookieStore->deleteAllCookies();
    }
}