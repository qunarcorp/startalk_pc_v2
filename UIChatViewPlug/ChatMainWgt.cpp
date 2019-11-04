#include "ChatMainWgt.h"
#include <QDateTime>
#include <QResizeEvent>
#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QScrollBar>
#include <QFileDialog>
#include <QJsonObject>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QStandardPaths>
#include <QEvent>
#include <QJsonArray>
#include <QPointer>
#include <QUrlQuery>
#include "ChatViewMainPanel.h"
#include "MessageItems/TextMessItem.h"
#include "MessageItems/ImageMessItem.h"
#include "MessageItems/EmojiMessItem.h"
#include "MessageItems/FileSendReceiveMessItem.h"
#include "MessageItems/TipMessageItem.h"
#include "MessageItems/CommonTrdInfoItem.h"
#include "MessageItems/NoteMessageItem.h"
#include "MessageItems/NoticeMessageItem.h"
#include "MessageItems/MeetingRemindItem.h"
#include "MessageItems/SystemMessageItem.h"
#include "MessageItems/VideoMessageItem.h"
#include "MessageItems/MedalMind.h"
#include "MessageItems/CodeItem.h"
#include "MessageItems/AudioVideoItem.h"
#include "MessageItems/VoiceMessageItem.h"
#include "../Emoticon/EmoticonMainWgt.h"
#include "../QtUtil/Entity/JID.h"
#include "InputWgt.h"
#include "../Platform/Platform.h"
#include "../CustomUi/LiteMessageBox.h"
#include "../CustomUi/QtMessageBox.h"
#include "../QtUtil/lib/cjson/cJSON.h"
#include "../QtUtil/lib/cjson/cJSON_inc.h"
#include "../UICom/qimage/qimage.h"
#include "../include/perfcounter.h"
#include "search/LocalSearchMainWgt.h"
#include "ChatUtil.h"
#include "MessageItems/HotLineTipItem.h"
#include "MessageItems/HotLineAnswerItem.h"
#include "ShareMessageFrm.h"

#define DEM_ATALL_STR "@all"

extern ChatViewMainPanel *g_pMainPanel;

int get_index(QListWidget* wgt, qint64 time, bool isHis)
{
    int index = 0;
    if(isHis)
    {
        for(index = 0; index < wgt->count(); index++)
        {
            qint64 t = wgt->item(index)->data(ChatMainWgt::EM_DATE_TYPE_TIME).toLongLong();
            if(time <= t)
                break;
        }
    }
    else
    {
        for(index = wgt->count() - 1; index >= 0; index--)
        {
            qint64 t = wgt->item(index)->data(ChatMainWgt::EM_DATE_TYPE_TIME).toLongLong();
            if(time >= t)
                break;
        }
        index+=1;
    }

    return index;
}

ChatMainWgt::ChatMainWgt(ChatViewItem *pViewItem)
        : QListWidget(pViewItem),
        _pViewItem(pViewItem),
        _hasnotHistoryMsg(true),
        _oldScrollBarVal(0),
        _selectEnable(false) {

    // 设置滚动方式
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->verticalScrollBar()->setSingleStep(15);
//    this->setAutoScroll(false);
    this->setObjectName("ChatMainWgt");
    this->setFrameShape(QFrame::NoFrame);
    this->setFocusPolicy(Qt::NoFocus);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->verticalScrollBar()->setVisible(false);
    //
    _pNewMessageTipItem = new NewMessageTip(this);
    _pNewMessageTipItem->setVisible(false);
    _pAtMessageTipItem = new AtMessageTip(this);
    _pAtMessageTipItem->setVisible(false);
    //
    std::string selfName = Platform::instance().getSelfName();
    _selfUserName = QString::fromStdString(selfName);

    _pMenu = new QMenu(this);
    _pMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    saveAsAct = new QAction(tr("另存为"), _pMenu);
    copyAct = new QAction(tr("拷贝"), _pMenu);
    quoteAct = new QAction(tr("引用"), _pMenu);
    forwardAct = new QAction(tr("转发"), _pMenu);
    revokeAct = new QAction(tr("撤回"), _pMenu);
    collectionAct = new QAction(tr("收藏为表情"), _pMenu);
    shareMessageAct = new QAction(tr("多选"), _pMenu);
    qrcodeAct = new QAction(tr("识别图片二维码"), _pMenu);

    _pMenu->addAction(copyAct);
    _pMenu->addAction(saveAsAct);
    _pMenu->addAction(collectionAct);
    _pMenu->addAction(qrcodeAct);
    _pMenu->addAction(forwardAct);
    _pMenu->addSeparator();
    _pMenu->addAction(quoteAct);
    _pMenu->addAction(revokeAct);
    _pMenu->addSeparator();
    _pMenu->addAction(shareMessageAct);

    setContextMenuPolicy(Qt::CustomContextMenu);
    qRegisterMetaType<std::map<std::string, QInt32> >("std::map<std::string, QInt32 >");
    qRegisterMetaType<std::vector<std::string> >("std::vector<std::string>");
    qRegisterMetaType<QInt64>("QInt64");
    connects();
    //
    std::function<int(STLazyQueue<bool>*)> adjustItemsFunc = [this](STLazyQueue<bool> *q) ->int {
        int runningCount = 0;
        if (q != nullptr && !q->empty()) {
            while (!q->empty()) {
                q->pop();
                runningCount++;
            }
            QPointer<ChatMainWgt> pThis(this);
            if(pThis)
                emit adjustItems();
        }
        return runningCount;
    };
    std::function<int(STLazyQueue<bool>*)> selectItemFun = [this](STLazyQueue<bool> *q) ->int {
        int runningCount = 0;
        if (q != nullptr && !q->empty()) {
            while (!q->empty()) {
                q->pop();
                runningCount++;
            }

            QPointer<ChatMainWgt> pThis(this);
            if(pThis)
                emit sgSelectItem();
        }
        return runningCount;
    };
    _resizeQueue = new STLazyQueue<bool>(50, adjustItemsFunc);
    _selectItemQueue = new STLazyQueue<bool>(50, selectItemFun);
}

ChatMainWgt::~ChatMainWgt() = default;

void ChatMainWgt::connects() {
    //
    connect(_pViewItem->_pShareMessageFrm, &ShareMessageFrm::sgShareMessage, this, &ChatMainWgt::onShareMessage);
    connect(this, &ChatMainWgt::sgSelectedSize, _pViewItem->_pShareMessageFrm, &ShareMessageFrm::setSelectCount);

    connect(this, &ChatMainWgt::sgUploadShareMsgSuccess, g_pMainPanel, &ChatViewMainPanel::sendShareMessage);
    //
    connect(this, SIGNAL(sgRecvFRileProcess(double, double, double, double, double, std::string)),
            this, SLOT(onRecvFRileProcess(double, double, double, double, double, std::string)));


    connect(this, &ChatMainWgt::gotReadStatueSignal, this, &ChatMainWgt::onRecvReadState);
    connect(this, &ChatMainWgt::sgGotMState, this, &ChatMainWgt::onMState);
    connect(this, &ChatMainWgt::sgGotGroupMStatue, this, &ChatMainWgt::onRecvGroupMState);
    connect(this, &ChatMainWgt::sgDisConnected, this, &ChatMainWgt::onDisconnected);
    connect(this, &ChatMainWgt::sgSendFailed, this, &ChatMainWgt::onSendMessageFailed);
    connect(this, &ChatMainWgt::updateRevokeSignal, this, &ChatMainWgt::updateRevokeMessage);
    connect(this, &ChatMainWgt::sgImageDownloaded, this, &ChatMainWgt::onImageDownloaded, Qt::QueuedConnection);
    connect(this, &ChatMainWgt::customContextMenuRequested, this, &ChatMainWgt::onCustomContextMenuRequested);

    connect(copyAct, &QAction::triggered, this, &ChatMainWgt::onCopyAction);
    connect(saveAsAct, &QAction::triggered, this, &ChatMainWgt::onSaveAsAction);
    connect(revokeAct, &QAction::triggered, this, &ChatMainWgt::onRevokeAction);
    connect(quoteAct, &QAction::triggered, this, &ChatMainWgt::onQuoteAct);
    connect(collectionAct, &QAction::triggered, this, &ChatMainWgt::onCollectionAct);
    connect(qrcodeAct, &QAction::triggered, this, &ChatMainWgt::onQRCodeAct);
    connect(shareMessageAct, &QAction::triggered, [this](){
        if(!_selectEnable)
            _pViewItem->setShareMessageState(true);
    });

    connect(this, &ChatMainWgt::showTipMessageSignal,
            this, &ChatMainWgt::showTipMessage);
    connect(forwardAct, &QAction::triggered, this, &ChatMainWgt::onForwardAct);
    QScrollBar *scrolbar = this->verticalScrollBar();
    connect(scrolbar, &QScrollBar::valueChanged, this, &ChatMainWgt::onScrollBarChanged, Qt::QueuedConnection);
    connect(this, &ChatMainWgt::adjustItems, this, &ChatMainWgt::onAdjustItems, Qt::QueuedConnection);
    connect(this, &QListWidget::itemSelectionChanged, this, &ChatMainWgt::onItemSelectionChanged, Qt::QueuedConnection);
    connect(this, &ChatMainWgt::sgSelectItem, this, &ChatMainWgt::onItemChanged, Qt::QueuedConnection);
}

//
void ChatMainWgt::onCustomContextMenuRequested(const QPoint &pos) {

    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    bool isItem = (nullptr != itemWgt);
    saveAsAct->setVisible(isItem);
    revokeAct->setVisible(isItem);
    quoteAct->setVisible(isItem);
    copyAct->setVisible(isItem);
    forwardAct->setVisible(isItem);
    collectionAct->setVisible(false);
    shareMessageAct->setVisible(true);
    qrcodeAct->setVisible(false);

    if (nullptr != itemWgt)
    {
        saveAsAct->setEnabled(true);
        revokeAct->setEnabled(true);
        quoteAct->setEnabled(true);
        copyAct->setEnabled(true);

        switch (itemWgt->type()) {
            case QTalk::Entity::MessageTypeText:
            case QTalk::Entity::MessageTypeRobotAnswer:
            case QTalk::Entity::MessageTypeGroupAt:
            case QTalk::Entity::MessageTypeImageNew:
            case QTalk::Entity::MessageTypeFile:
            case QTalk::Entity::MessageTypePhoto:
            case QTalk::Entity::MessageTypeCommonTrdInfo:
            case QTalk::Entity::MessageTypeCommonTrdInfoV2:
            case QTalk::Entity::MessageTypeSourceCode:
            case QTalk::Entity::MessageTypeSmallVideo:

                if (!itemWgt->contains(QCursor::pos())) return;
                switch (itemWgt->type()) {
                    case QTalk::Entity::MessageTypeFile:
                        saveAsAct->setEnabled(true);
                        quoteAct->setEnabled(false);
                        break;
                    case QTalk::Entity::MessageTypeText:
                    case QTalk::Entity::MessageTypeRobotAnswer:
                    case QTalk::Entity::MessageTypeGroupAt: {
                        auto textItem = qobject_cast<TextMessItem *>(itemWgt);
                        bool isImage = textItem && textItem->isImageContext();
                        saveAsAct->setEnabled(isImage);
                        collectionAct->setVisible(isImage);
                        qrcodeAct->setVisible(isImage);
                        quoteAct->setEnabled(true);
                        break;
                    }
                    case QTalk::Entity::MessageTypeSourceCode: {
                        auto codeItem = qobject_cast<CodeItem *>(itemWgt);
                        saveAsAct->setEnabled(false);
                        copyAct->setEnabled(true);
                        quoteAct->setEnabled(false);
                        break;
                    }


                    case QTalk::Entity::MessageTypePhoto:
                    case QTalk::Entity::MessageTypeImageNew:
                    {
                        auto* imgItem = qobject_cast<ImageMessItem*>(itemWgt);
                        if(imgItem)
                        {
                            copyAct->setEnabled(true);
                            saveAsAct->setEnabled(true);
                            quoteAct->setEnabled(true);
                            collectionAct->setVisible(true);
                            qrcodeAct->setVisible(true);
                            break;
                        }
                    }
                    case QTalk::Entity::MessageTypeCommonTrdInfo:
                    case QTalk::Entity::MessageTypeCommonTrdInfoV2:
                    case QTalk::Entity::MessageTypeSmallVideo:
                        copyAct->setEnabled(false);
                        quoteAct->setEnabled(false);
                    default:
                        saveAsAct->setEnabled(false);
                        break;
                }
                break;
            default:
                return;
        }
        bool isSelfMsg = itemWgt->msgInfo().From == g_pMainPanel->getSelfUserId();
        revokeAct->setEnabled(isSelfMsg);
    }
    _pMenu->exec(mapToGlobal(pos));

}

/**
  * @函数名   showMessage
  * @功能描述 显示消息接口
  * @参数
  * @author   cc
  * @date     2018/09/18
  */
void ChatMainWgt::showMessage(const QTalk::Entity::ImMessageInfo &msg, bool isHistory) {
	
    if(_arHistoryIds.find(msg.MsgId) != _arHistoryIds.end())
    {
        if(_itemWgts.contains(msg.MsgId.data()))
        {
            for(MessageItemBase* item : _itemWgts.values(msg.MsgId.data()))
            {
                if(item->_adjust) {
//                    warn_log("repeat message id: {0} content: {1}", msg.MsgId, msg.Content);
                    return;
                }

                if(msg.Content.empty())
                    return;

                item->_adjust = true;

                switch (item->msgInfo().Type)
                {
                    case QTalk::Entity::MessageTypeText:
                    case QTalk::Entity::MessageTypeGroupAt:
                    {
                        QVector<StTextMessage> msgs;
                        analysisMessage(msgs, msg.Content, msg.MsgId);
                        auto *textItem = qobject_cast<TextMessItem*>(item);
                        textItem->setStTextMessages(msgs);
                        if(_items.contains(textItem))
                        {
                            textItem->setMsgInfo(msg);
                            textItem->setMessageContent();
                            dealMessage(textItem, _items[textItem], isHistory);
//                            emit adjustItems();
//                            dealMessage(textItem, _items[textItem], isHistory);
                        }
                        break;
                    }
                    case QTalk::Entity::MessageTypeFile:
                        break;
                    default:
                        break;
                }
            }
            return;
        }
        else
        {
            warn_log("repeat message id: {0} content: {1}", msg.MsgId, msg.Content);
            return;
        }
    }
    else
        _arHistoryIds.insert(msg.MsgId);

    //
    switch (msg.Type) {
        case QTalk::Entity::MessageTypeEmpty:
        {
            return;
        }
        case QTalk::Entity::MessageTypeGroupNotify:
        {
            showTipMessage(msg.Type, QString::fromStdString(msg.Content), isHistory, msg.LastUpdateTime);
            return;
        }
        case QTalk::Entity::MessageTypeShock:
        {
            std::string msgFrom = msg.From;
            QString content = tr("你");
            if (g_pMainPanel->getSelfUserId() == msgFrom)
                content.append(tr("抖了他一下"));
            else
                content.append(tr("收到一个窗口抖动"));

            showTipMessage(msg.Type, content, isHistory, msg.LastUpdateTime);

            return;
        }
        case QTalk::Entity::MessageTypeRevoke: {
            QString userName = tr("你");
            std::string msgFrom = msg.From;
            if(msgFrom.empty() && !msg.ExtendedInfo.empty())
            {
                cJSON *data = cJSON_Parse(msg.ExtendedInfo.data());
                if(nullptr != data)
                {
                    msgFrom = QTalk::JSON::cJSON_SafeGetStringValue(data, "fromId");
                    cJSON_Delete(data);
                }
            }

            if (g_pMainPanel->getSelfUserId() != msgFrom) {
                userName = QString::fromStdString(msg.UserName);
                if (userName.isEmpty()) {
                    std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo
                            = dbPlatForm::instance().getUserInfo(msgFrom);
                    if (userInfo) {
                        userName = QString::fromStdString(QTalk::getUserName(userInfo));
                    }
                }
            }
            showRevokeMessage(userName, isHistory, msg.LastUpdateTime);
            return;
        }
        default: {
            showMessageTime(msg.MsgId.data(), msg.LastUpdateTime, isHistory);
            switch (msg.Type) {
                case QTalk::Entity::MessageTypeFile: {
                    QTalk::Entity::ImMessageInfo msgT = analysisFileMessage(msg, isHistory);
                    auto *pChatItem = new FileSendReceiveMessItem(msgT, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    _fileItems.insert(pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeCommonTrdInfo:
                case QTalk::Entity::MessageTypeCommonTrdInfoV2: {
                    auto *pChatItem = new CommonTrdInfoItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeProduct:
                case QTalk::Entity::MessageTypeNote:{
                    auto *pChatItem = new NoteMessageItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeSystem:{
                    auto *pChatItem = new SystemMessageItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeNotice:{
                    auto *pChatItem = new NoticeMessageItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeMeetingRemind:{
                    auto *pChatItem = new MeetingRemindItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeSourceCode: {
                    auto *pCodeItem = new CodeItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pCodeItem);
                    dealMessage(pCodeItem, new QListWidgetItem(), isHistory);
                    break;
                }

                // 表情
                case QTalk::Entity::MessageTypeImageNew:
                case QTalk::Entity::MessageTypePhoto: {

                    QString strContent = QString::fromStdString(msg.Content);

                    if(strContent.contains("[obj type=\"emoticon\""))
                    {
                        QString pkgid, shortCut, imagePath;
                        double width = 0, height = 0;
                        analysisEmoticonMessage(strContent, pkgid, shortCut);

                        if(pkgid.isEmpty() || shortCut.isEmpty())
                        {
                            width = height = 128;
                            imagePath = ":/chatview/image1/default.png";
                        }
                        else
                        {
                            QString localEmo = EmoticonMainWgt::getInstance()->getEmoticonLocalFilePath(pkgid, shortCut);
                            QFileInfo info(localEmo);
                            if(localEmo.isEmpty() || !info.exists() || info.isDir())
                            {
                                imagePath = ":/chatview/image1/default.png";
                                width = height = 128;
                                downloadEmoticon(msg.MsgId.data(), pkgid, shortCut);
//
                            }
                            else
                            {
                                imagePath = localEmo;
                                QPixmap emo(localEmo);
                                width = emo.width();
                                height = emo.height();
                                QTalk::Image::scaImageSize(width, height);
                            }
                        }
                        auto *pChatItem = new EmojiMessItem(msg, imagePath, {width, height}, this);
                        _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                        auto* item = new QListWidgetItem();
                        dealMessage(pChatItem, item, isHistory);

                        connect(pChatItem, &EmojiMessItem::sgItemChanged, [pChatItem, item](){
                            auto size = pChatItem->itemWdtSize();
                            item->setSizeHint(size);
                        });
                    }
                    else if(strContent.contains("[obj type=\"image\""))
                    {

                        QString imagePath, imageLink;
                        double width = 0, height = 0;

                        analysisImageMessage(strContent, imageLink, width, height);

                        QString localImg = QTalk::GetImagePathByUrl(imageLink.toStdString()).data();
                        QFileInfo info(localImg);
                        if(!info.exists() || info.isDir())
                        {
                            imagePath = ":/chatview/image1/default.png";
                            downloadImage(msg.MsgId.data(), imageLink, width, height);
                        }
                        else
                            imagePath = localImg;

                        auto *pChatItem = new ImageMessItem(msg, imageLink, imagePath, QSize(width, height), this);
                        _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                        auto* item = new QListWidgetItem();
                        dealMessage(pChatItem, item, isHistory);
                        connect(pChatItem, &ImageMessItem::sgItemChanged, [pChatItem, item](){
                            auto size = pChatItem->itemWdtSize();
                            item->setSizeHint(size);
                        });
                    }
                    else
                    {
                        error_log("analysis error type content:{0}", strContent.toStdString());
                    }
                    break;
                }
                case QTalk::Entity::MessageTypeSmallVideo:
                {
                    auto *pChatItem = new VideoMessageItem(msg, this);
                    connect(pChatItem, &VideoMessageItem::sgPlayVideo,
                            g_pMainPanel->_pVideoPlayWnd, &VideoPlayWnd::showVideo, Qt::QueuedConnection);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    _videoItems.insert(pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeVoice:
                {
                    auto *pChatItem = new VoiceMessageItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::WebRTC_MsgType_VideoCall:
                case QTalk::Entity::WebRTC_MsgType_AudioCall:
                case QTalk::Entity::WebRTC_MsgType_Video:
                case QTalk::Entity::WebRTC_MsgType_Video_Group:
                {
                    auto *pVideoItem = new AudioVideoItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pVideoItem);
                    dealMessage(pVideoItem, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeRobotTurnToUser:
                {
                    auto *pHotLineTip = new HotLineTipItem(msg, this);

                    auto *item = new QListWidgetItem;
                    item->setData(EM_DATETYPE_MSGTYPE, QTalk::Entity::MessageTypeRobotTurnToUser);
                    item->setData(EM_DATE_TYPE_TIME, msg.LastUpdateTime);
                    QSize size = QSize(this->width(), 40);
                    item->setSizeHint(size);

                    if (isHistory) {
                        auto index = get_index(this, msg.LastUpdateTime, isHistory);
                        this->insertItem(index, item);
                        this->scrollToItem(item, QAbstractItemView::PositionAtBottom);
                    } else {
                        this->addItem(item);
                        this->scrollToBottom();
//                        int scroBarval = this->verticalScrollBar()->value();
//                        int maximum = this->verticalScrollBar()->maximum();
//                        if ((maximum - scroBarval) < 300) {
//
//                        } else {
//                            if (scroBarval != maximum && _pNewMessageTipItem) {
//                                _pNewMessageTipItem->onNewMessage();
//                            }
//                        }
                    }

                    setItemWidget(item, pHotLineTip);
                    break;
                }
                case QTalk::Entity::MessageTypeRobotAnswerList:
                {
                    auto *pHotLineAnswer = new HotLineAnswerItem(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pHotLineAnswer);
                    dealMessage(pHotLineAnswer, new QListWidgetItem(), isHistory);
                    break;
                }
                case QTalk::Entity::MessageTypeMedalRemind:
                {
                    auto *pChatItem = new MedalMind(msg, this);
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), pChatItem);
                    dealMessage(pChatItem, new QListWidgetItem(), isHistory);
                    break;
                }
                default: {
                    QVector<StTextMessage> msgs;
                    bool isSendMessage = msg.Direction == QTalk::Entity::MessageDirectionSent && msg.State == 0;
                    if(isSendMessage && msg.Content.empty())
                    {
                        analysisSendTextMessage(msgs, msg.ExtendedInfo.data());
                    }
                    else
                    {
                        if(msg.Content.empty() && !msg.ExtendedInfo.empty())
                            analysisSendTextMessage(msgs, msg.ExtendedInfo.data());
                        else
                            analysisMessage(msgs, msg.Content, msg.MsgId);
                    }
                    //
                    auto *msgbase = new TextMessItem(msgs, msg, this);
                    if(isSendMessage && msg.Content.empty())
                        msgbase->_adjust = false;
                    _itemWgts.insert(QString::fromStdString(msg.MsgId), msgbase);
                    dealMessage(msgbase, new QListWidgetItem(), isHistory);

                    break;
                }
            }
        }
        //
//        QApplication::processEvents(QEventLoop::AllEvents, 50);
    }

    if (msg.State) {
        for (MessageItemBase *item : _itemWgts.values(QString::fromStdString(msg.MsgId))) {
            // 阅读状态
            if (msg.State & 1)
                item->setReadState(msg.ReadedTag | 1);
            else
                item->setReadState(msg.ReadedTag);
        }
    }
    else
    {
        for (MessageItemBase *item : _itemWgts.values(QString::fromStdString(msg.MsgId))) {
            if(!_connectState || isHistory)
                item->onDisconnected();
        }
    }

}

/**
  * @函数名   resizeEvent
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/09/18
  */
void ChatMainWgt::resizeEvent(QResizeEvent *e)
{
    _resizeQueue->push(true);
    QListWidget::resizeEvent(e);
}

/**
  * @函数名
  * @功能描述 业务传入的消息可能跨线程 用信号槽跨线程
  * @参数
  * @date 2018.10.22
  */
void
ChatMainWgt::onRecvFRileProcess(double speed, double dtotal, double dnow, double utotal, double unow, std::string key) {
    double total;
    double now;

    if (dtotal > utotal) {
        total = dtotal;
        now = dnow;
    } else {
        total = utotal;
        now = unow;
    }

    auto milliseconds_since_epoch = QDateTime::currentMSecsSinceEpoch();
    // 最多100ms 更新一次界面
    if (milliseconds_since_epoch - downloadProcess < 500 && (total - now) > 0.000001) {
        return;
    }
    downloadProcess = milliseconds_since_epoch;

    //
    {
        auto itFind = std::find_if(_fileItems.cbegin(), _fileItems.cend(), [key](const auto& fileItemWdt) {
            return fileItemWdt->msgInfo().MsgId == key;
        });
        if (itFind != _fileItems.cend())
        {
            (*itFind)->setProcess(speed, dtotal, dnow, utotal, unow);
        }
    }
	//
    {
        auto itFind = std::find_if(_videoItems.cbegin(), _videoItems.cend(), [key](const auto& itemwgt) {
            return itemwgt->msgInfo().MsgId == key;
        });
        if (itFind != _videoItems.cend())
        {
            (*itFind)->setProcess(speed, dtotal, dnow, utotal, unow);
        }
    }
	//
	QApplication::processEvents(QEventLoop::AllEvents, 100);
}

//
void ChatMainWgt::showMessageTime(const QString& msgId, const QInt64 &nCurTime, bool isHistoryMessage) {

    static QList<QString> msgList;
    if(msgList.contains(msgId))
        return;
    else
        msgList.push_back(msgId);

    bool isshowtime = false;
    if (this->count() == 0) {
        isshowtime = true;
    } else {

        int tmpIndex = this->count() - 1;
        QInt64 itemTime = 0;
        if(isHistoryMessage)
        {
            tmpIndex = 0;
            while (tmpIndex <= this->count() - 1)
            {
                QListWidgetItem *lastItem = this->item(tmpIndex);
                if (nullptr != lastItem) {
                    int msgType = lastItem->data(EM_DATETYPE_MSGTYPE).toInt();
                    if(msgType != QTalk::Entity::MessageTypeTime)
                    {
                        itemTime = lastItem->data(EM_DATE_TYPE_TIME).toLongLong();
                        if(itemTime > nCurTime)
                            break;
                    }
                }
                tmpIndex++;
            }
        }
        else
        {
            QListWidgetItem *lastItem = this->item(tmpIndex);
            if (nullptr != lastItem) {
                itemTime = lastItem->data(EM_DATE_TYPE_TIME).toLongLong();
            }
        }
        isshowtime = abs(nCurTime - itemTime) > 60 * 1000; // 两个消息相差一分钟
    }

    if (isshowtime) {
        QString content = "";
        QDateTime dateTime = QDateTime::currentDateTime();
        QInt64 thisDay = dateTime.toMSecsSinceEpoch() - dateTime.time().msecsSinceStartOfDay();
        if (nCurTime < thisDay) {
            content = QDateTime::fromMSecsSinceEpoch(nCurTime).toString("yyyy-MM-dd hh:mm:ss");
        } else {
            content = QDateTime::fromMSecsSinceEpoch(nCurTime).toString("hh:mm:ss");
        }
        //
        showTipMessage(QTalk::Entity::MessageTypeTime, content, isHistoryMessage, nCurTime - 1);
    }
}

/**
 *
 * @param msgs
 * @param jsonMessage
 */
void ChatMainWgt::analysisSendTextMessage(QVector<StTextMessage> &msgs, const std::string &jsonMessage)
{
    auto document = QJsonDocument::fromJson(jsonMessage.data());
    if(document.isNull()){

    }
    else
    {
        QJsonArray array = document.array();
        for(auto && i : array)
        {
            QJsonObject obj = i.toObject();
            int key = obj.value("key").toInt();
            QString value = obj.value("value").toString();
            switch (key)
            {
                case Type_Text:
                {
                    analysisTextMessage(msgs, value);
                    break;
                }
                case Type_Image:
                {
                    StTextMessage imageMsg(StTextMessage::EM_IMAGE);
                    QFileInfo info(value);
                    if(!info.exists() || info.isDir())
                    {
                        imageMsg.content = ":/chatview/image1/default.png";
                        imageMsg.imageWidth = imageMsg.imageHeight = 200;
                    }
                    else
                    {
                        imageMsg.content = value;
                        QPixmap pixmap(value);
                        imageMsg.imageWidth = pixmap.width();
                        imageMsg.imageHeight = pixmap.height();
                        QTalk::Image::scaImageSize(imageMsg.imageWidth, imageMsg.imageHeight);
                    }

                    msgs.push_back(imageMsg);
                    break;
                }
                case Type_At:
                {
                    if (!_selfUserName.isEmpty() && _selfUserName == value) {
                        //
                        StTextMessage atMsg(StTextMessage::EM_ATMSG);
                        atMsg.content = QString("@%1 ").arg(value);
                        msgs.push_back(atMsg);
                    } else {
                        StTextMessage textMsg(StTextMessage::EM_TEXT);
                        textMsg.content = value;
                        msgs.push_back(textMsg);
                    }
                    break;
                }
                case Type_Url:
                {
                    StTextMessage linkMsg(StTextMessage::EM_LINK);
                    linkMsg.content = value;
                    msgs.push_back(linkMsg);
                    break;
                }
                case Type_Invalid:
                default:
                    break;
            }
        }
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.17
  */
void ChatMainWgt::analysisMessage(QVector<StTextMessage> &msgs, const std::string& content, const std::string& msgId) {

    QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");
    regExp.setMinimal(true);

    int pos = 0;
    int prePos = 0;
    while ((pos = regExp.indexIn(QString::fromStdString(content), pos)) != -1) {
        // 前面的文本
        QString text = QString::fromStdString(content).mid(prePos, pos - prePos); // 取匹配开始位置到匹配到的位置文本
        if (!text.isEmpty())
            analysisTextMessage(msgs, text);

        QString item = regExp.cap(0); // 符合条件的整个字符串
        QString type = regExp.cap(1); // 多媒体类型
        QString val = regExp.cap(2); // 路径

        if ("url" == type) {
            StTextMessage linkMsg(StTextMessage::EM_LINK);
            linkMsg.content = val;
            msgs.push_back(linkMsg);
        } else if ("emoticon" == type) {

            StTextMessage emoMsg(StTextMessage::EM_EMOTICON);
            QString pkgid, shortCut;
            analysisEmoticonMessage(item, pkgid, shortCut);

            if(pkgid.isEmpty() || shortCut.isEmpty())
            {
                emoMsg.imageWidth = emoMsg.imageHeight = 50;
                emoMsg.content = ":/chatview/image1/default.png";
            }
            else
            {
                QString localEmo = EmoticonMainWgt::getInstance()->getEmoticonLocalFilePath(pkgid, shortCut);
                QFileInfo info(localEmo);
                if(localEmo.isEmpty() || !info.exists() || info.isDir())
                {
                    emoMsg.content = ":/chatview/image1/default.png";

                    emoMsg.imageWidth = emoMsg.imageHeight = 50;
                    downloadEmoticon(msgId.data(), pkgid, shortCut);
                }
                else
                {
                    emoMsg.content = localEmo;
                    QPixmap emo(localEmo);
                    emoMsg.imageWidth = emo.width();
                    emoMsg.imageHeight = emo.height();
                    QTalk::Image::scaImageSize(emoMsg.imageWidth, emoMsg.imageHeight);
                }
                shortCut.replace("/", "");
                emoMsg.imageLink = QString("%1_%2").arg(pkgid, shortCut);
            }

            msgs.push_back(emoMsg);
        }
        else if("image" == type)
        {
            StTextMessage imageMsg(StTextMessage::EM_IMAGE);
            analysisImageMessage(item, imageMsg.imageLink, imageMsg.imageWidth, imageMsg.imageHeight);

            QString localImg = QTalk::GetImagePathByUrl(imageMsg.imageLink.toStdString()).data();
            QFileInfo info(localImg);
            if(!info.exists() || info.isDir())
            {
                imageMsg.content = ":/chatview/image1/default.png";
                downloadImage(msgId.data(), imageMsg.imageLink, imageMsg.imageWidth, imageMsg.imageHeight);
            }
            else
                imageMsg.content = localImg;

            msgs.push_back(imageMsg);
        }
        //
        pos += regExp.matchedLength();
        prePos = pos;
    }

    QString lastStr = QString::fromStdString(content).mid(prePos);
    if (!lastStr.isEmpty())
        analysisTextMessage(msgs, lastStr);
}

/**
 * 处理文本类消息
 */
void ChatMainWgt::analysisTextMessage(QVector<StTextMessage> &msgs, const QString &text) {
    if (!text.isEmpty()) {
        // 对于没有安装[obj]格式发送的链接 进行转换
        QRegExp regExp("(((ftp|https?)://|www.)[a-zA-Z0-9\\.\\-]+\\.([a-zA-Z0-9]{1,4})([^ ^\"^\\[^\\]^\\r^\\n]*))");

        auto pos = 0;
        auto prePos = 0;
        while ((pos = regExp.indexIn(text, pos)) != -1) {
            QString url = regExp.cap(0);

            if (pos - prePos > 0) {
                QString tmpText = text.mid(prePos, pos - prePos); // 取匹配开始位置到匹配到的位置文本
                analysisAtMessage(msgs, tmpText);
            }

            StTextMessage linkMsg(StTextMessage::EM_LINK);
            linkMsg.content = url;
            msgs.push_back(linkMsg);

            pos += regExp.matchedLength();
            prePos = pos;
        }


        if (prePos <= text.size() - 1) {
            QString lastStr = text.mid(prePos);
            analysisAtMessage(msgs, lastStr);
        }
    }
}

/**
 * 
 */
void ChatMainWgt::analysisAtMessage(QVector<StTextMessage> &msgs, const QString &text) {
    if (_pViewItem->_chatType == QTalk::Enum::GroupChat) {
        if (text.contains(DEM_ATALL_STR)) {
            //
            QString leftStr = text.section(DEM_ATALL_STR, 0, 0);
            QString rightStr = text.section(DEM_ATALL_STR, 1, 1);
            if (!leftStr.isEmpty())
                analysisAtMessage(msgs, leftStr);
            //
            StTextMessage atMsg(StTextMessage::EM_ATMSG);
            atMsg.content = DEM_ATALL_STR;
            msgs.push_back(atMsg);
            //
            if (!rightStr.isEmpty())
                analysisAtMessage(msgs, rightStr);
        } else {
            QString atSelfName = QString("@%1").arg(_selfUserName);
            if (!_selfUserName.isEmpty() && text.contains(QString("@%1").arg(_selfUserName))) {

                auto pos = text.indexOf(atSelfName);
                QString leftStr = text.mid(0, pos);
                QString rightStr = text.mid(pos + atSelfName.size());
                if (!leftStr.isEmpty())
                    analysisAtMessage(msgs, leftStr);
                //
                StTextMessage atMsg(StTextMessage::EM_ATMSG);
                atMsg.content = atSelfName;
                msgs.push_back(atMsg);
                //
                if (!rightStr.isEmpty())
                    analysisAtMessage(msgs, rightStr);
            } else {
                StTextMessage textMsg(StTextMessage::EM_TEXT);
                textMsg.content = text;
                msgs.push_back(textMsg);
            }
        }
    } else {
        StTextMessage textMsg(StTextMessage::EM_TEXT);
        textMsg.content = text;
        msgs.push_back(textMsg);
    }
}

void ChatMainWgt::analysisImageMessage(const QString &content, QString &imageLink, qreal &width, qreal &height) {

    QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");

    if (regExp.indexIn(content) != -1) {

        QString item = regExp.cap(0); // 符合条件的整个字符串
        QString type = regExp.cap(1); // 多媒体类型
        QString val = regExp.cap(2); // 路径
        QString strWidth = regExp.cap(3); // 宽高

        std::string localFilePath;

        if ("image" == type) {

            QString rawUrl = QUrl(val).toEncoded();
            imageLink = rawUrl;

            if (!strWidth.isEmpty()) {
                QRegExp exp("width=([0-9]*\\.?[0-9]+) height=([0-9]*\\.?[0-9]+)");
                if (exp.indexIn(strWidth) >= 0) {
                    width = exp.cap(1).toDouble();
                    height = exp.cap(2).toDouble();
                }
            }
            //
            if(width == 0 && height == 0)
            {
                QString localImg = QTalk::GetImagePathByUrl(imageLink.toStdString()).data();
                QFileInfo info(localImg);
                if(info.exists() && !info.isDir())
                {
                    QPixmap pixmap(localImg);
                    width = pixmap.width();
                    height = pixmap.height();
                }
                else
                {
                    width = height = 200;
                    return;
                }
            }

            QTalk::Image::scaImageSize(width, height);

        } else if ("emoticon" == type) {
            error_log("analysis error type content:{0}", content.toStdString());
        }
    }
}

/**
 *
 * @param content
 * @param imageLink
 * @param width
 * @param height
 */
void ChatMainWgt::analysisEmoticonMessage(const QString& content, QString& pkgid, QString& shortCut)
{
    QRegExp regExp("\\[obj type=[\\\\]?\"([^\"]*)[\\\\]?\" value=[\\\\]?\"([^\"]*)[\\\\]?\"(.*)\\]");

    if (regExp.indexIn(content) != -1) {

        QString item = regExp.cap(0); // 符合条件的整个字符串
        QString type = regExp.cap(1); // 多媒体类型
        QString val = regExp.cap(2); // 路径
        QString strWidth = regExp.cap(3); // 宽高

        if ("image" == type) {

            error_log("analysis error type content:{0}", content.toStdString());

        } else if ("emoticon" == type) {
            shortCut = val;
            shortCut.remove("[").remove("]");
            if (!strWidth.isEmpty()) {
                QRegExp exp("width=(.+) height=(.+)");
                if (exp.indexIn(strWidth) >= 0) {
                    pkgid = exp.cap(1);
                }
            }
//            emoLocalPath = EmoticonMainWgt::getInstance()->getEmoticonLocalFilePath(pkgid, shortCut);
//            localFilePath = EmoticonMainWgt::getInstance()->getEmoticonFilePath(pkgid, shortCut).toStdString();
//
//            localPath = QString::fromStdString(localFilePath);
//            QPixmap image = QTalk::qimage::instance().loadPixmap(localPath, true);
//            width = image.width();
//            height = image.height();
//            scaImageSize(width, height);
        }
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author
  * @date 2018.10.22
  */
QTalk::Entity::ImMessageInfo ChatMainWgt::analysisFileMessage(const QTalk::Entity::ImMessageInfo &msg, bool isHis) {

    QTalk::Entity::ImMessageInfo msgT = msg;
    if(msg.Content.empty())
    {
        if(!msg.ExtendedInfo.empty())
        {
            cJSON *content = cJSON_Parse(msg.ExtendedInfo.data());
            if (nullptr == content) {
                return msgT;
            }
            //
            msgT.FileName = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileName");
            msgT.FileSize = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileSize");
            msgT.FileMd5 = QTalk::JSON::cJSON_SafeGetStringValue(content, "FILEMD5");
            cJSON_Delete(content);
        }
    }
    else
    {
        cJSON *content = cJSON_Parse(msg.Content.data());
        if (nullptr == content) {
            return msgT;
        }
        //
        msgT.FileName = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileName");
        msgT.FileSize = QTalk::JSON::cJSON_SafeGetStringValue(content, "FileSize");
        msgT.FileUrl = QTalk::JSON::cJSON_SafeGetStringValue(content, "HttpUrl");
        msgT.FileMd5 = QTalk::JSON::cJSON_SafeGetStringValue(content, "FILEMD5");
        cJSON_Delete(content);

        if(msgT.FileMd5.empty() && !msgT.FileUrl.empty())
        {
            QUrlQuery query(QString(msgT.FileUrl.data()).section("?", 1, 1));

            if(query.hasQueryItem("name"))
            {
                msgT.FileMd5 = query.queryItemValue("name").
                        section(".", 0, 0).toLower().toStdString();
            }
        }
    }

    return msgT;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.10.22
  */
void ChatMainWgt::recvFileProcess(const double &speed, const double &dtotal, const double &dnow, const double &utotal,
                                  const double &unow, const std::string &key) {
    if(_arHistoryIds.find(key) != _arHistoryIds.end())
        emit sgRecvFRileProcess(speed, dtotal, dnow, utotal, unow, key);
}

//
void ChatMainWgt::wheelEvent(QWheelEvent *e) {

    int scrollBarVal = this->verticalScrollBar()->value();
    int maximum = this->verticalScrollBar()->maximum();

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    int direct = e->angleDelta().y();
#else
    int direct = e->delta();
#endif

    //上翻
    if((maximum == 0 || scrollBarVal == 0) && direct > 0)
    {
        qint64 curTime = QDateTime::currentMSecsSinceEpoch();
        if(curTime - _historyTime < 500)
        {
//            warn_log("getHistory time too short {0}", curTime - _historyTime);
            return;
        }
        else
            _historyTime = curTime;

        QInt64 time = 0;
        if (count() > 0) {
            int i = 0;

            while (true) {
                long long type = this->item(i)->data(EM_DATETYPE_MSGTYPE).toLongLong();
                if (type != QTalk::Entity::MessageTypeTime) {
                    time = this->item(i)->data(EM_DATE_TYPE_TIME).toLongLong();
                    if (time != 0) {

                        break;
                    }
                }

                if (i == this->count())
                    break;
                i++;
            }
        }
        if(_hasnotHistoryMsg)
            g_pMainPanel->getHistoryMsg(time, _pViewItem->_chatType, _pViewItem->getPeerId());
    }

    QListWidget::wheelEvent(e);
}

void ChatMainWgt::mousePressEvent(QMouseEvent *e) {

    QListWidget::mousePressEvent(e);
}

void ChatMainWgt::setHasNotHistoryMsgFlag(bool hasHistory) {
    _hasnotHistoryMsg = hasHistory;

    if (!_hasnotHistoryMsg) {
        QListWidgetItem *item = nullptr;
        TipMessageItem *itemWgt = nullptr;

        if (count() > 0) {
            item = this->item(0);
            if (item->data(EM_DATETYPE_MSGTYPE).toLongLong() == QTalk::Entity::MessageTypeTime) {
                itemWgt = (TipMessageItem *) this->itemWidget(item);
            }
        }

        if (nullptr == item && nullptr != itemWgt) {
            item = new QListWidgetItem();
            itemWgt = new TipMessageItem();
            this->insertItem(0, item);
            setItemWidget(item, itemWgt);
        }

        if (itemWgt) {
            itemWgt->setText(tr("没有更多消息了"));
        }
    }
}

void ChatMainWgt::dealMessage(MessageItemBase *msgItemWgt, QListWidgetItem *pLstItem, bool isHistoryMessage) {

    if (msgItemWgt && pLstItem) {

        const QTalk::Entity::ImMessageInfo &stMessage = msgItemWgt->msgInfo();
        pLstItem->setData(EM_DATE_TYPE_TIME, stMessage.LastUpdateTime);
        pLstItem->setData(EM_DATETYPE_MSGTYPE, static_cast<long long>(msgItemWgt->type()));

        switch (msgItemWgt->type()) {
            case QTalk::Entity::MessageTypeText:
            case QTalk::Entity::MessageTypeRobotAnswer:
            case QTalk::Entity::MessageTypeGroupAt:
            case QTalk::Entity::MessageTypeImageNew:
            case QTalk::Entity::MessageTypePhoto:
            case QTalk::Entity::MessageTypeGroupNotify:
            case QTalk::Entity::MessageTypeFile:
            case QTalk::Entity::MessageTypeCommonTrdInfo:
            case QTalk::Entity::MessageTypeCommonTrdInfoV2:
            default: {
                QSize size = msgItemWgt->itemWdtSize();
                pLstItem->setSizeHint(size);
                break;
            }
        }

        int scroBarval = this->verticalScrollBar()->value();
        int maximum = this->verticalScrollBar()->maximum();
        this->update();
        // 判断是否是首次创建item
        if (_items.contains(msgItemWgt)) {
//            if ((maximum - scroBarval) < 50)
//                this->scrollToBottom();
            return;
        }
        //
        _items.insert(msgItemWgt, pLstItem);
        msgItemWgt->showShareCheckBtn(_selectEnable);
        connect(msgItemWgt, &MessageItemBase::sgSelectItem, [this, pLstItem](bool checked){
            disconnect(this, &QListWidget::itemSelectionChanged, this, &ChatMainWgt::onItemSelectionChanged);
            this->setItemSelected(pLstItem, checked);
            connect(this, &QListWidget::itemSelectionChanged, this, &ChatMainWgt::onItemSelectionChanged);
            emit sgSelectedSize(selectedItems().size());
        });


        if (QTalk::Entity::MessageTypeText == msgItemWgt->type() ||
            QTalk::Entity::MessageTypeGroupAt == msgItemWgt->type()) {
            //
            if (stMessage.ChatType == QTalk::Enum::GroupChat &&
                stMessage.Direction == QTalk::Entity::MessageDirectionReceive &&
                !((bool) (stMessage.ReadedTag & MessageItemBase::EM_READSTS_READED))) {
                if (QString::fromStdString(stMessage.Content).contains(DEM_ATALL_STR)) {
                    StShowAtInfo info(true, QString::fromStdString(stMessage.UserName), pLstItem);
                    _pAtMessageTipItem->addAt(info);
                } else {
                    if (!_selfUserName.isEmpty() &&
                        QString::fromStdString(stMessage.Content).contains(QString("@%1").arg(_selfUserName))) {
                        StShowAtInfo info(false, QString::fromStdString(stMessage.UserName), pLstItem);
                        _pAtMessageTipItem->addAt(info);
                    }
                }
            }
        }
        //
        if (isHistoryMessage) {
            auto index = get_index(this, stMessage.LastUpdateTime, isHistoryMessage);
            this->insertItem(index, pLstItem);
            this->scrollToItem(pLstItem, QAbstractItemView::PositionAtTop);
        } else {
            this->addItem(pLstItem);
            // 自己发消息时 或 当前就在最底端
            if (stMessage.Direction == QTalk::Entity::MessageDirectionSent || (maximum - scroBarval) < 300) {
                this->scrollToBottom();
            } else {
                if (scroBarval != maximum && _pNewMessageTipItem) {
                    _pNewMessageTipItem->onNewMessage();
                }
            }
        }
        info_log("show message id:{0}", stMessage.MsgId);
        setItemWidget(pLstItem, msgItemWgt);
        this->update();
    }
}

void ChatMainWgt::resizeItems() {
    for (int i = 0; i < count(); i++) {
        QListWidgetItem *item = this->item(i);
        item->setSizeHint(QSize(this->size().width(), item->sizeHint().height()));
    }
}

void ChatMainWgt::onRecvReadState(const std::map<std::string, QInt32> &readStates) {
    auto it = readStates.begin();
    for (; it != readStates.end(); it++) {

        QString messageId = QString::fromStdString(it->first);
        for(MessageItemBase *item : _itemWgts.values(messageId)) {
            item->setReadState(it->second);
        }
    }
}

void ChatMainWgt::onRecvGroupMState(const std::vector<std::string> &msgIds)
{
    auto it = msgIds.begin();
    for (; it != msgIds.end(); it++) {

        QString msgId = it->data();
        for(MessageItemBase *item : _itemWgts.values(msgId)) {
            item->setReadState(0x01);
        }
    }
}

void ChatMainWgt::updateRevokeMessage(const QString &fromId, const QString &messageId, const long long& time) {
    if (_itemWgts.contains(messageId)) {
        QMutexLocker locker(&_mutex);
        for (MessageItemBase *itemWgt : _itemWgts.values(messageId)) {

            QListWidgetItem *item = _items[itemWgt];
            item->setHidden(true);
        }
        _itemWgts.remove(messageId);

        if (!fromId.isEmpty()) {
            QString userName = tr("你");
            if (g_pMainPanel->getSelfUserId() != fromId.toStdString()) {
                std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo = dbPlatForm::instance().getUserInfo(
                        fromId.toStdString());
                if (userInfo) {
                    userName = QString::fromStdString(QTalk::getUserName(userInfo));
                }
            }
            showRevokeMessage(userName, false, time);
        }
    }
}

void ChatMainWgt::showRevokeMessage(const QString &userName, bool isHistory, QInt64 t) {
    showTipMessage(QTalk::Entity::MessageTypeRevoke, QString("%1撤回了一条消息").arg(userName), isHistory, t);
}

//
void ChatMainWgt::showTipMessage(long long type, const QString &content, bool isHistory, QInt64 t) {
    auto *tipItem = new TipMessageItem(this);
    auto *item = new QListWidgetItem;
    item->setData(EM_DATETYPE_MSGTYPE, type);
    item->setData(EM_DATE_TYPE_TIME, t);
    QSize size = QSize(this->width(), 40);
    item->setSizeHint(size);

    tipItem->setText(content);

    if (isHistory) {
        auto index = get_index(this, t, isHistory);
        this->insertItem(index, item);
        this->scrollToItem(item, QAbstractItemView::PositionAtBottom);
    } else {
        this->addItem(item);
        int scroBarval = this->verticalScrollBar()->value();
        int maximum = this->verticalScrollBar()->maximum();
        if ((maximum - scroBarval) < 300) {
            this->scrollToBottom();
        } else {
            if (scroBarval != maximum && _pNewMessageTipItem) {
                _pNewMessageTipItem->onNewMessage();
            }
        }
    }

    setItemWidget(item, tipItem);
}

void ChatMainWgt::saveAsImage(const QString &imageLink) {

    if(imageLink.isEmpty())
    {
        LiteMessageBox::success(QString(tr("无效图片:%1")).arg(imageLink));
        return;
    }

    std::string imageLocalFile = g_pMainPanel->getMessageManager()->getSouceImagePath(imageLink.toStdString());
    QString oldFilePath = QString::fromStdString(imageLocalFile);
    QFileInfo oldFileInfo(oldFilePath);
    if (!oldFilePath.isEmpty() && oldFileInfo.exists()) {
        QString suffix = QTalk::qimage::instance().getRealImageSuffix(oldFilePath).toLower();
        QString hisDir = QString::fromStdString(Platform::instance().getHistoryDir());

        if(suffix.toLower() == "webp")
        {
            QString newPath = QFileDialog::getSaveFileName(this, tr("请选择文件保存路径"),
                                                           QString("%1/%2").arg(hisDir, oldFileInfo.baseName()),
                                                           QString("(*.png);;(*.jpg);;(*.webp)"));
            if (!newPath.isEmpty()) {
                Platform::instance().setHistoryDir(QFileInfo(newPath).absoluteDir().absolutePath().toStdString());
                QString newSuffix = QFileInfo(newPath).suffix().toUpper();
                //
                auto tmpPix = QTalk::qimage::instance().loadPixmap(oldFilePath, false);
                if(!tmpPix.isNull())
                {
                    auto format = newSuffix.toUtf8().data();
                    tmpPix.save(newPath, format, 100);
                    LiteMessageBox::success(QString(tr("文件已另存为:%1")).arg(newPath));
                }
            }
        }
        else
        {
            QString saveDir = QFileDialog::getSaveFileName(this, tr("请选择文件保存路径"),
                                                           QString("%1/%2").arg(hisDir, oldFileInfo.baseName()),
                                                           QString("%1 (*.%1)").arg(suffix.isEmpty() ? "*" : suffix));
            if (!saveDir.isEmpty()) {
                Platform::instance().setHistoryDir(QFileInfo(saveDir).absoluteDir().absolutePath().toStdString());
                QString newPath = QString("%1").arg(saveDir);
                if(QFileInfo(newPath).suffix().isEmpty() && !oldFileInfo.suffix().isEmpty())
                    newPath += QString(".%1").arg(oldFileInfo.suffix());

                QFile::copy(oldFilePath, newPath);
                LiteMessageBox::success(QString(tr("文件已另存为:%1")).arg(newPath));
            }
        }
    }
}

void copyImage(const QString& imageLink, const QString& imagePath)
{
    std::string srcImgPath = g_pMainPanel->getMessageManager()->getSouceImagePath(imageLink.toStdString());
    QFileInfo srcInfo(srcImgPath.data());

    auto *mimeData = new QMimeData;
//    QList<QUrl> urls;
//
//    if(srcInfo.exists() && srcInfo.isFile())
//        urls << QUrl::fromLocalFile(srcImgPath.data());
//    else
//        urls << QUrl::fromLocalFile(imagePath);

//    mimeData->setUrls(urls);
    QPixmap pixmap = QTalk::qimage::instance().loadPixmap(srcImgPath.data(), false);
    mimeData->setImageData(pixmap.toImage());
    QApplication::clipboard()->setMimeData(mimeData);
}

// action 处理相关
void ChatMainWgt::onCopyAction(bool) {
    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;

    switch (itemWgt->type()) {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt: {
            auto *textWgt = qobject_cast<TextMessItem *>(itemWgt);
            if (nullptr == textWgt) return;
            textWgt->copyText();
            break;
        }
        case QTalk::Entity::MessageTypePhoto:
        case QTalk::Entity::MessageTypeImageNew:{
            auto *item = qobject_cast<ImageMessItem *>(itemWgt);
            if (item) {
                copyImage(item->_imageLink, item->_imagePath);
                break;
            }
        }
        default:
            return;
    }
}

// action 处理相关
void ChatMainWgt::onSaveAsAction(bool) {
    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;
    switch (itemWgt->type()) {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt: {
            auto *textItem = qobject_cast<TextMessItem *>(itemWgt);
            if (textItem) {
                QString imageLink = textItem->getImageLink();
                saveAsImage(imageLink);
            }
            break;
        }
        case QTalk::Entity::MessageTypePhoto:
        case QTalk::Entity::MessageTypeImageNew:{
            auto *imgItem = qobject_cast<ImageMessItem *>(itemWgt);
            if (imgItem)
                saveAsImage(imgItem->_imageLink);
            break;
        }
        case QTalk::Entity::MessageTypeFile: {
            auto *fileItem = qobject_cast<FileSendReceiveMessItem *>(itemWgt);
            if (fileItem) {
                fileItem->onSaveAsAct();
            }
            break;
        }
        default:
            return;
    }
}

// action 处理相关
void ChatMainWgt::onRevokeAction(bool) {
    emit g_pMainPanel->sgOperator(tr("撤回消息"));

    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;

    QInt64 now = QDateTime::currentDateTime().toMSecsSinceEpoch() - Platform::instance().getServerDiffTime() * 1000;
    if (now - itemWgt->msgInfo().LastUpdateTime > 2 * 60 * 1000) {
        // 时间戳传现在 可能有点问题 todo
        showTipMessage(QTalk::Entity::MessageTypeGroupNotify, tr("超过2分钟的消息不能撤回"), false, now);
        return;
    }

    QString messageId = QString::fromStdString(itemWgt->msgInfo().MsgId);
    auto time = QDateTime::currentMSecsSinceEpoch() - Platform::instance().getServerDiffTime() * 1000;
    updateRevokeMessage(QString::fromStdString(g_pMainPanel->getSelfUserId()), messageId, time);
    if (nullptr != g_pMainPanel && nullptr != g_pMainPanel->getMessageManager()) {
        g_pMainPanel->getMessageManager()->sendRevokeMessage(_pViewItem->_uid, g_pMainPanel->getSelfUserId(),
                                                             messageId.toStdString(),_pViewItem->_chatType);
    }
}

void ChatMainWgt::onQuoteAct(bool) {
    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;
    //
//    QString userName = QString::fromStdString(itemWgt->msgInfo().UserName);
    QTalk::Entity::JID jid(itemWgt->msgInfo().From.data());
    auto info = dbPlatForm::instance().getUserInfo(jid.barename());
    if(info)
    {
        QString userName = QString::fromStdString(info->NickName.empty() ? info->Name : info->NickName);
        //
        QString source = QString::fromStdString(itemWgt->msgInfo().Content);
        _pViewItem->_pInputWgt->insertQuote(userName, source);

        emit g_pMainPanel->sgOperator(tr("引用"));
    }
    else
    {
        warn_log("user info not exist user:{0}", itemWgt->msgInfo().From);
    }
}

void ChatMainWgt::onCollectionAct(bool) {
    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;
    //
    auto type = itemWgt->type();
    QString netPath;
    switch(type)
    {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt:
        {
            auto *textItem = qobject_cast<TextMessItem *>(itemWgt);
            netPath = textItem->getImageLink();
            break;
        }
        case QTalk::Entity::MessageTypeImageNew:
        case QTalk::Entity::MessageTypePhoto:
        {
            auto *imgItem = qobject_cast<ImageMessItem *>(itemWgt);
            if(imgItem)
                netPath = imgItem->_imageLink;
            break;
        }
        default:
            break;
    }

    QString name = QString::fromStdString(QTalk::GetFileNameByUrl(netPath.toStdString()));
    QString baseName = QFileInfo(name).baseName();
    //
    g_pMainPanel->addConllection(baseName, netPath);
}

void ChatMainWgt::onQRCodeAct(bool)
{
    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;
    //
    auto type = itemWgt->type();
    QString netPath;
    switch(type)
    {
        case QTalk::Entity::MessageTypeText:
        case QTalk::Entity::MessageTypeRobotAnswer:
        case QTalk::Entity::MessageTypeGroupAt:
        {
            auto *textItem = qobject_cast<TextMessItem *>(itemWgt);
            netPath = textItem->getImageLink();
            break;
        }
        case QTalk::Entity::MessageTypeImageNew:
        case QTalk::Entity::MessageTypePhoto:
        {
            auto *imgItem = qobject_cast<ImageMessItem *>(itemWgt);
            netPath = imgItem->_imageLink;
            break;
        }
        default:
            break;
    }
    //
    std::string imageLocalFile = g_pMainPanel->getMessageManager()->getSouceImagePath(netPath.toStdString());

    if(!imageLocalFile.empty() && QFile::exists(imageLocalFile.data()) && g_pMainPanel)
    {
        g_pMainPanel->scanQRcCodeImage(imageLocalFile.data());
    }
}

void ChatMainWgt::recvBlackListMessage(const QString &messageId) {
    if (_itemWgts.contains(messageId)) {
        for (MessageItemBase *item : _itemWgts.values(messageId)) {
            item->setReadState(MessageItemBase::EM_BLACK_RET);
            emit showTipMessageSignal(QTalk::Entity::MessageTypeGroupNotify, tr("对方已将你的消息屏蔽, 消息已被拦截"), false,
                    QDateTime::currentMSecsSinceEpoch() - Platform::instance().getServerDiffTime() * 1000);
        }
    }
}

/**
 *
 */
void ChatMainWgt::onForwardAct(bool) {
    auto *itemWgt = qobject_cast<MessageItemBase *>(itemWidget(currentItem()));
    if (nullptr == itemWgt) return;

    QTalk::Entity::ImMessageInfo stMsg = itemWgt->msgInfo();
    g_pMainPanel->forwardMessage(stMsg.MsgId);
    emit g_pMainPanel->sgOperator(tr("转发消息"));
}

/**
 * 
 */
void ChatMainWgt::onScrollBarChanged(int val) {
    if (_oldScrollBarVal - val > 0) {
        if (val > 5)
            return;

        qint64 curTime = QDateTime::currentMSecsSinceEpoch();
        if(curTime - _historyTime < 2000)
        {
//            warn_log("getHistory time too short {0}", curTime - _historyTime);
            return;
        }
        else
            _historyTime = curTime;

        QInt64 time = 0;
        if (count() > 0) {
            int i = 0;

            while (true) {
                long long type = this->item(i)->data(EM_DATETYPE_MSGTYPE).toLongLong();
                if (type != QTalk::Entity::MessageTypeTime) {
                    time = this->item(i)->data(EM_DATE_TYPE_TIME).toLongLong();
                    if (time != 0) {
                        info_log("get history message type: {0} time: {1}", type, QDateTime::fromMSecsSinceEpoch(time).toString("yyyy-MM-dd hh:mm:ss").toStdString());
                        break;
                    }
                }

                if (i == this->count() - 1)
                    break;
                i++;
            }
        }
        if(_hasnotHistoryMsg)
            g_pMainPanel->getHistoryMsg(time, _pViewItem->_chatType, _pViewItem->getPeerId());

    } else {
        int maximum = this->verticalScrollBar()->maximum();
        if (_pNewMessageTipItem && maximum - val < 5) {
            _pNewMessageTipItem->onResetWnd();
        }
    }
    _oldScrollBarVal = val;
}

void ChatMainWgt::onAdjustItems() {

    for (int i = 0; i < count(); i++) {
        auto *itemWgt = (QFrame *) itemWidget(item(i));
        QListWidgetItem *item = this->item(i);
        //
//        static int index = 0;
//        index = 0;
        //
        if (itemWgt && item && !item->isHidden()) {
            long long itemType = item->data(EM_DATETYPE_MSGTYPE).toLongLong();
            if (QTalk::Entity::MessageTypeText == itemType ||
                QTalk::Entity::MessageTypeGroupAt == itemType ||
                QTalk::Entity::MessageTypeRobotAnswer == itemType) {

                auto *chatItem = (TextMessItem *) itemWgt;
                chatItem->setMessageContent();
                dealMessage(chatItem, item, false);

//                if(++index == 10)
//                {
//                    QApplication::processEvents(QEventLoop::AllEvents, 200);
//                    index = 0;
//                }
            }
        }
    }
    resizeItems();
    _pAtMessageTipItem->showAtInfo();
    _pNewMessageTipItem->onResize();
}

void ChatMainWgt::setShareMessageState(bool flag)
{
    if(_selectEnable == flag)
        return;

    _selectEnable = flag;
    if(flag)
        this->setSelectionMode(QAbstractItemView::MultiSelection);
    else
    {
        this->clearSelection();
        this->setSelectionMode(QAbstractItemView::NoSelection);
    }

    for(const auto& item : _itemWgts)
    {
        item->showShareCheckBtn(flag);
        item->checkShareCheckBtn(false);
    }
}

void ChatMainWgt::onItemChanged()
{
    auto it = _items.begin();
    for(; it != _items.end(); it++)
    {
        bool isSelected = (*it)->isSelected();
        it.key()->checkShareCheckBtn(isSelected);
    }
    emit sgSelectedSize(selectedItems().size());
}

void ChatMainWgt::onItemSelectionChanged()
{
    _selectItemQueue->push(true);
}

/**
 *
 */
void ChatMainWgt::onShareMessage()
{
    if(selectedItems().empty())
    {
        QtMessageBox::warning(this, tr("警告"), tr("请选择需要转发的消息"));
        return;
    }

    std::vector<QTalk::Entity::ImMessageInfo> arMsgs;
    auto it = _items.begin();
    for(; it != _items.end(); it++)
    {
        if((*it)->isSelected())
            arMsgs.push_back(it.key()->msgInfo());
    }
    if(arMsgs.empty())
    {
        QtMessageBox::warning(this, tr("警告"), tr("请选择有效的消息"));
        return;
    }
    //
    if(_pViewItem)
    {
        _pViewItem->setShareMessageState(false);
    }
    //
//    LiteMessageBox::success("消息正在打包发送中...", 1000);
    //
    QPointer<ChatMainWgt> pThis(this);
    g_pMainPanel->pool().enqueue([pThis, arMsgs]() {
#ifdef _MACOS
        pthread_setname_np("ChatMainWgt onShareMessage thread");
#endif
        cJSON* objs = cJSON_CreateArray();

        for(const auto& msg : arMsgs)
        {
            cJSON* obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "d", msg.Direction);
            if(msg.ExtendedInfo.empty())
                cJSON_AddStringToObject(obj, "b", msg.Content.data());
            else
                cJSON_AddStringToObject(obj, "b", msg.ExtendedInfo.data());
            cJSON_AddStringToObject(obj, "n", QTalk::getUserNameNoMask(msg.From).data());
            cJSON_AddNumberToObject(obj, "s", msg.LastUpdateTime);
            cJSON_AddNumberToObject(obj, "t", msg.Type);

            cJSON_AddItemToArray(objs, obj);
        }

        std::string jsonMsg = QTalk::JSON::cJSON_to_string(objs);
        cJSON_Delete(objs);
        QString jsonFilePath = QString("%1/json_msg").arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
        QFile tmpFile(jsonFilePath);
        if(tmpFile.open(QIODevice::WriteOnly))
        {
            tmpFile.resize(0);
            tmpFile.write(jsonMsg.data(), jsonMsg.size());
            tmpFile.close();
        }
        //
        if(g_pMainPanel)
        {
            std::string url = g_pMainPanel->uploadFile(jsonFilePath.toStdString());
            if(pThis && pThis->_pViewItem)
                emit pThis->sgUploadShareMsgSuccess(pThis->_pViewItem->_uid.toQString(), pThis->_pViewItem->_chatType, QString::fromStdString(url));
        }
    });
}

bool ChatMainWgt::event(QEvent *e) {

    if(e->type() == QEvent::Enter)
    {
        this->verticalScrollBar()->setVisible(true);
    }
    else if(e->type() == QEvent::Leave)
    {
        this->verticalScrollBar()->setVisible(false);
    }

    return QListWidget::event(e);
}

/**
 * 更新群聊 尾巴状态
 */
void ChatMainWgt::onChangeUserMood(const std::string &id, const std::string &mood)
{
//    for(const auto item : _itemWgts.values())
//    {
//        std::string xmppId = item->msgInfo().From;
//        if(id == xmppId)
//        {
//            emit item->sgChangeUserMood(mood);
//        }
//    }
}

//
void ChatMainWgt::onUserMedalChanged(const std::set<std::string> &changedUser) {
    auto items = _itemWgts.values();
    for(auto* wgt : items)
    {
        if(wgt && wgt->msgInfo().Direction == QTalk::Entity::MessageDirectionReceive)
        {
            const auto& id = wgt->msgInfo().From;
            if(changedUser.find(id) != changedUser.end())
                emit wgt->sgUpdateUserMedal();
        }
    }
}

/**
 *
 * @param link
 * @param msgid
 */
void ChatMainWgt::downloadImage(const QString &msgid, const QString &link, int width, int height)
{
    QPointer<ChatMainWgt> pThis(this);
    g_pMainPanel->pool().enqueue([pThis, link, msgid, width, height](){
        g_pMainPanel->getMessageManager()->getLocalFilePath(link.toStdString());
        if(pThis)
            emit pThis->sgImageDownloaded(msgid, link);
    });

}

void ChatMainWgt::onImageDownloaded(const QString &msgid, const QString &path)
{
    if(_itemWgts.contains(msgid))
    {
        QList<MessageItemBase*> items = _itemWgts.values(msgid);
        for(MessageItemBase* item : items)
        {
            if( QTalk::Entity::MessageTypePhoto == item->type() ||
                    QTalk::Entity::MessageTypeImageNew == item->type())
            {
                auto* imgWgtItem = qobject_cast<ImageMessItem*>(item);
                if(imgWgtItem)
                    imgWgtItem->onImageDownloaded();
                else
                {
                    auto* emoWgtItem = qobject_cast<EmojiMessItem*>(item);
                    if(emoWgtItem)
                        emoWgtItem->onImageDownloaded(path);
                }
            }
            else if(QTalk::Entity::MessageTypeRobotAnswerList == item->type())
            {
                auto* ansItem = qobject_cast<HotLineAnswerItem*>(item);
                if(ansItem)
                    ansItem->onImageDownloaded(path);
            }
            // path 为下载地址
            else /*if(QTalk::Entity::MessageTypeText == item->type())*/
            {
                auto* textWgtItem = qobject_cast<TextMessItem*>(item);
                if(textWgtItem)
                    textWgtItem->onImageDownloaded(path);
            }
        }

        _resizeQueue->push(true);
    }
}

/**
 *
 * @param pkgid
 * @param shortCut
 */
void ChatMainWgt::downloadEmoticon(const QString& msgId, const QString& pkgid, const QString& shortCut)
{
    QPointer<ChatMainWgt> pThis(this);
    g_pMainPanel->pool().enqueue([pThis, msgId, pkgid, shortCut](){
        QString localPath = EmoticonMainWgt::getInstance()->downloadEmoticon(pkgid, shortCut);
        if(pThis)
            emit pThis->sgImageDownloaded(msgId, localPath);
    });
}

bool ChatMainWgt::positionIsTop() {
    int scrollBarVal = this->verticalScrollBar()->value();
    int maximum = this->verticalScrollBar()->maximum();

    if(scrollBarVal == maximum == 0)
        return true;
    else
        return scrollBarVal <= 5;
}

bool ChatMainWgt::positionIsBottom() {
    int scrollBarVal = this->verticalScrollBar()->value();
    int maximum = this->verticalScrollBar()->maximum();

    if(scrollBarVal == maximum == 0)
        return true;
    else
        return scrollBarVal >= maximum - 5;
}

void ChatMainWgt::onDisconnected()
{
    setConnectState(false);
    //
    for(auto *wgt : _itemWgts)
    {
        if(wgt)
            wgt->onDisconnected();
    }
}

void ChatMainWgt::onSendMessageFailed(const QString &msgId) {
    for(auto *wgt : _itemWgts.values(msgId))
    {
        if(wgt)
            wgt->onDisconnected();
    }
}

void ChatMainWgt::clearData()
{
    _hasnotHistoryMsg = true;
    _itemWgts.clear();
    _items.clear();
    _fileItems.clear();
    _videoItems.clear();
    _arHistoryIds.clear();
    //
//    this->disconnect(this);
    this->clear();
//    this->connects();
}

void ChatMainWgt::onMState(const QString& msgId, const long long& time) {
    for(MessageItemBase *wgt : _itemWgts.values(msgId)) {
        wgt->setReadState(0x01);
        _items[wgt]->setData(ChatMainWgt::EM_DATE_TYPE_TIME, time);
    }
}
