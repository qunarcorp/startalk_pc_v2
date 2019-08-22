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
#include <QAction>
#include "../CustomUi/UShadowWnd.h"
#include "../entity/im_userSupplement.h"
#include "../CustomUi/TextEdit.h"
#include "../entity/im_user.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QGridLayout;
class LinkButton;
class CardManager;
class QTextEdit;
class HeadPhotoLab;
class ModButton;
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

private:
    CardManager* _pMainPanel;

    HeadPhotoLab*    _pHeadlabel;   //头像
    QLineEdit*    _pNameLabel;   //用户名
    HeadPhotoLab*    _pSexLabel;   //性别
    QLineEdit* _pUserMoodEdit; //个性签名

	QLineEdit* _pUserMarks; //备注
	QLineEdit* _pUserNoEdit; //员工id
	QLineEdit* _pQunarIdEdit; //Qunar id
	QLabel*    _pDepartmentEdit; // 部门
	LinkButton* _pLeaderEdit; // 领导
	QLineEdit* _pMailEdit;  //邮箱
	LinkButton* _pPhoneNoEdit; //手机

	QLabel*    _pUserMaskLabel; // 备注
    ModButton* _editMaskBtn;
    ModButton* _pModMoodBtn;

	QPushButton* _pSendMessageBtn;
	QPushButton* _pAddFriendBtn;
	QPushButton* _pMailBtn;
	QPushButton* _pBtnStar;

	QGridLayout* bottomLayout;

private:
	QMenu*      _pMenu;
	QAction*    _pRemoveAct;    // 删除好友
	QAction*    _pAddBlackListAct; // 加入黑名单

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
