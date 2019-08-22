#ifndef IUITITLEBARPLUG_H
#define IUITITLEBARPLUG_H

#include "UIPluginInterface.h"

class IUITitlebarPlug : public UIPluginInterface
{
public:
    virtual void setCtrlWdt(QWidget *wdt) = 0;

};

Q_DECLARE_INTERFACE(IUITitlebarPlug, "com.plugin.titlebar.www/1.0")

#endif // IUITITLEBARPLUG_H
