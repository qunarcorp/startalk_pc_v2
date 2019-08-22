#include "MessageManager.h"
#include "../Message/UserMessage.h"
#include "../EventBus/EventBus.h"
#include "../Message/ChatMessage.h"

/**
  * 发送消息类
  * @author   cc
  * @date     2018/09/29
  */
PictureMsgManager::PictureMsgManager()
{

}


PictureMsgManager::~PictureMsgManager()
{

}

std::string PictureMsgManager::getSourceImage(const std::string& netPath)
{
    SourceNetImage e;
    e.netFilePath = netPath;
    EventBus::FireEvent(e);
    return e.localFilePath;
}

/**
 *
 * @param msgs
 */
void PictureMsgManager::getBeforeImgMessages(const std::string& messageId, std::vector<std::pair<std::string, std::string>> &msgs)
{
    ImageMessageEvt e(false, messageId, msgs);
    EventBus::FireEvent(e);
}

void PictureMsgManager::getNextImgMessages(const std::string& messageId, std::vector<std::pair<std::string, std::string>> &msgs)
{
    ImageMessageEvt e(true, messageId, msgs);
    EventBus::FireEvent(e);
}

/**
  * 接收消息类
  * @author   cc
  * @date     2018/09/29
  */
PictureMsgListener::PictureMsgListener(PictureBrowser *pMainPanel)
        :_pMainPanel(pMainPanel)
{

}

PictureMsgListener::~PictureMsgListener()
{

}