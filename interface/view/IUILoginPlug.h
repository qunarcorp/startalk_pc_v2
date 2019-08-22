#ifndef IUILOGINPLUG_H
#define IUILOGINPLUG_H
#include "UIPluginInterface.h"

class IUILoginPlug : public UIPluginInterface
{
public:

   // virtual void SetModel(ALogicLoginModel * model) = 0;
    virtual void enableAutoLogin(bool) = 0;
    virtual void initConf() = 0;
	virtual bool getAutoLoginFlag() = 0;
	virtual void setAutoLoginFlag(bool flag) = 0;
	virtual void saveHeadPath() = 0;
};

Q_DECLARE_INTERFACE(IUILoginPlug, "com.plugin.login.www/1.0")

#endif // IUILOGINPLUG_H
