#ifndef _QLISTWIDGET_H_
#define _QLISTWIDGET_H_

#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QMutexLocker>
#include "../include/CommonDefine.h"
#include "../include/CommonStrcut.h"
#include "../include/ThreadPool.h"
#include "../include/Spinlock.h"
#include <QStandardItemModel>

enum memberType
{
	EM_MEMBER_TYPE_IVALID,
	EM_MEMBER_TYPE_OWNER,
	EM_MEMBER_TYPE_ADMIN,
	EM_MEMBER_TYPE_MEMBER,
};

class GroupItemWgt;
class QMenu;
class GroupMemberSortModel;
class GroupMember : public QFrame
{
	Q_OBJECT
public:
    explicit GroupMember(QWidget *parent = nullptr, const QString& groupId = nullptr);
	~GroupMember() override;

Q_SIGNALS:
	void addMemberSignal(const std::string& xmppid, const std::string& userName,
	        const QString& headSrc, QInt8 userType, bool isOnline, const QString&);
    void sgUpdateMemberCount(unsigned int, unsigned int);

public:
	void addMember(const std::string& xmppid, const std::string& userName,
	        const QString& headSrc, QInt8 userType, bool isOnline, const QString& searchKey);
	void deleteMember(const std::string& xmppid);
	void updateHead();
	void updateMemberInfo(const std::vector<QTalk::StUserCard>& users);

	void updateGroupMember(const std::string& memberJid, const std::string& nick, int affiliation);

	void setMemberCount(unsigned int allCount, unsigned int onlineCount);

	QLineEdit* getSearchEdit() { return _pSearchLineEdit; }

	enum {
		EM_MENU_TYPE_SETADMIN,
		EM_MENU_TYPE_REMOVEADMIN,
		EM_MENU_TYPE_REMOVEGROUP,
	};

protected:
	void initUi();
	void onSearchBtnClick();
	bool eventFilter(QObject *, QEvent *) override;
	void setAdminByJid(const std::string& nick,const std::string& xmppId);
	void removeAdminByJid(const std::string& nick,const std::string& xmppId);
	void removeGroupByJid(const std::string& nick,const std::string& xmppId);

private:
	QMap<std::string, QStandardItem*> _mapMemberItem;
	bool _selfIsCreator;
	bool _selfIsAdmin;
	std::string _groupId;

private:
    QListView*    _pMemberList;
    QStandardItemModel* _srcModel;
    GroupMemberSortModel* _pModel;

private:
    QLineEdit*    _pSearchLineEdit;
    QPushButton*  _pSearchBtn;
	QMenu*        _pContextMenu;

    QTalk::util::spin_mutex _sm;
//

    QAction*	  _setAdminAction;
    QAction*	  _removeGroupAction;

private:
    int groupMemberCount = 0;
    int groupMemberOnlineCount = 0;
};

#endif // _QLISTWIDGET_H_