//
// Created by cc on 18-11-5.
//

#include "UserCard.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QtConcurrent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QTextEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QFile>
#include <QEvent>
#include <QDesktopServices>
#include "../CustomUi/HeadPhotoLab.h"
#include "../QtUtil/Entity/JID.h"
#include "../Platform/Platform.h"
#include "../CustomUi/LinkButton.h"
#include "CardManager.h"
#include "../Platform/dbPlatForm.h"
#include "../include/ModButton.h"
#include "../Platform/NavigationManager.h"
#include "UserCardMsgManager.h"
#include "../CustomUi/QtMessageBox.h"
#include "../CustomUi/LiteMessageBox.h"

user_card::user_card(CardManager* cardManager)
    :UShadowDialog(cardManager, true), _pMainPanel(cardManager)
{
    initUi();
}

user_card::~user_card()
{

}

void user_card::initUi()
{
#ifdef _MACOS
    macAdjustWindows();
#endif
    setObjectName("user_card");
    setFixedWidth(400);
    // title
    auto * closeBtn = new QPushButton(this);
    auto * titleLayout = new QHBoxLayout;

#ifdef _MACOS
    closeBtn->setFixedSize(12, 12);
    closeBtn->setObjectName("gmCloseBtn");
    titleLayout->addWidget(closeBtn);
    titleLayout->setContentsMargins(8, 8, 0, 0);
    titleLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
#else
    closeBtn->setFixedSize(30, 30);
    closeBtn->setObjectName("gwCloseBtn");
    titleLayout->setMargin(0);
    titleLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    titleLayout->addWidget(closeBtn);
#endif

    // top
#ifdef _STARTALK
    QString defaultHead = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
    QString defaultHead = ":/QTalk/image1/headPortrait.png";
#endif
    _pHeadlabel = new HeadPhotoLab(defaultHead, 30, false, false, false, this);
    _pHeadlabel->installEventFilter(this);
    _pNameLabel = new QLineEdit(this);
    _pSexLabel = new HeadPhotoLab;
    _pUserMoodEdit = new QLineEdit(this);
    _pNameLabel->setObjectName("NameLabel");
    _pUserMoodEdit->setObjectName("UserMoodEdit");
    _pUserMoodEdit->setText(tr("这个人很懒，什么都没留下"));
    _pUserMoodEdit->setPlaceholderText(tr("这个人很懒，什么都没留下"));
    _pUserMoodEdit->setMaximumHeight(50);
    //
    _pNameLabel->setReadOnly(true);
    _pSexLabel->setFixedWidth(15);
    //
    _pMailBtn = new QPushButton(this);
    auto * btnSendCard = new QPushButton(this);
    _pAddFriendBtn = new QPushButton(this);
    _pBtnStar = new QPushButton(this);
    auto * btnMore = new QPushButton(this);

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

    _pAddFriendBtn->setVisible(false);

    auto * btnLay = new QHBoxLayout;
    btnLay->setMargin(0);
    btnLay->setSpacing(16);
    btnLay->addWidget(_pMailBtn);
    btnLay->addWidget(btnSendCard);
    btnLay->addWidget(_pAddFriendBtn);
    btnLay->addWidget(_pBtnStar);
    btnLay->addWidget(btnMore);
    btnLay->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding));
    //
    btnSendCard->setVisible(false);
    _pAddFriendBtn->setVisible(false);
    //
    QFrame* topFrame = new QFrame(this);
    topFrame->setObjectName("TopFrame");
    //
    auto * topRTLayout = new QHBoxLayout;
    topRTLayout->setSpacing(2);
    topRTLayout->addWidget(_pNameLabel);
    topRTLayout->addWidget(_pSexLabel);
    topRTLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    topRTLayout->setAlignment(_pSexLabel, Qt::AlignLeft | Qt::AlignVCenter);

    //
    _pModMoodBtn = new ModButton(this);
    auto* moodLay = new QHBoxLayout;
    moodLay->addWidget(_pUserMoodEdit);
    moodLay->addWidget(_pModMoodBtn);
//    moodLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
    //
    auto * topRLayout = new QVBoxLayout;
    topRLayout->setSpacing(15);
    topRLayout->addLayout(topRTLayout);
    topRLayout->addLayout(moodLay);
    topRLayout->addLayout(btnLay);
    topRLayout->setAlignment(moodLay, Qt::AlignLeft);
    //
    auto * topMainLay = new QHBoxLayout;
    topMainLay->setContentsMargins(25, 0, 30, 0);
    topMainLay->setSpacing(30);
    topMainLay->addWidget(_pHeadlabel);
    topMainLay->addLayout(topRLayout);
    topMainLay->setAlignment(_pHeadlabel, Qt::AlignTop);
    //
    auto * topLayout = new QVBoxLayout(topFrame);
    topLayout->setContentsMargins(0, 0, 0, 20);
    topLayout->addLayout(titleLayout);
    topLayout->addLayout(topMainLay);

    // bottom
    _pUserMarks      = new QLineEdit(this);
    _pUserNoEdit     = new QLineEdit(this);
    _pQunarIdEdit    = new QLineEdit(this);
    _pDepartmentEdit = new QLabel(this);
    _pLeaderEdit     = new LinkButton();
    _pMailEdit       = new QLineEdit(this);
    _pPhoneNoEdit    = new LinkButton(tr("点击查看"), this);
    _pMedalWgt = new MedalWgt(21, this);
    _pMedalDetail = new LinkButton(tr("详情"), this);

    _pUserMarks->setReadOnly(true);
    _pUserNoEdit->setReadOnly(true);
    _pQunarIdEdit->setReadOnly(true);
    _pMailEdit->setReadOnly(true);
    _pPhoneNoEdit->setFixedHeight(25);
    _pLeaderEdit->setFixedHeight(25);
    _pUserMarks->setPlaceholderText(tr("修改备注"));

    _pDepartmentEdit->setObjectName("DepartmentEdit");
    _pDepartmentEdit->setWordWrap(true);
    _pDepartmentEdit->installEventFilter(this);

    bottomLayout = new QGridLayout;
    bottomLayout->setContentsMargins(20, 0, 20, 0);
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
    tmpLabel = new QLabel(tr("勋章"));
    bottomLayout->addWidget(tmpLabel, EM_MEDAL, 0);
    // 备注修改按钮
    _editMaskBtn = new ModButton(this);

    auto* userMaskLay = new QHBoxLayout;
    userMaskLay->setMargin(0);
    userMaskLay->setSpacing(0);
    userMaskLay->addWidget(_pUserMarks);
    userMaskLay->addWidget(_editMaskBtn);
    bottomLayout->addLayout(userMaskLay, EM_MASK, 1, 1, 2);
    bottomLayout->setAlignment(userMaskLay, Qt::AlignLeft);
//    bottomLayout->addWidget(_editMaskBtn, EM_MASK, 2, Qt::AlignLeft);
    bottomLayout->addWidget(_pUserNoEdit, EM_USERID, 1, 1, 2);

    bottomLayout->addWidget(_pQunarIdEdit, EM_QUNARID, 1, 1, 2);
    bottomLayout->addWidget(_pDepartmentEdit, EM_DEPARTMENT, 1, 1, 2);
    bottomLayout->addWidget(_pLeaderEdit, EM_LEADER, 1, 1, 2);
    bottomLayout->addWidget(_pMailEdit, EM_EMAIL, 1, 1, 2);
    bottomLayout->addWidget(_pPhoneNoEdit, EM_PHONENNO, 1, 1, 2);

    auto* userMedalLay = new QHBoxLayout;
    userMedalLay->setMargin(0);
    userMedalLay->setSpacing(8);
    userMedalLay->addWidget(_pMedalWgt);
    userMedalLay->addWidget(_pMedalDetail);
    bottomLayout->addLayout(userMedalLay, EM_MEDAL, 1, 1, 2);

    bottomLayout->setColumnStretch(0, 0);
    bottomLayout->setColumnStretch(1, 1);
    bottomLayout->setColumnStretch(2, 0);

    // buttons
    _pSendMessageBtn = new QPushButton(tr("发消息"));
    _pSendMessageBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _pSendMessageBtn->setObjectName("SendMessage");
    //
    QFrame* bottomFrame = new QFrame(this);
    bottomFrame->setContentsMargins(6, 25, 6, 0);
    bottomFrame->setObjectName("BottomFrame");
    auto * bLayout = new QVBoxLayout;
    bLayout->addLayout(bottomLayout);
    bLayout->setSpacing(36);
    bLayout->addWidget(_pSendMessageBtn);
    bottomFrame->setLayout(bLayout);
    _pSendMessageBtn->setFixedHeight(32);
    //
    auto * layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignHCenter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(topFrame, 0);
    layout->addWidget(bottomFrame);
    layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    _pCenternWgt->setLayout(layout);
    layout->setStretch(0, 0);
    layout->setStretch(1, 1);
    //
    setMoverAble(true, nullptr);
    // add action
    _pMenu = new QMenu(this);
    _pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _pRemoveAct = new QAction(tr("解除好友"), _pMenu);
    _pAddBlackListAct = new QAction(tr("加入黑名单"), _pMenu);
    _pMenu->addAction(_pRemoveAct);
    _pMenu->addAction(_pAddBlackListAct);
    //
    _pStackedShell = new StackShell(_pMainPanel);

    connect(_pLeaderEdit, SIGNAL(clicked()), this, SLOT(onLeaderBtnClick()));
    connect(_pPhoneNoEdit, SIGNAL(clicked()), this, SLOT(onPhoneNoBtnClick()));
    qRegisterMetaType<std::string>("std::string");
    connect(_pMainPanel, SIGNAL(gotPhoneNo(const std::string&, const std::string&)),
            this, SLOT(gotPhoneNo(const std::string&, const std::string&)));
    connect(this, SIGNAL(setWgtStatusSignal()), this, SLOT(setWgtStatus()));
    connect(_pSendMessageBtn, &QPushButton::clicked, this, &user_card::sendMessageSlot);
    connect(_pMailBtn, &QPushButton::clicked, this, &user_card::sendMailSlot);
    connect(_pBtnStar, &QPushButton::clicked, this, &user_card::starUserSlot);
    connect(_pAddBlackListAct, &QAction::triggered, this, &user_card::addBlackListSlot);
    connect(closeBtn, &QPushButton::clicked, [this](){this->close();});
    connect(btnMore, &QPushButton::clicked, [this]()
    {
        _pMenu->exec(QCursor::pos());
    });

//    _editMaskBtn->setCheckable(true);
//    _pModMoodBtn->setCheckable(true);
    _pUserMarks->installEventFilter(this);
    _pUserMoodEdit->installEventFilter(this);
    connect(_editMaskBtn, &QPushButton::clicked, [this](bool)
    {
        _pUserMarks->setReadOnly(false);
        _editMaskBtn->setVisible(false);
        _pUserMarks->setFocus();
    });

    connect(_pModMoodBtn, &QPushButton::clicked, [this](bool){
        _pUserMoodEdit->setReadOnly(false);
        _pModMoodBtn->setVisible(false);
        _pUserMoodEdit->setFocus();
    });

    connect(_pUserMarks, &QLineEdit::textChanged, [this](){
        QString text = _pUserMarks->text();
        if(!text.isEmpty())
        {
            QFontMetricsF tf(_pUserMarks->font());
            int width = tf.width(text);
            width = qMin(width, 210);
            width = qMax(width, 30);
            _pUserMarks->setFixedWidth(width + 10);
        }
    });

    connect(_pUserMoodEdit, &QLineEdit::textChanged, [this](){
        QString text = _pUserMoodEdit->text();
        if(text.isEmpty())
            text = tr("这个人很懒，什么都没留下");

        QFontMetricsF tf(_pUserMoodEdit->font());
        int width = tf.width(text);
        width = qMin(width, 210);
        width = qMax(width, 30);
        _pUserMoodEdit->setFixedWidth(width + 10);
    });

    connect(_pMedalDetail, &LinkButton::clicked, this, &user_card::onShowUserMedalDetail);
}

bool user_card::showUserCard(std::shared_ptr<QTalk::Entity::ImUserSupplement> imuserSup,
                             std::shared_ptr<QTalk::Entity::ImUserInfo> info)
{
    QMutexLocker locker(&_mutex);
    _pUserMarks->setReadOnly(true);
    _strMaskName = "";
    _strLeaderId = "";
    _pUserMoodEdit->setReadOnly(true);
    bool ret = false;
    QTalk::Entity::JID jid(imuserSup->XmppId.c_str());
    //
    _isSelf = jid.username() == Platform::instance().getSelfUserId();
	emit setWgtStatusSignal();
    //
    ret = nullptr != info;
    if(ret)
    {
        if(!imuserSup->LeaderId.empty())
            _strLeaderId = QString("%1@%2").arg(imuserSup->LeaderId.c_str()).arg(jid.domainname().c_str());
        _strUserId = jid.barename();

        std::string strName = info->NickName;
        if(strName.empty())
            strName = info->Name;
        if(strName.empty())
            strName = QTalk::Entity::JID(imuserSup->XmppId.data()).username();
		_strUserName = QString::fromStdString(strName);
        if(!info->HeaderSrc.empty())
        {
            QString headSrc = QString(QTalk::GetHeadPathByUrl(info->HeaderSrc).c_str());
            _strHeadLink = info->HeaderSrc;
            if (QFile::exists(headSrc))
			{
				_strHeadSrc = headSrc;
                _pHeadlabel->setHead(headSrc, 30, false, true);
			}
            else
            {
#ifdef _STARTALK
                QString defaultHead = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
                QString defaultHead = ":/QTalk/image1/headPortrait.png";
#endif
                _pHeadlabel->setHead(defaultHead, 30);
            }
        }
        switch (info->Gender)
        {
            case 1:
                _pHeadlabel->setToolTip(tr("小哥哥"));
                _pSexLabel->setHead(QString(":/CardManager/image1/sex_male_%1.png").arg(AppSetting::instance().getThemeMode()),
                        7, false, false, true);
                break;
            case 2:
                _pHeadlabel->setToolTip(tr("小姐姐"));
                _pSexLabel->setHead(QString(":/CardManager/image1/sex_female_%1.png").arg(AppSetting::instance().getThemeMode()),
                        7, false, false, true);
                break;
            default:
                _pHeadlabel->setToolTip("");
                break;
        }

        _pNameLabel->setText(_strUserName);
        int nameWidth = QFontMetricsF(_pNameLabel->font()).width(_strUserName);
        _pNameLabel->setFixedWidth(nameWidth + 6);
        QString id = QString::fromStdString(jid.username());
        //
        QString mood = QString::fromStdString(info->Mood);
        _strUserMood = mood;
        _pUserMoodEdit->clear();
        _pUserMoodEdit->setText(mood);
        _pUserMoodEdit->setCursorPosition(0);

        _pUserNoEdit->setText(QString::fromStdString(imuserSup->UserNo));
        _pQunarIdEdit->setText(id);

        _pDepartmentEdit->setText(QString::fromStdString(info->DescInfo));
        if(!imuserSup->LeaderName.empty())
            _pLeaderEdit->setLinkText(QString("%1 (%2)").arg(imuserSup->LeaderName.c_str()).arg(imuserSup->LeaderId.c_str()));
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

void user_card::showMedal(const std::set<QTalk::StUserMedal> &user_medal) {
    _user_medal = user_medal;
    _pMedalWgt->addMedals(user_medal);
}

#include <QStyle>
void user_card::setWgtStatus()
{
	_pUserMarks->setVisible(!_isSelf);
	_pUserMaskLabel->setVisible(!_isSelf);
    _editMaskBtn->setVisible(!_isSelf);
    _pModMoodBtn->setVisible(_isSelf);
//	_pAddFriendBtn->setVisible(!isSelf);
	_pMailBtn->setVisible(!_isSelf);

}

//
void user_card::onLeaderBtnClick()
{
    if(nullptr != _pMainPanel)
    {
        if(!_strLeaderId.isEmpty())
        {
            _pMainPanel->shwoUserCard(_strLeaderId);
            this->close();
        }
        else
        {
            //

        }
    }
}

//
void user_card::onPhoneNoBtnClick()
{
    if(nullptr != _pMainPanel)
    {
        emit _pMainPanel->sgOperator(tr("查看手机号"));
        _pMainPanel->getPhoneNo(_strUserId);
    }
}

void user_card::gotPhoneNo(const std::string &userId, const std::string &phoneNo)
{
    if(this->isVisible() && _strUserId == userId)
    {
        _pPhoneNoEdit->setNormalText(QString::fromStdString(phoneNo));
    }
}

/**
 * 发送消息
 */
void user_card::sendMessageSlot()
{
    StSessionInfo stSession(QTalk::Enum::TwoPersonChat, QString::fromStdString(_strUserId), _strUserName);
    stSession.headPhoto = _strHeadSrc;

    emit _pMainPanel->sgSwitchCurFun(0);
    emit _pMainPanel->sgOpenNewSession(stSession);
    this->close();
}

/**
 * 发送邮件
 */
void user_card::sendMailSlot()
{
    QString mailAddr = _pMailEdit->text();
    if(!mailAddr.isEmpty())
    {
        QString mailUrl = QString("mailto:%1").arg(mailAddr);
        QDesktopServices::openUrl(QUrl(mailUrl));
    }
    this->close();
}

/**
 * 星标联系人
 */
void user_card::starUserSlot()
{
    if(_pMainPanel)
    {
        emit _pMainPanel->sgOperator(tr("星标联系人"));
        _pMainPanel->starUser(_strUserId);
    }
}

void user_card::addBlackListSlot()
{
    if(_pMainPanel)
    {
        emit _pMainPanel->sgOperator(tr("加入黑名单"));
        _pMainPanel->addBlackList(_strUserId);
    }
}

/**
 *
 * @param flags
 */
void user_card::setFlags(int flags)
{
    bool isFriend = flags & EM_IS_FRIEND;
    bool isBlack = flags & EM_IS_BLACK;
//    _pAddFriendBtn->setVisible(!isFriend);
    _pRemoveAct->setEnabled(isFriend);
    _pBtnStar->setChecked(flags & EM_IS_START);
    _pAddBlackListAct->setText(isBlack ? tr("移除黑名单") : tr("加入黑名单"));
}


void user_card::setMaskName(const QString &maskName)
{
    _strMaskName = maskName;
    _pUserMarks->setText(maskName);
}

/**
 *
 * @param o
 * @param e
 * @return
 */
bool user_card::eventFilter(QObject *o, QEvent *e)
{
    if(o == _pDepartmentEdit)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            auto *evt = (QMouseEvent*)e;
            if(evt && evt->button() == Qt::LeftButton)
            {
                QString structreName = _pDepartmentEdit->text();
                if(!structreName.isEmpty())
                    emit sgJumpToStructre(structreName);
            }
        }
        else if(e->type() == QEvent::Enter)
        {
            setCursor(Qt::PointingHandCursor);
        }
        else if(e->type() == QEvent::Leave)
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else if(o == _pHeadlabel)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            std::thread([this](){
                if(_pMainPanel)
                {
                    std::string srcHead = _pMainPanel->getSourceHead(_strHeadLink);
                    emit _pMainPanel->sgShowHeadWnd(QString::fromStdString(srcHead), _isSelf);
                }
            }).detach();
            this->setVisible(false);
//            this->close();
        }
        else if(e->type() == QEvent::Enter)
        {
            setCursor(Qt::PointingHandCursor);
        }
        else if(e->type() == QEvent::Leave)
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else if(o == _pUserMarks || o == _pUserMoodEdit)
    {
        auto* edit = qobject_cast<QLineEdit*>(o);
        if(nullptr != edit)
        {
            if(e->type() == QEvent::KeyPress)
            {
                QKeyEvent* evt = (QKeyEvent*)e;
                if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
                {
                    edit->setReadOnly(true);
                    if(o == _pUserMarks)
                        _editMaskBtn->setVisible(!_isSelf);
                    else if(o == _pUserMoodEdit)
                        _pModMoodBtn->setVisible(_isSelf);
                }
            }
            else if(e->type() == QEvent::FocusOut)
            {
                edit->setReadOnly(true);
                if(o == _pUserMarks)
                    _editMaskBtn->setVisible(!_isSelf);
                else if(o == _pUserMoodEdit)
                    _pModMoodBtn->setVisible(_isSelf);
            }
        }
    }

    return QDialog::eventFilter(o, e);
}

bool user_card::event(QEvent* e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        if(_pUserMarks->hasFocus()  || _pUserMoodEdit->hasFocus() )
        {
            this->setFocus();
        }
    }
    return QDialog::event(e);
}

void user_card::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e)
    QString maskName = _pUserMarks->text();
    bool isSelf = _strUserId == Platform::instance().getSelfXmppId();
    QString mood = _pUserMoodEdit->text();

    if(nullptr == _pMainPanel) return;

    if(!isSelf && maskName != _strMaskName)
    {
        _pMainPanel->setUserMaskName(_strUserId, maskName.toStdString());
    }
    if(isSelf && mood != _strUserMood)
    {
        _pMainPanel->setUserMood(mood.toStdString());
    }
}

//
void user_card::onShowUserMedalDetail() {
    if(nullptr == _pUserMedalWnd)
    {
        _pUserMedalWnd = new UserMedalWnd(_strUserName,
                                _strHeadSrc, _user_medal, _pStackedShell);
        _pStackedShell->addWidget(_pUserMedalWnd);
        connect(_pUserMedalWnd, &UserMedalWnd::sgShowMedalDetail, this, &user_card::onShowMedalDetail);
    }
    this->setVisible(false);
    _pStackedShell->setCurrentWgt(_pUserMedalWnd);
    _pStackedShell->move(this->geometry().topLeft());
    _pStackedShell->show();
    _pStackedShell->setMoverAble(true, _pUserMedalWnd->getMoveWgt());
}

void user_card::modifyUserMedalStatus(int id, bool isWear)
{
    if(!_isSelf)
        return;
    int wear_count = 0;
    for(const auto& um : _user_medal)
    {
        if(um.medalStatus & 0x02)
            wear_count++;
    }
    //
    if(isWear && wear_count >= 3)
    {
        QtMessageBox::warning(this, tr("警告"), tr("最多佩戴三枚勋章, 请卸下其他勋章之后再操作"));
        return;
    }

    QFuture<bool> future = QtConcurrent::run(&UserCardMsgManager::modifyUserMedal, id, isWear);
    while (!future.isFinished())
        QApplication::processEvents(QEventLoop::AllEvents, 100);

    bool ret = future.result();
    if(ret)
    {
        LiteMessageBox::success(isWear ? tr("佩戴成功") : tr("已卸下"), 1000, this);
        //
        _pMedalDetailWnd->onModifySuccess(id, isWear);
        auto itFind = std::find_if(_user_medal.begin(), _user_medal.end(), [id](const QTalk::StUserMedal& stUserMedal){
            return stUserMedal.medalId == id;
        });
        if(itFind != _user_medal.end())
            _user_medal.erase(itFind);

        _user_medal.insert(QTalk::StUserMedal(id, isWear ? 0x03 : 0x01));
        _pUserMedalWnd->init_medals(_user_medal);
    }
    else
        QtMessageBox::warning(this, tr("警告"), tr("操作失败!"));
}

void user_card::onShowMedalDetail(int id) {
    if(nullptr == _pMedalDetailWnd)
    {
        _pMedalDetailWnd = new MedalDetailWnd(_pStackedShell);
        _pStackedShell->addWidget(_pMedalDetailWnd);
        connect(_pMedalDetailWnd, &MedalDetailWnd::sgShowBack, [this](){
            if(_pUserMedalWnd && _pStackedShell)
                _pStackedShell->setCurrentWgt(_pUserMedalWnd);
        });
        qRegisterMetaType<std::vector<QTalk::StMedalUser>>("std::vector<QTalk::StMedalUser>");
        connect(_pMedalDetailWnd, &MedalDetailWnd::sgShowUserList, this, &user_card::onShowUserList);
        //
        connect(_pMedalDetailWnd, &MedalDetailWnd::sgModifyStatus, this, &user_card::modifyUserMedalStatus);
    }
    //
    auto itFind = std::find_if(_user_medal.begin(), _user_medal.end(), [id](const QTalk::StUserMedal& it){
        return it.medalId == id;
    });
    auto medalInfo = dbPlatForm::instance().getMedal(id);
    bool isLight = itFind != _user_medal.end();
    _pStackedShell->setCurrentWgt(_pMedalDetailWnd);
    _pStackedShell->setMoverAble(true, _pMedalDetailWnd->getMoveWgt());
    _pMedalDetailWnd->showMedalDetail(medalInfo, _isSelf, isLight ? itFind->medalStatus : 0);

    QtConcurrent::run([this, id](){
        std::vector<QTalk::StMedalUser> users;
        UserCardMsgManager::getMedalUser(id, users);
        _pMedalDetailWnd->setMedalUsers(id, users);
    });
}

void user_card::onShowUserList(const std::vector<QTalk::StMedalUser> &useList) {

    if(nullptr == _pUserListWnd)
    {
        _pUserListWnd = new UserListWnd(_pStackedShell);
        _pStackedShell->addWidget(_pUserListWnd);
        connect(_pUserListWnd, &UserListWnd::sgShowBack, [this](){
            if(_pMedalDetailWnd && _pStackedShell)
                _pStackedShell->setCurrentWgt(_pMedalDetailWnd);
        });
    }
    _pStackedShell->setCurrentWgt(_pUserListWnd);
    _pStackedShell->setMoverAble(true, _pUserListWnd->getMoveWgt());
    _pUserListWnd->showUserList(useList);
}