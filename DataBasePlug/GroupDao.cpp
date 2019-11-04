#include "GroupDao.h"
#include "../entity/im_group.h"
#include "../QtUtil/Utils/Log.h"

GroupDao::GroupDao(qtalk::sqlite::database *sqlDb) :
        DaoInterface(sqlDb) {

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.27
  */
bool GroupDao::creatTable() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `IM_Group` ( "
                      "`GroupId`	TEXT, "
                      "`Name`	TEXT, "
                      "`Introduce`	TEXT, "
                      "`HeaderSrc`	TEXT, "
                      "`Topic`	TEXT, "
                      "`LastUpdateTime`	INTEGER, "
                      "`ExtendedFlag`	BLOB, "
                      " PRIMARY KEY(`GroupId`) )";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    return query.executeStep();
}

/**
 * clearData
 * @return
 */
bool GroupDao::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_Group`;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_Group error {0}", e.what());
        return false;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
bool GroupDao::insertGroupInfo(const QTalk::Entity::ImGroupInfo &groupInfo) {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "INSERT OR REPLACE INTO IM_Group(`GroupId`, `Name`, `Introduce`, `HeaderSrc`, `Topic`, "
                      "`LastUpdateTime`, `ExtendedFlag` ) "
                      "VALUES(?,?,?,?,?,?,?)";

    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        query.bind(1, groupInfo.GroupId);
        query.bind(2, groupInfo.Name);
        query.bind(3, groupInfo.Introduce);
        query.bind(4, groupInfo.HeaderSrc);
        query.bind(5, groupInfo.Topic);
        query.bind(6, groupInfo.LastUpdateTime);
        query.bind(7, groupInfo.ExtendedFlag);

        return query.executeStep();
    }
    catch (std::exception &e) {

        return false;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
bool GroupDao::bulkInsertGroupInfo(const std::vector<QTalk::Entity::ImGroupInfo> &groupInfos) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "INSERT OR IGNORE INTO IM_Group(`GroupId`, `Name`, `Introduce`, `HeaderSrc`, `Topic`, "
                      "`LastUpdateTime`, `ExtendedFlag` ) "
                      "VALUES(?,?,?,?,?,?,?)";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");
        for (const QTalk::Entity::ImGroupInfo &groupInfo : groupInfos) {
            query.bind(1, groupInfo.GroupId);
            query.bind(2, groupInfo.Name);
            query.bind(3, groupInfo.Introduce);
            query.bind(4, groupInfo.HeaderSrc);
            query.bind(5, groupInfo.Topic);
            query.bind(6, groupInfo.LastUpdateTime);
            query.bind(7, groupInfo.ExtendedFlag);
            bool sqlResult = query.executeStep();
            query.resetBindings();
            if (!sqlResult) {

            }
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
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
bool GroupDao::getGroupLastUpdateTime(long long &lastUpdateTime) {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "select Value from DB_Config where Key = 'Group' and SubKey = 'mainVersion';";

    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        if (query.executeNext()) {
            std::string version = query.getColumn(0).getString();
            lastUpdateTime = std::stol(version);
            return true;
        }
    }
    catch (const std::exception &e) {
        return false;
    }

    return false;
}

bool GroupDao::setGroupMainVersion(long long version) {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "update DB_Config set Value = ? where Key = 'Group' and SubKey = 'mainVersion';";
    bool sqlResult = false;
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        query.bind(1, std::to_string(version));
        return query.executeStep();
    } catch (const std::exception &e) {
        return false;
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/30
  */
bool GroupDao::updateGroupCard(const std::vector<QTalk::Entity::ImGroupInfo> &groups) {
    if (!_pSqlDb) {
        return false;
    }

    //
    std::string sql = "INSERT INTO IM_Group(`GroupId`, `Name`, `Introduce`, `HeaderSrc`, `Topic`, "
                      "`LastUpdateTime` ) "
                      "VALUES(?,?,?,?,?,?) "
                      "ON CONFLICT(GroupId) DO "
                      "update set Name = ?, Introduce = ?, HeaderSrc = ?, `Topic` = ? where GroupId = ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");
        for (const QTalk::Entity::ImGroupInfo& group : groups) {
            query.bind(1, group.GroupId);
            query.bind(2, group.Name);
            query.bind(3, group.Introduce);
            query.bind(4, group.HeaderSrc);
            query.bind(5, group.Topic);
            query.bind(6, group.LastUpdateTime);

            query.bind(7, group.Name);
            query.bind(8, group.Introduce);
            query.bind(9, group.HeaderSrc);
            query.bind(10, group.Topic);
            query.bind(11, group.GroupId);
            bool sqlResult = query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    }
    catch (std::exception &e) {
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        error_log(e.what());
        return false;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.30
  */
std::shared_ptr<QTalk::Entity::ImGroupInfo> GroupDao::getGroupInfoByXmppId(const std::string &xmppid) {
    if (!_pSqlDb) {
        return nullptr;
    }

    std::string sql = "SELECT `GroupId`, `Name`, `Introduce`, `HeaderSrc`, `Topic`, "
                      "`LastUpdateTime`, `ExtendedFlag` "
                      "FROM IM_Group WHERE `GroupId` = ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    query.bind(1, xmppid);
    try {
        if (query.executeNext()) {
            std::shared_ptr<QTalk::Entity::ImGroupInfo> pImGroupInfo(new QTalk::Entity::ImGroupInfo);
            pImGroupInfo->GroupId = query.getColumn(0).getText();
            pImGroupInfo->Name = query.getColumn(1).getText();
            pImGroupInfo->Introduce = query.getColumn(2).getText();
            pImGroupInfo->HeaderSrc = query.getColumn(3).getText();
            pImGroupInfo->Topic = query.getColumn(4).getText();
            pImGroupInfo->LastUpdateTime = query.getColumn(5).getInt64();
            pImGroupInfo->ExtendedFlag = query.getColumn(6).getInt();
            return pImGroupInfo;
        }

    }
    catch (std::exception &e) {
        return nullptr;
    }
    return nullptr;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/12
  */
bool GroupDao::getGroupTopic(const std::string &groupId, std::string &groupTopic) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT `Topic` FROM IM_Group WHERE `GroupId` = ?;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, groupId);
        if (query.executeNext()) {
            groupTopic = query.getColumn(0).getString();
        }
        return true;
    }
    catch (std::exception &e) {
        warn_log("getGroupTopic exception {0}", e.what());
        return false;
    }
}

bool GroupDao::bulkDeleteGroup(const std::vector<std::string> &groupIds) {

    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_Group WHERE `GroupId` = ?;";

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

bool GroupDao::getAllGroup(std::vector<QTalk::Entity::ImGroupInfo> &groups) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT `GroupId`, `Name`, `Introduce`, `HeaderSrc`, `Topic`, "
                      "`LastUpdateTime`, `ExtendedFlag` "
                      "FROM IM_Group";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        while (query.executeNext()) {
            QTalk::Entity::ImGroupInfo imGroupInfo;
            imGroupInfo.GroupId = query.getColumn(0).getText();
            imGroupInfo.Name = query.getColumn(1).getText();
            imGroupInfo.Introduce = query.getColumn(2).getText();
            imGroupInfo.HeaderSrc = query.getColumn(3).getText();
            imGroupInfo.Topic = query.getColumn(4).getText();
            imGroupInfo.LastUpdateTime = query.getColumn(5).getInt64();
            imGroupInfo.ExtendedFlag = query.getColumn(6).getInt();
            groups.push_back(imGroupInfo);
        }
        return true;
    }
    catch (std::exception &e) {
        error_log("getAllGroup error {0}", e.what());
        return false;
    }

}

/**
 * getGroupCardById
 * @param imGroupInfo
 * @return
 */
bool GroupDao::getGroupCardById(std::shared_ptr<QTalk::Entity::ImGroupInfo> &imGroupInfo) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "SELECT `GroupId`, `Name`, `Introduce`, `HeaderSrc`, `Topic`, "
                      "`LastUpdateTime`, `ExtendedFlag` "
                      "FROM IM_Group WHERE GroupId = ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, imGroupInfo->GroupId);

        if (query.executeNext()) {
            imGroupInfo->GroupId = query.getColumn(0).getText();
            imGroupInfo->Name = query.getColumn(1).getText();
            imGroupInfo->Introduce = query.getColumn(2).getText();
            imGroupInfo->HeaderSrc = query.getColumn(3).getText();
            imGroupInfo->Topic = query.getColumn(4).getText();
            imGroupInfo->LastUpdateTime = query.getColumn(5).getInt64();
            imGroupInfo->ExtendedFlag = query.getColumn(6).getInt();
        }
        return true;
    }
    catch (std::exception &e) {
        error_log("getGroupCardById error {0}", e.what());
        return false;
    }

}

bool GroupDao::deleteAllGroup() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_Group ";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        return query.executeStep();
    }
    catch (std::exception &e) {
        error_log("getAllGroup error {0}", e.what());
        return false;
    }
}

/**
 *
 */
void GroupDao::getGroupCardMaxVersion(long long &version)
{
    if (!_pSqlDb) {
        return;
    }

    std::string sql = "SELECT max(`LastUpdateTime`) FROM IM_Group ;";

    qtalk::sqlite::statement query(*_pSqlDb, sql);

    if (query.executeNext()) {
        version = query.getColumn(0).getInt64();
    }
}
