//
// Created by cc on 18-11-5.
//

#ifndef QTALK_USERCARD_H
#define QTALK_USERCARD_H

#include <QWidget>
#include <memory>
#include <QMutexLocker>
#include <QFrame>
#include "../entity/im_userSupplement.h"
#include "../CustomUi/TextEdit.h"
#include "../entity/im_user.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QGridLayout;
class LinkButton;
class AddressBookPanel;
class QTextEdit;
class QMenu;
class QAction;
class HeadPhotoLab;
class UserCard : public QFrame
{
Q_OBJECT

public:
    UserCard(AddressBookPanel* panel);
    ~UserCard();

Q_SIGNALS:
    void setWgtStatusSignal(bool);

public:
    bool showUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement> imuserSup,
                      std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo);
    void setMaskName(const QString& maskName);
    void setFlags(int flags);

protected:
    void initUi();

protected slots:
    void setWgtStatus(bool isSelf);
    void sendMessageSlot();
    void sendMailSlot();
    void starUserSlot();
    void addBlackListSlot();

public slots:
    void onLeaderBtnClick();
    void onPhoneNoBtnClick();
    void gotPhoneNo(const std::string& userId, const std::string& phoneNo);

private:
    AddressBookPanel* _pMainPanel;

    HeadPhotoLab*    _pHeadlabel;   //头像
    QLabel*    _pNameLabel;   //用户名
    QLabel*  _pUserMoodEdit; //个性签名

    QLineEdit* _pUserMarks; //备注
    QLineEdit* _pUserNoEdit; //员工id
    QLineEdit* _pQunarIdEdit; //Qunar id
    QLabel*    _pDepartmentEdit; // 部门
    LinkButton* _pLeaderEdit; // 领导
    QLineEdit* _pMailEdit;  //邮箱
    LinkButton* _pPhoneNoEdit; //手机

    QLabel*    _pUserMaskLabel; // 备注

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
