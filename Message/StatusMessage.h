#ifndef _DISCONNECTTOSERVER_H_
#define _DISCONNECTTOSERVER_H_

#include <utility>

#include "../EventBus/Event.hpp"
#include "../include/CommonStrcut.h"

// tcp断链
class DisconnectToServer : public Event { };

// 验证失败消息
class AuthFailed : public Event
{
public:
	std::string  message;
};

// 触发重连消息
class RetryConnectToServerEvt : public Event {};

// 重连结果消息
//class RetryConnectRet : public Event{ };

// 登录出错提示信息
class LoginErrMessage : public Event
{
public:
    std::string errorMessage;
};

class GetHistoryError : public Event{};

//
class UserOnlineState : public Event
{
public:
    explicit UserOnlineState(const QInt64& login_t, const QInt64& logout_t, std::string ip)
        : login_t(login_t), logout_t(logout_t), ip(std::move(ip)) {}

public:
    QInt64 login_t;
    QInt64 logout_t;
    std::string ip;
};

class OperatorStatistics : public Event
{
public:
    explicit OperatorStatistics(std::string ip, std::vector<QTalk::StActLog> operators)
        :ip(std::move(ip)), operators(std::move(operators)) {}

public:
    const std::string ip;
    const std::vector<QTalk::StActLog> operators;

};
#endif