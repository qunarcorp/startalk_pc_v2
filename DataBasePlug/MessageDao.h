#ifndef MESSAGEDAO_H
#define MESSAGEDAO_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "DaoInterface.h"
#include "../QtUtil/Enum/im_enum.h"
#include "../entity/IM_Session.h"
#include <vector>
#include <string>
#include "../include/CommonStrcut.h"
#include "../entity/UID.h"

namespace QTalk {
    namespace Entity {
        struct ImMessageInfo;
    }
}
class MessageDao : public DaoInterface{
public:
    explicit MessageDao(qtalk::sqlite::database *sqlDb = nullptr);
    bool creatTable();
    bool clearData();

public:
    bool creatIndex();

public:
    bool insertMessageInfo(const QTalk::Entity::ImMessageInfo &imMessageInfo);

    bool bulkInsertMessageInfo(const std::vector<QTalk::Entity::ImMessageInfo> &msgList,
                               std::map<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo> *sessionMap);

    bool bulkUpdateSessionList(std::map<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo> *sessionMap);

    bool bulkDeleteMessage(const std::vector<std::string> &groupIds);

    bool deleteMessageByMessageId(const std::string &meesageId);

    bool getUserMessage(const long long &time, const std::string &userName,const std::string &realJid,
                        std::vector<QTalk::Entity::ImMessageInfo> &msgList);

    long long getMaxTimeStampByChatType(QTalk::Enum::ChatType chatType);

    bool updateMState(const std::string &messageId, const QInt64 &time);

    bool updateReadMask(const std::map<std::string, QInt32> &readMasks);

    bool updateReadMask(const std::map<std::string, QInt64> &readMasks);

    bool getUnreadedMessages(const std::string &messageId, std::vector<std::string> &msgIds);

    bool getGroupMessageLastUpdateTime(const std::string &messageId, QInt64 &time);

    bool
    getGroupUnreadedCount(const std::map<std::string, QInt64> &readMasks, std::map<std::string, int> &unreadedCount);

    bool getMessageByMessageId(const std::string &messageId, QTalk::Entity::ImMessageInfo &imMessageInfo);

    // 撤销消息处理
    bool updateRevokeMessage(const std::string &messageId);

    //
    bool getUnreadCountById(const std::string &id,const std::string& realJid, int &count);

    bool getAtCount(const std::string &selfName, const std::string &id, int &count);

    bool getAllUnreadCount(int &count);

    //
    long long getMinUnReadTimeStamp();
    //
    void getMinUnReadTimeStamp(const std::map<std::string, QInt64> &readMasks, std::map<std::string, QInt64 >& minUnReadedTimes);

    void getBeforeImageMessage(const std::string& messageId,
                               std::vector<std::pair<std::string, std::string>> & msgs) ;

    void getNextImageMessage(const std::string& messageId,
                             std::vector<std::pair<std::string, std::string>> & msgs);

    // local message
    void getLocalMessage(const long long &time, const std::string &userId, const std::string &realJid,
                         std::vector<QTalk::Entity::ImMessageInfo> &msgList);

    void getFileMessage(const long long &time, const std::string &userId, const std::string &realJid,
                        std::vector<QTalk::Entity::ImMessageInfo> &msgList);

    void getImageMessage(const long long &time, const std::string &userId, const std::string &realJid,
                         std::vector<QTalk::Entity::ImMessageInfo> &msgList);

    void getSearchMessage(const long long &time, const std::string &userId, const std::string &realJid,
                          const std::string& searchKey, std::vector<QTalk::Entity::ImMessageInfo> &msgList);

    void getAfterMessage(const long long &time, const std::string &userId, const std::string &realJid,
                         std::vector<QTalk::Entity::ImMessageInfo>& msgList);

    //
    void updateMessageExtendInfo(const std::string& msgId, const std::string& info);

public:
    // 修正消息type
    void fixMessageType();
    //
    void updateMessageReadFlags(const std::map<std::string, int>& readFlags);
};

#endif // MESSAGEDAO_H
