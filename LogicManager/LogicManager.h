
#ifndef _LOGICMANAGER_H_
#define _LOGICMANAGER_H_

#include <string>
#include "logicmanager_global.h"
#include "../include/CommonDefine.h"
#include "../interface/logic/IDatabasePlug.h"
#include <set>
#include <map>

class ILogicObject;
class ILogicBase;
class LOGICMANAGER_EXPORT LogicManager
{
public:
	~LogicManager();
	static LogicManager* instance();
    IDatabasePlug * getDatabase();
    ILogicBase * getLogicBase();

private:
	LogicManager();
	void loadLibrary();
//    void loadLibraryDb();

private:
	static LogicManager* _pLogicManager;

private:
    IDatabasePlug * _dataBase;
    ILogicBase*     _logicBase;
    std::map<std::string, ILogicObject*> _logicPlugs;
};

#endif // _LOGICMANAGER_GLOBAL_H

