//
// Created by QITMAC000260 on 2019/10/15.
//

#include "UserMedalDao.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Entity/JID.h"

UserMedalDao::UserMedalDao(qtalk::sqlite::database *sqlDb)
    :DaoInterface(sqlDb)
{

}

bool UserMedalDao::creatTable() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS IM_User_Status_Medal( "
                      " medalId               INTEGER, "
                      " userId                TEXT, "
                      " host                  TEXT, "
                      " medalStatus           INTEGER, "
                      " mappingVersion        INTEGER, "
                      " updateTime            INTEGER, "
                      " primary key  (medalId, userId));";

    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("CREATE TABLE IM_User_Status_Medal error {0}", e.what());
    }
    return false;
}

bool UserMedalDao::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_User_Status_Medal;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_User_Status_Medal error {0}", e.what());
    }
    return false;
}

void UserMedalDao::insertMedals(const std::vector<QTalk::Entity::ImUserStatusMedal> &medals) {
    if (!_pSqlDb) {
        return;
    }

    std::string sql = "INSERT INTO IM_User_Status_Medal(`medalId`, `userId`, `host`, `medalStatus`, `mappingVersion`, "
                      " `updateTime` ) VALUES(?,?,?,?,?,?) "
                      "ON CONFLICT(medalId, userId) DO "
                      "update set host = ?, medalStatus = ?, `mappingVersion` = ?, `updateTime` = ? "
                      " where medalId = ? and userId = ? ";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    try {
        _pSqlDb->exec("begin immediate;");
        for(const auto & m : medals)
        {
            query.bind(1, m.medalId);
            query.bind(2, m.userId);
            query.bind(3, m.host);
            query.bind(4, m.medalStatus);
            query.bind(5, m.mappingVersion);
            query.bind(6, m.updateTime);
            query.bind(7, m.host);
            query.bind(8, m.medalStatus);
            query.bind(9, m.mappingVersion);
            query.bind(10, m.updateTime);
            query.bind(11, m.medalId);
            query.bind(12, m.userId);

            query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
    }
    catch (const std::exception& e)
    {
        _pSqlDb->exec("rollback transaction;");
        throw std::logic_error(e.what());
    }
}

void UserMedalDao::getUserMedal(const std::string &xmppId, std::set<QTalk::StUserMedal>& stMedals) {

    QTalk::Entity::JID jid(xmppId.data());
    std::string sql =
        "select u.medalId, u.medalStatus from IM_User_Status_Medal u "
        "left join IM_Medal_List l on u.medalId = l.medalId "
        "where l.status <> 0 and u.medalStatus <> 0 and u.userId = ? and u.host = ? ";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, jid.username());
    query.bind(2, jid.domainname());

    while (query.executeNext())
    {
        QTalk::StUserMedal stMedal;
        stMedal.medalId = query.getColumn(0).getInt();
        stMedal.medalStatus = query.getColumn(1).getInt();

        stMedals.insert(stMedal);
    }
}

void UserMedalDao::getMedalUsers(int medalId, std::vector<QTalk::StMedalUser> &metalUsers)
{
    std::string sql =
            "select u.xmppId, u.Name, u.HeaderSrc from IM_User_Status_Medal m "
            "left join IM_User u on m.UserId = u.UserId "
            "where m.medalId = ? and m.medalStatus <> 0 order by m.updatetime ";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, medalId);

    while (query.executeNext())
    {
        QTalk::StMedalUser stMedal;
        stMedal.xmppId = query.getColumn(0).getString();
        stMedal.userName = query.getColumn(1).getString();
        stMedal.userHead = query.getColumn(2).getString();

        metalUsers.push_back(stMedal);
    }
}

void UserMedalDao::modifyUserMedalStatus(const std::string& userId, int medalId, int status) {
    QTalk::Entity::JID jid(userId.data());

    std::string sql =
            "update IM_User_Status_Medal set `medalStatus` = ? where medalId = ? and userId = ? and `host` = ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, status);
    query.bind(2, medalId);
    query.bind(3, jid.username());
    query.bind(4, jid.domainname());
    query.executeStep();
}
