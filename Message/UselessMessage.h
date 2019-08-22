//
// Created by cc on 18-12-29.
//

#ifndef QTALK_V2_USELESSMESSAGE_H
#define QTALK_V2_USELESSMESSAGE_H

#include "../EventBus/Object.hpp"
#include "../EventBus/Event.hpp"
#include "../include/CommonStrcut.h"

class OAUiDataMessage : public Event
{
public:
    OAUiDataMessage(std::vector<QTalk::StOAUIData>&oAUIData)
        :stOAUIData(oAUIData){};
    ~OAUiDataMessage() {};

public:
    bool ret;
    std::vector<QTalk::StOAUIData>&stOAUIData;
};

#endif //QTALK_V2_USELESSMESSAGE_H