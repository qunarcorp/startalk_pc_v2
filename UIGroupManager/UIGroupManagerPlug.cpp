#include "UIGroupManagerPlug.h"
#include "UIGroupManager.h"

UIGroupManagerPlug::UIGroupManagerPlug()
    :_pGroupManager(nullptr)
{
}


UIGroupManagerPlug::~UIGroupManagerPlug()
{
	if (nullptr != _pGroupManager)
	{
		delete _pGroupManager;
		_pGroupManager = nullptr;
	}
}

QWidget * UIGroupManagerPlug::widget()
{
	if (nullptr != _pGroupManager)
		return _pGroupManager;
	return nullptr;
}

void UIGroupManagerPlug::init()
{
	_pGroupManager = new UIGroupManager;
}