#ifndef JSONCONFIG_H
#define JSONCONFIG_H

#include "qtutil_global.h"
#include <string>
#include "cjson/cJSON.h"

class QTALK_UTIL_EXPORT JsonConfig
{
public:
    JsonConfig();
//    virtual bool LoadJsonFile(const std::string &filePath);
//    virtual bool SaveJsonFile(const std::string &filePath);
//    virtual cJSON* Get(const std::string &key);
//    virtual void Set(const std::string &key, const cJSON &jsonvalue);
};

#endif // JSONCONFIG_H
