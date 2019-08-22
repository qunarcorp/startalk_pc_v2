#ifndef ADDRESSBOOKPANEL_H
#define ADDRESSBOOKPANEL_H

#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QMap>
#include <QStackedLayout>
#include <QMutexLocker>
#include <QListWidget>
#include <vector>
#include <memory>
#include <QTreeView>
#include <QStandardItemModel>
#include "../CustomUi/NavigationItem.h"
#include "UserCard.h"
#include "StaffDelegate.h"
#include "StaffStructure.h"
#include "../entity/im_config.h"
#include "../entity/im_friend_list.h"
#include "../entity/im_group.h"
#include "../entity/im_userSupplement.h"
#include "../UICom/UIEntity.h"
#include "../entity/im_user.h"
#include "../include/Spinlock.h"

class ListItemView;
class AddressBookMsgManager;
class AddressBookListener;
class AddressBookPanel : public QWidget
{
	Q_OBJECT

public:
	explicit AddressBookPanel(QWidget *parent = nullptr);
	~AddressBookPanel() override;

public:
//	void sysTreeData(void*);

//
public:
	void updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs);
	void updateUserConfig(const std::map<std::string, std::string> &deleteData,
                          const std::vector<QTalk::Entity::ImConfig>& arImConfig);
	void onRecvFriends(const std::vector<QTalk::Entity::IMFriendList>& friends);
	void onRecvGroups(const std::vector<QTalk::Entity::ImGroupInfo>& groups);

	void onListItemClicked(const QString& id, const QUInt8& type);
	void showUserCard(const QString& id);
	void showGroupCard(const QString& id);
	void getPhoneNo(const std::string &userId);
	void starUser(const std::string& userId);
	void addBlackList(const std::string& userId);
	void creatGroup(const QString& structure, const QString& groupName);
	void onCreatGroupRet(const std::string& groupId);
	void onDestroyGroupRet(const std::string& groupId);
    void
    gotIncrementUser(const std::vector<QTalk::Entity::ImUserInfo> &arUserInfo, const std::vector<std::string> &arDeletes);

protected:
	void initUi();
	void initStaff();
	
	void dealNavItem(QVBoxLayout* layout);
	void onNavItemClicked(QUInt8 type);
    void addItemByType(QUInt8 type, const QVector<std::string>& users);
	void removeItemByType(QUInt8 type, const QVector<std::string>& users);
    int getFixedHeight();
    void getShowItem(QStandardItem* parentItem, const QString& userId);
    void getShowItemByStructre(QStandardItem* parentItem, const QString& structre);
    void parentShow(QStandardItem* );
    void onStaffItemClicked(const QModelIndex&);

public slots:
    void jumpToUserStructre(const QString &);
    void onJumpToStructre(const QString&);

private slots:
	void updateStaffUi();

signals:
	void gotPhoneNo(const std::string&, const std::string&);
	void showHeadPicture(const QString&);
	void sgOpenNewSession(const StSessionInfo&);
	void sgSwitchCurFun(int);
	void updateStaffUiSinal();
	void sgOpeartor(const QString&);
    void updateUiSignal();

private:
	QSplitter *_mainSplitter;
	QStackedLayout* _rightLay;
	QVBoxLayout*    _leftLay;
	QLabel    *     _pEmptyLabel;

private: //ui
	QMap<QUInt8, NavigationItem*> _mapNavItems;
	QMap<QUInt8, QWidget*>        _mapItemWidgets;
	UserCard*                     _pUserCard;
	QTreeView*                    _pStaffView;
    StaffDelegate*                _pStaffDelegate;
	StaffStructure*               _pStaffStructure;

private: //data
	QVector<std::string>         _arStarContact; // 星标联系人
	QVector<std::string>         _arBlackList; // 黑名单
	QVector<std::string>         _arFriends;   // 好友
	QVector<QTalk::Entity::ImGroupInfo> _arGroups; // 群组列表
	QMap<std::string, std::string> _mapMaskNames;
	//
	QMap<std::string, std::vector<std::string>> mapGroupMembers;

private:
	QStandardItem*             _pShowItem;

private: //mutex
	QMutex    _mutex;

private:
	AddressBookMsgManager* _pMsgManager;
	AddressBookListener*   _pMsgListener;
	std::shared_ptr<QTalk::Entity::ImUserSupplement> _imuserSup;
    std::shared_ptr<QTalk::Entity::ImUserInfo>   _userInfo;

private:

    struct StStrcuture {
        QString curStrcutureName;
        std::vector<std::shared_ptr<QTalk::Entity::ImUserInfo> > oUsers;
        std::map<QString, StStrcuture *> mapChild;
        //
        QStandardItem* item = nullptr;
    };

    QStandardItemModel*            _pStaffModel;
    StStrcuture *_pstStrcuture;
    QTalk::util::spin_mutex sm;

    std::map<std::string, QStandardItem*> _staffItems;

    void getStructure();
    void updateUi();
    void creatChildItem(const StStrcuture *os, QStandardItem *parentItem);

};

#endif//ADDRESSBOOKPANEL_H