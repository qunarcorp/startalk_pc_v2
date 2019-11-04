//
// Created by QITMAC000260 on 2018/11/29.
//

#include "MessagePrompt.h"
#include "ChatViewMainPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QPropertyAnimation>
#include "../CustomUi/HeadPhotoLab.h"
#include "../Platform/Platform.h"
#include "../Platform/dbPlatForm.h"
#include "../QtUtil/Entity/JID.h"
#include "../UICom/uicom.h"

extern ChatViewMainPanel *g_pMainPanel;
MessagePrompt::MessagePrompt(const QTalk::Entity::ImMessageInfo& msg)
    : QFrame(nullptr), _msg(msg)
    , _timer(nullptr), animation(nullptr)
{
    setFocusPolicy(Qt::NoFocus);

    initUi();
    //initQss();
    //
    animation = new QPropertyAnimation(this, "pos");
    //
    dealTimer();
}

MessagePrompt::~MessagePrompt()
{
    if(_timer)
    {
        delete _timer;
        _timer = nullptr;
    }
    if(animation)
    {
        delete animation;
        animation = nullptr;
    }
}

void MessagePrompt::initUi()
{

    Qt::WindowFlags flags =  Qt::WindowDoesNotAcceptFocus | Qt::WindowContextHelpButtonHint | Qt::FramelessWindowHint
                            | Qt::WindowFullscreenButtonHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint |
                             Qt::WindowStaysOnTopHint;

#ifdef _WINDOWS
    flags |= Qt::Tool;
#else
    flags |= Qt::Widget;
#endif

    setWindowFlags(flags);
	// 设置不抢主窗口焦点
	setAttribute(Qt::WA_X11DoNotAcceptFocus, true);
	setAttribute(Qt::WA_ShowWithoutActivating, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setFixedSize(300, 80);
    //
    QFrame* mainFrm = new QFrame(this);
    mainFrm->setObjectName("MessagePromptMainFrm");
    //
    _pLabelHead = new HeadPhotoLab;
    _pLabelHead->setParent(this);
    _pLabelName = new QLabel(this);
    _pLabelContent = new QLabel(this);
    _pLabelName->setObjectName("MessagePromptName");
    _pLabelContent->setObjectName("MessagePromptContent");
    _pLabelContent->setAlignment(Qt::AlignTop);
    _pLabelHead->setFixedWidth(34);

    auto * mainLay = new QHBoxLayout(mainFrm);
    auto * rifghtLay = new QVBoxLayout;

    rifghtLay->setSpacing(5);
    rifghtLay->addWidget(_pLabelName);
    rifghtLay->addWidget(_pLabelContent);
    mainLay->addWidget(_pLabelHead);
    mainLay->addLayout(rifghtLay);
   //
    auto * layout = new QHBoxLayout(this);
    layout->setMargin(6);
    layout->addWidget(mainFrm);
    //
    QString headPath = "";

    //
	QTalk::Entity::JID jid(_msg.SendJid.data());
    if(_msg.ChatType == QTalk::Enum::GroupChat)
    {
        //
        std::shared_ptr<QTalk::Entity::ImGroupInfo> groupInfo = dbPlatForm::instance().getGroupInfo(jid.barename(), true);
        QString strHead = "";
        if(nullptr != groupInfo)
        {
            _pLabelName->setText(QString::fromStdString(groupInfo->Name));
            strHead = QString::fromStdString(groupInfo->HeaderSrc);
        }

        if(!strHead.isEmpty())
        {
            headPath = QString::fromStdString(QTalk::GetHeadPathByUrl(strHead.toStdString()));
        }
        if(headPath.isEmpty() || !QFile::exists(headPath))
        {
            headPath = ":/chatview/image1/defaultGroupHead.png";
#ifdef _STARTALK
            headPath = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
            headPath = ":/QTalk/image1/defaultGroupHead.png";
#endif
        }
    }
    else if(_msg.ChatType == QTalk::Enum::System)
    {
        headPath = ":/chatview/image1/system.png";
        _pLabelName->setText(tr("系统消息"));
    }
    else
    {
        std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo = dbPlatForm::instance().getUserInfo(jid.barename());
        QString strHead = "";
        if(nullptr != userInfo)
        {
            _pLabelName->setText(QString::fromStdString(QTalk::getUserName(userInfo)));
            strHead = QString::fromStdString(userInfo->HeaderSrc);
        }

        if(!strHead.isEmpty())
        {
            headPath = QString::fromStdString(QTalk::GetHeadPathByUrl(strHead.toStdString()));
        }
        if(headPath.isEmpty() || !QFile::exists(headPath))
        {
#ifdef _STARTALK
            headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
            headPath = ":/QTalk/image1/headPortrait.png";
#endif
        }
    }
    _pLabelHead->setHead(headPath, 17);
    _pLabelContent->setText(dealMessageContent());

    this->setStyleSheet("QFrame#MessagePromptMainFrm{background:rgba(255,255,255,1); "
                        "border: 1px solid rgb(245, 245, 245); "
                        "border-radius:6px;}");
}

void MessagePrompt::startToshow(const QPoint& startPoint)
{
    // "pos" or “geometry” or “windowOpacity”
    animation->setDuration(500);
    animation->setStartValue(startPoint);
    animation->setEndValue(QPoint(startPoint.x() - 350, startPoint.y()));
    animation->setEasingCurve(QEasingCurve::InOutQuart);
    animation->start();
    //
    _timer->start();
}

//
void MessagePrompt::stopAndDelete() {

    animation->setDuration(300);
    QPoint startPos = geometry().topLeft();
    animation->setStartValue(startPos);
    animation->setEndValue(QPoint(startPos.x() + 350, startPos.y()));
    animation->setEasingCurve(QEasingCurve::InOutQuart);
    animation->start();

    QTimer::singleShot(310, [this](){
        g_pMainPanel->removePrompt(this);
    });
}

//
void MessagePrompt::moveTop() {

    animation->setDuration(100);
    QPoint startPos = geometry().topLeft();
    animation->setStartValue(startPos);
    animation->setEndValue(QPoint(startPos.x(), startPos.y() - 80));
    animation->setEasingCurve(QEasingCurve::InOutQuart);
    animation->start();
}

void MessagePrompt::dealTimer() {
    _timer = new QTimer;
    _timer->setSingleShot(true);
    _timer->setInterval(3000);
    connect(_timer, &QTimer::timeout, this, &MessagePrompt::stopAndDelete);
}

QString MessagePrompt::dealMessageContent() {

    QString ret = "";
    if (_msg.ChatType == QTalk::Enum::GroupChat && !_msg.UserName.empty()) {
        ret += QString(_msg.UserName.data()) + ": ";
    }

    switch (_msg.Type)
    {

	case QTalk::Entity::MessageTypeFile:
		ret += tr("[文件]");
		break;
    case QTalk::Entity::MessageTypePhoto:
        ret += tr("[图片]");
        break;
	case QTalk::Entity::MessageTypeImageNew:
		ret += tr("[表情]");
		break;
    case QTalk::Entity::MessageTypeSmallVideo:
        ret += tr("[视频]");
        break;
    case QTalk::Entity::WebRTC_MsgType_VideoCall:
        ret += tr("[实时视频]");
        break;
    case QTalk::Entity::WebRTC_MsgType_AudioCall:
        ret += tr("[实时音频]");
        break;
    case QTalk::Entity::WebRTC_MsgType_Video_Group:
        ret += tr("[群组视频]");
        break;
	case QTalk::Entity::MessageTypeCommonTrdInfo:
	case QTalk::Entity::MessageTypeCommonTrdInfoV2:
		ret += tr("[链接卡片]");
		break;
    case QTalk::Entity::MessageTypeSourceCode:
        ret += tr("[代码块]");
        break;
    case QTalk::Entity::MessageTypeVoice:
        ret += tr("[语音]");
        break;
    case QTalk::Entity::MessageTypeProduct:
    case QTalk::Entity::MessageTypeNote:
        ret += tr("[产品详情]");
        break;
    case QTalk::Entity::MessageTypeSystem:
        ret += tr("[系统消息]");
        break;
    case QTalk::Entity::MessageTypeNotice:
        ret += tr("[公告消息]");
        break;
     case QTalk::Entity::MessageTypeGrabMenuVcard:
     case QTalk::Entity::MessageTypeGrabMenuResult:
         ret += tr("[抢单消息]");
         break;
    case 65537:
    case 65538:
        ret += tr("[热线提示信息]");
        break;
    default:
    case QTalk::Entity::MessageTypeText:
    case QTalk::Entity::MessageTypeGroupAt:
    case QTalk::Entity::MessageTypeRobotAnswer:
        {
            QString tmpContent = QString::fromStdString(_msg.Content).split("\n").first();

            QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");
            regExp.setMinimal(true);

            int pos = 0;
            while ((pos = regExp.indexIn(tmpContent)) != -1) {
                QString item = regExp.cap(0); // 符合条件的整个字符串
                QString type = regExp.cap(1); // 多媒体类型

                if ("url" == type) {
                    tmpContent.replace(pos, item.size(), tr("[链接]"));
                } else if ("image" == type) {
                    tmpContent.replace(pos, item.size(), tr("[图片]"));
                } else if ("emoticon" == type) {
                    tmpContent = tr("[表情]");
                } else {
                    tmpContent.replace(pos, item.size(), tr("[未知类型]"));
                }
            }

            ret += tmpContent;
            break;
        }
    }

    return ret;
}

void MessagePrompt::mousePressEvent(QMouseEvent *e) {

    if(_pLabelName)
    {
		QTalk::Entity::JID jid(_msg.SendJid.data());
        QString strName = _pLabelName->text();
        emit openChatWnd(_msg.ChatType, QString::fromStdString(jid.barename()),
                QString::fromStdString(_msg.RealJid), _pLabelName->text(), g_pMainPanel->getSelfUserId().data());
    }

    QWidget::mousePressEvent(e);
}
