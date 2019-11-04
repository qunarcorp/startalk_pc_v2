//
// Created by cc on 18-11-5.
//

#include "GroupCard.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>
#include <QGridLayout>
#include <QFile>
#include <QEvent>
#include <QKeyEvent>
#include <QAction>
#include <QScrollBar>
#include "../CustomUi/HeadPhotoLab.h"
#include "CardManager.h"
#include "../include/ModButton.h"
#include "../include/Line.h"
#include "../CustomUi/LinkButton.h"
#include "../Platform/Platform.h"
#include "../QtUtil/Entity/JID.h"
#include "../CustomUi/QtMessageBox.h"

GroupCard::GroupCard(CardManager* cardManager)
    :UShadowDialog(cardManager, true)
    , _pCardManager(cardManager)
    , _moded(false)
{
    initUi();
}

GroupCard::~GroupCard()
{

}

void GroupCard::initUi()
{
    setObjectName("GroupCard");
#ifdef _MACOS
    macAdjustWindows();
#endif
    setMinimumWidth(360);

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
    //
    _pNameLabel = new QLabel(this);
    _pHeadLabel = new HeadPhotoLab;
    _pSendMailBtn = new QPushButton(this);
    _pExitGroupBtn = new QPushButton(this);
    _pDestroyGroupBtn = new QPushButton(this);
    //
    _pSendMailBtn->setToolTip(tr("发邮件"));
    _pExitGroupBtn->setToolTip(tr("退出群组"));
    _pDestroyGroupBtn->setToolTip(tr("销毁群"));
    _pDestroyGroupBtn->setObjectName("btn_destroy_group");
    //
    auto* btnLay = new QHBoxLayout;
    btnLay->addWidget(_pSendMailBtn);
    btnLay->addWidget(_pExitGroupBtn);
    btnLay->addWidget(_pDestroyGroupBtn);
	btnLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
	//
	_pSendMailBtn->setVisible(false);
    _pDestroyGroupBtn->setVisible(false);
    //
    auto* topMainLay = new QGridLayout;
    topMainLay->setContentsMargins(20, 5, 20, 20);
    topMainLay->setHorizontalSpacing(20);
    topMainLay->setVerticalSpacing(20);
    topMainLay->addWidget(_pHeadLabel, 0, 0);
    topMainLay->addWidget(_pNameLabel, 0, 1);
    topMainLay->addLayout(btnLay, 1, 1);
    topMainLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding), 0, 2, 2, 1);
    //
    QFrame* topFrame = new QFrame(this);
    topFrame->setFixedHeight(160);
    topFrame->setObjectName("TopFrame");
    auto* topLayout = new QVBoxLayout(topFrame);
    topLayout->setContentsMargins(0, 0, 0, 20);
    topLayout->addLayout(titleLayout);
    topLayout->addLayout(topMainLay);
    topLayout->setStretch(1, 1);
    //
    QFrame* bodyFrm = new QFrame(this);
    bodyFrm->setContentsMargins(6, 25, 6, 0);
    bodyFrm->setObjectName("BottomFrame");
    auto* bodyLay = new QGridLayout;
    bodyLay->setHorizontalSpacing(15);
    bodyLay->setVerticalSpacing(15);
    //row 0
    _modGroupName = new ModButton;
    _pGroupNameEdit = new QLineEdit;
	_pGroupNameEdit->setReadOnly(true);
    _pGroupNameEdit->setPlaceholderText(tr("修改群名称"));
	_pGroupNameEdit->setObjectName("GroupNameEdit");
	//
	auto *groupNameLay = new QHBoxLayout;
    groupNameLay->setMargin(0);
    groupNameLay->setSpacing(0);
    groupNameLay->addWidget(_pGroupNameEdit);
    groupNameLay->addWidget(_modGroupName);
	//
    bodyLay->addWidget(new QLabel(tr("群名称")), 0, 0);
    bodyLay->addLayout(groupNameLay, 0, 1, 1, 3);
    bodyLay->setAlignment(groupNameLay, Qt::AlignLeft);
//    bodyLay->addWidget(_modGroupName, 0, 3);
    // row 1
    _pGroupIdEdit = new QTextEdit(this);
    _pGroupIdEdit->setAcceptRichText(false);
    bodyLay->addWidget(new QLabel(tr("群ID")), 1, 0, Qt::AlignTop);
    bodyLay->addWidget(_pGroupIdEdit, 1, 1, 1, 3);
    // line
    bodyLay->addWidget(new Line, 2, 0, 1, 4);
    // row 3
    _modGroupTopic = new ModButton(this);
    bodyLay->addWidget(new QLabel(tr("群公告")), 3, 0);
    bodyLay->addWidget(_modGroupTopic, 3, 1);
    // row 4
    _pGroupTopicEdit = new QTextEdit(this);
    _pGroupTopicEdit->setAcceptRichText(false);
	_pGroupTopicEdit->setObjectName("GroupTopicEdit");
    bodyLay->addWidget(_pGroupTopicEdit, 4, 0, 1, 4);
    // row 5
    bodyLay->addWidget(new Line, 5, 0, 1, 4);
    // row 6
    QLabel *memberLabel = new QLabel(tr("群成员"));
    _pGroupMemberLabel = new QLabel;
    LinkButton* showMemberBtn = new LinkButton(tr("查看"));
    bodyLay->addWidget(memberLabel, 6, 0);
    bodyLay->addWidget(_pGroupMemberLabel, 6, 1);
    bodyLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding), 6, 2);
    bodyLay->addWidget(showMemberBtn, 6, 3);
    // row 7
    bodyLay->addWidget(new Line, 7, 0, 1, 4);
    // row 8
    _modGroupJJ = new ModButton(this);
    bodyLay->addWidget(new QLabel(tr("群简介")), 8, 0);
    bodyLay->addWidget(_modGroupJJ, 8, 1);
    // row 9
    _pGroupIntroduce = new QTextEdit(this);
    _pGroupIntroduce->setAcceptRichText(false);
	_pGroupIntroduce->setObjectName("GroupIntroduceEdit");
    bodyLay->addWidget(_pGroupIntroduce, 9, 0, 1, 4);
    //
    _pSendMsgBtn = new QPushButton(tr("发消息"));
    _pSendMsgBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _pSendMsgBtn->setObjectName("SendMessage");
    _pSendMsgBtn->setFixedHeight(32);
    //
    auto* bLayout = new QVBoxLayout(bodyFrm);
    bLayout->addLayout(bodyLay);
    bLayout->setSpacing(36);
    bLayout->addWidget(_pSendMsgBtn);
    //
    auto* mainLay = new QVBoxLayout(_pCenternWgt);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
    mainLay->addWidget(topFrame);
    mainLay->addWidget(bodyFrm);
    //
    _pSendMailBtn->setFixedSize(30, 30);
    _pExitGroupBtn->setFixedSize(30, 30);
    _pDestroyGroupBtn->setFixedSize(30, 30);
    _pHeadLabel->setFixedSize(60, 60);
    _pSendMailBtn->setObjectName("SendMail");
    _pExitGroupBtn->setObjectName("QuitGroup");
    _pNameLabel->setObjectName("NameLabel");
    //
    _pGroupIdEdit->installEventFilter(this);
    _pGroupTopicEdit->installEventFilter(this);
    _pGroupIntroduce->installEventFilter(this);
    _pGroupIdEdit->verticalScrollBar()->setVisible(false);
    _pGroupTopicEdit->verticalScrollBar()->setVisible(false);
    _pGroupIntroduce->verticalScrollBar()->setVisible(false);
    //
    _pGroupIdEdit->setReadOnly(true);
    _pGroupTopicEdit->setReadOnly(true);
    _pGroupIntroduce->setReadOnly(true);
    _pGroupIdEdit->setFrameShape(QFrame::NoFrame);
    _pGroupTopicEdit->setFrameShape(QFrame::NoFrame);
    _pGroupIntroduce->setFrameShape(QFrame::NoFrame);
    _pGroupIdEdit->setFixedHeight(55);
    _pGroupTopicEdit->setFixedHeight(55);
    _pGroupIntroduce->setFixedHeight(55);
    //
    _pGroupNameEdit->installEventFilter(this);
    _pGroupTopicEdit->installEventFilter(this);
    _pGroupIntroduce->installEventFilter(this);
    //
    setMoverAble(true, topFrame);
    _pGroupMemberPopWnd = new GroupMemberPopWnd(this);
    //
    connect(closeBtn, &QPushButton::clicked, [this](){this->close();});
    connect(showMemberBtn, &LinkButton::clicked, [this] (){
        QPoint pos(this->x() + this->width(), this->y());
        _pGroupMemberPopWnd->move(pos);
        _pGroupMemberPopWnd->showModel();
    });

    connect(_pSendMsgBtn, &QPushButton::clicked, this, &GroupCard::sendMessageSlot);
    connect(_modGroupName, &ModButton::clicked, [this](bool){
        _modGroupName->setVisible(false);
        _pGroupNameEdit->setReadOnly(false);
        _pGroupNameEdit->setFocusPolicy(Qt::ClickFocus);
        _pGroupNameEdit->setFocus();
    });
    connect(_modGroupTopic, &ModButton::clicked, [this](bool){
        _modGroupTopic->setVisible(false);
        _pGroupTopicEdit->setReadOnly(false);
        _pGroupTopicEdit->setFocusPolicy(Qt::ClickFocus);
        _pGroupTopicEdit->setFocus();
    });
    connect(_modGroupJJ, &ModButton::clicked, [this](bool){
        _modGroupJJ->setVisible(false);
        _pGroupIntroduce->setReadOnly(false);
        _pGroupIntroduce->setFocusPolicy(Qt::ClickFocus);
        _pGroupIntroduce->setFocus();
    });
    //
    connect(_pGroupNameEdit, &QLineEdit::textChanged, [this](){_moded = true;});
    connect(_pGroupTopicEdit, &QTextEdit::textChanged, [this](){_moded = true;});
    connect(_pGroupIntroduce, &QTextEdit::textChanged, [this](){_moded = true;});
    connect(this, &GroupCard::closeSignal, this, &GroupCard::onClose);

    connect(_pGroupNameEdit, &QLineEdit::textChanged, [this](){
        QString text = _pGroupNameEdit->text();
        if(!text.isEmpty())
        {
            QFontMetricsF tf(_pGroupNameEdit->font());
            int width = tf.width(text);
            width = qMin(width, 210);
            width = qMax(width, 50);
            _pGroupNameEdit->setFixedWidth(width + 10);
        }
    });

    connect(_pExitGroupBtn, &QPushButton::clicked, [this](){
        int ret = QtMessageBox::warning(this, tr("警告"), tr("即将退出本群, 是否继续?"),
                QtMessageBox::EM_BUTTON_YES | QtMessageBox::EM_BUTTON_NO);
        if(ret == QtMessageBox::EM_BUTTON_YES)
        {
            if(_pCardManager)
            {
                _pCardManager->quitGroup(_strGroupId);
                _moded = false;
                this->close();
            }
        }
    });

    connect(_pDestroyGroupBtn, &QPushButton::clicked, [this](){
        int ret = QtMessageBox::warning(this, tr("警告"), tr("群即将被销毁, 是否继续?"), QtMessageBox::EM_BUTTON_YES | QtMessageBox::EM_BUTTON_NO);
        if(ret == QtMessageBox::EM_BUTTON_YES)
        {
            if(_pCardManager)
            {
                _pCardManager->destroyGroup(_strGroupId);
                _moded = false;
                this->close();
            }
        }
    });
}

/**
 * data
 */
void GroupCard::setData(std::shared_ptr<QTalk::Entity::ImGroupInfo> &data)
{
    QString localHead = QString::fromStdString(QTalk::GetHeadPathByUrl(data->HeaderSrc));
    if(QFile::exists(localHead))
        _pHeadLabel->setHead(localHead, 30, false, true);

    QString name = QString::fromStdString(data->Name);
    QFontMetricsF f(_pNameLabel->font());
    name = f.elidedText(name, Qt::ElideRight, 200);

    _pNameLabel->setText(name);
    _pGroupNameEdit->setText(QString::fromStdString(data->Name));
    _pGroupIdEdit->setText(QString::fromStdString(data->GroupId));
    _pGroupTopicEdit->setText(QString::fromStdString(data->Topic));
    _pGroupIntroduce->setText(QString::fromStdString(data->Introduce));
    //
    _strGroupId = QString::fromStdString(data->GroupId);
    _srtHead = QString::fromStdString(data->HeaderSrc);
    _groupName = QString::fromStdString(data->Name);

    _moded = false;
}

/**
 * 显示群成员
 */
void GroupCard::showGroupMember(const std::map<std::string, QTalk::StUserCard>& userCards, const std::map<std::string, QUInt8>& userRole)
{
    int onlineNum = 0;
    //
    _pGroupMemberPopWnd->reset();
    auto it = userCards.begin();
    for(; it != userCards.end(); it++)
    {
        std::string name = it->second.nickName;
        if(name.empty())
            name = it->second.userName;
        if(name.empty())
            name = QTalk::Entity::JID(it->first.data()).username();

        QString xmppId = QString::fromStdString(it->first);
        QString userName = QString::fromStdString(name);
        QString headSrc = QString::fromStdString(QTalk::GetHeadPathByUrl(it->second.headerSrc));
        QString searchKey = QString::fromStdString(it->second.searchKey);

        QUInt8 role = 3;
        if(userRole.find(it->first) != userRole.end())
            role = userRole.at(it->first);
        //
        bool isOnline = Platform::instance().isOnline(QTalk::Entity::JID(it->first.c_str()).barename());

        if(role == 1)
        {
            std::string userId = QTalk::Entity::JID(it->first.c_str()).username();
            std::string selfId = Platform::instance().getSelfUserId();
            _pDestroyGroupBtn->setVisible(userId == selfId);
        }

        _pGroupMemberPopWnd->addItem(xmppId, userName, headSrc, role, isOnline, searchKey);

        if(isOnline)
            onlineNum++;
    }
    _pGroupMemberPopWnd->addEnd();
    _pGroupMemberLabel->setText(QString("( %1/ %2)").arg(onlineNum).arg(userCards.size()));
}

/**
 * 发送消息接口
 */
void GroupCard::sendMessageSlot()
{
    if(!_pCardManager)
        return;

    StSessionInfo stSession(QTalk::Enum::GroupChat, _strGroupId, _groupName);
    stSession.headPhoto = _srtHead;

    emit _pCardManager->sgSwitchCurFun(0);
    emit _pCardManager->sgOpenNewSession(stSession);
    if(_pGroupMemberPopWnd->isActiveWindow())
    {
        _pGroupMemberPopWnd->close();
    }
    this->close();
}

bool GroupCard::eventFilter(QObject *o, QEvent *e) {

    _pGroupIdEdit->installEventFilter(this);
    _pGroupTopicEdit->installEventFilter(this);
    _pGroupIntroduce->installEventFilter(this);
    if(o == _pGroupIdEdit ||
        o == _pGroupTopicEdit ||
        o == _pGroupIntroduce)
    {
        auto* edit = qobject_cast<QTextEdit*>(o);

        if(e->type() == QEvent::Enter)
        {
            edit->verticalScrollBar()->setVisible(true);
        }
        else if(e->type() == QEvent::Leave)
        {
            edit->verticalScrollBar()->setVisible(false);
        }
    }


    if(e->type() == QEvent::FocusOut)
    {
        if(o == _pGroupNameEdit)
        {
            _pGroupNameEdit->clearFocus();
            _pGroupNameEdit->setReadOnly(true);
        }
        else if(o == _pGroupTopicEdit || o == _pGroupIntroduce)
        {
            auto* textEdit = qobject_cast<QTextEdit*>(o);
            textEdit->clearFocus();
            textEdit->setReadOnly(true);
        }

        if(o == _pGroupNameEdit)
            _modGroupName->setVisible(true);
        if(o == _pGroupTopicEdit)
            _modGroupTopic->setVisible(true);
        if(o == _pGroupIntroduce)
            _modGroupJJ->setVisible(true);
    }
    else
    if(e->type() == QEvent::KeyPress)
    {
        auto* evt = (QKeyEvent*)e;
        if((evt->key() == Qt::Key_Return || evt->key() == Qt::Key_Enter))
        {

            int mod = evt->modifiers() & 0x0E000000;
            if(mod == 0x0E000000 || mod == 0)
            {
                if(o == _pGroupNameEdit)

                {
                    _pGroupNameEdit->clearFocus();
                    _pGroupNameEdit->setReadOnly(true);
                }
                else if(o == _pGroupTopicEdit || o == _pGroupIntroduce)
                {
                    auto* textEdit = qobject_cast<QTextEdit*>(o);
                    textEdit->clearFocus();
                    textEdit->setReadOnly(true);
                }

                if(o == _pGroupNameEdit)
                    _modGroupName->setVisible(true);
                if(o == _pGroupTopicEdit)
                    _modGroupTopic->setVisible(true);
                if(o == _pGroupIntroduce)
                    _modGroupJJ->setVisible(true);
            }
            else
            {
//                textEdit->textCursor().insertText("\n");
            }
        }
    }
    return QDialog::eventFilter(o, e);
}

bool GroupCard::event(QEvent* e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        if(_pGroupNameEdit->hasFocus() || _pGroupTopicEdit->hasFocus() || _pGroupIntroduce->hasFocus())
        {
            this->setFocus();
        }
    }

    return QDialog::event(e);
}

/**
 *
 */
void GroupCard::onClose() {

	if (_pGroupMemberPopWnd && _pGroupMemberPopWnd->isVisible())
	{
		_pGroupMemberPopWnd->setVisible(false);
	}
    if(_moded)
    {
        QString groupName = _pGroupNameEdit->text();
        QString groupTopic = _pGroupTopicEdit->toPlainText();
        QString groupDesc = _pGroupIntroduce->toPlainText();

        _pCardManager->updateGroupInfo(_strGroupId, groupName, groupTopic, groupDesc);
    }
}