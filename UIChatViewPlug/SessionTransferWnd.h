//
// Created by lihaibin on 2019-06-18.
//

#ifndef QTALK_V2_SESSIONTRANSFERWND_H
#define QTALK_V2_SESSIONTRANSFERWND_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QButtonGroup>
#include <QTextEdit>
#include <QVBoxLayout>
#include "../CustomUi/UShadowWnd.h"
#include "../entity/im_transfer.h"

class SessionTransferWnd : public UShadowDialog{
	Q_OBJECT
public:
    explicit SessionTransferWnd(QWidget* parent = nullptr);
    ~SessionTransferWnd() override;

public:
    void initUI();
    void showSeats(std::vector<QTalk::Entity::ImTransfer> _transfers);


private slots:
    void operatingModeButtonsToggled(int, bool);

Q_SIGNALS:
    void sessiontransfer(std::string&,std::string&);

public:
    bool isHaveShow = false;


private:
    QLabel* _pTitleLabel;
    QButtonGroup *_pButtonGroup;
    QTextEdit *_pEdit;
    QVBoxLayout* _pSeats;

    QFrame*    _pTitleFrm;
    std::string newCsrName;
};

#endif //QTALK_V2_SESSIONTRANSFERWND_H
