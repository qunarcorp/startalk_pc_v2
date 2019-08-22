#include "LoginPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QLocalSocket>
#include <QApplication>
#include <QProcess>
#include <QNetworkConfigurationManager>
#include <QTcpSocket>
#include "../CustomUi/UShadowEffect.h"
#include "../UICom/uicom.h"
#include "../QtUtil/lib/ini/ConfigLoader.h"
#include "../Platform/Platform.h"
#include "../Platform/dbPlatForm.h"
#include "../QtUtil/Utils/Log.h"
#include "../CustomUi/QtMessageBox.h"
#include "../Platform/AppSetting.h"
#include "../Platform/NavigationManager.h"

#ifdef _WINDOWS
#include <windows.h>
#endif // _WINDOWS


#define CONFIG_KEY_SAVEPASSWORD "config_key_savepassword"
#define CONFIG_KEY_DEFAULT "config_key_default"
#define CONFIG_KEY_AUTOLOGIN    "config_key_autologin"
#define CONFIG_KEY_USERNAME    "config_key_username"
#define CONFIG_KEY_PASSWORD    "config_key_password"
#define CONFIG_KEY_HEADPATH    "config_key_head"
#ifdef _QCHAT
#define DEM_DEFAULT_NAV ""
#else
#define DEM_DEFAULT_NAV ""
#endif

LoginPanel::LoginPanel(QWidget *parent) :
        QDialog(parent), _pStLoginConfig(nullptr), _pDefaultConfig(nullptr), _pLocalServer(nullptr) {
    _pManager = new UILoginMsgManager;
    _pListener = new UILoginMsgListener(this);
    _mousePressed = false;


    this->setAttribute(Qt::WA_TranslucentBackground); //设置顶层面板背景透明
    setWindowFlags(Qt::FramelessWindowHint);  //设置无边框
    setContentsMargins(10, 10, 10, 10);
    init();

    UICom::getInstance()->setAcltiveMainWnd(this);

}

LoginPanel::~LoginPanel() {
    if (nullptr != _pStLoginConfig) {
        delete _pStLoginConfig;
        _pStLoginConfig = nullptr;
    }
    if(nullptr != _pLocalServer)
    {
        delete _pLocalServer;
        _pLocalServer = nullptr;
    }
}

void LoginPanel::onGotLoginstauts(const QString &msg) {
    emit showStatusMessage(msg);
}

/**
 *
 */
void LoginPanel::onSynDataSuccess() {

    emit sgSynDataSuccess();
}

void LoginPanel::init() {
    initLayout();
    connects();
}

/**
 *
 */
void LoginPanel::initLayout() {

    // load tips xml
    QTalk::StConfig config;
    QTalk::qConfig::loadConfig(":/login/tips.xml", false, &config);
    QVector<QString> tips;
    for (QTalk::StConfig *tmpConf : config.children) {
        QString tip = tmpConf->tagVal;
        if (!tip.isEmpty())
            tips.push_back(tip);
    }
    _pNavManager = new NavManager(this);
    //
    this->setObjectName("loginMainPanel");
    this->setFixedSize(740, 560);
    auto *glay = new QHBoxLayout(this);
    glay->setMargin(0);
    glay->setSpacing(0);
    _loginMainFrm = new QFrame(this);
    _loginMainFrm->setObjectName("loginMainFrm");
    glay->addWidget(_loginMainFrm);

//    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
//    int picnum = qrand() % 18 + 1;
//    QString strStyle = QString("QFrame#loginMainFrm { border-radius:6px; background:rgba(5, 13, 23, 1);"
//                               "border-image:url(:/login/image1/loginbj/%1.png); }").arg(QString::number(picnum));
//    _loginMainFrm->setStyleSheet(strStyle);
#ifndef _MACOS
    auto *defaultShadow = new QGraphicsDropShadowEffect();
    defaultShadow->setBlurRadius(10.0);
    defaultShadow->setColor(QColor(0, 0, 0, 160));
    defaultShadow->setOffset(0, 0);
    _loginMainFrm->setGraphicsEffect(defaultShadow);
#endif
    _mainLayout = new QHBoxLayout(_loginMainFrm);
    _mainLayout->setMargin(0);
    _mainLayout->setSpacing(0);
    _leftFrame = new QFrame(this);
    _leftFrame->setObjectName("leftFrame");
    _leftFrame->setFixedWidth(280);
    _mainLayout->addWidget(_leftFrame);
    auto *leftFrmLay = new QVBoxLayout(_leftFrame);
    leftFrmLay->setMargin(0);
    leftFrmLay->setSpacing(0);

    _pAuthFailedFrm = new QFrame(this);
    _pAuthFailedFrm->setObjectName("AuthFailedFrm");
    _pAuthFailedFrm->setFixedHeight(40);
    leftFrmLay->addWidget(_pAuthFailedFrm);
    auto *authFailLay = new QHBoxLayout(_pAuthFailedFrm);
    HeadPhotoLab *errPic = new HeadPhotoLab;
    errPic->setParent(this);
    errPic->setHead(":/login/image1/error.png", 10, false, false, true);
    authFailLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    authFailLay->addWidget(errPic);
    _pAuthFailLabel = new QLabel(this);
    _pAuthFailLabel->setObjectName("AuthFailedLabel");
    authFailLay->addWidget(_pAuthFailLabel);
    authFailLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    _pAuthFailedFrm->setVisible(false);

    _pAuthFailedSpaceFrm = new QFrame(this);
    _pAuthFailedSpaceFrm->setFixedHeight(40);
    leftFrmLay->addWidget(_pAuthFailedSpaceFrm);

    //
    auto *headhbox = new QHBoxLayout;
    leftFrmLay->addLayout(headhbox);
    headhbox->setContentsMargins(0, 10, 0, 0);

    _headPhotoLab = new HeadPhotoLab;
    _headPhotoLab->setParent(this);
#ifdef _STARTALK
    QString headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
    QString headPath = ":/QTalk/image1/headPortrait.png";
#endif
    _headPhotoLab->setHead(headPath, 52, false, true);
    headhbox->addItem(new QSpacerItem(10, 200, QSizePolicy::Minimum, QSizePolicy::Fixed));
    headhbox->addWidget(_headPhotoLab);
    //
    initloginWnd();
    leftFrmLay->addWidget(_pLoginFrm);
    //
    initLogingWnd();
    leftFrmLay->addWidget(_pLogingFrm);
    _pLogingFrm->setVisible(false);
    //
    QFrame* rightFrm = new QFrame(this);
    rightFrm->setObjectName("rightFrame");
    auto *rhbox = new QVBoxLayout(rightFrm);
    rhbox->setMargin(0);
    _mainLayout->addWidget(rightFrm);
    auto *thbox = new QHBoxLayout;
    rhbox->addLayout(thbox);
    thbox->addItem(new QSpacerItem(40, 1, QSizePolicy::Expanding));
    _closeBtn = new QPushButton;
    _closeBtn->setObjectName("closeBtn");
    _closeBtn->setFixedSize(30, 30);
    _closeBtn->setFocusPolicy(Qt::NoFocus);
    thbox->addWidget(_closeBtn);
    rhbox->addItem(new QSpacerItem(1, 40, QSizePolicy::Fixed, QSizePolicy::Expanding));

    auto *verLayout = new QHBoxLayout;

    // 其他文案
    QString tip = tr("QTalk测试版本 不代表最终品质");
    if (!tips.empty()) {
        int tipIdex = qrand() % tips.size();
        tip = tips[tipIdex];
    }
    QLabel *waLabel = new QLabel(tip, this);
    waLabel->setObjectName("wenanLabel");
    waLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    waLabel->setContentsMargins(15, 3, 0, 10);
    waLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //
    verLayout->addWidget(waLabel);
    rhbox->addLayout(verLayout);


    leftFrmLay->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    //
    connect(_cancelLoginBtn, &QPushButton::clicked, [this](){
        int ret =QtMessageBox::question(this, "提示", "确认取消登录 ? ");
        if(ret == QtMessageBox::EM_BUTTON_YES)
        {
            QString program = QApplication::applicationFilePath();
            QStringList arguments;
            arguments << "AUTO_LOGIN=OFF";
            QProcess::startDetached(program, arguments);
            exit(0);
        }
    });
}

/**
 *
 */
void LoginPanel::connects() {
    connect(_loginBtn, &QPushButton::clicked, this, &LoginPanel::onLoginBtnClicked);
    connect(_closeBtn, &QPushButton::clicked, []() { exit(0); });
    connect(this, &LoginPanel::sgSynDataSuccess, _pTimer, &QTimer::stop);
    connect(_severBtn, &QPushButton::clicked, [this](bool) {
        _pNavManager->showCenter(true, this);
    });
    connect(_userNameEdt, &QLineEdit::textChanged, [this](const QString &name) {

        _pDefaultConfig = nullptr;

        QString strName = name;
        strName = strName.replace(QRegExp("\\s{1,}"), "").toLower();

        auto itFind = std::find_if(_pStLoginConfig->children.begin(), _pStLoginConfig->children.end(),
                                   [ strName](QTalk::StConfig *tmpConf) {
                                       return tmpConf->attribute(CONFIG_KEY_USERNAME) == strName;
                                   });
        if (itFind != _pStLoginConfig->children.end()) {
            _pDefaultConfig = *itFind;
            //
            QString headPath = _pDefaultConfig->attribute(CONFIG_KEY_HEADPATH);
            QString password = _pDefaultConfig->attribute(CONFIG_KEY_PASSWORD);
            setHead(headPath);
            if(_rememberPassBtn->isChecked())
                _passworldEdt->setText(password);
        } else {
            setHead("");
            _passworldEdt->setText("");
        }
    });
}

/**
 *
 */
void LoginPanel::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        _mousePressed = true;
        _mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
    QDialog::mousePressEvent(e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author
  * @date 2018.9.24
  */
void LoginPanel::mouseReleaseEvent(QMouseEvent *e) {
    Q_UNUSED(e)
    _mousePressed = false;
    QDialog::mouseReleaseEvent(e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author
  * @date 2018.9.24
  */
void LoginPanel::mouseMoveEvent(QMouseEvent *e) {

    if (_mousePressed) {
        this->move(e->globalPos() - _mousePoint);
        e->accept();
    }
    QDialog::mouseMoveEvent(e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author
  * @date 2018.9.24
  */
void LoginPanel::closeEvent(QCloseEvent *e) {
    Q_UNUSED(e)

    QDialog::closeEvent(e);
}

bool LoginPanel::eventFilter(QObject *o, QEvent *e) {

    if (o == _pWebLogin) {
        if (e->type() == QEvent::Enter) {
            setCursor(Qt::PointingHandCursor);
        } else if (e->type() == QEvent::Leave) {
            setCursor(Qt::ArrowCursor);
        } else if (e->type() == QEvent::MouseButtonPress) {
            // web 登录时先获取一次导航
            std::string nav = _pNavManager->getDefaultNavUrl().toStdString();
            if(nav.empty())
                emit AuthFailedSignal(tr("导航不能为空!"));

            bool ret = _pManager->getNavInfo(nav);
            if (!ret)
                emit AuthFailedSignal(tr("导航获取失败, 请检查网络连接!"));
            else
                QWebLogin::load(this);
        }
    }
    return QDialog::eventFilter(o, e);
}

//
void LoginPanel::initConf() {

    //std::thread([this](){

    //    QString userDirPath = AppSetting::instance().getUserDirectory().data();
    //    QDir dir(userDirPath);
    //    QSet<QString> users;
    //    auto lst = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    //    for(const QFileInfo& info : lst )
    //    {
    //        if(info.fileName().contains("@"))
    //        {
    //            QString fileName = info.fileName();
    //            int pos = fileName.lastIndexOf("_");
    //            fileName = fileName.mid(0, pos);
    //            users.insert(fileName);
    //        }
    //    }

    //    QString strUsers;
    //    for(const QString& user : users)
    //    {
    //        strUsers.append(user);
    //        strUsers.append("|");
    //    }

    //    if(_pManager)
    //    {
    //        _pManager->startUpdater(strUsers.toStdString());
    //    }

    //}).detach();
	//
	std::string usrDir = AppSetting::instance().getUserDirectory();
#if defined(_WINDOWS)
	std::wstring dir(usrDir.begin(), usrDir.end());
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;

	BOOL fResult = GetDiskFreeSpaceEx(
		(LPCSTR)dir.data(),
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	//GetDiskFreeSpaceEx函数，可以获取驱动器磁盘的空间状态,函数返回的是个BOOL类型数据
	if (fResult)//通过返回的BOOL数据判断驱动器是否在工作状态
	{
		float leftSpace = (float)i64FreeBytesToCaller / 1024 / 1024;
		if (leftSpace < 1024)
		{
			int ret = QtMessageBox::question(this, "磁盘空间不足", "磁盘不足, 可能会导致程序异常退出, 是否立即退出?");
			if (ret == QtMessageBox::EM_BUTTON_YES)
			{
				exit(0);
			}
		}
	}
	else
	{
		error_log("get hard disk space error");
	}
#endif
    //
    _strConfPath = QString("%1/login.data").arg(Platform::instance().getConfigPath().c_str());
    _pStLoginConfig = new QTalk::StConfig;

    if (QFile::exists(_strConfPath)) {

        QTalk::qConfig::loadConfig(_strConfPath, true, _pStLoginConfig);
        if (_pStLoginConfig->tagName != "loginConf") {
            //
            // 这里这个文件已经有问题了，要干掉生成新的。by dan.liu
            QFile::remove(_strConfPath);
            _pStLoginConfig->tagName = "loginConf";
            return;
        }

        //
        _pDefaultConfig = nullptr;
        for (QTalk::StConfig *tmpConf : _pStLoginConfig->children) {
            if (tmpConf->hasAttribute(CONFIG_KEY_DEFAULT) &&
                tmpConf->attribute(CONFIG_KEY_DEFAULT).toInt()) {
                _pDefaultConfig = tmpConf;
            }
        }
        if (nullptr != _pDefaultConfig) {
            int savePassword = _pDefaultConfig->attribute(CONFIG_KEY_SAVEPASSWORD).toInt();
            int autoLogin = _pDefaultConfig->attribute(CONFIG_KEY_AUTOLOGIN).toInt();
            AppSetting::instance().setAutoLoginEnable(autoLogin);
            QString userName = _pDefaultConfig->attribute(CONFIG_KEY_USERNAME);
            QString headPath = _pDefaultConfig->attribute(CONFIG_KEY_HEADPATH);
            _userNameEdt->setText(userName);
            _rememberPassBtn->setChecked(savePassword);
            setHead(headPath);
            if (savePassword) {
                _autoLoginBtn->setChecked(autoLogin);
                QString password = _pDefaultConfig->attribute(CONFIG_KEY_PASSWORD);
                _passworldEdt->setText(password);
                if (autoLogin && _enableAutoLogin) {
#ifdef _QCHAT
                    getTokenByQVT("", true);
#else
                    _loginBtn->clicked();
#endif

                }
            }
        }
    } else {
        _pStLoginConfig->tagName = "loginConf";
    }
}

void LoginPanel::initloginWnd() {
    _pLoginFrm = new QFrame(this);
    auto *loginLayout = new QVBoxLayout(_pLoginFrm);
    loginLayout->setMargin(0);
    loginLayout->setSpacing(0);

    auto *userpasslay = new QVBoxLayout;
    loginLayout->addLayout(userpasslay);
    userpasslay->setContentsMargins(30, 30, 30, 0);
    _userNameFrm = new QFrame(this);
    _userNameFrm->setObjectName("userNameFrm");
    _userNameFrm->setFixedSize(220, 35);
    userpasslay->addWidget(_userNameFrm);
    userpasslay->addWidget(new Line);
    auto *userNamelay = new QHBoxLayout(_userNameFrm);
    userNamelay->setMargin(0);
    userNamelay->setSpacing(0);
    _userNameEdt = new QLineEdit(this);
    _userNameEdt->setPlaceholderText(tr("请输入账号"));
    _userNameEdt->setObjectName("userNameEdt");
    userNamelay->addWidget(_userNameEdt);

    _passworldFrm = new QFrame(this);
    _passworldFrm->setObjectName("passworldFrm");
    _passworldFrm->setFixedSize(220, 35);
    userpasslay->addWidget(_passworldFrm);
    userpasslay->addWidget(new Line);
    auto *passworldlay = new QHBoxLayout(_passworldFrm);
    passworldlay->setMargin(0);
    passworldlay->setSpacing(0);
    _passworldEdt = new QLineEdit(this);
    _passworldEdt->setPlaceholderText(tr("请输入密码"));
    _passworldEdt->setObjectName("passworldEdt");
    _passworldEdt->setEchoMode(QLineEdit::Password);
    passworldlay->addWidget(_passworldEdt);
    _loginBtn = new QPushButton;
    _loginBtn->setObjectName("loginBtn");
    _loginBtn->setFixedSize(30, 30);
    passworldlay->addWidget(_loginBtn);

#ifdef _QCHAT
    _pWebLogin = new QLabel(this);
    _pWebLogin->setText("<a style='color:#00CABE;' href='aa'>web登录</a>");
    _pWebLogin->setContentsMargins(0,10,0,0);
    _pWebLogin->installEventFilter(this);

    userpasslay->addWidget(_pWebLogin);;
#endif

    _settingBtn = new QPushButton(this);
    _settingBtn->setObjectName("settingBtn");
    _settingBtn->setFixedSize(30, 30);
    _settingBtn->setCheckable(true);
    auto *settingBtnlay = new QHBoxLayout;
    loginLayout->addLayout(settingBtnlay);
    settingBtnlay->setContentsMargins(0, 20, 0, 0);
    settingBtnlay->addWidget(_settingBtn);

    _settingFrm = new QFrame(this);
    _settingFrm->setObjectName("settingFrm");
    loginLayout->addWidget(_settingFrm);
    auto *radioLay = new QHBoxLayout;
    radioLay->setContentsMargins(30, 15, 30, 10);
    _rememberPassBtn = new QCheckBox(tr("记住密码"));
    radioLay->addWidget(_rememberPassBtn);
    radioLay->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    _autoLoginBtn = new QCheckBox(tr("自动登录"));
    radioLay->addWidget(_autoLoginBtn);
    auto *serverLay = new QHBoxLayout;
    serverLay->setMargin(15);
    _severBtn = new QPushButton(tr("服务器"), this);
    _severBtn->setObjectName("severBtn");
    _severBtn->setFixedSize(72, 24);
    serverLay->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    serverLay->addWidget(_severBtn);
    serverLay->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    // version label
    std::string globalversion = Platform::instance().getGlobalVersion();
    QLabel *verLabel = new QLabel(QString("V.%1").arg(globalversion.c_str()), this);
    verLabel->setObjectName("GlobalVersion");
    verLabel->setAlignment(Qt::AlignCenter);
    //
    auto *setttingLayout = new QVBoxLayout(_settingFrm);
    setttingLayout->setMargin(0);
    setttingLayout->addLayout(radioLay);
    setttingLayout->addLayout(serverLay);

    QFrame* spaceFrm = new QFrame(this);
    spaceFrm->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    loginLayout->addWidget(spaceFrm);
    loginLayout->addWidget(verLabel);
    connect(_passworldEdt, &QLineEdit::textChanged, [this]{ style()->polish(_passworldEdt); });
    connect(_userNameEdt, &QLineEdit::textChanged, [this]{ style()->polish(_userNameEdt); });
    //
    //_settingBtn
    connect(_settingBtn, &QPushButton::clicked, [this, spaceFrm](bool checked){
        _settingFrm->adjustSize();
        spaceFrm->setFixedSize(_settingFrm->size());

        spaceFrm->setVisible(checked);
        _settingFrm->setVisible(!checked);
    });
    _settingBtn->click();
}

/**
 *
 */
void LoginPanel::initLogingWnd() {
    _pLogingFrm = new QFrame(this);
    auto *logingLayout = new QVBoxLayout(_pLogingFrm);
    logingLayout->setMargin(0);
    logingLayout->setSpacing(15);
    //
    auto *logingLabel = new QLabel(this);
    logingLabel->setText(tr("登录中"));
    logingLabel->setObjectName("LogingWnd_Loging");
    logingLabel->setAlignment(Qt::AlignCenter);
    logingLayout->addWidget(logingLabel);
    //
    _pStsLabel = new QLabel();
    _pStsLabel->setObjectName("LogingWnd_status");
    _pStsLabel->setAlignment(Qt::AlignCenter);
    logingLayout->addWidget(_pStsLabel);
    //
    _cancelLoginBtn = new QPushButton(tr("取消登录"));
    _cancelLoginBtn->setFixedSize(85, 26);
    _cancelLoginBtn->setObjectName("cancelLoginButton");
    auto *cancelLay = new QHBoxLayout;
    cancelLay->setContentsMargins(0, 80, 0, 0);
    cancelLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    cancelLay->addWidget(_cancelLoginBtn);
    cancelLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    logingLayout->addLayout(cancelLay);

    //
    _timeTimes = 0;
    _pTimer = new QTimer();
    _pTimer->setInterval(500);
    connect(_pTimer, &QTimer::timeout, [this, logingLabel]() {
        _timeTimes++;
        if (_timeTimes == 4) {
            _timeTimes = 0;
        }
        QString strText = QString(tr("登录中")) + QString(_timeTimes, QChar('.'));
        logingLabel->setText(strText);
    });
    _pTimer->start();
}

void LoginPanel::authFailed() {
    emit AuthFailedSignal(tr("账户或密码错误!"));
}

bool LoginPanel::getAutoLoginFlag() {
    if (_pDefaultConfig) {
        int isok = _pDefaultConfig->attribute(CONFIG_KEY_AUTOLOGIN).toInt();
        return (bool) isok;
    }
    return false;
}

void LoginPanel::setAutoLoginFlag(bool flag) {
    if (_pDefaultConfig) {
        _autoLoginBtn->setChecked(flag);
        _pDefaultConfig->setAttribute(CONFIG_KEY_AUTOLOGIN, QString::number(flag));
        QTalk::qConfig::saveConfig(_strConfPath, true, _pStLoginConfig);
    }
}

void LoginPanel::onLoginBtnClicked()
{
    connect(this, &LoginPanel::showStatusMessage, _pStsLabel, &QLabel::setText, Qt::QueuedConnection);
    connect(this, &LoginPanel::AuthFailedSignal, this, &LoginPanel::onAuthFailed);
    //
    QString strName = _userNameEdt->text();
    QString strPassword = _passworldEdt->text();
    QString domain = _pNavManager->getDefaultDomain();
    if (strName.isEmpty() || strPassword.isEmpty()) {
        emit AuthFailedSignal(tr("账户或密码不能为空!"));
        return;
    }
    if(domain.isEmpty())
    {
        emit AuthFailedSignal(tr("导航配置出错!"));
        return;
    }
    //
    strName = strName.replace(QRegExp("\\s{1,}"), "").toLower();

    if(nullptr == _pLocalServer)
        _pLocalServer = new QLocalServer;

    // 单进程监听
#ifndef _DEBUG
    QLocalSocket localSocket;
    QString listenName = QString("%1@%2").arg(strName, domain);
    localSocket.connectToServer(listenName);
    if (localSocket.waitForConnected(1000)) {
        localSocket.disconnectFromServer();
        localSocket.close();
        emit AuthFailedSignal(tr("该账号已登录!"));
        return;
    }
#endif

    // 设置画面显隐
    _pLoginFrm->setVisible(false);
    _pLogingFrm->setVisible(true);
    _pAuthFailedFrm->setVisible(false);
    _pAuthFailedSpaceFrm->setVisible(true);
    // 设置登陆相关配置文件
    if (nullptr == _pDefaultConfig) {
        _pDefaultConfig = new QTalk::StConfig("item");
        _pDefaultConfig->setAttribute(CONFIG_KEY_DEFAULT, true);
        _pStLoginConfig->addChild(_pDefaultConfig);
    }
    _pDefaultConfig->setAttribute(CONFIG_KEY_USERNAME, strName);
    if (_rememberPassBtn->isChecked()) {
        _pDefaultConfig->setAttribute(CONFIG_KEY_PASSWORD, strPassword);
//        _pDefaultConfig->setAttribute(CONFIG_KEY_SAVEPASSWORD, true);
//        _pDefaultConfig->setAttribute(CONFIG_KEY_AUTOLOGIN, _autoLoginBtn->isChecked());
    } else {
        _pDefaultConfig->setAttribute(CONFIG_KEY_SAVEPASSWORD, false);
        _pDefaultConfig->setAttribute(CONFIG_KEY_AUTOLOGIN, false);
    }
    // 设置默认账户
    for (QTalk::StConfig *tmpConf : _pStLoginConfig->children) {
        QString tmpName = tmpConf->attribute(CONFIG_KEY_USERNAME);
        tmpConf->setAttribute(CONFIG_KEY_DEFAULT, tmpName == strName);
    }
    //
    QTalk::qConfig::saveConfig(_strConfPath, true, _pStLoginConfig);
    // 开始登陆
    if (_pManager != nullptr) {
        // 获取导航信息
        std::string nav = _pNavManager->getDefaultNavUrl().toStdString();

        if(nav.empty())
        {
            emit AuthFailedSignal(tr("导航不能为空!"));
        }
        //
        Platform::instance().setLoginNav(nav);
        //
        QByteArray navName = _pNavManager->getNavName().toLocal8Bit();
        Platform::instance().setNavName(navName.toStdString());
        //
        _pStsLabel->setText(tr("正在获取导航信息"));
        bool ret = _pManager->getNavInfo(nav);
        if (!ret) {
            emit AuthFailedSignal(tr("导航获取失败, 请检查网络连接!"));
            return;
        }
        // check server host
        const std::string host = NavigationManager::instance().getXmppHost();
        const int port = NavigationManager::instance().getProbufPort();
        if (port == 0 || host.empty()) {
            warn_log("nav info error (port == 0 || domain.empty())");
            emit AuthFailedSignal(tr("获取服务器地址失败!"));
            return;
        }
        // try connect to server
        _pStsLabel->setText(tr("正在尝试连接服务器"));
        std::auto_ptr<QTcpSocket> tcpSocket(new QTcpSocket);
        tcpSocket->connectToHost(host.data(), port);
        if(!tcpSocket->waitForConnected(5000))
        {
            tcpSocket->abort();
            emit AuthFailedSignal(tr("连接服务器失败!"));
            return;
        }
        tcpSocket->close();
        // login
        _pStsLabel->setText(tr("正在验证账户信息"));
        ret = _pManager->SendLoginMessage(strName.toStdString(), strPassword.toStdString());
        if (!ret) {
//            emit AuthFailedSignal(tr("账户或密码错误!"));
            return;
        }
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author   cc
  * @date     2018/11/08
  */
void LoginPanel::onAuthFailed(const QString &msg) {

    error_log("login error :{0}", msg.toStdString());
    QString tmpMsg = msg;
    tmpMsg.replace("\"", " ");
    tmpMsg.replace("\n", " ");

    _pAuthFailLabel->setText(msg);
    _pLoginFrm->setVisible(true);
    _pLogingFrm->setVisible(false);
    _pAuthFailedFrm->setVisible(true);
    _pAuthFailedSpaceFrm->setVisible(false);
}

/**
 * 根据qvt换取token
 * @param qvt
 */
void LoginPanel::getTokenByQVT(const std::string& newQvt,bool isAutoLogin){
    std::map<std::string,std::string> userMap;
    if(isAutoLogin){
        std::string currentQvt = _pManager->getQchatQvt();
        Platform::instance().setQvt(currentQvt);
        userMap = _pManager->getQchatToken(currentQvt);
    } else{
        Platform::instance().setQvt(newQvt);
        _pManager->saveQvtToDB(newQvt);
        userMap = _pManager->getQchatToken(newQvt);
    }
    _userNameEdt->setText(QString::fromStdString(userMap["name"]));
    _passworldEdt->setText(QString::fromStdString(userMap["password"]));
    _loginBtn->clicked();
}
/**
 * 根据导航地址获取domain
 * @return
 */
QString LoginPanel::getDomainByNav(const QString &nav) {
    if (_pManager) {
        return QString::fromStdString(_pManager->getNavDomain(nav.toStdString()));
    }
    return QString();
}

void LoginPanel::saveHeadPath() {
    if (_pDefaultConfig) {
        std::string selfUserId = Platform::instance().getSelfXmppId();
        std::shared_ptr<QTalk::Entity::ImUserInfo> info = dbPlatForm::instance().getUserInfo(selfUserId, true);
        if (info) {
            std::string head = QTalk::GetHeadPathByUrl(info->HeaderSrc);
            _pDefaultConfig->setAttribute(CONFIG_KEY_HEADPATH, QString::fromStdString(head));
            QTalk::qConfig::saveConfig(_strConfPath, true, _pStLoginConfig);
        }
    }
}

void LoginPanel::setHead(const QString &headPath) {
    QFileInfo info(headPath);
    if (!headPath.isEmpty() && info.exists() && info.isFile()) {
        _headPhotoLab->setHead(headPath, 52, false, true);
    } else {

#ifdef _STARTALK
        _headPhotoLab->setHead(":/QTalk/image1/StarTalk_defaultHead.png", 52, false, true);
#else
        _headPhotoLab->setHead(":/QTalk/image1/headPortrait.png", 52, false, true);
#endif
    }
}

/**
 *
 * @param errMs
 */
void LoginPanel::loginError(const std::string &errMs) {
    QString errorMsg;
    if("not-authorized" == errMs)
    {
        errorMsg = tr("账户或密码错误");
    }
    else if("out_of_date" == errMs)
    {
        errorMsg = tr("登录超时");
    }
    else
    {
        errorMsg = QString::fromStdString(errMs);
    }
    emit AuthFailedSignal(errorMsg);
}

/**
 *
 */
void LoginPanel::loginSuccess()
{
    // 监听
#ifndef _DEBUG
    QString strName = Platform::instance().getSelfUserId().data();
    QString domain = _pNavManager->getDefaultDomain();
    QString listenName = QString("%1@%2").arg(strName, domain);
    QLocalServer::removeServer(listenName);
    _pLocalServer->listen(listenName);
#endif

    if (_rememberPassBtn->isChecked()) {
        _pDefaultConfig->setAttribute(CONFIG_KEY_SAVEPASSWORD, true);
        _pDefaultConfig->setAttribute(CONFIG_KEY_AUTOLOGIN, _autoLoginBtn->isChecked());
    }
}

/**
 *
 */
void LoginPanel::enableAutoLogin(bool enable)
{
    _enableAutoLogin = enable;
}

bool LoginPanel::event(QEvent *e) {
    if(e->type() == QEvent::Show)
    {
        qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
        QString strStyle = QString("QFrame#loginMainFrm {"
                                   "border-image:url(:/login/image1/loginbj/%1.png); }").arg(qrand() % 18 + 1);
        _loginMainFrm->setStyleSheet(strStyle);
    }
    return QWidget::event(e);
}

//void LoginPanel::paintEvent(QPaintEvent *e) {
//
//    static bool hasPainted = false;
//    if(!hasPainted)
//    {
//        qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
//        int picnum = qrand() % 18 + 1;
//        QString path = QString(":/login/image1/loginbj/%1.png").arg(picnum);
//        QPainter painter(this);
//        painter.save();
//        painter.drawPixmap(_loginMainFrm->geometry(), QPixmap(path));
//        painter.restore();
//        hasPainted = true;
//    }
//    //
//    QWidget::paintEvent(e);
//}
