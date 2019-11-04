#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef LOGINPANEL_H
#define LOGINPANEL_H

#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QLocalServer>
#include <QCompleter>
#include "MessageManager.h"
#include "../CustomUi/HeadPhotoLab.h"
#include "../CustomUi/UShadowWnd.h"
#include "../CustomUi/ShadowHeadPhoto.h"
#include "NavManager.h"
#include "../UICom/qconfig/qconfig.h"
#include "QWebLogin.h"
#include "../CustomUi/HeadPhotoLab.h"

class LoginPanel : public QDialog
{
Q_OBJECT

public:
    explicit LoginPanel(QWidget *parent = nullptr);
    ~LoginPanel();

public:
    void initConf();
    void onGotLoginstauts(const QString& msg);
    void authFailed();
    void loginSuccess();
    void enableAutoLogin(bool);

public:
    bool getAutoLoginFlag();
    void setAutoLoginFlag(bool flag);
    QString getDomainByNav(const QString& nav);
    void saveHeadPath();
    void loginError(const std::string& errMsg);
    void getTokenByQVT(const std::string& qvt,bool isAutoLogin);

public slots:
    void onSynDataSuccess();
    Q_INVOKABLE void onAuthFailed(const QString& msg);

Q_SIGNALS:
    void showStatusMessage(const QString&);
    void sgSynDataSuccess();
    void AuthFailedSignal(const QString& msg);

protected:
    void init();
    void initLayout();
    void connects();
    void initloginWnd();
    void initLogingWnd();
    void onLoginBtnClicked();

protected:
    void mousePressEvent(QMouseEvent *e) override ;
    void mouseReleaseEvent(QMouseEvent *e) override ;
    void mouseMoveEvent(QMouseEvent *e) override ;
    void closeEvent(QCloseEvent *e) override ;
    bool event(QEvent* e) override ;
//    void paintEvent(QPaintEvent* e);

protected:
    bool eventFilter(QObject* o, QEvent* e) override;

private:
    void setHead(const QString& headPath);

private:
    UILoginMsgManager* _pManager;
    UILoginMsgListener* _pListener;
    QHBoxLayout* _mainLayout;

    QFrame * _loginMainFrm;

    QFrame * _leftFrame;
    HeadPhotoLab * _headPhotoLab;

    QFrame * _userNameFrm;
    QLineEdit* _userNameEdt;
    QCompleter* _userNameCompleter;

    QFrame * _passworldFrm;
    QLineEdit* _passworldEdt;
    QPushButton* _loginBtn;

    QPushButton * _settingBtn;

    QFrame * _settingFrm;
    QCheckBox * _rememberPassBtn;
    QCheckBox * _autoLoginBtn;
    QPushButton * _severBtn;

    QPushButton * _closeBtn;

    QPoint _mousePoint;
    bool _mousePressed;

private:
    NavManager* _pNavManager;
    QLabel* _pAuthFailLabel;

private:
    QFrame* _pLoginFrm;
    QFrame* _pLogingFrm;
    QFrame* _pAuthFailedFrm;
    QFrame* _pAuthFailedSpaceFrm;
    QLabel* _pStsLabel;
    QPushButton * _cancelLoginBtn;

private:
    QLabel* _pWebLogin;

private:
    QString            _strConfPath;
    QTalk::StConfig     *_pStLoginConfig;
    QTalk::StConfig     *_pDefaultConfig;

private:
    QTimer*              _pTimer;
    int                  _timeTimes;

private:
    QLocalServer   *_pLocalServer;

private:
    bool _enableAutoLogin;
};

#endif // LOGINPANEL_H