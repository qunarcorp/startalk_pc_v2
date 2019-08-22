//
// Created by lihaibin on 2019-06-26.
//

#ifndef QTALK_V2_QUICKREPLYDAO_H
#define QTALK_V2_QUICKREPLYDAO_H

#include "sqlite/database.h"
#include "../include/CommonDefine.h"
#include "../entity/im_qr_group.h"
#include "../entity/im_qr_content.h"
#include <vector>

class QuickReplyDao{

public:
    explicit QuickReplyDao(qtalk::sqlite::database *sqlDb);
    bool creatTable();

public:
    void batchInsertQuickReply(const std::string &data);
    void getQuickReplyVersion(QInt64 version[]);

public:
    void getQuickGroups(std::vector<QTalk::Entity::ImQRgroup>& groups);
    void getQuickContentByGroup(std::vector<QTalk::Entity::IMQRContent>& contents,int id);

private:
    qtalk::sqlite::database *_pSqlDb;
};
#endif //QTALK_V2_QUICKREPLYDAO_H
