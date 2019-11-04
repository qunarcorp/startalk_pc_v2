
#ifndef _CHARMESSAGE_H_
#define _CHARMESSAGE_H_

#include <memory>
#include <set>
#include "../EventBus/Event.hpp"
#include "../include/CommonDefine.h"
#include "../QtUtil/Enum/im_enum.h"
#include "../entity/IM_Session.h"
#include "../entity/im_message.h"
#include "../include/CommonStrcut.h"
#include "../entity/UID.h"
#include <map>
#include <utility>

typedef std::vector<QTalk::Entity::ImMessageInfo> VectorMessage;

// 发送消息
class S_Message : public Event
{
public:
	QInt64                          time;
	QUInt8                          chatType;
	std::string                     userId;
	QTalk::Entity::ImMessageInfo    message;
};

// 接收消息
class R_Message : public Event
{
public:
	QInt64                          time;
	QUInt8                          chatType;
	std::string                     userId;
	QTalk::Entity::ImMessageInfo    message;
};

class PreSendMessageEvt : public Event
{
public:
    QTalk::Entity::ImMessageInfo    message;
};

// 拉的历史消息
class HistoryMessage : public Event
{
public:
	QInt64         time;
	QUInt8         chatType;
	std::string    userid;
	std::string    realJid;
	VectorMessage  msgList;
};

class LocalHistoryMessage : public Event
{
public:
    QInt64         time;
    std::string    userid;
    std::string    realJid;
    VectorMessage  msgList;
};

class FileHistoryMessage : public Event
{
public:
    QInt64         time;
    std::string    userid;
    std::string    realJid;
    VectorMessage  msgList;
};

class ImageHistoryMessage : public Event
{
public:
    QInt64         time;
    std::string    userid;
    std::string    realJid;
    VectorMessage  msgList;
};

class SearchHistoryMessage : public Event
{
public:
    QInt64         time;
    std::string    userid;
    std::string    realJid;
    std::string    searchKey;
    VectorMessage  msgList;
};

/**
 * 后翻拉取本地历史
 */
class AfterMessage : public Event
{
public:
    QInt64         time;
    std::string    userid;
    std::string    realJid;
    VectorMessage  msgList;
};

// 获取本地图片的网络地址
class LocalImgEvt : public Event
{
public:
	std::string localFilePath;
	std::string netFilePath;

};

//获取网络图片的本地地址
class NetImgEvt : public LocalImgEvt{};

class SourceNetImage : public LocalImgEvt{};

// 获取网络头像的本地地址
class NetHeadImgEvt : public NetImgEvt{};

// 批量下载图片
class DownLoadHeadPhotoEvent : public Event
{
public:
    std::vector<std::string> _withoutHeadPhotos;
};

// 批量下载图片成功
class DownloadHeadSuccess : public Event
{
};

// 批量下载群图片
class DownLoadGroupHeadPhotoEvent : public Event
{
public:
    std::vector<std::string> _withoutHeadPhotos;
};

// 批量下载群图片成功
class DownloadGroupHeadSuccess : public Event{};

// 更新在线状态
class UpdateOnlineEvent : public Event{};

// 获取用户在离线信息
class GetUsersOnlineEvent : public Event
{
public:
    std::set<std::string> _users;

};

class GetUsersOnlineSucessEvent : public Event
{
public:
    std::map<std::string, std::string> _userstatus;
};

//
class DownloadFileWithProcess : public Event
{
public:
	std::string strUri;
	std::string strLocalPath;
	std::string processKey;
};

class FileWrited : public Event
{
public:
	std::string localPath;
};

class FileProcessMessage : public Event
{
public:
	std::string key;
	double downloadTotal;
	double downloadNow;
	double uploadTotal;
	double uploadNow;
	double speed;
	double leftTime;
};

class DownloadFileComplete : public Event {
public:
    std::string key;
    std::string localPath;
	bool finish;
};

//
class GetEmoticonFileMessage : public Event
{
public:
	GetEmoticonFileMessage(std::string  pid, std::string  sid, std::string  name) :
	    pkgid(std::move(pid)), emoShortcut(std::move(sid)), fileName(std::move(name)) {
	}

public:
	std::string pkgid;
	std::string emoShortcut;
	std::string fileName;
	std::string realFilePath;
};

//
class GetNetEmoticon : public Event
{
public:
	ArStNetEmoticon	 arEmoInfo;
};

class SearchInfoEvent : public Event
{
public:
    //"start":0, "length":5, "key":"dan.liu xuejie.bi", "qtalkId": "jingyu.he", "groupId":"Q07", "cKey":"xxx"
    int start = 0;
    int length = 0;
    std::string key;
    int action;
    //
    SearchResult searchRet;
};

//
class ReadedMessage : public Event
{
public:
    explicit ReadedMessage(std::string messageId, std::string userId, QUInt8 chatType)
        : messageId(std::move(messageId)), userId(std::move(userId)), chatType(chatType) {
    };

public:
    const std::string messageId;
    const std::string userId;
    const QUInt8 chatType;
};

class SignalReadState : public Event
{
public:
    SignalReadState(const std::map<std::string, QInt32>& mapRead, const std::string& userId,const std::string& realJid)
        :mapReadState(mapRead), userId(userId),realJid(realJid) {
    };

public:
    const std::string& userId;
    const std::string& realJid;
    const std::map<std::string, QInt32 >& mapReadState;
};

class MStateEvt : public Event
{
public:
    std::string userId;
    std::string realJid;
    std::string messageId;
    QInt64 time;
};

class GroupReadState : public Event
{
public:
    GroupReadState(const std::map<std::string, int >& readCount, const std::string& groupId)
        : mapReadCount(readCount), groupId(groupId) {
    }

public:
    const std::string& groupId;
    const std::map<std::string, int >& mapReadCount;
};

class GroupReadMState : public Event
{
public:
    GroupReadMState(const std::vector<std::string>& ids, const std::string& groupId)
            : msgIds(ids), groupId(groupId)
    {
    }

public:
    const std::string& groupId;
    const std::vector<std::string>& msgIds;
};

class RevokeMessage : public Event
{
public:
    RevokeMessage(QTalk::Entity::UID uid, std::string fromId,
                  std::string messageId)
        : uid(std::move(uid)), messageFrom(std::move(fromId)), messageId(std::move(messageId)),chatType(QTalk::Enum::ChatType::TwoPersonChat) {
    }

public:
	RevokeMessage(QTalk::Entity::UID uid, std::string fromId,
				  std::string messageId,QUInt8 chatType)
			:uid(std::move(uid)), messageFrom(std::move(fromId)), messageId(std::move(messageId)),chatType(chatType) {
	}

public:
    const QTalk::Entity::UID uid;
    const std::string messageFrom;
    const std::string messageId;
	const QUInt8 chatType;
    QInt64 time;
};

class S_RevokeMessage : public RevokeMessage
{
public:
    S_RevokeMessage(const QTalk::Entity::UID& uid, const std::string& fromId, const std::string& messageId,const QUInt8 chatType)
        :RevokeMessage(uid, fromId, messageId,chatType) {}
    ~S_RevokeMessage() override = default;
};

/**
 * 对方拉黑自己 收到反馈
 */
class R_BlackListMessage : public Event
{
public:
    std::string messageId;
    std::string messageFrom;

};

class DownloadCollection : public Event
{
public:
    explicit DownloadCollection(const std::vector<std::string>& downloads)
		:arDownloads(downloads){}

	~DownloadCollection() override = default;

public:
    const std::vector<std::string>& arDownloads;
};

class ForwardMessage : public Event
{
public:
    ForwardMessage(std::string  messageId, std::map<std::string, int>  users)
        :messageId(std::move(messageId)), users(std::move(users)) {}
public:
    std::string messageId;
	std::map<std::string, int> users;
};

class LogReportMessage: public Event
{
public:
    LogReportMessage(std::string  desc, std::string  logPath)
        : _strLogPath(std::move(logPath)), _desc(std::move(desc)) {
    }

public:
    std::string _desc;
    std::string _strLogPath;
};

class ReportDump : public Event
{
public:
  explicit ReportDump(std::string  dump):dumpFile(std::move(dump)) {}

public:
    std::string dumpFile;
};

class CheckUpdaterEvt : public Event
{
public:
    int version = 0;
    std::string updater_link;
};

class LogReportMessageRet : public Event
{
public:
    explicit LogReportMessageRet(bool isSuccess, std::string  msg)
            : _strLogRetMsg(std::move(msg)), _success(isSuccess)
    {
    }

public:
    bool _success;
    std::string _strLogRetMsg;
};

class RecvVideoMessage : public Event
{
public:
    explicit RecvVideoMessage(std::string userId) : _userId(std::move(userId)) {}

public:
    std::string _userId;
};


class SaveConfigEvt : public Event
{
public:
	SaveConfigEvt() = default;

};

class ClearSystemCacheEvt : public Event
{
public:
	ClearSystemCacheEvt() = default;

};

class ChangeHeadMessage : public Event
{
public:
    explicit ChangeHeadMessage(std::string userHead)
        : userHead(std::move(userHead)) {};

public:
    std::string userHead;
};

class ChangeHeadRetMessage : public Event
{
public:
    explicit ChangeHeadRetMessage(bool ret, std::string localHead)
        : ret(ret), localHead(std::move(localHead))
    {
    }

public:
    bool ret{};
    std::string localHead;
};

class UploadFileEvt : public Event
{
public:
    explicit UploadFileEvt(std::string localPath, bool withProcess, std::string processKey)
        : localPath(std::move(localPath)), withProcess(withProcess), process_key(std::move(processKey)) {

    }

public:
    bool        withProcess;
    std::string localPath;
    std::string fileUrl;
	std::string process_key;
};

class RecentSessionEvt : public Event
{
public:
    explicit RecentSessionEvt(std::vector<QTalk::StShareSession>& ss)
        : sessions(ss) {
    };

public:
    std::vector<QTalk::StShareSession>& sessions;
};

class ContactsSessionEvt : public Event
{
public:
    explicit ContactsSessionEvt(std::vector<QTalk::StShareSession>& ss)
            : sessions(ss) {
    };

public:
    std::vector<QTalk::StShareSession>& sessions;
};

class ImageMessageEvt : public Event
{
public:
    explicit ImageMessageEvt(bool isNext, const std::string& messageId, std::vector<std::pair<std::string, std::string>> & msgs)
    : messageId(messageId), msgs(msgs), isNext(isNext){ };

public:
    bool isNext;
    const std::string& messageId;
    std::vector<std::pair<std::string, std::string>> & msgs;
};


class EmptyMessageEvt : public Event
{
public:
    explicit EmptyMessageEvt(QTalk::Entity::ImMessageInfo msgInfo) {
        this->msgInfo = std::move(msgInfo);
    }

public:
    QTalk::Entity::ImMessageInfo msgInfo;
};

class HotLineMessageListEvt : public Event
{
public:
    std::string xmppId;
};

class UpdateMsgExtendInfo : public Event
{
public:
    std::string msgId;
    std::string extendInfo;
};
#endif//_CHARMESSAGE_H_
