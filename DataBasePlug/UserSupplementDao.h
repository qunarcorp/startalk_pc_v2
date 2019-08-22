//
// Created by cc on 18-11-6.
//

#ifndef QTALK_V2_USERSUPPLEMENTDAO_H
#define QTALK_V2_USERSUPPLEMENTDAO_H

#include "sqlite/database.h"
#include <memory>
#include "../entity/im_userSupplement.h"



class UserSupplementDao
{
public:
    UserSupplementDao(qtalk::sqlite::database * sqlDb = nullptr);
    ~UserSupplementDao();

public:
    bool creatTable();

    bool clearData();

    bool insertOrUpdateUserMood(const std::string& userId, const std::string& userMood, const int& version);

    bool insertOrUpdateUserPhoneNo(const std::string& userId, const std::string& phoneNo);

    bool insertOrUpdateUserSuppl(std::shared_ptr<QTalk::Entity::ImUserSupplement> imUserSup);

private:
    int checkRecordCount(const std::string &userId);

private:
    qtalk::sqlite::database * _sqlDb;
};


#endif //QTALK_V2_USERSUPPLEMENTDAO_H
