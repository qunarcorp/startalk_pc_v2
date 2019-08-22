
#ifndef _GROUPMANAGER_H_
#define _GROUPMANAGER_H_

#include <map>
#include <vector>
#include "../entity/im_group.h"
#include "../include/CommonStrcut.h"

typedef std::map<std::string, std::vector<QTalk::Entity::ImGroupInfo> > MapGroupCard;

class Communication;

class GroupManager {
public:
    explicit GroupManager(Communication *pComm);

    ~GroupManager();

public:
    //
    bool getUserGroupInfo(MapGroupCard &groups);
    //
    bool getGroupCard(const MapGroupCard &groups);
    //
    void upateGroupInfo(const std::vector<QTalk::StGroupInfo>& groupInfos);
    //
    void getUserIncrementMucVcard();

protected:
    Communication *_pComm;
};

#endif//_GROUPMANAGER_H_