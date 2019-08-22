//
// Created by cc on 18-12-6.
//

#ifndef QTALK_V2_DBCONFIG_H
#define QTALK_V2_DBCONFIG_H

#include "sqlite/database.h"

class DbConfig
{
public:
    DbConfig(qtalk::sqlite::database * sqlDb = nullptr);

public:
    bool creatTable();
    bool clearData();

    bool getDbVersion(int &version);
    bool setDbVersion(const int& ver);

    bool initVersions();

private:
    bool initGroupMainVersion();

private:
    qtalk::sqlite::database * _pSqlDb;
};


#endif //QTALK_V2_DBCONFIG_H
