#ifndef _CARDMANAGER_H_
#define _CARDMANAGER_H_


#include <QWidget>
#include <QMap>
#include <mutex>
#include <map>
#include <set>
#include "../include/ThreadPool.h"
#include "../UICom/UIEntity.h"
#include "../entity/im_userSupplement.h"
#include "../entity/im_config.h"
#include "../entity/im_friend_list.h"
#include "../entity/im_group.h"
#include "../include/CommonStrcut.h"
#include "../include/Spinlock.h"
#include "../entity/im_user.h"

class user_card;

class GroupCard;

class UserCardMsgManager;

class UserCardMessageListener;

class CardManager : public QWidget {
Q_OBJECT

public:
    CardManager();

    ~CardManager() override;

public:
    void getPhoneNo(const std::string &userId);

    void updateUserConfig(const std::vector<QTalk::Entity::ImConfig> &arConfigs);
    void updateUserConfig(const std::map<std::string, std::string> &deleteData,
                          const std::vector<QTalk::Entity::ImConfig>& arImConfig);

    void onRecvFriends(const std::vector<QTalk::Entity::IMFriendList> &friends);

    void starUser(const std::string &userId);

    void addBlackList(const std::string &userId);

    void setUserMaskName(const std::string &userId, const std::string &maskName);

    void setUserMood(const std::string &mood);

    void onRecvGroupMember(const std::string &groupId, const std::map<std::string, QTalk::StUserCard> &userCards,
                            const std::map<std::string, QUInt8> &userRole);

public:
    void updateGroupInfo(const QString &GroupId, const QString &groupName, const QString &topic, const QString &desc);

    void quitGroup(const QString &groupId);

    void destroyGroup(const QString &groupId);

    std::string getSourceHead(const std::string& headLink);

Q_SIGNALS:

    void showUserCardSignal();

    void showGroupCardSignal();

    void gotPhoneNo(const std::string &, const std::string &);

    void sgOpenNewSession(const StSessionInfo &);

    void sgSwitchCurFun(int);

    void sgUpdateGroupMember(std::map<std::string, QTalk::StUserCard>, std::map<std::string, QUInt8> userRole);

    void sgJumpToStructre(const QString &);

    void sgOperator(const QString& desc);

    void sgShowHeadWnd(const QString&, bool);

public slots:

    void shwoUserCard(const QString &userId);

    void showGroupCard(const QString &groupId);

    void updateGroupMember(std::map<std::string, QTalk::StUserCard> userCards, std::map<std::string, QUInt8> userRole);

private:
    void showUserCardSlot();

    void showGroupCardSlot();

private: //data
    QVector<std::string> _arStarContact; // 星标联系人
    QVector<std::string> _arBlackList; // 黑名单
    QVector<std::string> _arFriends;   // 好友
    QMap<std::string, std::string> _mapMaskNames; //

private:
    user_card *_pUserCard;
    GroupCard *_groupCard;

    UserCardMsgManager *_pMsgManager;
    UserCardMessageListener *_pMsgListener;
    std::shared_ptr<QTalk::Entity::ImUserSupplement> _imuserSup;
    std::shared_ptr<QTalk::Entity::ImUserInfo>   _userInfo;
    std::shared_ptr<QTalk::Entity::ImGroupInfo> _imGroupSup;
    std::set<QTalk::StUserMedal> _user_medal;

private:
    QTalk::util::spin_mutex sm;
};

#endif // _CARDMANAGER_H_