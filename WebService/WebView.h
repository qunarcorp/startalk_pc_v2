//
// Created by QITMAC000260 on 2019-02-13.
//

#ifndef QTALK_V2_WEBVIEW_H
#define QTALK_V2_WEBVIEW_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QFrame>
#include <QWebEngineView>
#include <QWebChannel>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDesktopServices>
#include "../include/Line.h"

class DownLoadWgt : public QWidget
{
    Q_OBJECT
public:
    explicit DownLoadWgt(QWidget* parent = nullptr)
        :_download(nullptr)
    {
        _pProcessBar = new QProgressBar(this);
        _pCancelBtn = new QPushButton(tr("取消"), this);
        _pOpenDir = new QPushButton(tr("打开文件夹"), this);
        _closeBtn = new QPushButton(tr("关闭"), this);
        _pUrlLabel = new QLabel(this);
        auto* titLay = new QHBoxLayout();
        titLay->addWidget(_pUrlLabel, 1);
        titLay->addWidget(_pCancelBtn, 0);
        titLay->addWidget(_pOpenDir, 0);
        titLay->addWidget(_closeBtn, 0);

        auto* lay = new QVBoxLayout(this);
        lay->addWidget(new Line());
        lay->addLayout(titLay);
        lay->addWidget(_pProcessBar);

        _pProcessBar->setMaximumHeight(18);
        _pProcessBar->setStyleSheet(QStringLiteral("QProgressBar {border: 0px; text-align:center; font-size: 12px;} QProgressBar::chunk {background-color: lightgreen}"));

        connect(_pCancelBtn, &QPushButton::clicked, this, &DownLoadWgt::onCancelClicked);
        connect(_pOpenDir, &QPushButton::clicked, this, &DownLoadWgt::openDir);
        connect(_closeBtn, &QPushButton::clicked, this, &DownLoadWgt::onCloseClicked);
    }

public:
    void addDownLoaded(QWebEngineDownloadItem *download)
    {
        _download = download;
        _pUrlLabel->setText(download->url().toDisplayString());
        _pCancelBtn->setVisible(true);
        _pOpenDir->setVisible(true);
        _closeBtn->setVisible(false);

        connect(_download, &QWebEngineDownloadItem::downloadProgress,
                this, &DownLoadWgt::updateWidget);
        connect(_download, &QWebEngineDownloadItem::stateChanged,
                this, &DownLoadWgt::updateWidget);

        this->setVisible(true);
        updateWidget();
    }

private:
    void onCancelClicked()
    {
        if(!_download) return;
        if (_download->state() == QWebEngineDownloadItem::DownloadInProgress)
            _download->cancel();
    }

    void openDir()
    {
        if(!_download) return;

        QString path = QFileInfo(_download->path()).absolutePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

    void onCloseClicked()
    {
        this->setVisible(false);
    }

private:
    inline QString withUnit(qreal bytes)
    {
        if (bytes < (1 << 10))
            return tr("%1 B").arg(bytes);
        else if (bytes < (1 << 20))
            return tr("%1 KB").arg(bytes / (1 << 10), 0, 'f', 2);
        else if (bytes < (1 << 30))
            return tr("%1 MB").arg(bytes / (1 << 20), 0, 'f', 2);
        else
            return tr("%1 GB").arg(bytes / (1 << 30), 0, 'f', 2);
    }

private:
    void updateWidget()
    {
        qreal totalBytes = _download->totalBytes();
        qreal receivedBytes = _download->receivedBytes();
        qreal bytesPerSecond = receivedBytes / _timeAdded.elapsed() * 1000;

        auto state = _download->state();
        switch (state) {
            case QWebEngineDownloadItem::DownloadRequested:
                Q_UNREACHABLE();
                break;
            case QWebEngineDownloadItem::DownloadInProgress:
                if (totalBytes >= 0) {
                    _pProcessBar->setValue(qRound(100 * receivedBytes / totalBytes));
                    _pProcessBar->setDisabled(false);
                    _pProcessBar->setFormat(
                            tr("%p% - %1 of %2 downloaded - %3/s")
                                    .arg(withUnit(receivedBytes))
                                    .arg(withUnit(totalBytes))
                                    .arg(withUnit(bytesPerSecond)));
                } else {
                    _pProcessBar->setValue(0);
                    _pProcessBar->setDisabled(false);
                    _pProcessBar->setFormat(
                            tr("unknown size - %1 downloaded - %2/s")
                                    .arg(withUnit(receivedBytes))
                                    .arg(withUnit(bytesPerSecond)));
                }
                break;
            case QWebEngineDownloadItem::DownloadCompleted:
                _pProcessBar->setValue(100);
                _pProcessBar->setDisabled(true);
                _pProcessBar->setFormat(
                        tr("completed - %1 downloaded - %2/s")
                                .arg(withUnit(receivedBytes))
                                .arg(withUnit(bytesPerSecond)));
                _pCancelBtn->setVisible(false);
                _closeBtn->setVisible(true);
                break;
            case QWebEngineDownloadItem::DownloadCancelled:
                _pProcessBar->setValue(0);
                _pProcessBar->setDisabled(true);
                _pProcessBar->setFormat(
                        tr("cancelled - %1 downloaded - %2/s")
                                .arg(withUnit(receivedBytes))
                                .arg(withUnit(bytesPerSecond)));
                _pCancelBtn->setVisible(false);
                _pOpenDir->setVisible(false);
                _closeBtn->setVisible(true);
                break;
            case QWebEngineDownloadItem::DownloadInterrupted:
                _pProcessBar->setValue(0);
                _pProcessBar->setDisabled(true);
                _pProcessBar->setFormat(
                        tr("interrupted: %1")
                                .arg(_download->interruptReasonString()));
                break;
        }
    }

private:
    QProgressBar* _pProcessBar;
    QPushButton* _pCancelBtn;
    QPushButton* _pOpenDir;
    QPushButton* _closeBtn;
    QLabel*      _pUrlLabel;

private:
    QWebEngineDownloadItem *_download;
    QTime _timeAdded;
};

/**
* @description: WebView
* @author: cc
* @create: 2019-02-13 21:22
**/
#include "webservice_global.h"
class WebEnginePage;
class WebJsObj;
class WEBSERVICE_EXPORT WebView : public QFrame {
    Q_OBJECT
public:
    explicit WebView(QWidget* parent = nullptr);
    ~WebView() override;

Q_SIGNALS:
    void sgFullScreen();
    void sgLoadFinished();
    void closeVideo();
    void sgCookieAdded(const QNetworkCookie &cookie);
    void sgCookieRemoved(const QNetworkCookie &cookie);

public:
    void loadUrl(const QUrl& url);
    void startReq(const QWebEngineHttpRequest& req);
    void setCookie(const QNetworkCookie& cookie, const QUrl& hostUrl);
	void setAgent(const QString& userAgent);
	void excuteJs(const QString& jsCode);
    inline QWebEngineView* getWebView() { return _pWebView; }
    void clearHistory();
    void clearCookieAndCache();

public:
    void onDownLoadFile(QWebEngineDownloadItem *download);

public:
    void setObj(WebJsObj* obj);

private:
    WebEnginePage* _pWebPage;
    QWebEngineView* _pWebView;
	QWebChannel* _pWebCannel;

    DownLoadWgt* _downloadWgt;
};


#endif //QTALK_V2_WEBVIEW_H
