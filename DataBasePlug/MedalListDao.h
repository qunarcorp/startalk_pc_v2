//
// Created by QITMAC000260 on 2019/10/15.
//

#ifndef QTALK_V2_MEDALLISTDAO_H
#define QTALK_V2_MEDALLISTDAO_H

#include "DaoInterface.h"
#include <vector>
#include "../entity/im_medal_list.h"

/**
* @description: MedalListDao
* @author: cc
* @create: 2019-10-15 14:11
**/
class MedalListDao : public DaoInterface{
public:
    explicit MedalListDao(qtalk::sqlite::database *sqlDb);
    bool creatTable() override;
    bool clearData() override;

public:
    void insertMedalList(const std::vector<QTalk::Entity::ImMedalList>& medals);
    void getMedalList(std::vector<QTalk::Entity::ImMedalList>& medals);
};


#endif //QTALK_V2_MEDALLISTDAO_H
