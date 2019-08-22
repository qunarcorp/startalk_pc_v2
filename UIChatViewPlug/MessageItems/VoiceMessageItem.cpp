//
// Created by QITMAC000260 on 2019-05-08.
//

#include "VoiceMessageItem.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QFile>
#include <ChatViewMainPanel.h>
#include "../../CustomUi/HeadPhotoLab.h"
#include "../../QtUtil/lib/cjson/cJSON.h"
#include "../../QtUtil/lib/cjson/cJSON_inc.h"
#include "../../Platform/NavigationManager.h"
#include "../../Platform/Platform.h"
#include "../VoiceHelper.h"

extern ChatViewMainPanel *g_pMainPanel;
VoiceMessageItem::VoiceMessageItem(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent)
    :MessageItemBase(msgInfo, parent),
     _spaceFrm(nullptr),
     mainLay(nullptr){
    init();
}

void VoiceMessageItem::init() {
    initLayout();
}

void VoiceMessageItem::initLayout() {

    _contentSize = QSize(370, 160);
    _mainMargin = QMargins(15, 0, 20, 0);
    _mainSpacing = 10;
    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        _headPixSize = QSize(0, 0);
        _nameLabHeight = 0;
        _leftMargin = QMargins(0, 0, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 0;
        initSendLayout();
    } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
        _headPixSize = QSize(28, 28);
        _nameLabHeight = 16;
        _leftMargin = QMargins(0, 10, 0, 0);
        _rightMargin = QMargins(0, 0, 0, 0);
        _leftSpacing = 0;
        _rightSpacing = 4;
        initReceiveLayout();
    }
    if (QTalk::Enum::ChatType::GroupChat != _msgInfo.ChatType) {
        _nameLabHeight = 0;
    }
    setContentsMargins(0, 5, 0, 5);
}

QSize VoiceMessageItem::itemWdtSize() {
    int height = qMax(_mainMargin.top() + _nameLabHeight + _mainSpacing + _contentFrm->height() + _mainMargin.bottom(),
                      _headPixSize.height()); // 头像和文本取大的
    int width = _contentFrm->width();
    if(nullptr != _readStateLabel)
    {
        height += 12;
    }
    return {width, height + 8};
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void VoiceMessageItem::initSendLayout() {

    secondsLabel = new QLabel(this);
    secondsLabel->setObjectName("SecondsLabel");

    mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    auto *horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLay->addItem(horizontalSpacer);
    mainLay->addWidget(secondsLabel);
    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    mainLay->addLayout(rightLay);
    if (!_contentFrm) {
        _contentFrm = new QFrame(this);
    }
    _contentFrm->setObjectName("messSendContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
    //
    auto* tmpLay = new QHBoxLayout;
    tmpLay->setMargin(0);
    tmpLay->setSpacing(5);
    if(nullptr == _sending)
    {
        _sending = new HeadPhotoLab(":/chatview/image1/messageItem/loading.gif", 10, false, false, true, this);
        tmpLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));
        tmpLay->addWidget(_sending);
        bool startMovie = (0 == _msgInfo.ReadedTag && 0 == _msgInfo.State);
        _sending->setVisible(startMovie);
        if(startMovie)
            _sending->startMovie();
    }
    if(nullptr != _resending)
    {
        tmpLay->addWidget(_resending);
        _resending->setVisible(false);
    }
    tmpLay->addWidget(_contentFrm);
    tmpLay->setAlignment(_contentFrm, Qt::AlignRight);
    rightLay->addLayout(tmpLay);
    if (nullptr != _readStateLabel) {
        auto *rsLay = new QHBoxLayout;
        rsLay->setMargin(0);
        rsLay->addWidget(_readStateLabel);
        rsLay->setAlignment(_readStateLabel, Qt::AlignRight);
        rightLay->addLayout(rsLay);
    }
    mainLay->setAlignment(secondsLabel, Qt::AlignRight | Qt::AlignTop);
    secondsLabel->setContentsMargins(0, 10, 0, 0);
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 0);

    initContentLayout();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.19
  */
void VoiceMessageItem::initReceiveLayout() {

    secondsLabel = new QLabel(this);
    secondsLabel->setObjectName("SecondsLabel");

    mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(_mainMargin);
    mainLay->setSpacing(_mainSpacing);
    mainLay->addWidget(_btnShareCheck);
    auto *leftLay = new QVBoxLayout;
    leftLay->setContentsMargins(_leftMargin);
    leftLay->setSpacing(_leftSpacing);
    mainLay->addLayout(leftLay);
    if (!_headLab) {
        _headLab = new HeadPhotoLab;
    }
    _headLab->setFixedSize(_headPixSize);
    _headLab->setHead(QString::fromStdString(_msgInfo.HeadSrc), HEAD_RADIUS);
    _headLab->installEventFilter(this);
    leftLay->addWidget(_headLab);
    auto *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    leftLay->addItem(vSpacer);

    leftLay->setStretch(0, 0);
    leftLay->setStretch(1, 1);

    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(_rightMargin);
    rightLay->setSpacing(_rightSpacing);
    mainLay->addLayout(rightLay);

    if (QTalk::Enum::ChatType::GroupChat == _msgInfo.ChatType
        && QTalk::Entity::MessageDirectionReceive == _msgInfo.Direction ) {
        rightLay->addWidget(_nameLab);
    }
    if (!_contentFrm) {
        _contentFrm = new QFrame(this);
    }
    _contentFrm->setObjectName("messReceiveContentFrm");
    _contentFrm->setFixedWidth(_contentSize.width());
    auto* contentLay = new QHBoxLayout;
    contentLay->setMargin(0);
    contentLay->setSpacing(10);
    contentLay->addWidget(_contentFrm);
    contentLay->addWidget(secondsLabel);
    contentLay->setAlignment(secondsLabel, Qt::AlignLeft | Qt::AlignTop);
    secondsLabel->setContentsMargins(0, 10, 0, 0);
    rightLay->addLayout(contentLay);
    rightLay->setStretch(0, 0);
    rightLay->setStretch(1, 1);
    initContentLayout();
    mainLay->addItem(new QSpacerItem(40, 1, QSizePolicy::Expanding));
}

/**
 *
 */
void VoiceMessageItem::initContentLayout() {

    _contentFrm->installEventFilter(this);
    auto* contentLay = new QHBoxLayout(_contentFrm);
    contentLay->setMargin(10);
    contentLay->setSpacing(0);
    //
    pixLabel = new HeadPhotoLab();
    //
    _spaceFrm = new QFrame(this);
    _spaceFrm->setMaximumWidth(300);
    //

    //
    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        contentLay->addWidget(_spaceFrm);
        contentLay->addWidget(pixLabel);
        pixLabel->setHead(":chatview/image1/messageItem/voice_message_3_s.png", 10, false, false, true);
    } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
        contentLay->addWidget(pixLabel);
        contentLay->addWidget(_spaceFrm);
        pixLabel->setHead(":chatview/image1/messageItem/voice_message_3.png", 10, false, false, true);
    }
    pixLabel->setAlignment(Qt::AlignVCenter);
    //
    cJSON* json = cJSON_Parse(_msgInfo.Content.data());
    if(nullptr == json)
    {
        _contentFrm->setFixedWidth(50);
    }
    else
    {
        std::string _voicePath = QTalk::JSON::cJSON_SafeGetStringValue(json, "HttpUrl");
        local_path = _msgInfo.MsgId;
        int seconds = QTalk::JSON::cJSON_SafeGetIntValue(json, "Seconds");
        secondsLabel->setText(QString("%1''").arg(seconds));
        _spaceFrm->setFixedWidth(qMin(seconds * 15, 300));
        _contentFrm->setFixedWidth(qMin(seconds * 15, 300) + 30);
        //
        local_path = Platform::instance().getAppdataRoamingUserPath() + "/voice/" + local_path + ".amr";
        //
        std::thread([this, _voicePath](){
            if(g_pMainPanel)
            {
                g_pMainPanel->getMessageManager()->sendDownLoadFile(local_path, _voicePath, _msgInfo.MsgId);
#ifdef Q_NO_AMR
				bool isok = QTalk::VoiceHelper::amrToWav(local_path + ".amr", local_path + ".wav");
				if (isok)
					local_path = local_path + ".wav";
#endif // Q_NO_AMR

            }
        }).detach();

    }
    _contentFrm->setFixedHeight(40);
    //
    _pTimer = new QTimer(this);
    _pTimer->setInterval(300);
    connect(_pTimer, &QTimer::timeout, [this](){
        static unsigned char num = 0;

        int index = num++ % 3 + 1;
        QString fileName;
        if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
            fileName = QString(":chatview/image1/messageItem/voice_message_%1_s.png").arg(index);
        } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
            fileName = QString(":chatview/image1/messageItem/voice_message_%1.png").arg(index);
        }
        pixLabel->setHead(fileName, 10, false, false, true);
    });
}

void VoiceMessageItem::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && _contentFrm->geometry().contains(e->pos()))
    {
       if(g_pMainPanel && QFile::exists(local_path.data()))
       {
           g_pMainPanel->playVoice(local_path, this);

           if(!_pTimer->isActive())
               _pTimer->start();
       }
    }

    QWidget::mousePressEvent(e);
}

/**
 *
 * @param o
 * @param e
 * @return
 */
bool VoiceMessageItem::eventFilter(QObject* o, QEvent* e)
{
    if(o == _contentFrm)
    {
        if(e->type() == QEvent::Enter)
        {
            setCursor(Qt::PointingHandCursor);
        }
        else if(e->type() == QEvent::Leave)
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    return MessageItemBase::eventFilter(o, e);
}

void VoiceMessageItem::stopVoice()
{
    _pTimer->stop();
    if (QTalk::Entity::MessageDirectionSent == _msgDirection) {
        pixLabel->setHead(":chatview/image1/messageItem/voice_message_3_s.png", 10, false, false, true);
    } else if (QTalk::Entity::MessageDirectionReceive == _msgDirection) {
        pixLabel->setHead(":chatview/image1/messageItem/voice_message_3.png", 10, false, false, true);
    }
}