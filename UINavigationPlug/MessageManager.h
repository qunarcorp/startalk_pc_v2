#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "../EventBus/EventHandler.hpp"
#include "../EventBus/Object.hpp"
#include "../EventBus/HandlerRegistration.hpp"
#include "NavigationMianPanel.h"
#include "../Message/StatusMessage.h"
#include "../Message/LoginMessgae.h"
#include "../Message/GroupMessage.h"
#include "../Message/UserMessage.h"
#include <set>


class SynOfflineSuccees;

class NavigationMsgManager : public Object {
public:
    NavigationMsgManager();

    ~NavigationMsgManager() override;

public:
    void getSessionData();

    void removeSession(std::string &peerId);

    void sendDownLoadHeadPhotosMsg(std::vector<std::string> &_withoutHeadPhotos,
                                   std::vector<std::string> &_withoutGroupHeadPhotos);

    void sendGetUserStatus(const std::set<std::string> &users);

    void retryConnecToServer();

    void sendReadedMessage(const std::string &messageId, const std::string& userId, QUInt8 chatType);

    void setUserSetting(bool isSetting, const std::string &key, const std::string &subKey, const std::string &value);

    void addEmptyMessage(const QTalk::Entity::ImMessageInfo& info);
};

/**
 * @brief The MessageListener class
 * 消息监听器
 * @author cyz
 * @date 2018.9.20
 */
class NavigationMsgListener
        : public EventHandler<R_Message>,
          public EventHandler<S_Message>,
          public EventHandler<DownloadHeadSuccess>,
          public EventHandler<DownloadGroupHeadSuccess>,
          public EventHandler<UpdateOnlineEvent>,
          public EventHandler<GetUsersOnlineSucessEvent>,
          public EventHandler<DisconnectToServer>,
//          public EventHandler<RetryConnectRet>,
          public EventHandler<SynOfflineSuccees>,
          public EventHandler<UpdateGroupInfoRet>,
          public EventHandler<SignalReadState>,
          public EventHandler<GroupReadState>,
          public EventHandler<RevokeMessage>,
          public EventHandler<S_RevokeMessage>,
          public EventHandler<UpdateUserConfigMsg>,
          public EventHandler<DestroyGroupRet>,
          public EventHandler<ChangeHeadRetMessage>,
          public EventHandler<UserCardMessgae>,
          public EventHandler<IncrementConfig>
          {
public:
    explicit NavigationMsgListener(NavigationMianPanel *navigationMianPanel);

    ~NavigationMsgListener() override;

public:
    void onEvent(R_Message &e) override;
    void onEvent(S_Message &e) override;
    void onEvent(DownloadHeadSuccess &e) override;
    void onEvent(DownloadGroupHeadSuccess &e) override;
    void onEvent(UpdateOnlineEvent &e) override;
    void onEvent(GetUsersOnlineSucessEvent &e) override;
    void onEvent(DisconnectToServer &e) override;
//    void onEvent(RetryConnectRet &e) override;
    void onEvent(SynOfflineSuccees &e) override;
    void onEvent(UpdateGroupInfoRet &e) override;
    void onEvent(SignalReadState &e) override;
    void onEvent(GroupReadState &e) override;
    void onEvent(RevokeMessage &e) override;
    void onEvent(S_RevokeMessage &e) override;
    void onEvent(UpdateUserConfigMsg &e) override;
    void onEvent(DestroyGroupRet &e) override;
    void onEvent(ChangeHeadRetMessage& e) override;
    void onEvent(UserCardMessgae& e) override;
    void onEvent(IncrementConfig& e) override;

private:
    NavigationMianPanel *_pNavigationMianPanel;
};

#endif // MESSAGEMANAGER_H
