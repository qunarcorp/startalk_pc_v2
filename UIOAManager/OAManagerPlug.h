#pragma once

#include <QObject>
#include "../interface/view/IUIOAManagerPlug.h"
#include "OAManagerPanel.h"

class OAManagerPlug : public QObject, public IUIOAManagerPlug
{
	Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		Q_PLUGIN_METADATA(IID "com.plugin.oamanager.www/1.0" FILE "oamanager.json")
#endif
		Q_INTERFACES(IUIOAManagerPlug)
public:
	OAManagerPlug();
	~OAManagerPlug();

public:
	QWidget *widget();

private:
	void init();

private:
	OAManagerPanel* _mainPanel;
};
