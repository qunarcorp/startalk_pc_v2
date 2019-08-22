#include "MainPanel.h"

#include <QButtonGroup>
#include <QMouseEvent>
#include "MessageManager.h"
#include "../Platform/Platform.h"
#include "../UICom/uicom.h"
#include "SessionBtn.h"
#include <thread>
#include <QDebug>
#include "../include/Line.h"

#include <QProcess>
#include <QApplication>
#include <QDir>
#include "../CustomUi/QtMessageBox.h"
#include "ChangeHeadWnd.h"

MainPanel::MainPanel(QWidget *parent) :
        QFrame(parent),
        _pCtrlWdt(nullptr),
        _pSearchResultPanel(nullptr),
        _pAboutWnd(nullptr),
        _pSystemSettingWnd(nullptr) {
    init();
    connects();
    getSelfCard();

//    this->setAutoFillBackground(false);
}

MainPanel::~MainPanel() = default;

/**
  * @功能描述 设置工具栏操控面板
  * @参数
  * @date 2018.9.17
  */
void MainPanel::setCtrlWdt(QWidget *wdt) {
    _pCtrlWdt = wdt;
    _pCtrlWdt->installEventFilter(this);
}

void MainPanel::getSelfCard() {
    std::thread([this]() {
#ifdef _MACOS
        pthread_setname_np("MainPanel::getSelfCard");
#endif
        if (_pMessageManager) {
            _pMessageManager->getUserCard(Platform::instance().getSelfDomain(),
                                          Platform::instance().getSelfUserId(), 0);
        }
    }).detach();
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
void MainPanel::recvUserCard(const std::vector<QTalk::StUserCard> &userCards) {
    std::string strSelfId = Platform::instance().getSelfUserId() + "@" + Platform::instance().getSelfDomain();

    auto itFind = std::find_if(userCards.begin(), userCards.end(), [&](const QTalk::StUserCard user) {
        return user.xmppId == strSelfId;
    });
    if (itFind != userCards.end() && _pMessageManager) {
        std::string headPath = _pMessageManager->getHeadPath(itFind->headerSrc);
        if (!headPath.empty() && _userBtn) {
            emit setHeadSignal(QString::fromStdString(itFind->nickName), QString::fromStdString(headPath));
        }
    }
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void MainPanel::mousePressEvent(QMouseEvent *me) {

    _press = true;
    chickPos = QCursor::pos();
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void MainPanel::mouseMoveEvent(QMouseEvent *me) {
    if(_press)
    {
        if (_pCtrlWdt && _pCtrlWdt->isMaximized()) {
            _pCtrlWdt->setWindowState(_pCtrlWdt->windowState() & ~Qt::MaximumSize);
            _press = false;
            return;
        }

        QPoint curPos = QCursor::pos();
        QPoint movePos = curPos - chickPos;
        chickPos = curPos;
        if (_pCtrlWdt) {
			_pCtrlWdt->move(_pCtrlWdt->x() + movePos.x(), _pCtrlWdt->y() + movePos.y());
		}
    }
}

void MainPanel::mouseReleaseEvent(QMouseEvent *m)
{
    _press = false;
}

/**
  * @功能描述 初始化布局面板各控件
  * @参数
  * @date 2018.9.17
  */
void MainPanel::init() {
    _pMessageManager = new TitlebarMsgManager;
    _pMessageListener = new TitlebarMsgListener(this);

    adaptiveResolution();
    this->setObjectName("titleMainFrm");
    this->setFixedHeight(_property._mainFrmHeight);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //
    _minimizeBtn = new QToolButton(this);
    _maximizeBtn = new QToolButton(this);
    _closeBtn = new QToolButton(this);
    _restoreBtn = new QToolButton(this);
    _minimizeBtn->setFixedSize(_property._maxminBtnsSize);
    _maximizeBtn->setFixedSize(_property._maxminBtnsSize);
    _restoreBtn->setFixedSize(_property._maxminBtnsSize);
    _closeBtn->setFixedSize(_property._maxminBtnsSize);
    _maxminFrm = new QFrame(this);
    auto *maxminLay = new QHBoxLayout(_maxminFrm);
#ifdef _MACOS
    maxminLay->addWidget(_closeBtn);
    maxminLay->addWidget(_minimizeBtn);
    maxminLay->addWidget(_maximizeBtn);
    maxminLay->addWidget(_restoreBtn);
#else
    maxminLay->addWidget(_minimizeBtn);
    maxminLay->addWidget(_maximizeBtn);
    maxminLay->addWidget(_restoreBtn);
    maxminLay->addWidget(_closeBtn);
#endif
    maxminLay->setMargin(0);
    maxminLay->setSpacing(0);
    //
    _hbox = new QHBoxLayout(this);
    _hbox->setMargin(0);
    _hbox->setSpacing(0);
    _leftCorFrm = new QFrame(this);
    _leftCorFrm->setFixedWidth(20);
    _leftCorFrm->setObjectName("leftCorFrm");
    _hbox->addWidget(_leftCorFrm);
#ifdef _MACOS
    _leftCorFrm->setFixedWidth(12);
    _maxminFrm->setFixedWidth(80);
    _hbox->addWidget(_maxminFrm);
    _maximizeBtn->setObjectName("macmaximizeBtn");
    _closeBtn->setObjectName("gmCloseBtn");
    _minimizeBtn->setObjectName("macminimizeBtn");
    _restoreBtn->setObjectName("macmaximizeBtn");
    _restoreBtn->setVisible(false);
    maxminLay->setContentsMargins(0, 0, 16, 0);
#endif
    _searchFrm = new SeachEditPanel();
    _searchFrm->setFixedSize(_property._searchFrmSize);
    _searchFrm->setObjectName(QStringLiteral("searchFrm"));
#ifdef _MACOS
    _searchFrm->setFixedWidth(150);
#endif
    _hbox->addWidget(_searchFrm);
    _hbox->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Fixed));

    _creatGroupBtn = new QToolButton(this);
    _creatGroupBtn->setObjectName(QStringLiteral("creatGroupBtn"));
    _creatGroupBtn->setFixedSize(_property._qrCodeBtnSize);
    _creatGroupBtn->setToolTip(tr("快速建群"));
    _hbox->addWidget(_creatGroupBtn);
    _hbox->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));

    auto *midLay = new QHBoxLayout;

    auto *tabGroup = new QButtonGroup(this);
    _sessionBtn = new SessionBtn(this);
    _sessionBtn->setObjectName(QStringLiteral("sessionBtn"));
    _sessionBtn->setFixedSize(40, 30);
    _sessionBtn->setCheckable(true);
    _contactBtn = new QToolButton(this);
    _contactBtn->setObjectName(QStringLiteral("contactBtn"));
    _contactBtn->setFixedSize(_property._contactBtnSize);
    _contactBtn->setCheckable(true);
    _multifunctionBtn = new QToolButton(this);
    _multifunctionBtn->setObjectName(QStringLiteral("multifunctionBtn"));
    _multifunctionBtn->setFixedSize(_property._multifunctionBtnSize);
    _multifunctionBtn->setCheckable(true);

    tabGroup->addButton(_sessionBtn, 0);
    tabGroup->addButton(_contactBtn, 1);
    tabGroup->addButton(_multifunctionBtn, 2);
    tabGroup->setExclusive(true);

    midLay->addWidget(_sessionBtn);
    midLay->addWidget(_contactBtn);
    midLay->addWidget(_multifunctionBtn);
    midLay->setMargin(0);
    midLay->setSpacing(65);
    _hbox->addLayout(midLay);
    _hbox->addItem(new QSpacerItem(40, 1, QSizePolicy::Expanding));

#ifdef _MACOS
    _hbox->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed));
#endif

    _userBtn = new HeadPhotoLab;
    _userBtn->setObjectName("userBtn");
#ifdef _STARTALK
    QString defaultHead(":/QTalk/image1/StarTalk_defaultHead.png");
#else
    QString defaultHead(":/QTalk/image1/headPortrait.png");
#endif
    _userBtn->setHead(defaultHead, 18, false, false);
    _userBtn->installEventFilter(this);
    _hbox->addWidget(_userBtn);
    _hbox->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Fixed));

#ifndef _MACOS
    _maxminFrm->setFixedWidth(120);
    _hbox->addWidget(_maxminFrm);
    _minimizeBtn->setObjectName("minimizeBtn");
    _maximizeBtn->setObjectName("maximizeBtn");
    _closeBtn->setObjectName("closeBtn");
    _restoreBtn->setObjectName("restoreBtn");
#endif // !_MACOS
    _pSearchResultPanel = new SearchResultPanel(_pMessageManager, _pMessageListener, this);
    //
    _pChangeHeadWnd = new ChangeHeadWnd(this);

    //
    _dropMenu = new DropMenu(this);
    _pAboutWnd = new AboutWnd(this);
    _pSystemSettingWnd = new SystemSettingWnd(_pMessageManager,this);
    this->setFocusProxy(this);
    //
    connect(_closeBtn, SIGNAL(clicked()), this, SLOT(onCtrlWdtClose()));
    connect(_minimizeBtn, SIGNAL(clicked()), this, SLOT(showSmall()));
    connect(_maximizeBtn, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
    connect(_restoreBtn, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
    _restoreBtn->hide();

    connect(tabGroup, SIGNAL(buttonClicked(int)), this, SLOT(onTabGroupClicked(int)));
    connect(_pSearchResultPanel, SIGNAL(sgOpenNewSession(StSessionInfo)), this, SLOT(onOpenNewSession(StSessionInfo)));
    connect(this, &MainPanel::setHeadSignal, this, &MainPanel::setNewHead);
    connect(_dropMenu, &DropMenu::sgShowAboutWnd, [this](){
        if(nullptr!= _pAboutWnd)
        {
            _pAboutWnd->showCenter(true, _pCtrlWdt);
        }
    });
    connect(_dropMenu, &DropMenu::sgShowSystemSetting, [this](){
        if (nullptr != _pSystemSettingWnd) {
//            _pSystemSettingWnd->showCenter(false, _pCtrlWdt);
            _pSystemSettingWnd->setVisible(false);
            _pSystemSettingWnd->setVisible(true);
        }
    });
    connect(_pSystemSettingWnd, &SystemSettingWnd::sgSetAutoLogin, this, &MainPanel::sgSetAutoLogin);
    connect(_pSystemSettingWnd, &SystemSettingWnd::sgFeedbackLog, this, &MainPanel::feedbackLog);

	connect(_pSystemSettingWnd, &SystemSettingWnd::saveConfig, [this]() {
		if (_pMessageManager)
		{
            emit sgSaveSysConfig();
            _pMessageManager->saveConfig();
		}
	});
    connect(_pSystemSettingWnd, &SystemSettingWnd::sgClearSystemCache, [this](){
        int ret = QtMessageBox::question(this, "友情提示", "是否要清除应用缓存，清除后应用会自动重启？", QtMessageBox::EM_BUTTON_YES | QtMessageBox::EM_BUTTON_NO);
        if(ret == QtMessageBox::EM_BUTTON_YES)
        {
            if (_pMessageManager)
                _pMessageManager->clearSystemCache();

            // 重启应用
            QString program = QApplication::applicationFilePath();
            QProcess::startDetached(program);
            exit(0);
        }
    });

    connect(_pChangeHeadWnd, &ChangeHeadWnd::sgChangeHead, [this](const QString& headPath){

        if(headPath.isEmpty() || headPath == _headPath)
            return;

        if(_pMessageManager)
        {
            _pMessageManager->changeUserHead(headPath.toLocal8Bit().toStdString());
        }

    });

    connect(_pSearchResultPanel, &SearchResultPanel::sgSetEditFocus, [this](){
        this->activateWindow();
        _searchFrm->_searchEdt->setFocus();
        emit sgOperator("搜索");
    });

    connect(_creatGroupBtn, &QToolButton::clicked, [this](){
        QString userCard = QString::fromStdString(Platform::instance().getSelfXmppId());
        emit creatGroup(userCard);
    });

    _sessionBtn->setChecked(true);
}

void MainPanel::onTabGroupClicked(int tab)
{
    QString desc;
    switch (tab)
    {
        case 0:
            desc = "tab - 会话";
            break;
        case 1:
            desc = "tab - 联系人";
            break;
        case 2:
            desc = "tab - 应用";
            break;
        default:
            break;
    }
    emit sgOperator(desc);
    emit sgCurFunChanged(tab);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void MainPanel::connects() {
    connect(_searchFrm, &SeachEditPanel::sgIsOpenSearch, this, &MainPanel::onSearchResultVisible);

    connect(_searchFrm, &SeachEditPanel::sgSelectUp, _pSearchResultPanel, &SearchResultPanel::sgSelectUp);
    connect(_searchFrm, &SeachEditPanel::sgSelectDown, _pSearchResultPanel, &SearchResultPanel::sgSelectDown);
    connect(_searchFrm, &SeachEditPanel::sgSelectItem, _pSearchResultPanel, &SearchResultPanel::sgSelectItem);
    connect(_searchFrm, &SeachEditPanel::sgKeyEsc, [this](){
        if(_pSearchResultPanel->isVisible())
            _pSearchResultPanel->setVisible(false);
    });

    connect(_pSearchResultPanel, &SearchResultPanel::sgOpenSearch, this, &MainPanel::onSearchResultVisible);
    connect(_searchFrm, &SeachEditPanel::sgStartSearch, _pSearchResultPanel, &SearchResultPanel::addSearchReq);

    connect(_sessionBtn, &SessionBtn::clicked, this, &MainPanel::sgJumpToNewMessage);
    connect(_dropMenu, &DropMenu::showSelfUserCard, [this]() {
        QString userCard = QString::fromStdString(Platform::instance().getSelfXmppId());
        emit showSelfUserCard(userCard);
    });
    connect(_dropMenu, &DropMenu::sysQuit, this, &MainPanel::systemQuitSignal);

    connect(_dropMenu, &DropMenu::sgCheckUpdate, this, &MainPanel::sgCheckUpdate);
    connect(_pSystemSettingWnd, &SystemSettingWnd::sgCheckUpdate, this, &MainPanel::sgCheckUpdate);
    connect(_pSystemSettingWnd, &SystemSettingWnd::sgUpdateHotKey, this, &MainPanel::sgUpdateHotKey);
    connect(_pSystemSettingWnd, &SystemSettingWnd::msgSoundChanged, this, &MainPanel::msgSoundChanged);
    //
    connect(_dropMenu, &DropMenu::sgSwitchUserStatus, this, &MainPanel::onSwitchUserStatus);
    connect(this, &MainPanel::sgSwitchUserStatusRet, _dropMenu, &DropMenu::onSwitchUserStatusRet, Qt::QueuedConnection);
}

/**
 *
 */
void MainPanel::onSwitchUserStatus(const QString& status)
{

    _dropMenu->setVisible(false);
    std::thread([this, status](){

        if(_pMessageManager)
        {
            _pMessageManager->chanegUserStatus(status.toStdString());
        }

    }).detach();
}

/**
  * @功能描述  适应分辨率
  * @参数
  * @date 2018.9.17
  */
void MainPanel::adaptiveResolution() {
    // 这里需要根据分辨率设置不同属性值
    // _property;
}

void MainPanel::showSmall() {

    if (_pCtrlWdt)
    {
#ifdef _MACOS
//        _pCtrlWdt->setWindowFlags(_pCtrlWdt->windowFlags() & ~Qt::FramelessWindowHint);
#endif
        _pCtrlWdt->showMinimized();
    }

}

void MainPanel::showMaxRestore() {
    if (_pCtrlWdt) {

#ifndef _MACOS
        if (_pCtrlWdt->isMaximized()) {
            _restoreBtn->hide();
            _maximizeBtn->show();
            _pCtrlWdt->showNormal();
        } else {
            _restoreBtn->show();
            _maximizeBtn->hide();
            _pCtrlWdt->showMaximized();
        }
#else
        if (_pCtrlWdt->isFullScreen()) {
            _pCtrlWdt->showNormal();
        }
        else
        {
            _pCtrlWdt->showFullScreen();
        }
#endif
    }
}

void MainPanel::onCtrlWdtClose() {

    if (nullptr == _pCtrlWdt) return;

    if(_pCtrlWdt->isFullScreen())
        _pCtrlWdt->showNormal();
    else
        _pCtrlWdt->setVisible(false);
}

bool MainPanel::eventFilter(QObject *o, QEvent *e) {
    if (o == _userBtn && e->type() == QEvent::MouseButtonPress) {
        if (_dropMenu) {
            QPoint pos = _userBtn->geometry().bottomLeft();
            _dropMenu->move(mapToGlobal(QPoint(pos.x() - _dropMenu->width() / 2, pos.y() - 8)));
            _dropMenu->show();
        }
    }
    else if(o == _pCtrlWdt && e->type() == QEvent::WindowStateChange)
    {

    }
    else if(o == _pSearchResultPanel)
    {

    }

    return QFrame::eventFilter(o, e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.12.06
  */
void MainPanel::onSearchResultVisible(const bool &visible) {
    if (_pSearchResultPanel) {
        if (visible) {
            QWidget *wgt = UICom::getInstance()->getAcltiveMainWnd();
            if (nullptr == wgt)
                return;

            QPoint pos1 = QPoint(_leftCorFrm->rect().x(), _leftCorFrm->rect().bottom());
            QPoint pos = _leftCorFrm->mapToGlobal(pos1);
            pos = QPoint(pos.x(), pos.y());
            _pSearchResultPanel->setFixedHeight(wgt->height() - 80);
            _pSearchResultPanel->setVisible(true);
            _pSearchResultPanel->move(pos);
//            _searchFrm->setEditFocus();
        } else {
            _pSearchResultPanel->close();
        }
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.06
  */
void MainPanel::onMousePressGolbalPos(const QPoint &gpos) {
    if (_searchFrm && _pSearchResultPanel) {
        QPoint startpos = _searchFrm->mapToGlobal(QPoint(0, 0));
        QRect _searchFrmGR = QRect(startpos, QSize(_searchFrm->width(), _searchFrm->height()));

        startpos = _pSearchResultPanel->mapToGlobal(QPoint(0, 0));
        QRect _searchResultGR = QRect(startpos, QSize(_pSearchResultPanel->width(), _pSearchResultPanel->height()));

        if (_pSearchResultPanel->isVisible() && !_searchResultGR.contains(gpos) && !_searchFrmGR.contains(gpos)) {
            _searchFrm->closeSearch();
            _pSearchResultPanel->closeSearch();
            _pSearchResultPanel->hide();
            this->setFocus();
        }

#ifdef _WINDOWS
        startpos = _dropMenu->mapToGlobal(QPoint(0, 0));
        QRect dropMenuRect = QRect(startpos, QSize(_dropMenu->width(), _dropMenu->height()));
        if (_dropMenu->isVisible() && !dropMenuRect.contains(gpos))
        {
            _dropMenu->setVisible(false);
        }
#endif // _WINDOWS

    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void MainPanel::onOpenNewSession(const StSessionInfo &into) {
    _searchFrm->closeSearch();
    _pSearchResultPanel->closeSearch();
    _pSearchResultPanel->hide();
    emit sgOpenNewSession(into);
}

void MainPanel::onAppDeactivated() {

    if (_pSearchResultPanel && _searchFrm) {
        _searchFrm->closeSearch();
        _pSearchResultPanel->closeSearch();
        _pSearchResultPanel->hide();
        _pSearchResultPanel->setVisible(false);
    }
#ifdef _WINDOWS
    if (_dropMenu->isVisible())
    {
        _dropMenu->setVisible(false);
    }
#endif
}

/**
 *
 * @param s
 */
void MainPanel::onSwitchFunc(int s) {
    switch (s) {
        case 0: {
            _sessionBtn->click();
            break;
        }
        case 1: {
            _contactBtn->click();
            break;
        }
        case 2: {
            _multifunctionBtn->click();
            break;
        }
        default:break;
    }
}

/**
 *
 * @param count
 */
void MainPanel::updateUnreadCount(int cout) {
    _sessionBtn->setUnreadCount(cout);
}

void MainPanel::mouseDoubleClickEvent(QMouseEvent *e) {

    if(_pCtrlWdt)
    {
        if(_pCtrlWdt->isMaximized())
            _pCtrlWdt->showNormal();
        else if(_pCtrlWdt->isFullScreen())
            return;
        else
            _pCtrlWdt->showMaximized();
    }
    QWidget::mouseDoubleClickEvent(e);
}

/**
 *
 * @param newHeadPath
 */
void MainPanel::setNewHead(const QString &userName, const QString &newHeadPath) {
    _headPath = newHeadPath;
    _userBtn->setHead(newHeadPath, 18, false, true);
    _dropMenu->setName(userName);
    _dropMenu->setHead(newHeadPath);
}

/**
 *
 * @param ret
 * @param locaHead
 */
void MainPanel::onChangeHeadRet(bool ret, const std::string &locaHead)
{
    if(ret)
    {
        std::string userName = Platform::instance().getSelfName();
        emit setHeadSignal(QString::fromStdString(userName), QString::fromStdString(locaHead));
        emit sgOperator("更换头像");
        //_userBtn->setHead(QString::fromStdString(locaHead), 18, false, true);
    }
}

/**
 *
 * @param xmppId
 * @param isSelf
 */
void MainPanel::onShowHeadWnd(const QString &headPath, bool isSelf)
{
    if(_pChangeHeadWnd)
    {
        emit sgOperator("查看头像");
        if(isSelf)
            _pChangeHeadWnd->onChangeHeadWnd(headPath);
        else
            _pChangeHeadWnd->onShowHead(headPath);
    }
}

void MainPanel::recvSwitchUserStatus(const std::string& sts)
{
    emit sgAutoReply(sts == "away");
    emit sgSwitchUserStatusRet(QString::fromStdString(sts));
}