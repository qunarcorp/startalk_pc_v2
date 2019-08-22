#ifndef _CONFIGDAO_H_
#define _CONFIGDAO_H_

#include "sqlite/database.h"
#include <vector>
#include "../entity/im_config.h"

class ConfigDao
{
public:
	ConfigDao(qtalk::sqlite::database *sqlDb);
	bool creatTable();
	bool clearData();
public:
    bool getConfigVersion(int& version);
    bool bulkInsertConfig(const std::vector<QTalk::Entity::ImConfig>& config);
    bool bulkRemoveConfig(const std::map<std::string, std::string>& mapConf);
	bool insertConfig(const std::string& key, const std::string& subKey, const std::string& val);
	bool getConfig(const std::string& key, const std::string& subKey, std::string& val);
	bool getConfig(const std::string& key, std::map<std::string, std::string>& mapConf);
    bool getAllConfig(std::vector<QTalk::Entity::ImConfig>& configs);

private:
	qtalk::sqlite::database *_pSqlDb;
};

#endif//_CONFIGDAO_H_