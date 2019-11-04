#include "SessionFrm.h"
#include <QFileInfo>
#include <QEvent>
#include <QMenu>
#include <QAction>
#include <QTime>
#include <QDebug>
#include <iostream>
#include <QJsonObject>
#include <thread>
#include <QTimer>
#include <QScrollBar>
#include <set>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtConcurrent>
#include "../CustomUi/HeadPhotoLab.h"
#include "../UICom/UIEntity.h"
#include "../Message/ChatMessage.h"
#include "../Platform/Platform.h"
#include "SessionitemDelegate.h"
#include "../QtUtil/Entity/JID.h"
#include "../QtUtil/Utils/Log.h"
#include "../QtUtil/Utils/utils.h"
#include "NavigationMianPanel.h"
#include "../Platform/dbPlatForm.h"
#include "../include/perfcounter.h"
#include "../CustomUi/QtMessageBox.h"

#define SYSTEM_XMPPID "SystemMessage"
#define RBT_SYSTEM "rbt-system"
#define RBT_NOTICE "rbt-notice"
#define RBT_QIANGDAN "rbt-qiangdan"
#define RBT_NOTICE_NAME "公告消息"
#define RBT_QIANGDAN_NAME "抢单消息"
#define SYSTEM_NAME  "系统消息"

using namespace QTalk::Entity;

SessionFrm::SessionFrm(NavigationMianPanel *parent) :
        QFrame(nullptr),
        _sessionmainLayout(nullptr),
        _pSessionView(nullptr),
        _normalTopRow(0),
        _messageManager(nullptr),
        _mainPanel(parent),
        _jumpIndex(0),
        pSessions(nullptr) {
    init();
}

SessionFrm::~SessionFrm() = default;

void SessionFrm::connects() {

    connect(_pSessionView, &QListView::pressed,
            this, &SessionFrm::onItemSelectChanged);
    connect(_showCardAct, &QAction::triggered, this, &SessionFrm::onShowCardAct);
    //
    connect(_closeSessionAct, &QAction::triggered, this, &SessionFrm::onCloseSession);
    //
    connect(_toTopOrCancelTopAct, &QAction::triggered, this, &SessionFrm::onToTopAct);
    //
    connect(_noticeAct, &QAction::triggered, this, &SessionFrm::onUnNoticeAct);
    //
    connect(_addFriendAct, &QAction::triggered, this, &SessionFrm::onAddFriendAct);
    connect(_starAct, &QAction::triggered, this, &SessionFrm::onStarAct);
    connect(_blackAct, &QAction::triggered, this, &SessionFrm::onBlackAct);
    connect(_clearUnreadAct, &QAction::triggered, this, &SessionFrm::onClearUnreadAct);
    connect(_quitGroupAct, &QAction::triggered, this, &SessionFrm::onQuitGroupAct);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.29
  */
void SessionFrm::setMessageManager(NavigationMsgManager *messageManager) {
    _messageManager = messageManager;
}

/**
  * @函数名
  * @功能描述 处理
  * @参数
  * @author cc
  * @date 2018.9.20
  */
void SessionFrm::onReceiveSession(const ReceiveSession &mess, bool isSend) {
    perf_counter_warning(100, "onReceiveSession:{0}", mess.xmppId.toStdString());

//    if (_historyMessageId.contains(mess.messageId)) {
//        return;
//    } else {
//        _historyMessageId.push_back(mess.messageId);
//    }

    QString userId = mess.xmppId;
    QString realJid = (mess.realJid.isNull() || mess.realJid.isEmpty()) ? mess.xmppId : mess.realJid;
    UID uid(userId, realJid);
    QString name;

#ifdef _STARTALK
    QString headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
    QString headPath = ":/QTalk/image1/headPortrait.png";
#endif
    if (mess.chatType == QTalk::Enum::GroupChat) {
#ifdef _STARTALK
        headPath = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
        headPath = ":/QTalk/image1/defaultGroupHead.png";
#endif
        int pos = userId.indexOf("/");
        if (-1 != pos) {
            userId = userId.left(pos);
        }
        std::shared_ptr<QTalk::Entity::ImGroupInfo> groupInfo =
                dbPlatForm::instance().getGroupInfo(userId.toStdString(), true);
        if (groupInfo) {
            name = QString::fromStdString(groupInfo->Name);
            QString userpath = QString::fromStdString(Platform::instance().getAppdataRoamingUserPath());
            QString headfilename = QString::fromStdString(groupInfo->HeaderSrc);
            if (headfilename.isEmpty()) {
                _normalGroupHeadPhotos.push_back(userId.toStdString());
            } else {
                GenerateHeadPhotoName(headfilename);

                QString rheadpath = userpath + "/image/headphoto/" + headfilename;
                QFileInfo headfile(rheadpath);
                if (headfile.isFile() && headfile.exists()) {
                    headPath = rheadpath;
                } else {
                    _withoutGroupHeadPhotos.push_back(userId.toStdString());
                    _withoutGroupHeadPhotoSrcs.push_back(groupInfo->HeaderSrc);
                }
            }
        }
    } else {
        int pos = userId.indexOf("/V");
        if (-1 != pos) {
            userId = userId.left(pos);
        }
        //客服consult消息 头像要显示客人 而不是店铺
        std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo =
                dbPlatForm::instance().getUserInfo(
                        mess.chatType == QTalk::Enum::ConsultServer ? realJid.toStdString() : userId.toStdString(),
                        true);
        if (userInfo) {
//            name = QString::fromStdString(QTalk::getUserName(userInfo));
            QString userpath = QString::fromStdString(Platform::instance().getAppdataRoamingUserPath());
            QString headfilename = QString::fromStdString(userInfo->HeaderSrc);
            if (headfilename.isEmpty() && (userInfo->HeadVersion != 0)) {
                _normalHeadPhotos.push_back(userId.toStdString());
            } else {
                GenerateHeadPhotoName(headfilename);
                QString rheadpath = userpath + "/image/headphoto/" + headfilename;
                QFileInfo headfile(rheadpath);
                if (headfile.isFile() && headfile.exists()) {
                    headPath = rheadpath;
                } else {
                    _withoutHeadPhotos.push_back(userId.toStdString());
                }
            }
        }
    }

    QStandardItem *item = _sessionMap.value(uid);
    bool notCurItem = true;
    bool unNotice = _mapNotice.find(userId.toStdString()) != _mapNotice.end();
    if (nullptr != item) {
        ////info_log("会话已存在:{0}", mess.messageId);
        {
            QMutexLocker locker(&_mutex);
            notCurItem = !(UID(userId, realJid) == _curUserId);
        }

        if (mess.sendJid != _strSelfId) {
            if (notCurItem) {
                if (mess.chatType == QTalk::Enum::GroupChat) {
                    unsigned int atCount = item->data(ITEM_DATATYPE_ATCOUNT).toUInt();
                    if (mess.messageContent.contains("@all"))
                        atCount |= 0x000F;
                    else if (mess.messageContent.contains(
                            QString("@%1").arg(Platform::instance().getSelfName().data())))
                        atCount |= 0x00F0;

                    item->setData(atCount, ITEM_DATATYPE_ATCOUNT);
                }
            } else {
                if (_messageManager) {
                    // 发送消息已读
                    info_log("send read mask u:{0} messageId:{1}", realJid.toStdString(), mess.messageId.toStdString());
                    _messageManager->sendReadedMessage(mess.messageId.toStdString(), realJid.toStdString(),
                                                       mess.chatType);
                }
            }
        }

    } else //新会话需要创建
    {
        //info_log("创建新会话:{0}", mess.messageId);
        item = new QStandardItem;
        item->setData(mess.chatType, ITEM_DATATYPE_CHATTYPE);
        item->setData(userId, ITEM_DATATYPE_USERID);
        item->setData(realJid, ITEM_DATATYPE_REALJID);

        item->setData(_mapStick.find(uid.toStdString()) != _mapStick.end(), ITEM_DATATYPE_ISTOP);
        item->setData(unNotice, ITEM_DATATYPE_UNNOTICE);

        //
        QTalk::Entity::JID jid(userId.toStdString().data());
        std::string id = jid.barename();
        if (SYSTEM_XMPPID == jid.username() || RBT_SYSTEM == jid.username()) {
            QString syshead = ":/UINavigationPlug/image1/system.png";
            item->setData(tr(SYSTEM_NAME), ITEM_DATATYPE_USERNAME);
            item->setData(syshead, ITEM_DATATYPE_HEADPATH);
            item->setData(true, ITEM_DATATYPE_ISONLINE);
        } else if (RBT_NOTICE == jid.username()) {
            QString noticehead = ":/UINavigationPlug/image1/atom_ui_rbt_notice.png";
            item->setData(tr(RBT_NOTICE_NAME), ITEM_DATATYPE_USERNAME);
            item->setData(noticehead, ITEM_DATATYPE_HEADPATH);
            item->setData(true, ITEM_DATATYPE_ISONLINE);
        } else if(RBT_QIANGDAN == jid.username()){
            QString qdhead = ":/UINavigationPlug/image1/atom_ui_robot_qiangdan.png";
            item->setData(tr(RBT_QIANGDAN_NAME), ITEM_DATATYPE_USERNAME);
            item->setData(qdhead, ITEM_DATATYPE_HEADPATH);
            item->setData(true, ITEM_DATATYPE_ISONLINE);
        } else {
            if (name.isEmpty()) {
                name = getUserName(id, mess.chatType == QTalk::Enum::GroupChat);
            }
            item->setData((name.isEmpty() ? QString::fromStdString(id) : name), ITEM_DATATYPE_USERNAME);
            Q_ASSERT(!headPath.startsWith("http"));
            item->setData(headPath, ITEM_DATATYPE_HEADPATH);
            item->setToolTip(name);

            if (mess.chatType == QTalk::Enum::GroupChat ||
                id == Platform::instance().getSelfUserId()) {
                item->setData(true, ITEM_DATATYPE_ISONLINE);
            } else {
                bool isOnline = Platform::instance().isOnline(id);
                item->setData(isOnline, ITEM_DATATYPE_ISONLINE);
            }
        }
        // 显示新会话

        _sessionMap.insert(uid, item);
        _pSrcModel->appendRow(item);
        //info_log("显示新回话: {0}", mess.messageId);
    }
    item->setData(mess.messageId, ITEM_DATATYPE_LAST_MESSAGE_ID);
    item->setData(mess.messageRecvTime, ITEM_DATATYPE_LASTTIME);
    item->setData(GenerateTimeText(mess.messageRecvTime), ITEM_DATATYPE_LASTSTRTIME);

    if (!isSend && notCurItem) {
        unsigned int count = item->data(ITEM_DATATYPE_UNREADCOUNT).toUInt();
        item->setData(++count, ITEM_DATATYPE_UNREADCOUNT);
        if (!unNotice) {
            _totalUnReadCount++;
            emit _mainPanel->updateTotalUnreadCount(_totalUnReadCount);
        }
    }

    QString userName;
    if (mess.chatType == QTalk::Enum::GroupChat) {
        std::shared_ptr<QTalk::Entity::ImUserInfo> info
                = dbPlatForm::instance().getUserInfo(mess.sendJid.toStdString());
        if (info) {
            userName = QString::fromStdString(QTalk::getUserName(info));
        }
    }

    item->setData(GenerateContent(mess.messageContent, mess.chatType, mess.messtype,
                                  userName), ITEM_DATATYPE_MESSAGECONTENT);
    // sort
    _pSessionView->update();
    _pModel->sort(0);
    //
    if (_messageManager) {
        _messageManager->sendDownLoadHeadPhotosMsg(_withoutHeadPhotos, _withoutGroupHeadPhotoSrcs);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.27
  */
void SessionFrm::onloadSessionData() {
    //
    QMutexLocker locker(&_mutex);
    _totalUnReadCount = 0;
    _normalHeadPhotos.clear();
    _withoutHeadPhotos.clear();
    _normalGroupHeadPhotos.clear();
    _withoutGroupHeadPhotos.clear();
    _withoutGroupHeadPhotoSrcs.clear();
    if (pSessions) {
        // 处理置顶窗口却没有在sessionlist里的session
        {
            auto it = _mapStick.begin();
            for (; it != _mapStick.end(); it++) {
                const std::string &xmppId = it->first;
                UID uid(xmppId);
                auto itFind = std::find_if(pSessions->begin(), pSessions->end(),
                                           [uid](std::shared_ptr<QTalk::Entity::ImSessionInfo> session) {
                                               return session->XmppId == uid.usrId();
                                           });
                if (itFind == pSessions->end()) {
                    QJsonParseError jsoErr{};
                    QJsonDocument jsoDoc = QJsonDocument::fromJson(it->second.data(), &jsoErr);
                    if (jsoErr.error == QJsonParseError::NoError) {
                        QJsonObject obj = jsoDoc.object();
                        std::shared_ptr<QTalk::Entity::ImSessionInfo> session(new QTalk::Entity::ImSessionInfo);
                        session->XmppId = uid.usrId();
                        session->RealJid = uid.realId();
                        session->ChatType = obj.value("chatType").toInt(); //
                        session->LastUpdateTime = 0;
                        session->UnreadCount = 0;
                        pSessions->push_back(session);
                    }
                }
            }
        }
        //
        for (const std::shared_ptr<QTalk::Entity::ImSessionInfo> &sessionInfo : *pSessions) {
            QString name;
            std::string xmppId = sessionInfo->XmppId;
            std::string realJid = sessionInfo->RealJid;
            QString headPath;
            if (sessionInfo->ChatType == QTalk::Enum::GroupChat) {
#ifdef _STARTALK
                headPath = ":/QTalk/image1/StarTalk_defaultGroup.png";
#else
                headPath = ":/QTalk/image1/defaultGroupHead.png";
#endif
                std::shared_ptr<QTalk::Entity::ImGroupInfo> groupInfo =
                        dbPlatForm::instance().getGroupInfo(sessionInfo->XmppId);
                if (groupInfo) {
                    name = QString::fromStdString(groupInfo->Name);
                    QString userpath = QString::fromStdString(Platform::instance().getAppdataRoamingUserPath());
                    QString headfilename = QString::fromStdString(groupInfo->HeaderSrc);
                    if (headfilename.isEmpty()) {
                        _normalGroupHeadPhotos.push_back(sessionInfo->XmppId);
                    } else {
                        GenerateHeadPhotoName(headfilename);

                        QString rheadpath = userpath + "/image/headphoto/" + headfilename;
                        QFileInfo headfile(rheadpath);
                        if (headfile.isFile() && headfile.exists()) {
                            headPath = rheadpath;
                        } else {
                            _withoutGroupHeadPhotos.push_back(sessionInfo->XmppId);
                            _withoutGroupHeadPhotoSrcs.push_back(groupInfo->HeaderSrc);
                        }
                    }
                }
            } else {
#ifdef _STARTALK
                headPath = ":/QTalk/image1/StarTalk_defaultHead.png";
#else
                headPath = ":/QTalk/image1/headPortrait.png";
#endif
                //客服consult消息 头像要显示客人 而不是店铺
                std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo =
                        dbPlatForm::instance().getUserInfo(
                                sessionInfo->ChatType == QTalk::Enum::ConsultServer ? sessionInfo->RealJid
                                                                                    : sessionInfo->XmppId);
                if (userInfo) {
//                    name = QString::fromStdString(QTalk::getUserName(userInfo));
                    QString userpath = QString::fromStdString(Platform::instance().getAppdataRoamingUserPath());
                    QString headfilename = QString::fromStdString(userInfo->HeaderSrc);
                    if (headfilename.isEmpty() && (userInfo->HeadVersion != 0)) {
                        _normalHeadPhotos.push_back(sessionInfo->XmppId);
                    } else {
                        GenerateHeadPhotoName(headfilename);

                        QString rheadpath = userpath + "/image/headphoto/" + headfilename;
                        QFileInfo headfile(rheadpath);
                        if (headfile.exists() && headfile.isFile()) {
                            headPath = rheadpath;
                        } else {
                            _withoutHeadPhotos.push_back(sessionInfo->XmppId);
                        }
                    }
                }
            }

            QStandardItem *item = nullptr;
            UID uid(xmppId, realJid);
            bool unNotice = _mapNotice.find(sessionInfo->XmppId) != _mapNotice.end();
            if (_sessionMap.contains(uid)) {
                item = _sessionMap.value(uid);
            } else {
                item = new QStandardItem;
                if (sessionInfo->ChatType == QTalk::Enum::GroupChat) {
                    item->setData(0, ITEM_DATATYPE_ATCOUNT);
                }
            }

            item->setData(sessionInfo->UnreadCount, ITEM_DATATYPE_UNREADCOUNT);
            if (!unNotice)
                _totalUnReadCount += sessionInfo->UnreadCount;

            QString userName;
            if (sessionInfo->ChatType == QTalk::Enum::GroupChat) {
                std::shared_ptr<QTalk::Entity::ImUserInfo> info = dbPlatForm::instance().getUserInfo(
                        sessionInfo->UserId);
                if (nullptr != info) {
                    userName = QString::fromStdString(QTalk::getUserName(info));
                }
            }

            item->setData(sessionInfo->ChatType, ITEM_DATATYPE_CHATTYPE);
            item->setData(QString::fromStdString(sessionInfo->XmppId), ITEM_DATATYPE_USERID);
            item->setData(sessionInfo->LastUpdateTime, ITEM_DATATYPE_LASTTIME);
            item->setData(GenerateTimeText(sessionInfo->LastUpdateTime), ITEM_DATATYPE_LASTSTRTIME);
            item->setData(QString::fromStdString(realJid), ITEM_DATATYPE_REALJID);
            item->setData(QString::fromStdString(sessionInfo->LastMessageId), ITEM_DATATYPE_LAST_MESSAGE_ID);

            if (SYSTEM_XMPPID == QTalk::Entity::JID(sessionInfo->XmppId.c_str()).username() ||
                RBT_SYSTEM == QTalk::Entity::JID(sessionInfo->XmppId.c_str()).username()) {
                QString syshead = ":/UINavigationPlug/image1/system.png";
                item->setData(tr(SYSTEM_NAME), ITEM_DATATYPE_USERNAME);
                item->setData(syshead, ITEM_DATATYPE_HEADPATH);
                item->setData(true, ITEM_DATATYPE_ISONLINE);
            } else if (RBT_NOTICE == QTalk::Entity::JID(sessionInfo->XmppId.c_str()).username()) {
                QString noticehead = ":/UINavigationPlug/image1/atom_ui_rbt_notice.png";
                item->setData(tr(RBT_NOTICE_NAME), ITEM_DATATYPE_USERNAME);
                item->setData(noticehead, ITEM_DATATYPE_HEADPATH);
                item->setData(true, ITEM_DATATYPE_ISONLINE);
            } else if(RBT_QIANGDAN == QTalk::Entity::JID(sessionInfo->XmppId.c_str()).username()){
                QString qhead = ":/UINavigationPlug/image1/atom_ui_robot_qiangdan.png";
                item->setData(tr(RBT_QIANGDAN_NAME), ITEM_DATATYPE_USERNAME);
                item->setData(qhead, ITEM_DATATYPE_HEADPATH);
                item->setData(true, ITEM_DATATYPE_ISONLINE);
            } else {
                if (name.isEmpty())
                    name = getUserName(xmppId, sessionInfo->ChatType == QTalk::Enum::GroupChat);
                item->setToolTip(name);
                item->setData(name.isEmpty() ? QString::fromStdString(xmppId) : name, ITEM_DATATYPE_USERNAME);
                Q_ASSERT(!headPath.startsWith("http"));
                item->setData(headPath, ITEM_DATATYPE_HEADPATH);
                item->setData((sessionInfo->ChatType == QTalk::Enum::GroupChat), ITEM_DATATYPE_ISONLINE);
            }
            item->setData(GenerateContent(QString::fromStdString(sessionInfo->Content), sessionInfo->ChatType,
                                          sessionInfo->MessType, userName), ITEM_DATATYPE_MESSAGECONTENT);
            //is top
            item->setData(_mapStick.find(uid.toStdString()) != _mapStick.end(), ITEM_DATATYPE_ISTOP);

            item->setData(unNotice, ITEM_DATATYPE_UNNOTICE);
            //
            if (!_sessionMap.contains(uid)) {
                _pSrcModel->appendRow(item);
                _sessionMap.insert(uid, item);
            }
        }
    }
    if (_messageManager) {
        _messageManager->sendDownLoadHeadPhotosMsg(_withoutHeadPhotos, _withoutGroupHeadPhotoSrcs);
    }

    _strSelfId = QString("%1@%2").arg(Platform::instance().getSelfUserId().c_str())
            .arg(Platform::instance().getSelfDomain().c_str());
    onUpdateOnline(); // 加载完会话后刷新下在线状态
    // sort
    _pModel->sort(0);
    emit _mainPanel->updateTotalUnreadCount(_totalUnReadCount);
}

/**
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.18
  */
void SessionFrm::onItemSelectChanged(const QModelIndex &index) {
    //_headPhotoLab
    if (!index.isValid()) {
        return;
    }
    StSessionInfo sessionInfo;
    sessionInfo.chatType = static_cast<QUInt8>(index.data(ITEM_DATATYPE_CHATTYPE).toInt());
    sessionInfo.userId = index.data(ITEM_DATATYPE_USERID).toString();
    sessionInfo.realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    sessionInfo.userName = index.data(ITEM_DATATYPE_USERNAME).toString();
    sessionInfo.headPhoto = index.data(ITEM_DATATYPE_HEADPATH).toString();

    {
//        QMutexLocker locker(&_mutex);
        _curUserId = UID(sessionInfo.userId, sessionInfo.realJid);
    }
    UID uid(sessionInfo.userId, sessionInfo.realJid);
    if (sessionInfo.userName.isEmpty()) {
        std::shared_ptr<QTalk::Entity::ImGroupInfo> groupInfo = dbPlatForm::instance().getGroupInfo(
                sessionInfo.userId.toStdString(),
                true);
        if (nullptr != groupInfo) {
            sessionInfo.userName = QString::fromStdString(groupInfo->Name);
            if (!sessionInfo.userName.isEmpty())
                _sessionMap[uid]->setData(sessionInfo.userName, ITEM_DATATYPE_USERNAME);
        }
    }
    // draft
    _sessionMap[uid]->setData("", ITEM_DATATYPE_DRAFT);
    //if (sessionInfo.chatType != QTalk::Enum::GroupChat) {
    unsigned int count = _sessionMap[uid]->data(ITEM_DATATYPE_UNREADCOUNT).toUInt();
    _sessionMap[uid]->setData(0, ITEM_DATATYPE_UNREADCOUNT);
    bool unNotice = _mapNotice.find(sessionInfo.userId.toStdString()) != _mapNotice.end();
    if (!unNotice) {
        _totalUnReadCount -= count;
        emit _mainPanel->updateTotalUnreadCount(_totalUnReadCount);
    }
    _sessionMap[uid]->setData(0, ITEM_DATATYPE_ATCOUNT);

    //抢单直接打开webview
    if(sessionInfo.userId.startsWith(RBT_QIANGDAN)){
        _pMultifunctionFrm->openQiangDan();
    } else{
        emit sgSessionInfo(sessionInfo);
    }

    auto func = [this, sessionInfo, index, count]() {
#ifdef _MACOS
        pthread_setname_np("SessionFrm::onItemSelectChanged");
#endif
        if (_messageManager && sessionInfo.chatType != QTalk::Enum::GroupChat) {
            QString userid = sessionInfo.userId;
            std::set<std::string> users;
            users.insert(sessionInfo.chatType == QTalk::Enum::ConsultServer ? sessionInfo.realJid.toStdString()
                                                                            : userid.toStdString());
            _messageManager->sendGetUserStatus(users);
        }
        if (_messageManager && count > 0) {
            //if(sessionInfo.chatType == QTalk::Enum::TwoPersonChat)
            {
                // 发送消息已读
                QString messageId = index.data(ITEM_DATATYPE_LAST_MESSAGE_ID).toString();
                _messageManager->sendReadedMessage(messageId.toStdString(), sessionInfo.realJid.toStdString(),
                                                   sessionInfo.chatType);
            }
        }
    };

    std::thread t(func);
    t.detach();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.29
  */
void SessionFrm::onDownLoadHeadPhotosFinish() {
    // 刷新双人聊天头像
    // todo
    for (const std::string &xmppid : _withoutHeadPhotos) {
        std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo =
                dbPlatForm::instance().getUserInfo(xmppid, true);
        if (userInfo) {
            QString headfilename = QString::fromStdString(userInfo->HeaderSrc);
            GenerateHeadPhotoName(headfilename);
            QString userpath = QString::fromStdString(Platform::instance().getAppdataRoamingUserPath());
            QString rheadpath = userpath + "/image/headphoto/" + headfilename;
            QFileInfo headfile(rheadpath);
            if (headfile.isFile() && headfile.exists()) {
                QStandardItem *item = _sessionMap.value(UID(xmppid));
                if (item) {
                    Q_ASSERT(!rheadpath.startsWith("http"));
                    item->setData(rheadpath, ITEM_DATATYPE_HEADPATH);
                }
            }
        }
    }
    _withoutHeadPhotos.clear();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.30
  */
void SessionFrm::onDownLoadGroupHeadPhotosFinish() {
    // 刷新群聊天头像
    for (const std::string &xmppid : _withoutGroupHeadPhotos) {
        std::shared_ptr<QTalk::Entity::ImGroupInfo> groupInfo =
                dbPlatForm::instance().getGroupInfo(xmppid, true);
        if (groupInfo) {
            QString headfilename = QString::fromStdString(groupInfo->HeaderSrc);
            GenerateHeadPhotoName(headfilename);
            QString userpath = QString::fromStdString(Platform::instance().getAppdataRoamingUserPath());
            QString rheadpath = userpath + "/image/headphoto/" + headfilename;
            QFileInfo headfile(rheadpath);
            if (headfile.isFile() && headfile.exists()) {
                QStandardItem *item = _sessionMap.value(UID(xmppid));
                if (item) {

//                    item->setData(ITEM_DATATYPE_HEADPHOTO, HeadPhotoLab::generateCirclePixmap(rheadpath, DEFAULT_HEAD_RADIUS));
                    Q_ASSERT(!rheadpath.startsWith("http"));
                    item->setData(rheadpath, ITEM_DATATYPE_HEADPATH);
                }
            }
        }
    }
    _withoutGroupHeadPhotos.clear();
    _withoutGroupHeadPhotoSrcs.clear();
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.12
  */
void SessionFrm::onUpdateOnline() {
    perf_counter_warning(200, "onUpdateOnline");
    for (const UID &uid: _sessionMap.keys()) {
        setUserStatus(uid);
    }
}

/**
  * @函数名
  * @功能描述 本来打算传入暂时没用
  * @参数
  * @author cc
  * @date 2018.10.15
  */
void SessionFrm::onUpdateOnlineUsers(std::map<std::string, std::string> userstatus) {
    std::map<std::string, std::string>::const_iterator iter;

    for (iter = userstatus.cbegin(); iter != userstatus.cend(); ++iter) {
        QString userid = QString::fromStdString(iter->first);
        setUserStatus(UID(userid), true);
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.11.08
  */
void SessionFrm::onNewSession(const StSessionInfo &info) {
    ReceiveSession mess;

    mess.xmppId = info.userId;
    if(dbPlatForm::instance().isHotLine(mess.xmppId.toStdString()))
    {
        mess.chatType = QTalk::Enum::Consult;
    }
    else
        mess.chatType = (QTalk::Enum::ChatType) info.chatType;
    mess.realJid = info.realJid.isEmpty() ? info.userId : info.realJid;
    mess.headSrc = info.headPhoto;

    // 临时处理
    UID uid(mess.xmppId, mess.realJid);
    if (_sessionMap.contains(uid)) {
        QStandardItem *item = _sessionMap[uid];
        mess.messageContent = item->data(ITEM_DATATYPE_MESSAGECONTENT).toString();
        mess.messageRecvTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        mess.messageId = item->data(ITEM_DATATYPE_LAST_MESSAGE_ID).toString();
    } else {
        std::string messagedId = QTalk::utils::getMessageId();
        mess.messageContent = "";
        mess.messageRecvTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        mess.messageId = QString::fromStdString(messagedId);
        //
        if (_messageManager) {
            QTalk::Entity::ImMessageInfo msgInfo;
            msgInfo.MsgId = messagedId;
            msgInfo.XmppId = uid.usrId();
            msgInfo.RealJid = uid.realId();
            msgInfo.ChatType = info.chatType;
            msgInfo.Platform = 0;
            msgInfo.From = _strSelfId.toStdString();
            msgInfo.To = uid.usrId();
            msgInfo.Content = "";
            msgInfo.Type = QTalk::Entity::MessageTypeEmpty;
            msgInfo.State = 1;
            msgInfo.Direction = QTalk::Entity::MessageDirectionSent;
            msgInfo.LastUpdateTime = mess.messageRecvTime;
            msgInfo.SendJid = _strSelfId.toStdString();

            _messageManager->addEmptyMessage(msgInfo);
        }
    }
    //
    onReceiveSession(mess, true);
    //
    QModelIndex index = _sessionMap[uid]->index();
    _pSessionView->setCurrentIndex(_pModel->mapFromSource(index));
    onItemSelectChanged(index);
}

/**
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.18
  */
void SessionFrm::init() {
    initLayout();
    connects();
}

/**
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.18
  */
void SessionFrm::initLayout() {
    if (nullptr == _sessionmainLayout) {
        _sessionmainLayout = new QVBoxLayout(this);
    }
    if(nullptr == _pMultifunctionFrm){
        _pMultifunctionFrm = new MultifunctionFrm(this);
    }
    _sessionmainLayout->setMargin(0);
    if (nullptr == _pSessionView) {
        _pSessionView = new QListView(this);
        _pSessionView->setObjectName("SessionView");
        _pSessionView->verticalScrollBar()->setVisible(false);
        _pSessionView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        _pSessionView->verticalScrollBar()->setSingleStep(15);
        _pItemDelegate = new SessionitemDelegate(_pSessionView);
        _pSrcModel = new QStandardItemModel;
        _pModel = new SessionSortModel();
        _pSessionView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        _pSrcModel->setColumnCount(1);
        _pModel->setDynamicSortFilter(false);
        _pModel->setSourceModel(_pSrcModel);
        _pSessionView->setModel(_pModel);
        _pSessionView->setItemDelegate(_pItemDelegate);
        _pSessionView->setDragEnabled(false);
    }
    _pSessionScrollBar = new QScrollBar(Qt::Vertical, this);
    _pSessionScrollBar->setRange(0, 0);
    _pSessionScrollBar->setMinimumHeight(100);
    _pSessionScrollBar->setVisible(false);
    connect(_pSessionView->verticalScrollBar(), SIGNAL(valueChanged(int)), _pSessionScrollBar, SLOT(setValue(int)));
    connect(_pSessionScrollBar, SIGNAL(valueChanged(int)), _pSessionView->verticalScrollBar(), SLOT(setValue(int)));
    connect(_pSessionView->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), _pSessionScrollBar,
            SLOT(setRange(int, int)));

    _pSessionView->setFrameShape(QFrame::NoFrame);
    _pSessionView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pSessionView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    _sessionView->setFocusPolicy(Qt::NoFocus);
    _sessionmainLayout->addWidget(_pSessionView);
    _pSessionView->installEventFilter(this);

    _pContextMenu = new QMenu(_pSessionView);
    _pContextMenu->setAttribute(Qt::WA_TranslucentBackground, true);
    _showCardAct = new QAction(tr("资料卡片"), _pContextMenu);
    _closeSessionAct = new QAction(tr("移除会话"), _pContextMenu);
    _clearUnreadAct = new QAction(tr("一键清除未读"), _pContextMenu);
    _toTopOrCancelTopAct = new QAction(_pContextMenu);
    _noticeAct = new QAction(_pContextMenu);
    _addFriendAct = new QAction(_pContextMenu);
    _starAct = new QAction(_pContextMenu);
    _blackAct = new QAction(_pContextMenu);
    _quitGroupAct = new QAction(tr("退出群聊"), _pContextMenu);

    _pContextMenu->addAction(_toTopOrCancelTopAct);
    _pContextMenu->addAction(_noticeAct);
    _pContextMenu->addSeparator();
    _pContextMenu->addAction(_addFriendAct);
    _pContextMenu->addAction(_starAct);
    _pContextMenu->addAction(_blackAct);
    _pContextMenu->addAction(_showCardAct);
    _pContextMenu->addSeparator();
    _pContextMenu->addAction(_clearUnreadAct);
    _pContextMenu->addAction(_closeSessionAct);
    _pContextMenu->addSeparator();
    _pContextMenu->addAction(_quitGroupAct);
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.27
  */
QString SessionFrm::GenerateTimeText(const QInt64 &time) {

    if(0 == time)
        return "";

    QDateTime curTime = QDateTime::currentDateTimeUtc();
    QDateTime msgTime = QDateTime::fromMSecsSinceEpoch(time);
    int curYear = curTime.date().year();
    int msgYear = msgTime.date().year();
    if (curYear > msgYear)
        return msgTime.date().toString("yyyy-MM-dd");

    QInt64 curDays = curTime.date().toJulianDay();
    QInt64 msgDays = msgTime.date().toJulianDay();
    if (curDays - msgDays > 10 * 24) {
        return "";
    }
    QString t = curDays > msgDays ? msgTime.date().toString("MM-dd") : msgTime.time().toString("hh:mm");
    std::string tt = t.toStdString();
    return t;
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.9.30
  */
void SessionFrm::GenerateHeadPhotoName(QString &photosrc) {

    std::string tmpPath = QTalk::GetFileNameByUrl(photosrc.toStdString());
    photosrc = tmpPath.data();
    Q_ASSERT(!photosrc.startsWith("http"));
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @author cc
  * @date 2018.10.15
  */
void SessionFrm::setUserStatus(const QTalk::Entity::UID &uid, bool check) {
    QTalk::Entity::JID jid(uid.usrId().data());
    //系统消息 机器人 群 自己 单独处理
    QStandardItem *item = _sessionMap.value(uid);
    if (nullptr != item) {
        if ((item->data(ITEM_DATATYPE_CHATTYPE).toInt() == QTalk::Enum::GroupChat)) {
            return;
        }
        if (jid.username() == SYSTEM_XMPPID || jid.username() == RBT_SYSTEM || jid.username() == RBT_NOTICE || jid.username() == RBT_QIANGDAN) {
            item->setData(true, ITEM_DATATYPE_ISONLINE);
            return;
        }
        if (jid.barename() == Platform::instance().getSelfXmppId()) {
            item->setData(true, ITEM_DATATYPE_ISONLINE);
            return;
        }
    }
    //单人在线状态 为了兼容consult 迭代判断usrId||realId
    if(check)
    {
        QMapIterator<QTalk::Entity::UID, QStandardItem *> mapIterator(_sessionMap);
        while (mapIterator.hasNext()) {
            QTalk::Entity::UID uidNext = mapIterator.next().key();
            if (uidNext.usrId() == jid.barename() || uidNext.realId() == jid.barename()) {
                QStandardItem *item = mapIterator.value();
                if (nullptr == item) continue;
                //
                bool isOnline = Platform::instance().isOnline(jid.barename());
                item->setData(isOnline, ITEM_DATATYPE_ISONLINE);
            }
        }
    }
    else if(_sessionMap.contains(uid))
    {
        bool isOnline = Platform::instance().isOnline(uid.realId());
        item->setData(isOnline, ITEM_DATATYPE_ISONLINE);
    }
}

//
bool SessionFrm::eventFilter(QObject *o, QEvent *e) {
    if (o == _pSessionView) {
        if (e->type() == QEvent::ContextMenu) {
            QUInt8 chatType = static_cast<QUInt8>(_pSessionView->currentIndex().data(ITEM_DATATYPE_CHATTYPE).toInt());
            std::string id = _pSessionView->currentIndex().data(ITEM_DATATYPE_USERID).toString().toStdString();
            bool isTop = _pSessionView->currentIndex().data(ITEM_DATATYPE_ISTOP).toBool();
            bool unNotice = _pSessionView->currentIndex().data(ITEM_DATATYPE_UNNOTICE).toBool();
            _toTopOrCancelTopAct->setText(isTop ? tr("取消置顶") : tr("会话置顶"));
            _noticeAct->setText(unNotice ? tr("取消免打扰") : tr("消息免打扰"));
            _quitGroupAct->setVisible(chatType == QTalk::Enum::GroupChat);
            if (chatType == QTalk::Enum::GroupChat) {
                _addFriendAct->setVisible(false);
                _starAct->setVisible(false);
                _blackAct->setVisible(false);
            } else {
                // todo
                _addFriendAct->setVisible(false);
                //
                bool isStar = std::find(_arSatr.begin(), _arSatr.end(), id) != _arSatr.end();
                bool isBlack = std::find(_arBlackList.begin(), _arBlackList.end(), id) != _arBlackList.end();
                _starAct->setText(isStar ? tr("取消星标") : tr("星标联系人"));
                _blackAct->setText(isBlack ? tr("移出黑名单") : tr("加入黑名单"));
            }
            _pContextMenu->exec(QCursor::pos());

            _addFriendAct->setVisible(true);
            _starAct->setVisible(true);
            _blackAct->setVisible(true);
        } else if (e->type() == QEvent::MouseMove) {
            _pSessionView->update();
        } else if (e->type() == QEvent::Resize) {
            _pSessionScrollBar->setGeometry(_pSessionView->width() - 15, 5,
                                            17, _pSessionView->height() - 10);
        }
    }

    return QFrame::eventFilter(o, e);
}

bool SessionFrm::event(QEvent *e) {
    if (e->type() == QEvent::Enter) {
        if (_pSessionScrollBar->maximum() > 0 && !_pSessionScrollBar->isVisible())
            _pSessionScrollBar->setVisible(true);
    } else if (e->type() == QEvent::Leave) {
        if (_pSessionScrollBar->isVisible())
            _pSessionScrollBar->setVisible(false);
    }
    return QFrame::event(e);
}

void SessionFrm::onUpdateGroupInfo(const QTalk::StGroupInfo &gInfo) {
    UID uid(gInfo.groupId);
    if (_sessionMap.contains(uid)) {
        QStandardItem *item = _sessionMap[uid];
        QString name = QString::fromStdString(gInfo.name);
        if (!name.isEmpty()) {
            item->setData(name, ITEM_DATATYPE_USERNAME);
            item->setToolTip(name);

            QString headPath = QTalk::GetHeadPathByUrl(gInfo.headSrc).data();
            item->setData(headPath, ITEM_DATATYPE_HEADPATH);
        }
    }
}

QString SessionFrm::GenerateContent(const QString &content, const QUInt8 &chatType, const int &msgType,
                                    const QString &userName) {
    QString ret = "";
    if (chatType == QTalk::Enum::GroupChat && !userName.isEmpty()) {
        ret += userName + ":";
    }

    switch (msgType) {
        case QTalk::Entity::MessageTypePhoto:
            ret += tr("[图片]");
            break;
        case QTalk::Entity::MessageTypeImageNew:
            ret += tr("[表情]");
            break;
        case QTalk::Entity::MessageTypeFile:
            ret += tr("[文件]");
            break;
        case QTalk::Entity::MessageTypeCommonTrdInfo:
        case QTalk::Entity::MessageTypeCommonTrdInfoV2:
            ret += tr("[链接卡片]");
            break;
        case QTalk::Entity::MessageTypeSourceCode:
            ret += tr("[代码块]");
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
        default: {
            QString tmpContent = content.split("\n").first();
            ////info_log(tmpContent);
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
                    return ret += tr("[表情]");
                } else {
                    tmpContent.replace(pos, item.size(), tr("[未知]"));
                }
            }

            ret += tmpContent;
            break;
        }
    }
    return ret;
}

void SessionFrm::onUpdateReadedCount(const QTalk::Entity::UID &uid, const int &count) {

    if (_curUserId == uid)
        return;

    if (_sessionMap.contains(uid) && nullptr != _sessionMap[uid]) {
        int chatType = _sessionMap[uid]->data(ITEM_DATATYPE_CHATTYPE).toInt();

        int tmpC = _sessionMap[uid]->data(ITEM_DATATYPE_UNREADCOUNT).toUInt();
        int minc = qMin(tmpC, count);

        _sessionMap[uid]->setData(tmpC - minc, ITEM_DATATYPE_UNREADCOUNT);

        bool unNotice = _mapNotice.find(uid.usrId()) != _mapNotice.end();
        if (!unNotice) {
            _totalUnReadCount -= minc;
            emit _mainPanel->updateTotalUnreadCount(_totalUnReadCount);
        }

//         if (chatType == QTalk::Enum::GroupChat) {
//             int atCount = dbPlatForm::instance().getAtCount(uid.usrId());
//             _sessionMap[uid]->setData(atCount, ITEM_DATATYPE_ATCOUNT);
//         }
        //
        //info_log("ui更新群未读数 id:{0} 未读数{1}", id.toStdString(), unreadCount);
    }
}

void SessionFrm::recvRevikeMessage(const QTalk::Entity::UID &uid, const QString &fromId) {
    if (_sessionMap.contains(uid) && nullptr != _sessionMap[uid]) {
        QString userName = tr("你");

        if (fromId != _strSelfId) {
            std::shared_ptr<QTalk::Entity::ImUserInfo> info =
                    dbPlatForm::instance().getUserInfo(fromId.toStdString());
            if (info && !info->Name.empty()) {
                userName = QString::fromStdString(QTalk::getUserName(info));
            } else {
                userName = "";
            }
        }

        QInt64 time = QDateTime::currentDateTime().toMSecsSinceEpoch();
        _sessionMap[uid]->setData(QString("%1撤回了一条消息").arg(userName), ITEM_DATATYPE_MESSAGECONTENT);
        _sessionMap[uid]->setData(time, ITEM_DATATYPE_LASTTIME);
        _sessionMap[uid]->setData(GenerateTimeText(time), ITEM_DATATYPE_LASTSTRTIME);
        // sort
        _pModel->sort(0);
    }
}

void SessionFrm::onShowCardAct(bool) {

    QModelIndex index = _pSessionView->currentIndex();
    if (index.isValid()) {
        QString xmppId = index.data(ITEM_DATATYPE_USERID).toString();
        if (index.data(ITEM_DATATYPE_CHATTYPE).toInt() == QTalk::Enum::GroupChat) {
            emit _mainPanel->showGroupCardSignal(xmppId);
        } else {
            emit showUserCard(xmppId);
        }

    }
}

void SessionFrm::onCloseSession(bool) {

    QModelIndex index = _pSessionView->currentIndex();
    QString peerId = index.data(ITEM_DATATYPE_USERID).toString();
    QString realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    UID uid(peerId, realJid);
    auto itFind = _sessionMap.find(uid);
    if (itFind != _sessionMap.end() && nullptr != *itFind) {
        emit removeSession(uid);
        _pSrcModel->removeRow((*itFind)->row());
        _sessionMap.remove(uid);
//        index = _pModel->index(0, 0);
//        onItemSelectChanged(_pModel->mapToSource(index));
//        _pSessionView->setCurrentIndex(index);
    }
}

//
void SessionFrm::onToTopAct(bool) {
    QModelIndex index = _pSessionView->currentIndex();
    int chatType = index.data(ITEM_DATATYPE_CHATTYPE).toInt();
    QString peerId = index.data(ITEM_DATATYPE_USERID).toString();
    bool isTop = index.data(ITEM_DATATYPE_ISTOP).toBool();
    QString val = QString("{\"topType\":%1,\"chatType\":%2}").arg(isTop ? 0 : 1).arg(chatType);

    QString realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    UID uid(peerId, realJid);
    auto itFind = _sessionMap.find(uid);
    if (itFind != _sessionMap.end() && nullptr != *itFind) {
        if (_messageManager) {
            //
            _messageManager->setUserSetting(isTop, "kStickJidDic", uid.toStdString(), val.toStdString());
        }
    }
}

//
void SessionFrm::updateUserConfig() {
    QMutexLocker locker(&_mutex);

    auto it = _sessionMap.begin();
    for (; it != _sessionMap.end(); it++) {
        (*it)->setData(_mapStick.find(it.key().toStdString()) != _mapStick.end(), ITEM_DATATYPE_ISTOP);
        (*it)->setData(_mapNotice.find(it.key().toStdString()) != _mapNotice.end(), ITEM_DATATYPE_UNNOTICE);
    }
    _pModel->sort(0);
}

/**
 * 销毁群结果处理
 * @param groupId
 */
void SessionFrm::onDestroyGroup(const QString &groupId) {
    UID uid(groupId);
    auto itFind = _sessionMap.find(uid);
    if (itFind != _sessionMap.end() && nullptr != *itFind) {
        // 销毁群之后更新未读数
        unsigned int count = (*itFind)->data(ITEM_DATATYPE_UNREADCOUNT).toUInt();
        _totalUnReadCount -= count;
        emit _mainPanel->updateTotalUnreadCount(_totalUnReadCount);
        //
        emit removeSession(uid);
        _pSrcModel->removeRow((*itFind)->row());
        _sessionMap.remove(uid);
        QModelIndex index = _pModel->index(0, 0);
        _pSessionView->setCurrentIndex(index);
        onItemSelectChanged(_pModel->mapToSource(index));
    }
}

/**
 * 消息面打扰
 */
void SessionFrm::onUnNoticeAct(bool) {

    QModelIndex index = _pSessionView->currentIndex();
    QString peerId = index.data(ITEM_DATATYPE_USERID).toString();
    bool unNotice = index.data(ITEM_DATATYPE_UNNOTICE).toBool();
    QString val = QString::number(unNotice ? 0 : 1);

    QString realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    UID uid(peerId, realJid);
    auto itFind = _sessionMap.find(uid);
    if (itFind != _sessionMap.end() && nullptr != *itFind) {
        if (_messageManager) {
            _messageManager->setUserSetting(unNotice, "kNoticeStickJidDic", peerId.toStdString(), val.toStdString());
        }
    }
}

/**
 *
 */
void SessionFrm::onAddFriendAct(bool) {
    //
}

/**
 * 星标联系人
 */
void SessionFrm::onStarAct(bool) {

    emit _mainPanel->sgOperator(tr("星标联系人"));

    QModelIndex index = _pSessionView->currentIndex();
    QString peerId = index.data(ITEM_DATATYPE_USERID).toString();
    bool isStar = std::find(_arSatr.begin(), _arSatr.end(), peerId.toStdString()) != _arSatr.end();
    QString val = QString::number(isStar ? 0 : 1);

    QString realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    UID uid(peerId, realJid);

    auto itFind = _sessionMap.find(uid);
    if (itFind != _sessionMap.end() && nullptr != *itFind) {
        if (_messageManager) {
            _messageManager->setUserSetting(isStar, "kStarContact", peerId.toStdString(), val.toStdString());
        }
    }
}

/**
 * 加入黑名单
 */
void SessionFrm::onBlackAct(bool) {

    emit _mainPanel->sgOperator(tr("加入黑名单"));

    QModelIndex index = _pSessionView->currentIndex();
    QString peerId = index.data(ITEM_DATATYPE_USERID).toString();
    bool isBlack = std::find(_arBlackList.begin(), _arBlackList.end(), peerId.toStdString()) != _arBlackList.end();
    QString val = QString::number(isBlack ? 0 : 1);

    QString realJid = index.data(ITEM_DATATYPE_REALJID).toString();
    UID uid(peerId, realJid);

    auto itFind = _sessionMap.find(uid);
    if (itFind != _sessionMap.end() && nullptr != *itFind) {
        if (_messageManager) {
            _messageManager->setUserSetting(isBlack, "kBlackList", peerId.toStdString(), val.toStdString());
        }
    }
}

/**
 *
 */
void SessionFrm::jumpToNewMessage() {
    int row = _pModel->rowCount();
    for (int i = _jumpIndex; i < row; i++) {
        QModelIndex index = _pModel->index(i, 0);
        QModelIndex srcIndex = _pModel->mapToSource(index);
        int unreadCout = srcIndex.data(ITEM_DATATYPE_UNREADCOUNT).toInt();
        if (unreadCout > 0) {
            _pSessionView->scrollTo(index, QAbstractItemView::PositionAtTop);
            _jumpIndex = i + 1;
            return;
        }
    }
    if (_jumpIndex > 0) {
        _jumpIndex = 0;
        jumpToNewMessage();
    }
}

QString SessionFrm::getUserName(const std::string &id, bool isGroup) {

    std::string userId = QTalk::Entity::JID(id.c_str()).username();
    if (SYSTEM_XMPPID == userId ||
        RBT_SYSTEM == userId)
    {
        return tr(SYSTEM_NAME);
    }

    if (!id.empty()) {
        if (isGroup) {
            std::shared_ptr<QTalk::Entity::ImGroupInfo> groupInfo = dbPlatForm::instance().getGroupInfo(id);
            if (groupInfo && !groupInfo->Name.empty())
                return QString::fromStdString(groupInfo->Name);
        } else {
            std::shared_ptr<QTalk::Entity::ImUserInfo> userInfo = dbPlatForm::instance().getUserInfo(id);
            if (userInfo)
                return QString::fromStdString(QTalk::getUserName(userInfo));
            else
                info_log("{0} -> no info", id);
        }
    }
    return QString();
}

/**
 *
 * @param key
 */
void SessionFrm::onShortCutSwitchSession(int key) {

    int index = _pSessionView->currentIndex().row();
    switch (key) {
        case Qt::Key_Up: {
            index--;
            if (index <= 0)
                index = 0;
            break;
        }
        case Qt::Key_Down: {
            index++;
            if (index >= _pModel->rowCount())
                index = _pModel->rowCount();
            break;
        }
        default:
            break;
    }

    QModelIndex modelIndex = _pModel->index(index, 0);
    QModelIndex srcIndex = _pModel->mapToSource(modelIndex);
    _pSessionView->setCurrentIndex(modelIndex);
    onItemSelectChanged(srcIndex);
}

/**
 *
 * @param userId
 * @param localHead
 */
void SessionFrm::onUserHeadChange(const QString &userId, const QString &localHead) {
    // todo
    UID uid(userId);
    if (_sessionMap.contains(uid)) {
        Q_ASSERT(!localHead.startsWith("http"));
        _sessionMap[uid]->setData(localHead, ITEM_DATATYPE_HEADPATH);
    }

}

int SessionFrm::getAllCount() {
    return _totalUnReadCount;
}

void SessionFrm::onClearUnreadAct(bool) {
    //
    QMap<UID, QUInt8> mapUnreadIds;

    auto it = _sessionMap.begin();
    for (; it != _sessionMap.end(); it++) {
        unsigned int count = (*it)->data(ITEM_DATATYPE_UNREADCOUNT).toUInt();
        if (count > 0) {
            QUInt8 chatType = static_cast<QUInt8>((*it)->data(ITEM_DATATYPE_CHATTYPE).toInt());
            (*it)->setData(0, ITEM_DATATYPE_UNREADCOUNT);
            mapUnreadIds[it.key()] = chatType;
        }
    }

    _totalUnReadCount = 0;
    emit _mainPanel->updateTotalUnreadCount(_totalUnReadCount);

    std::thread([mapUnreadIds, this]() {
        auto itr = mapUnreadIds.begin();
        for (; itr != mapUnreadIds.end(); itr++) {
            QString messageId = _sessionMap[itr.key()]->data(ITEM_DATATYPE_LAST_MESSAGE_ID).toString();
            _messageManager->sendReadedMessage(messageId.toStdString(), itr.key().realId(), *itr);
        }
    }).detach();
}

void SessionFrm::onUserConfigChanged(const QTalk::Entity::UID& uid)
{
    if(_sessionMap.contains(uid))
    {
        int type = _sessionMap[uid]->data(ITEM_DATATYPE_CHATTYPE).toInt();
        bool isTop = _mapStick.find(uid.toStdString()) != _mapStick.end();
        bool unNotice = _mapNotice.find(uid.realId()) != _mapNotice.end();
//        bool isStar = std::find(_arSatr.begin(), _arSatr.end(), id) != _arSatr.end();
//        bool isBlack = std::find(_arBlackList.begin(), _arBlackList.end(), uid.realId()) != _arBlackList.end();

        _sessionMap[uid]->setData(isTop, ITEM_DATATYPE_ISTOP);
        _sessionMap[uid]->setData(unNotice, ITEM_DATATYPE_UNNOTICE);
        _sessionMap[uid]->setData(getUserName(uid.realId(), type == QTalk::Enum::GroupChat), ITEM_DATATYPE_USERNAME);
//        _sessionMap[uid]->setData(isBlack, ITEM_DATATYPE_USERNAME);

        // sort
        _pSessionView->update();
        _pModel->sort(0);
    }
}

void SessionFrm::onShowDraft(const QTalk::Entity::UID& uid, const QString &draft) {

    enum {
        Type_Invalid,
        Type_Text,
        Type_Image,
        Type_At,
        Type_Url,
    };
    if(!_sessionMap.contains(uid))
        return;
    //
    QString a_data;
    auto document = QJsonDocument::fromJson(draft.toUtf8());
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
                    a_data.append(value);
                    break;
                }
                case Type_Url:
                {
                    a_data.append(tr("[链接]"));
                    break;
                }
                case Type_Image:
                {
                    a_data.append(tr("[图片]"));
                    break;
                }
                case Type_At:
                {
                    a_data.append(QString("@%1").arg(value));
                    break;
                }
                case Type_Invalid:
                default:
                    break;
            }
        }
    }

    a_data.replace("\n", " ");

    if(_sessionMap.contains(uid) && !a_data.trimmed().isEmpty())
    {
        _sessionMap[uid]->setData(a_data, ITEM_DATATYPE_DRAFT);
    }
}

void SessionFrm::onAppDeactivated() {
    _curUserId = UID();
}

void SessionFrm::onAppActive() {
    auto index = _pSessionView->currentIndex();
    if(index.isValid())
    {
//        auto userId = index.data(ITEM_DATATYPE_USERID).toString();
//        auto readJid = index.data(ITEM_DATATYPE_REALJID).toString();
//        _curUserId = UID(userId, readJid);
        onItemSelectChanged(index);
    }
}

/**
 *
 * @param messageId
 * @param time
 */
void SessionFrm::onGotMState(const QTalk::Entity::UID &uid, const QString &messageId, const long long &time) {

    if(_sessionMap.contains(uid) && _sessionMap[uid])
    {
        auto itemMsgId = _sessionMap[uid]->data(ITEM_DATATYPE_LAST_MESSAGE_ID).toString();
        if(messageId == itemMsgId)
            _sessionMap[uid]->setData(GenerateTimeText(time), ITEM_DATATYPE_LASTSTRTIME);
    }
}

//
void SessionFrm::onQuitGroupAct(bool) {

    QModelIndex index = _pSessionView->currentIndex();
    if (index.isValid()) {
        QString xmppId = index.data(ITEM_DATATYPE_USERID).toString();
        int chatType = index.data(ITEM_DATATYPE_CHATTYPE).toInt();
        QString name = index.data(ITEM_DATATYPE_USERNAME).toString();

        int ret = QtMessageBox::warning(this, tr("警告"), tr("即将退出群聊%1 (%2), 是否继续?").arg(name, xmppId),
                                        QtMessageBox::EM_BUTTON_YES | QtMessageBox::EM_BUTTON_NO);
        if(ret == QtMessageBox::EM_BUTTON_YES) {
            QtConcurrent::run([this, xmppId, chatType](){
                if (_messageManager && chatType == QTalk::Enum::GroupChat) {
                    _messageManager->quitGroupById(xmppId.toStdString());
                }
            });
        }
    }
}