#include "MessageItemBase.h"
#include <QLabel>
#include <QtCore/QEvent>
#include "ChatViewMainPanel.h"
#include <QDebug>
#include <QFileInfo>
#include <QTimer>
#include "../../CustomUi/HeadPhotoLab.h"
#include "../QtUtil/Entity/JID.h"
#include "../ChatMainWgt.h"
#include "../InputWgt.h"
#include "../../Platform/NavigationManager.h"
#include "../../Platform/Platform.h"
#include "../../CustomUi/QtMessageBox.h"

extern ChatViewMainPanel* g_pMainPanel;
MessageItemBase::MessageItemBase(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent) :
    QFrame(parent),
    _pParentWgt(parent),
    _headLab(nullptr),
    _nameLab(nullptr),
    _readStateLabel(nullptr),
    _contentFrm(nullptr),
    _btnShareCheck(nullptr),
    _isPressEvent(false),
    _alreadyRelease(false),
    _medalWgt(nullptr),
    _readSts(0)
{
	QTalk::Entity::JID userId(msgInfo.From.data());
    QTalk::Entity::JID realJid(msgInfo.RealJid.data());
	_msgInfo = msgInfo;
	_msgDirection = msgInfo.Direction;
	_strUserId = QString::fromStdString(userId.barename());
    _strRealJid = QString::fromStdString(realJid.barename());
	_type = msgInfo.Type;
    _btnShareCheck = new QToolButton(this);
    _btnShareCheck->setCheckable(true);
    _btnShareCheck->setFixedSize(20, 20);
    _btnShareCheck->setObjectName("ShareCheck");
    connect(_btnShareCheck, &QToolButton::clicked, this, &MessageItemBase::sgSelectItem);

    if (QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType
        && QTalk::Entity::MessageDirectionReceive == _msgInfo.Direction ) {
        if (!_nameLab) {
            _nameLab = new QLabel(this);
        }
        _nameLab->setObjectName("chatitemnameLab");
        _nameLab->setFixedHeight(16);
        _nameLab->setTextFormat(Qt::PlainText);
        _nameLab->installEventFilter(this);
        QString nameText = QString::fromStdString(_msgInfo.UserName);
        _nameLab->setText(nameText);
//        std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo
//                = dbPlatForm::instance().getUserInfo(_msgInfo.From);
//        changeUserMood(userInfo ? userInfo->Mood : "");
        //
        updateUserMedal();
    }

	QFileInfo fInfo(QString::fromStdString(_msgInfo.HeadSrc));
	if (_msgInfo.HeadSrc.empty() || !fInfo.exists() || fInfo.isDir())
	{
#ifdef _STARTALK
        _msgInfo.HeadSrc = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
        _msgInfo.HeadSrc = ":/QTalk/image1/headPortrait.png";
#endif
	}

	if (NavigationManager::instance().isShowmsgstat() && (_msgInfo.ChatType == QTalk::Enum::TwoPersonChat || _msgInfo.ChatType == QTalk::Enum::Consult || _msgInfo.ChatType == QTalk::Enum::ConsultServer))
	{
		_readSts = EM_SENDDING;
		_readStateLabel = new QLabel;
		_readStateLabel->setContentsMargins(0, 0, 0, 0);
		_readStateLabel->setObjectName("ReadStateLabel");
//		if(0 == _msgInfo.ReadedTag && 0 == _msgInfo.State)
//		    _readStateLabel->setText("发送中...");
	}

	if(QTalk::Entity::MessageDirectionSent == _msgInfo.Direction)
    {
        _resending = new HeadPhotoLab;
        _resending->setParent(this);
        _resending->setHead(":/chatview/image1/error.png", 8, false, false, true);
        _resending->installEventFilter(this);
    }

	qRegisterMetaType<std::string>("std::string");
//	connect(this, &MessageItemBase::sgChangeUserMood, this, &MessageItemBase::changeUserMood, Qt::QueuedConnection);
	connect(this, &MessageItemBase::sgDisconnected, this, &MessageItemBase::onDisconnected, Qt::QueuedConnection);
	connect(this, &MessageItemBase::sgUpdateUserMedal, this, &MessageItemBase::updateUserMedal, Qt::QueuedConnection);
}

MessageItemBase::~MessageItemBase()
{
	//if (_readStateLabel)
	//{
	//	delete _readStateLabel;
	//	_readStateLabel = nullptr;
	//}
}

bool MessageItemBase::eventFilter(QObject *o, QEvent *e)
{
    if((o == _headLab || (_nameLab && o == _nameLab)) && g_pMainPanel)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            if(msgInfo().ChatType == QTalk::Enum::GroupChat)
            {
                _isPressEvent = false;

                StSessionInfo sessionInfo;
                sessionInfo.userId = QString::fromStdString(_msgInfo.From);
                sessionInfo.chatType = QTalk::Enum::TwoPersonChat;
                emit g_pMainPanel->sgOpenNewSession(sessionInfo);
            }

        }
        else if(e->type() == QEvent::MouseButtonPress)
        {
            _isPressEvent = true;
            _alreadyRelease = false;
            QTimer::singleShot(350, [this](){
                if(_isPressEvent){

                    if(!_alreadyRelease && _msgInfo.ChatType == QTalk::Enum::GroupChat)
                    {
                        auto* mainWgt = qobject_cast<ChatMainWgt*>(_pParentWgt);
                        if(nullptr != mainWgt)
                        {
                            mainWgt->_pViewItem->_pInputWgt->insertAt(_msgInfo.From);
                        }
                    }
                    else
                    {
                        if(msgInfo().ChatType == QTalk::Enum::ChatType::ConsultServer){
                            g_pMainPanel->showUserCard(_strRealJid);
                        } else{
                            g_pMainPanel->showUserCard(_strUserId);
                        }
                    }
                }
            });
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            _alreadyRelease = true;
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
    else if(o == _resending)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            if(g_pMainPanel && g_pMainPanel->getConnectStatus())
            {
                //
                int ret = QtMessageBox::question(g_pMainPanel, tr("提醒"), tr("确认重发此消息？"));
                if(ret == QtMessageBox::EM_BUTTON_YES)
                {
                    //
                    if(_sending)
                    {
                        _sending->setVisible(true);
                        _sending->startMovie();
                    }
                    if(_resending)
                        _resending->setVisible(false);

                    g_pMainPanel->resendMessage(this);
                }
            }
        }
    }

    return QObject::eventFilter(o, e);
}

void MessageItemBase::setReadState(const QInt32& state)
{
    if (_readSts == EM_BLACK_RET) {
        return;
    }
    //
    if(_readSts > state)
        return;
    _readSts = state;
    if(state > 0)
    {
        if(_sending)
        {
            _sending->deleteLater();
            _sending = nullptr;
        }
        if(_resending)
        {
            _resending->deleteLater();
            _resending = nullptr;
        }
    }


//    if(state == EM_BLACK_RET)
//    {
//        _readStateLabel->setText("发送失败");
//        _readStateLabel->setStyleSheet("color:rgba(255, 48, 48, 1)");
//        return;
//    }

    if (_readStateLabel == nullptr) {
        return;
    }

    if(state & EM_READSTS_READED)
    {
        _readStateLabel->setText(tr("已读"));
        _readStateLabel->setStyleSheet("color:rgba(181, 181, 181, 1)");
    }
    else if(state & EM_READSTS_UNREAD)
    {
        _readStateLabel->setText(tr("未读"));
        _readStateLabel->setStyleSheet("color:rgba(0, 195, 188, 1)");
    }
}

bool MessageItemBase::contains(const QPoint& pos)
{
    return _contentFrm->geometry().contains(mapFromGlobal(pos));
}

void MessageItemBase::showShareCheckBtn(bool show)
{
    _btnShareCheck->setVisible(show);
}

void MessageItemBase::checkShareCheckBtn(bool check)
{
    _btnShareCheck->setChecked(check);
}

/**
 *
 */
void MessageItemBase::changeUserMood(const std::string& mood)
{
    if(QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType
       && QTalk::Entity::MessageDirectionReceive == _msgInfo.Direction
       && _nameLab)
    {
        QString nameText = QString::fromStdString(_msgInfo.UserName);
        QString strMood = QString::fromStdString(mood).trimmed().replace(QRegExp("\\s{1,}"), " ");
        if(!strMood.isEmpty() && mood != "这个人很懒，什么都没留下")
        {
            nameText.append(QString(tr("「 %1 」")).arg(strMood));
        }
        _nameLab->setToolTip(mood.data());
        nameText = nameText.replace("\n", " ");
        nameText = QFontMetricsF(_nameLab->font()).elidedText(nameText, Qt::ElideRight, 350);
        _nameLab->setText(nameText);
    }
}

void MessageItemBase::onDisconnected() {

    if(!Platform::instance().isMainThread())
    {
        emit sgDisconnected();
        return;
    }

    if(0 == _readSts)
    {
        if(_sending)
        {
            _sending->setVisible(false);
            _sending->stopMovie();
        }
        if(_resending)
            _resending->setVisible(true);
    }
}

void MessageItemBase::updateUserMedal() {
    if (QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType
        && QTalk::Entity::MessageDirectionReceive == _msgInfo.Direction ) {

        if(nullptr == _medalWgt)
            _medalWgt = new MedalWgt(18, this);
        std::set<QTalk::StUserMedal> user_medal;
        g_pMainPanel->getUserMedal(_msgInfo.From, user_medal);
        _medalWgt->addMedals(user_medal, true);
    }
}