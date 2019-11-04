#include "GroupMemberDao.h"
#include "../QtUtil/Utils/Log.h"


GroupMemberDao::GroupMemberDao(qtalk::sqlite::database *sqlDb)
        : DaoInterface(sqlDb) {

}

/**
  * @函数名   creatTable
  * @功能描述 
  * @参数
     bool
  * @author   cc
  * @date     2018/10/03
  */
bool GroupMemberDao::creatTable() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `IM_Group_Member` ( "
                      "`MemberId`       TEXT,"
                      "`GroupId`        TEXT,"
                      "`MemberJid`      TEXT,"
                      "`Name`           TEXT,"
                      "`Affiliation`    TEXT,"
                      "`LastUpdateTime` INTEGER,"
                      "`ExtendedFlag`   BLOB, "
                      "PRIMARY KEY(`MemberId`,`GroupId`))";

    qtalk::sqlite::statement query(*_pSqlDb, sql);

    try {
        bool ret = query.executeStep();
        return ret;
    }
    catch (const std::exception &e) {
        error_log("CREATE TABLE IM_Group_Member error {0}", e.what());
        return false;
    }
}

/**
 * clearData
 * @return
 */
bool GroupMemberDao::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_Group_Member`;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_Group_Member error {0}", e.what());
        return false;
    }
}

/**
  * @函数名   getGroupInfoById
  * @功能描述 根据数据库id
  * @参数
     bool
  * @author   cc
  * @date     2018/10/03
  */
bool GroupMemberDao::getGroupMemberById(const std::string &groupId, std::vector<QTalk::StUserCard> &members,
                                        std::map<std::string, QUInt8> &userRole) {
    if (!_pSqlDb) {
        return false;
    }

    // 获取所有用户
    std::string sql = "SELECT U.`XmppId`, U.`HeaderSrc`, U.`Name` as name, G.`Affiliation` as role, U.`SearchIndex`"
                      "FROM IM_Group_Member AS G JOIN IM_User AS U ON G.`MemberId` = U.`XmppId` "
                      "WHERE `GroupId` = ? ORDER BY role DESC;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, groupId);

        while (query.executeNext()) {
            QTalk::StUserCard member;
            member.xmppId = query.getColumn(0).getString();
            member.headerSrc = query.getColumn(1).getString();
            member.userName = query.getColumn(2).getString();
            QUInt8 affiliation = atoi(query.getColumn(3).getText());
            std::string xmppId = member.xmppId;
            userRole[xmppId] = affiliation;
            QUInt8 aaa = userRole[xmppId];
            member.searchKey = query.getColumn(4).getString();
            members.push_back(member);
        }
        return true;
    }
    catch (const std::exception &e) {
        error_log("GroupMemberDao getGroupMemberById {0}", e.what());
        return false;
    }
}

/**
  * @函数名   bulkInsertGroupMember
  * @功能描述 批量插入群成员
  * @参数
     bool
  * @author   cc
  * @date     2018/10/11
  */
bool GroupMemberDao::bulkInsertGroupMember(const std::string &groupId, const std::map<std::string, QUInt8> &members) {
    if (!_pSqlDb) {
        return false;
    }

    info_log("向数据库插入群成员 群:{0} 人数:{1}", groupId, std::to_string(members.size()));

    // 获取所有用户
    std::string sql = "INSERT OR REPLACE INTO IM_Group_Member (`GroupId`, `MemberId`, `Affiliation`) values (?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");

        for (const auto &member : members) {
            query.bind(1, groupId);
            query.bind(2, member.first);
            query.bind(3, member.second);
            bool sqlResult = query.executeStep();
            query.resetBindings();

            if (!sqlResult) {
                continue;
            }
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        return false;
    }


}

bool GroupMemberDao::bulkDeleteGroupMember(const std::vector<std::string> &groupIds) {

    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_Group_Member WHERE `GroupId` = ?;";

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
 * @param users
 */
void GroupMemberDao::getCareUsers(std::set<std::string>& users)
{

    if (!_pSqlDb) {
        return;
    }

    std::string sql = "select distinct memberId from IM_Group_Member where GroupId in "
                      "(select distinct xmppid from IM_SessionList where ChatType = 1 order by LastUpdateTime desc limit 20 ) "
                      "union "
                      "select distinct xmppid from IM_SessionList where ChatType = 0";

    qtalk::sqlite::statement query(*_pSqlDb, sql);

    while (query.executeNext())
    {
        users.insert(query.getColumn(0).getString());
    }
}

/**
 *
 */
void GroupMemberDao::getAllGroupMembers(std::map<std::string, std::set<std::string>> &members)
{
    if (!_pSqlDb) {
        return;
    }

    std::string sql = "select memberId, GroupId from IM_Group_Member";

    qtalk::sqlite::statement query(*_pSqlDb, sql);

    while (query.executeNext())
    {
        std::string memberId = query.getColumn(0).getString();
        std::string groupId = query.getColumn(1).getString();
        members[groupId].insert(memberId);
    }
}
