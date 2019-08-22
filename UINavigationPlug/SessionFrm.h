#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef SESSIONFRM_H
#define SESSIONFRM_H

#include <QFrame>
#include <QListWidget>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QMutexLocker>
#include <QScrollBar>
#include "MessageManager.h"
#include "MultifunctionFrm.h"

class StSessionInfo;

class QMenu;

class QAction;

class SessionSortModel;

class SessionitemDelegate;

class NavigationMianPanel;

class SessionFrm : public QFrame {
Q_OBJECT

public:

    explicit SessionFrm(NavigationMianPanel *parent);

    ~SessionFrm() override;

public:
    void setMessageManager(NavigationMsgManager *messageManager);
    void jumpToNewMessage();
    void onUserHeadChange(const QString& userId, const QString& localHead);
    int getAllCount();

public slots:
    void onloadSessionData();
    void onReceiveSession(const ReceiveSession &mess, bool isSend);
    void onItemSelectChanged(const QModelIndex &index);
    void onDownLoadHeadPhotosFinish();
    void onDownLoadGroupHeadPhotosFinish();
    void onUpdateOnline();
    void onUpdateOnlineUsers(std::map<std::string, std::string> userstatus);
    void onNewSession(const StSessionInfo &into);
    void onUpdateGroupInfo(const QTalk::StGroupInfo &gInfo);
    void onUpdateReadedCount(const QTalk::Entity::UID& uid, const int &count);
    void recvRevikeMessage(const QTalk::Entity::UID& uid, const QString &fromId);
    void onToTopAct(bool);
    void onUnNoticeAct(bool);
    void updateUserConfig();
    void onDestroyGroup(const QString &groupId);
    void onShortCutSwitchSession(int key);
    void onUserConfigChanged(const QTalk::Entity::UID&);

private:
    void onShowCardAct(bool);
    void onCloseSession(bool);
    void onAddFriendAct(bool);
    void onStarAct(bool);
    void onBlackAct(bool);
    void onClearUnreadAct(bool);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    bool event(QEvent *e) override;

private:
    void init();
    void initLayout();
    void connects();
    QString GenerateTimeText(const QInt64 &time);
    QString
    GenerateContent(const QString &content, const QUInt8 &chatType, const int &msgType, const QString &userName);
    void GenerateHeadPhotoName(QString &photosrc);
    void setUserStatus(const QTalk::Entity::UID&, bool check = false);
    QString getUserName(const std::string &xmppId, bool isGroup);

signals:

    void sgSessionInfo(const StSessionInfo &into);
    void showUserCard(const QString &);
    void removeSession(const QTalk::Entity::UID &);

private:
    QListView *_pSessionView;
    QScrollBar* _pSessionScrollBar;
    QStandardItemModel *_pSrcModel;
    SessionSortModel *_pModel;
    SessionitemDelegate *_pItemDelegate;
    MultifunctionFrm *_pMultifunctionFrm;

private:
    QVBoxLayout *_sessionmainLayout;
    QMenu  *_pContextMenu;
    QAction *_showCardAct; // 名片
    QAction *_closeSessionAct; // 关闭会话
    QAction *_toTopOrCancelTopAct; // 置顶
    QAction *_noticeAct; // 免打扰
    QAction *_addFriendAct; // 加好友
    QAction *_starAct; // 星标联系人
    QAction *_blackAct; // 黑名单
    QAction *_clearUnreadAct; // 清除所有未读
    QMap<QTalk::Entity::UID, QStandardItem *> _sessionMap; //
    NavigationMsgManager *_messageManager;
private:
    unsigned int _totalUnReadCount;

private:
    QVector<QString> _historyMessageId;
    QString _strSelfId;
    QTalk::Entity::UID _curUserId;
    int _jumpIndex;

private:
    int _normalTopRow; // 普通会话列表第一行位置
    std::vector<std::string> _normalHeadPhotos; // 没有设置头像使用默认头像
    std::vector<std::string> _withoutHeadPhotos; // 本地路径下没有图片需要下载
    std::vector<std::string> _normalGroupHeadPhotos; // 群没有设置头像使用默认头像
    std::vector<std::string> _withoutGroupHeadPhotos; // 群本地路径下没有图片需要下载
    std::vector<std::string> _withoutGroupHeadPhotoSrcs; // 群本地路径下没有图片需要下载

public:
    QMutex _mutex;
    ImSessions pSessions;
    std::map<std::string, std::string> _mapStick;  // 置顶
    std::map<std::string, std::string> _mapNotice; // 面打扰
    std::vector<std::string> _arSatr;
    std::vector<std::string> _arBlackList;

private:
    NavigationMianPanel *_mainPanel;
};

#endif // SESSIONFRM_H
