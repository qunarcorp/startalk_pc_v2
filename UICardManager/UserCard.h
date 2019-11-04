//
// Created by cc on 18-11-5.
//
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef QTALK_V2_USERCARD_H
#define QTALK_V2_USERCARD_H

#include <QWidget>
#include <memory>
#include <QMutexLocker>
#include <QMenu>
#include <QStackedWidget>
#include <QAction>
#include <QHBoxLayout>
#include <set>
#include "../CustomUi/UShadowWnd.h"
#include "../entity/im_userSupplement.h"
#include "../CustomUi/TextEdit.h"
#include "../entity/im_user.h"
#include "../include/CommonStrcut.h"
#include "../CustomUi/MedalWgt.h"
#include "medal/UserMedalWnd.h"
#include "medal/MedalDetailWnd.h"
#include "medal/UserListWnd.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QGridLayout;
class LinkButton;
class CardManager;
class QTextEdit;
class HeadPhotoLab;
class ModButton;

class StackShell : public UShadowDialog
{
public:
    explicit StackShell(QWidget* parent)
        :UShadowDialog(parent, true)
    {
#ifdef _MACOS
        macAdjustWindows();
#endif
        setFixedWidth(385);
        stackedWidget = new QStackedWidget(this);
        auto* lay = new QHBoxLayout(_pCenternWgt);
        lay->setMargin(0);
        lay->addWidget(stackedWidget);
    }

public:
    void addWidget(QWidget* wgt)
    {
        if(stackedWidget)
            stackedWidget->addWidget(wgt);
    }

    void setCurrentWgt(QWidget* wgt)
    {
        if(stackedWidget)
            stackedWidget->setCurrentWidget(wgt);
    }

private:
    QStackedWidget* stackedWidget{};
};

class user_card : public UShadowDialog
{
	Q_OBJECT

public:
    explicit user_card(CardManager* cardManager);
    ~user_card() override;

Q_SIGNALS:
	void setWgtStatusSignal();
	void sgJumpToStructre(const QString&);

public:
    bool showUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement> imuserSup,
                      std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo);
    void setMaskName(const QString& maskName);
	void setFlags(int flags);
	void showMedal(const std::set<QTalk::StUserMedal> &user_medal);

protected:
    void initUi();
    bool eventFilter(QObject* o, QEvent* e) override;
    bool event(QEvent* e) override;
    void closeEvent(QCloseEvent* e) override;

protected slots:
    void setWgtStatus();
	void sendMessageSlot();
	void sendMailSlot();
	void starUserSlot();
	void addBlackListSlot();

protected slots:
    void onLeaderBtnClick();
    void onPhoneNoBtnClick();
    void gotPhoneNo(const std::string& userId, const std::string& phoneNo);
    void onShowUserMedalDetail();
    void onShowMedalDetail(int id);
    void onShowUserList(const std::vector<QTalk::StMedalUser>&);
    void modifyUserMedalStatus(int id, bool isWear);

private:
    CardManager* _pMainPanel = nullptr;

    HeadPhotoLab* _pHeadlabel = nullptr;   //头像
    QLineEdit* _pNameLabel = nullptr;   //用户名
    HeadPhotoLab* _pSexLabel = nullptr;   //性别
    QLineEdit* _pUserMoodEdit = nullptr; //个性签名

	QLineEdit* _pUserMarks = nullptr; //备注
	QLineEdit* _pUserNoEdit = nullptr; //员工id
	QLineEdit* _pQunarIdEdit = nullptr; //Qunar id
	QLabel* _pDepartmentEdit = nullptr; // 部门
	LinkButton* _pLeaderEdit = nullptr; // 领导
	QLineEdit* _pMailEdit = nullptr;  //邮箱
	LinkButton* _pPhoneNoEdit = nullptr; //手机

    MedalWgt* _pMedalWgt = nullptr;

	QLabel* _pUserMaskLabel = nullptr; // 备注
    ModButton* _editMaskBtn = nullptr;
    ModButton* _pModMoodBtn = nullptr;

    LinkButton* _pMedalDetail = nullptr;

	QPushButton* _pSendMessageBtn = nullptr;
	QPushButton* _pAddFriendBtn = nullptr;
	QPushButton* _pMailBtn = nullptr;
	QPushButton* _pBtnStar = nullptr;

	QGridLayout* bottomLayout;
	//
    UserMedalWnd* _pUserMedalWnd = nullptr;
    MedalDetailWnd*  _pMedalDetailWnd = nullptr;
    StackShell*  _pStackedShell = nullptr;
    UserListWnd* _pUserListWnd = nullptr;

private:
	QMenu* _pMenu = nullptr;
	QAction* _pRemoveAct = nullptr;    // 删除好友
	QAction* _pAddBlackListAct = nullptr; // 加入黑名单

private:
    std::string _strUserId;
    QString     _strLeaderId;
	QString     _strUserName;
	QString     _strHeadSrc;
	std::string _strHeadLink;
	QString     _strMaskName; // old mask name
	QString     _strUserMood; // old mood
    bool        _isSelf;
	QMutex      _mutex;

    std::set<QTalk::StUserMedal> _user_medal;
private:
    enum
    {
        EM_MASK,
        EM_USERID,
        EM_QUNARID,
        EM_DEPARTMENT,
        EM_LEADER,
        EM_EMAIL,
        EM_PHONENNO,
        EM_MEDAL,
    };

public:
	enum
	{
		EM_IS_INVALID = 0,
		EM_IS_START = 1,
		EM_IS_BLACK = 2,
		EM_IS_FRIEND = 4,
	};
};


#endif //QTALK_V2_USERCARD_H
