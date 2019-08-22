#ifndef IUINAVIGATIONPLUG_H
#define IUINAVIGATIONPLUG_H

#include "UIPluginInterface.h"

class IUINavigationPlug : public UIPluginInterface
{
public:
    virtual void updateReadCount() = 0;
};

Q_DECLARE_INTERFACE(IUINavigationPlug, "com.plugin.navigation.www/1.0")

#endif // IUINAVIGATIONPLUG_H
