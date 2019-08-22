#ifndef NAVIGATIONMIANPANEL_H
#define NAVIGATIONMIANPANEL_H

#include <QFrame>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "../Message/ChatMessage.h"
#include "../Message/LoginMessgae.h"
#include "../entity/im_config.h"
#include "../QtUtil/Entity/JID.h"
#include "../include/STLazyQueue.h"


class SessionFrm;
class ContactFrm;
class MultifunctionFrm;
struct StSessionInfo;
class NavigationMsgManager;
class NavigationMsgListener;
class QTimer;

class ReceiveSession
{
public:
    ReceiveSession()
        : messageRecvTime(0), messtype(-1) {};
    QTalk::Enum::ChatType chatType;
    QString messageId;
    QString messageContent;
    QString xmppId;
    QString realJid;
    QInt64  messageRecvTime;
    QString sendJid;
    int messtype;

    QString headSrc;
};

class TcpDisconnect;
class NavigationMianPanel : public QFrame
{
	Q_OBJECT

public:
    explicit NavigationMianPanel(QWidget *parent = 0);
    ~NavigationMianPanel() override;

public:
    void receiveSession(R_Message mess);
    void sendSession(S_Message mess);
    void onDownLoadHeadPhotosFinish();
    void onDownLoadGroupHeadPhotosFinish();
    void onUpdateOnline();
    void onUpdateOnlineUsers(const std::map<std::string, std::string> &userstatus);
	void onTcpDisconnect();
//	void onRetryConnected();
	void onLoginSuccess();
    void onUpdateGroupInfo(std::shared_ptr<QTalk::StGroupInfo> info);
    void updateReadCount(const QTalk::Entity::UID& uid, const int& count);
    void recvRevokeMessage(const QTalk::Entity::UID& uid, const std::string& from);
    void onUpdateUserConfig(const std::vector<QTalk::Entity::ImConfig>& arConfigs);
    void onUpdateUserConfig(const std::map<std::string, std::string> &deleteData,
                            const std::vector<QTalk::Entity::ImConfig>& arImConfig);
	void onDestroyGroup(const std::string& groupId);
	void onChangeHeadRet(bool ret, const std::string& xmppId, const std::string& localHead);
	void updateTatalReadCount();

public slots:
	void retryToConnect();
    void onNewSession(const StSessionInfo &into);
	void jumpToNewMessage();
    void onShortCutSwitchSession(int);

private:
    void init();
    void initLayout();
    void initMessage();
    void connects();
	void onSendShowCardSigal(const QString& userId);

private slots:
	void connToServerTimerSlot(bool);

	void removeSessionAction(const QTalk::Entity::UID&);

signals:
    void sgSessionInfo(const StSessionInfo &into);
    void sgReceiveSession(ReceiveSession mess, bool isSend);
    void sgDownLoadHeadPhotosFinish();
    void sgDownLoadGroupHeadPhotosFinish();
    void sgUpdateOnline();
    void sgUpdateOnlineUsers(std::map<std::string, std::string> userstatus);
	void setDisconnectWgtVisible(bool);
	void connToServerTimerSignal(bool);
	void showUserCardSignal(const QString&);
	void showGroupCardSignal(const QString&);
	void updateGroupInfoSignal(const QTalk::StGroupInfo&);
	void updateReadedCountSignal(const QTalk::Entity::UID&, const int&);
	void recvRevokeMessageSignal(const QTalk::Entity::UID&, const QString&);
    void loadSession();
    void removeSession(const QTalk::Entity::UID&);
	void updateTotalUnreadCount(int);
	void destoryGroupSignal(const QString&);
    void sgShortCutSwitchSession(int);
    void sgChangeUserHead(const QString& userId, const QString& userHead);
    void sgOperator(const QString& desc);
    void sgUserConfigChanged(const QTalk::Entity::UID& );

private:
    QVBoxLayout * _mainLayout;
    QStackedWidget * _stackWdt;
    SessionFrm * _pSessionFrm;
    ContactFrm * _contactFrm;
    MultifunctionFrm * _multifunctionFrm;
	TcpDisconnect*    _pTcpDisconnect;
    NavigationMsgManager * _messageManager;
    NavigationMsgListener * _messageListener;

private:
	QTimer*          _pConnToServerTimer;
	bool             _conneted = false;
};

#endif // NAVIGATIONMIANPANEL_H
