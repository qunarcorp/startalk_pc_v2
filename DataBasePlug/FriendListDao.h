//
// Created by cc on 18-12-18.
//

#ifndef QTALK_V2_FRIENDLISTDAO_H
#define QTALK_V2_FRIENDLISTDAO_H


#include "DaoInterface.h"
#include <vector>
#include "../entity/im_friend_list.h"

class FriendListDao : public DaoInterface
{
public:
    explicit FriendListDao(qtalk::sqlite::database *sqlDb);
    bool creatTable() override;
    bool clearData() override;

public:
    bool bulkInsertFriends(const std::vector<QTalk::Entity::IMFriendList>& friends);
    bool insertFriend(QTalk::Entity::IMFriendList imfriend);
    bool deleteAllFriends();
    bool deleteFriendByXmppId(const std::string& xmppId);
    bool getAllFriends(std::vector<QTalk::Entity::IMFriendList>& friends);


};


#endif //QTALK_V2_FRIENDLISTDAO_H
