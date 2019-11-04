//
// Created by cc on 18-12-6.
//

#ifndef QTALK_V2_DBCONFIG_H
#define QTALK_V2_DBCONFIG_H

#include "DaoInterface.h"

class DbConfig : public DaoInterface
{
public:
    explicit DbConfig(qtalk::sqlite::database * sqlDb = nullptr);
    bool creatTable() override;
    bool clearData() override;

public:
    bool getDbVersion(int &version);
    bool setDbVersion(const int& ver);

    bool initVersions();

private:
    bool initGroupMainVersion();
};


#endif //QTALK_V2_DBCONFIG_H
