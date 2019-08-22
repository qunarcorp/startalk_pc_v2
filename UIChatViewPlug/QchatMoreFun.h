//
// Created by lihaibin on 2019-07-17.
//

#ifndef QTALK_V2_PRODUCTSUGGEST_H
#define QTALK_V2_PRODUCTSUGGEST_H


#include "../WebService/WebService.h"
#include "../Platform/Platform.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../entity/UID.h"

#define PRODUCT_SUGGEST_URL ""

class QchatMoreFun{
public:
    QchatMoreFun(QTalk::Entity::UID _uid,QTalk::Enum::ChatType _chatType);

    ~QchatMoreFun();

public:
    void showSuggestProducts();

private:
    QTalk::Entity::UID uid;
    QTalk::Enum::ChatType chatType;
};

#endif //QTALK_V2_PRODUCTSUGGEST_H
