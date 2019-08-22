#ifndef USERDAO_H
#define USERDAO_H

#include "sqlite/database.h"
#include <vector>
#include <memory>
#include "../include/CommonStrcut.h"

namespace QTalk {
    namespace Entity {
        struct ImUserInfo;
    }
}

class UserDao {
public:
    explicit UserDao(qtalk::sqlite::database *sqlDb = nullptr);

    bool creatTable();

    bool clearData();

public:
    bool getUserVersion(int &version);

    bool insertUserInfo(const QTalk::Entity::ImUserInfo &userInfo);

    bool bulkInsertUserInfo(const std::vector<QTalk::Entity::ImUserInfo> &userInfos);

    bool bulkDeleteUserInfo(const std::vector<std::string> &userIds);

    std::shared_ptr<QTalk::Entity::ImUserInfo> getUserInfoByXmppId(const std::string &xmppid);

    bool setUserCardInfo(const std::vector<QTalk::StUserCard> &userInfos);

    //
    bool getUserCardInfos(const std::vector<std::string>& arUserIds, std::vector<QTalk::StUserCard> &userInfos);
    bool getUserCardInfos(std::map<std::string, QTalk::StUserCard> &userInfos);

    //
    bool getStructure(std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo>> &structure);

    //
    bool getStructureCount(const std::string &strName, int &count);

    //
    bool getStructureMember(const std::string &strName, std::vector<std::string> &arMember);


    void geContactsSession(std::vector<QTalk::StShareSession> &sessions);

public:
    bool addColumn_03();
    void addColumn_04();
    void modDefaultValue_05();

private:
    qtalk::sqlite::database *_sqlDb;
};

#endif // USERDAO_H
