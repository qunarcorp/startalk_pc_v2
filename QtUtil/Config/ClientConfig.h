//
// Created by may on 2018/7/23.
//

#ifndef QTALK_CLIENTCONFIG_H
#define QTALK_CLIENTCONFIG_H

#include <utility>
#include <vector>
#include "../lib/cjson/cJSON.h"

namespace QTalk {

    class ClientConfig {

    public:
        virtual void addConfigJson(cJSON *pJSON) {};

        virtual const char *getKeyName() { return  NULL;}

        virtual std::pair<const char *, const char *> *nextAdd(void) { return  NULL;}

        virtual std::pair<const char *, const char *> *nextDelete(void) { return  NULL;}

        virtual ~ClientConfig() {}

        static void parseHtmlResult(cJSON *json, std::vector<std::pair<const char *, const char *>> *addItems,
                                    std::vector<std::pair<const char *, const char *>> *removeItems);
    };

    class ClientConfigFactory {

    public:
        static ClientConfig *getConfig(const char *name);
    };

};


#endif //QTALK_CLIENTCONFIG_H
