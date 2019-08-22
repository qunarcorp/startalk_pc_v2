#include "UserDao.h"
#include "../entity/im_user.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Utils/utils.h"
#include "../QtUtil/Enum/im_enum.h"

#include <sstream>

UserDao::UserDao(qtalk::sqlite::database *sqlDb) :
        _sqlDb(sqlDb) {

}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.27
  */
bool UserDao::creatTable() {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `IM_User` ( "
                      "`UserId`	        TEXT, "
                      "`XmppId`	        TEXT, "
                      "`Name`	            TEXT, "
                      "`DescInfo`	        TEXT, "
                      "`HeaderSrc`	    TEXT, "
                      "`SearchIndex`	    TEXT, "
                      "`NickName`	        TEXT, "
                      "`HeadVersion`	INTEGER, "
                      "`IncrementVersion`	INTEGER, "
                      "`ExtendedFlag`	BLOB, "
                      "PRIMARY KEY(`XmppId`) ) ";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    return query.executeStep();
}

/**
 *
 * @return
 */
bool UserDao::clearData() {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_User`;";
    try {
        qtalk::sqlite::statement query(*_sqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_User error {0}", e.what());
        return false;
    }
}

/**
  * @函数名   getUserVersion
  * @功能描述 获取组织架构版本号
  * @参数     version 版本号
     bool    查询结果
  * @author   cc
  * @date     2018/09/29
  */
bool UserDao::getUserVersion(int &version) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "select max(`IncrementVersion`) from IM_User ;";

    try {
        qtalk::sqlite::statement query(*_sqlDb, sql);
        if (query.executeNext()) {
            version = query.getColumn(0).getInt();
            return true;
        }

    }
    catch (const std::exception &e) {
        error_log(e.what());
        return false;
    }

    return false;
}


/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
bool UserDao::insertUserInfo(const QTalk::Entity::ImUserInfo &userInfo) {

    if (!_sqlDb) {
        return false;
    }
    std::string sql = "INSERT OR REPLACE INTO IM_User(`UserId`, `XmppId`, `Name`, `DescInfo`, `HeaderSrc`, "
                      "`SearchIndex`, `NickName`, `HeadVersion`, `IncrementVersion`, `ExtendedFlag`, sex, "
                      "userType, isVisible ) "
                      "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)";

    qtalk::sqlite::statement query(*_sqlDb, sql);

    try {
        query.bind(1, userInfo.UserId);
        query.bind(2, userInfo.XmppId);
        query.bind(3, userInfo.Name);
        query.bind(4, userInfo.DescInfo);
        query.bind(5, userInfo.HeaderSrc);
        query.bind(6, userInfo.SearchIndex);
        query.bind(7, userInfo.NickName);
        query.bind(8, userInfo.HeadVersion);
        query.bind(9, userInfo.IncrementVersion);
        query.bind(10, userInfo.ExtendedFlag);
        // add at 2019.06.17
        query.bind(11, userInfo.Gender);
        query.bind(12, userInfo.userType);
        query.bind(13, userInfo.isVisible);
        // add end

        return query.executeStep();
    }
    catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
bool UserDao::bulkInsertUserInfo(const std::vector<QTalk::Entity::ImUserInfo> &userInfos) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "INSERT OR REPLACE INTO IM_User(`UserId`, `XmppId`, `Name`, `DescInfo`, `HeaderSrc`, "
                      "`SearchIndex`, `NickName`, `HeadVersion`, `IncrementVersion`, `ExtendedFlag`, sex, "
                      "userType, isVisible ) "
                      "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        for (const QTalk::Entity::ImUserInfo& userInfo : userInfos) {
            query.bind(1, userInfo.UserId);
            query.bind(2, userInfo.XmppId);
            query.bind(3, userInfo.Name);
            query.bind(4, userInfo.DescInfo);
            query.bind(5, userInfo.HeaderSrc);
            query.bind(6, userInfo.SearchIndex);
            query.bind(7, userInfo.NickName);
            query.bind(8, userInfo.HeadVersion);
            query.bind(9, userInfo.IncrementVersion);
            query.bind(10, userInfo.ExtendedFlag);

            // add at 2019.06.17
            query.bind(11, userInfo.Gender);
            query.bind(12, userInfo.userType);
            query.bind(13, userInfo.isVisible);
            // add end
            bool sqlResult = query.executeStep();
            query.resetBindings();

        }
        query.clearBindings();
        _sqlDb->exec("commit transaction;");
        return true;
    } catch (std::exception &e) {
        error_log(e.what());
        query.clearBindings();
        _sqlDb->exec("rollback transaction;");
        return false;
    }
}

/**
 * 批量删除用户
 * @param userIds
 */
bool UserDao::bulkDeleteUserInfo(const std::vector<std::string> &userIds) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM IM_User WHERE `XmppId` = ?;";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        for (const std::string& userId : userIds) {
            query.bind(1, userId);
            query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _sqlDb->exec("commit transaction;");
        return true;
    } catch (std::exception &e) {
        error_log(e.what());
        query.clearBindings();
        _sqlDb->exec("rollback transaction;");
        return false;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.29
  */
std::shared_ptr<QTalk::Entity::ImUserInfo> UserDao::getUserInfoByXmppId(const std::string &xmppid) {
    if (!_sqlDb) {
        return nullptr;
    }

    std::string sql = "SELECT `UserId`, `XmppId`, `Name`, `DescInfo`, `HeaderSrc`, "
                      "`SearchIndex`, `HeadVersion`, `IncrementVersion`, `ExtendedFlag`, `NickName`, `mood`"
                      "FROM IM_User WHERE `XmppId` = ?";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    query.bind(1, xmppid);
    try {
        if (query.executeNext()) {
            std::shared_ptr<QTalk::Entity::ImUserInfo> pImUserInfo(new QTalk::Entity::ImUserInfo);
            pImUserInfo->UserId = query.getColumn(0).getText();
            pImUserInfo->XmppId = query.getColumn(1).getText();
            pImUserInfo->Name = query.getColumn(2).getText();
            pImUserInfo->DescInfo = query.getColumn(3).getText();
            pImUserInfo->HeaderSrc = query.getColumn(4).getText();
            pImUserInfo->SearchIndex = query.getColumn(5).getText();
            pImUserInfo->HeadVersion = query.getColumn(6).getInt();
            pImUserInfo->IncrementVersion = query.getColumn(7).getInt();
            pImUserInfo->ExtendedFlag = query.getColumn(8).getInt();
            pImUserInfo->NickName = query.getColumn(9).getText();
            pImUserInfo->Mood = query.getColumn(10).getText();
            return pImUserInfo;
        }
        else
        {
            info_log("no user info {0}", xmppid);
            return nullptr;
        }
    }
    catch (std::exception &e) {
        error_log(e.what());
        return nullptr;
    }
    return nullptr;
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/29
  */
bool UserDao::setUserCardInfo(const std::vector<QTalk::StUserCard> &userInfos) {
    if (!_sqlDb) {
        return false;
    }
    //先insert 再update
    std::string sqlInsert = "insert or ignore into IM_User (`XmppId`, `NickName`, `HeaderSrc`,`mood`) values(?, ?, ?, ?);";
    qtalk::sqlite::statement queryInsert(*_sqlDb, sqlInsert);

    std::string sql = "UPDATE IM_User set `HeaderSrc` = ? , `HeadVersion` = ?, `NickName` = ? ,'mood' = ? WHERE `XmppId` = ?;";

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        _sqlDb->exec("begin immediate;");
        for (const QTalk::StUserCard& userInfo : userInfos) {
            queryInsert.bind(1, userInfo.xmppId);
            queryInsert.bind(2, userInfo.nickName);
            queryInsert.bind(3, userInfo.headerSrc);
            queryInsert.bind(4, userInfo.mood);
            queryInsert.executeStep();
            queryInsert.resetBindings();

            query.bind(1, userInfo.headerSrc);
            query.bind(2, userInfo.version);
            query.bind(3, userInfo.nickName);
            query.bind(4, userInfo.mood);
            query.bind(5, userInfo.xmppId);
            query.executeStep();
            query.resetBindings();
        }
        queryInsert.clearBindings();
        query.clearBindings();
        _sqlDb->exec("commit transaction;");
        return true;
    }
    catch (std::exception &e) {
        error_log(e.what());
        queryInsert.clearBindings();
        query.clearBindings();
        _sqlDb->exec("rollback transaction;");
        return false;
    }
}

/**
  * @函数名   getUserCardInfos
  * @功能描述 批量获取名片信息
  * @参数
  * @author   cc
  * @date     2018/10/08
  */
bool UserDao::getUserCardInfos(const std::vector<std::string>& arUserIds, std::vector<QTalk::StUserCard> &userInfos) {
    if (!_sqlDb) {
        return false;
    }

    std::ostringstream stringStream;
    stringStream
            << "SELECT `XmppId`, `Name`, `HeaderSrc`, `HeadVersion`, `SearchIndex` FROM IM_User where `xmppId` in (";

    int count = 0;

    for (const auto &id : arUserIds) {
        if (count >= arUserIds.size() - 1) {
            stringStream << QTalk::utils::format("'%s');", id.c_str());
        } else {
            stringStream << QTalk::utils::format("'%s', ", id.c_str());
        }
        count++;
    }

    std::string sql = stringStream.str();

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {

        while (query.executeNext()) {
            QTalk::StUserCard user;
            user.xmppId = query.getColumn(0).getString();
            user.userName = query.getColumn(1).getText();
            user.headerSrc = query.getColumn(2).getText();
            user.version = query.getColumn(3).getInt();
            user.searchKey = query.getColumn(4).getString();
            userInfos.push_back(user);
        }

        return true;
    }
    catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

bool UserDao::getUserCardInfos(std::map<std::string, QTalk::StUserCard> &userInfos) {
    if (!_sqlDb) {
        return false;
    }

    std::ostringstream stringStream;
    stringStream
            << "SELECT `XmppId`, `Name`, `HeaderSrc`, `HeadVersion`, `SearchIndex` FROM IM_User where `xmppId` in (";

    int count = 0;

    for (const auto &info : userInfos) {
        if (count >= userInfos.size() - 1) {
            stringStream << QTalk::utils::format("'%s');", info.first.c_str());
        } else {
            stringStream << QTalk::utils::format("'%s', ", info.first.c_str());
        }
        count++;
    }

    std::string sql = stringStream.str();

    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {

        while (query.executeNext()) {
            QTalk::StUserCard user;
            user.xmppId = query.getColumn(0).getString();
            user.userName = query.getColumn(1).getText();
            user.headerSrc = query.getColumn(2).getText();
            user.version = query.getColumn(3).getInt();
            user.searchKey = query.getColumn(4).getString();

            userInfos[user.xmppId] = user;
        }

        return true;
    }
    catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

bool UserDao::getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "SELECT `XmppId`, `Name`, `DescInfo`, `HeaderSrc`, `SearchIndex`, isVisible, NickName FROM IM_User;";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        while (query.executeNext())
        {
            std::shared_ptr<QTalk::Entity::ImUserInfo> pImUserInfo(new QTalk::Entity::ImUserInfo);
            pImUserInfo->XmppId = query.getColumn(0).getText();
            pImUserInfo->Name = query.getColumn(1).getText();
            pImUserInfo->DescInfo = query.getColumn(2).getText();
            pImUserInfo->HeaderSrc = query.getColumn(3).getText();
            pImUserInfo->SearchIndex = query.getColumn(4).getText();
            pImUserInfo->isVisible = query.getColumn(5).getInt();
            pImUserInfo->NickName = query.getColumn(6).getText();

            structure.push_back(pImUserInfo);
        }

        return true;
    }
    catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

/**
 *
 * @param strName
 * @param count
 * @return
 */
bool UserDao::getStructureCount(const std::string &strName, int &count) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "SELECT count(`XmppId`) FROM IM_User WHERE DescInfo like ? and isVisible = true";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        query.bind(1, strName + "%");
        if (query.executeNext()) {
            count = query.getColumn(0).getInt();
        }
        return true;
    }
    catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

bool UserDao::getStructureMember(const std::string &strName, std::vector<std::string> &arMember) {
    if (!_sqlDb) {
        return false;
    }

    std::string sql = "SELECT `XmppId` FROM IM_User WHERE DescInfo like ?;";
    qtalk::sqlite::statement query(*_sqlDb, sql);
    try {
        query.bind(1, strName + "%");
        while (query.executeNext()) {
            arMember.push_back(query.getColumn(0).getString());
        }
        return true;
    }
    catch (std::exception &e) {
        error_log(e.what());
        return false;
    }
}

bool UserDao::addColumn_03() {
    if (!_sqlDb) {
        return false;
    }
    std::string sql = "ALTER TABLE IM_User ADD COLUMN mood TEXT;";

    try{
        qtalk::sqlite::statement query(*_sqlDb, sql);
        return query.executeStep();;
    }catch (std::exception &e){
        error_log(e.what());
        return false;
    }

}

void UserDao::geContactsSession(std::vector<QTalk::StShareSession> &sessions)
{
    if (!_sqlDb) {
        return;
    }
    std::string sql = "select XmppId, HeaderSrc,"
                      " (case Name when '' then NickName else Name end) as N, SearchIndex "
                      "From IM_User order by SearchIndex;";

    qtalk::sqlite::statement query(*_sqlDb, sql);

    while (query.executeNext())
    {
        QTalk::StShareSession sess;
        sess.chatType = QTalk::Enum::TwoPersonChat;
        sess.xmppId = query.getColumn(0).getString();
        sess.realJid = sess.xmppId;
        sess.headUrl = query.getColumn(1).getString();
        sess.name = query.getColumn(2).getString();
        sess.searchKey = query.getColumn(3).getString();
        sessions.push_back(sess);
    }
}

void UserDao::addColumn_04()
{
    if (!_sqlDb) {
        return;
    }

    try {
        // sex column
        std::string sql = "ALTER TABLE IM_User ADD COLUMN sex INTEGER default 1;";
        qtalk::sqlite::statement query(*_sqlDb, sql);
        query.executeStep();
        // userType column
        sql = "ALTER TABLE IM_User ADD COLUMN userType TEXT";
        qtalk::sqlite::statement queryRobot(*_sqlDb, sql);
        queryRobot.executeStep();
        // isVisible column
        sql = "ALTER TABLE IM_User ADD COLUMN isVisible Boolean default true;";
        qtalk::sqlite::statement queryVisible(*_sqlDb, sql);
        queryVisible.executeStep();
    }
    catch (const std::exception& e)
    {
        error_log(e.what());
    }
}

void UserDao::modDefaultValue_05()
{
    if (!_sqlDb) {
        return;
    }

    try {
        // sex column
        std::string sql = "DROP TABLE IM_User;";
        qtalk::sqlite::statement query(*_sqlDb, sql);
        query.executeStep();

        sql = "CREATE TABLE IF NOT EXISTS `IM_User` ( "
              "`UserId`	        TEXT, "
              "`XmppId`	        TEXT, "
              "`Name`	            TEXT, "
              "`DescInfo`	        TEXT, "
              "`HeaderSrc`	    TEXT, "
              "`SearchIndex`	    TEXT, "
              "`NickName`	        TEXT, "
              "`HeadVersion`	INTEGER, "
              "`IncrementVersion`	INTEGER, "
              "`ExtendedFlag`	BLOB, "
              " mood TEXT, "
              " sex INTEGER default 1 , "
              " userType TEXT , "
              " isVisible Boolean default true , "
              "PRIMARY KEY(`XmppId`) ) ";

        qtalk::sqlite::statement queryVisible(*_sqlDb, sql);
        queryVisible.executeStep();
    }
    catch (const std::exception& e)
    {
        error_log(e.what());
    }
}
