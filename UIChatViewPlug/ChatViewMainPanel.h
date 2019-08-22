#ifndef CHATVIEWMIANPANEL_H
#define CHATVIEWMIANPANEL_H

#include <QFrame>
#include <vector>
#include <QVector>
#include <QMap>
#include <QMutexLocker>
#include <QLabel>
#include <QRcode/QRcode.h>
#include "../UICom/UIEntity.h"
#include "MessageManager.h"
#include "NativeChatStruct.h"
#include "../Message/ChatMessage.h"
#include "../include/ThreadPool.h"
#include "../QtUtil/Enum/im_enum.h"
#include "../QtUtil/lib/ini/ConfigLoader.h"
#include "../Message/GroupMessage.h"
#include "VideoPlayWnd.h"
#include "../entity/UID.h"
#include "../entity/im_transfer.h"
#include "GIFManager.h"

class StatusWgt;
class ChatMainWgt;
class GroupChatSidebar;
class ToolWgt;
class InputWgt;
class QHBoxLayout;
class QStackedLayout;
class ChatViewMainPanel;
class MessagePrompt;
class QMediaPlayer;
class SendCodeWnd;
class CodeShowWnd;
class VoiceMessageItem;

// Item
class QSplitter;
class ShareMessageFrm;
class LocalSearchMainWgt;
class ChatViewItem : public QFrame
{
	Q_OBJECT
public:
	ChatViewItem(const QUInt8& chatType, const QString& userName, const QTalk::Entity::UID& uid, ChatViewMainPanel* panel);
    ~ChatViewItem() override;

Q_SIGNALS:
	void showMessage(const QTalk::Entity::ImMessageInfo&, bool);
	void sgRemoveGroupMember(const std::string&);
	void sgUpdateUserStatus(const QString& sts);
	void sgShowSeats(std::vector<QTalk::Entity::ImTransfer>);

public:
    // 发消息前处理 -> 入库
    void preSendMessage(int msgType, const QString& info, const std::string& messageId);

	void sendTextMessage(const std::string& text, const std::map<std::string, std::string>& mapAt
	        , const std::string& messageId = std::string());
	void sendCodeMessage(const std::string &text, const std::string &codeType, const std::string &codeLanguage
            , const std::string& messageId = std::string());
	void sendFileMessage(const QString& filePath, const QString& fileName, const QString& fileSize
            , const std::string& messageId = std::string());
	void sendEmoticonMessage(const QString& id, const QString& messageContent, bool isShowAll
            , const std::string& messageId = std::string());
	void sendCollectionMessage(const QString& id, const QString& imagePath, const QString& imgLink);
    void sendAudioVideoMessage();
    void sendShockMessage();
	QTalk::Entity::UID getPeerId();
	QString conversionId();
	void onRecvAddGroupMember(const std::string& memberId, const std::string& nick, int affiliation);
	void onRecvRemoveGroupMember(const std::string& memberId);
	void onShowSearchWnd();
    void onPositionMessage(qint64);

public:
    void setShareMessageState(bool flag);

private:
    void keyPressEvent(QKeyEvent* e) override;

private:
	void   initUi();
	std::string getMessageId();

private:
    void onGetBeforeAllMessage(qint64 time);
    void onGetBeforeFileMessage(qint64 time);
    void onSearch(qint64 time, const QString&);

private slots:
	void showMessageSlot(const QTalk::Entity::ImMessageInfo& msg, bool isHis);

public:
	QTalk::Entity::UID     _uid;
	QTalk::Enum::ChatType  _chatType; // 是否为群组聊天
    ChatMainWgt*      _pChatMainWgt = nullptr;
	GroupChatSidebar* _pGroupSidebar = nullptr;
    InputWgt*         _pInputWgt = nullptr;
    ToolWgt*          _pToolWgt = nullptr;
	StatusWgt*        _pStatusWgt = nullptr;
    QSplitter*        splitter = nullptr;

    LocalSearchMainWgt*    _pSearchMainWgt = nullptr;

private:
	ChatViewMainPanel* _pMainPanel = nullptr;
	QString            _strConversionId;
    QVBoxLayout*       _leftLay  = nullptr;
    QHBoxLayout*       _sendBtnLay = nullptr;
    QPushButton*       _sendBtn = nullptr;

    ShareMessageFrm*   _pShareMessageFrm = nullptr;
    QFrame *_pInputFrm = nullptr;
};


//
class SnipScreenTool;
class SelectUserWnd;
class MessageItemBase;
class ChatViewMainPanel : public QFrame
{
    Q_OBJECT

public:
    explicit ChatViewMainPanel(QWidget *parent = 0);
    ~ChatViewMainPanel() override;

public slots:
	void onChatUserChanged(const StSessionInfo& info);
    void onAppDeactivated();
	void packAndSendLog(const QString& describe);
	void systemShortCut();
	void onRemoveSession(const QTalk::Entity::UID&);
	void onAppActive();
	void onMsgSoundChanged();
	void setAutoReplyFlag(bool);

Q_SIGNALS:
	void showUserCardSignal(const QString&);
	void showGroupCardSignal(const QString&);
	void sgOpenNewSession(const StSessionInfo&);
    void creatGroup(const QString&);
    void addGroupMember(const QString&);
    void showChatPicture(const QString&, const QString&, int);
    void showMessagePromptSignal(const QTalk::Entity::ImMessageInfo&);
	void recvMessageSignal();
	void sgSwitchCurFun(int);
	void sgJumpToStructre(const QString&);
	void sgShortCutSwitchSession(int); // 快捷键切换会话
	void sgForwardMessage(const QString& messageId);
	void sgReloadChatView();
	void sgDisconnected();
	void sgShowLiteMessageBox(bool isSuccess, const QString& message);
	void sgShowInfoMessageBox(const QString& message);
	void sgPlaySound();
	void sgShowVidioWnd(const std::string& caller, const std::string& callee);
	void sgWakeUpWindow();
    void sgOperator(const QString& desc);
    void showQRcode();
    void sgShockWnd();
    void sgUserSendMessage();

public:
	void setConnectStatus(bool isConnect);
	bool getConnectStatus();
	ChatMsgManager* getMessageManager();
	std::string getSelfUserId();

public:
	void onRecvMessage(R_Message& e);
	void onRecvFileProcessMessage(const FileProcessMessage& e);
	void updateGroupMember(const GroupMemberMessage& e);
	void updateHead();
	void updateUserHeadPath(const std::vector<QTalk::StUserCard>& users);
	void updateGroupTopic(const std::string& groupId, const std::string& topic);
	void getOfflineMessage();
	void updateGroupMemberInfo(const std::string& groupId, const std::vector<QTalk::StUserCard>& userCards);
	void getHistoryMsg(const QInt64& t, const QUInt8& chatType, const QTalk::Entity::UID& uid);
	void gotReadState(const QTalk::Entity::UID& uid, const std::map<std::string, QInt32>& msgMasks);
    void updateRevokeMessage(const QTalk::Entity::UID& uid, const std::string& fromId, const std::string& messageId);
	void onDestroyGroup(const std::string& groupId, bool isDestroy);
	void recvBlackMessage(const std::string& fromId, const std::string& messageId);
	void addConllection(const QString& key, const QString& path);
	void updateGroupInfo(const std::shared_ptr<QTalk::StGroupInfo>& groupinfo);
	void onRemoveGroupMember(const std::string& groupId, const std::string& memberId);
	void onLogReportMessageRet(bool isSuccess, const std::string& message);
	void recvUserStatus(const std::map<std::string, std::string>& userstatus);
	void onRecvVideo(const std::string& peerId);
	void onRecvAddGroupMember(const std::string& groupId, const std::string& memberId, const std::string& nick, int affiliation);
	void onRecvRemoveGroupMember(const std::string& groupId, const std::string& memberId);
	void onChangeHeadSuccess(const std::string& localHead);
	void onUpdateMoodSuccess(const std::string& userId, const std::string& mood);
	void onDisConnected();

public:
	void setFileMsgLocalPath(const std::string &key, const std::string &val);
	std::string getFileMsgLocalPath(const std::string& key);
	std::string uploadFile(const std::string& localFile);

public:
	void showUserCard(const QString& userId);
	void removePrompt(MessagePrompt* prompt);
    //
    void setNoticeMap(const std::map<std::string, std::string>& mapNotice);
    void updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &configs);
    void updateUserConfig(const std::map<std::string, std::string> &deleteData,
                          const std::vector<QTalk::Entity::ImConfig>& arImConfig);
    void updateUserMaskName();
	void onShowChatWnd(QUInt8, QString,QString, QString, QString);
    void showShowCodeWnd(const QString& type, const QString& language, const QString& content);
    void showSendCodeWnd(const QTalk::Entity::UID& uid);
    void sendCodeMessage(const QTalk::Entity::UID& uid, const std::string &text,
            const std::string &codeType, const std::string &codeLanguage);
    void sendShareMessage(const QString&, int type, const QString&);
    // search share
    void searchLocalSession(int type, const std::string& key);
    void forwardMessage(const std::string& messageId);
    void playVoice(const std::string& localFile, VoiceMessageItem* msgItem);
    void setSeatList(const QTalk::Entity::UID& uid, const std::vector<QTalk::Entity::ImTransfer>& transfers);
    void updateGroupReadMState(const std::string &groupId, const std::vector<std::string>& ids);
    //
    void resendMessage(MessageItemBase* item);
    //
    void scanQRcCodeImage(const QString& imgPath);

protected:
	void addItem(const QUInt8& chatType, const QString& userName, const QTalk::Entity::UID& uid);
	void initPlug(); // 加载截屏 表情插件
	void showPrompt(const QTalk::Entity::ImMessageInfo &msg);
	void showEmptyLabel();
	void onSnapFinish(const QString& );
	void onSnapCancel(const QString& );
	void reloadChatView();
	void showLiteMessageBox(bool isSuccess, const QString& message);
	void showInfoMessageBox(const QString& message);
	void initSound();
	void playSound();
	void onActiveMainWindow();
    void onShowVidioWnd(const std::string& caller, const std::string& callee);
    void sendAutoPeplyMessage(const std::string& receiver, const std::string& messageId);

public:
    void makeFileLink(const QString& filePath, const QString& fileMd5);
    QString getFileLink(const QString& fileMd5);

public:
    void setSearchStyle(const QString& qss);

public:
    QString getRealText(const QString& json, const QString& msgId, bool & success, std::map<std::string, std::string>& mapAt);

public:
    VideoPlayWnd*     _pVideoPlayWnd;
    GIFManager*       gifManager;

private:
	std::string       _strSelfId; // 自己id
	bool              _isConnected = true;

private:
	ChatMsgManager*   _pMessageManager;
	ChatMsgListener*  _pMessageListener;
	QStackedLayout*              _pStackedLayout; 
	QMap<QTalk::Entity::UID, ChatViewItem*> _mapSession; //
	QMap<QString, QString>       _mapHeadPath;//头像路径
	QVector<MessagePrompt*>      _arMessagePrompt;
	QLabel*                     _pEmptyLabel;
	SnipScreenTool*              _pSnipScreenTool;
    CodeShowWnd*      _pCodeShowWnd;
    SendCodeWnd*      _pSendCodeWnd;
    SelectUserWnd*    _pSelectUserWnd;

    QRcode*           _pQRcode;

private:
	QMutex             _mutex;

private:
    std::map<std::string, std::string> _mapNotice;//

private:
	QTalk::ConfigLoader* _pFileMsgConfig;

private:
    QMediaPlayer* _pPlayer;
    QMediaPlayer* _pVoicePlayer = nullptr;

private:
    bool _autoReply = false;

private:
    QString _qss;
};

#endif // CHATVIEWMIANPANEL_H
