//
// Created by cc on 18-12-18.
//

#ifndef QTALK_V2_FRIENDLISTDAO_H
#define QTALK_V2_FRIENDLISTDAO_H


#include "sqlite/database.h"
#include <vector>
#include "../entity/im_friend_list.h"

class FriendListDao
{
public:
    FriendListDao(qtalk::sqlite::database *sqlDb);
    bool creatTable();
    bool clearData();
public:
    bool bulkInsertFriends(const std::vector<QTalk::Entity::IMFriendList>& friends);
    bool insertFriend(QTalk::Entity::IMFriendList imfriend);
    bool deleteAllFriends();
    bool deleteFriendByXmppId(const std::string& xmppId);
    bool getAllFriends(std::vector<QTalk::Entity::IMFriendList>& friends);

private:
    qtalk::sqlite::database *_pSqlDb;
};


#endif //QTALK_V2_FRIENDLISTDAO_H
