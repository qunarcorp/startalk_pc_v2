//
// Created by cc on 18-11-5.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef QTALK_V2_GROUPCARD_H
#define QTALK_V2_GROUPCARD_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <memory>
#include <QMenu>
#include "../CustomUi/TextEdit.h"
#include "../CustomUi/UShadowWnd.h"
#include "../entity/im_group.h"
#include "../include/CommonStrcut.h"
#include "../CustomUi/GroupMemberPopWnd.hpp"

class CardManager;
class HeadPhotoLab;
class ModButton;
class GroupCard : public UShadowDialog
{
	Q_OBJECT
public:
    GroupCard(CardManager* cardManager);
    ~GroupCard();

public:
    void setData(std::shared_ptr<QTalk::Entity::ImGroupInfo>& data);
    void showGroupMember(const std::map<std::string, QTalk::StUserCard>&, const std::map<std::string, QUInt8>& userRole);

protected:
    bool eventFilter(QObject* o, QEvent* e) override;
    bool event(QEvent* e) override ;

private:
    void initUi();
    void sendMessageSlot();
    void onClose();

private:
    CardManager* _pCardManager;

    QLabel*      _pNameLabel;
    HeadPhotoLab*      _pHeadLabel;
    QPushButton* _pSendMailBtn;
    QPushButton* _pExitGroupBtn;
    QPushButton* _pDestroyGroupBtn;

    QLineEdit*   _pGroupNameEdit;
    QTextEdit*    _pGroupIdEdit;
    QTextEdit*    _pGroupTopicEdit;
    QLabel*      _pGroupMemberLabel;
    QTextEdit*    _pGroupIntroduce;
    QPushButton* _pSendMsgBtn;

    GroupMemberPopWnd* _pGroupMemberPopWnd;

    ModButton*    _modGroupName;
    ModButton*    _modGroupTopic;
    ModButton*    _modGroupJJ;

private:
    QString _strGroupId;
    QString _srtHead;
    QString _groupName;

private:
    bool   _moded;
};

#endif //QTALK_V2_GROUPCARD_H
