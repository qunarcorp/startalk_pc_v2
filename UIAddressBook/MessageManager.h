#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "../EventBus/Object.hpp"
#include "../EventBus/EventHandler.hpp"
#include "../Message/UserMessage.h"
#include "../EventBus/HandlerRegistration.hpp"
#include "../Message/GroupMessage.h"

class AddressBookMsgManager : public Object
{
public:
	AddressBookMsgManager();
	~AddressBookMsgManager() override;

public:
	void getUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement>&,
                     std::shared_ptr<QTalk::Entity::ImUserInfo>& userInfo);
	void getUserPhoneNo(const std::string& userId, std::string& phoneNo);
	void setUserSetting(bool isSetting, const std::string& key, const std::string& subKey, const std::string& value);
	void getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>>& structure);
	void getStructureCount(const std::string& structName, int &count);
	void getStructureMembers(const std::string& structName, std::vector<std::string>& members);
	void creatGroup(const std::string& groupId, const std::string& groupName);
	void addGroupMember(const std::vector<std::string>& members, const std::string & groupId);
    void getUserInfo(std::shared_ptr<QTalk::Entity::ImUserInfo>& info);
};


class AddressBookPanel;
class AddressBookListener : public EventHandler<UpdateUserConfigMsg>
		, public EventHandler<AllFriends>, public EventHandler<AllGroupList>
		, public EventHandler<CreatGroupRet>, public EventHandler<DestroyGroupRet>
		, public EventHandler<IncrementConfig>
		, public EventHandler<IncrementUser>
{
public:
    explicit AddressBookListener(AddressBookPanel* mainPanel);
	~AddressBookListener() override;

protected:
	void onEvent(UpdateUserConfigMsg& e) override;
	void onEvent(AllFriends& e) override;
	void onEvent(AllGroupList& e) override;
	void onEvent(CreatGroupRet& e) override;
	void onEvent(DestroyGroupRet& e) override;
	void onEvent(IncrementConfig &e) override;
	void onEvent(IncrementUser &e) override;

private:
	AddressBookPanel* _mainPanel;


							};

#endif//MESSAGEMANAGER_H
