#ifndef _GROUPMEMBER_H_
#define _GROUPMEMBER_H_

#include <vector>
#include <set>
#include "DaoInterface.h"
#include "../include/CommonDefine.h"
#include "../include/CommonStrcut.h"

class GroupMemberDao : public DaoInterface
{
public:
	explicit GroupMemberDao(qtalk::sqlite::database *sqlDb);
	bool creatTable();
	bool clearData();

public:
	bool getGroupMemberById(const std::string& groupId, std::vector<QTalk::StUserCard>& member, std::map<std::string, QUInt8>& userRole);

	bool bulkInsertGroupMember(const std::string& groupId, const std::map<std::string, QUInt8>& member);

    bool bulkDeleteGroupMember(const std::vector<std::string> &groupIds);

    void getCareUsers(std::set<std::string>& users);

    void getAllGroupMembers(std::map<std::string, std::set<std::string>> &members);


};

#endif//_GROUPMEMBER_H_