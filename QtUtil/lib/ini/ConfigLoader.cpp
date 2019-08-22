//
// Created by may on 2018/11/7.
//

#include "ConfigLoader.h"
#include "ini.h"

//
// ini parser 回调，会初始化map
namespace QTalk {
    namespace ConfigLoaderSection {
        static int handler(void *user, const char *section, const char *name,
                           const char *value) {

            auto *config = (std::map<std::string, std::string> *) user;

            auto ptr = config->end();

            config->insert(ptr, std::pair<std::string, std::string>(name, value));

            return 1;

        }
    }
}


QTalk::ConfigLoader::ConfigLoader(const char *url) {
    filename = url;
	FILE *fp = fopen(filename.c_str(), "a");
	if(fp)
		fclose(fp);
}

QTalk::ConfigLoader::~ConfigLoader() {

}

//
// load ini 文件
bool QTalk::ConfigLoader::reload() {
    if (ini_parse(filename.c_str(), QTalk::ConfigLoaderSection::handler, &config) < 0) {
        printf(filename.c_str());
        printf("load error \n");
        return false;
    }
    return true;
}

//
// 获取int值
int QTalk::ConfigLoader::getInteger(const std::string &key) {

    if (config.find(key) != config.end()) {
        return strtol(config[key].c_str(), nullptr, 10);
    }
    return 0;
}

//
// 获取string型值
std::string QTalk::ConfigLoader::getString(const std::string &key) {

    if (config.find(key) != config.end())
        return config[key];
    else
        return std::string();
}

double QTalk::ConfigLoader::getDouble(const std::string &key) {
    if (config.find(key) != config.end())
        return strtod(config[key].c_str(), nullptr);
    else
        return 0.0;
}

float QTalk::ConfigLoader::getFloat(const std::string &key) {
    if (config.find(key) != config.end())
        return strtof(config[key].c_str(), nullptr);
    else
        return 0.0;
}

void QTalk::ConfigLoader::setDouble(const std::string &key, double value) {

}

void QTalk::ConfigLoader::setFloat(const std::string &key, float value) {

}


//
// 获取bool类型值
bool QTalk::ConfigLoader::getBool(const std::string &key) {

    if (config.find(key) != config.end())
        return strtol(config[key].c_str(), nullptr, 0) != 0;
    else
        return false;
}

//
// 设置bool类型值
void QTalk::ConfigLoader::setBool(const std::string &key, bool value) {
    config[key] = std::to_string(value ? 1 : 0);
}

//
// 设置string类型值
void QTalk::ConfigLoader::setString(const std::string &key, const std::string &value) {
    config[key] = value;
}

//
// 设置int类型值
void QTalk::ConfigLoader::setInteger(const std::string &key, int value) {
    config[key] = std::to_string(value);
}

//
// 判断是否包含key
bool QTalk::ConfigLoader::hasKey(const std::string &key) {
    return (config.find(key) != config.end());
}

//
// 删除节点
void QTalk::ConfigLoader::remove(const std::string &key) {
    const std::string &theKey(key);
    if (config.find(theKey) != config.end())
        config.erase(theKey);
}

//
// ini writer,配置写入。一次性全量写入
void QTalk::ConfigLoader::saveConfig() {

    std::string data;
    auto it = config.begin();
    while (it != config.end()) {
        data += it->first + "=" + it->second + "\n";
        it++;
    }
	FILE *fp = fopen(filename.c_str(), "w");
    if(fp)
    {
        fwrite(data.c_str(), 1, data.length(), fp);
        fclose(fp);
    }
}



