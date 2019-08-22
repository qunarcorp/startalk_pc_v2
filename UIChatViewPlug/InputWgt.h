#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef _INPUTWGT_H_
#define _INPUTWGT_H_

#include <QTextEdit>
#include <QVector>
#include <vector>
#include <QMutexLocker>
#include <QListView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "../include/CommonStrcut.h"
#include "AtMessageView.h"


enum {
    Type_Invalid,
    Type_Text,
    Type_Image,
    Type_At,
    Type_Url,
};

class ChatMainWgt;

class QMovie;

class ChatViewItem;

class QMenu;

class InputWgt : public QTextEdit 
{
    Q_OBJECT
public:
	enum 
	{
		MimeDataFormatType_Text = 0,
		MimeDataFormatType_Img = 1
	};

public:
    explicit InputWgt(ChatMainWgt *pMainWgt, ChatViewItem *parent = nullptr);

    ~InputWgt() override;

public:
    void dealFile(const QString &filePath, bool isFile, const QString& imageLink = QString());
    void onSnapFinish(const QString &);
    void onSnapCancel(const QString &);
    void insertAt(const int& pos, const QString& name, const QString& XmppId);
    void insertAt(const std::string& XmppId);
    //
    void insertQuote(const QString& userName, const QString& source);
    //
    void updateGroupMember(const std::map<std::string, QTalk::StUserCard>& member);
    void updateGroupMemberInfo(const std::vector<QTalk::StUserCard>& member);
    //
    void onAppDeactivated();
    //
    void removeGroupMember(const std::string& memberId);
    // 粘贴处理
    void onPaste();
    // 发送消息处理
    void sendMessage();

Q_SIGNALS:
    void sgTranslated(const QString&);
    void sgShowMessage(const QString&);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void hideEvent(QHideEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    void initUi();
    void sendText(const QString&);
    QString translateText();
    QString getNetImgPath(const QString &localPath); //
    void onTextChanged();
    void showAtView();
    void clearDocument();
    void showMessage(const QString&);

protected:
    void onCopy();

private:
    ChatMainWgt *_pMainWgt;
    ChatViewItem *_pChatView;
    QMenu *_pMenu;
    AtMessageView         *_atView;

private:
    QMutex                       _mutex;

private:
    bool _atMessage = false;
    bool _isSending;
    bool _showAtView = false;

};

#endif//_INPUTWGT_H_