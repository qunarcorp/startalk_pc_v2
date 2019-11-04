#include "MessageManager.h"

#include <utility>
#include "MainPanel.h"
#include "../Message/UserMessage.h"
#include "../EventBus/EventBus.h"
#include "../Message/ChatMessage.h"
#include "../Message/LogicBaseMessage.h"

/**
  * 发送消息类
  * @author   cc
  * @date     2018/09/29
  */
TitlebarMsgManager::TitlebarMsgManager()
{

}


TitlebarMsgManager::~TitlebarMsgManager()
{

}

/**
  * @函数名   getUserCard
  * @功能描述 获取用户名片
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
void TitlebarMsgManager::getUserCard(const std::string &domain, const std::string& userName, const int& version)
{
	GetUserCardMessage e;
	e.mapUserIds[domain][userName] = version;
	EventBus::FireEvent(e);
}

std::string TitlebarMsgManager::uploadImage(const std::string& localFilePath)
{
    LocalImgEvt e;
    e.localFilePath = localFilePath;
    EventBus::FireEvent(e, false);

    return e.netFilePath;
}

void TitlebarMsgManager::sendPostReq(const std::string &url, const std::string &params,
        std::function<void(int code, const std::string &responseData)> callback)
{
    S_AddHttpQeq req;
    req.request.url = url;
    req.request.method = 1;
    req.request.body = params;
    req.request.header["Content-Type"] = "application/json;";
    req.callback = std::move(callback);
    EventBus::FireEvent(req);
}

/**
  * @函数名   getHeadPath
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
std::string TitlebarMsgManager::getHeadPath(const std::string& netPath)
{
	NetHeadImgEvt e;
	e.netFilePath = netPath;
	EventBus::FireEvent(e, false);
    return e.localFilePath;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.11.08
  */
void TitlebarMsgManager::sendSearch(SearchInfoEvent & event)
{
    EventBus::FireEvent(event);
}


void TitlebarMsgManager::saveConfig() {

	SaveConfigEvt e;
	EventBus::FireEvent(e);
}

void TitlebarMsgManager::clearSystemCache() {
	ClearSystemCacheEvt e;
	EventBus::FireEvent(e);
}

/**
 *
 * @param head
 */
void TitlebarMsgManager::changeUserHead(const std::string &head)
{
    ChangeHeadMessage e(head);
    EventBus::FireEvent(e);
}

void TitlebarMsgManager::chanegUserStatus(const std::string& status)
{
    SwitchUserStatusEvt evt(status);
    EventBus::FireEvent(evt);
}

/**
 * qchat 服务模式
 * @param sid
 * @param seat
 */
void TitlebarMsgManager::setServiceSeat(int sid, int seat) {
    SetUserSeatEvt evt(sid,seat);
    EventBus::FireEvent(evt);
}

/**
  * 接收消息类
  * @author   cc
  * @date     2018/09/29
  */
TitlebarMsgListener::TitlebarMsgListener(MainPanel *pMainPanel)
	:_pMainPanel(pMainPanel)
{
	EventBus::AddHandler<UserCardMessgae>(*this);
	EventBus::AddHandler<ChangeHeadRetMessage>(*this);
	EventBus::AddHandler<SwitchUserStatusRet>(*this);
}

TitlebarMsgListener::~TitlebarMsgListener()
{

}

void TitlebarMsgListener::onEvent(UserCardMessgae& e)
{
	if (e.getCanceled()) return;

	if (_pMainPanel)
	{
		_pMainPanel->recvUserCard(e.userCards);
	}
}

/**
 *
 */
void TitlebarMsgListener::onEvent(ChangeHeadRetMessage &e)
{
    if (e.getCanceled()) return;

    if (_pMainPanel)
    {
        _pMainPanel->onChangeHeadRet(e.ret, e.localHead);
    }
}

void TitlebarMsgListener::onEvent(SwitchUserStatusRet &e)
{
    if (e.getCanceled()) return;

    if (_pMainPanel)
    {
        _pMainPanel->recvSwitchUserStatus(e.user_status);
    }
}
