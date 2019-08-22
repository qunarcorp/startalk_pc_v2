//
// Created by cc on 18-11-6.
//

#ifndef QTALK_V2_UIGROUPMANAGERPLUG_H
#define QTALK_V2_UIGROUPMANAGERPLUG_H

#include "uigroupmanager_global.h"
#include <QObject>
#include "../interface/view/IUIGroupManagerPlug.h"

class UIGroupManager;
class UIGROUPMANAGER_EXPORT UIGroupManagerPlug : public QObject, public IUIGroupManagerPlug
{
	Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	Q_PLUGIN_METADATA(IID "com.plugin.UIGroupManager.www/1.0" FILE "UIGroupManager.json")
#endif
	Q_INTERFACES(IUIGroupManagerPlug)

public:
	UIGroupManagerPlug();
	~UIGroupManagerPlug() override;

public:
	QWidget *widget() override;

private:
	void init() override;

private:
	UIGroupManager *_pGroupManager;
};


#endif //QTALK_V2_UIGROUPMANAGERPLUG_H
