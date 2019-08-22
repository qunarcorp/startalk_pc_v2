#include "MessageManager.h"
#include "../EventBus/EventBus.h"
#include "LoginPanel.h"
#include "../QtUtil/Utils/Log.h"
#include <QDebug>
UILoginMsgManager::UILoginMsgManager()
{
}


UILoginMsgManager::~UILoginMsgManager()
{
}

// 发送登陆消息
bool UILoginMsgManager::SendLoginMessage(const std::string& userName, const std::string& password)
{
    LoginMessage msg(userName, password);
    EventBus::FireEvent(msg);
	return msg.ret;
}

/**
 * 获取导航信息
 */
bool UILoginMsgManager::getNavInfo(const std::string &navAddr)
{
	GetNavAddrInfo e(navAddr);
	EventBus::FireEvent(e, false);
	return e.ret;
}


/**
 * 获取导航domain
 * @param addr 导航地址
 * @return
 */
std::string UILoginMsgManager::getNavDomain(const std::string &addr)
{
    GetNavDomain e(addr);
    EventBus::FireEvent(e, false);
    return e.doamin;
}

/**
 * 将qvt存入db
 * @param qvt
 * @return
 */
bool UILoginMsgManager::saveQvtToDB(const std::string &qvt) {
    SaveQchatQVTToDB e(qvt);
    EventBus::FireEvent(e, false);
    return true;
}

/**
 * qchat 从db获取qvt
 * @return
 */
std::string UILoginMsgManager::getQchatQvt() {
    GetQchatQVTFromDB e;
    EventBus::FireEvent(e, false);
    return e.strQVT;
}

/**
 * qchat用qvt换取token
 * @param qvt
 * @return
 */
std::map<std::string,std::string> UILoginMsgManager::getQchatToken(const std::string &qvt)
{
	GetQchatToken e(qvt);
	EventBus::FireEvent(e);
	return e.userMap;
}

void UILoginMsgManager::startUpdater(const std::string &users)
{
    StartUpdaterEvt e(users);
    EventBus::FireEvent(e);
}

/**
 * UILoginMsgListener
 */
UILoginMsgListener::UILoginMsgListener(LoginPanel* pLoginPanel)
	:_pLoginPanel(pLoginPanel)
{
	EventBus::AddHandler<LoginSuccessMessage>(*this);
	EventBus::AddHandler<SynOfflineSuccees>(*this);
	EventBus::AddHandler<AuthFailed>(*this);
	EventBus::AddHandler<DataBaseOpenMessage>(*this);
	EventBus::AddHandler<GotStructureMessage>(*this);
	EventBus::AddHandler<GotUserGroup>(*this);
	EventBus::AddHandler<LoginErrMessage>(*this);
	EventBus::AddHandler<ChangeHeadRetMessage>(*this);
	EventBus::AddHandler<LoginProcessMessage>(*this);
}

UILoginMsgListener::~UILoginMsgListener()
{

}

//
void UILoginMsgListener::onEvent(LoginSuccessMessage& e)
{
	if (e.getCanceled())
	{
		return;
	}
	if (nullptr != _pLoginPanel)
	{
        _pLoginPanel->loginSuccess();
    }
}

void UILoginMsgListener::onEvent(SynOfflineSuccees &e)
{
    if (e.getCanceled())
    {
        return;
    }
    if (nullptr != _pLoginPanel)
    {
        _pLoginPanel->onSynDataSuccess();
    }
}

void UILoginMsgListener::onEvent(AuthFailed &e)
{
	if (e.getCanceled()) return;

	if (nullptr != _pLoginPanel)
	{
		_pLoginPanel->loginError(e.message);
	}
}

void UILoginMsgListener::onEvent(DataBaseOpenMessage &e)
{
	if (e.getCanceled())
	{
		return;
	}
//	if (nullptr != _pLoginPanel)
//	{
//		info_log("--------------正在获取组织架构--------------");
//		_pLoginPanel->onGotLoginstauts("正在获取组织架构");
//	}
}

//
void UILoginMsgListener::onEvent(GotStructureMessage &e)
{
	if (e.getCanceled())
	{
		return;
	}
//	if (nullptr != _pLoginPanel)
//	{
//		info_log("--------------GotStructureMessage--------------");
//		_pLoginPanel->onGotLoginstauts("正在获取群信息");
//	}
}

void UILoginMsgListener::onEvent(GotUserGroup &e)
{
	if (e.getCanceled())
	{
		return;
	}
//	if (nullptr != _pLoginPanel)
//	{
//		info_log("--------------GotUserGroup--------------");
//		_pLoginPanel->onGotLoginstauts("正在拉取离线消息");
//	}
}

/**
 *
 * @param e
 */
void UILoginMsgListener::onEvent(LoginErrMessage &e)
{
    if (e.getCanceled())
    {
        return;
    }
    if (nullptr != _pLoginPanel)
    {
        info_log(e.errorMessage);
        _pLoginPanel->loginError(e.errorMessage);
    }
}


void UILoginMsgListener::onEvent(ChangeHeadRetMessage &e)
{
    if(e.getCanceled()) return;;

    if(e.ret && nullptr != _pLoginPanel)
    {
        _pLoginPanel->saveHeadPath();
    }
}

void UILoginMsgListener::onEvent(LoginProcessMessage& e)
{
    if(e.getCanceled()) return;;

    if(nullptr != _pLoginPanel)
    {
        info_log("-------------- {0} --------------", e.message);
        _pLoginPanel->onGotLoginstauts(e.message.data());
    }
}
