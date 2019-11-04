#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#ifndef UILOGINPLUG_H
#define UILOGINPLUG_H

#include "LoginPanel.h"
#include "uiloginplug_global.h"
#include "../interface/view/IUILoginPlug.h"

class UILOGINPLUGSHARED_EXPORT UILoginPlug : public QObject, public IUILoginPlug
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.plugin.login.www/1.0" FILE "login.json")
#endif
    Q_INTERFACES(IUILoginPlug) //声明IUILoginPlug是一个接口

public:
    UILoginPlug();
    ~UILoginPlug() override;

    // UIPluginInterface interface`
public:
    QWidget *widget() override;

    // IUILoginPlug interface
public:
    void enableAutoLogin(bool) override;
	bool getAutoLoginFlag() override;
	void setAutoLoginFlag(bool flag) override;
    void initConf() override;
	void saveHeadPath() override;
    void setLoginMessage(const QString&) override;

private:
    void init() override;

private:
    LoginPanel *_mainPanel;
};

#endif // UILOGINPLUG_H
