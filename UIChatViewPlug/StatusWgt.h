#ifndef _STATUSWGT_H_
#define _STATUSWGT_H_

#include <QFrame>
#include <QLabel>
#include "../include/CommonDefine.h"
#include "../entity/UID.h"
#include "../CustomUi/HeadPhotoLab.h"


enum Status
{
	EM_STS_INVALID,
	EM_STS_ONLINE,
	EM_STS_BUSY,
	EM_STS_AWAY,
	EM_STS_OFFLINE,
};

class StautsLabel : public QLabel
{
public:
    explicit StautsLabel(QWidget* parent);
	~StautsLabel() override;

public:
	void setStatus(Status sts);

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	int _sts;
};

class QLabel;
class QPushButton;
class StatusWgt : public QFrame
{
	Q_OBJECT
public:
    explicit StatusWgt(QWidget* parent = nullptr);
	~StatusWgt() override;
	
public:
	void switchUser(QUInt8 t, const QTalk::Entity::UID &uid, const QString& userName);
	void updateUserSts(const QString& sts);
	void showResource(const std::string& resource);

signals:
	void updateName(const QString&);
	void updateMood(const QString&);

protected:
	void initUi();
	void setName(const QString&);
	void setMood(const QString&);
    bool eventFilter(QObject* o, QEvent* e) override;

private:
	QLabel*      _pLabelChatUser{};
    HeadPhotoLab*      _pLabelPlat{};
	QLabel*      _pmood{};
	StautsLabel*  _pStsLabel{};
	QPushButton* _pBtnStructure{}; // 组织架构
	QPushButton* _pBtnAddGroup{};  // 增加群聊/邀请加入群
	QPushButton* _pBtnLock{};      //锁

private:
    bool         _isGroupChat{};
    bool         _isConsultServer{};
    QTalk::Entity::UID  _uid;
};

#endif//_STATUSWGT_H_