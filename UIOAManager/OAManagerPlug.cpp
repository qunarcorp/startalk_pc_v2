#include "OAManagerPlug.h"

OAManagerPlug::OAManagerPlug()
	: QObject(), _mainPanel(nullptr)
{
}

OAManagerPlug::~OAManagerPlug()
{
}

QWidget * OAManagerPlug::widget()
{
	return _mainPanel;
}

void OAManagerPlug::init()
{
	if (nullptr == _mainPanel)
	{
		_mainPanel = new OAManagerPanel;
	}
}
