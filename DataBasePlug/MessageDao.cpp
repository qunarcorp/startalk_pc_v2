#include "MessageDao.h"
#include "../entity/im_message.h"
#include <iostream>
#include <string.h>
#include <climits>
#include "../QtUtil/Utils/Log.h"
#include "../Platform/Platform.h"
#include "../include/perfcounter.h"

#ifdef TRANSACTION_ON
#define START_TRANSACTION() sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL)
#define END_TRANSACTION()   sqlite3_exec(db, "commit transaction;", NULL, NULL, NULL)
#else
#define START_TRANSACTION()
#define END_TRANSACTION()
#endif

MessageDao::MessageDao(qtalk::sqlite::database *sqlDb) :
        DaoInterface(sqlDb) {

}

bool MessageDao::creatTable() {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "CREATE TABLE IF NOT EXISTS IM_Message ( "
                      "`MsgId` TEXT, "
                      "`XmppId` TEXT, "
                      "`ChatType` INTEGER, "
                      "`Platform` INTEGER, "
                      "`From` TEXT, "
                      "`To` TEXT, "
                      "`Content` TEXT, "
                      "`Type` INTEGER, "
                      "`State` INTEGER, "
                      "`Direction` INTEGER, "
                      "`ReadedTag` INTEGER DEFAULT 0, "
                      "`LastUpdateTime` INTEGER, "
                      "`MessageRaw` TEXT, "
                      "`RealJid` TEXT, "
                      "`ExtendedInfo` TEXT, "
                      "`ExtendedFlag` BLOB, "
                      "`BackupInfo` TEXT, "
                      "PRIMARY KEY(`MsgId`) )";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    bool sqlResult = query.executeStep();
    if (!sqlResult) {

    }
    return sqlResult;
}

/**
 * clearData
 * @return
 */
bool MessageDao::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_Message`;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_Message error {0}", e.what());
        return false;
    }
}

bool MessageDao::creatIndex() {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "CREATE INDEX IF NOT EXISTS IX_IM_MESSAGE_ID_READ_TIME ON IM_Message(XmppId, ReadedTag, LastUpdateTime);";

    qtalk::sqlite::statement query(*_pSqlDb, sql);

    return query.executeStep();;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.24
  */
bool MessageDao::insertMessageInfo(const QTalk::Entity::ImMessageInfo &imMessageInfo) {
    try {
        if (!_pSqlDb) {
            return false;
        }
        std::string sql = "INSERT OR REPLACE INTO IM_Message(MsgId, XmppId, `ChatType`, Platform, `From`, "
                          "`To`, Content, Type, State, Direction, ReadedTag, LastUpdateTime, "
                          "MessageRaw, RealJid, ExtendedInfo, ExtendedFlag, `BackupInfo` ) "
                          "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

        qtalk::sqlite::statement query(*_pSqlDb, sql);
        query.bind(1, imMessageInfo.MsgId);
        query.bind(2, imMessageInfo.XmppId);
        query.bind(3, imMessageInfo.ChatType);
        query.bind(4, imMessageInfo.Platform);
        query.bind(5, imMessageInfo.From);
        query.bind(6, imMessageInfo.To);
        query.bind(7, imMessageInfo.Content);
        query.bind(8, imMessageInfo.Type);
        query.bind(9, imMessageInfo.State);
        query.bind(10, imMessageInfo.Direction);
        query.bind(11, imMessageInfo.ReadedTag);
        query.bind(12, imMessageInfo.LastUpdateTime);
        query.bind(13, imMessageInfo.MessageRaw);
        query.bind(14, imMessageInfo.RealJid);
        query.bind(15, imMessageInfo.ExtendedInfo);
        query.bind(16, imMessageInfo.ExtendedFlag.c_str(), imMessageInfo.ExtendedFlag.size());
        query.bind(17, imMessageInfo.BackupInfo);
        bool sqlResult = query.executeStep();
        if (!sqlResult) {

        }
        return sqlResult;
    }
    catch (std::exception &e) {

        return false;
    }
}

bool MessageDao::bulkUpdateSessionList(std::map<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo> *sessionMap) {
    if (!_pSqlDb) {
        return false;
    }

    if (sessionMap == nullptr || sessionMap->empty()) {
        return true;
    }

    std::string sql = "INSERT into "
                      "IM_SessionList (UserId , LastMessageId, LastUpdateTime, ChatType, ExtendedFlag, UnreadCount, MessageState, XmppId, RealJid) "
                      "values (?, ?, ?, ?, ?, ?, ?, ?, ?) "
                      "ON CONFLICT(XmppId, RealJid) DO "
                      "update set UserId = ?, LastMessageId = ?, LastUpdateTime = ?, ChatType = ?, ExtendedFlag = ?, MessageState = ? "
                      "where XmppId = ? and RealJid = ? and LastUpdateTime < ?;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");

        std::vector<std::string> xmppIds;

        auto pItem = sessionMap->begin();

        while (pItem != sessionMap->end()) {

            std::string xmppid = pItem->first.usrId();
            std::string realJid = pItem->first.realId();

            query.bind(1, pItem->second.UserId);
            query.bind(2, pItem->second.LastMessageId);
            query.bind(3, pItem->second.LastUpdateTime);
            query.bind(4, pItem->second.ChatType);
            query.bind(5, pItem->second.ExtendedFlag);
            query.bind(6, pItem->second.UnreadCount);
            query.bind(7, pItem->second.MessageState);
            query.bind(8, xmppid);
            query.bind(9, realJid);
            query.bind(10, pItem->second.UserId);
            query.bind(11, pItem->second.LastMessageId);
            query.bind(12, pItem->second.LastUpdateTime);
            query.bind(13, pItem->second.ChatType);
            query.bind(14, pItem->second.ExtendedFlag);
//            query.bind(15, pItem->second.UnreadCount);
            query.bind(15, pItem->second.MessageState);
            query.bind(16, xmppid);
            query.bind(17, realJid);
            query.bind(18, pItem->second.LastUpdateTime);

            bool sqlResult = query.executeStep();

            if (!sqlResult) {
                xmppIds.push_back(xmppid);
            }
            query.resetBindings();

            pItem++;
        }

        query.clearBindings();

        _pSqlDb->exec("commit transaction;");
        return true;
    } catch (std::exception &e) {

        warn_log("exception : {0}", e.what());
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        return false;
    }
}

bool MessageDao::bulkInsertMessageInfo(const std::vector<QTalk::Entity::ImMessageInfo> &msgList,
                                       std::map<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo> *sessionMap) {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "INSERT OR REPLACE INTO IM_Message(MsgId, XmppId, `ChatType`, Platform, `From`, "
                      "`To`, Content, Type, State, Direction, ReadedTag, LastUpdateTime, "
                      "MessageRaw, RealJid, ExtendedInfo, ExtendedFlag, `BackupInfo` ) "
                      "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) "
                      "ON CONFLICT(MsgId) DO "
                      "update set Content = ?,Type = ?, Direction = ?, ExtendedInfo = ? where MsgId = ?";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");
        for (const QTalk::Entity::ImMessageInfo &imMessageInfo : msgList) {

            {
                std::string key = imMessageInfo.XmppId;
                std::string realJid = imMessageInfo.RealJid.empty() ? key : imMessageInfo.RealJid;
                QTalk::Entity::UID uid(key,realJid);

                auto item = sessionMap->find(uid);

                if (item == sessionMap->end()) {
                    QTalk::Entity::ImSessionInfo myInfo{};

                    myInfo.XmppId = key;
                    myInfo.ChatType = imMessageInfo.ChatType;
                    myInfo.Content = imMessageInfo.Content;
                    myInfo.LastUpdateTime = imMessageInfo.LastUpdateTime;
                    myInfo.MessageState = imMessageInfo.State;
                    myInfo.LastMessageId = imMessageInfo.MsgId;
                    myInfo.RealJid = imMessageInfo.RealJid;
                    myInfo.UserId = imMessageInfo.XmppId;
                    myInfo.MessType = imMessageInfo.Type;
                    if(imMessageInfo.From == Platform::instance().getSelfXmppId()){
                        myInfo.UnreadCount = 0;
                    } else{
                        myInfo.UnreadCount = ((imMessageInfo.ReadedTag & 2) == 2) ? 0 : 1;
                    }

                    sessionMap->insert(std::pair<QTalk::Entity::UID, QTalk::Entity::ImSessionInfo>(uid, myInfo));
                } else {
                    if (item->second.LastUpdateTime < imMessageInfo.LastUpdateTime) {
                        item->second.XmppId = key;
                        item->second.ChatType = imMessageInfo.ChatType;
                        item->second.Content = imMessageInfo.Content;
                        item->second.LastUpdateTime = imMessageInfo.LastUpdateTime;
                        item->second.MessageState = imMessageInfo.State;
                        item->second.LastMessageId = imMessageInfo.MsgId;
                        item->second.RealJid = imMessageInfo.RealJid;
                        item->second.UserId = imMessageInfo.XmppId;
                        item->second.MessType = imMessageInfo.Type;
                        if(imMessageInfo.From != Platform::instance().getSelfXmppId() && (imMessageInfo.ReadedTag & 2) != 2){
                            item->second.UnreadCount ++;
                        }
                    }
                }
            }

            query.bind(1, imMessageInfo.MsgId);
            query.bind(2, imMessageInfo.XmppId);
            query.bind(3, imMessageInfo.ChatType);
            query.bind(4, imMessageInfo.Platform);
            query.bind(5, imMessageInfo.From);
            query.bind(6, imMessageInfo.To);
            query.bind(7, imMessageInfo.Content);
            query.bind(8, imMessageInfo.Type);
            query.bind(9, imMessageInfo.State);
            query.bind(10, imMessageInfo.Direction);
            query.bind(11, imMessageInfo.ReadedTag);
            query.bind(12, imMessageInfo.LastUpdateTime);
            query.bind(13, imMessageInfo.MessageRaw);
            query.bind(14, imMessageInfo.RealJid);
            query.bind(15, imMessageInfo.ExtendedInfo);
            query.bind(16, imMessageInfo.ExtendedFlag.c_str(), imMessageInfo.ExtendedFlag.size());
            query.bind(17, imMessageInfo.BackupInfo);

            //处理类似撤销消息
            query.bind(18,imMessageInfo.Content);
            query.bind(19,imMessageInfo.Type);
            query.bind(20,imMessageInfo.Direction);
            query.bind(21,imMessageInfo.ExtendedInfo);
            query.bind(22,imMessageInfo.MsgId);

            bool sqlResult = query.executeStep();
            query.resetBindings();
            if (!sqlResult) {
//                _pSqlDb->exec("rollback transaction;");
//                return false;
            }
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    } catch (std::exception &e) {
        warn_log("exception : {0}", e.what());
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        return false;
    }
}

/**
  * @函数名   getUserMessage
  * @功能描述 获取对应人小于对应时间的10条消息
  * @参数
  * @author   cc
  * @date     2018/09/28
  */
bool MessageDao::getUserMessage(const long long &time, const std::string &userName,const std::string &realJid,
                                std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    if (!_pSqlDb) {
        return false;
    }

    QInt64 lastT = time;
    if(lastT == 0)
        lastT = LLONG_MAX;

    std::string sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`, "
                      "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid` "
                      "from IM_Message AS M "
                      "left join  IM_User AS U "
                      "on M.`From` = U.`XmppId` "
                      "where M.`XmppId` = ? "
                      "and M.`RealJid` = ? "
                      "and M.`Type` <> ?"
                      "and M.`LastUpdateTime` < ? "
                      "order by M.`LastUpdateTime` DESC "
                      "limit 10";\

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, userName);
        query.bind(2, realJid.empty() ? userName : realJid);
        query.bind(3, INT_MIN);
        query.bind(4, lastT);

        while (query.executeNext()) {
            QTalk::Entity::ImMessageInfo msgInfo;
            msgInfo.XmppId = query.getColumn(0).getString();
            msgInfo.ChatType = query.getColumn(1).getInt();
            msgInfo.UserName = query.getColumn(2).getString();
            msgInfo.Content = query.getColumn(3).getString();
            msgInfo.State = query.getColumn(4).getInt();
            msgInfo.Direction = query.getColumn(5).getInt();
            msgInfo.ReadedTag = query.getColumn(6).getInt();
            msgInfo.LastUpdateTime = query.getColumn(7).getInt64();
            msgInfo.HeadSrc = query.getColumn(8).getString();
            msgInfo.From = query.getColumn(9).getString();
            msgInfo.Type = query.getColumn(10).getInt();
            msgInfo.MsgId = query.getColumn(12).getString();
            msgInfo.ExtendedInfo = query.getColumn(13).getString();
            msgInfo.RealJid = query.getColumn(14).getString();

            msgList.insert(msgList.begin(), msgInfo);
        }

        return true;
    }
    catch (std::exception &e) {
        warn_log("MessageDao getUserMessage exception : {0}", e.what());
        return false;
    }
}

long long MessageDao::getMaxTimeStampByChatType(QTalk::Enum::ChatType chatType) {
    if (!_pSqlDb) {
        return 0;
    }

    std::string sql;
    if(chatType == QTalk::Enum::TwoPersonChat){
        sql = "SELECT max(`LastUpdateTime`) from IM_Message WHERE `ChatType` <> ? and `ChatType` <> ? and State = 1;";
    } else{
        sql = "SELECT max(`LastUpdateTime`) from IM_Message WHERE `ChatType` = ? and State = 1;";
    }

    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        QInt64 timeStamp = 0;
        if(chatType == QTalk::Enum::TwoPersonChat){
            query.bind(1, QTalk::Enum::GroupChat);
            query.bind(2, QTalk::Enum::System);
        } else{
            query.bind(1, (int) chatType);
        }
        if (query.executeNext()) {
            timeStamp = query.getColumn(0).getInt64();
        }
        return timeStamp;
    }
    catch (std::exception &e) {
        warn_log("MessageDao getMaxTimeStampByChatType exception : {0}", e.what());
    }
    return 0;
}


/**
  * @函数名   updateMState
  * @功能描述 服务器已接收消息
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool MessageDao::updateMState(const std::string &messageId, const QInt64 &time) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "UPDATE IM_Message SET `State` = 1, LastUpdateTime = ? WHERE `MsgId` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, time);
        query.bind(2, messageId);
        if (query.executeStep()) {
            return true;
        }
        return false;
    }
    catch (std::exception &e) {
        warn_log("MessageDao updateMState exception : {0}", e.what());
        return false;
    }
}

/**
  * @函数名   updateReadMask
  * @功能描述 更新消息阅读状态
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool MessageDao::updateReadMask(const std::map<std::string, QInt32> &readMasks) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "UPDATE IM_Message SET `ReadedTag` = (`ReadedTag`|? ) WHERE `MsgId` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");

        for (const auto & readMask : readMasks) {
            query.bind(1, readMask.second);
            query.bind(2, readMask.first);
            query.executeStep();
            query.resetBindings();
        }

        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    }
    catch (std::exception &e) {
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        warn_log("MessageDao updateReadMask exception : {0}", e.what());
        return false;
    }
}

//
bool MessageDao::updateReadMask(const std::map<std::string, QInt64> &readMasks) {
    if (!_pSqlDb) {
        return false;
    }

    if(readMasks.empty())
    {
        return true;
    }

    std::map<std::string, QInt64> minUnReadTimes;
    getMinUnReadTimeStamp(readMasks, minUnReadTimes);

    std::string sql = "UPDATE IM_Message SET `ReadedTag` = (`ReadedTag`| 3 ) "
                      "WHERE `XmppId` = ? and `LastUpdateTime` <= ? and `LastUpdateTime` >= ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");

        for (const auto &readMask : readMasks) {
            query.bind(1, readMask.first);
            query.bind(2, readMask.second);
            query.bind(3, minUnReadTimes[readMask.first]);
            query.executeStep();
            query.resetBindings();
        }

        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    }
    catch (std::exception &e) {
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        error_log("MessageDao updateReadMask exception : {0}", e.what());
        return false;
    }
}

bool MessageDao::getUnreadedMessages(const std::string &messageId, std::vector<std::string> &msgIds) {
    if (!_pSqlDb) {
        return false;
    }
//    a.LastUpdateTime <= b.LastUpdateTime and 去掉时间比对
    std::string sql = "select a.MsgId  from IM_Message a, "
                      "( select XmppId, RealJid from IM_Message where MsgId = ? ) b "
                      "where a.XmppId = b.XmppId and a.RealJid = b.RealJid "
                      "and (a.ReadedTag&2) <> 2 and a.Direction = 0";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, messageId);
        while (query.executeNext()) {
            msgIds.push_back(query.getColumn(0).getString());
        }
        return true;
    }
    catch (std::exception &e) {
        error_log("MessageDao getUnreadedMessages exception : {0}", e.what());
        return false;
    }
}

bool MessageDao::getGroupMessageLastUpdateTime(const std::string &messageId, QInt64 &time) {

    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT LastUpdateTime from IM_Message WHERE MsgId = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, messageId);
        if (query.executeNext()) {
            time = query.getColumn(0).getInt64();
        }
        return true;
    }
    catch (std::exception &e) {
        warn_log("MessageDao getGroupMessageLastUpdateTime exception : {0}", e.what());
        return false;
    }

}

bool MessageDao::getGroupUnreadedCount(const std::map<std::string, QInt64> &readMasks,
                                       std::map<std::string, int> &unreadedCount) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT count(1) from IM_Message WHERE XmppId = ? and `LastUpdateTime` <= ? "
                      " and (ReadedTag&2) <> 2;;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        for (const auto &readMask : readMasks) {
            query.bind(1, readMask.first);
            query.bind(2, readMask.second);
            if (query.executeNext()) {
                unreadedCount[readMask.first] = query.getColumn(0).getInt();
            }
        }
        return true;
    } catch (std::exception &e) {
        warn_log("MessageDao getGroupUnreadedCount exception : {0}", e.what());
        return false;
    }

}

//
bool MessageDao::getMessageByMessageId(const std::string &messageId, QTalk::Entity::ImMessageInfo &imMessageInfo) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT MsgId, XmppId, `ChatType`, Platform, `From`,"
                      " `To`, Content, Type, State, Direction, ReadedTag, LastUpdateTime, "
                      "  MessageRaw, RealJid, ExtendedInfo, ExtendedFlag, `BackupInfo` from IM_Message"
                      " WHERE MsgId = ?";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    try {

        query.bind(1, messageId);

        if (query.executeNext()) {
            imMessageInfo.MsgId = query.getColumn(0).getString();
            imMessageInfo.XmppId = query.getColumn(1).getString();
            imMessageInfo.ChatType = query.getColumn(2).getInt();
            imMessageInfo.Platform = query.getColumn(3).getInt();
            imMessageInfo.From = query.getColumn(4).getString();
            imMessageInfo.To = query.getColumn(5).getString();
            imMessageInfo.Content = query.getColumn(6).getString();
            imMessageInfo.Type = query.getColumn(7).getInt();
            imMessageInfo.State = query.getColumn(8).getInt();
            imMessageInfo.Direction = query.getColumn(9).getInt();
            imMessageInfo.ReadedTag = query.getColumn(10).getInt();
            imMessageInfo.LastUpdateTime = query.getColumn(11).getInt64();
            imMessageInfo.MessageRaw = query.getColumn(12).getString();
            imMessageInfo.RealJid = query.getColumn(13).getString();
            imMessageInfo.ExtendedInfo = query.getColumn(14).getString();
            imMessageInfo.ExtendedFlag = query.getColumn(15).getInt();
            imMessageInfo.BackupInfo = query.getColumn(16).getString();
        }

        return true;
    }
    catch (std::exception &e) {
        return false;
        warn_log("MessageDao getMessageByMessageId exception : {0}", e.what());
        throw "MessageDao getMessageByMessageId exception ";
    }

}

bool MessageDao::updateRevokeMessage(const std::string &messageId) {

    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "UPDATE IM_Message SET `Content` = '[撤销一条消息]', Type = -1 "
                      "WHERE `MsgId` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, messageId);
        query.executeStep();
        return true;
    } catch (std::exception &e) {
        warn_log("MessageDao updateRevokeMessage exception : {0}", e.what());
        return false;
    }
}

/**
 *
 */
bool MessageDao::getUnreadCountById(const std::string &id,const std::string& realJid, int &count) {
    if (!_pSqlDb) {
        return false;
    }

    info_log("getUnreadCountById: {0}", id);

    std::string sql = "SELECT COUNT(`MsgId`) FROM IM_Message WHERE (ReadedTag&2) <> 2  and Direction = 0 and XmppId = ? and RealJid = ?; ";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, id);
        query.bind(2, realJid == "" ? id : realJid);
        if (query.executeNext()) {
            count = query.getColumn(0).getInt();
        }
        info_log("gotUnreadCount id:{0} count:{1}", id, count);
        return true;
    }
    catch (std::exception &e) {
        warn_log("MessageDao getUnreadCountById exception : {0}", e.what());
        return false;
    }
}

/**
 *
 */
bool MessageDao::getAtCount(const std::string &selfName, const std::string &id, int &count) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT COUNT(`MsgId`) FROM IM_Message "
                      "WHERE (ReadedTag&2) <> 2  "
                      "and Direction = 0 "
                      "and XmppId = ? "
                      "and (`Content` like '%@all%' or `Content` like ?) ; ";

    if (selfName.empty()) {
        sql = "SELECT COUNT(`MsgId`) FROM IM_Message "
              "WHERE (ReadedTag&2) <> 2  "
              "and Direction = 0 "
              "and `Content` like '%@all%'; ";
    }

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, id);
        if (!selfName.empty()) {
            query.bind(2, std::string("%@") + selfName + "%");
        }

        if (query.executeNext()) {
            count = query.getColumn(0).getInt();
        }
        return true;
    }
    catch (std::exception &e) {
        warn_log("MessageDao updateRevokeMessage exception : {0}", e.what());
        return false;
    }
    return false;
}

bool MessageDao::getAllUnreadCount(int &count) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT COUNT(`MsgId`) FROM IM_Message WHERE (ReadedTag&2) <> 2  and Direction = 0 ";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        if (query.executeNext()) {
            count = query.getColumn(0).getInt();
        }
        return true;
    }
    catch (std::exception &e) {
        warn_log("MessageDao getUnreadCountById exception : {0}", e.what());
        return false;
    }
}

/**
 *
 */
long long MessageDao::getMinUnReadTimeStamp() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT min(LastUpdateTime) FROM IM_Message where (ReadedTag&2) = 2 and Direction = 0 and ChatType = 1";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    long long ret = 0;
    if (query.executeNext()) {
        ret = query.getColumn(0).getInt64();
    }
    return ret;
}


bool MessageDao::bulkDeleteMessage(const std::vector<std::string> &groupIds) {

    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_Message WHERE `XmppId` = ?;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");
        for (std::string id : groupIds) {
            query.bind(1, id);

            query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");

        return true;

    } catch (std::exception &e) {
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        return false;
    }
}

/**
 *
 */
bool MessageDao::deleteMessageByMessageId(const std::string &meesageId) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_Message WHERE `MsgId` = ?;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, meesageId);
        return query.executeStep();
    } catch (std::exception &e) {
        error_log(" deleteMessageByMessageId exception {0}", e.what());
        return false;
    }
}

/**
 *
 * @param readMasks
 * @return
 */
void MessageDao::getMinUnReadTimeStamp(const std::map<std::string, QInt64> &readMasks, std::map<std::string, QInt64 >& minUnReadedTimes) {

    perf_counter("getMinUnReadTimeStamp<map, map> {0}", readMasks.size());

//    std::ostringstream os;
//    os << "SELECT min(LastUpdateTime), XmppId FROM IM_Message where XmppId in (";
//
//    int count = 0;
//    for(const auto& item : readMasks )
//    {
//        if(count >= readMasks.size() - 1)
//            os << "'" << item.first << "') and (ReadedTag&2) = 2 group by XmppId;";
//        else
//            os << "'" << item.first << "' ,";
//
//        count++;
//    }
//
//    std::string sql = os.str();
//    qtalk::sqlite::statement query(*_pSqlDb, sql);
//    while (query.executeNext()) {
//        QInt64 t = query.getColumn(0).getInt64();
//        std::string xmppid = query.getColumn(1).getString();
//        minUnReadedTimes[xmppid] = t;
//    }

    std::string sql = "SELECT min(LastUpdateTime) FROM IM_Message where (ReadedTag&2) <> 2 and XmppId = ?";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    for(const auto& item : readMasks)
    {
        query.bind(1, item.first);
        if(query.executeNext())
        {
            QInt64 t = query.getColumn(0).getInt64();
            minUnReadedTimes[item.first] = t;
            query.clearBindings();
        }
    }
}

void MessageDao::getBeforeImageMessage(const std::string &messageId,
                                       std::vector<std::pair<std::string, std::string>> &msgs) {

    if (!_pSqlDb) {
        return;
    }

    std::string sql = "select  MsgId,  Content "
                      "from IM_Message "
                      "where content like '%[obj type=\"image\"%' "
                      "and XmppId = (select XmppId  from IM_Message where MsgId = ?)  "
                      "and LastUpdateTime < (select LastUpdateTime from IM_Message where MsgId = ?) "
                      "order by LastUpdateTime desc "
                      "limit 20;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, messageId);
    query.bind(2, messageId);
    while (query.executeNext())
    {
        std::string msgId = query.getColumn(0).getString();
        std::string content = query.getColumn(1).getString();
        msgs.emplace_back(msgId, content);
    }
}

void
MessageDao::getNextImageMessage(const std::string &messageId, std::vector<std::pair<std::string, std::string>> &msgs) {

    if (!_pSqlDb) {
        return;
    }

    std::string sql = "select  MsgId,  Content "
                      "from IM_Message "
                      "where content like '%[obj type=\"image\"%' "
                      "and XmppId = (select XmppId  from IM_Message where MsgId = ?)  "
                      "and LastUpdateTime > (select LastUpdateTime from IM_Message where MsgId = ?) "
                      "order by LastUpdateTime  "
                      "limit 20;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, messageId);
    query.bind(2, messageId);
    while (query.executeNext())
    {
        std::string msgId = query.getColumn(0).getString();
        std::string content = query.getColumn(1).getString();
        msgs.emplace_back(msgId, content);
    }
}

void dealMessage(qtalk::sqlite::statement & query, std::vector<QTalk::Entity::ImMessageInfo> &msgList)
{
    while (query.executeNext())
    {
        QTalk::Entity::ImMessageInfo msgInfo;
        msgInfo.XmppId = query.getColumn(0).getString();
        msgInfo.ChatType = query.getColumn(1).getInt();
        msgInfo.UserName = query.getColumn(2).getString();
        msgInfo.Content = query.getColumn(3).getString();
        msgInfo.State = query.getColumn(4).getInt();
        msgInfo.Direction = query.getColumn(5).getInt();
        msgInfo.ReadedTag = query.getColumn(6).getInt();
        msgInfo.LastUpdateTime = query.getColumn(7).getInt64();
        msgInfo.HeadSrc = query.getColumn(8).getString();
        msgInfo.From = query.getColumn(9).getString();
        msgInfo.Type = query.getColumn(10).getInt();
        msgInfo.MsgId = query.getColumn(12).getString();
        msgInfo.ExtendedInfo = query.getColumn(13).getString();
        msgInfo.RealJid = query.getColumn(14).getString();

        msgList.insert(msgList.begin(), msgInfo);
    }
}

void MessageDao::getLocalMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                 std::vector<QTalk::Entity::ImMessageInfo> &msgList)
{
    if (!_pSqlDb) {
        return;
    }

    bool hasR = (time != 0);
    long long t = hasR ? time : LLONG_MAX;

    std::string sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`,  "
                      "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid`  "
                      "from IM_Message AS M  "
                      "left join  IM_User AS U  "
                      "on M.`From` = U.`XmppId` "
                      "where M.`XmppId` = ? "
                      "and M.`RealJid` = ? "
                      "and M.`LastUpdateTime` < ?  "
                      "order by M.`LastUpdateTime` DESC "
                      "limit 20;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, userId);
    query.bind(2, realJid);
    query.bind(3, t);

    dealMessage(query, msgList);

//    if(hasR)
//    {
//        sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`,  "
//              "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid`  "
//              "from IM_Message AS M  "
//              "left join  IM_User AS U  "
//              "on M.`From` = U.`XmppId` "
//              "where M.`XmppId` = ? "
//              "and M.`RealJid` = ? "
//              "and M.`LastUpdateTime` >= ?  "
//              "order by M.`LastUpdateTime` DESC "
//              "limit 10;";
//
//        qtalk::sqlite::statement query_r(*_pSqlDb, sql);
//        query_r.bind(1, userId);
//        query_r.bind(2, realJid);
//        query_r.bind(3, t);
//
//        dealMessage(query_r, msgList);
//    }
}

void MessageDao::getFileMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                std::vector<QTalk::Entity::ImMessageInfo> &msgList) {


    long long t = time == 0 ? LLONG_MAX : time;

    std::string sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`,  "
                      "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid`  "
                      "from IM_Message AS M  "
                      "left join  IM_User AS U  "
                      "on M.`From` = U.`XmppId` "
                      "where M.`XmppId` = ? "
                      "and M.`RealJid` = ? "
                      "and M.`LastUpdateTime` < ?  "
                      "and M.`Type` = ? "
                      "order by M.`LastUpdateTime` DESC "
                      "limit 20;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, userId);
    query.bind(2, realJid);
    query.bind(3, t);
    query.bind(4, QTalk::Entity::MessageTypeFile);

    dealMessage(query, msgList);
    info_log("got file message {0}", msgList.size());
}

void MessageDao::getImageMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                 std::vector<QTalk::Entity::ImMessageInfo> &msgList) {

    long long t = time == 0 ? LLONG_MAX : time;

    std::string sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`,  "
                      "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid`  "
                      "from IM_Message AS M  "
                      "left join  IM_User AS U  "
                      "on M.`From` = U.`XmppId` "
                      "where M.`XmppId` = ? "
                      "and M.`RealJid` = ? "
                      "and M.`LastUpdateTime` < ?  "
                      "and M.content like '%[obj type=\"image\"%' "
                      "order by M.`LastUpdateTime` DESC "
                      "limit 10;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, userId);
    query.bind(2, realJid);
    query.bind(3, t);

    dealMessage(query, msgList);

}

void MessageDao::getSearchMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                  const std::string &searchKey, std::vector<QTalk::Entity::ImMessageInfo> &msgList) {
    long long t = time == 0 ? LLONG_MAX : time;

    std::string sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`,  "
                      "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid`  "
                      "from IM_Message AS M  "
                      "left join  IM_User AS U  "
                      "on M.`From` = U.`XmppId` "
                      "where M.`XmppId` = ? "
                      "and M.`RealJid` = ? "
                      "and M.`LastUpdateTime` < ?  "
                      "and lower(M.content) like ? "
                      "order by M.`LastUpdateTime` DESC "
                      "limit 50 ;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, userId);
    query.bind(2, realJid);
    query.bind(3, t);
    query.bind(4, "%" + searchKey + "%");

    dealMessage(query, msgList);

}

void MessageDao::getAfterMessage(const long long &time, const std::string &userId, const std::string &realJid,
                                 std::vector<QTalk::Entity::ImMessageInfo> &msgList) {

    long long t = time == 0 ? LLONG_MAX : time;

    std::string sql = "SELECT M.`XmppId`, M.`ChatType`, U.`Name`, M.`Content`, M.`State`, M.`Direction`, M.`ReadedTag`, M.`LastUpdateTime`,  "
                      "U.`HeaderSrc`, M.`From`, M.`Type`, M.`ExtendedFlag`, M.`MsgId`, M.`ExtendedInfo`,M.`RealJid`  "
                      "from IM_Message AS M  "
                      "left join  IM_User AS U  "
                      "on M.`From` = U.`XmppId` "
                      "where M.`XmppId` = ? "
                      "and M.`RealJid` = ? "
                      "and M.`LastUpdateTime` >= ?  "
                      "order by M.`LastUpdateTime` "
                      "limit 20 ;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, userId);
    query.bind(2, realJid);
    query.bind(3, t);

    dealMessage(query, msgList);
}

void MessageDao::fixMessageType() {

    if(!_pSqlDb){
        return;
    }
    std::string sql = "update IM_Message set ChatType = ChatType - 1 where ChatType = 1 or ChatType = 2;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");
        query.executeStep();
        _pSqlDb->exec("commit transaction;");

    } catch (std::exception &e) {
        error_log(e.what());
        _pSqlDb->exec("rollback transaction;");
    }
}

void MessageDao::updateMessageReadFlags(const std::map<std::string, int> &readFlags) {
    if(!_pSqlDb){
        return;
    }
    std::string sql = "update IM_Message set ReadedTag = ?, State = 1 where MsgId = ?;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        _pSqlDb->exec("begin immediate;");

        for(const auto& flag : readFlags)
        {
            query.bind(1, flag.second);
            query.bind(2, flag.first);
            query.executeStep();

            query.clearBindings();
        }
        _pSqlDb->exec("commit transaction;");

    } catch (std::exception &e) {
        _pSqlDb->exec("rollback transaction;");
        throw e.what();
    }
}

void MessageDao::updateMessageExtendInfo(const std::string &msgId, const std::string &info) {

    if(!_pSqlDb){
        return;
    }

    std::string sql = "update IM_Message set `ExtendedInfo` = ? where `MsgId` = ? ";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, info);
    query.bind(2, msgId);

    bool sqlResult = query.executeStep();
    if (!sqlResult) {
        warn_log("excute failed {0}, {1}", msgId, info);
    }
}
