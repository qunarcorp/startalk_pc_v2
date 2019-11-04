//
// Created by QITMAC000260 on 2019/10/15.
//

#ifndef QTALK_V2_USERMEDALDAO_H
#define QTALK_V2_USERMEDALDAO_H

#include "DaoInterface.h"
#include <vector>
#include <set>
#include "../entity/im_user_status_medal.h"
#include "../include/CommonStrcut.h"

/**
* @description: UserMedalDao
* @author: cc
* @create: 2019-10-15 14:25
**/
class UserMedalDao : public  DaoInterface{
public:
    explicit UserMedalDao(qtalk::sqlite::database *sqlDb);
    bool creatTable() override ;
    bool clearData() override ;

public:
    void insertMedals(const std::vector<QTalk::Entity::ImUserStatusMedal>& medals);
    void getUserMedal(const std::string& xmppId, std::set<QTalk::StUserMedal>& stMedal);
    void getMedalUsers(int medalId, std::vector<QTalk::StMedalUser>& metalUsers);
    void modifyUserMedalStatus(const std::string& userId, int medalId, int status);
};


#endif //QTALK_V2_USERMEDALDAO_H
