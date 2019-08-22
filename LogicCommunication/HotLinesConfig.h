//
// Created by lihaibin on 2019-02-26.
//

#ifndef QTALK_V2_HOTLINESCONFIG_H
#define QTALK_V2_HOTLINESCONFIG_H

#include <string>
#include <map>
#include "../include/CommonDefine.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../entity/UID.h"

class Communication;
class HotLinesConfig
{
public:
    HotLinesConfig(Communication* comm);
    ~HotLinesConfig();

public:
    //获取客服信息
    void getVirtualUserRole();

public:
    //设置客服坐席服务模式
    void getServiceSeat();
    void setServiceSeat(int sid,int state);

public:
    //会话转移
    void getTransferSeatsList(const QTalk::Entity::UID uid);
    void transferCsr(const QTalk::Entity::UID uid,const std::string newCsrName,const std::string reason);

public:
    //客服挂断
    void serverCloseSession(const std::string username, const std::string seatname, const std::string virtualname);
    //发送产品卡片
    void sendProduct(const std::string username, const std::string virtualname, const std::string product,const std::string type);

public:
    //快捷回复
    void updateQuickReply();

public:
    void sendWechat(const QTalk::Entity::UID uid);

private:
    Communication* _pComm;
};
#endif //QTALK_V2_HOTLINESCONFIG_H
