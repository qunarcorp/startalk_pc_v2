#include <utility>

#include <utility>

#ifndef _UIENTITY_H_
#define _UIENTITY_H_

#include <QString>
#include "../include/CommonDefine.h"

/**
 * 会话窗口传向聊天窗口数据
 */
struct StSessionInfo
{
	QUInt8  chatType{};
	QString userId;
	QString realJid;
	QString userName;
	QString headPhoto;

	StSessionInfo() = default;

    StSessionInfo(const QUInt8& type, QString id, QString name)
		: chatType(type), userId(std::move(id)), userName(std::move(name))
	{

	}
};

#endif//
