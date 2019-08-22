#include "ToolWgt.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include "InputWgt.h"

#include "../UICom/uicom.h"
#include "ChatViewMainPanel.h"
#include "../Emoticon/EmoticonMainWgt.h"
#include "../Platform/Platform.h"
#include "../WebService/AudioVideo.h"
#include "../QtUtil/Entity/JID.h"
#include "../WebService/WebService.h"
#include "../Platform/NavigationManager.h"
#include "../CustomUi/LiteMessageBox.h"
#include "../CustomUi/QtMessageBox.h"
#include "search/LocalSearchMainWgt.h"
#ifdef _MACOS
#include "../Screenshot/mac/SnipScreenTool.h"
#else
#include "../Screenshot/SnipScreenTool.h"
#endif


#define DEM_BTN_ICON_LEN  30
extern ChatViewMainPanel* g_pMainPanel;
ToolWgt::ToolWgt(InputWgt* pInputWgt, ChatViewItem* chatItem)
    :QFrame(), _pInputWgt(pInputWgt), _pChatItem(chatItem)
{
	//
	initUi();

}


ToolWgt::~ToolWgt()
{
}

/**
  * @函数名   initUi
  * @功能描述 初始化画面
  * @参数
  * @author   cc
  * @date     2018/09/19
  */
void ToolWgt::initUi()
{
	this->setObjectName("ToolWgt");

    _pSessionTransferWnd = new SessionTransferWnd(this);
    _pSendProductWnd = new SendProductWnd(this);
    _pQuickReplyWnd = new QuickReplyWnd(this);

#ifdef _QCHAT
    qchatMoreFun = new QchatMoreFun(_pChatItem->getPeerId(),_pChatItem->_chatType);
#endif

	//
	_pBtnEmoticon = new QPushButton("", this);
	_pBtnScreenshot = new QPushButton("", this);
	_pBtnScreenshotSet = new QPushButton("", this);
	_pBtnFile = new QPushButton("", this);
    _pBtnCode = new QPushButton("", this);
    _pBtnMultifunction = new QPushButton(this);
	//
    _pBtnEmoticon->setFocusPolicy(Qt::NoFocus);
    _pBtnScreenshot->setFocusPolicy(Qt::NoFocus);
    _pBtnScreenshotSet->setFocusPolicy(Qt::NoFocus);
    _pBtnFile->setFocusPolicy(Qt::NoFocus);
    _pBtnCode->setFocusPolicy(Qt::NoFocus);
    _pBtnMultifunction->setFocusPolicy(Qt::NoFocus);
    //
    _pMenu = new QMenu(this);
    _pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _pScreenShotHideWnd = new QAction("截屏时隐藏当前窗口", _pMenu);
    _pScreenShotHideWnd->setCheckable(true);
    _pMenu->addAction(_pScreenShotHideWnd);

    //
    _pBtnEmoticon->setObjectName("Emoticon");
    _pBtnScreenshot->setObjectName("Screenshot");
    _pBtnScreenshotSet->setObjectName("ScreenshotSet");
    _pBtnFile->setObjectName("File");
    _pScreenShotHideWnd->setObjectName("ScreenShotHideWndAction");
    _pBtnCode->setObjectName("CodeBtn");
    _pBtnMultifunction->setObjectName("Multifunction");
    //
    _pBtnEmoticon->setToolTip(tr("表情"));
    _pBtnScreenshot->setToolTip(tr("截图"));
    _pBtnFile->setToolTip(tr("发送文件或图片"));
    _pBtnCode->setToolTip(tr("发送代码"));
    _pBtnMultifunction->setToolTip(tr("多功能"));
    //
    _pBtnEmoticon->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnScreenshot->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnScreenshotSet->setFixedSize(10, 10);
    _pBtnFile->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnCode->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnMultifunction->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);

    _pBtnVideo = new QPushButton("", this);
    _pBtnVideo->setFocusPolicy(Qt::NoFocus);
    _pBtnVideo->setObjectName("Video");
    _pBtnVideo->setToolTip("实时视频");
    _pBtnVideo->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);

    _pBtnHistory = new QPushButton("", this);
    _pBtnHistory->setFocusPolicy(Qt::NoFocus);
    _pBtnHistory->setObjectName("History");
    _pBtnHistory->setToolTip("历史消息");
    _pBtnHistory->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnVideo->setVisible(_pChatItem->_chatType == QTalk::Enum::TwoPersonChat);

    _pBtnShock = new QPushButton("", this);
    _pBtnShock->setFocusPolicy(Qt::NoFocus);
    _pBtnShock->setObjectName("Shock");
    _pBtnShock->setToolTip("窗口抖动");
    _pBtnShock->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnShock->setVisible(_pChatItem->_chatType == QTalk::Enum::TwoPersonChat);

    _pBtnCloseService = new QPushButton("",this);
    _pBtnCloseService->setFocusPolicy(Qt::NoFocus);
    _pBtnCloseService->setObjectName("CloseServer");
    _pBtnCloseService->setToolTip("挂断");
    _pBtnCloseService->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnCloseService->setVisible(_pChatItem->_chatType == QTalk::Enum::ConsultServer);

    _pBtnTransfer = new QPushButton("",this);
    _pBtnTransfer->setFocusPolicy(Qt::NoFocus);
    _pBtnTransfer->setObjectName("Transfer");
    _pBtnTransfer->setToolTip("会话转移");
    _pBtnTransfer->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
    _pBtnTransfer->setVisible(_pChatItem->_chatType == QTalk::Enum::ConsultServer);

    _pBtnQuickReply = new QPushButton("",this);
    _pBtnQuickReply->setFocusPolicy(Qt::NoFocus);
    _pBtnQuickReply->setObjectName("QuickReply");
    _pBtnQuickReply->setToolTip("快捷回复");
    _pBtnQuickReply->setFixedSize(DEM_BTN_ICON_LEN, DEM_BTN_ICON_LEN);
#ifdef _QCHAT
    _pBtnQuickReply->setVisible(_pChatItem->_chatType == QTalk::Enum::ConsultServer);
#else
    _pBtnQuickReply->setVisible(false);
#endif

//#ifdef _QCHAT
//    _pBtnCloseService->setVisible(_pChatItem->_chatType == QTalk::Enum::ConsultServer);
//    _pBtnTransfer->setVisible(_pChatItem->_chatType == QTalk::Enum::ConsultServer);
//#else
//    _pBtnCloseService->setVisible(false);
//    _pBtnTransfer->setVisible(false);
//#endif

    //
    auto * screenShotLayout = new QHBoxLayout;
	screenShotLayout->setMargin(0);
	screenShotLayout->setSpacing(0);
	screenShotLayout->addWidget(_pBtnScreenshot);
	screenShotLayout->addWidget(_pBtnScreenshotSet);
    auto * layout = new QHBoxLayout(this);
	layout->setSpacing(13);
	layout->addWidget(_pBtnEmoticon);
	layout->addLayout(screenShotLayout);
	layout->addWidget(_pBtnFile);
    layout->addWidget(_pBtnMultifunction);
    layout->addWidget(_pBtnVideo);
    layout->addWidget(_pBtnCode);
    layout->addWidget(_pBtnHistory);
    layout->addWidget(_pBtnShock);
    layout->addWidget(_pBtnQuickReply);

    auto* rightLayout = new QHBoxLayout;
    rightLayout->addWidget(_pBtnCloseService,1,Qt::AlignmentFlag::AlignRight);
    rightLayout->addWidget(_pBtnTransfer,Qt::AlignmentFlag::AlignRight);
    layout->addLayout(rightLayout,1);

	layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed));
	//
	setLayout(layout);
	//
    _pMultiMenu = new QMenu(this);
    _pMultiMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _pMultiMenu->setAttribute(Qt::WA_TranslucentBackground, true);

    auto* actionVote = new QAction(tr("投票"), this);
    auto* actionTask = new QAction(tr("任务系统"), this);
    auto* scanQRCode = new QAction(tr("扫一扫"), this);
    auto* actionPrud = new QAction(tr("产品卡片"),this);
    auto* actionSuggest = new QAction(tr("产品推荐"),this);
    auto* actionSendWechat = new QAction(tr("推送微信"),this);

    _pMultiMenu->addAction(scanQRCode);
#ifndef _STARTALk
    _pMultiMenu->addAction(actionVote);
    _pMultiMenu->addAction(actionTask);
#endif

#ifdef _QCHAT
    _pMultiMenu->addAction(actionSuggest);
    if(_pChatItem->_chatType == QTalk::Enum::TwoPersonChat){
        _pMultiMenu->addAction(actionPrud);
    } else if(_pChatItem->_chatType == QTalk::Enum::ConsultServer){
        _pMultiMenu->addAction(actionPrud);
        _pMultiMenu->addAction(actionSendWechat);
    }
#endif

    connect(actionVote, &QAction::triggered, [this](){
//        QString linkUrl = QString("%5username=%1&company=%2&group_id=%3&rk=%4").arg(Platform::instance().getSelfUserId().data())
//                .arg("qunar").arg(_pChatItem->_uid.qUsrId()).arg(Platform::instance().getServerAuthKey().data()).arg("vote/vote_list.php?");
//
//        WebService::loadUrl(QUrl(linkUrl), false);
    });
    connect(actionTask, &QAction::triggered, [this](){

//        QString linkUrl = QString("%5username=%1&company=%2&group_id=%3&rk=%4").arg(Platform::instance().getSelfUserId().data())
//                .arg("qunar").arg(_pChatItem->_uid.qUsrId()).arg(Platform::instance().getServerAuthKey().data()).arg("task/task_list.php?");
//
//        WebService::loadUrl(QUrl(linkUrl), false);
    });

    connect(scanQRCode, &QAction::triggered, [this](bool){
        emit g_pMainPanel->showQRcode();
    });

    connect(actionPrud,&QAction::triggered,[this](){
        _pSendProductWnd->clear();
        _pSendProductWnd->showModel();
    });

    connect(actionSuggest,&QAction::triggered,[this](){
        if(nullptr != qchatMoreFun){
            qchatMoreFun->showSuggestProducts();
        }
    });

    connect(actionSendWechat,&QAction::triggered,[this](){
        sendWechat();
    });

	//_pBtnFile
	connect(_pBtnFile, &QPushButton::clicked, this, &ToolWgt::onFileBtnClicked);
	//
	connect(_pBtnScreenshot, &QPushButton::clicked, this, &ToolWgt::onBtnScreenshot);
	connect(_pBtnEmoticon, &QPushButton::clicked, this, &ToolWgt::onpBtnEmoticon);
	connect(_pBtnScreenshotSet, &QPushButton::clicked, [&]()
	{
	    bool hideWnd = AppSetting::instance().getScreentShotHideWndFlag();
        _pScreenShotHideWnd->setChecked(hideWnd);
		_pMenu->exec(QCursor::pos());
	});

    connect(_pScreenShotHideWnd, &QAction::triggered, [](bool isChecked){
        AppSetting::instance().setScreentShotHideWndFlag(isChecked);
    });

    connect(_pBtnVideo, &QPushButton::clicked, [this]()
    {
        if(_pChatItem)
        {
            if(_pChatItem->_chatType == QTalk::Enum::TwoPersonChat)
            {
                _pChatItem->sendAudioVideoMessage();
                std::string selfId = Platform::instance().getSelfUserId();
                std::string peerId = QTalk::Entity::JID(_pChatItem->getPeerId().usrId().data()).username();
                AudioVideo::start2Talk(selfId, peerId);
            }
            else if(_pChatItem->_chatType == QTalk::Enum::GroupChat)
            {
                AudioVideo::startGroupTalk();
            }
        }

    });
    connect(_pBtnCode, &QPushButton::clicked, [this](){
        if(g_pMainPanel)
        {
            emit g_pMainPanel->sgOperator("发送代码");
            g_pMainPanel->showSendCodeWnd(_pChatItem->getPeerId());
        }
    });
    connect(_pBtnHistory, &QPushButton::clicked, this, &ToolWgt::showSearchWnd);
//        if(_pChatItem)
//        {
////            _pChatItem->_pSearchMainWgt->setVisible(true);
////            QApplication::setActiveWindow(_pChatItem->_pSearchMainWgt);
////            _pChatItem->_pSearchMainWgt->raise();
//        }
//    });

    connect(_pBtnMultifunction, &QPushButton::clicked, [this, actionVote, actionTask,actionPrud](){
        bool isGroup = _pChatItem->_chatType == QTalk::Enum::GroupChat;
        bool isConsultServer = false;
        actionVote->setVisible(isGroup);
        actionTask->setVisible(isGroup);
        _pMultiMenu->exec(QCursor::pos());
    });

    connect(_pBtnShock, &QPushButton::clicked, [this](){

        static long long time = 0;
        long long cur_t = QDateTime::currentMSecsSinceEpoch();
        if(cur_t - time > 1000 * 5)
        {
            time = cur_t;
            _pChatItem->sendShockMessage();
            emit g_pMainPanel->sgShockWnd();
        }
        else
        {
            LiteMessageBox::failed("抖动太频繁, 待会再试吧!", 2000);
        }

    });

    connect(_pBtnCloseService, &QPushButton::clicked, [this](){
        int ret = QtMessageBox::warning(this, "提示", "您确认结束本次服务吗?",
                                        QtMessageBox::EM_BUTTON_YES | QtMessageBox::EM_BUTTON_NO);
        if(ret == QtMessageBox::EM_BUTTON_YES)
        {
            if(g_pMainPanel && _pChatItem){
                QTalk::Entity::UID uid = _pChatItem->_uid;
                g_pMainPanel->getMessageManager()->serverCloseSession(uid.realId(),Platform::instance().getSelfXmppId(),uid.usrId());
            }
        }
    });

    connect(_pBtnTransfer,&QPushButton::clicked,[this](){
        _pSessionTransferWnd->showModel();
        if(g_pMainPanel && _pChatItem && !_pSessionTransferWnd->isHaveShow){
            g_pMainPanel->getMessageManager()->getSeatList(_pChatItem->_uid);
        }
    });

    connect(_pSendProductWnd,&SendProductWnd::sendJsonSig,this,&ToolWgt::sendJsonPrud);

    connect(_pBtnQuickReply,&QPushButton::clicked,[this](){
        _pQuickReplyWnd->showModel();
    });

    connect(_pQuickReplyWnd,&QuickReplyWnd::sendQuickReply,this,&ToolWgt::sendQuickReply);

    connect(_pSessionTransferWnd,&SessionTransferWnd::sessiontransfer,this,&ToolWgt::sessionTransfer);
}

/**
  * @函数名   onFileBtnClicked
  * @功能描述 选择发送文件
  * @参数
  * @author   cc
  * @date     2018/09/21
  */
void ToolWgt::onFileBtnClicked()
{
    emit g_pMainPanel->sgOperator("发送文件");

    QString strHistoryFileDir = QString::fromStdString(Platform::instance().getHistoryDir());
    QString strFilePath = QFileDialog::getOpenFileName(g_pMainPanel, QString::fromLocal8Bit("选择需要发送的文件"), strHistoryFileDir);
	if (!strFilePath.isEmpty() && _pInputWgt)
	{
        Platform::instance().setHistoryDir(strFilePath.toStdString());
		_pInputWgt->dealFile(strFilePath, true);
	}

	_pInputWgt->setFocus();

}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/17
  */
void ToolWgt::onBtnScreenshot()
{
    emit g_pMainPanel->sgOperator("截图");

	int waitSecond = 0;
    bool hideWnd = AppSetting::instance().getScreentShotHideWndFlag();
	if (hideWnd)
	{
		waitSecond = 200;
		UICom::getInstance()->getAcltiveMainWnd()->hide();
	}
	QTimer::singleShot(waitSecond, [this]()
	{
        SnipScreenTool::getInstance()->setConversionId(_pChatItem->conversionId());
		SnipScreenTool::getInstance()->Init().Start();
	});
}

/**
  * @函数名   
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/17
  */
void ToolWgt::onpBtnEmoticon()
{
    emit g_pMainPanel->sgOperator("表情");

	QPoint pos = QCursor::pos();
	EmoticonMainWgt::getInstance()->setConversionId(_pChatItem->conversionId());
    EmoticonMainWgt::getInstance()->setVisible(false);
    EmoticonMainWgt::getInstance()->setVisible(true);
	EmoticonMainWgt::getInstance()->move(pos.x() - 60, pos.y() - 300);
	EmoticonMainWgt::getInstance()->setFocus();
}

void ToolWgt::openLinkWithCkey(const QUrl& url)
{
    MapCookie cookies;
    cookies["q_ckey"] = QString::fromStdString(Platform::instance().getClientAuthKey());
    WebService::loadUrl(url, false, cookies);
}

void ToolWgt::sendJsonPrud(const QString &products) {
    if(g_pMainPanel && _pChatItem){
        QTalk::Entity::UID uid = _pChatItem->_uid;
        QTalk::Entity::JID jid(uid.realId().c_str());
        QTalk::Entity::JID userId(uid.usrId().c_str());
        std::string type = _pChatItem->_chatType == QTalk::Enum::ConsultServer ? "consult" : "note";
        g_pMainPanel->getMessageManager()->sendProduct(jid.username(),userId.username(),products.toStdString(),type);
    }
}

void ToolWgt::sendQuickReply(const std::string &text) {
    if(g_pMainPanel && _pChatItem){
        _pChatItem->sendTextMessage(text,_mapAtInfo);
    }
}

void ToolWgt::showSeats(std::vector<QTalk::Entity::ImTransfer> transfers) {
    if(_pSessionTransferWnd){
        _pSessionTransferWnd->showSeats(transfers);
    }
}

void ToolWgt::sessionTransfer(const std::string &newJid, const std::string &reason) {
    if(g_pMainPanel && _pChatItem){
        QTalk::Entity::UID uid = _pChatItem->_uid;
        g_pMainPanel->getMessageManager()->sessionTransfer(uid,newJid,reason);
    }
}

void ToolWgt::sendWechat() {
    if(g_pMainPanel && _pChatItem){
        QTalk::Entity::UID uid = _pChatItem->_uid;
        g_pMainPanel->getMessageManager()->sendWechat(uid);
    }
}
