//
// Created by may on 2018/7/23.
//

#include "ClientConfig.h"
#include <vector>
#include <string>
#include <map>

namespace QTalk {
    class BaseClientConfig : public ClientConfig {
    public:

        BaseClientConfig(const char *name) {
            keyName = name;
        }

        void addConfigJson(cJSON *pJSON) override {
            if (pJSON != NULL && cJSON_IsArray(pJSON)) {
                int size = cJSON_GetArraySize(pJSON);
                for (int i = 0; i < size; ++i) {
                    cJSON *subItem = cJSON_GetArrayItem(pJSON, i);
                    cJSON *key = cJSON_GetObjectItem(subItem, "subkey");
                    cJSON *value = cJSON_GetObjectItem(subItem, "configinfo");
                    cJSON *jsDel = cJSON_GetObjectItem(subItem, "isdel");
                    bool needDelete = jsDel->valueint != 0;

                    std::pair<const char *, const char *> addItem(cJSON_GetStringValue(key),
                                                                  cJSON_GetStringValue(value));
                    if (needDelete) {
                        deleteItems.push_back(addItem);
                    } else {
                        addItems.push_back(addItem);
                    }
                }
            }

            pAddItems = addItems.begin();
            pDeleteItems = deleteItems.begin();
        };

        std::pair<const char *, const char *> *nextAdd(void) override {
            if (pAddItems != addItems.end()) {
                std::pair<const char *, const char *> *result = &(*pAddItems);
                pAddItems++;
                return result;

            } else {
                return NULL;
            }

        };

        std::pair<const char *, const char *> *nextDelete(void) override {

            if (pDeleteItems != deleteItems.end()) {
                std::pair<const char *, const char *> *result = &(*pDeleteItems); //
                pDeleteItems++;
                return result;

            } else {
                return NULL;
            }
        };

        const char *getKeyName(void) override {
            return keyName.c_str();
        }

    protected:

        std::string keyName;
        std::vector<std::pair<const char *, const char *>> addItems;
        std::vector<std::pair<const char *, const char *>> deleteItems;

        std::vector<std::pair<const char *, const char *>>::iterator pAddItems;
        std::vector<std::pair<const char *, const char *>>::iterator pDeleteItems;
    };

//    class MarkupNameConfig : public BaseClientConfig {
//    public:
//        virtual void addConfigJson(cJSON *pJSON) {
//
//        }
//
//        virtual std::pair<const char *, const char *> next(void) {
//
//        }
//    };
//
//    class CollectionCacheKeyConfig : public BaseClientConfig {
//    public:
//        virtual void addConfigJson(cJSON *pJSON) {
//
//        }
//
//        virtual std::pair<const char *, const char *> next(void) {
//
//        }
//    };
//
//    class StickJidDicConfig : public BaseClientConfig {
//    public:
//        virtual void addConfigJson(cJSON *pJSON) {
//
//        }
//
//        virtual std::pair<const char *, const char *> next(void) {

//
//        }
//    };

    ClientConfig *ClientConfigFactory::getConfig(const char *name) {
        return new BaseClientConfig(name);
    }

    void ClientConfig::parseHtmlResult(cJSON *json, std::vector<std::pair<const char *, const char *>> *addItems,
                                       std::vector<std::pair<const char *, const char *>> *removeItems) {
        cJSON *retData = cJSON_GetObjectItem(json, "data");
        if (retData != nullptr) {

            cJSON *versionJson = cJSON_GetObjectItem(retData, "version");
            long version = versionJson->valueint;
            if (version > 0) {
                std::map<const char *, QTalk::ClientConfig *> maps;

                cJSON *clientConfigInfoArray = cJSON_GetObjectItem(retData, "clientConfigInfos");
                if (clientConfigInfoArray != nullptr && cJSON_IsArray(clientConfigInfoArray)) {
                    int arraySize = cJSON_GetArraySize(clientConfigInfoArray);
                    for (int i = 0; i < arraySize; ++i) {
                        cJSON *subItem = cJSON_GetArrayItem(clientConfigInfoArray, i);

                        cJSON *keyNameJson = cJSON_GetObjectItem(subItem, "key");
                        const char *keyName = NULL;
                        if (keyNameJson != nullptr && cJSON_IsString(keyNameJson)) {
                            keyName = cJSON_GetStringValue(keyNameJson);
                        }

                        auto it = maps.find(keyName);
                        QTalk::ClientConfig *config;
                        if (it != maps.end()) {
                            config = it->second;
                        } else {
                            config = QTalk::ClientConfigFactory::getConfig(keyName);
                            maps.insert(std::pair<const char *, QTalk::ClientConfig *>(keyName, config));

                        }
                        config->addConfigJson(cJSON_GetObjectItem(subItem, "infos"));
                    }
                }

                auto pMap = maps.begin();

                while (pMap != maps.end()) {

                    std::pair<const char *, const char *> *pKeyValue;
                    if (addItems != nullptr) {

                        while ((pKeyValue = pMap->second->nextAdd()) != nullptr) {
                            addItems->push_back(*pKeyValue);
                        }
                    }

                    if (removeItems != nullptr) {
                        while ((pKeyValue = pMap->second->nextDelete()) != nullptr) {
                            removeItems->push_back(*pKeyValue);
                        }
                    }
                    delete pMap->second;
                    pMap++;
                }
            }
        }
    }
};

