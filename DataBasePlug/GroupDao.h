#ifndef GROUPDAO_H
#define GROUPDAO_H

#include <vector>
#include "../include/CommonDefine.h"
#include "DaoInterface.h"
#include <memory>

namespace QTalk {
    namespace Entity {
        struct ImGroupInfo;
    }
}

class GroupDao : public DaoInterface {
public:
    explicit GroupDao(qtalk::sqlite::database *sqlDb = nullptr);
    bool creatTable() override;
    bool clearData() override;

public:
    bool insertGroupInfo(const QTalk::Entity::ImGroupInfo &userInfo);

    bool bulkInsertGroupInfo(const std::vector<QTalk::Entity::ImGroupInfo> &groups);

    bool bulkDeleteGroup(const std::vector<std::string> &groupIds);

    bool getGroupLastUpdateTime(long long &lastUpdateTime);

    bool setGroupMainVersion(long long version);

    bool updateGroupCard(const std::vector<QTalk::Entity::ImGroupInfo> &groups);

    std::shared_ptr<QTalk::Entity::ImGroupInfo> getGroupInfoByXmppId(const std::string &xmppid);
    //
    bool getGroupTopic(const std::string &groupId, std::string &groupTopic);
    //
    bool getAllGroup(std::vector<QTalk::Entity::ImGroupInfo> &groups);

    //
    bool getGroupCardById(std::shared_ptr<QTalk::Entity::ImGroupInfo> &group);

    bool deleteAllGroup();

    void getGroupCardMaxVersion(long long &version);
};

#endif // GROUPDAO_H
