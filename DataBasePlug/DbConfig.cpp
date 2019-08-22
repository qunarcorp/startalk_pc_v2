//
// Created by cc on 18-12-6.
//

#include "DbConfig.h"
#include "../QtUtil/Utils/Log.h"

DbConfig::DbConfig(qtalk::sqlite::database *sqlDb)
        : _pSqlDb(sqlDb) {

}

bool DbConfig::creatTable() {

    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `DB_Config` ( "
                      "`Key`	TEXT, "
                      "`SubKey`	TEXT, "
                      "`Value`	TEXT, "
                      "PRIMARY KEY(`Key`, `SubKey`) )";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    return query.executeStep();
}

/**
 * clearData
 * @return
 */
bool DbConfig::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `DB_Config`;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data DB_Config error {0}", e.what());
        return false;
    }
}

bool DbConfig::getDbVersion(int &version) {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "SELECT `Value` FROM `DB_Config` WHERE `Key` = 'DB_VER'";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    try {
        if (query.executeNext()) {
            version = atoi(query.getColumn(0).getString().c_str());
        }
    }
    catch (std::exception &e) {
        error_log("getDbVersion error!");
        throw "getDbVersion error";
    }

    return true;
}

bool DbConfig::setDbVersion(const int &ver) {

    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "INSERT OR REPLACE INTO `DB_Config` (`Key`, `SubKey`, `Value`) VALUES ('DB_VER', 'DB_VER', ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    try {
        query.bind(1, std::to_string(ver).c_str());

        return query.executeStep();
    }
    catch (std::exception &e) {
        error_log("setDbVersion error!");
        throw "setDbVersion error";
    }
}


bool DbConfig::initGroupMainVersion() {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "INSERT OR IGNORE INTO DB_Config (Key, SubKey, Value) VALUES ('Group', 'mainVersion', ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    try {
        query.bind(1, std::to_string(0).c_str());

        return query.executeStep();
    }
    catch (std::exception &e) {
        error_log("initVersions error!");
        throw "setDbVersion error";
    }
}

bool DbConfig::initVersions() {
    bool result = initGroupMainVersion();
    return result;
}
