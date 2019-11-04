//
// Created by cc on 18-11-6.
//

#ifndef QTALK_V2_USERSUPPLEMENTDAO_H
#define QTALK_V2_USERSUPPLEMENTDAO_H

#include "DaoInterface.h"
#include <memory>
#include "../entity/im_userSupplement.h"

class UserSupplementDao : public DaoInterface
{
public:
    explicit UserSupplementDao(qtalk::sqlite::database * sqlDb = nullptr);
    bool creatTable() override;
    bool clearData() override;

public:
    bool insertOrUpdateUserMood(const std::string& userId, const std::string& userMood, const int& version);

    bool insertOrUpdateUserPhoneNo(const std::string& userId, const std::string& phoneNo);

    bool insertOrUpdateUserSuppl(std::shared_ptr<QTalk::Entity::ImUserSupplement> imUserSup);

private:
    int checkRecordCount(const std::string &userId);
};


#endif //QTALK_V2_USERSUPPLEMENTDAO_H
