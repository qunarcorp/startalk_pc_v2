#include "UILoginPlug.h"
#include "LoginPanel.h"

UILoginPlug::UILoginPlug() :
    _mainPanel(nullptr)
{
}

UILoginPlug::~UILoginPlug()
{
    if(nullptr != _mainPanel)
        delete _mainPanel;
}

QWidget *UILoginPlug::widget()
{
    return _mainPanel;
}

void UILoginPlug::enableAutoLogin(bool enable)
{
    if (_mainPanel)
        _mainPanel->enableAutoLogin(enable);
}

bool UILoginPlug::getAutoLoginFlag()
{
	if (_mainPanel)
		return _mainPanel->getAutoLoginFlag();
	return false;
}

void UILoginPlug::setAutoLoginFlag(bool flag)
{
	if (_mainPanel)
		_mainPanel->setAutoLoginFlag(flag);
}

void UILoginPlug::initConf()
{
    if(_mainPanel)
        _mainPanel->initConf();
}

void UILoginPlug::init()
{
    _mainPanel = new LoginPanel ;
}

void UILoginPlug::saveHeadPath() {
    if(_mainPanel)
        _mainPanel->saveHeadPath();
}

void UILoginPlug::setLoginMessage(const QString &msg) {
    if(_mainPanel)
        _mainPanel->onAuthFailed(msg);
}
