#ifndef _MESSAGEMANAGER_H_
#define _MESSAGEMANAGER_H_

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <string>
#include "../EventBus/Object.hpp"
#include "../EventBus/HandlerRegistration.hpp"
#include "../Message/ChatMessage.h"
#include "../EventBus/EventHandler.hpp"
#include "../Message/GroupMessage.h"
#include "../Message/UserMessage.h"
#include "../Message/StatusMessage.h"
#include "../Message/LoginMessgae.h"
#include "../entity/UID.h"

class ChatMsgManager : public Object
{
public:
	ChatMsgManager();

    ~ChatMsgManager() override;

public:

	std::string getNetFilePath(const std::string& localFilePath);

	std::string getLocalFilePath(const std::string& netFilePath);

	std::string getSouceImagePath(const std::string& netFilePath);

	std::string getLocalHeadPath(const std::string& netHeadPath);

	VectorMessage getUserHistoryMessage(const QInt64& time, const QUInt8& chatType, const QTalk::Entity::UID& uid);

	VectorMessage getUserLocalHistoryMessage(const QInt64& time, const QTalk::Entity::UID& uid);
	VectorMessage getUserFileHistoryMessage(const QInt64& time, const QTalk::Entity::UID& uid);
	VectorMessage getUserImageHistoryMessage(const QInt64& time, const QTalk::Entity::UID& uid);
	VectorMessage getSearchMessage(const QInt64& time, const QTalk::Entity::UID& uid, const std::string& text);
	VectorMessage getAfterMessage(const QInt64& time, const QTalk::Entity::UID& uid);

	void getGroupInfo(const std::string& groupId);
	
	void sendMessage(S_Message& e);

	void preSendMessage(const QTalk::Entity::ImMessageInfo &message);

    void sendDownLoadFile(const std::string &strLocalPath, const std::string &strUri, const std::string& processKey);

    void sendRevokeMessage(const QTalk::Entity::UID& uid, const std::string& from, const std::string& messageId);
    //
	void setUserSetting(bool isSetting, const std::string& key, const std::string& subKey, const std::string& value);
	//
	void sendLogReport(const std::string& desc, const std::string& logPath);

	void setGroupAdmin(const std::string& groupId, const std::string& nick, const std::string& memberJid, bool isAdmin);

	void removeGroupMember(const std::string& groupId, const std::string& nick, const std::string& memberJid);

	void getUserInfo(std::shared_ptr<QTalk::Entity::ImUserInfo>& info);

	std::string uploadFile(const std::string& localFile, bool = false, const std::string& processKey = std::string());

	void getRecentSession(std::vector<QTalk::StShareSession> &ss);
	//
	void getContactsSession(std::vector<QTalk::StShareSession> &ss);
	//
    void forwardMesssage(const std::string& messsageId, const std::map<std::string, int>& users);
    //
    void addGroupMember(const std::vector<std::string>& members, const std::string& groupId);
    //qchat 客服挂断
    void serverCloseSession(const std::string& username,const std::string& seatname,const std::string& virtualname);
    //qchat 发送产品卡片
    void sendProduct(const std::string& userQName, const std::string& virtualId,const std::string &product,const std::string &type);
    //qchat 会话转移
    void getSeatList(const QTalk::Entity::UID uid);
    void sessionTransfer(const QTalk::Entity::UID uid, const std::string newCsrName, const std::string reason);
    void sendWechat(const QTalk::Entity::UID uid);
	//
	void getQuickGroups(std::vector<QTalk::Entity::ImQRgroup> &groups);
	void getQuickContentByGroup(std::vector<QTalk::Entity::IMQRContent> &contents, int id);
};

// 消息接收
class ChatViewMainPanel;
class ChatMsgListener : public EventHandler<R_Message>, public EventHandler<GroupMemberMessage>
				      , public EventHandler<UserCardMessgae>, public EventHandler<R_BlackListMessage>
					  , public EventHandler<GroupTopicEvt>, public EventHandler<UpdateOnlineEvent>
                      , public EventHandler<FileProcessMessage>, public EventHandler<DisconnectToServer>
					  , public EventHandler<LoginSuccessMessage>, public EventHandler<UpdateGroupMember>
					  , public EventHandler<SignalReadState>, public EventHandler<RemoveGroupMemberRet>
					  , public EventHandler<RevokeMessage>, public EventHandler<UpdateUserConfigMsg>
					  , public EventHandler<DestroyGroupRet>, public EventHandler<UpdateGroupInfoRet>
					  , public EventHandler<LogReportMessageRet>, public EventHandler<GetUsersOnlineSucessEvent>
					  , public EventHandler<RecvVideoMessage>, public EventHandler<GroupMemberChangeRet>
					  , public EventHandler<ChangeHeadRetMessage>, public EventHandler<UpdateMoodRet>
					  , public EventHandler<FeedBackLogEvt> , public EventHandler<GetSeatListRet>
					  , public EventHandler<IncrementConfig>, public EventHandler<GroupReadMState>
{
public:
	ChatMsgListener();

    ~ChatMsgListener() override;

public:
	void onEvent(R_Message& e) override;
	void onEvent(GroupMemberMessage& e) override;
	void onEvent(UserCardMessgae& e) override;
	void onEvent(GroupTopicEvt& e) override;
	void onEvent(UpdateOnlineEvent& e) override;
	void onEvent(FileProcessMessage& e) override;
	void onEvent(DisconnectToServer& e) override;
	void onEvent(LoginSuccessMessage& e) override;
	void onEvent(UpdateGroupMember& e) override;
    void onEvent(SignalReadState& e) override;
    void onEvent(RevokeMessage& e) override;
    void onEvent(UpdateUserConfigMsg& e) override;
	void onEvent(DestroyGroupRet& e) override;
	void onEvent(R_BlackListMessage& e) override;
	void onEvent(UpdateGroupInfoRet& e) override;
	void onEvent(RemoveGroupMemberRet& e) override;
	void onEvent(LogReportMessageRet& e) override;
	void onEvent(GetUsersOnlineSucessEvent& e) override;
    void onEvent(RecvVideoMessage& e) override;
    void onEvent(ChangeHeadRetMessage& e) override;
    void onEvent(GroupMemberChangeRet& e) override;
    void onEvent(UpdateMoodRet& e) override;
    void onEvent(FeedBackLogEvt& e) override;
    void onEvent(GetSeatListRet& e) override;
    void onEvent(IncrementConfig& e) override;
    void onEvent(GroupReadMState& e) override;

private:

																											};

#endif//
