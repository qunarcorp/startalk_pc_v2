//
// Created by cc on 18-12-10.
//

#include "SystemTray.h"
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QFile>
#include "../interface/view/IUILoginPlug.h"
#include "MainWindow.h"
#include "../Platform/AppSetting.h"
#include "../Platform/Platform.h"
#include "../quazip/JlCompress.h"
#include "MessageManager.h"

extern bool _bSystemRun;
SystemTray::SystemTray(MainWindow* mainWnd)
    : QObject(mainWnd), _pSysTrayIcon(nullptr),
      _timer(nullptr), _pMainWindow(mainWnd),
      _timerCount(0)
{
    _pSysTrayIcon = new QSystemTrayIcon(this);
#if defined(_ATALK)
    _pSysTrayIcon->setToolTip("ATalk");
    _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/atalk.png"));
#elif defined(_STARTALK)
    _pSysTrayIcon->setToolTip("StarTalk");
    _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/StarTalk.png"));
#elif defined(_QCHAT)
    _pSysTrayIcon->setToolTip("QChat");
    _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/Qchat.png"));
#else
    _pSysTrayIcon->setToolTip("QTalk");
    _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/QTalk.png"));
#endif

    _pSysTrayIcon->show();

    //
    QMenu* pSysTrayMenu = new QMenu;
    pSysTrayMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    QAction* sysQuitAct = new QAction("系统退出");
    QAction* showWmdAct = new QAction("显示面板");
    QAction* autoLoginAct = new QAction("自动登录");
    QAction* sendLog = new QAction("快速反馈日志");
    if (_pMainWindow->getLoginPlug())
    {
        bool isAuto = _pMainWindow->getLoginPlug()->getAutoLoginFlag();
        if (isAuto) autoLoginAct->setText("取消自动登录");
    }
    pSysTrayMenu->addAction(autoLoginAct);
    pSysTrayMenu->addAction(sendLog);
    pSysTrayMenu->addAction(showWmdAct);
    pSysTrayMenu->addSeparator();
    pSysTrayMenu->addAction(sysQuitAct);
    _pSysTrayIcon->setContextMenu(pSysTrayMenu);
    bool ok = _pSysTrayIcon->supportsMessages();
    _timer = new QTimer;
    _timer->setInterval(500);

    //
    connect(_pSysTrayIcon, &QSystemTrayIcon::activated, this, &SystemTray::activeTray);
    connect(sysQuitAct, &QAction::triggered, _pMainWindow, &MainWindow::systemQuit);
    connect(sendLog, &QAction::triggered, this, &SystemTray::onSendLog);
    connect(showWmdAct, &QAction::triggered, [this]()
    {
        stopTimer();
        _pMainWindow->wakeUpWindow();
    });
    connect(autoLoginAct, &QAction::triggered, [this, autoLoginAct]()
    {
        stopTimer();
        if (_pMainWindow->getLoginPlug())
        {
            bool isAuto = _pMainWindow->getLoginPlug()->getAutoLoginFlag();
            _pMainWindow->setAutoLogin(!isAuto);
            if (isAuto)
                autoLoginAct->setText("自动登录");
            else
                autoLoginAct->setText("取消自动登录");
        }
    });

    connect(_timer, &QTimer::timeout, this, &SystemTray::onTimer);
}

SystemTray::~SystemTray() {

}

/**
  * @函数名   系统托盘事件处理
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/11/09
  */
void SystemTray::activeTray(QSystemTrayIcon::ActivationReason reason)
{
    stopTimer();
    if (reason == QSystemTrayIcon::Trigger)
    {
        _pMainWindow->wakeUpWindow();
    }
}

void SystemTray::onRecvMessage()
{
    //
    if(!_timer->isActive() && !_pMainWindow->isActiveWindow())
    {
        _timerCount = 0;
        _timer->start();
        if(AppSetting::instance().getStrongWarnFlag())
        {
            QApplication::alert(_pMainWindow);
        }
    }
}

void SystemTray::onTimer() {

    _timerCount ++;
	
    if(_timerCount % 2 == 0)
    {
#if defined(_ATALK)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/atalk.png"));
#elif defined(_STARTALK)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/StarTalk.png"));
#else
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/QTalk.png"));
#endif
    } else {
#if defined(_ATALK)
        _pSysTrayIcon->setIcon(QIcon());
#elif defined(_STARTALK)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/starTalkTip.png"));
#elif defined(_QCHAT)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/Qchat.png"));
#else
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/redPoint.png"));
#endif
    }
}

void SystemTray::stopTimer() {
    if(_timer->isActive())
    {
        _timer->stop();
#if defined(_ATALK)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/atalk.png"));
#elif defined(_STARTALK)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/StarTalk.png"));
#elif defined(_QCHAT)
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/Qchat.png"));
#else
        _pSysTrayIcon->setIcon(QIcon(":/QTalk/image1/QTalk.png"));
#endif
    }
}

void SystemTray::onWndActived()
{
    stopTimer();
}

/**
 *
 */
void SystemTray::onSendLog()
{
    std::thread([this]() {
#ifdef _MACOS
        pthread_setname_np("ChatViewMainPanel::packAndSendLog");
#endif
        //db 文件
        QString logBasePath;
        logBasePath = QString::fromStdString(Platform::instance().getAppdataRoamingPath()) + "/logs";
        // zip
        QString logZip = logBasePath + "/log.zip";
        if (QFile::exists(logZip))
            QFile::remove(logZip);
        //
        bool ret = JlCompress::compressDir(logZip, logBasePath);
        if (ret) {
            if (_pMainWindow && _pMainWindow->_pMessageManager) {
                _pMainWindow->_pMessageManager->sendLogReport("quick report", logZip.toStdString());
            }
        }
    }).detach();
}
