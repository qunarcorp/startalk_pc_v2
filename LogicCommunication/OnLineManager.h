#ifndef ONLINEMANAGER_H
#define ONLINEMANAGER_H

#include <vector>
#include <string>
#include <set>

class Communication;
class OnLineManager
{
public:
    explicit OnLineManager(Communication* pComm);
    bool OnGetOnLineUser(const std::set<std::string> &users, bool = false);

private:
    Communication* _pComm;
};

#endif // ONLINEMANAGER_H
