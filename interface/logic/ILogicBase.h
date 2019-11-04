//
// Created by QITMAC000260 on 2019-08-13.
//

#ifndef QTALK_V2_ILOGICBASE_H
#define QTALK_V2_ILOGICBASE_H

#include "ILogicObject.h"
#include <string>
#include <functional>
#include <map>
#include <vector>
#include "../include/CommonDefine.h"

class ILogicBase : public ILogicObject {
public:

public:
    // 连接服务器
    virtual bool tryConnectToServer(const std::string &userName, const std::string &password, const std::string &host, int port,bool isNewLogin) = 0;
    // chat rsa
    virtual std::string chatRsaEncrypt(const std::string &value) = 0;
    //normal rsa
    virtual std::string normalRsaEncrypt(const std::string &value) = 0;
    // 销毁群处理
    virtual void destroyGroup(const std::string &groupId) = 0;
    // 退出群
    virtual void quitGroup(const std::string &groupId) = 0;
    // 移除群成员
    virtual void removeGroupMember(const std::string& groupId,
                                   const std::string& nickName,
                                   const std::string& memberJid) = 0;
    // 设置群管理/取消群管理
    virtual void setGroupAdmin(const std::string& groupId, const std::string &nickName,
                               const std::string &memberJid, bool isAdmin) = 0;
    // 获取好友列表
    virtual void getFriendList() = 0;
    // 获取群成员
    virtual void getGroupMemberById(const std::string &groupId) = 0;
    //邀请用户入群
    virtual void inviteGroupMembers(std::vector<std::string> &users, const std::string &groupId) = 0 ;
    //创建群组
    virtual void createGroup(const std::string &groupId) = 0;
    //
    virtual void sendHeartbeat() = 0;

public:
    virtual bool sendReportMessage(const std::string& msg, std::string& error) = 0;
    virtual bool sendReportMail(const std::string& strSub,
                                const std::string& body,
                                bool isHtml, std::string& error) = 0;
};

#endif //QTALK_V2_ILOGICBASE_H
