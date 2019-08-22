#include "SessionListDao.h"
#include "../entity/IM_Session.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Utils/utils.h"
#include <algorithm>
#include <iostream>
#include <sstream>

#ifdef TRANSACTION_ON
#define START_TRANSACTION() sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL)
#define END_TRANSACTION()   sqlite3_exec(db, "commit transaction;", NULL, NULL, NULL)
#else
#define START_TRANSACTION()
#define END_TRANSACTION()
#endif

SessionListDao::SessionListDao(qtalk::sqlite::database *sqlDb) :
        _sqlDb(sqlDb) {

}

/**
  * @函数名
  * @功能描述 创建表
  * @参数
  * @date 2018.9.21
  */
bool SessionListDao::creatTable() {
    if (!_sqlDb) {
        return false;
    }
    std::string sql = "CREATE TABLE IF NOT EXISTS IM_SessionList ( "
                      "`XmppId` TEXT, "
                      "`RealJid` TEXT, "
                      "`UserId` TEXT, "
                      "`LastMessageId` TEXT, "
                      "`LastUpdateTime` INTEGER, "
                      "`ChatType` INTEGER, "
                      "`ExtendedFlag` BLOB, "
                      "`UnreadCount` INTEGER DEFAULT 0, "
                      "`MessageState` INTEGER, "
                      "PRIMARY KEY(`XmppId`, `RealJid`) )";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    bool sqlResult = query.executeStep();
    if (!sqlResult) {

    }
    return sqlResult;
}

/**
 * clearData
 * @return
 */
bool SessionListDao::clearData() {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_SessionList`;";
    try {
        qtalk::sqlite::statement query(*_sqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_SessionList error {0}", e.what());
        return false;
    }
}

/**
  * @函数名
  * @功能描述 插入会话信息
  * @参数
  * @date 2018.9.21
  */
bool SessionListDao::insertSessionInfo(const QTalk::Entity::ImSessionInfo &imSessionInfo) {
    if (!_sqlDb) {
        return false;
    }
    std::string sql = "INSERT OR REPLACE INTO IM_SessionList(XmppId, RealJid, UserId, LastMessageId, "
                      "LastUpdateTime, ChatType, ExtendedFlag, UnreadCount, MessageState ) "
                      "VALUES(?,?,?,?,?,?,?,?,?)";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        query.bind(1, imSessionInfo.XmppId);
        query.bind(2, imSessionInfo.RealJid);
        query.bind(3, imSessionInfo.UserId);
        query.bind(4, imSessionInfo.LastMessageId);
        query.bind(5, imSessionInfo.LastUpdateTime);
        query.bind(6, imSessionInfo.ChatType);
        query.bind(7, imSessionInfo.ExtendedFlag);
        query.bind(8, imSessionInfo.UnreadCount);
        query.bind(9, imSessionInfo.MessageState);
        return query.executeStep();
    }
    catch (std::exception &e) {
        return false;
    }
}

bool SessionListDao::bulkInsertSessionInfo(const std::vector<QTalk::Entity::ImSessionInfo> &sessionList) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "INSERT OR REPLACE INTO IM_SessionList(XmppId, RealJid, UserId, LastMessageId, "
                      "LastUpdateTime, ChatType, ExtendedFlag, UnreadCount, MessageState ) "
                      "VALUES(?,?,?,?,?,?,?,?,?)";

    try {
        qtalk::sqlite::statement query(*_sqlDb, sql);
        _sqlDb->exec("begin immediate;");
        for (const QTalk::Entity::ImSessionInfo &imSessionInfo : sessionList) {
            query.bind(1, imSessionInfo.XmppId);
            query.bind(2, imSessionInfo.RealJid);
            query.bind(3, imSessionInfo.UserId);
            query.bind(4, imSessionInfo.LastMessageId);
            query.bind(5, imSessionInfo.LastUpdateTime);
            query.bind(6, imSessionInfo.ChatType);
            query.bind(7, imSessionInfo.ExtendedFlag);
            query.bind(8, imSessionInfo.UnreadCount);
            query.bind(9, imSessionInfo.MessageState);
            bool sqlResult = query.executeStep();
            query.resetBindings();
            if (!sqlResult) {
//                _sqlDb->exec("rollback transaction;");
//                return false;
            }
        }
        query.clearBindings();
        _sqlDb->exec("commit transaction;");
        return true;
    } catch (std::exception &e) {
        warn_log("exception : {0}", e.what());
        _sqlDb->exec("rollback transaction;");
        return false;
    }
}

/**
  * @函数名
  * @功能描述 获取所有最后一次会话信息
  * @参数
  * @date 2018.9.21
  */
std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > SessionListDao::QueryImSessionInfos() {
    if (!_sqlDb) {
        return nullptr;
    }
    //
    //initSession();

//    updateUnreadCount();

    return reloadSession();
}

std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > SessionListDao::reloadSession() {
    if (!_sqlDb) {
        return nullptr;
    }
    //
    std::string sql = "SELECT a.XmppId, a.RealJid, b.`From` as UserId, a.LastMessageId, b.LastUpdateTime, a.ChatType, a.ExtendedFlag, "
                      "a.UnreadCount, a.MessageState, b.Content, b.Type "
                      "FROM IM_SessionList AS a LEFT JOIN IM_Message AS b "
                      "ON a.LastMessageId = b.MsgId";

    qtalk::sqlite::statement query(*_sqlDb, sql);

    try {
        std::shared_ptr<std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> > > pImSessionInfos(
                new std::vector<std::shared_ptr<QTalk::Entity::ImSessionInfo> >);
        while (query.executeNext()) {
            std::shared_ptr<QTalk::Entity::ImSessionInfo> pImSessionInfo(new QTalk::Entity::ImSessionInfo);
            pImSessionInfo->XmppId = query.getColumn(0).getText();
            pImSessionInfo->RealJid = query.getColumn(1).getText();
            pImSessionInfo->UserId = query.getColumn(2).getText();
            pImSessionInfo->LastMessageId = query.getColumn(3).getText();
            pImSessionInfo->LastUpdateTime = query.getColumn(4).getInt64();
            pImSessionInfo->ChatType = query.getColumn(5).getInt();
            pImSessionInfo->ExtendedFlag = query.getColumn(6).getInt();
            pImSessionInfo->UnreadCount = query.getColumn(7).getInt();
            pImSessionInfo->MessageState = query.getColumn(8).getInt();
            pImSessionInfo->Content = query.getColumn(9).getText();
            pImSessionInfo->MessType = query.getColumn(10).getInt();
            pImSessionInfos->push_back(pImSessionInfo);
        }
        return pImSessionInfos;
    }
    catch (std::exception &e) {
        throw "SessionListDao::reloadSession error";
    }
}

void SessionListDao::updateUnreadCount() {
    if (!_sqlDb) {
        return;
    }

    std::string sql = "update IM_SessionList "
                      "set UnreadCount = "
                      " (select count(MsgId) "
                      " from IM_Message where ReadedTag&2 <> 2  and Direction = 0 and IM_SessionList.XmppId = XmppId and IM_SessionList.RealJid = RealJid)";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        query.executeStep();
    }
    catch (std::exception &e) {
        throw "updateUnreadCount error";
    }

}

bool SessionListDao::bulkremoveSessionMessage(const std::vector<std::string> &peerIds) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_Message WHERE `XmppId` = ?;";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        for (const std::string &id : peerIds) {
            if (!id.empty()) {
                query.bind(1, id);
                query.executeStep();
                query.resetBindings();
            }
        }
        query.clearBindings();
        _sqlDb->exec("commit transaction;");

        return true;

    } catch (std::exception &e) {
        query.clearBindings();
        _sqlDb->exec("rollback transaction;");
        return false;
    }

}

bool SessionListDao::bulkDeleteSession(const std::vector<std::string> &groupIds) {

    if (!_sqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_SessionList WHERE `XmppId` = ?;";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        for (const std::string &id : groupIds) {
            query.bind(1, id);

            query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _sqlDb->exec("commit transaction;");

        return true;

    } catch (std::exception &e) {
        query.clearBindings();
        _sqlDb->exec("rollback transaction;");
        return false;
    }
}

bool SessionListDao::deleteAllSession() {
    if (!_sqlDb)
        return false;
    //
    std::string sql = "DELETE FROM IM_SessionList;";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        return query.executeStep();
    } catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}


bool SessionListDao::initSessionInfo(std::vector<std::string> *userList) {
    if (userList != nullptr && !userList->empty()) {

        std::ostringstream stringStream;

        stringStream << "insert or replace INTO IM_SessionList "
                        "(XmppId, RealJid, UserId, LastMessageId, LastUpdateTime, ChatType) "
                        "select XmppId, RealJid, `from`, MsgId, max(LastUpdateTime), ChatType from IM_Message where XmppId in ( ";

        int count = 0;

        for (const auto &id : *userList) {
            if (count >= userList->size() - 1) {
                stringStream << QTalk::utils::format("'%s') group by IM_Message.XmppId;", id.c_str());
            } else {
                stringStream << QTalk::utils::format("'%s', ", id.c_str());
            }
            count++;
        }

        qtalk::sqlite::statement update(*_sqlDb, stringStream.str());
        try {
            return update.executeStep();
        } catch (std::exception &e) {
            error_log(e.what());
            return false;
        }
        return true;
    }
    return false;
}

bool SessionListDao::initConfigSessions() {
    if (!_sqlDb) {
        return false;
    }

    std::vector<std::string> users;

    std::string key = "kStickJidDic";

    // 获取所有用户
    std::string sql = "SELECT `ConfigSubKey`, `ConfigValue` FROM IM_Config WHERE `ConfigKey` = ?;";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        query.bind(1, key);

        while (query.executeNext()) {
            users.push_back(query.getColumn(0).getString());
        }

    }
    catch (const std::exception &e) {
        error_log(std::string("exception : ") + e.what());
    }

    std::vector<std::string> peerids;

    if (!users.empty()) {
        for (const std::string &user : users) {
            if (user.length() > 0) {
                auto pos = user.find_first_of("<>");
                if (pos != std::string::npos) {
                    std::string peerId = user.substr(0, pos);
                    peerids.push_back(peerId);
                }
            }
        }
    }

    return initSessionInfo(&peerids);


}

bool SessionListDao::initSession() {
    if (!_sqlDb)
        return false;
    //
    std::string sql = "insert or REPLACE INTO IM_SessionList "
                      "(XmppId, RealJid, UserId, LastMessageId, LastUpdateTime, ChatType ) "
                      "select XmppId, RealJid, `from`, MsgId, max(LastUpdateTime), ChatType from IM_Message where XmppId in "
                      "(select DISTINCT(XmppId) from IM_Message where (((ReadedTag&2) <> 2)  and Direction = 0))  group by IM_Message.XmppId;";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        return query.executeStep();
    } catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

//void SessionListDao::searchSessionByUser(const std::string &key, std::vector<QTalk::StShareSession> &sessions, int pos,
//                                         int count) {
//
//    if (pos > 0 || count > 0) {
//        std::string sql = "Select UserId, XmppId, Name, DescInfo, HeaderSrc,SearchIndex from IM_User as a LEFT JOIN "
//                          "(select *from IM_Config where ConfigValue like '%%?%%' and ConfigKey='kMarkupNames') as b "
//                          "where (a.XmppId=b.ConfigSubKey or a.UserId like '%%?%%' OR a.Name like '%%?%%' OR "
//                          "a.SearchIndex like '%%?%%') limit ? offset ?;";
//
//        qtalk::sqlite::statement query(*_sqlDb, sql);
//
//        query.bind(1, key);
//        query.bind(2, key);
//        query.bind(3, key);
//        query.bind(4, key);
//        query.bind(5, count);
//        query.bind(6, pos);
//
//        while (query.executeNext()) {
//            QTalk::StShareSession sess;
//            sess.chatType = QTalk::Enum::ChatType::TwoPersonChat;
//            sess.xmppId = query.getColumn(1).getString();
//            sess.realJid = query.getColumn(1).getString();
//            sess.headUrl = query.getColumn(4).getString();
//            sess.name = query.getColumn(2).getString();
//            sessions.push_back(sess);
//        }
//    } else {
//        std::string sql = "Select UserId, XmppId, Name, DescInfo, HeaderSrc,SearchIndex from IM_User as a LEFT JOIN "
//                          "(select *from IM_Config where ConfigValue like '%%?%%' and ConfigKey='kMarkupNames') as b "
//                          "where (a.XmppId=b.ConfigSubKey or a.UserId like '%%?%%' OR a.Name like '%%?%%' OR "
//                          "a.SearchIndex like '%%?%%');";
//
//        qtalk::sqlite::statement query(*_sqlDb, sql);
//
//        query.bind(1, key);
//        query.bind(2, key);
//        query.bind(3, key);
//        query.bind(4, key);
//
//        while (query.executeNext()) {
//            QTalk::StShareSession sess;
//            sess.chatType = QTalk::Enum::ChatType::TwoPersonChat;
//            sess.xmppId = query.getColumn(1).getString();
//            sess.realJid = query.getColumn(1).getString();
//            sess.headUrl = query.getColumn(4).getString();
//            sess.name = query.getColumn(2).getString();
//            sessions.push_back(sess);
//        }
//    }
//}

/**
 *
 * @param key
 * @param sessions
 */
void SessionListDao::getRecentSession(std::vector<QTalk::StShareSession> &sessions)
{
    if (!_sqlDb)
        return;
    //
    std::string sql = "SELECT S.CHATTYPE, S.XMPPID, S.REALJID, "
                      "(CASE  S.CHATTYPE WHEN 1 THEN G.HEADERSRC ELSE U.HEADERSRC END) AS HEAD, "
                      "(CASE  S.CHATTYPE WHEN 1 THEN G.NAME ELSE U.NAME END) AS N, "
                      "U.SearchIndex "
                      "FROM IM_SESSIONLIST S "
                      "LEFT JOIN IM_USER U ON S.XMPPID = U.XMPPID "
                      "LEFT JOIN IM_GROUP G ON S.XMPPID = G.GROUPID "
                      " order by S.LastUpdateTime desc;";

    qtalk::sqlite::statement query(*_sqlDb, sql);

    while (query.executeNext())
    {
        QTalk::StShareSession sess;
        sess.chatType = query.getColumn(0).getInt();
        sess.xmppId = query.getColumn(1).getString();
        sess.realJid = query.getColumn(2).getString();
        sess.headUrl = query.getColumn(3).getString();
        sess.name = query.getColumn(4).getString();
        sess.searchKey = query.getColumn(5).getString();
        sessions.push_back(sess);
    }
}

void SessionListDao::updateRealJidForFixBug() {
    if(!_sqlDb){
        return;
    }
    std::string sql = "update IM_Message set RealJid = XmppId where content='' and RealJid ='';";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        query.executeStep();
        _sqlDb->exec("commit transaction;");

    } catch (std::exception &e) {
        _sqlDb->exec("rollback transaction;");
    }
}

/**
 *
 */
void SessionListDao::fixMessageType()
{
    if(!_sqlDb){
        return;
    }
    std::string sql = "update IM_SessionList set ChatType = ChatType - 1 where ChatType = 1 or ChatType = 2;";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        query.executeStep();
        _sqlDb->exec("commit transaction;");

    } catch (std::exception &e) {
        error_log(e.what());
        _sqlDb->exec("rollback transaction;");
    }
}
