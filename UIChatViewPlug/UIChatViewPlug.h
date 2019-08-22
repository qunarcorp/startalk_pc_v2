#ifndef UICHATVIEW_H
#define UICHATVIEW_H

#include "uichatviewplug_global.h"
//#include "../interface/view/IUIChatViewPLug.h"
#include "../interface/view/IUIChatViewPlug.h"
#include "ChatViewMainPanel.h"

class UICHATVIEWSHARED_EXPORT UIChatViewPlug : public QObject, public IUIChatViewPlug
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.plugin.chatview.www/1.0" FILE "chatview.json")
#endif
    Q_INTERFACES(IUIChatViewPlug)

public:
    UIChatViewPlug();
    ~UIChatViewPlug();

    // UIPluginInterface interface
public:
    QWidget *widget();

    void setStyleSheet(const QString& sheet) override;

private:
    void init();

private:
    ChatViewMainPanel * _mainPanel;
};

#endif // UICHATVIEW_H
