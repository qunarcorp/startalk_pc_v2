#ifndef _USER_MESSAGE_H_
#define _USER_MESSAGE_H_

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <set>
#include "../include/CommonStrcut.h"
#include "../EventBus/Event.hpp"
#include "../entity/im_user.h"
#include "../entity/im_userSupplement.h"
#include "../entity/im_config.h"
#include "../entity/im_friend_list.h"
#include "../entity/im_transfer.h"
#include "../entity/UID.h"
#include "../entity/im_qr_group.h"
#include "../entity/im_qr_content.h"
#include "../entity/im_user_status_medal.h"

// 获取用户列表
class GetUserCardMessage : public Event
{
public:
	std::map<std::string, std::map<std::string, int> > mapUserIds; //
};

// 用户列表结果
class UserCardMessgae : public Event
{
public:
	std::vector<QTalk::StUserCard> userCards;
};

// 用户列表结果
class UserCardSupple: public Event
{
public:
	UserCardSupple(std::shared_ptr<QTalk::Entity::ImUserSupplement>& imUserSup,
                   std::shared_ptr<QTalk::Entity::ImUserInfo>& userInfo)
	    : imUserSup(imUserSup), userInfo(userInfo)
	{
	}

public:
    std::shared_ptr<QTalk::Entity::ImUserSupplement>& imUserSup;
    std::shared_ptr<QTalk::Entity::ImUserInfo>& userInfo;
};

//
class UserPhoneNo : public Event
{
public:
	std::string userId;
	std::string phoneNo;
};

// user setting
class UserSettingMsg : public Event
{
public:
    enum {EM_OPERATOR_INVALID, EM_OPERATOR_SET, EM_OPERATOR_CANCEL};
    //
    QUInt8 operatorType = EM_OPERATOR_INVALID;
    std::string key;
    std::string subKey;
    std::string value;
};

//
class UpdateUserConfigMsg : public Event
{
public:
    std::vector<QTalk::Entity::ImConfig> arConfigs;
};

// 增量配置修改
class IncrementConfig : public Event
{
public:
    std::map<std::string, std::string> deleteData;
    std::vector<QTalk::Entity::ImConfig> arImConfig;
};

class AllFriends : public Event
{
public:
    std::vector<QTalk::Entity::IMFriendList> friends;
};

class UserCardInfo : public Event
{
public:
    explicit UserCardInfo(std::shared_ptr<QTalk::Entity::ImUserInfo>& info) : info(info) {};

public:
    std::shared_ptr<QTalk::Entity::ImUserInfo>& info;
};

class UpdateMoodEvt : public Event
{
public:
    explicit UpdateMoodEvt(std::string mood) :mood(std::move(mood)) {}

public:
    std::string mood;
};

class UpdateMoodRet : public Event
{
public:
    explicit UpdateMoodRet(std::string userId, std::string mood)
            :mood(std::move(mood)), userId(std::move(userId)){}

public:
    std::string userId;
    std::string mood;
};

class SwitchUserStatusEvt : public Event
{
public:
    explicit SwitchUserStatusEvt(std::string status) : user_status(std::move(status)){};

public:
    std::string user_status;
};

class SwitchUserStatusRet : public Event
{
public:
    explicit SwitchUserStatusRet(std::string status) : user_status(std::move(status)){};

public:
    std::string user_status;
};

class SetUserSeatEvt : public Event
{
public:
    explicit SetUserSeatEvt(int sid, int seat) :sid(sid), seat(seat) {}

public:
	int sid; int seat;
};

class ServerCloseSessionEvt : public Event
{
public:
    ServerCloseSessionEvt(std::string username,std::string seatname,std::string virtualname)
			:username(std::move(username)), seatname(std::move(seatname)),virtualname(std::move(virtualname)) {}

public:
	std::string username;
	std::string seatname;
	std::string virtualname;
};

class GetSeatListEvt : public Event
{
public:
	GetSeatListEvt(QTalk::Entity::UID _uid) :uid(std::move(_uid)) {}

public:
	QTalk::Entity::UID uid;
};

class GetSeatListRet : public Event
{

public:
	GetSeatListRet(std::vector<QTalk::Entity::ImTransfer> _transfers,QTalk::Entity::UID _uid)
		:transfers(std::move(_transfers)),uid(std::move(_uid)) {}

public:
	std::vector<QTalk::Entity::ImTransfer> transfers;
	QTalk::Entity::UID uid;

};

class SessionTransferEvt : public Event
{

public:
	SessionTransferEvt(QTalk::Entity::UID _uid,std::string _newCsrName,std::string _reason)
			:uid(std::move(_uid)),newCsrName(std::move(_newCsrName)),reason(std::move(_reason)) {}

public:
	std::string reason;
	std::string newCsrName;
	QTalk::Entity::UID uid;
};

class SendWechatEvt : public Event
{
public:
	SendWechatEvt(QTalk::Entity::UID _uid) :uid(std::move(_uid)) {}

public:
	QTalk::Entity::UID uid;
};

class SendProductEvt : public Event
{
public:
	SendProductEvt(std::string userQName,std::string virtualname, std::string product,std::string type)
			:userQName(std::move(userQName)),
			virtualname(std::move(virtualname)),
			product(std::move(product)),
			type(std::move(type)) {}

public:
	std::string userQName;
	std::string virtualname;
	std::string type;
	std::string product;
};

class FeedBackLogEvt : public Event
{
public:
	explicit FeedBackLogEvt(std::string text) : text(std::move(text)){};

public:
	std::string text;
};

class QuickGroupEvt : public Event {

public:
	explicit QuickGroupEvt(std::vector<QTalk::Entity::ImQRgroup> &groups) : groups(groups) { }

public:
	std::vector<QTalk::Entity::ImQRgroup> &groups;
};

class QuickContentByGroupEvt : public Event {

public:
	QuickContentByGroupEvt(std::vector<QTalk::Entity::IMQRContent>& contents, int id)
		:groupId(id), contents(contents) {}

public:
	int groupId;
	std::vector<QTalk::Entity::IMQRContent>& contents;
};

class IncrementUser : public Event {
public:
    std::vector<QTalk::Entity::ImUserInfo> arUserInfo;
    std::vector<std::string> arDeletes;
};

class GetHotLines : public Event {

};

class UserMedalEvt: public Event {
public:
    UserMedalEvt(std::string xmppId, std::set<QTalk::StUserMedal>& medal)
        :medal(medal), xmppId(std::move(xmppId)) {
    }

public:
    std::string xmppId;
    std::set<QTalk::StUserMedal>& medal;
};

class UserMedalChangedEvt : public Event
{
public:
    std::vector<QTalk::Entity::ImUserStatusMedal> userMedals;
};

class GetMedalUserEvt : public Event
{
public:
    int medalId;
    std::vector<QTalk::StMedalUser> metalUsers;
};

class ModifyUserMedalStatusEvt : public Event {
public:
    int medalId = -1;
    bool isWear = false;

    bool result = false;
};
#endif//_USER_MESSAGE_H_