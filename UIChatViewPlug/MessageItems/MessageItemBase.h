#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#ifndef MESSAGEITEMBASE_H
#define MESSAGEITEMBASE_H

#include <QFrame>
#include <QMenu>
#include <QToolButton>
#include "../NativeChatStruct.h"
#include "../../entity/im_message.h"
#include "../../Platform/dbPlatForm.h"

#define HEAD_RADIUS 14

/**
  * @函数名
  * @功能描述 所有消息类基类
  * @参数
  * @author
  * @date 2018.10.15
  */
class HeadPhotoLab;
class QLabel;
class ChatViewItem;
class MessageItemBase : public QFrame
{
    Q_OBJECT

public:
    explicit MessageItemBase(const QTalk::Entity::ImMessageInfo &msgInfo, QWidget *parent = nullptr);
    ~MessageItemBase() override;

public:
    virtual void setMessageContent(const QString &strMessageContent) { _strMessageContent = strMessageContent; }

    virtual QSize itemWdtSize() { return {}; }

    inline QTalk::Entity::MessageType type() const { return static_cast<QTalk::Entity::MessageType>(_type); }
    inline void setType(QTalk::Entity::MessageType type) { _type = type; }

    inline QTalk::Entity::MessageDirection msgDirection() const { return static_cast<QTalk::Entity::MessageDirection>(_msgDirection); }

    inline QTalk::Entity::ImMessageInfo & msgInfo() { return _msgInfo; }
    inline void setMsgInfo(const QTalk::Entity::ImMessageInfo & info) { _msgInfo = info; }

public:
    void setReadState(const QInt32& state);
    // 判断一个全局点是否在item显示区域内
    bool contains(const QPoint& pos);
    //
    void showShareCheckBtn(bool show);
    //
    void checkShareCheckBtn(bool check);
    //
    void changeUserMood(const std::string &mood);
    //
    void onDisconnected();

signals:
    void sgSelectItem(bool);
    void sgChangeUserMood(const std::string&);
    void sgDisconnected();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

protected:
    ushort _type;
    ushort _msgDirection;
    QTalk::Entity::ImMessageInfo _msgInfo;
    QInt32 _readSts;

protected:
    HeadPhotoLab *_headLab;
    QLabel *_nameLab;
    QFrame *_contentFrm;
    QLabel *_readStateLabel;
    QString _strUserId;
    QString _strRealJid;
    QToolButton* _btnShareCheck;

    QString _strMessageContent;
    HeadPhotoLab *_sending = nullptr;
    HeadPhotoLab *_resending = nullptr;

private:
    QWidget* _pParentWgt;
    bool _isPressEvent;
    bool _alreadyRelease;

public:
    enum readState
    {
        EM_BLACK_RET = -1,
        EM_SENDDING = 0,
        EM_READSTS_UNREAD = 1,
        EM_READSTS_READED = 2,
    };

    bool _adjust = true;
};

#endif // MESSAGEITEMBASE_H
