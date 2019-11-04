#include "MessageManager.h"
#include "../EventBus/EventBus.h"
#include "Communication.h"
#include "OfflineMessageManager.h"
#include "UserManager.h"
#include "FileHelper.h"
#include "../Message/GroupMessage.h"
#include "../QtUtil/Utils/Log.h"
#include "OnLineManager.h"
#include "SearchManager.h"
#include "UserConfig.h"
#include "GroupManager.h"
#include "HotLinesConfig.h"

/**
 * 
 */
CommMsgManager::CommMsgManager() {
}


CommMsgManager::~CommMsgManager() {
}


void CommMsgManager::sendDataBaseOpen() {
    DataBaseOpenMessage e;
    EventBus::FireEvent(e);
}

//
//void CommMsgManager::sendReconnectResult() {
//    RetryConnectRet e;
//    EventBus::FireEvent(e);
//}

//void CommMsgManager::OnRecvIQMessage(IQMessageEvt &e) {
//    EventBus::FireEvent(e);
//}


void CommMsgManager::sendLoginErrMessage(const std::string &message) {
    LoginErrMessage e;
    e.errorMessage = message;
    EventBus::FireEvent(e);
}

/**
  * @函数名   
  * @功能描述 拉取组织架构信号
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void CommMsgManager::sendGotStructure(bool ret) {
    GotStructureMessage e;
    e._bRet = ret;
    EventBus::FireEvent(e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
void CommMsgManager::sendSynOfflineSuccees() {
    SynOfflineSuccees e;
    EventBus::FireEvent(e);
}

/**
  * @函数名   sendGotUserCard
  * @功能描述 返回获取名片结果
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
void CommMsgManager::sendGotUserCard(const std::vector<QTalk::StUserCard> &userCard) {
    info_log("发送服务器查询名片结果Event 个数:{0}", userCard.size());
    UserCardMessgae e;
    e.userCards = userCard;
    EventBus::FireEvent(e);
}

/**
  * @函数名 sendGotUsersStatus
  * @功能描述 发送获取的用户状态
  * @参数
  * @date 2018.10.15
  */
void CommMsgManager::sendGotUsersStatus(const std::map<string, string> &userStatus) {
    GetUsersOnlineSucessEvent event;
    event._userstatus = userStatus;
    EventBus::FireEvent(event);
}

/**
  * @函数名   sendDownloadHeadSuccess
  * @功能描述 批量下载头像成功
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void CommMsgManager::sendDownloadHeadSuccess() {
    DownloadHeadSuccess e;
    EventBus::FireEvent(e);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void CommMsgManager::sendGotGroupCard() {
    GotUserGroup e;
    EventBus::FireEvent(e);
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void CommMsgManager::sendDownloadGroupHeadSuccess() {
    DownloadGroupHeadSuccess e;
    EventBus::FireEvent(e);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.12
  */
void CommMsgManager::sendOnlineUpdate() {
    UpdateOnlineEvent e;
    EventBus::FireEvent(e);
}

/**
  * @函数名   gotGroupMember
  * @功能描述 获取群成员
  * @参数
  * @author   cc
  * @date     2018/10/08
  */
void CommMsgManager::gotGroupMember(GroupMemberMessage &e) {
    info_log("发送获取到群成员列表Event 个数:{0} 群id:{1}", e.members.size(), e.groupId);
    EventBus::FireEvent(e);
}

/**
  * @函数名   updateGroupMemberInfo
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/26
  */
void
CommMsgManager::updateGroupMemberInfo(const std::string &groupId, const std::vector<QTalk::StUserCard> &userCards) {
    UpdateGroupMember e;
    e.groupId = groupId;
    e.userCards = userCards;
    EventBus::FireEvent(e);
}

/**
  * @函数名   gotGroupTopic
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/12
  */
void CommMsgManager::gotGroupTopic(const std::string &groupId, const std::string &topic) {
    GroupTopicEvt e;
    e.groupId = groupId;
    e.groupTopic = topic;
    EventBus::FireEvent(e);
}

/**
  * @函数名   updateFileProcess
  * @功能描述 文件上传/下载进度
  * @参数
  * @author   cc
  * @date     2018/10/16
  */
void
CommMsgManager::updateFileProcess(const std::string &key, double dltotal, double dlnow, double ultotal, double ulnow,
                                  double speed, double leftTime) {
    FileProcessMessage e;
    e.key = key;
    e.downloadTotal = dltotal;
    e.downloadNow = dlnow;
    e.uploadTotal = ultotal;
    e.uploadNow = ulnow;
    e.speed = speed;
    e.leftTime = leftTime;
    EventBus::FireEvent(e);
}

void CommMsgManager::downloadFileComplete(const std::string &key, const std::string &localPath, bool finish) {
    DownloadFileComplete e;
    e.key = key;
    e.localPath = localPath;
    e.finish = finish;
    //EventBus::FireEvent(e);
}

void CommMsgManager::onUpdateGroupInfo(std::shared_ptr<QTalk::StGroupInfo> info) {
    UpdateGroupInfoRet e;
    e.groupinfo = info;
    EventBus::FireEvent(e);
}

void CommMsgManager::onGroupJoinMember(std::shared_ptr<QTalk::StGroupMember> member) {
    GroupMemberChangeRet e;
    e.groupId = member->groupId;
    e.memberJid = member->memberJid;
    e.nick = member->nick;
    e.affiliation = member->affiliation;
    e.isAdd = true;
    EventBus::FireEvent(e);
}



void CommMsgManager::updateUserConfigs(const std::vector<QTalk::Entity::ImConfig> &arConfigs) {
    UpdateUserConfigMsg e;
    e.arConfigs = arConfigs;
    EventBus::FireEvent(e);
}

void CommMsgManager::incrementConfigs(const std::map<std::string, std::string> &deleteData,
                      const std::vector<QTalk::Entity::ImConfig>& arImConfig)
{
    IncrementConfig config;
    config.deleteData = deleteData;
    config.arImConfig = arImConfig;
    EventBus::FireEvent(config);
}

void CommMsgManager::sendGotFriends(const std::vector<QTalk::Entity::IMFriendList> &friends) {
    AllFriends e;
    e.friends = friends;
    EventBus::FireEvent(e);
}

void CommMsgManager::sendGotGroupList(const std::vector<QTalk::Entity::ImGroupInfo> &groups) {
    AllGroupList e;
    e.groups = groups;
    EventBus::FireEvent(e);
}

void CommMsgManager::removeSession(const std::string &peerId) {

}



void CommMsgManager::sendLogReportRet(bool isSuccess, const std::string &msg) {
    LogReportMessageRet e(isSuccess, msg);
    EventBus::FireEvent(e);
}

void CommMsgManager::sendFileWritedMessage(const std::string &localPath) {
    FileWrited e;
    e.localPath = localPath;
    EventBus::FireEvent(e);
}


void CommMsgManager::changeHeadRetMessage(bool ret, const std::string& localHead)
{
    ChangeHeadRetMessage e(ret, localHead);
    EventBus::FireEvent(e);
}

void CommMsgManager::updateMoodRet(const std::string& userId, const std::string& mood)
{
    UpdateMoodRet e(userId, mood);
    EventBus::FireEvent(e);
}



void CommMsgManager::setSeatList(QTalk::Entity::UID uid,std::vector<QTalk::Entity::ImTransfer> transfers) {
    GetSeatListRet ret(transfers,uid);
    EventBus::FireEvent(ret);
}

void CommMsgManager::sendLoginProcessMessage(const std::string &message) {
    bool status = false;
    LoginProcessMessage e(message, status);
    EventBus::FireEvent(e);
}

void CommMsgManager::gotIncrementUser(const std::vector<QTalk::Entity::ImUserInfo> &arUserInfo,
                                      const std::vector<std::string> &arDeletes) {
    IncrementUser e;
    e.arUserInfo = arUserInfo;
    e.arDeletes = arDeletes;
    EventBus::FireEvent(e);
}

void CommMsgManager::sendGetHistoryError() {
    GetHistoryError e;
    EventBus::FireEvent(e);
}

void CommMsgManager::onUserMadelChanged(const std::vector<QTalk::Entity::ImUserStatusMedal> &userMedals) {
    UserMedalChangedEvt e;
    e.userMedals = userMedals;
    EventBus::FireEvent(e);
}

/**
 * 
 */
CommMsgListener::CommMsgListener(Communication *pComm)
        : _pComm(pComm) {
	EventBus::AddHandler<RemoveSessionData>(*this);
	EventBus::AddHandler<DataBaseOpenMessage>(*this);
	EventBus::AddHandler<LocalImgEvt>(*this);
	EventBus::AddHandler<NetImgEvt>(*this);
	EventBus::AddHandler<HistoryMessage>(*this);
	EventBus::AddHandler<GetUserCardMessage>(*this);
	EventBus::AddHandler<DownLoadHeadPhotoEvent>(*this);
	EventBus::AddHandler<DownLoadGroupHeadPhotoEvent>(*this);
	EventBus::AddHandler<NetHeadImgEvt>(*this);
	EventBus::AddHandler<GetGroupMessage>(*this);
	EventBus::AddHandler<GetUsersOnlineEvent>(*this);
	EventBus::AddHandler<GetEmoticonFileMessage>(*this);
	EventBus::AddHandler<GetNetEmoticon>(*this);
	EventBus::AddHandler<DownloadFileWithProcess>(*this);
	EventBus::AddHandler<UserCardSupple>(*this);
	EventBus::AddHandler<UserPhoneNo>(*this);
	EventBus::AddHandler<SearchInfoEvent>(*this);
	EventBus::AddHandler<StructureMessage>(*this);
	EventBus::AddHandler<AddGroupMember>(*this);
	EventBus::AddHandler<CreatGroup>(*this);
	EventBus::AddHandler<GetSessionData>(*this);
	EventBus::AddHandler<UserSettingMsg>(*this);
	EventBus::AddHandler<GetStructureCount>(*this);
	EventBus::AddHandler<GetStructureMember>(*this);
	EventBus::AddHandler<GetNavAddrInfo>(*this);
	EventBus::AddHandler<GetNavDomain>(*this);
	EventBus::AddHandler<DownloadCollection>(*this);
	EventBus::AddHandler<OAUiDataMessage>(*this);
	EventBus::AddHandler<GetGroupInfoMessage>(*this);
	EventBus::AddHandler<UpdateGroupInfoMsg>(*this);
	EventBus::AddHandler<QuitGroupMsg>(*this);
	EventBus::AddHandler<DestroyGroupMsg>(*this);
	EventBus::AddHandler<HeartBeat>(*this);
	EventBus::AddHandler<LogReportMessage>(*this);
	EventBus::AddHandler<SourceNetImage>(*this);
	EventBus::AddHandler<SetGroupAdmin>(*this);
	EventBus::AddHandler<RemoveGroupMember>(*this);
	EventBus::AddHandler<SaveConfigEvt>(*this);
	EventBus::AddHandler<ClearSystemCacheEvt>(*this);
	EventBus::AddHandler<ChangeHeadMessage>(*this);
	EventBus::AddHandler<UserOnlineState>(*this);
	EventBus::AddHandler<OperatorStatistics>(*this);
	EventBus::AddHandler<UserCardInfo>(*this);
	EventBus::AddHandler<UploadFileEvt>(*this);
	EventBus::AddHandler<RecentSessionEvt>(*this);
	EventBus::AddHandler<ContactsSessionEvt>(*this);
	EventBus::AddHandler<UpdateMoodEvt>(*this);
    EventBus::AddHandler<GetQchatToken>(*this);
	EventBus::AddHandler<ImageMessageEvt>(*this);
	EventBus::AddHandler<StartUpdaterEvt>(*this);
	EventBus::AddHandler<EmptyMessageEvt>(*this);
	EventBus::AddHandler<SetUserSeatEvt>(*this);
	EventBus::AddHandler<ServerCloseSessionEvt>(*this);
	EventBus::AddHandler<AllGroupMemberEvt>(*this);
	EventBus::AddHandler<SendProductEvt>(*this);
	EventBus::AddHandler<LocalHistoryMessage>(*this);
	EventBus::AddHandler<FileHistoryMessage>(*this);
	EventBus::AddHandler<ImageHistoryMessage>(*this);
	EventBus::AddHandler<SearchHistoryMessage>(*this);
	EventBus::AddHandler<AfterMessage>(*this);
	EventBus::AddHandler<GetSeatListEvt>(*this);
	EventBus::AddHandler<SessionTransferEvt>(*this);
	EventBus::AddHandler<QuickGroupEvt>(*this);
	EventBus::AddHandler<QuickContentByGroupEvt>(*this);
	EventBus::AddHandler<GetQchatQVTFromDB>(*this);
	EventBus::AddHandler<SaveQchatQVTToDB>(*this);
	EventBus::AddHandler<SendWechatEvt>(*this);
	EventBus::AddHandler<ReportDump>(*this);
	EventBus::AddHandler<LoginMessage>(*this);
	EventBus::AddHandler<S_RecvGroupMemberEvt>(*this);
	EventBus::AddHandler<CreatGroupRet>(*this);
	EventBus::AddHandler<S_InviteGroupMemberEvt>(*this);
	EventBus::AddHandler<S_AllFriendsEvt>(*this);
	EventBus::AddHandler<S_DealBind>(*this);
	EventBus::AddHandler<S_UpdateTimeStamp>(*this);
	EventBus::AddHandler<S_UserConfigChanged>(*this);
	EventBus::AddHandler<S_UserJoinGroup>(*this);
	EventBus::AddHandler<S_StaffChanged>(*this);
	EventBus::AddHandler<DestroyGroupRet>(*this);
	EventBus::AddHandler<RetryConnectToServerEvt>(*this);
	EventBus::AddHandler<S_AddHttpQeq>(*this);
	EventBus::AddHandler<HotLineMessageListEvt>(*this);
	EventBus::AddHandler<UpdateMsgExtendInfo>(*this);
	EventBus::AddHandler<GetHotLines>(*this);
	EventBus::AddHandler<CheckUpdaterEvt>(*this);
	EventBus::AddHandler<UserMedalEvt>(*this);
	EventBus::AddHandler<SgMedalListChanged>(*this);
	EventBus::AddHandler<SgUserMedalChanged>(*this);
	EventBus::AddHandler<GetMedalUserEvt>(*this);
	EventBus::AddHandler<ModifyUserMedalStatusEvt>(*this);
}

CommMsgListener::~CommMsgListener() {

}

/**
 * 登录成功
 * @brief onEvent
 * @param e
 */
void CommMsgListener::onEvent(DataBaseOpenMessage &e) {

    if (e.getCanceled()) {
        return;
    }

    if (nullptr != _pComm) {
        _pComm->synSeverData();
    }

}

/**
  * @函数名   onEvent
  * @功能描述 
  * @参数     LocalImgEvt 
     void
  * @author   cc
  * @date     2018/09/27
  */
void CommMsgListener::onEvent(LocalImgEvt &e) {
    if (e.getCanceled()) {
        return;
    }

    if (nullptr != _pComm && _pComm->_pFileHelper) {
        e.netFilePath = _pComm->_pFileHelper->getNetImgFilePath(e.localFilePath);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/27
  */
void CommMsgListener::onEvent(NetImgEvt &e) {
    if (e.getCanceled()) {
        return;
    }
    if (nullptr != _pComm && _pComm->_pFileHelper) {
        e.localFilePath = _pComm->_pFileHelper->getLocalImgFilePath(e.netFilePath, "/image/temp/", true);
    }
}

void CommMsgListener::onEvent(SourceNetImage &e) {
    if (e.getCanceled()) {
        return;
    }
    if (nullptr != _pComm && _pComm->_pFileHelper) {
        e.localFilePath = _pComm->_pFileHelper->getLocalImgFilePath(e.netFilePath, "/image/source/", false);
    }
}

void CommMsgListener::onEvent(SetGroupAdmin& e) {
    if(_pComm)
        _pComm->setGroupAdmin(e.groupId,e.nick,e.memberJid,e.isAdmin);
}

void CommMsgListener::onEvent(RemoveGroupMember& e) {
    _pComm->removeGroupMember(e.groupId, e.nick, e.memberJid);
}

void CommMsgListener::onEvent(SaveConfigEvt & e)
{
	if (e.getCanceled()) return;
	if (nullptr != _pComm) {
		_pComm->saveUserConfig();
	}
}

void CommMsgListener::onEvent(ClearSystemCacheEvt& e) {
    if (e.getCanceled()) return;
    if (nullptr != _pComm) {
        _pComm->clearSystemCache();
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数     HistoryMessage 
     void
  * @author   cc
  * @date     2018/09/28
  */
void CommMsgListener::onEvent(HistoryMessage &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm) {
        _pComm->getUserHistoryMessage(e.time, e.chatType, e.userid,e.realJid, e.msgList);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
void CommMsgListener::onEvent(GetUserCardMessage &e) {
    if (e.getCanceled()) return;

    if (_pComm && _pComm->_pUserManager) {
        std::vector<QTalk::StUserCard> arUserInfo;
        _pComm->_pUserManager->getUserCard(e.mapUserIds, arUserInfo);
        _pComm->_pMsgManager->sendGotUserCard(arUserInfo);
    }
}

/**
  * @函数名   
  * @功能描述 批量下载图片
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void CommMsgListener::onEvent(DownLoadHeadPhotoEvent &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        _pComm->batchUpdateHead(e._withoutHeadPhotos);
    }
}

//void MessageListener::onEvent(UserCardMessgae &e)
//{
//	if (e.getCanceled()) return;
//
//	log_info("收到服务器查询名片结果Event 个数:" << e.userCards.size();
//
//	if (_pComm && _pComm->_pFileHelper && _pComm->_pMsgManager)
//	{
//		std::vector<std::string> urls;
//
//		for (auto it = e.userCards.begin(); it != e.userCards.end(); it++)
//		{
//			urls.push_back(it->headerSrc);
//		}
//
//		auto fun = [this]() 
//		{
//			_pComm->_pMsgManager->sendDownloadHeadSuccess(); 
//		};
//		_pComm->_pFileHelper->batchDownloadHead(urls, fun);
//	}
//}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
void CommMsgListener::onEvent(DownLoadGroupHeadPhotoEvent &e) {
    if (_pComm && _pComm->_pFileHelper && _pComm->_pMsgManager) {
        _pComm->_pFileHelper->batchDownloadHead(e._withoutHeadPhotos);
        _pComm->_pMsgManager->sendDownloadGroupHeadSuccess();
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/01
  */
void CommMsgListener::onEvent(NetHeadImgEvt &e) {
    if (e.getCanceled()) {
        return;
    }
    if (nullptr != _pComm && _pComm->_pFileHelper) {
        e.localFilePath = _pComm->_pFileHelper->getLocalImgFilePath(e.netFilePath, "/image/headphoto/", false);
    }
}

// 
void CommMsgListener::onEvent(GetGroupMessage &e) {
    if (e.getCanceled()) return;

    if (nullptr != _pComm && _pComm->_pUserGroupManager) {
        _pComm->getGroupMemberById(e.groupId);
    }
}

void CommMsgListener::onEvent(GetUsersOnlineEvent &e) {
    if (e.getCanceled()) return;
    if (_pComm && _pComm->_pOnLineManager) {
        _pComm->_pOnLineManager->OnGetOnLineUser(e._users, true);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
void CommMsgListener::onEvent(GetEmoticonFileMessage &e) {
    if (e.getCanceled()) return;

    if (_pComm && _pComm->_pFileHelper) {
        e.realFilePath = _pComm->_pFileHelper->getEmotionPath(e.pkgid, e.emoShortcut, e.fileName);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
void CommMsgListener::onEvent(GetNetEmoticon &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        _pComm->getNetEmoticon(e);
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void CommMsgListener::onEvent(DownloadFileWithProcess &e) {
    if (e.getCanceled()) return;

    if (_pComm && _pComm->_pFileHelper) {
        _pComm->_pFileHelper->downloadFile(e.strUri, e.strLocalPath, true, e.processKey);
    }
}



void CommMsgListener::onEvent(UserCardSupple &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm && _pComm->_pUserManager) {
        _pComm->_pUserManager->getUserFullInfo(e.imUserSup, e.userInfo);
    }
}

void CommMsgListener::onEvent(UserPhoneNo &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm && _pComm->_pUserManager) {
        _pComm->_pUserManager->getPhoneNo(e.userId, e.phoneNo);
    }
}

void CommMsgListener::onEvent(SearchInfoEvent &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm && _pComm->_pSearchManager) {
        _pComm->_pSearchManager->GetSearchResult(e);
    }
}

void CommMsgListener::onEvent(StructureMessage &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm) {
        _pComm->getStructure(e.structure);
    }
}

void CommMsgListener::onEvent(AddGroupMember &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm) {
        _pComm->inviteGroupMembers(e.members, e.groupId);
    }
}

void CommMsgListener::onEvent(CreatGroup &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm) {
        _pComm->createGroup(e.groupId, e.groupName);
    }
}

void CommMsgListener::onEvent(GetQchatToken &e) {
    if (e.getCanceled()) {
        return;
    }
    if(_pComm){
        _pComm->getQchatTokenByQVT(e.strQVT,e.userMap);
    }
}

void CommMsgListener::onEvent(SaveQchatQVTToDB &e){
    if (e.getCanceled()) {
        return;
    }
    if(_pComm && _pComm->_pFileHelper){
        _pComm->_pFileHelper->writeQvtToFile(e.strQVT);
    }
}

void CommMsgListener::onEvent(GetQchatQVTFromDB &e){
    if (e.getCanceled()) {
        return;
    }
    if(_pComm && _pComm->_pFileHelper){
        e.strQVT = _pComm->_pFileHelper->getQvtFromFile();
    }
}


void CommMsgListener::onEvent(RemoveSessionData &e) {
    if (e.getCanceled()) {
        return;
    }
    if (_pComm) {
        _pComm->removeSession(e._peerId);
    }

}

void CommMsgListener::onEvent(GetSessionData &e) {

    if (e.getCanceled()) {
        return;
    }
    if (_pComm) {
        _pComm->getSessionData();
    }
}

void CommMsgListener::onEvent(UserSettingMsg &e) {

    if (e.getCanceled()) {
        return;
    }

    if (_pComm) {
        _pComm->_pUserConfig->updateUserSetting(e.operatorType, e.key, e.subKey, e.value);
    }
}

void CommMsgListener::onEvent(GetStructureCount &e) {
    if (e.getCanceled()) {
        return;
    }
    //
    if (_pComm) {
        _pComm->getStructureCount(e.structName, e.count);
    }
}

void CommMsgListener::onEvent(GetStructureMember &e) {
    if (e.getCanceled()) {
        return;
    }
    //
    if (_pComm) {
        _pComm->getStructureMember(e.structName, e.arMembers);
    }
}

/**
 * 获取导航信息
 * @param e
 */
void CommMsgListener::onEvent(GetNavAddrInfo &e) {
    if (e.getCanceled()) {
        return;
    }
    //
    if (_pComm) {
        QTalk::StNav nav;
        e.ret = _pComm->getNavInfo(e.navAddr, nav);
        _pComm->setLoginNav(nav);
    }
}

void CommMsgListener::onEvent(GetNavDomain &e) {
    if (e.getCanceled()) {
        return;
    }

    if (_pComm) {
        QTalk::StNav nav;
        _pComm->getNavInfo(e.navAddr, nav);
        e.doamin = nav.domain;
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(DownloadCollection &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        _pComm->downloadCollection(e.arDownloads);
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(OAUiDataMessage &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        e.ret = _pComm->geiOaUiData(e.stOAUIData);
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(GetGroupInfoMessage &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        _pComm->getGroupCardInfo(e.imGroupInfo);
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(UpdateGroupInfoMsg &e) {
    if (e.getCanceled()) return;

    if (_pComm && _pComm->_pUserGroupManager) {
        std::vector<QTalk::StGroupInfo> groupInfos;
        groupInfos.push_back(*e.groupinfo);
        _pComm->_pUserGroupManager->upateGroupInfo(groupInfos);
    }
}

void CommMsgListener::onEvent(QuitGroupMsg &e) {
    if (e.getCanceled()) return;
    if (_pComm) {
        _pComm->quitGroup(e.groupId);
    }
}

void CommMsgListener::onEvent(DestroyGroupMsg &e) {
    if (e.getCanceled()) return;
    if (_pComm) {
        _pComm->destroyGroup(e.groupId);
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(HeartBeat &e) {
    if (e.getCanceled())
        return;

    if (_pComm) {
//        _pComm->sendHeartbeat();
        _pComm->synUsersUserStatus();
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(LogReportMessage &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        _pComm->reportLog(e._desc, e._strLogPath);
    }
}

//
void CommMsgListener::onEvent(ReportDump &e) {
    if (e.getCanceled()) return;

    if (_pComm) {
        _pComm->reportDump(e.dumpFile);
    }
}

void CommMsgListener::onEvent(ChangeHeadMessage &e) {

    if(e.getCanceled()) return;

    if(nullptr != _pComm && nullptr != _pComm->_pUserManager)
    {
        _pComm->changeUserHead(e.userHead);
    }
}

void CommMsgListener::onEvent(UserOnlineState& e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm)
    {
        _pComm->sendUserOnlineState(e.login_t, e.logout_t, e.ip);
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(OperatorStatistics &e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm)
    {
        _pComm->sendOperatorStatistics(e.ip, e.operators);
    }
}

void CommMsgListener::onEvent(UserCardInfo &e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm)
    {
        _pComm->getUserCard(e.info);
    }
}

void CommMsgListener::onEvent(UploadFileEvt &e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm)
    {
        e.fileUrl = _pComm->_pFileHelper->uploadFile(e.localPath, e.withProcess, e.process_key);
    }
}

void CommMsgListener::onEvent(RecentSessionEvt &e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm)
    {
        _pComm->getRecntSession(e.sessions);
    }
}

void CommMsgListener::onEvent(ContactsSessionEvt &e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm)
    {
        _pComm->geContactsSession(e.sessions);
    }
}

void CommMsgListener::onEvent(UpdateMoodEvt& e)
{
    if(e.getCanceled()) return;

    if(nullptr != _pComm && nullptr != _pComm->_pUserManager)
    {
        std::string mood = e.mood;
        std::thread([this, mood](){
            _pComm->_pUserManager->UpdateMood(mood);
        }).detach();
    }
}

#include "../LogicManager/LogicManager.h"
void CommMsgListener::onEvent(ImageMessageEvt &e)
{
    if(e.getCanceled()) return;

    try {
        if(nullptr != _pComm )
        {
            if(e.isNext)
                LogicManager::instance()->getDatabase()->getNextImageMessage(e.messageId, e.msgs);
            else
                LogicManager::instance()->getDatabase()->getBeforeImageMessage(e.messageId, e.msgs);
        }
    }
    catch (const std::exception& e)
    {
        error_log("getNextImageMessage exception: {0}", e.what());
    }
}

void CommMsgListener::onEvent(StartUpdaterEvt &e)
{
    if(e.getCanceled()) return;
//    if(_pComm && _pComm->_pUpdaterManager)
//    {
//        _pComm->_pUpdaterManager->startUpdater(e.users);
//    }
}

void CommMsgListener::onEvent(EmptyMessageEvt &e)
{
    if(e.getCanceled()) return;
    LogicManager::instance()->getDatabase()->insertMessageInfo(e.msgInfo);
}

void CommMsgListener::onEvent(AllGroupMemberEvt &e)
{
    if(e.getCanceled()) return;
    LogicManager::instance()->getDatabase()->getAllGroupMembers(e.members);
}


void CommMsgListener::onEvent(SetUserSeatEvt &e)
{
    if(e.getCanceled()) return;
    if(_pComm){
        int sid = e.sid;
        int seat = e.seat;
        _pComm->setServiceSeat(sid,seat);
    }
}

void CommMsgListener::onEvent(ServerCloseSessionEvt &e) {
    if(e.getCanceled()) return;
    if(_pComm){
        std::string userName = e.username;
        std::string seatname = e.seatname;
        std::string virtualname = e.virtualname;

        _pComm->serverCloseSession(userName,seatname,virtualname);
    }
}

void CommMsgListener::onEvent(GetSeatListEvt& e){
    if(_pComm){
        _pComm->getSeatList(e.uid);
    }
}

void CommMsgListener::onEvent(SessionTransferEvt& e){
    if(_pComm){
        _pComm->sessionTransfer(e.uid,e.newCsrName,e.reason);
    }
}

void CommMsgListener::onEvent(SendWechatEvt &e) {
    if(_pComm){
        _pComm->sendWechat(e.uid);
    }
}

void CommMsgListener::onEvent(QuickGroupEvt & e)
{
    LogicManager::instance()->getDatabase()->getQuickGroups(e.groups);
}

void CommMsgListener::onEvent(QuickContentByGroupEvt & e)
{
    LogicManager::instance()->getDatabase()->getQuickContentByGroup(e.contents, e.groupId);
}

void CommMsgListener::onEvent(SendProductEvt& e){
    if(e.getCanceled()) return;
    if(_pComm){
        std::string userName = e.userQName;
        std::string virtualname = e.virtualname;
        std::string product = e.product;
        std::string type = e.type;
        _pComm->sendProduct(userName,virtualname,product,type);
    }
}

/**
 *
 * @param e
 */
void CommMsgListener::onEvent(LocalHistoryMessage &e)
{
    if(e.getCanceled()) return;

    if(_pComm) {
        LogicManager::instance()->getDatabase()->getLocalMessage(e.time, e.userid, e.realJid, e.msgList);
    }
}

void CommMsgListener::onEvent(FileHistoryMessage &e) {
    if(e.getCanceled()) return;

    if(_pComm) {
        LogicManager::instance()->getDatabase()->getFileMessage(e.time, e.userid, e.realJid, e.msgList);
    }
}

void CommMsgListener::onEvent(ImageHistoryMessage &e) {
    if(e.getCanceled()) return;

    if(_pComm) {
        LogicManager::instance()->getDatabase()->getImageMessage(e.time, e.userid, e.realJid, e.msgList);
    }
}

void CommMsgListener::onEvent(SearchHistoryMessage &e)
{
    if(e.getCanceled()) return;

    if(_pComm) {
        LogicManager::instance()->getDatabase()->getSearchMessage(e.time, e.userid, e.realJid, e.searchKey, e.msgList);
    }
}

void CommMsgListener::onEvent(AfterMessage &e)
{
    if(e.getCanceled()) return;

    if(_pComm) {
        LogicManager::instance()->getDatabase()->getAfterMessage(e.time, e.userid, e.realJid, e.msgList);
    }
}

void CommMsgListener::onEvent(LoginMessage &e) {
    if (e.getCanceled()) return;

    if (nullptr != _pComm)
    {
        e.ret = _pComm->OnLogin(e.strUserName, e.strPassword);
    }
}

void CommMsgListener::onEvent(S_RecvGroupMemberEvt &e)
{
    if (nullptr != _pComm)
        _pComm->onRecvGroupMembers(e.groupId, e.mapUserRole);
}

void CommMsgListener::onEvent(CreatGroupRet &e) {
    if (nullptr != _pComm)
        _pComm->onCreateGroupComplete(e.groupId, e.ret);
}

void CommMsgListener::onEvent(S_InviteGroupMemberEvt &e) {
    if (nullptr != _pComm)
        _pComm->onInviteGroupMembers(e.groupId);
}

void CommMsgListener::onEvent(S_AllFriendsEvt &e) {
    if (nullptr != _pComm)
        _pComm->onRecvFriendList(e.friends);
}

void CommMsgListener::onEvent(S_DealBind &e) {
    if (nullptr != _pComm)
        _pComm->dealBindMsg();
}

void CommMsgListener::onEvent(S_UpdateTimeStamp &e) {
    if (nullptr != _pComm)
        _pComm->updateTimeStamp();
}

void CommMsgListener::onEvent(S_UserConfigChanged &e) {
    if (nullptr != _pComm && _pComm->_pUserConfig)
        _pComm->_pUserConfig->getUserConfigFromServer(true);
}

void CommMsgListener::onEvent(S_UserJoinGroup &e) {
    if (nullptr != _pComm)
        _pComm->onUserJoinGroup(e.groupId, e.memberJid, e.affiliation);
}

void CommMsgListener::onEvent(S_StaffChanged &e) {
    if (nullptr != _pComm)
        _pComm->onStaffChanged();
}

void CommMsgListener::onEvent(DestroyGroupRet &e) {
    if (nullptr != _pComm && _pComm->_pUserConfig)
    {
        // 移除置顶
        _pComm->_pUserConfig->updateUserSetting(UserSettingMsg::EM_OPERATOR_CANCEL,
                                        "kStickJidDic",
                                        QTalk::Entity::UID(e.groupId).toStdString(),
                                        "{\"topType\":0,\"chatType\":1}");
    }
}

void CommMsgListener::onEvent(RetryConnectToServerEvt& e)
{
    if (nullptr != _pComm){
#ifdef _QCHAT
        _pComm->tryConneteToServerByQVT();
#else
        _pComm->tryConneteToServer();
#endif
    }
}

// send http request
void CommMsgListener::onEvent(S_AddHttpQeq& e)
{
    if (nullptr != _pComm)
        _pComm->addHttpRequest(e.request, e.callback);
}

void CommMsgListener::onEvent(HotLineMessageListEvt &e) {
    if (nullptr != _pComm && _pComm->_pHotLinesConfig)
        _pComm->_pHotLinesConfig->getHotLineMessageList(e.xmppId);
}

void CommMsgListener::onEvent(UpdateMsgExtendInfo &e) {
    try {
        LogicManager::instance()->getDatabase()->updateMessageExtendInfo(e.msgId, e.extendInfo);
    }
    catch (const std::exception& e)
    {
        error_log(e.what());
    }
}

void CommMsgListener::onEvent(GetHotLines &e) {
    if (nullptr != _pComm && _pComm->_pHotLinesConfig)
        _pComm->_pHotLinesConfig->getVirtualUserRole();
}

void CommMsgListener::onEvent(CheckUpdaterEvt &e) {
    if (nullptr != _pComm)
        e.updater_link = _pComm->checkUpdater(e.version);
}

void CommMsgListener::onEvent(UserMedalEvt &e)
{
    try {
        LogicManager::instance()->getDatabase()->getUserMedal(e.xmppId, e.medal);
    }
    catch (const std::exception& e)
    {
        error_log(e.what());
    }
}

void CommMsgListener::onEvent(SgMedalListChanged &e) {
    if (nullptr != _pComm)
        _pComm->getMedalList();
}

void CommMsgListener::onEvent(SgUserMedalChanged &e) {
    if (nullptr != _pComm)
        _pComm->getUserMedal(true);
}

void CommMsgListener::onEvent(GetMedalUserEvt &e) {
    if (nullptr != _pComm)
        _pComm->getMedalUser(e.medalId, e.metalUsers);
}

void CommMsgListener::onEvent(ModifyUserMedalStatusEvt &e) {
    if(nullptr != _pComm)
        e.result = _pComm->modifyUserMedalStatus(e.medalId, e.isWear);
}
