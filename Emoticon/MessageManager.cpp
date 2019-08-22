#include "MessageManager.h"
#include "../Message/ChatMessage.h"
#include "../EventBus/EventBus.h"
#include "EmoticonMainWgt.h"

EmoMsgManager::EmoMsgManager()
{
}


EmoMsgManager::~EmoMsgManager()
{
}

/**
  * @函数名   getEmoRealFilePath
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/19
  */
std::string EmoMsgManager::getEmoRealFilePath(const std::string& pid, const std::string& sid, const std::string& fileName)
{
	GetEmoticonFileMessage e( pid, sid, fileName);
	EventBus::FireEvent(e, false);
	return e.realFilePath;
}

/**
  * @函数名   getNetEmoticon
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/21
  */
ArStNetEmoticon EmoMsgManager::getNetEmoticon()
{
	GetNetEmoticon e;
	EventBus::FireEvent(e, false);
	return e.arEmoInfo;
}

/**
  * @函数名   downloadNetEmoticon
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void EmoMsgManager::downloadNetEmoticon(const std::string& uri, const std::string& localPath, const std::string& key)
{
	DownloadFileWithProcess e;
	e.strUri = uri;
	e.strLocalPath = localPath;
	e.processKey = key;
	EventBus::FireEvent(e);
}

/**
 * 下载收藏表情
 * @param arDownloads
 */
void EmoMsgManager::downloadCollections(const std::vector<std::string>& dowloads)
{
	DownloadCollection e(dowloads);
	EventBus::FireEvent(e);
}

void EmoMsgManager::updateUserSetting(bool isSetting, const std::string &key, const std::string &subKey,
                       const std::string &val)
{
    UserSettingMsg e;
    e.operatorType = isSetting ? UserSettingMsg::EM_OPERATOR_CANCEL : UserSettingMsg::EM_OPERATOR_SET;
    e.key = key;
    e.subKey = subKey;
    e.value = val;
    EventBus::FireEvent(e);
}

/*************************************************************************/
EmoMsgListener::EmoMsgListener()
{
	EventBus::AddHandler<FileProcessMessage>(*this);
	EventBus::AddHandler<UpdateUserConfigMsg>(*this);
	EventBus::AddHandler<IncrementConfig>(*this);
}

EmoMsgListener::~EmoMsgListener()
{

}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/22
  */
void EmoMsgListener::onEvent(FileProcessMessage& e)
{
	if (e.getCanceled()) return;

	EmoticonMainWgt::getInstance()->updateDownloadNetEmotiocnProcess(e.key, e.downloadTotal, e.downloadNow);
}

/**
 *
 * @param e
 */
void EmoMsgListener::onEvent(UpdateUserConfigMsg &e)
{
	if (e.getCanceled()) return;

	EmoticonMainWgt::getInstance()->updateCollectionConfig(e.arConfigs);
}

void EmoMsgListener::onEvent(IncrementConfig &e)
{
    if (e.getCanceled()) return;

    EmoticonMainWgt::getInstance()->updateCollectionConfig(e.deleteData, e.arImConfig);
}
