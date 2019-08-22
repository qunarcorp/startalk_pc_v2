
#ifndef _GROUPMESSAGE_H_
#define _GROUPMESSAGE_H_

#include <utility>
#include <vector>
#include <map>
#include "../EventBus/Event.hpp"
#include "../EventBus/Object.hpp"
#include "../include/CommonStrcut.h"
#include "../entity/im_user.h"
#include "../entity/im_group.h"
#include <set>

//
class GotUserGroup : public Event {};

// 
class GetGroupMessage : public Event
{
public:
	std::string groupId;
};

//
class GroupMemberMessage : public Event
{
public:
	std::string                    groupId;
	std::map<std::string, QUInt8>  userRole;
    std::map<std::string, QTalk::StUserCard> members;

};

//
class GroupTopicEvt : public Event
{
public:
	std::string groupId;
	std::string groupTopic;
};

class UpdateGroupMember : public Event
{
public:
	std::string groupId;
	std::vector<QTalk::StUserCard> userCards;
};

class StructureMessage : public Event
{
public:
    StructureMessage(Object& sender, std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>>& structure)
    :Event(sender), structure(structure) { }

public:
    std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>>& structure;
};

//
class AddGroupMember: public Event
{
public:
    AddGroupMember(Object& sender, std::vector<std::string>  members, std::string  groupId)
    :Event(sender),members(std::move(members)), groupId(std::move(groupId)) { }

public:
    std::string groupId;
    std::vector<std::string> members;

};

class CreatGroup: public Event
{
public:
    CreatGroup(Object& sender, std::string  groupId)
    :Event(sender), groupId(std::move(groupId))
    {

    }

public:
    std::string groupId;
    std::string groupName;
};

class CreatGroupRet : public Event
{
public:
    CreatGroupRet(std::string groupId, bool ret)
    : groupId(std::move(groupId)), ret(ret) { }

public:
    bool ret;
    std::string groupId;
};

class UpdateGroupInfoMsg : public Event
{
public:
	std::shared_ptr<QTalk::StGroupInfo> groupinfo;
};

class UpdateGroupInfoRet : public Event
{
public:
    std::shared_ptr<QTalk::StGroupInfo> groupinfo;
};

class GroupMemberChangeRet : public Event
{
public:
	std::string groupId;
	std::string memberJid;
	std::string nick;
	int affiliation;
	bool isAdd;
	bool isRemove;
};

class DestroyGroupRet : public Event
{
public:
	std::string groupId;
	bool        isDestroy = false;
};

class AllGroupList : public Event
{
public:
    std::vector<QTalk::Entity::ImGroupInfo> groups;
};

class GetStructureCount : public Event
{
public:
	GetStructureCount(int & count): count(count) {}

public:
	std::string structName; // 组织架构名称
	int &count; //
};

class GetStructureMember : public Event
{
public:
    explicit GetStructureMember(std::vector<std::string>& arMembers)
        : arMembers(arMembers) {}

public:
	std::string structName; // 组织架构名称
	std::vector<std::string>& arMembers;
};

class GetGroupInfoMessage : public Event
{
public:
    explicit GetGroupInfoMessage(std::shared_ptr<QTalk::Entity::ImGroupInfo>& group)
		:imGroupInfo(group) {};
public:
	std::shared_ptr<QTalk::Entity::ImGroupInfo>& imGroupInfo;
};

class SetGroupAdmin : public Event
{
public:
	std::string groupId;
	std::string nick;
	std::string memberJid;
	bool 		isAdmin;
};

class RemoveGroupMember : public Event
{
public:
	std::string groupId;
	std::string nick;
	std::string memberJid;
};

class QuitGroupMsg : public Event
{
public:
	std::string groupId;
};

class DestroyGroupMsg : public Event
{
public:
	std::string groupId;
};

class RemoveGroupMemberRet : public Event {
public:
    RemoveGroupMemberRet(std::string groupId, std::string  memberId)
    : memberId(std::move(memberId)), groupId(std::move(groupId)) {}

public:
    std::string groupId;
    std::string memberId;
};

class AllGroupMemberEvt : public Event {
public:
    explicit AllGroupMemberEvt(std::map<std::string, std::set<std::string>> &members)
        : members(members){}

public:
    std::map<std::string, std::set<std::string>> &members;
};
#endif//_GROUPMESSAGE_H_