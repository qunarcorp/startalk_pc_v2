#ifndef _MESSAGEMANAGER_H_
#define _MESSAGEMANAGER_H_

#include "../EventBus/EventHandler.hpp"
#include "../Message/LoginMessgae.h"
#include "../EventBus/HandlerRegistration.hpp"
#include "../Message/UserMessage.h"
#include "../Message/ChatMessage.h"

class SearchInfoEvent;
class TitlebarMsgManager : public Object
{
public:
	TitlebarMsgManager();
	~TitlebarMsgManager() override;

public:
	void getUserCard(const std::string &domain, const std::string &userName, const int &version);

	std::string getHeadPath(const std::string& netPath);

    void sendSearch(SearchInfoEvent & event);
	void saveConfig();
	void clearSystemCache();
	void changeUserHead(const std::string& headpath);

	void chanegUserStatus(const std::string& status);
	void setServiceSeat(int sid,int seat);
};

class MainPanel;
class TitlebarMsgListener : public EventHandler<UserCardMessgae>, public EventHandler<ChangeHeadRetMessage>
        , public EventHandler<SwitchUserStatusRet>
{
public:
    explicit TitlebarMsgListener(MainPanel *pMainPanel);
    ~TitlebarMsgListener() override;

public:
	void onEvent(UserCardMessgae& e) override;
	void onEvent(ChangeHeadRetMessage& e) override;
	void onEvent(SwitchUserStatusRet& e) override;

private:
	MainPanel* _pMainPanel;
};

#endif//_MESSAGEMANAGER_H_
