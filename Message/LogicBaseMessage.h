//
// Created by QITMAC000260 on 2019-08-14.
//

#ifndef QTALK_V2_LOGICBASEMESSAGE_H
#define QTALK_V2_LOGICBASEMESSAGE_H

#include <string>
#include <map>
#include <utility>
#include <vector>
#include "../EventBus/Event.hpp"

// 收到群成员
class S_RecvGroupMemberEvt : public Event {
public:
    std::string groupId;
    std::map<std::string, QUInt8> mapUserRole;
};

// 邀请群成员
class S_InviteGroupMemberEvt : public Event {
public:
    std::string groupId;
};

// 好友列表
class S_AllFriendsEvt : public Event
{
public:
    std::vector<QTalk::Entity::IMFriendList> friends;
};

// deal bind
class S_DealBind : public Event{};

//
class S_UpdateTimeStamp : public Event {};

class S_UserConfigChanged : public Event {};

class S_UserJoinGroup : public Event {
public:
    std::string groupId;
    std::string memberJid;
    int affiliation;
};

class S_StaffChanged : public Event{};

class S_AddHttpQeq : public Event {
public:
    QTalk::HttpRequest request;
    std::function<void(int, const std::string &)> callback;
};
#endif //QTALK_V2_LOGICBASEMESSAGE_H
