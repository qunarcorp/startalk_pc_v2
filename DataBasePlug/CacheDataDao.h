//
// Created by lihaibin on 2019-02-27.
//

#ifndef QTALK_V2_CACHEDATADAO_H
#define QTALK_V2_CACHEDATADAO_H

#include "DaoInterface.h"
#include <vector>

class CacheDataDao : public DaoInterface
{
public:
    explicit CacheDataDao(qtalk::sqlite::database *sqlDb);
    bool creatTable() override;
    bool clearData() override;

public:
    bool insertUserId(const std::string& value);
    bool insertHotLine(std::string value);
    void getHotLines(std::string &hotLines);
    bool isHotlineMerchant(const std::string xmppid);

public:
    std::string getGroupReadMarkTime();
    bool updateGroupReadMarkTime(const std::string time);

    std::string getLoginBeforeGroupReadMarkTime();
    bool saveLoginBeforeGroupReadMarkTime(const std::string& time);

public:
    //
    void clear_data_01();

};

#endif //QTALK_V2_CACHEDATADAO_H
