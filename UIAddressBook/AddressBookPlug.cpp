#include "AddressBookPlug.h"

AddressBookPlug::AddressBookPlug()
	: QObject(), _mainPanel(nullptr)
{
}

AddressBookPlug::~AddressBookPlug()
{
}

QWidget *AddressBookPlug::widget()
{
	return _mainPanel;
}

void AddressBookPlug::init()
{
	if (nullptr == _mainPanel)
	{
		_mainPanel = new AddressBookPanel;
	}
}
