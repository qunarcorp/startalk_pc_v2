//
// Created by lihaibin on 2019-07-08.
//

#ifndef QTALK_V2_TRIGGERCONFIG_H
#define QTALK_V2_TRIGGERCONFIG_H

#include "sqlite/database.h"

class TriggerConfig
{
public:
    explicit TriggerConfig(qtalk::sqlite::database *sqlDb = nullptr);
    bool  createUnreadUpdateTrigger();
    bool  createUnreadInserttrigger();

private:
    qtalk::sqlite::database *_sqlDb;
};

#endif //QTALK_V2_TRIGGERCONFIG_H
