#ifndef _MESSAGEMANAGER_H
#define _MESSAGEMANAGER_H

#include <string>
#include "../EventBus/Object.hpp"
#include "../EventBus/EventHandler.hpp"
#include "../EventBus/HandlerRegistration.hpp"
#include "../Message/LoginMessgae.h"
#include "../Message/ChatMessage.h"
#include "../entity/IM_Session.h"
#include "../Message/UserMessage.h"
#include "../include/CommonStrcut.h"
#include "../Message/GroupMessage.h"
#include "../Message/StatusMessage.h"
#include "../entity/im_friend_list.h"
#include "../entity/im_group.h"
#include "../Message/UselessMessage.h"
#include "../Message/LogicBaseMessage.h"
#include "../entity/im_user_status_medal.h"

class CommMsgManager : public Object
{
public:
	CommMsgManager();
	virtual ~CommMsgManager();

public:
    void sendDataBaseOpen();
	void LoginSuccess(const std::string& strSessionId);
//	void sendReconnectResult();
//	void OnRecvIQMessage(IQMessageEvt& e);
    // 登录失败消息
    void sendLoginErrMessage(const std::string& message);
    void sendGetHistoryError();
    void sendGotStructure(bool ret);
    void sendSynOfflineSuccees();
	void sendGotUserCard(const std::vector<QTalk::StUserCard> &userCard);
    void sendGotUsersStatus(const std::map<std::string, std::string> &userStatus);
	void sendDownloadHeadSuccess();
	void sendGotGroupCard();
	void sendDownloadGroupHeadSuccess();
    void sendOnlineUpdate();
	void gotGroupMember(GroupMemberMessage& e);
	void updateGroupMemberInfo(const std::string& groupId, const std::vector<QTalk::StUserCard>& userCards);
	void gotGroupTopic(const std::string& groupId, const std::string& topic);
	void updateFileProcess(const std::string& key, double dltotal, double dlnow, double ultotal, double ulnow, double speed, double leftTime);
	void downloadFileComplete(const std::string& key,const std::string& localPath,bool finish);
	void onUpdateGroupInfo(std::shared_ptr<QTalk::StGroupInfo> info);
	void onGroupJoinMember(std::shared_ptr<QTalk::StGroupMember> member);

    void updateUserConfigs(const std::vector<QTalk::Entity::ImConfig>& arConfigs);
    void incrementConfigs(const std::map<std::string, std::string> &deleteData,
            const std::vector<QTalk::Entity::ImConfig>& arImConfig);
    //
    void sendLoginProcessMessage(const std::string& message);
	//
	void sendGotFriends(const std::vector<QTalk::Entity::IMFriendList>& friends);
	void sendGotGroupList(const std::vector<QTalk::Entity::ImGroupInfo>& groups);

    void removeSession(const std::string& peerId);
	//
	void sendLogReportRet(bool isSuccess, const std::string& msg);
	void sendFileWritedMessage(const std::string& localPath);
	//
	void changeHeadRetMessage(bool ret, const std::string& locaHead);
	//
	void updateMoodRet(const std::string& userId, const std::string& mood);

	//qchat 会话转移
	void setSeatList(QTalk::Entity::UID uid,std::vector<QTalk::Entity::ImTransfer> transfers);
	//
	void gotIncrementUser(const std::vector<QTalk::Entity::ImUserInfo> &arUserInfo,
	        const std::vector<std::string> &arDeletes);

	void onUserMadelChanged(const std::vector<QTalk::Entity::ImUserStatusMedal>& userMedals);
};

// 
class Communication;
class CommMsgListener :
        public EventHandler<RetryConnectToServerEvt>
        , public EventHandler<DataBaseOpenMessage>
        , public EventHandler<SourceNetImage>
		, public EventHandler<NetImgEvt>
		, public EventHandler<HistoryMessage>
		, public EventHandler<GetUserCardMessage>
		, public EventHandler<DownLoadHeadPhotoEvent>
		, public EventHandler<DownLoadGroupHeadPhotoEvent>
		, public EventHandler<UserCardSupple>
        , public EventHandler<NetHeadImgEvt>
        , public EventHandler<GetGroupMessage>
        , public EventHandler<GetUsersOnlineEvent>
        , public EventHandler<GetEmoticonFileMessage>
		, public EventHandler<GetNetEmoticon>
		, public EventHandler<DownloadFileWithProcess>
        , public EventHandler<GetStructureCount>
        , public EventHandler<UserPhoneNo>
        , public EventHandler<SearchInfoEvent>
        , public EventHandler<StructureMessage>
        , public EventHandler<AddGroupMember>
        , public EventHandler<CreatGroup>
        , public EventHandler<GetSessionData>
        , public EventHandler<LocalImgEvt>
        , public EventHandler<UserSettingMsg>
        , public EventHandler<GetStructureMember>
        , public EventHandler<GetNavAddrInfo>
        , public EventHandler<DownloadCollection>
        , public EventHandler<GetNavDomain>
        , public EventHandler<RemoveSessionData>
        , public EventHandler<OAUiDataMessage>
        , public EventHandler<GetGroupInfoMessage>
        , public EventHandler<UpdateGroupInfoMsg>
        , public EventHandler<QuitGroupMsg>
        , public EventHandler<DestroyGroupMsg>
        , public EventHandler<HeartBeat>
        , public EventHandler<LogReportMessage>
		, public EventHandler<SetGroupAdmin>
		, public EventHandler<RemoveGroupMember>
		, public EventHandler<SaveConfigEvt>
		, public EventHandler<ClearSystemCacheEvt>
		, public EventHandler<ChangeHeadMessage>
		, public EventHandler<UserOnlineState>
        , public EventHandler<OperatorStatistics>
        , public EventHandler<UserCardInfo>
        , public EventHandler<UploadFileEvt>
        , public EventHandler<RecentSessionEvt>
        , public EventHandler<ContactsSessionEvt>
        , public EventHandler<UpdateMoodEvt>
		, public EventHandler<GetQchatToken>
		, public EventHandler<ImageMessageEvt>
		, public EventHandler<StartUpdaterEvt>
		, public EventHandler<EmptyMessageEvt>
		, public EventHandler<SetUserSeatEvt>
		, public EventHandler<ServerCloseSessionEvt>
		, public EventHandler<SendProductEvt>
		, public EventHandler<AllGroupMemberEvt>
		, public EventHandler<LocalHistoryMessage>
		, public EventHandler<FileHistoryMessage>
		, public EventHandler<ImageHistoryMessage>
		, public EventHandler<SearchHistoryMessage>
		, public EventHandler<AfterMessage>
		, public EventHandler<GetSeatListEvt>
		, public EventHandler<SessionTransferEvt>
		, public EventHandler<QuickGroupEvt>
		, public EventHandler<QuickContentByGroupEvt>
		, public EventHandler<GetQchatQVTFromDB>
		, public EventHandler<SaveQchatQVTToDB>
		, public EventHandler<SendWechatEvt>
		, public EventHandler<ReportDump>
		, public EventHandler<LoginMessage>
		, public EventHandler<S_RecvGroupMemberEvt>
		, public EventHandler<CreatGroupRet>
		, public EventHandler<S_InviteGroupMemberEvt>
		, public EventHandler<S_AllFriendsEvt>
		, public EventHandler<S_DealBind>
		, public EventHandler<S_UpdateTimeStamp>
		, public EventHandler<S_UserConfigChanged>
		, public EventHandler<S_UserJoinGroup>
		, public EventHandler<S_StaffChanged>
		, public EventHandler<DestroyGroupRet>
		, public EventHandler<S_AddHttpQeq>
		, public EventHandler<HotLineMessageListEvt>
		, public EventHandler<UpdateMsgExtendInfo>
		, public EventHandler<GetHotLines>
		, public EventHandler<CheckUpdaterEvt>
		, public EventHandler<UserMedalEvt>
		, public EventHandler<SgMedalListChanged>
		, public EventHandler<SgUserMedalChanged>
		, public EventHandler<GetMedalUserEvt>
		, public EventHandler<ModifyUserMedalStatusEvt>

{
public:
    explicit CommMsgListener(Communication* pComm);
    ~CommMsgListener() override;

public:
    void onEvent(RetryConnectToServerEvt& e) override;
    void onEvent(DataBaseOpenMessage& e) override;
    void onEvent(LocalImgEvt &e) override;
    void onEvent(NetImgEvt &e) override;
    void onEvent(HistoryMessage &e) override;
    void onEvent(GetUserCardMessage &e) override;
    void onEvent(DownLoadHeadPhotoEvent &e) override;
    void onEvent(DownLoadGroupHeadPhotoEvent &e) override;
    void onEvent(NetHeadImgEvt& e) override;
    void onEvent(GetGroupMessage& e) override;
    void onEvent(GetUsersOnlineEvent& e) override;
    void onEvent(GetEmoticonFileMessage& e) override;
    void onEvent(GetNetEmoticon& e) override;
    void onEvent(DownloadFileWithProcess& e) override;
    void onEvent(UserCardSupple& e) override;
    void onEvent(UserPhoneNo& e) override;
    void onEvent(SearchInfoEvent& e) override;
    void onEvent(StructureMessage& e) override;
    void onEvent(AddGroupMember& e) override;
    void onEvent(CreatGroup& e) override;
    void onEvent(GetSessionData& e) override;
    void onEvent(UserSettingMsg& e) override;
    void onEvent(GetStructureCount& e) override;
    void onEvent(GetStructureMember& e) override;
    void onEvent(GetNavAddrInfo& e) override;
    void onEvent(GetNavDomain& e) override;
    void onEvent(DownloadCollection& e) override;
    void onEvent(OAUiDataMessage& e) override;
    void onEvent(GetGroupInfoMessage& e) override;
    void onEvent(UpdateGroupInfoMsg& e) override;
    void onEvent(QuitGroupMsg& e) override;
    void onEvent(DestroyGroupMsg& e) override;
    void onEvent(HeartBeat& e) override;
    void onEvent(LogReportMessage& e) override;
    void onEvent(ReportDump& e) override;
    void onEvent(RemoveSessionData& e) override;
    void onEvent(SourceNetImage& e) override;
    void onEvent(SaveConfigEvt& e) override;
    void onEvent(ClearSystemCacheEvt& e) override;
    void onEvent(SetGroupAdmin& e) override;
    void onEvent(RemoveGroupMember& e) override;
    void onEvent(ChangeHeadMessage& e) override;
    void onEvent(UserOnlineState& e) override;
    void onEvent(OperatorStatistics& e) override;
    void onEvent(UserCardInfo& e) override;
    void onEvent(UploadFileEvt& e) override;
    void onEvent(RecentSessionEvt& e) override;
    void onEvent(ContactsSessionEvt& e) override;
    void onEvent(UpdateMoodEvt& e) override;
    void onEvent(SaveQchatQVTToDB& e) override;
    void onEvent(GetQchatQVTFromDB& e) override;
	void onEvent(GetQchatToken& e) override;
	void onEvent(ImageMessageEvt& e) override;
	void onEvent(StartUpdaterEvt& e) override;
	void onEvent(EmptyMessageEvt& e) override;
	void onEvent(AllGroupMemberEvt& e) override;
	void onEvent(SetUserSeatEvt& e) override;
	void onEvent(ServerCloseSessionEvt& e) override;
	void onEvent(LocalHistoryMessage& e) override;
	void onEvent(FileHistoryMessage& e) override;
	void onEvent(ImageHistoryMessage& e) override;
	void onEvent(SearchHistoryMessage& e) override;
	void onEvent(AfterMessage& e) override;
	void onEvent(SendProductEvt& e) override;
	void onEvent(GetSeatListEvt& e) override;
	void onEvent(SessionTransferEvt& e) override;
	void onEvent(QuickGroupEvt& e) override;
	void onEvent(QuickContentByGroupEvt& e) override;
	void onEvent(SendWechatEvt& e) override;
	// 登录处理
	void onEvent(LoginMessage& e) override;
	// 收到群成员列表
	void onEvent(S_RecvGroupMemberEvt& e) override;
	// 建群结果
	void onEvent(CreatGroupRet& e) override ;
	//
	void onEvent(S_InviteGroupMemberEvt& e) override ;
	//
    void onEvent(S_AllFriendsEvt& e) override ;
    //
    void onEvent(S_DealBind& e) override ;
    //
    void onEvent(S_UpdateTimeStamp& e) override ;
    //
    void onEvent(S_UserConfigChanged& e) override ;
    //
    void onEvent(S_UserJoinGroup& e) override ;
    //
    void onEvent(S_StaffChanged& e) override ;
    //
    void onEvent(DestroyGroupRet& e) override ;
    //
    void onEvent(S_AddHttpQeq& e) override ;
    // 获取热线消问题列表
    void onEvent(HotLineMessageListEvt& e) override ;
    //
    void onEvent(UpdateMsgExtendInfo& e) override ;

    void onEvent(GetHotLines& e) override;
    void onEvent(CheckUpdaterEvt& e) override;
    void onEvent(UserMedalEvt& e) override;
    //
    void onEvent(SgMedalListChanged& e) override;
    void onEvent(SgUserMedalChanged& e) override;
    void onEvent(GetMedalUserEvt& e) override;
    void onEvent(ModifyUserMedalStatusEvt& e) override;

private:
	Communication*       _pComm;
};

#endif
