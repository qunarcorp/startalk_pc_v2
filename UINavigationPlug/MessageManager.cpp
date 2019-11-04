#include "MessageManager.h"
#include "../Message/ChatMessage.h"
#include "../EventBus/EventBus.h"
#include "../Message/LoginMessgae.h"
#include <QDebug>
#include "../Message/StatusMessage.h"
#include "../Message/UserMessage.h"
#include "../QtUtil/Utils/Log.h"
#include "../Platform/Platform.h"

NavigationMsgManager::NavigationMsgManager() {

}

NavigationMsgManager::~NavigationMsgManager() {

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
void NavigationMsgManager::sendDownLoadHeadPhotosMsg(std::vector<std::string> &_withoutHeadPhotos,
                                                     std::vector<std::string> &_withoutGroupHeadPhotos) {
    DownLoadHeadPhotoEvent event;
    event._withoutHeadPhotos = _withoutHeadPhotos;
    EventBus::FireEvent(event);

    DownLoadGroupHeadPhotoEvent eventg;
    eventg._withoutHeadPhotos = _withoutGroupHeadPhotos;
    EventBus::FireEvent(eventg);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.12
  */
void NavigationMsgManager::sendGetUserStatus(const std::set<std::string> &users) {
    GetUsersOnlineEvent e;
    e._users = users;
    EventBus::FireEvent(e);
}

/**
  * @函数名   retryConnecToServer
  * @功能描述 触发重连服务器
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
void NavigationMsgManager::retryConnecToServer() {
    RetryConnectToServerEvt e;
    EventBus::FireEvent(e);
}

//
void NavigationMsgManager::sendReadedMessage(const std::string &messageId, const std::string& userId, QUInt8 chatType) {
    ReadedMessage e(messageId, userId, chatType);
    EventBus::FireEvent(e);
}

void NavigationMsgManager::getSessionData() {

    GetSessionData e;
    EventBus::FireEvent(e);
}

void NavigationMsgManager::removeSession(std::string &peerId) {
    RemoveSessionData e(peerId);
    EventBus::FireEvent(e);
}

void NavigationMsgManager::setUserSetting(bool isSetting, const std::string &key, const std::string &subKey,
                                          const std::string &val) {

    info_log("setting userconfig -> issetting:{0}, key:{1}, subKey:{2}, val:{3}", isSetting, key, subKey, val);

    UserSettingMsg e;
    e.operatorType = isSetting ? UserSettingMsg::EM_OPERATOR_CANCEL : UserSettingMsg::EM_OPERATOR_SET;
    e.key = key;
    e.subKey = subKey;
    e.value = val;
    EventBus::FireEvent(e);
}

void NavigationMsgManager::addEmptyMessage(const QTalk::Entity::ImMessageInfo &info)
{
    //
    EmptyMessageEvt evt(info);
    EventBus::FireEvent(evt);
}

//
void NavigationMsgManager::quitGroupById(const std::string& groupId) {
    QuitGroupMsg e;
    e.groupId = groupId;
    EventBus::FireEvent(e);
}

/**
  * @函数名 MessageListener
  * @功能描述 构造监听器 并注册到eventbus中
  * @参数
  * @date 2018.9.20
  */
NavigationMsgListener::NavigationMsgListener(NavigationMianPanel *navigationMianPanel) :
        _pNavigationMianPanel(navigationMianPanel) {
	EventBus::AddHandler<R_Message>(*this);
	EventBus::AddHandler<S_Message>(*this);
	EventBus::AddHandler<DownloadHeadSuccess>(*this);
	EventBus::AddHandler<DownloadGroupHeadSuccess>(*this);
	EventBus::AddHandler<UpdateOnlineEvent>(*this);
	EventBus::AddHandler<GetUsersOnlineSucessEvent>(*this);
	EventBus::AddHandler<DisconnectToServer>(*this);
//	EventBus::AddHandler<RetryConnectRet>(*this);
	EventBus::AddHandler<SynOfflineSuccees>(*this);
	EventBus::AddHandler<UpdateGroupInfoRet>(*this);
	EventBus::AddHandler<SignalReadState>(*this);
	EventBus::AddHandler<GroupReadState>(*this);
	EventBus::AddHandler<RevokeMessage>(*this);
	EventBus::AddHandler<S_RevokeMessage>(*this);
	EventBus::AddHandler<UpdateUserConfigMsg>(*this);
	EventBus::AddHandler<DestroyGroupRet>(*this);
	EventBus::AddHandler<ChangeHeadRetMessage>(*this);
	EventBus::AddHandler<UserCardMessgae>(*this);
	EventBus::AddHandler<IncrementConfig>(*this);
	EventBus::AddHandler<RecvVideoMessage>(*this);
	EventBus::AddHandler<MStateEvt>(*this);
}

NavigationMsgListener::~NavigationMsgListener() {

}

/**
  * @函数名 onEvent
  * @功能描述 接受eventbus分发的消息
  * @参数
  * @date 2018.9.20
  */
void NavigationMsgListener::onEvent(R_Message &e) {
    _pNavigationMianPanel->receiveSession(e);
}

/**
  * @函数名 onEvent
  * @功能描述 接受eventbus分发的消息
  * @参数
  * @date 2018.10.26
  */
void NavigationMsgListener::onEvent(S_Message &e) {
    _pNavigationMianPanel->sendSession(e);
}


/**
  * @函数名
  * @功能描述 头像现在完刷新头像
  * @参数
  * @date 2018.9.29
  */
void NavigationMsgListener::onEvent(DownloadHeadSuccess &e) {

    if (_pNavigationMianPanel) {
        _pNavigationMianPanel->onDownLoadHeadPhotosFinish();
    }
}


/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.30
  */
void NavigationMsgListener::onEvent(DownloadGroupHeadSuccess &e) {
    if (_pNavigationMianPanel) {
        _pNavigationMianPanel->onDownLoadGroupHeadPhotosFinish();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.12
  */
void NavigationMsgListener::onEvent(UpdateOnlineEvent &e) {
    if (_pNavigationMianPanel) {
        _pNavigationMianPanel->onUpdateOnline();
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.15
  */
void NavigationMsgListener::onEvent(GetUsersOnlineSucessEvent &e) {
    if (_pNavigationMianPanel) {
        _pNavigationMianPanel->onUpdateOnlineUsers(e._userstatus);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数     DisconnectToServer 断链消息
     void
  * @author   cc
  * @date     2018/10/23
  */
void NavigationMsgListener::onEvent(DisconnectToServer &e) {
    if (e.getCanceled()) return;

    if (_pNavigationMianPanel) {
        _pNavigationMianPanel->onTcpDisconnect();
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
//void NavigationMsgListener::onEvent(RetryConnectRet &e) {
//    if (e.getCanceled()) return;
//
//    if (_pNavigationMianPanel) {
//        _pNavigationMianPanel->onRetryConnected();
//    }
//}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/24
  */
void NavigationMsgListener::onEvent(SynOfflineSuccees &e) {
    if (e.getCanceled()) {
        return;
    }
    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->onLoginSuccess();
    }
}

void NavigationMsgListener::onEvent(UpdateGroupInfoRet &e) {
    if (e.getCanceled()) {
        return;
    }
    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->onUpdateGroupInfo(e.groupinfo);
    }
}

void NavigationMsgListener::onEvent(SignalReadState &e) {

    if (e.getCanceled()) return;

    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->updateReadCount(QTalk::Entity::UID(e.userId,e.realJid), e.mapReadState.size());
    }
}

void NavigationMsgListener::onEvent(GroupReadState &e) {

    if (e.getCanceled()) return;

    if (nullptr != _pNavigationMianPanel) {
        auto it = e.mapReadCount.begin();
        for (; it != e.mapReadCount.end(); it++) {
            _pNavigationMianPanel->updateReadCount(QTalk::Entity::UID(it->first,it->first), it->second);
        }
    }
}

void NavigationMsgListener::onEvent(RevokeMessage &e) {

    if (e.getCanceled()) return;

    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->recvRevokeMessage(e.uid, e.messageFrom);
    }
}

void NavigationMsgListener::onEvent(S_RevokeMessage &e) {
    if (e.getCanceled()) return;

    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->recvRevokeMessage(e.uid, e.messageFrom);
    }
}

void NavigationMsgListener::onEvent(UpdateUserConfigMsg &e) {

    if (e.getCanceled()) return;

    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->onUpdateUserConfig(e.arConfigs);
    }
}

void NavigationMsgListener::onEvent(DestroyGroupRet &e) {
    if (e.getCanceled()) return;

    if (_pNavigationMianPanel) {
        _pNavigationMianPanel->onDestroyGroup(e.groupId);
    }
}

/**
 *
 * @param e
 */
void NavigationMsgListener::onEvent(ChangeHeadRetMessage &e)
{
    if(e.getCanceled()) return;

    if(_pNavigationMianPanel)
    {
        std::string xmppId = Platform::instance().getSelfXmppId();
        _pNavigationMianPanel->onChangeHeadRet(e.ret, xmppId, e.localHead);
    }
}

void NavigationMsgListener::onEvent(UserCardMessgae &e)
{
    if(e.getCanceled()) return;

    if(_pNavigationMianPanel)
    {
        auto userCards = e.userCards;
        for(const auto& userCard : userCards)
        {
            std::string localHead = QTalk::GetHeadPathByUrl(userCard.headerSrc);
            _pNavigationMianPanel->onChangeHeadRet(true, userCard.xmppId, localHead);
        }
    }
}

void NavigationMsgListener::onEvent(IncrementConfig &e) {
    if(e.getCanceled()) return;

    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->onUpdateUserConfig(e.deleteData, e.arImConfig);
    }
}

void NavigationMsgListener::onEvent(RecvVideoMessage &e) {
    if (nullptr != _pNavigationMianPanel) {

    }
}

void NavigationMsgListener::onEvent(MStateEvt &e) {
    if (nullptr != _pNavigationMianPanel) {
        _pNavigationMianPanel->onGotMState(QTalk::Entity::UID(e.userId, e.realJid), e.messageId.data(), e.time);
    }
}
