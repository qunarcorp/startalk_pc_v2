//
// Created by QITMAC000260 on 2019/10/15.
//

#ifndef QTALK_V2_DAOINTERFACE_H
#define QTALK_V2_DAOINTERFACE_H

#include "sqlite/database.h"

class DaoInterface
{
public:
    explicit DaoInterface(qtalk::sqlite::database *sqlDb = nullptr)
        : _pSqlDb(sqlDb) {
    }
    virtual bool creatTable() = 0;
    virtual bool clearData() = 0;

protected:
    qtalk::sqlite::database *_pSqlDb;
};

#endif //QTALK_V2_DAOINTERFACE_H
