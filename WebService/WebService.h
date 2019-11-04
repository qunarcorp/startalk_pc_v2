//
// Created by QITMAC000260 on 2019-01-27.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_WEBSERVICE_H
#define QTALK_V2_WEBSERVICE_H

#include <QDialog>
#include "WebView.h"
#include <QToolButton>
#include "webservice_global.h"
#include <QLineEdit>

typedef QMap<QString, QString> MapCookie;

/**
* @description: WebService
* @author: cc
* @create: 2019-01-27 23:46
**/
class WEBSERVICE_EXPORT WebService : public QDialog{
    Q_OBJECT
private:
    WebService();

public:
    ~WebService() override;

public:
    static void loadCoEdit(const QUrl& url);
    static void loadUrl(const QUrl& url, bool showUrl = true, const MapCookie& cookies = MapCookie());
    static void loadUrl(const QUrl& url,const std::string& domain, bool showUrl = true, const MapCookie& cookies = MapCookie());

protected:
    void closeEvent(QCloseEvent* e) override;
	void mouseDoubleClickEvent(QMouseEvent *e) override;
	bool event(QEvent* e) override;

private:
    void loadQss();
    void onLoadFinish(bool ok);

private:
    WebView* _webView;

private:
    QFrame* _pTitleFrm;
    QFrame* _toolFrm;
    QLabel* _pTitleLabel;
    QLineEdit* _pAddressEdit;
    QToolButton* _pCloseBtn;

    QToolButton* _pBackBtn;     //
    QToolButton* _pForwardBtn;  //
    QToolButton* _pReloadBtn;   //

    QToolButton* _pShareBtn;      // 转发
    QToolButton* _pCopyUrlBtn;
    QToolButton* _pJumpToBrowserBtn; //

private:
    static WebService* _service;
};


#endif //QTALK_V2_WEBSERVICE_H
