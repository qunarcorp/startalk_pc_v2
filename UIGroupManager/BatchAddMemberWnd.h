//
// Created by QITMAC000260 on 2019-03-22.
//

#ifndef QTALK_V2_BATCHADDMEMBERWND_H
#define QTALK_V2_BATCHADDMEMBERWND_H
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QFrame>
#include "../CustomUi/UShadowWnd.h"
#include "AddGroupEdit.h"

/**
* @description: BatchAddMemberWnd
* @author: cc
* @create: 2019-03-22 17:28
**/
class BatchAddMemberWnd : public UShadowDialog {
    Q_OBJECT
public:
    explicit BatchAddMemberWnd(QWidget* parent = nullptr);
    ~BatchAddMemberWnd() override;

public:
    void clear();

Q_SIGNALS:
    void sgBatchAddGroupMember(const QString&);

private:
    AddGroupEdit* _pEidt;

};


#endif //QTALK_V2_BATCHADDMEMBERWND_H
