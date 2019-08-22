//
// Created by lihaibin on 2019-02-27.
//

#ifndef QTALK_V2_CACHEDATADAO_H
#define QTALK_V2_CACHEDATADAO_H

#include "sqlite/database.h"
#include <vector>

class CacheDataDao
{
public:
    CacheDataDao(qtalk::sqlite::database *sqlDb);
    bool creatTable();

public:
    bool insertUserId(std::string value);
    bool insertHotLine(std::string value);
    bool isHotLine(const std::string xmppid);
    bool isHotlineMerchant(const std::string xmppid);

public:
    std::string getGroupReadMarkTime();
    bool updateGroupReadMarkTime(const std::string time);

    std::string getLoginBeforeGroupReadMarkTime();
    bool saveLoginBeforeGroupReadMarkTime(const std::string time);

private:
    qtalk::sqlite::database *_pSqlDb;
};

#endif //QTALK_V2_CACHEDATADAO_H
