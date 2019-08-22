#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef _UIGROUPMANAGER_H_
#define _UIGROUPMANAGER_H_

#include "../CustomUi/UShadowWnd.h"
#include <QMap>
#include "../include/Spinlock.h"
#include <QtWidgets/QListWidgetItem>
#include "../include/ThreadPool.h"
#include "MessageManager.h"
#include "SearchView.h"
#include <set>
#include <QMutexLocker>
#include <QTreeView>

class SearchWgt;
class QLabel;
class QListWidget;
class QStandardItemModel;
class QStandardItem;
class TreeItemSortModel;
class NavigationItem;
class BatchAddMemberWnd;
class TreeItemDelegate;
class UIGroupManager : public UShadowDialog {
Q_OBJECT
public:
    UIGroupManager();

    ~UIGroupManager() override;

private:
    enum {
        EM_TYPE_INVALID, EM_TYPE_CREATGROUP, EM_TYPE_ADDGROUPMEMBER
    };

signals:
    void setTreeDataFinish();
    void initDataFinish();

public slots:
    // 建群
    void onCreatGroup(const QString &memberId);
    // 拉群成员
    void onAddGroupMember(const QString &groupId);

public:
    void onCreatGroupRet(bool ret, const std::string &groupId);
    void addGroupMember(const QString &memberId, const QString &headSrc);
    void onRecvFriends(const std::vector<QTalk::Entity::IMFriendList>& friends);
    void onRecvGroupList(const std::vector<QTalk::Entity::ImGroupInfo>& groupInfos);
    void updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs);
    void updateUserConfig(const std::map<std::string, std::string> &deleteData,
                          const std::vector<QTalk::Entity::ImConfig>& arImConfig);
    //
    void gotIncrementUser(const std::vector<QTalk::Entity::ImUserInfo> &arUserInfo,
                          const std::vector<std::string> &arDeletes);


protected:
    void initUi();
    void removeGroupMeber(const QString &memberId);
    void resetUi(const QString &memberId = "");
    void onBatchAddMember(const QString& ids);
    //
    void initGroupMembers();
    void initFriends();
    void initStarUser();
    void initRecentSession();
    void initRecentSessionData();
    void initStructure();
    void initData();
    void updateUi();

protected:
    void sendAddGroupMemberMessage(const QString &groupId, const QList<QString> &members, bool = true);
    void creatGroup();
    QStandardItem* creatGroupItem(QStandardItem *item, const std::string &groupId);
    QStandardItem* creatItem(QStandardItem *item, const std::string &xmppId);
    void onItemDoubleClick(const QString& memberId, const QString& name);

private:
    SearchWgt *_pSearchWgt;
    QListView *_pLstWgt;
    QStandardItemModel* _plstModel;
    QLabel *_pTipLabel;
    QLabel *_pTitleLabel;

    QFrame *_pGroupNaemFrm;
    QLineEdit *_pGroupNameEdit;

    QStandardItemModel *_pTreeModel;
    QTreeView *_pTreeWgt;
    TreeItemDelegate* _pItemDelegate;

    QPushButton *_batchAddMemberBtn;
    BatchAddMemberWnd* _batchAddMemberWnd;

private:
    GroupManagerMsgManager *_pMsgManager;
    GroupMsgListener *_pMsgListener;

private:
    QString _strGroupId;
    QUInt8 _type;
    QMap<QString, QString> _mapUserName;
    QMap<QString, QStandardItem *> _mapLstWgtItem;
    QMap<QString, QList<QString>> _mapGroupMembers;

    QTalk::util::spin_mutex sm;
    std::map<std::string, std::shared_ptr<QTalk::Entity::ImUserInfo>> _structure;
    std::map<std::string, std::set<std::string>> _groupmemebers;
    std::vector<QTalk::Entity::IMFriendList> _friends;
    std::map<std::string, QTalk::Entity::ImGroupInfo> _groupInfos;
    std::set<std::string>            _arStarContact; // 星标联系人
    std::set<std::string>            _arTopUsers;    // 置顶
    std::map<std::string, std::map<int, QStandardItem*>> _mapItems;

    QStandardItem* _pRecentItem;
    QStandardItem* _pFriendItem;
    QStandardItem* _pStarItem;
    int flags = 1;
    enum data_flag{
        EM_ALL = 32
    };

    enum {
        EM_STRUCTURE = 1,
        EM_GROUPMEMEBER = 2,
        EM_FRIEND = 4,
        EM_STAR = 8,
        EM_RECENT = 16,
    };

private:
    QListView *_pSearchView;
    QStandardItemModel* _pSearchSrcModel;
    SearchItemSortModel* _pSearchModel;
    SearchItemDelegate* _pSearchItemDelegate;
};

#endif//_UIGROUPMANAGER_H_