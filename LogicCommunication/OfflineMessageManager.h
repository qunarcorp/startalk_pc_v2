#ifndef OFFLINEMESSAGEMANAGER_H
#define OFFLINEMESSAGEMANAGER_H

#include <map>
#include <string>
#include <vector>
#include "../include/CommonDefine.h"
#include "../QtUtil/Enum/im_enum.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../entity/IM_Session.h"
#include "../entity/im_message.h"
#include "../include/CommonStrcut.h"

#define DEM_MESSAGE_MAXTIMESTAMP "MessageMaxTime"
#define DEM_TWOPERSONCHAT        "TwoPersonChat"
#define DEM_GROUPCHAT            "GroupChat"
#define DEM_SYSTEM               "System"

class Communication;
class OfflineMessageManager
{
public:
    explicit OfflineMessageManager(Communication* comm);
    ~OfflineMessageManager()= default;

public:
    bool updateChatOfflineMessage();
    bool updateGroupOfflineMessage();
    bool updateNoticeOfflineMessage();
    void updateGroupMasks();
    void updateChatMasks();

public:
	void getUserMessage(const QInt64 &time, const std::string &userId);
	void getGroupMessage(const QInt64 &time, const std::string &userId);
	void getSystemMessage(const QInt64 &time, const std::string &userId);
	void getConsultServerMessage(const QInt64 &time, const std::string &userId, const std::string &realJid);

private:
    //
    std::string safeGetJsonStringValue(const cJSON *cjson, const char *key);
    int safeGetJsonIntValue(const cJSON *cjson, const char *key);
    QInt64 safeGetJsonLongValue(const cJSON *cjson, const char *key);

    /**
     * @brief getOfflineChatMessageJson 获取单人离线消息
     * @param chatTimestamp 单人会话记录的最新一条时间
     * @param count 获取单人消息的条数
     * @param complete 完成状态
     * @param errMsg 错误信息
     * @param outMsgList 输出消息列表
     * @param outSessionList 输出会话列表
     */
    void getOfflineChatMessageJson(long long chatTimestamp, int count, bool &complete, std::string &errMsg,
                                   std::vector<QTalk::Entity::ImMessageInfo> &outMsgList,
                                   std::vector<QTalk::Entity::ImSessionInfo> &outSessionList);

    // 获取离线群的离线消息
    void getOfflineGroupMessageJson(long long groupTimestamp, int count, bool &complete, std::string &errMsg,
                                    std::vector<QTalk::Entity::ImMessageInfo> &outMsgList,
                                    std::vector<QTalk::Entity::ImSessionInfo> &outSessionList);

    // 获取离线系统消息
    void getOfflineNoticeMessageJson(long long noticeTimestamp, int count, bool &complete, std::string &errMsg,
                                     std::vector<QTalk::Entity::ImMessageInfo> &outMsgList,
                                     std::vector<QTalk::Entity::ImSessionInfo> &outSessionList);

    void getGroupReadMark(std::map<std::string, QInt64> &readMarkList);


private:
	QInt64 getTimeStamp(QTalk::Enum::ChatType chatType);

private:
	Communication *_pComm;
};

#endif // OFFLINEMESSAGEMANAGER_H
