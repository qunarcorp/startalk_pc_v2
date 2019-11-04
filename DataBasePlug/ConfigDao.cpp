#include <iostream>
#include "ConfigDao.h"
#include "../QtUtil/Utils/Log.h"

ConfigDao::ConfigDao(qtalk::sqlite::database *sqlDb)
        : DaoInterface(sqlDb) {

}

bool ConfigDao::creatTable() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `IM_Config` ( "
                      "`ConfigKey`       TEXT,"
                      "`ConfigSubKey`    TEXT,"
                      "`ConfigValue`     TEXT,"
                      "`Version`         INTEGER default 0,"
                      "`ExtendedFlag`   BLOB, "
                      "PRIMARY KEY(`ConfigKey`,`ConfigSubKey`))";

    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("CREATE TABLE IM_Config error {0}", e.what());
        return false;
    }
}

/**
 * clearData
 * @return
 */
bool ConfigDao::clearData() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "DELETE FROM `IM_Config`;";
    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("Clear Data IM_Config error {0}", e.what());
        return false;
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool ConfigDao::insertConfig(const std::string &key, const std::string &subKey, const std::string &val) {
    if (!_pSqlDb) {
        return false;
    }

    info_log("insert config key: {0} subkey: {1} value: {2}", key, subKey, val);

    // 获取所有用户
    std::string sql = "INSERT OR REPLACE INTO IM_Config (`ConfigKey`, `ConfigSubKey`, `ConfigValue`) values (?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, key);
        query.bind(2, subKey);
        query.bind(3, val);

        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool ConfigDao::getConfig(const std::string &key, const std::string &subKey, std::string &val) {
    if (!_pSqlDb) {
        return false;
    }

    // 获取所有用户
    std::string sql = "SELECT `ConfigValue` FROM IM_Config WHERE `ConfigKey` = ? AND `ConfigSubKey` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, key);
        query.bind(2, subKey);

        if (query.executeNext()) {
            val = query.getColumn(0).getString();
            return true;
        }

        info_log("get config key: {0} subkey: {1} value: {2}", key, subKey, val);

        return false;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/25
  */
bool ConfigDao::getConfig(const std::string &key, std::map<std::string, std::string> &mapConf) {
    if (!_pSqlDb) {
        return false;
    }

    // 获取所有用户
    std::string sql = "SELECT `ConfigSubKey`, `ConfigValue` FROM IM_Config WHERE `ConfigKey` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, key);

        while (query.executeNext()) {
            std::string subkey = query.getColumn(0).getString();
            std::string val = query.getColumn(1).getString();
            mapConf[subkey] = val;
        }
        return true;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}

bool ConfigDao::getConfigVersion(int &version) {

    if (!_pSqlDb) {
        return false;
    }

    // 获取所有用户
    std::string sql = "SELECT max(`Version`) FROM IM_Config;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        if (query.executeNext()) {
            version = query.getColumn(0).getInt();
        }
        return true;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}

//
bool ConfigDao::bulkInsertConfig(const std::vector<QTalk::Entity::ImConfig> &configs) {
    if (!_pSqlDb) {
        return false;
    }

    //
    std::string sql = "INSERT OR REPLACE INTO IM_Config "
                      "(`ConfigKey`, `ConfigSubKey`, `ConfigValue`, `Version`) "
                      "values "
                      "(?, ?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);

    std::cout << "bulkInsertConfig---" << configs.size() << std::endl;

    try {
        _pSqlDb->exec("begin immediate;");
        for (QTalk::Entity::ImConfig conf: configs) {
            query.bind(1, conf.ConfigKey);
            query.bind(2, conf.ConfigSubKey);
            query.bind(3, conf.ConfigValue);
            query.bind(4, conf.Version);

            bool sqlResult = query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    }
    catch (const std::exception &e) {
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        error_log("exception : {0}", e.what());
        return false;
    }
}

//
bool ConfigDao::bulkRemoveConfig(const std::map<std::string, std::string> &mapConf) {
    if (!_pSqlDb) {
        return false;
    }

    //
    std::string sql = "delete from IM_Config where ConfigKey = ? and ConfigSubKey = ?; ";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        _pSqlDb->exec("begin immediate;");
        for (std::pair<std::string, std::string> conf : mapConf) {
            query.bind(1, conf.second);
            query.bind(2, conf.first);

            bool sqlResult = query.executeStep();
            query.resetBindings();
        }
        query.clearBindings();
        _pSqlDb->exec("commit transaction;");
        return true;
    }
    catch (const std::exception &e) {
        query.clearBindings();
        _pSqlDb->exec("rollback transaction;");
        error_log("exception : {0}", e.what());
        return false;
    }
}

bool ConfigDao::getAllConfig(std::vector<QTalk::Entity::ImConfig> &configs) {
    if (!_pSqlDb) {
        return false;
    }
    //
    std::string sql = "SELECT `ConfigKey`, `ConfigSubKey`, `ConfigValue`, `Version` FROM IM_Config; ";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        while (query.executeNext()) {
            QTalk::Entity::ImConfig config;
            config.ConfigKey = query.getColumn(0).getString();
            config.ConfigSubKey = query.getColumn(1).getString();
            config.ConfigValue = query.getColumn(2).getString();
            config.Version = query.getColumn(3).getInt();
            configs.push_back(config);
        }
        return true;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}
