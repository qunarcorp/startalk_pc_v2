//
// Created by cc on 18-12-29.
//

#ifndef QTALK_V2_MESSAGEMANAGER_H
#define QTALK_V2_MESSAGEMANAGER_H

#include "../EventBus/Object.hpp"
#include "../EventBus/EventHandler.hpp"
#include "../include/CommonStrcut.h"

class MessageManager : public Object
{
public:
    MessageManager();
    ~MessageManager();

public:
    bool getOAUiData(std::vector<QTalk::StOAUIData>&oAUIData);

};


#endif //QTALK_V2_MESSAGEMANAGER_H
