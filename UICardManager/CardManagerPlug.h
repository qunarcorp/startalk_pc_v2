//
// Created by cc on 18-11-6.
//

#ifndef QTALK_V2_CARDMANAGERPLUG_H
#define QTALK_V2_CARDMANAGERPLUG_H

#include "cardmanager_global.h"
#include <QObject>
#include "../interface/view/IUICardManagerPlug.h"

class CardManager;
class CARDMANAGER_EXPORT CardManagerPlug : public QObject, public IUICardManagerPlug
{
	Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.plugin.UICardManager.www/1.0" FILE "UICardManager.json")
#endif
    Q_INTERFACES(IUICardManagerPlug) //声明IUILoginPlug是一个接口

public:
    CardManagerPlug();

public:
    QWidget *widget();

private:
    void init();

private:
    CardManager *_pCardManager;
};


#endif //QTALK_V2_CARDMANAGERPLUG_H
