//
// Created by cc on 18-11-6.
//

#include "UserSupplementDao.h"
#include "../QtUtil/Utils/Log.h"

UserSupplementDao::UserSupplementDao(qtalk::sqlite::database *sqlDb)
    :DaoInterface(sqlDb)
{

}

bool UserSupplementDao::creatTable()
{
    if (!_pSqlDb)
    {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `IM_UserSupplement` ( "
                      "`XmppId`	        TEXT, "
                      "`UserNo`	        TEXT, "
                      "`UserMood`       TEXT, "
                      "`UserMoodVersion` INTEGER, "
                      "`LeaderId`       TEXT, "
                      "`LeaderName`     TEXT, "
                      "`PhoneNo`        TEXT, "
                      "`MailAddr`       TEXT, "
                      "`ExtendedFlag`	BLOB, "
                      "PRIMARY KEY(`XmppId`) ) ";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    bool sqlResult = query.executeStep();
    if (!sqlResult)
    {

    }
    return sqlResult;
}

/**
 * clearData
 * @return
 */
bool UserSupplementDao::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_UserSupplement`;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_UserSupplement error {0}", e.what());
        return false;
    }
}

bool UserSupplementDao::insertOrUpdateUserMood(const std::string &userId, const std::string &userMood, const int &version)
{
    if (!_pSqlDb)
    {
        return false;
    }
    int recordCount = checkRecordCount(userId);
    std::string sql;
    if(recordCount == 0)
        sql = "INSERT INTO `IM_UserSupplement` (`UserMood`, `UserMoodVersion`, `XmppId`) values (?, ?, ?);";
    else
        sql = "UPDATE `IM_UserSupplement` SET `UserMood` = ?, `UserMoodVersion` = ? WHERE `XmppId` = ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try
    {
        query.bind(1, userMood);
        query.bind(2, version);
        query.bind(3, userId);

        return  query.executeStep();
    }
    catch (std::exception &e) {

        return false;
    }
}

int UserSupplementDao::checkRecordCount(const std::string &userId)
{
    if (!_pSqlDb)
    {
        return 0;
    }

    int ret = 0;

    std::string sql = "SELECT COUNT(`XmppId`) FROM `IM_UserSupplement` WHERE XmppId = ?";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try
    {
        query.bind(1, userId);
        if (query.executeNext())
        {
            ret = query.getColumn(0).getInt();
        }
    }
    catch (std::exception &e) {

        return ret;
    }

    return ret;
}

bool UserSupplementDao::insertOrUpdateUserPhoneNo(const std::string &userId, const std::string &phoneNo)
{
    if (!_pSqlDb)
    {
        return false;
    }
    int recordCount = checkRecordCount(userId);
    std::string sql;
    if(recordCount == 0)
        sql = "INSERT INTO `IM_UserSupplement` (`PhoneNo`, `XmppId`) values (?, ?);";
    else
        sql = "UPDATE `IM_UserSupplement` SET `PhoneNo` = ? WHERE `XmppId` = ?";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try
    {
        query.bind(1, phoneNo);
        query.bind(2, userId);

        return query.executeStep();
    }
    catch (std::exception &e) {

        return false;
    }
}

bool UserSupplementDao::insertOrUpdateUserSuppl(std::shared_ptr<QTalk::Entity::ImUserSupplement> imUserSup)
{
    if (!_pSqlDb)
    {
        return false;
    }
    std::string sql = "INSERT OR REPLACE INTO `IM_UserSupplement` "
                      "(`XmppId`, `UserNo`,`UserMood`, `UserMoodVersion`, "
                      "`LeaderId`, `LeaderName`, `PhoneNo`, `MailAddr`) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try
    {
        query.bind(1, imUserSup->XmppId);
        query.bind(2, imUserSup->UserNo);
        query.bind(3, imUserSup->UserMood);
        query.bind(4, imUserSup->UserMoodVersion);
        query.bind(5, imUserSup->LeaderId);
        query.bind(6, imUserSup->LeaderName);
        query.bind(7, imUserSup->PhoneNo);
        query.bind(8, imUserSup->MailAddr);

        return query.executeStep();
    }
    catch (std::exception &e) {

        return false;
    }
}
