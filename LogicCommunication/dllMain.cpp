#include "Communication.h"
#include "communication_global.h"

extern "C" LOGICMANAGER_EXPORT ILogicObject* Handle()
{
	return new Communication();
}