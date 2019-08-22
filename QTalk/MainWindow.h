#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFrame>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMutexLocker>
#include "../CustomUi/UShadowWnd.h"
#include "../QtUtil/lib/ini/ConfigLoader.h"
#include "HotKey/qhotkey.h"
#include "../include/CommonStrcut.h"
#include "../include/STLazyQueue.h"
#include "NoOperationThread.h"

#ifdef _MACOS
#include <QMacNativeWidget>
#endif

class IUITitlebarPlug;
class IUINavigationPlug;
class IUIChatViewPlug;
class IUICardManagerPlug;
class QSystemTrayIcon;
class QTalkMsgListener;
class QTalkMsgManager;
class IUILoginPlug;
class IUIGroupManagerPlug;
class IUIPictureBroswerPlug;
class IUIAddressBookPlug;
class IUIOAManagerPlug;
class SystemTray;
class MainWindow : public UShadowDialog
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public:
    void synSeverFinish();
	void OnLoginSuccess(const std::string& strSessionId);
    void onAppActive();
    void onAppDeactivate();
    void saveWndState();
    void initSystemTray();

public:
	inline IUILoginPlug* getLoginPlug() { return _pLoginPlug; }

signals:
	void LoginSuccess(bool);
	void synDataSuccees();
	void appDeactivated();
	void systemShortCut();
	void sgAppActive();
	void sgRunNewInstance();
	void sgResetOperator();

public slots:
	void InitLogin(bool);
	void LoginResult(bool result);
	void openMainwindow();
	void onCurFunChanged(int index);
	void systemQuit();
	void setAutoLogin(bool autoFlag);
	void onUpdateHotKey();
	void wakeUpWindow();
    //
    void addOperatorLog(const QString& desc);
    void setUserStatus(bool);

private slots:
    //
    void onShockWnd();
    void onSaveSysConfig();
    void onUserSendMessage();

    // QWidget interface
protected:
	void closeEvent(QCloseEvent *e) override;
	void hideEvent(QHideEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;

private:
    void init();
    void initPanels();
    void initLayouts();
    void initTitleBar();
    void initNavigation();
    void initChatView();
    void initCardManager();
    void connectPlugs();
	void initGroupManager();
	void initPictureBrowser();
	//
	void initAddressBook();
	//
	void initOAManager();
	// 心跳timer
    void sendHeartBeat();
    // 初始化位置超出屏幕处理
	void adjustWndRect();
	// 截屏
    void onScreentShot();

public:
    QTalkMsgManager  *_pMessageManager;

private:
    QTimer  *_timr;

private:
    IUITitlebarPlug * _titleBarPlugin;
    IUINavigationPlug * _navigationPlugin;
    IUIChatViewPlug * _chatViewPlugin;
    IUICardManagerPlug* _pCardManagerPlug;
	IUIGroupManagerPlug* _pGroupManagerPlug;
    IUIPictureBroswerPlug* _pPictureBrowserPlug;
	IUIAddressBookPlug*    _pAddressBookPlug;
	IUIOAManagerPlug*      _pOAManagerPlug;
    QFrame * _mainFrm;
    QFrame * _mainBottomFrm;
    QWidget * _titleBar;
    QWidget * _navigationPanel;
    QWidget * _chatViewPanel;
    QWidget * _pCardManager;
    QWidget * _pGroupManager;
    QWidget*  _pPictureBrowser;
	QWidget*  _pAddressBook;
	QWidget*  _pOAManager;
    QVBoxLayout * _mainLayout;
    QFrame * _bottomFrm;
    QSplitter * _bottomSplt;

	SystemTray* _pSysTrayIcon;

public:
    static bool _sys_run;

private:
	QTalkMsgListener *_pListener;
	IUILoginPlug     *_pLoginPlug;
	QDialog          *_logindlg;

private:
    QTalk::ConfigLoader *_pConfigLoader;

private:
    enum {WND_INVALID, WND_NORMAL, WND_MAXSIZE, WND_FULLSCREEN};

private:
	bool _initUi;
	NoOperationThread *_noOperatorThread;

private:
    QHotkey* _pScreentShot;
    QHotkey* _pWakeWnd;
    STLazyQueue<bool> *_pScreentShotQueue;

private:
    qint64 _login_t;
    qint64 _logout_t;
    QTimer* _pOfflineTimer;
    bool _isOffline; // 是否离线(包括逻辑离线)
    QString _ipv4Address;

    QMutex _logMutex;
    QTimer* _pLogTimer;
    std::vector<QTalk::StActLog> _operators;
};
#endif // MAINWINDOW_H
