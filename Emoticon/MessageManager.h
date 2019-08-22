#include "../EventBus/Object.hpp"
#ifndef _MESSAGEMANAGER_H_
#define _MESSAGEMANAGER_H_

#include <string>
#include <vector>
#include "../include/CommonStrcut.h"
#include "../Message/ChatMessage.h"
#include "../EventBus/EventHandler.hpp"
#include "../EventBus/HandlerRegistration.hpp"
#include "../Message/UserMessage.h"

class EmoMsgManager : public Object
{
public:
	EmoMsgManager();
	virtual ~EmoMsgManager();

public:
	std::string getEmoRealFilePath(const std::string& pid, const std::string& sid, const std::string& fileName);
	ArStNetEmoticon getNetEmoticon();
	void downloadNetEmoticon(const std::string& uri, const std::string& localPath, const std::string& key);
	void downloadCollections(const std::vector<std::string>& downloads);

	void updateUserSetting(bool isSetting, const std::string &key, const std::string &subKey,
                           const std::string &val);
};

class EmoMsgListener : public EventHandler<FileProcessMessage>, public EventHandler<UpdateUserConfigMsg>
        , public EventHandler<IncrementConfig>
{
public:
    EmoMsgListener();
    ~EmoMsgListener() override;

public:
	void onEvent(FileProcessMessage& e) override;
	void onEvent(UpdateUserConfigMsg& e) override;
	void onEvent(IncrementConfig& e) override;
};

#endif//_MESSAGEMANAGER_H_
