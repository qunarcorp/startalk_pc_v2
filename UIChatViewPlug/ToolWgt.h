#ifndef _TOOLWGT_H_
#define _TOOLWGT_H_

#include <QFrame>
#include "SessionTransferWnd.h"
#include "SendProductWnd.h"
#include "QuickReplyWnd.h"
#include "QchatMoreFun.h"
#include "../entity/im_transfer.h"

class QPushButton;
class InputWgt;
class ChatViewItem;
class QMenu;
class QCheckBox;
class QComboBox;
class EmoticonMainWgt;
class ToolWgt : public QFrame
{
	Q_OBJECT
public:
	ToolWgt(InputWgt* pInputWgt, ChatViewItem* chatItem);
	~ToolWgt() override;

private:
    void initUi();
    void onFileBtnClicked();
    void onBtnScreenshot();
	void onpBtnEmoticon();
	void openLinkWithCkey(const QUrl& url);
	void sendJsonPrud(const QString &products);
	void sendQuickReply(const std::string& text);
	void sessionTransfer(const std::string& newJid,const std::string& reason);

public:
	void showSeats(std::vector<QTalk::Entity::ImTransfer> transfers);
	void sendWechat();

Q_SIGNALS:
    void showSearchWnd();

private:
	InputWgt*     _pInputWgt;
	ChatViewItem* _pChatItem;
	QPushButton* _pBtnEmoticon;
	QPushButton* _pBtnScreenshot;
	QPushButton* _pBtnScreenshotSet;
    QPushButton* _pBtnCode;
	QPushButton* _pBtnShock;
	QPushButton* _pBtnFile;
	QPushButton* _pBtnVideo;
	QPushButton* _pBtnHistory;
	QPushButton* _pBtnShare;
	QPushButton* _pBtnMultifunction;
	QPushButton* _pBtnCloseService;
	QPushButton* _pBtnTransfer;
	QPushButton* _pBtnQuickReply;
	QMenu*       _pMenu;
	QAction*     _pScreenShotHideWnd;

	SendProductWnd* _pSendProductWnd;
	QuickReplyWnd* _pQuickReplyWnd;
	SessionTransferWnd* _pSessionTransferWnd;
	QchatMoreFun* qchatMoreFun;

	QMenu* _pMultiMenu;


};

#endif//_TOOLWGT_H_
