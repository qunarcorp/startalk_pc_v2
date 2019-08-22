#include "NavigationMianPanel.h"
#include "SessionFrm.h"
#include "MultifunctionFrm.h"
#include "ContactFrm.h"
#include <QDebug>
#include <QTimer>
#include "../UICom/UIEntity.h"
#include "MessageManager.h"
#include <QMetaType>
#include <string>
#include <QTcpSocket>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include "TcpDisconnect.h"
#include "../Platform/Platform.h"
#include "../Platform/NavigationManager.h"
#include "../Platform/dbPlatForm.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Entity/JID.h"

NavigationMianPanel::NavigationMianPanel(QWidget *parent) :
        QFrame(parent),
        _mainLayout(nullptr),
        _stackWdt(nullptr),
        _pSessionFrm(nullptr),
        _contactFrm(nullptr),
        _multifunctionFrm(nullptr),
        _messageManager(nullptr),
        _messageListener(nullptr),
        _pTcpDisconnect(nullptr),
        _pConnToServerTimer(nullptr){
    init();
    qRegisterMetaType<ReceiveSession>("ReceiveSession");
}

NavigationMianPanel::~NavigationMianPanel() {
}

void NavigationMianPanel::receiveSession(R_Message mess) 
{
	QTalk::Entity::ImMessageInfo message = mess.message;
	ReceiveSession info;
	info.chatType = (QTalk::Enum::ChatType)message.ChatType;
	info.messageId = QString::fromStdString(message.MsgId);
	info.messageContent = QString::fromStdString(message.Content);
	info.xmppId = QString::fromStdString(QTalk::Entity::JID(message.SendJid.c_str()).barename());
	info.realJid = QString::fromStdString(QTalk::Entity::JID(message.RealJid.c_str()).barename());
	info.messageRecvTime = message.LastUpdateTime;
	std::string from = QTalk::Entity::JID(message.From.c_str()).barename();
	info.sendJid = QString::fromStdString(from);
	info.messtype = message.Type;


	emit sgReceiveSession(info, from == Platform::instance().getSelfXmppId());
	//qlog_info(QString("emit sessioon id:%1").arg(info.messageId));
}

void NavigationMianPanel::sendSession(S_Message mess) {
    QTalk::Entity::ImMessageInfo message = mess.message;
    ReceiveSession info;
    info.chatType = (QTalk::Enum::ChatType) message.ChatType;
    info.messageId = QString::fromStdString(message.MsgId);
    info.messageContent = QString::fromStdString(message.Content);
    info.xmppId = QString::fromStdString(message.To);
    info.realJid = QString::fromStdString(message.RealJid);
    info.messageRecvTime = message.LastUpdateTime;
    info.messtype = message.Type;

    info.sendJid = QString::fromStdString(Platform::instance().getSelfXmppId());
    emit sgReceiveSession(info, true);
}

/**
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.17
  */
void NavigationMianPanel::init() {
    this->setFocusPolicy(Qt::NoFocus);
    initLayout();
    initMessage();
    connects();
    if (nullptr == _pConnToServerTimer) {
        _pConnToServerTimer = new QTimer(this);
        _pConnToServerTimer->setInterval(5000);
        connect(_pConnToServerTimer, SIGNAL(timeout()), this, SLOT(retryToConnect()));
        connect(this, SIGNAL(connToServerTimerSignal(bool)), this, SLOT(connToServerTimerSlot(bool)));
    }
    //
    if (_messageManager) {
        _messageManager->getSessionData();
    }
//
//
//    //Set Internet Access Point
//    QNetworkConfigurationManager manager;
//    //Is there default access point, use it
//    QNetworkConfiguration cfg = manager.defaultConfiguration();
//
//    //Open session
//    auto *_session = new QNetworkSession(cfg);
//    connect(_session, &QNetworkSession::closed, [](){
//        qDebug() << "---------------closed";
//    });
//    connect(_session, &QNetworkSession::stateChanged, [](QNetworkSession::State state){
//        qDebug() << "----------------" << state;
//    });
//    _session->open();
//    _session->waitForOpened();
}

/**
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.18
  */
void NavigationMianPanel::initLayout() {
    this->setObjectName("navigationMianPanel");
    if (!_mainLayout) {
        _mainLayout = new QVBoxLayout(this);
        _mainLayout->setSpacing(0);
        _mainLayout->setMargin(0);
    }
    if (nullptr == _pTcpDisconnect) {
        _pTcpDisconnect = new TcpDisconnect(this);
        _mainLayout->addWidget(_pTcpDisconnect);
        _pTcpDisconnect->setVisible(false);
    }
    if (!_stackWdt) {
        _stackWdt = new QStackedWidget(this);
        _mainLayout->addWidget(_stackWdt);
    }
    if (!_pSessionFrm) {
        _pSessionFrm = new SessionFrm(this);
    }
    if (_pSessionFrm) {
        _stackWdt->addWidget(_pSessionFrm);
    }

    if (!_contactFrm) {
        _contactFrm = new ContactFrm;
    }
    if (_contactFrm) {
        _stackWdt->addWidget(_contactFrm);
    }

    if (!_multifunctionFrm) {
        _multifunctionFrm = new MultifunctionFrm;
    }
    if (_multifunctionFrm) {
        _stackWdt->addWidget(_multifunctionFrm);
    }

    _stackWdt->setMinimumWidth(260);
    _stackWdt->setCurrentIndex(0);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.20
  */
void NavigationMianPanel::initMessage() {
    if (!_messageManager) {
        _messageManager = new NavigationMsgManager;
        _pSessionFrm->setMessageManager(_messageManager);
    }
    if (!_messageListener) {
        _messageListener = new NavigationMsgListener(this);
    }
    if (_pSessionFrm) {
    }
}

/**
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.18
  */
void NavigationMianPanel::connects() {
    qRegisterMetaType<QTalk::Entity::UID>("QTalk::Entity::UID");

    qRegisterMetaType<QTalk::StGroupInfo>("QTalk::StGroupInfo");
    qRegisterMetaType<std::map<std::string, std::string>>("std::map<std::string,std::string>");
    qRegisterMetaType<ReceiveSession>("ReceiveSession");
    qRegisterMetaType<StSessionInfo>("StSessionInfo");

    connect(_pSessionFrm, SIGNAL(sgSessionInfo(StSessionInfo)),
            this, SIGNAL(sgSessionInfo(StSessionInfo)));
    connect(this, &NavigationMianPanel::sgReceiveSession, _pSessionFrm, &SessionFrm::onReceiveSession);
    connect(this, SIGNAL(sgUpdateOnline()), _pSessionFrm, SLOT(onUpdateOnline()));
    connect(this, SIGNAL(sgUpdateOnlineUsers(std::map<std::string, std::string>)),
            _pSessionFrm, SLOT(onUpdateOnlineUsers(std::map<std::string, std::string>)));
    connect(this, SIGNAL(sgDownLoadHeadPhotosFinish()), _pSessionFrm, SLOT(onDownLoadHeadPhotosFinish()));
    connect(this, SIGNAL(sgDownLoadGroupHeadPhotosFinish()), _pSessionFrm, SLOT(onDownLoadGroupHeadPhotosFinish()));
    connect(this, &NavigationMianPanel::setDisconnectWgtVisible, _pTcpDisconnect, &TcpDisconnect::setVisible);
    connect(_pSessionFrm, &SessionFrm::showUserCard, this, &NavigationMianPanel::onSendShowCardSigal);
    connect(this, &NavigationMianPanel::updateGroupInfoSignal, _pSessionFrm, &SessionFrm::onUpdateGroupInfo);
    connect(this, &NavigationMianPanel::updateReadedCountSignal, _pSessionFrm, &SessionFrm::onUpdateReadedCount);
    connect(this, &NavigationMianPanel::recvRevokeMessageSignal, _pSessionFrm, &SessionFrm::recvRevikeMessage);
    connect(this, &NavigationMianPanel::loadSession, _pSessionFrm, &SessionFrm::onloadSessionData);
    qRegisterMetaType<QTalk::Entity::UID>("QTalk::Entity::UID");
    connect(_pSessionFrm, &SessionFrm::removeSession, this, &NavigationMianPanel::removeSession);
    connect(_pSessionFrm, &SessionFrm::removeSession, this, &NavigationMianPanel::removeSessionAction);
    connect(this, &NavigationMianPanel::destoryGroupSignal, _pSessionFrm, &SessionFrm::onDestroyGroup);
    connect(this, &NavigationMianPanel::sgShortCutSwitchSession, _pSessionFrm, &SessionFrm::onShortCutSwitchSession);
    connect(this, &NavigationMianPanel::sgChangeUserHead, _pSessionFrm, &SessionFrm::onUserHeadChange);
    connect(this, &NavigationMianPanel::sgUserConfigChanged, _pSessionFrm, &SessionFrm::onUserConfigChanged);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/26
  */
void NavigationMianPanel::connToServerTimerSlot(bool sts) {
    if (nullptr != _pConnToServerTimer) {
        // 如果是断链状态 则重新连接后 重现加载session
        if (_conneted && _pConnToServerTimer->isActive())
            emit loadSession();
        sts ? _pConnToServerTimer->start() : _pConnToServerTimer->stop();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.29
  */
void NavigationMianPanel::onDownLoadHeadPhotosFinish() {
    emit sgDownLoadHeadPhotosFinish();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.30
  */
void NavigationMianPanel::onDownLoadGroupHeadPhotosFinish() {
    emit sgDownLoadGroupHeadPhotosFinish();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.12
  */
void NavigationMianPanel::onUpdateOnline() {
    emit sgUpdateOnline();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.15
  */
void NavigationMianPanel::onUpdateOnlineUsers(const std::map<std::string, std::string> &userstatus) {
    emit sgUpdateOnlineUsers(userstatus);
}

/**
  * @函数名   onTcpDisconnect
  * @功能描述 断线处理
  * @参数
  * @author   cc
  * @date     2018/10/23
  */
void NavigationMianPanel::onTcpDisconnect() {
    _conneted = false;
    emit setDisconnectWgtVisible(true);
//    emit connToServerTimerSignal(true);
    // 立即重连一次
    retryToConnect();
}

/**
  * @函数名   retryToConnect
  * @功能描述 触发重连
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
void NavigationMianPanel::retryToConnect() {

    if (nullptr != _pConnToServerTimer && _pConnToServerTimer->isActive()) {
        _pConnToServerTimer->stop();
    }

    emit connToServerTimerSignal(false);
    // check server host
    const std::string host = NavigationManager::instance().getXmppHost();
    const int port = NavigationManager::instance().getProbufPort();
    if (port == 0 || host.empty()) {
        warn_log("nav info error (port == 0 || domain.empty())");
        emit connToServerTimerSignal(true);
        return;
    }
    //
    if(_pTcpDisconnect)
        _pTcpDisconnect->setText(QStringLiteral("正在重连"));

    if (nullptr != _messageManager) {
        std::thread([this, host, port](){
            // try connect to server
            std::auto_ptr<QTcpSocket> tcpSocket(new QTcpSocket);
            tcpSocket->connectToHost(host.data(), port);
            if(!tcpSocket->waitForConnected(5000))
            {
                tcpSocket->abort();
                emit connToServerTimerSignal(true);
                return;
            }
            tcpSocket->close();
            // 重连
            _messageManager->retryConnecToServer();
        }).detach();
    }
    //
    QTimer::singleShot(6000, [this](){
        if(!_conneted)
        {
            if(_pTcpDisconnect)
                _pTcpDisconnect->setText(QStringLiteral("当前网络不可用"));

            emit connToServerTimerSignal(true);
        }
    });
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.11.08
  */
void NavigationMianPanel::onNewSession(const StSessionInfo &into) {
    if (_pSessionFrm) {
        if (!Platform::instance().isMainThread()) {
            ////info_log("NavigationMianPanel::onNewSession");
            throw std::runtime_error("not main thread");
        }
        _pSessionFrm->onNewSession(into);
    }
}

///**
//  * @函数名   onRetryConnected
//  * @功能描述
//  * @参数
//  * @author   cc
//  * @date     2018/10/24
//  */
//void NavigationMianPanel::onRetryConnected() {
//    if (nullptr != _pTcpDisconnect) {
//        _pTcpDisconnect->onRetryConnected();
//        emit connToServerTimerSignal(true);
//    }
//}

/**
  * @函数名   onLoginSuccess
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
void NavigationMianPanel::onLoginSuccess() {
    {
        QMutexLocker locker(&_pSessionFrm->_mutex);
        _pSessionFrm->pSessions = dbPlatForm::instance().QueryImSessionInfos();
        _conneted = true;
    }
    emit setDisconnectWgtVisible(false);
    emit connToServerTimerSignal(false);
}

void NavigationMianPanel::onSendShowCardSigal(const QString &userId) {
    emit showUserCardSignal(userId);
}

//
void NavigationMianPanel::onUpdateGroupInfo(std::shared_ptr<QTalk::StGroupInfo> info) {
    emit updateGroupInfoSignal(*info);
}

void NavigationMianPanel::updateReadCount(const QTalk::Entity::UID& uid, const int &count) {
    emit updateReadedCountSignal(uid, count);
}

void NavigationMianPanel::recvRevokeMessage(const QTalk::Entity::UID& uid, const std::string &from) {
    emit recvRevokeMessageSignal(uid, QString::fromStdString(from));
}

void NavigationMianPanel::onUpdateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs) {

    if (nullptr == _pSessionFrm) {
        return;
    }
    //
    std::map<std::string, std::string> tmpStick;
    std::map<std::string, std::string> tmpNotice;
    std::vector<std::string> arSatr;
    std::vector<std::string> arBlackList;
    auto it = arConfigs.begin();
    for (; it != arConfigs.end(); it++) {
        //info_log("onUpdateUserConfig {0} {1} {2}", it->ConfigKey, it->ConfigSubKey, it->ConfigValue);
        if (it->ConfigKey == "kStickJidDic") {
            QString xmppId = QString::fromStdString(it->ConfigSubKey);
            tmpStick[xmppId.toStdString()] = it->ConfigValue;
        } else if (it->ConfigKey == "kNoticeStickJidDic") {
            tmpNotice[it->ConfigSubKey] = it->ConfigValue;
        } else if (it->ConfigKey == "kStarContact") {
            arSatr.push_back(it->ConfigSubKey);
        } else if (it->ConfigKey == "kBlackList") {
            arBlackList.push_back(it->ConfigSubKey);
        }
    }
    //
    {
        QMutexLocker locker(&_pSessionFrm->_mutex);
        _pSessionFrm->_mapStick = tmpStick;
        _pSessionFrm->_mapNotice = tmpNotice;
        _pSessionFrm->_arSatr = arSatr;
        _pSessionFrm->_arBlackList = arBlackList;
        if(nullptr == _pSessionFrm->pSessions)
            _pSessionFrm->pSessions = dbPlatForm::instance().QueryImSessionInfos();
        else
            _pSessionFrm->pSessions = dbPlatForm::instance().reloadSession();
    }
    emit loadSession();
}

void NavigationMianPanel::onUpdateUserConfig(const std::map<std::string, std::string> &deleteData,
                                             const std::vector<QTalk::Entity::ImConfig>& arImConfig)
{
    QMutexLocker locker(&_pSessionFrm->_mutex);
    for(const auto& it : deleteData)
    {
        QTalk::Entity::UID uid(it.first);
        //
        if(it.second == "kStickJidDic")
        {
            _pSessionFrm->_mapStick.erase(it.first);
        }
        else if(it.second == "kNoticeStickJidDic")
        {
            _pSessionFrm->_mapNotice.erase(it.first);
        }
        else if(it.second == "kStarContact")
        {
            auto itFind = std::find(_pSessionFrm->_arSatr.begin(), _pSessionFrm->_arSatr.end(), it.first);
            if(itFind != _pSessionFrm->_arSatr.end())
                _pSessionFrm->_arSatr.erase(itFind);
        }
        else if(it.second == "kBlackList")
        {
            auto itFind = std::find(_pSessionFrm->_arBlackList.begin(), _pSessionFrm->_arBlackList.end(), it.first);
            if(itFind != _pSessionFrm->_arBlackList.end())
                _pSessionFrm->_arBlackList.erase(itFind);
        }
        else {}
        //
        emit sgUserConfigChanged(uid);
    }
    //
    for(const auto& conf : arImConfig)
    {
        QTalk::Entity::UID uid(conf.ConfigSubKey);

        if(conf.ConfigKey == "kStickJidDic")
            _pSessionFrm->_mapStick[conf.ConfigSubKey] = conf.ConfigValue;
        else if(conf.ConfigKey == "kNoticeStickJidDic")
            _pSessionFrm->_mapNotice[conf.ConfigSubKey] = conf.ConfigValue;
        else if(conf.ConfigKey == "kStarContact")
            _pSessionFrm->_arSatr.push_back(conf.ConfigSubKey);
        else if(conf.ConfigKey == "kBlackList")
            _pSessionFrm->_arBlackList.push_back(conf.ConfigSubKey);
        else {}
        //
        emit sgUserConfigChanged(uid);
    }
}

void NavigationMianPanel::onDestroyGroup(const std::string &groupId) {
    emit destoryGroupSignal(QString::fromStdString(groupId));
}

/**
 *
 */
void NavigationMianPanel::jumpToNewMessage() {
    if (!Platform::instance().isMainThread()) {
        ////info_log("NavigationMianPanel::jumpToNewMessage");
        throw std::runtime_error("not main thread");
    }
    if (_pSessionFrm)
        _pSessionFrm->jumpToNewMessage();
}

/**
 *
 */
void NavigationMianPanel::onShortCutSwitchSession(int key) {
    emit sgShortCutSwitchSession(key);
}

void NavigationMianPanel::removeSessionAction(const QTalk::Entity::UID& uid) {
    if (_messageManager) {
        std::string peerIdName = uid.usrId();
        _messageManager->removeSession(peerIdName);
    }
}

/**
 *
 * @param ret
 * @param localHead
 */
void NavigationMianPanel::onChangeHeadRet(bool ret,  const std::string& xmppId, const std::string &localHead)
{
    if(ret)
    {
        emit sgChangeUserHead(QString::fromStdString(xmppId), QString::fromStdString(localHead));
    }
}

void NavigationMianPanel::updateTatalReadCount()
{
    emit updateTotalUnreadCount(_pSessionFrm->getAllCount());
}
