//
// Created by QITMAC000260 on 2019-01-27.
//

#include "WebService.h"
#include <mutex>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDesktopServices>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QNetworkCookie>
#include "../CustomUi/LiteMessageBox.h"
#include "../Platform/Platform.h"
#ifdef _MACOS
#include <objc/objc-runtime.h>
#endif

WebService* WebService::_service = nullptr;
std::mutex _mutex;
WebService::WebService()
    : QFrame(nullptr)
{
    setMinimumSize(512, 256);
    Qt::WindowFlags flags = Qt::Window | Qt::WindowContextHelpButtonHint | Qt::FramelessWindowHint
#ifdef _WINDOWS
    | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowSystemMenuHint
#endif
    | Qt::WindowFullscreenButtonHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint;
    this->setWindowFlags(flags);
    setAttribute(Qt::WA_TranslucentBackground, true);
    //
    loadQss();
    //
    _pTitleLabel = new QLabel(this);
    _pTitleLabel->setAlignment(Qt::AlignCenter);
    _pCloseBtn = new QToolButton(this);
    //
    _pBackBtn = new QToolButton(this);
    _pForwardBtn = new QToolButton(this);
    _pReloadBtn = new QToolButton(this);
    _pShareBtn = new QToolButton(this);
    _pCopyUrlBtn = new QToolButton(this);
    _pJumpToBrowserBtn = new QToolButton(this);

    _pBackBtn->setObjectName("BackBtn");
    _pForwardBtn->setObjectName("ForwardBtn");
    _pReloadBtn->setObjectName("ReloadBtn");
    _pShareBtn->setObjectName("ShareBtn");
    _pCopyUrlBtn->setObjectName("CopyUrlBtn");
    _pJumpToBrowserBtn->setObjectName("JumpToBrowserBtn");

    _pBackBtn->setToolTip("后退");
    _pForwardBtn->setToolTip("前进");
    _pReloadBtn->setToolTip("重新加载");
    _pShareBtn->setToolTip("分享");
    _pCopyUrlBtn->setToolTip("复制链接");
    _pJumpToBrowserBtn->setToolTip("使用默认浏览器打开");

    _pBackBtn->setFixedSize(24, 24);
    _pForwardBtn->setFixedSize(24, 24);
    _pReloadBtn->setFixedSize(24, 24);
    _pShareBtn->setFixedSize(24, 24);
    _pCopyUrlBtn->setFixedSize(24, 24);
    _pJumpToBrowserBtn->setFixedSize(24, 24);
    //
    _webView = new WebView(this);
    //
    _pAddressEdit = new QLineEdit(this);
    _pAddressEdit->setObjectName("LinkAddressEdit");
    _pAddressEdit->installEventFilter(this);
    //
    _pTitleFrm = new QFrame(this);
    _pTitleFrm->setFixedHeight(40);
    auto * titleLay = new QHBoxLayout(_pTitleFrm);
    titleLay->setContentsMargins(15, 0, 15, 0);
    //
    _toolFrm = new QFrame(this);
    _toolFrm->setFixedHeight(50);
    auto* toolLay = new QHBoxLayout(_toolFrm);
    toolLay->setSpacing(10);
    toolLay->setContentsMargins(25, 0, 25, 0);
    toolLay->addWidget(_pBackBtn);
    toolLay->addWidget(_pForwardBtn);
    toolLay->addWidget(_pReloadBtn);
    toolLay->addWidget(_pAddressEdit);
    toolLay->addWidget(_pShareBtn);
    toolLay->addWidget(_pCopyUrlBtn);
    toolLay->addWidget(_pJumpToBrowserBtn);

    _pShareBtn->setVisible(false);
    //
#ifdef _MACOS
    _pCloseBtn->setFixedSize(12, 12);
    _pCloseBtn->setObjectName("gmCloseBtn");

    titleLay->addWidget(_pCloseBtn);
    titleLay->addWidget(_pTitleLabel);
#else
    _pCloseBtn->setFixedSize(20, 20);
    _pCloseBtn->setObjectName("gwCloseBtn");

    titleLay->addWidget(_pTitleLabel);
    titleLay->addWidget(_pCloseBtn);
#endif
//    setMoverAble(true, titleFrm);
    //
    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("WebServiceMainFrm");
    auto * mainLay = new QVBoxLayout(mainFrm);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(_pTitleFrm);
    mainLay->addWidget(_toolFrm);
    mainLay->addWidget(_webView);
    _webView->setContentsMargins(10, 10, 10, 10);
    mainLay->setStretch(0, 0);
    mainLay->setStretch(1, 1);
    //
    auto * lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->addWidget(mainFrm);

    connect(_pCloseBtn, &QToolButton::clicked, [this](){setVisible(false);});
    connect(_webView->getWebView(), &QWebEngineView::titleChanged, [this](const QString& title){ _pTitleLabel->setText(title); });
    connect(_webView->getWebView(), &QWebEngineView::loadFinished, this, &WebService::onLoadFinish);

    connect(_pReloadBtn, &QToolButton::clicked, _webView->getWebView(), &QWebEngineView::reload);
    connect(_pBackBtn, &QToolButton::clicked, _webView->getWebView(), &QWebEngineView::back);
    connect(_pForwardBtn, &QToolButton::clicked, _webView->getWebView(), &QWebEngineView::forward);

    connect(_pCopyUrlBtn, &QToolButton::clicked, [this](){
        if(_webView)
        {
            QString curUrl = _webView->getWebView()->url().toString();
            auto *mimeData = new QMimeData;
            mimeData->setText(curUrl);
            QApplication::clipboard()->setMimeData(mimeData);
            LiteMessageBox::success("链接已复制", 1000);
        }
    });
    connect(_pJumpToBrowserBtn, &QToolButton::clicked, [this](){
        if(_webView && _webView->getWebView())
        {
            QUrl curUrl = _webView->getWebView()->url();
            QDesktopServices::openUrl(curUrl);
        }
    });

    connect(_pAddressEdit, &QLineEdit::returnPressed, [this](){
        QString strUrl = _pAddressEdit->text();
        if(!strUrl.isEmpty())
        {
            QWebEngineHttpRequest req;
            _service->_pAddressEdit->setText(strUrl);
            req.setUrl(QUrl::fromUserInput(strUrl));
            _webView->startReq(req);
            _webView->setFocus();
        }
    });

    connect(_webView->getWebView(), &QWebEngineView::urlChanged, [this](const QUrl& url){
        _pAddressEdit->setText(url.toString());
        _pAddressEdit->setCursorPosition(0);
    });

    connect(_webView, &WebView::sgCookieAdded, [this](const QNetworkCookie &cookie){
//        qDebug() << cookie;
    });
#ifdef _MACOS
    setWindowFlags(this->windowFlags() | Qt::Tool);
#endif
}

WebService::~WebService()
= default;

void WebService::loadUrl(const QUrl &url, bool showUrl, const MapCookie& cookies) {
    loadUrl(url,"",showUrl,cookies);
}

void WebService::loadUrl(const QUrl &url, const std::string &domain, bool showUrl, const MapCookie &cookies) {
    if (nullptr == _service)
    {
        if(_mutex.try_lock())
        {
            _service = new WebService;
            _mutex.unlock();
        }
    }

    if(nullptr != _service)
    {
        _service->_toolFrm->setVisible(showUrl);
        if(_service->isVisible())
        {
            _service->setVisible(false);
        }

        std::string strCkey = Platform::instance().getClientAuthKey();
        QString agent = QString("startalk/%1/(pc:%2)").arg(Platform::instance().getGlobalVersion().data())
#if defined(_WINDOWS )
                        .arg("windows");
#elif defined(_LINUX)
                        .arg("linux");
#else
                .arg("macos");
#endif

        _service->_webView->clearHistory();
        QWebEngineHttpRequest req;
        _service->_pAddressEdit->setText(url.toString());
        _service->_pAddressEdit->setCursorPosition(0);
        req.setUrl(url);

        auto it = cookies.begin();
        for(; it != cookies.end(); it++)
        {
            QNetworkCookie tmpCookie(it.key().toUtf8(), it.value().toUtf8());
            tmpCookie.setPath("/");
            if(!domain.empty()){
                tmpCookie.setDomain(QString::fromStdString(domain));
            }
            _service->_webView->setCookie(tmpCookie, url);
        }
        QNetworkCookie ckeyCookie("q_ckey", strCkey.data());
        ckeyCookie.setPath("/");
        if(!domain.empty()){
            ckeyCookie.setDomain(QString::fromStdString(domain));
        }
        _service->_webView->setCookie(ckeyCookie, url);
        _service->_webView->setAgent(agent);
        _service->_webView->startReq(req);
        if(_service->isMaximized())
        {
            _service->showMaximized();
        }
        else
        {
            _service->resize(1000, 750);
            _service->setVisible(true);
        }
        _service->activateWindow();
    }
}

void WebService::mouseDoubleClickEvent(QMouseEvent * e)
{
	QPoint pos = e->pos();

	if (e->button() == Qt::LeftButton && _pTitleLabel->geometry().contains(pos))
	{
		if (this->isMaximized())
			this->showNormal();
		else
			this->showMaximized();
	}
	QFrame::mouseDoubleClickEvent(e);
}

/**
 *
 */
void WebService::loadQss()
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

void WebService::onLoadFinish(bool ok)
{

}


void WebService::closeEvent(QCloseEvent *e)
{
    if(_webView)
    {
//        _webView->excuteJs("window.close();");
        QWebEngineHttpRequest req;
        req.setUrl(QUrl());
    }
    QFrame::closeEvent(e);
}

/**
 *
 * @param o
 * @param e
 * @return
 */
bool WebService::event(QEvent *e)
{
    static bool isPress = false;
    static QPoint pos = QPoint();

    if(e->type() == QEvent::MouseButtonRelease)
    {
        isPress = false;
    }
    else if(e->type() == QEvent::MouseButtonPress)
    {
        isPress = true;
        pos = QCursor::pos();
    }
    else if(e->type() == QEvent::MouseMove)
    {
        if(isPress)
        {
            QPoint newPos = QCursor::pos();
            QPoint movePos = newPos - pos;
            pos = newPos;
            move(x() + movePos.x(), y() + movePos.y());
        }
    }
    else if(e->type() == QEvent::Hide)
    {
        QWebEngineHttpRequest req;
        req.setUrl(QUrl());
        _webView->startReq(req);
    }
    return QFrame::event(e);
}
