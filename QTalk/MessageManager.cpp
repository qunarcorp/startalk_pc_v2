#include "MessageManager.h"
#include "../EventBus/EventBus.h"
#include "MainWindow.h"
#include "../Message/StatusMessage.h"
#include "../Message/ChatMessage.h"
#include "../Message/UserMessage.h"

QTalkMsgManager::QTalkMsgManager()
{
}


QTalkMsgManager::~QTalkMsgManager()
{
}

void QTalkMsgManager::sendHearBeat()
{
    HeartBeat e;
    EventBus::FireEvent(e);
}

void QTalkMsgManager::sendOnlineState(const QInt64 &login_t, const QInt64 &logout_t, const std::string& ip)
{
    UserOnlineState e(login_t, logout_t, ip);
    EventBus::FireEvent(e);
}

void QTalkMsgManager::sendOperatorStatistics(const std::string& ip, const std::vector<QTalk::StActLog>& operators)
{
    OperatorStatistics e(ip, operators);
    EventBus::FireEvent(e);
}

void QTalkMsgManager::sendLogReport(const std::string& desc, const std::string &logPath)
{
    LogReportMessage e(desc, logPath);
    EventBus::FireEvent(e);
}

void QTalkMsgManager::chanegUserStatus(const std::string& status)
{
    SwitchUserStatusEvt evt(status);
    EventBus::FireEvent(evt);
}

void QTalkMsgManager::reportDump(const std::string &dump) {
    ReportDump e(dump);
    EventBus::FireEvent(e);
}

std::string QTalkMsgManager::checkUpdater(int version) {
    CheckUpdaterEvt e;
    e.version = version;
    EventBus::FireEvent(e);
    return e.updater_link;
}

/*------------------*/
QTalkMsgListener::QTalkMsgListener(MainWindow* pUiControl)
	:_pUiControl(pUiControl)
{
	EventBus::AddHandler<LoginSuccessMessage>(*this);
	EventBus::AddHandler<GetHistoryError>(*this);
}

QTalkMsgListener::~QTalkMsgListener()
{
}

//
void QTalkMsgListener::onEvent(LoginSuccessMessage& e)
{
	if (e.getCanceled())
	{
		return;
	}
	if (nullptr != _pUiControl)
	{
		_pUiControl->OnLoginSuccess(e.getSessionId());
	}
}

void QTalkMsgListener::onEvent(GetHistoryError &e) {
    if (nullptr != _pUiControl)
    {
        _pUiControl->onGetHistoryError();
    }
}
