//
// Created by cc on 18-12-29.
//

#include "MessageManager.h"
#include "../Message/UselessMessage.h"
#include "../EventBus/EventBus.h"

MessageManager::MessageManager()
{

}

MessageManager::~MessageManager()
{

}

/**
 * 获取Oa
 */
bool MessageManager::getOAUiData(std::vector<QTalk::StOAUIData>&oAUIData)
{
    OAUiDataMessage msg(oAUIData);
    EventBus::FireEvent(msg);
    return msg.ret;
}