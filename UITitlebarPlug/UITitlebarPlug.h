#ifndef UITITLEBARPLUG_H
#define UITITLEBARPLUG_H

#include "uititlebarplug_global.h"
#include "../interface/view/IUITitleBarPlug.h"
#include "MainPanel.h"

class UITITLEBARPLUGSHARED_EXPORT UITitlebarPlug : public QObject, public IUITitlebarPlug
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.plugin.titlebar.www/1.0" FILE "titlebar.json")
#endif
    Q_INTERFACES(IUITitlebarPlug)
public:
    UITitlebarPlug();

    // UIPluginInterface interface
public:
    QWidget *widget();

    // IUITitlebarPlug interface
public:
    void setCtrlWdt(QWidget *wdt);

private:
    void init();

private:
    MainPanel * _mainPanel;

};

#endif // UITITLEBARPLUG_H
