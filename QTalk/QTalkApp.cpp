#include "QTalkApp.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <thread>
#include <sstream>
#include <QEvent>
#include <iostream>
#include <QMouseEvent>
#include <iostream>
#include <QSettings>
#include <QTranslator>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QFontDatabase>
#include <curl/curl.h>
#include "UIGolbalManager.h"
#include "../LogicManager/LogicManager.h"
#include "../Platform/Platform.h"
#include "../QtUtil/Utils/Log.h"
#include "MainWindow.h"
#include "../CustomUi/QtMessageBox.h"
#include "MessageManager.h"

#ifdef _WINDOWS
#define UNICODE
#else
#ifdef _MACOS
#include "MacApp.h"
#endif
#endif


QTalkApp::QTalkApp(int argc, char *argv[])
        : QApplication(argc, argv) {

    QString strExcutePath = argv[0];
    strExcutePath = QFileInfo(strExcutePath).absolutePath();
    //strExcutePath = strExcutePath.replace("/", "\\");
	// 启动日志
	initLogSys();
	// 崩溃处理 获取dump 暂时处理以后用breakpad替代
    initDump();

    QLocale locale;
    qInfo() << locale.system().language();
    switch (locale.system().language())
    {

        case QLocale::English:
        {
            // 加载翻译文件
            static QTranslator wgtQm;
            wgtQm.load(":/QTalk/config/qtalk_en.qm");
            qApp->installTranslator(&wgtQm);
            break;
        }
        case QLocale::Chinese:
        default:
        {
            // 加载翻译文件
            static QTranslator qtGloble;
            qtGloble.load(":/QTalk/config/qt_zh_CN.qm");
            qApp->installTranslator(&qtGloble);
            //
            static QTranslator wgtQm;
            wgtQm.load(":/QTalk/config/widgets_zh_CN.qm");
            qApp->installTranslator(&wgtQm);
            break;
        }
    }

#ifdef _MACOS
    MacApp::initApp();
#endif
    //设置当前路径
#if defined(_WINDOWS) || defined(_MACOS)
    QString curPath = QApplication::applicationDirPath();
    QDir::setCurrent(curPath);
#endif

    curl_global_init(CURL_GLOBAL_ALL);
    // 加载全局单利platform
    Platform::instance().setMainThreadId();
    Platform::instance().setExcutePath(strExcutePath.toStdString());
    Platform::instance().setProcessId(qApp->applicationPid());
    QString systemStr = QSysInfo::prettyProductName();
    QString productType = QSysInfo::productType();
    QString productVersion = QSysInfo::productVersion();
    Platform::instance().setOSInfo(systemStr.toStdString());
    Platform::instance().setOSProductType(productType.toStdString());
    Platform::instance().setOSVersion(productVersion.toStdString());

    // Ui管理单例
    _pUiManager = UIGolbalManager::GetUIGolbalManager();
    //
    initTTF();
    // 加载主Qss文件
    _pUiManager->setStylesForApp();
    // 业务管理单例
    _pLogicManager = LogicManager::instance();
    //
#if defined(_STARTALK)
    setWindowIcon(QIcon(":/QTalk/image1/StarTalk.png"));
#elif defined(_QCHAT)
    setWindowIcon(QIcon(":/QTalk/image1/Qchat.png"));
#else
#ifdef _LINUX
    setWindowIcon(QIcon(":/QTalk/image1/QTalk_50.png"));
#else
    setWindowIcon(QIcon(":/QTalk/image1/QTalk.png"));
#endif
#endif
    // 去除mac默认边框
#ifdef _MACOS
    QApplication::setStyle("fusion");
#endif
    _pMainWnd = new MainWindow;
#ifdef _MACOS
    MacApp::AllowMinimizeForFramelessWindow(_pMainWnd);
    MacApp::checkValidToVisitMicroPhone();
    connect(_pMainWnd, &MainWindow::sgRunNewInstance, [](){
        QStringList params;
#if defined(_STARTALK)
        params << "-n" << "/Applications/StarTalk.app";
#else
        params << "-n" << QString("/Applications/%1.app").arg(qApp->applicationName());
#endif
        QProcess::execute("/usr/bin/open", params);
    });
#endif
    bool enableAutoLogin = true;
    QString loginMsg;
    //
    {
        int index = 0;
        QMap<QString, QString> params;
        while (index < argc)
        {
            QString tmpParam(argv[index++]);
            if(tmpParam.contains("=") && tmpParam.count("=") == 1)
            {
                params.insert(tmpParam.section("=", 0, 0),
                        tmpParam.section("=", 1, 1));
            }
        }

        if(params.contains("AUTO_LOGIN"))
            enableAutoLogin = params["AUTO_LOGIN"] == "ON";
        if(params.contains("MSG"))
            loginMsg = params["MSG"];

#ifndef _DEBUG
//#ifdef ooo
#if defined(_MACOS) || defined(_WINDOWS)
        bool startByStarter = params.contains("START_BY_STARTER");
        if(_pUiManager->_check_updater)
        {
            if(!startByStarter && _pUiManager->_updater_version > 0)
            {
#if defined(_MACOS)
                QString updater_path = QString("/Applications/%1.app/Contents/MacOS/%1").arg(qApp->applicationName());
                error_log(updater_path.toStdString());
                error_log(QApplication::applicationFilePath().toStdString());
                if(QApplication::applicationFilePath() != updater_path)
                {
                    QProcess::startDetached(updater_path, QStringList());
                    return;
                }
#else
				auto path = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
				path = QString("%1/%2/%2.lnk").arg(path).arg(qApp->applicationName());
				QFileInfo linkFileInfo(path);
				if (linkFileInfo.exists() && !linkFileInfo.symLinkTarget().isEmpty())
				{
					path = linkFileInfo.symLinkTarget();
					if(QApplication::applicationFilePath() != path)
                    {
                        QProcess::startDetached(path, QStringList());
					    return;
                    }
				}
#endif
            }
        }
        else
        {
//            if(startByStarter)
//            {
//                _pUiManager->_check_updater = true;
//                _pUiManager->saveSysConfig();
//            }
        }
#endif
#endif
    }

    _pMainWnd->InitLogin(enableAutoLogin, loginMsg);
    _pMainWnd->initSystemTray();
	// deal dump file
//	dealDumpFile();

//    do
//    {
//        if(MainWindow::_sys_run)
//        {
            exec();
            //
//            if(MainWindow::_sys_run)
//            {
//                _pMainWnd->wakeUpWindow();

//                int ret = QtMessageBox::question(_pMainWnd, "提醒", "是否立即退出");
//                if(ret == QtMessageBox::EM_BUTTON_YES)
//                    MainWindow::_sys_run = false;
//            }
//        }
//
//    } while (MainWindow::_sys_run);
}

QTalkApp::~QTalkApp() {
    if (_pLogicManager) {
        delete _pLogicManager;
        _pLogicManager = nullptr;
    }
    if (nullptr != _pMainWnd) {
        delete _pMainWnd;
        _pMainWnd = nullptr;
    }
    if (nullptr != _pUiManager) {
        delete _pUiManager;
        _pUiManager = nullptr;
    }
    if (nullptr != _pLogicManager) {
        delete _pLogicManager;
        _pLogicManager = nullptr;
    }
    QTalk::logger::exit();
}

/**
  * @函数名   QDebug 生成日志
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/10/11
  */
//QMutex mutex;//日志代码互斥锁
QString strlogPath;
QString strqLogPath;

void LogMsgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString log;

    QString localMsg(msg);
    localMsg.replace("\"", "");

    QString localPath(context.file);
    localPath.replace("\\", "/");

    log.append(QString("[time: %1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    switch (type) {
        case QtDebugMsg:
            log.append(QString("[Debug] "));
//            return;
            break;
        case QtInfoMsg:
            log.append(QString("[Info] "));
            break;
        case QtWarningMsg:
            log.append(QString("[Warning] "));
            break;
        case QtCriticalMsg:
            log.append(QString("[Critical] "));
            break;
        case QtFatalMsg:
            log.append(QString("[Fatal] "));
            break;
    }
#ifdef _DEBUG
    std::cout
#ifndef _WINDOWS
    << "\033[31m"
#endif
    << localMsg.toStdString() << std::endl;
#endif
    //
    log.append(QString("%2 %1\n").arg(localMsg).arg(context.function));

    QFile file(strqLogPath);
    if (!file.isOpen() && !file.open(QIODevice::ReadWrite | QIODevice::Append)) {
        QString erinfo = file.errorString();
    } else {
        static QTalk::util::spin_mutex sm;
        std::lock_guard<QTalk::util::spin_mutex> lock(sm);
        file.write(log.toUtf8());
        file.flush();
    }
}

/**
  * @函数名   initLogSys
  * @功能描述 启动日志系统
  * @参数
  * @author   cc
  * @date     2018/10/11
  */
void QTalkApp::initLogSys() {
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString appdata = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    strlogPath = QString("%1/logs/%2/")
            .arg(appdata)
            .arg(curDateTime.toString("yyyy-MM-dd"));
    strqLogPath = QString("/%1/%2_qdebug.log").arg(strlogPath).arg(curDateTime.toString("yyyy-MM-dd hh-mm-dd"));

    QDir logDir(strlogPath);
    if (!logDir.exists()) {
        logDir.mkpath(strlogPath);
    }

	std::string fileName = std::string(strlogPath.toLocal8Bit());

    QTalk::logger::initLog(fileName,
#ifdef _DEBUG
            QTalk::logger::LEVEL_INFO
#else
            QTalk::logger::LEVEL_WARING
#endif
    );
    //
    qInstallMessageHandler(LogMsgOutput);
    //
//    info_log("系统启动 当前版本号:{0}", Platform::instance().getClientVersion());
//    qDebug() << QStringLiteral("系统启动");
}

/**
  * @函数名   notify
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/10/11
  */
bool QTalkApp::notify(QObject *receiver, QEvent *e) {
    try {
        auto t = QDateTime::currentMSecsSinceEpoch();
        if (e->type() == QEvent::MouseButtonPress) {
            auto *mouseEvent = dynamic_cast<QMouseEvent *>(e);
            if (_pUiManager)
                emit _pUiManager->sgMousePressGlobalPos(mouseEvent->globalPos());

            if(_pMainWnd)
                emit _pMainWnd->sgResetOperator();

        } else if (e->type() == QEvent::ApplicationActivate) {
            if (nullptr != _pMainWnd) {
                _pMainWnd->onAppActive();
            }
        } else if (e->type()  == QEvent::ApplicationDeactivate) {
            if (nullptr != _pMainWnd) {
                _pMainWnd->onAppDeactivate();
            }
        }
        auto ret = QApplication::notify(receiver, e);

        t = QDateTime::currentMSecsSinceEpoch() - t;
        if(t > 1000)
        {
            qWarning() << "hang hang hang!!! use time:" << t
            << " type:" << e->type()
            << " class:" << (receiver ? receiver->metaObject()->className() : "");
        }
        return ret;
    }
    catch (const std::bad_alloc &) {
//        qDebug() << "std::bad_alloc" << t << receiver;
        return false;
    }
    catch (const std::exception &e) {
        return false;
    }
}

/**
 * 
 */
#ifdef _WINDOWS

#include <time.h>
#include <windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo) {
    // 返回EXCEPTION_CONTINUE_SEARCH，让程序停止运行
    LONG ret = EXCEPTION_CONTINUE_SEARCH;

    time_t nowtime;
    time(&nowtime);
    struct tm *pTime = localtime(&nowtime);

    // 设置core文件生成目录和文件名
    QString dmpName = QString("%1%2.dmp").arg(strlogPath).arg(QDateTime::currentDateTime().toMSecsSinceEpoch());

    HANDLE hFile = ::CreateFileW((dmpName.toStdWString().data()), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION ExInfo;

        ExInfo.ThreadId = ::GetCurrentThreadId();
        ExInfo.ExceptionPointers = pExceptionInfo;
        ExInfo.ClientPointers = NULL;

        // write the dump
        BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo,
                                     NULL, NULL);
        ret = EXCEPTION_EXECUTE_HANDLER;
        ::CloseHandle(hFile);
    }

    return ret;
}

#endif // _WINDOWS

void QTalkApp::dealDumpFile()
{
#ifdef _WINDOWS
	// deal dump
	QDateTime curDateTime = QDateTime::currentDateTime();
	QString appdata = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QString logDirPath = QString("%1/logs/").arg(appdata);
	std::function<void(const QString&)> delDmpfun;
	delDmpfun = [this, curDateTime, &delDmpfun](const QString& path) {

		QDir dir(path);
		QFileInfoList infoList = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
		for (QFileInfo tmpInfo : infoList)
		{
			if (tmpInfo.isDir())
			{
				delDmpfun(tmpInfo.absoluteFilePath());
			}
			else if (tmpInfo.isFile())
			{
				if (tmpInfo.suffix().toLower() == "dmp")
				{
					if (curDateTime.toMSecsSinceEpoch() - tmpInfo.baseName().toLongLong() < 1000 * 60 * 60 * 24 * 2) {

						std::string dumpFilePath = std::string(tmpInfo.absoluteFilePath().toLocal8Bit());
						std::thread([this, tmpInfo, dumpFilePath]() {
							if (_pMainWnd)
							{
								_pMainWnd->_pMessageManager->reportDump(dumpFilePath);
								QFile::remove(tmpInfo.absoluteFilePath());
							}
						}).detach();
					}
					else
						QFile::remove(tmpInfo.absoluteFilePath());
				}
			}
		}
	};
	delDmpfun(logDirPath);
#endif // _WINDOWS
}

void QTalkApp::initDump() {
#ifdef _WINDOWS
    ::SetUnhandledExceptionFilter(TopLevelFilter);
#endif // _WINDOWS
#ifdef _LINUX
    QString corePath = QFileInfo(".").absoluteFilePath();
    if(!corePath.isEmpty())
    {
        QFileInfo info(corePath);
        if(info.isDir())
        {
            QDir dir(info.absoluteFilePath());
            QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            QString dirPath = QString::fromStdString(Platform::instance().getAppdataRoamingPath());
            for(QFileInfo fileInfo : fileList) {
                QString path = fileInfo.absolutePath();
                if(fileInfo.baseName().toLower() == "core")
                {
                    QString newName = QString("/core_%1").arg(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
                    QFile::copy(fileInfo.absoluteFilePath(), dirPath + "/logs/" + newName);
                    break;
                }
            }
        }
    }
#endif //_LINUX
}

void QTalkApp::initTTF() {
    std::string appDataPath = Platform::instance().getAppdataRoamingPath();
    QString ttfPath = QString("%1/ttf").arg(appDataPath.data());
    {
        QString tmpTTF = QString("%1/%2").arg(ttfPath, "FZLTHJW.TTF");
        if(!QFile::exists(tmpTTF))
        {
            if(!QFile::exists(ttfPath))
            {
                QDir dir(appDataPath.data());
                dir.mkpath(ttfPath);
            }

            QTemporaryFile* tmpFile = QTemporaryFile::createNativeFile(":/QTalk/ttf/FZLTHJW.TTF");
            tmpFile->copy(tmpTTF);
        }
        qDebug() << QFontDatabase::addApplicationFont(tmpTTF);
    }
//    {
//        QString tmpTTF = QString("%1/%2").arg(ttfPath, "FZLTZHJW.TTF");
//        if(!QFile::exists(tmpTTF))
//        {
//            if(!QFile::exists(ttfPath))
//            {
//                QDir dir(appDataPath.data());
//                dir.mkpath(ttfPath);
//            }
//
//            QTemporaryFile* tmpFile = QTemporaryFile::createNativeFile(":/QTalk/ttf/FZLTZHJW.TTF");
//            tmpFile->copy(tmpTTF);
//        }
//        qDebug() << QFontDatabase::addApplicationFont(tmpTTF);
//    }
}
