#include "GroupChatSidebar.h"
#include "GroupMember.h"
#include <GroupTopic.h>
#include <QVBoxLayout>
#include <sstream>
#include <QRect>
#include "../Platform/Platform.h"
#include "ChatViewMainPanel.h"
#include "../QtUtil/Entity/JID.h"
#include "../include/Line.h"

extern ChatViewMainPanel* g_pMainPanel;

using namespace QTalk;
GroupChatSidebar::GroupChatSidebar(QWidget* parent, const QString& groupId)
	:QFrame(parent), _pGroupTopic(nullptr), _pGroupMember(nullptr)
{
	_groupId = groupId.toStdString();
	initUi();
}


GroupChatSidebar::~GroupChatSidebar()
{
	if (nullptr != _pGroupTopic)
	{
		delete _pGroupTopic;
		_pGroupTopic = nullptr;
	}
	if (nullptr != _pGroupMember)
	{
		delete _pGroupMember;
		_pGroupMember = nullptr;
	}
}

void GroupChatSidebar::updateGroupMember(const std::string& memberJid, const std::string& nick, int affiliation) {
    if(_pGroupMember)
	    _pGroupMember->updateGroupMember(memberJid, nick, affiliation);
}

/**
  * @函数名   updateGroupMember
  * @功能描述 更新群成员
  * @参数
  * @author   cc
  * @date     2018/10/09
  */
void GroupChatSidebar::updateGroupMember(const GroupMemberMessage& e)
{
	QMutexLocker locker(&_mutex);
	std::vector<std::string> deletedMembers(_arMembers);

	const std::map<std::string, QTalk::StUserCard>& members = e.members;
	std::map<std::string, QUInt8> userRole = e.userRole;
	//
	unsigned int onlineUserSize = 0;
	// 添加
    auto it = members.cbegin();
	for (; it != members.cend(); it++)
	{
		//
		bool isOnline = Platform::instance().isOnline(Entity::JID(it->first.c_str()).barename());
		if (isOnline)
		{
			onlineUserSize++;
		}
		auto itFind = std::find(deletedMembers.begin(), deletedMembers.end(), it->first);
		if (itFind !=  deletedMembers.end())
		{
			deletedMembers.erase(itFind);
		}
		else if (_pGroupMember)
		{
			std::string headSrc = GetHeadPathByUrl(it->second.headerSrc);
			if (userRole.find(it->first) != userRole.end())
			{
                std::string name = it->second.nickName;
                if(name.empty())
                    name = it->second.userName;
                if(name.empty())
                    name = QTalk::Entity::JID(it->first.data()).username();

                emit _pGroupMember->addMemberSignal(it->first, name, QString(headSrc.c_str()), userRole[it->first],
                                                    isOnline, QString::fromStdString(it->second.searchKey));
				_arMembers.push_back(it->first);
			}
		}
	}

	//
	if(_pGroupMember)
        emit _pGroupMember->sgUpdateMemberCount(members.size(), onlineUserSize);

	// 删除
	for (const std::string& id : deletedMembers)
	{
		if (_pGroupMember)
		{
			emit deleteMember(id);
		}
	}
}

/**
  * @函数名   initUi
  * @功能描述 
  * @参数
  * @author   cc
  * @date     2018/10/08
  */
void GroupChatSidebar::initUi()
{
	setObjectName("GroupChatSidebar");
	//
	_pGroupTopic = new GroupTopic();
	_pGroupMember = new GroupMember(this,QString::fromStdString(_groupId));

	setFixedWidth(164);
	_pGroupTopic->setFixedHeight(135);

	auto* layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(_pGroupTopic);
    layout->addWidget(new Line(Qt::Horizontal));
	layout->addWidget(_pGroupMember);
	setLayout(layout);

	qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<QInt8>("QInt8");

	connect(this, &GroupChatSidebar::deleteMember, _pGroupMember, &GroupMember::deleteMember);
	connect(this, &GroupChatSidebar::updateGroupTopic, _pGroupTopic, &GroupTopic::setTopic);
}
