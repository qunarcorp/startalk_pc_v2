//
// Created by lihaibin on 2019-02-27.
//
#include <iostream>
#include "CacheDataDao.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"

#define HOTLINE_KEY "hotline"
#define HOTLINE_TYPE 8

#define USER_ID "USER_ID"
#define USER_ID_TYPE 6

#define GROUP_READMARK "GROUP_READMARK"
#define GROUP_READMARK_TIME 7

#define GROUP_READMARK_LOGIN_BEFORE "GROUP_READMARK_LOGIN_BEFORE"
#define GROUP_READMARK_LOGIN_BEFORE_TIME 12


CacheDataDao::CacheDataDao(qtalk::sqlite::database *sqlDb)
        : _pSqlDb(sqlDb) {

}

bool CacheDataDao::creatTable() {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS `IM_Cache_Data` ( "
                      "`key`       TEXT,"
                      "`type`    INTEGER,"
                      "`value`     TEXT,"
                      "`valueInt`         INTEGER default 0,"
                      "PRIMARY KEY(`key`,`type`))";

    try {
        qtalk::sqlite::statement query(*_pSqlDb, sql);
        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("CREATE TABLE IM_Cache_Data error {0}", e.what());
        return false;
    }
}

bool CacheDataDao::insertUserId(std::string value) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "INSERT OR REPLACE INTO IM_Cache_Data (`key`, `type`, `value`) values (?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, USER_ID);
        query.bind(2, USER_ID_TYPE);
        query.bind(3, value);

        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("insertHotLine exception : {0}", e.what());
        return false;
    }
}

bool CacheDataDao::insertHotLine(std::string value) {
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "INSERT OR REPLACE INTO IM_Cache_Data (`key`, `type`, `value`) values (?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, HOTLINE_KEY);
        query.bind(2, HOTLINE_TYPE);
        query.bind(3, value);

        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("insertHotLine exception : {0}", e.what());
        return false;
    }
}

bool CacheDataDao::isHotLine(const std::string xmppid) {
    if (!_pSqlDb) {
        return false;
    }
    std::string sql = "SELECT `value` FROM IM_Cache_Data WHERE `key` = ? AND `type` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, HOTLINE_KEY);
        query.bind(2, HOTLINE_TYPE);

        if (query.executeNext()) {
            std::string value = query.getColumn(0).getString();
            cJSON *jsonObj = cJSON_Parse(value.c_str());
            if (jsonObj == nullptr) {
                error_log("json paring error"); return false;
            }
            int ret = cJSON_GetObjectItem(jsonObj, "ret")->valueint;
            if(ret){
                cJSON *data = cJSON_GetObjectItem(jsonObj, "data");
                cJSON *allHotLines = cJSON_GetObjectItem(data,"allhotlines");

                std::string result = QTalk::JSON::cJSON_SafeGetStringValue(allHotLines,xmppid.c_str());
                cJSON_Delete(jsonObj);
                return !result.empty();
            }
            return true;
        }
        return false;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}

bool CacheDataDao::isHotlineMerchant(const std::string xmppid) {
    std::string sql = "SELECT `value` FROM IM_Cache_Data WHERE `key` = ? AND `type` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, HOTLINE_KEY);
        query.bind(2, HOTLINE_TYPE);

        if (query.executeNext()) {
            std::string value = query.getColumn(0).getString();
            cJSON *jsonObj = cJSON_Parse(value.c_str());
            if (jsonObj == nullptr) {
                error_log("json paring error"); return false;
            }
            int ret = cJSON_GetObjectItem(jsonObj, "ret")->valueint;
            if(ret){
                cJSON *data = cJSON_GetObjectItem(jsonObj, "data");
                cJSON* array = cJSON_GetObjectItem(data,"myhotlines");

                if(array == nullptr){
                    return false;
                }
                int size = cJSON_GetArraySize(array);
                if(size == 0){
                    return false;
                }
                for(int i = 0;i<size;i++){
                    std::string result = cJSON_GetArrayItem(array,i)->valuestring;
                    if(xmppid.compare(result)){

                        cJSON_Delete(jsonObj);
                        return true;
                    }
                }

                return false;
            }
        }
        return false;
    }
    catch (const std::exception &e) {
        error_log("exception : {0}", e.what());
        return false;
    }
}

std::string CacheDataDao::getGroupReadMarkTime(){
    std::string sql = "SELECT `value` FROM IM_Cache_Data WHERE `key` = ? AND `type` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, GROUP_READMARK);
        query.bind(2, GROUP_READMARK_TIME);

        if (query.executeNext()) {
            std::string value = query.getColumn(0).getString();
            return value;
        }
        return "0";
    }
    catch (const std::exception &e) {
        error_log("getGroupReadMarkTime exception : {0}", e.what());
        return "0";
    }
}
bool CacheDataDao::updateGroupReadMarkTime(const std::string time){
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "INSERT OR REPLACE INTO IM_Cache_Data (`key`, `type`, `value`) values (?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, GROUP_READMARK);
        query.bind(2, GROUP_READMARK_TIME);
        query.bind(3, time);

        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("updateGroupReadMarkTime exception : {0}", e.what());
        return false;
    }
}

std::string CacheDataDao::getLoginBeforeGroupReadMarkTime(){
    std::string sql = "SELECT `value` FROM IM_Cache_Data WHERE `key` = ? AND `type` = ?;";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, GROUP_READMARK_LOGIN_BEFORE);
        query.bind(2, GROUP_READMARK_LOGIN_BEFORE_TIME);

        if (query.executeNext()) {
            std::string value = query.getColumn(0).getString();
            return value;
        }
        return "0";
    }
    catch (const std::exception &e) {
        error_log("getGroupReadMarkFailTime exception : {0}", e.what());
        return "0";
    }
}
bool CacheDataDao::saveLoginBeforeGroupReadMarkTime(const std::string time){
    if (!_pSqlDb) {
        return false;
    }

    std::string sql = "INSERT OR REPLACE INTO IM_Cache_Data (`key`, `type`, `value`) values (?, ?, ?);";
    qtalk::sqlite::statement query(*_pSqlDb, sql);
    try {
        query.bind(1, GROUP_READMARK_LOGIN_BEFORE);
        query.bind(2, GROUP_READMARK_LOGIN_BEFORE_TIME);
        query.bind(3, time);

        return query.executeStep();
    }
    catch (const std::exception &e) {
        error_log("saveGroupReadMarkFailTime exception : {0}", e.what());
        return false;
    }
}
