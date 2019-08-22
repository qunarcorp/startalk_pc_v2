//
// Created by cc on 18-12-21.
//

#ifndef QTALK_V2_CREATGROUPPOUPWND_H
#define QTALK_V2_CREATGROUPPOUPWND_H

#include "../CustomUi/UShadowWnd.h"
#include <QLineEdit>
#include <QLabel>

enum
{
    EM_BUTTON_NO,
    EM_BUTTON_YES,
};

/**
 * 建群弹出窗口
 */
class CreatGroupPoupWnd : public UShadowDialog
{
    Q_OBJECT
public:
    explicit CreatGroupPoupWnd(QWidget* parent);
    ~CreatGroupPoupWnd() override;

public:
    QString getGroupName();
    void setCunt(int count);

Q_SIGNALS:
    void sgCreatGroupSignal();

private:
    QLineEdit* _pLineEdit; //
    QLabel*    _pWarningLabel;
    QFrame*    _pTitleFrm;
};


#endif //QTALK_V2_CREATGROUPPOUPWND_H