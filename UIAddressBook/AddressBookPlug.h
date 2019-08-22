#ifndef ADDRESSBOOKPLUG_H
#define ADDRESSBOOKPLUG_H

#include <QObject>
#include "../interface/view/IUIAddressBookPlug.h"
#include "AddressBookPanel.h"

class AddressBookPlug : public QObject, public IUIAddressBookPlug
{
Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "com.plugin.addressbook.www/1.0" FILE "addressbook.json")
#endif
    Q_INTERFACES(IUIAddressBookPlug)
public:
    AddressBookPlug();
    ~AddressBookPlug();

public:
    QWidget *widget();

private:
    void init();

private:
    AddressBookPanel* _mainPanel;
};

#endif //ADDRESSBOOKPLUG_H