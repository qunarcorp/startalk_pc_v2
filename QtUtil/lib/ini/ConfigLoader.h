//
// Created by may on 2018/11/7.
//

#ifndef QTALK_V2_CONFIGLOADER_H
#define QTALK_V2_CONFIGLOADER_H


#include <string>
#include <map>
#include "../../qtutil_global.h"

namespace QTalk {
    class QTALK_UTIL_EXPORT ConfigLoader {
    public:
        explicit ConfigLoader(const char *url);
        ~ConfigLoader();

    public:
        bool reload();

        void saveConfig();

    public:
        int getInteger(const std::string &key);

        std::string getString(const std::string &key);

        bool getBool(const std::string &key);

        double getDouble(const std::string &key);

        float getFloat(const std::string &key);

    public:
        //
        bool hasKey(const std::string &key);

        void remove(const std::string &key);

        void setInteger(const std::string &key, int value);

        void setFloat(const std::string &key, float value);

        void setDouble(const std::string &key, double value);

        void setBool(const std::string &key, bool value);

        void setString(const std::string &key, const std::string &value);

    private:
        std::string filename;
        std::map<std::string, std::string> config;
    };
}


#endif //QTALK_V2_CONFIGLOADER_H
