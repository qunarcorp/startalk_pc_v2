//
// Created by cc on 18-11-5.
//

#include "UserCard.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QTextEdit>
#include <QMouseEvent>
#include <QFile>
#include <QDesktopServices>
#include <QMenu>
#include <QAction>
#include <QFileInfo>
#include "../CustomUi/HeadPhotoLab.h"
#include "../QtUtil/Entity/JID.h"
#include "../Platform/Platform.h"
#include "../CustomUi/LinkButton.h"
#include "AddressBookPanel.h"
#include "../UICom/UIEntity.h"
#include "../UICardManager/UserCard.h"
#include "../Platform/dbPlatForm.h"
#include "../Platform/NavigationManager.h"

UserCard::UserCard(AddressBookPanel* panel)
    :QFrame(panel), _pMainPanel(panel)
{
    initUi();
}

UserCard::~UserCard()
{

}

void UserCard::initUi()
{
    setObjectName("UserCard");
    // title
    QFrame* titleFrame = new QFrame(this);
    titleFrame->setObjectName("UserCard_titleFrame");
    titleFrame->setFixedHeight(50);
    LinkButton* findFriend = new LinkButton(tr("查找并添加好友"));
    findFriend->setAlignment(Qt::AlignBottom);
    QHBoxLayout* titleLayout = new QHBoxLayout(titleFrame);
    titleLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    titleLayout->addWidget(findFriend);
    findFriend->setVisible(false);
    // top

#ifdef _STARTALK
    QString defaultHead(":/QTalk/image1/StarTalk_defaultHead.png");
#else
    QString defaultHead(":/QTalk/image1/headPortrait.png");
#endif
    _pHeadlabel = new HeadPhotoLab(defaultHead, 30, false, false, false, this);
    _pNameLabel = new QLabel(this);
    _pUserMoodEdit = new QLabel(this);
    _pNameLabel->setObjectName("UserCard_NameLabel");

    //
    _pMailBtn = new QPushButton(this);
    QPushButton* btnSendCard = new QPushButton(this);
    _pAddFriendBtn = new QPushButton(this);
    _pBtnStar = new QPushButton(this);
    QPushButton* btnMore = new QPushButton(this);

    _pMailBtn->setObjectName("UserCard_SendMail");
    btnSendCard->setObjectName("UserCard_SendCard");
    _pAddFriendBtn->setObjectName("UserCard_AddFriend");
    _pBtnStar->setObjectName("UserCard_Star");
    btnMore->setObjectName("btn_More");

    _pBtnStar->setCheckable(true);

    _pMailBtn->setToolTip(tr("发送邮件"));
    btnSendCard->setToolTip(tr("转发名片"));
    _pAddFriendBtn->setToolTip(tr("添加好友"));
    _pBtnStar->setToolTip(tr("星标联系人"));
    btnMore->setToolTip(tr("更多"));

    _pMailBtn->setFixedSize(30, 30);
    btnSendCard->setFixedSize(30, 30);
    _pAddFriendBtn->setFixedSize(30, 30);
    _pBtnStar->setFixedSize(30, 30);
    btnMore->setFixedSize(30, 30);

    auto* btnLay = new QHBoxLayout;
    btnLay->setMargin(0);
    btnLay->setSpacing(16);
    btnLay->addWidget(_pMailBtn);
    btnLay->addWidget(btnSendCard);
    btnLay->addWidget(_pAddFriendBtn);
    btnLay->addWidget(_pBtnStar);
    btnLay->addWidget(btnMore);
    btnLay->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding));

    btnSendCard->setVisible(false);
    _pAddFriendBtn->setVisible(false);
    //
    QFrame* topFrame = new QFrame(this);
    //topFrame->setFixedHeight(140);
    topFrame->setObjectName("UserCard_TopFrame");
    auto* topLayout = new QHBoxLayout(topFrame);
    auto* topRLayout = new QVBoxLayout;
    auto* topRTLayout = new QHBoxLayout;
    topLayout->setMargin(15);
    topLayout->setSpacing(20);
    topRLayout->setSpacing(10);
    topRTLayout->addWidget(_pNameLabel);
    topRTLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    topRLayout->addLayout(topRTLayout);
    topRLayout->addWidget(_pUserMoodEdit);
    topRLayout->addLayout(btnLay);
    topLayout->addLayout(topRLayout);
    topLayout->addWidget(_pHeadlabel);

    // bottom
    _pUserMarks      = new QLineEdit(this);
    _pUserNoEdit     = new QLineEdit(this);
    _pQunarIdEdit    = new QLineEdit(this);
    _pDepartmentEdit = new QLabel(this);
    _pLeaderEdit     = new LinkButton();
    _pMailEdit       = new QLineEdit(this);
    _pPhoneNoEdit    = new LinkButton(tr("点击查看"), this);

    _pUserMarks->setReadOnly(true);
    _pUserNoEdit->setReadOnly(true);
    _pQunarIdEdit->setReadOnly(true);
    _pMailEdit->setReadOnly(true);
    _pPhoneNoEdit->setFixedHeight(25);
    _pLeaderEdit->setFixedHeight(25);
    _pUserMarks->setPlaceholderText(tr("备注"));

    _pUserMarks->setContextMenuPolicy (Qt::NoContextMenu);
    _pUserNoEdit->setContextMenuPolicy (Qt::NoContextMenu);
    _pQunarIdEdit->setContextMenuPolicy (Qt::NoContextMenu);
    _pMailEdit->setContextMenuPolicy (Qt::NoContextMenu);

    _pDepartmentEdit->setWordWrap(true);
    _pDepartmentEdit->setObjectName("DepartmentEdit");

    bottomLayout = new QGridLayout;
    bottomLayout->setHorizontalSpacing(20);
    bottomLayout->setVerticalSpacing(15);
    _pUserMaskLabel = new QLabel(tr("备注"));
    bottomLayout->addWidget(_pUserMaskLabel, EM_MASK, 0);
    QLabel* tmpLabel = new QLabel(tr("员工 ID"));
    bottomLayout->addWidget(tmpLabel, EM_USERID, 0);
    if(NavigationManager::instance().getLeaderUrl().empty())
    {
        tmpLabel->setVisible(false);
        _pUserNoEdit->setVisible(false);
    }
#if defined(_ATALK)
    tmpLabel = new QLabel(tr("ATalk ID"));
#elif defined(_STARTALK)
    tmpLabel = new QLabel(tr("StarTalk ID"));
#else
    tmpLabel = new QLabel(tr("Qunar ID"));
#endif
    bottomLayout->addWidget(tmpLabel, EM_QUNARID, 0);
    tmpLabel = new QLabel(tr("所在部门"));
    bottomLayout->addWidget(tmpLabel, EM_DEPARTMENT, 0);
    tmpLabel = new QLabel(tr("直属领导"));
    bottomLayout->addWidget(tmpLabel, EM_LEADER, 0);
    if(NavigationManager::instance().getLeaderUrl().empty())
    {
        tmpLabel->setVisible(false);
        _pLeaderEdit->setVisible(false);
    }
    tmpLabel = new QLabel(tr("工作邮箱"));
    bottomLayout->addWidget(tmpLabel, EM_EMAIL, 0);
    if(NavigationManager::instance().getMailSuffix().empty())
    {
        tmpLabel->setVisible(false);
        _pMailEdit->setVisible(false);
    }
    tmpLabel = new QLabel(tr("手机号"));
    bottomLayout->addWidget(tmpLabel, EM_PHONENNO, 0);
    if(NavigationManager::instance().getPhoneNumAddr().empty())
    {
        tmpLabel->setVisible(false);
        _pPhoneNoEdit->setVisible(false);
    }

    bottomLayout->addWidget(_pUserMarks, EM_MASK, 1);
    bottomLayout->addWidget(_pUserNoEdit, EM_USERID, 1);
    bottomLayout->addWidget(_pQunarIdEdit, EM_QUNARID, 1);
    bottomLayout->addWidget(_pDepartmentEdit, EM_DEPARTMENT, 1);
    bottomLayout->addWidget(_pLeaderEdit, EM_LEADER, 1);
    bottomLayout->addWidget(_pMailEdit, EM_EMAIL, 1);
    bottomLayout->addWidget(_pPhoneNoEdit, EM_PHONENNO, 1);

    // buttons
    _pSendMessageBtn = new QPushButton(tr("发消息"));
    _pSendMessageBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _pSendMessageBtn->setObjectName("UserCard_SendMessage");
    //
    QFrame* bottomFrame = new QFrame(this);
    bottomFrame->setContentsMargins(6, 25, 6, 0);
    bottomFrame->setObjectName("UserCard_BottomFrame");
    QVBoxLayout* bLayout = new QVBoxLayout;
    bLayout->addLayout(bottomLayout);
    bLayout->setSpacing(36);
    bLayout->addWidget(_pSendMessageBtn);
    bottomFrame->setLayout(bLayout);
    //
    QVBoxLayout *bodyLay = new QVBoxLayout;
    bodyLay->addWidget(topFrame);
    bodyLay->addWidget(bottomFrame);
    topFrame->setFixedWidth(360);
    bottomFrame->setFixedWidth(360);
    QHBoxLayout* mainLay = new QHBoxLayout;
    mainLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    mainLay->addLayout(bodyLay);
    mainLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    //
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignHCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(titleFrame);
    layout->addItem(new QSpacerItem(10, 60 , QSizePolicy::Fixed, QSizePolicy::Fixed));
    layout->addLayout(mainLay);
    layout->addItem(new QSpacerItem(10, 10 , QSizePolicy::Fixed, QSizePolicy::Expanding));
    this->setLayout(layout);

    // add action
    _pMenu = new QMenu(this);
    _pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _pRemoveAct = new QAction(tr("解除好友"), _pMenu);
    _pAddBlackListAct = new QAction(tr("加入黑名单"), _pMenu);
    _pMenu->addAction(_pRemoveAct);
    _pMenu->addAction(_pAddBlackListAct);

    connect(_pLeaderEdit, SIGNAL(clicked()), this, SLOT(onLeaderBtnClick()));
    connect(_pPhoneNoEdit, SIGNAL(clicked()), this, SLOT(onPhoneNoBtnClick()));
    qRegisterMetaType<std::string>("std::string");
    connect(_pMainPanel, SIGNAL(gotPhoneNo(const std::string&, const std::string&)),
            this, SLOT(gotPhoneNo(const std::string&, const std::string&)));
	connect(this, SIGNAL(setWgtStatusSignal(bool)), this, SLOT(setWgtStatus(bool)));
	connect(_pSendMessageBtn, &QPushButton::clicked, this, &UserCard::sendMessageSlot);
	connect(_pMailBtn, &QPushButton::clicked, this, &UserCard::sendMailSlot);
	connect(_pBtnStar, &QPushButton::clicked, this, &UserCard::starUserSlot);
	connect(_pAddBlackListAct, &QAction::triggered, this, &UserCard::addBlackListSlot);
	connect(btnMore, &QPushButton::clicked, [this]()
	{
	    _pMenu->exec(QCursor::pos());
	});
}

bool UserCard::showUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement> imuserSup,
                            std::shared_ptr<QTalk::Entity::ImUserInfo> info)
{
    QMutexLocker locker(&_mutex);

    QTalk::Entity::JID jid(imuserSup->XmppId.c_str());
    //
	emit setWgtStatusSignal(jid.username() == Platform::instance().getSelfUserId());
    //
    bool ret = (nullptr != info);
    if(ret)
    {
        _strLeaderId = QString("%1@%2").arg(imuserSup->LeaderId.c_str()).arg(jid.domainname().c_str());
        _strUserId = imuserSup->XmppId;

        std::string strName = info->NickName;
        if(strName.empty())
            strName = info->Name;
        if(strName.empty())
            strName = QTalk::Entity::JID(imuserSup->XmppId.data()).username();
        _strUserName = QString::fromStdString(strName);

        QString headSrc = QString(QTalk::GetHeadPathByUrl(info->HeaderSrc).c_str());
        QFileInfo headFileinfo(headSrc);
        if(headFileinfo.exists() && headFileinfo.isFile())
        {
            _strHeadSrc = headSrc;
            _pHeadlabel->setHead(headSrc, 30, false, true);
        }
        else
        {
#ifdef _STARTALK
            QString defaultHead(":/QTalk/image1/StarTalk_defaultHead.png");
#else
            QString defaultHead(":/QTalk/image1/headPortrait.png");
#endif
            _pHeadlabel->setHead(defaultHead, 30);
        }
        switch (info->Gender)
        {
            case 1:
                _pHeadlabel->setToolTip(tr("小哥哥"));
                break;
            case 2:
                _pHeadlabel->setToolTip(tr("小姐姐"));
                break;
            default:
                _pHeadlabel->setToolTip("");
                break;
        }
        _pNameLabel->setText(_strUserName);
        _pUserMoodEdit->setText(QString::fromStdString(imuserSup->UserMood));

        _pUserMarks->setText("");
        _pUserNoEdit->setText(QString::fromStdString(imuserSup->UserNo));
        _pQunarIdEdit->setText(QString::fromStdString(jid.username()));
        QString descText = QString::fromStdString(info->DescInfo);
        _pDepartmentEdit->setText(descText);
        _pDepartmentEdit->scroll(0,0);
        if(!imuserSup->LeaderName.empty())
            _pLeaderEdit->setLinkText(QString("%1(%2)").arg(imuserSup->LeaderName.c_str()).arg(imuserSup->LeaderId.c_str()));
        else
            _pLeaderEdit->setNormalText("");
        _pPhoneNoEdit->setLinkText(tr("点击查看"));
        _pMailEdit->setText(QString::fromStdString(imuserSup->MailAddr));

        //
        _pUserMarks->setCursorPosition(0);
        _pUserNoEdit->setCursorPosition(0);
        _pQunarIdEdit->setCursorPosition(0);
        _pMailEdit->setCursorPosition(0);
    }

    return ret;
}


void UserCard::setWgtStatus(bool isSelf)
{
	_pUserMarks->setVisible(!isSelf);
	_pUserMaskLabel->setVisible(!isSelf);

//	_pAddFriendBtn->setVisible(!isSelf);
	_pMailBtn->setVisible(!isSelf);
}

//
void UserCard::onLeaderBtnClick()
{
    if(nullptr != _pMainPanel)
    {
        _pMainPanel->showUserCard(_strLeaderId);
    }
}

//
void UserCard::onPhoneNoBtnClick()
{
    if(nullptr != _pMainPanel)
    {
        _pMainPanel->getPhoneNo(_strUserId);
    }
}

void UserCard::gotPhoneNo(const std::string &userId, const std::string &phoneNo)
{
    if(this->isVisible() && _strUserId == userId)
    {
        _pPhoneNoEdit->setNormalText(QString::fromStdString(phoneNo));
    }
}

/**
 *
 * @param flags
 */
void UserCard::setFlags(int flags)
{
    bool isFriend = flags & EM_IS_FRIEND;
    bool isBlack = flags & EM_IS_BLACK;
//    _pAddFriendBtn->setVisible(!isFriend);
    _pRemoveAct->setEnabled(isFriend);
    _pBtnStar->setChecked(flags & EM_IS_START);
    _pAddBlackListAct->setText(isBlack ? tr("移除黑名单") : tr("加入黑名单"));
}

/**
 * 发送消息
 */
void UserCard::sendMessageSlot()
{
    StSessionInfo stSession(QTalk::Enum::TwoPersonChat, QString::fromStdString(_strUserId), _strUserName);
    stSession.headPhoto = _strHeadSrc;

    emit _pMainPanel->sgSwitchCurFun(0);
    emit _pMainPanel->sgOpenNewSession(stSession);
}

/**
 * 发送邮件
 */
void UserCard::sendMailSlot()
{
    QString mailAddr = _pMailEdit->text();
    if(!mailAddr.isEmpty())
    {
        QString mailUrl = QString("mailto:%1").arg(mailAddr);
        QDesktopServices::openUrl(QUrl(mailUrl));
    }
}

/**
 * 星标联系人
 */
void UserCard::starUserSlot()
{
    if(_pMainPanel)
    {
        _pMainPanel->starUser(_strUserId);
    }
}

void UserCard::addBlackListSlot()
{
    if(_pMainPanel)
    {
        _pMainPanel->addBlackList(_strUserId);
    }
}

void UserCard::setMaskName(const QString &maskName)
{
    _pUserMarks->setText(maskName);
}
