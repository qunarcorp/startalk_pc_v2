#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef _CHATMAINWGT_H_
#define _CHATMAINWGT_H_

#include <QListWidget>
#include <QTime>
#include <QMultiMap>
#include <QMenu>
#include <QMutexLocker>
#include <set>
#include <vector>
#include "../include/CommonDefine.h"
#include "NativeChatStruct.h"
#include "NewMessageTip.h"
#include "AtMessageTip.h"
#include "../entity/im_message.h"
#include "../include/STLazyQueue.h"


class ChatViewItem;
class MessageItemBase;
class FileSendReceiveMessItem;
class VideoMessageItem;
class ChatMainWgt : public QListWidget
{
	Q_OBJECT
public:
	explicit ChatMainWgt(ChatViewItem *pViewItem);
	~ChatMainWgt() override;

public:
	enum {EM_DATE_TYPE_TIME = Qt::UserRole, EM_DATETYPE_MSGTYPE};

public:
    void analysisMessage(QVector<StTextMessage> &msgs, const std::string& content, const std::string& msgId);
	// 显示消息
    void showMessage(const QTalk::Entity::ImMessageInfo &msg, bool isHistory);
    void recvFileProcess(const double &speed, const double &dtotal, const double &dnow, const double &utotal, const double &unow, const std::string &key);
	void setHasNotHistoryMsgFlag(bool hasHistory);
    void resizeItems();
    void recvBlackListMessage(const QString& messageId);
    void clearData();
    // 处理时间信息判断是否显示 超一分钟显示
    void showMessageTime(const QString& msgId, const QInt64& strTime, bool isHistoryMessage = false);

private:
    void connects();

Q_SIGNALS:
	void showTipMessageSignal(int, const QString&, bool, QInt64);
	void adjustItems();
	void sgSelectItem();
	void sgSelectedSize(unsigned int);
	void sgUploadShareMsgSuccess(const QString&, int type, const QString&);
	void sgImageDownloaded(const QString&, const QString&);

public:
    void setShareMessageState(bool flag);
    void onShareMessage();
    void onChangeUserMood(const std::string&, const std::string&);
    void setConnectState(bool isConnected) {_connectState = isConnected; };
    void onUserMedalChanged(const std::set<std::string>& changedUser);

protected:
	void resizeEvent(QResizeEvent *e) override;
	void wheelEvent(QWheelEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	bool event(QEvent* e) override ;

private slots:
    void onRecvFRileProcess(double speed, double dtotal, double dnow, double utotal, double unow, std::string key);
    void onRecvReadState(const std::map<std::string, QInt32 >& readStates);
    void onMState(const QString& msgId, const long long& time);
    void onRecvGroupMState(const std::vector<std::string> &msgIds);
    void updateRevokeMessage(const QString& fromId, const QString& messageId, const long long&);
    void onForwardAct(bool);
	void onScrollBarChanged(int val);
	void onImageDownloaded(const QString& msgId, const QString& path);
	void onCustomContextMenuRequested(const QPoint &pos);

private:

	//

    void analysisSendTextMessage(QVector<StTextMessage> &msgs, const std::string &msg);

    void analysisTextMessage(QVector<StTextMessage> &msgs, const QString& text);
	void analysisAtMessage(QVector<StTextMessage> &msgs, const QString& text);
    void analysisImageMessage(const QString& content, QString& imageLink, qreal& width, qreal& height);
    void analysisEmoticonMessage(const QString& content, QString& pkgid, QString& shortCut);
	QTalk::Entity::ImMessageInfo analysisFileMessage(const QTalk::Entity::ImMessageInfo &msg, bool isHis);
    void dealMessage(MessageItemBase *msgItemWgt, QListWidgetItem *pLstItem, bool isHistoryMessage);
    void showRevokeMessage(const QString& userName, bool isHistory, QInt64 t);
    void showTipMessage(long long type, const QString& content, bool isHistory, QInt64 t);
    void saveAsImage(const QString &oldFilePath);

private:
    void onCopyAction(bool);
    void onSaveAsAction(bool);
    void onRevokeAction(bool);
    void onQuoteAct(bool);
    void onCollectionAct(bool);
    void onQRCodeAct(bool);
    void onAdjustItems();
    void onItemSelectionChanged();
    void onItemChanged();
    void onDisconnected();
    void onSendMessageFailed(const QString& msgId);

Q_SIGNALS:
    void sgRecvFRileProcess(double speed, double dtotal, double dnow, double utotal, double unow, std::string key);
    void gotReadStatueSignal(const std::map<std::string, QInt32 >& readStates);
    void sgGotGroupMStatue(const std::vector<std::string>& msgIds);
    void sgDisConnected();
    void updateRevokeSignal(const QString& fromId, const QString& messageId, const long long&);
    void sgSendFailed(const QString& msgId);
    void sgGotMState(const QString& msgId, const long long& time);

private:
    void downloadImage(const QString& msgId, const QString& link, int width, int height);
    void downloadEmoticon(const QString& msgId, const QString& pkgid, const QString& shortCut);

private:
    bool positionIsTop();
    bool positionIsBottom();

public:
	ChatViewItem* _pViewItem;

public:
    bool _connectState = false;

private:

//    QTime         _downLoadElapsed; //用于进度条
    //long long      downloadProcess = 0;
    bool          _hasnotHistoryMsg;
    QMutex        _mutex;
    QMenu         *_pMenu;

    QMultiMap<QString, MessageItemBase*> _itemWgts;
    QMap<MessageItemBase*, QListWidgetItem*> _items;
    std::set<FileSendReceiveMessItem*>   _fileItems;
    std::set<VideoMessageItem*>          _videoItems;
    std::set<std::string>                _arHistoryIds;

private:
	NewMessageTip* _pNewMessageTipItem;
	AtMessageTip*   _pAtMessageTipItem;

private:
	QString _selfUserName;

private:
	int _oldScrollBarVal;
    long long downloadProcess = 0;
    qint64 _historyTime = 0;
    STLazyQueue<bool> *_resizeQueue;
    STLazyQueue<bool> *_selectItemQueue;

private:
    bool _selectEnable;

private:
    QAction* saveAsAct;
    QAction* copyAct;
    QAction* quoteAct;
    QAction* forwardAct;
    QAction* revokeAct;
    QAction* collectionAct;
    QAction* shareMessageAct;
    QAction* qrcodeAct;
};

#endif//_CHATMAINWGT_H_
