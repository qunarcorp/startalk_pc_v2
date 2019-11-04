//
// Created by lihaibin on 2019-06-26.
//

#ifndef QTALK_V2_QUICKREPLYDAO_H
#define QTALK_V2_QUICKREPLYDAO_H

#include "DaoInterface.h"
#include "../include/CommonDefine.h"
#include "../entity/im_qr_group.h"
#include "../entity/im_qr_content.h"
#include <vector>

class QuickReplyDao : public DaoInterface{

public:
    explicit QuickReplyDao(qtalk::sqlite::database *sqlDb);
    bool creatTable() override;
    bool clearData() override ;

public:
    void batchInsertQuickReply(const std::string &data);
    void getQuickReplyVersion(QInt64 version[]);

public:
    void getQuickGroups(std::vector<QTalk::Entity::ImQRgroup>& groups);
    void getQuickContentByGroup(std::vector<QTalk::Entity::IMQRContent>& contents,int id);


};
#endif //QTALK_V2_QUICKREPLYDAO_H
