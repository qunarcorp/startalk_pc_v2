#include "UINavigationPlug.h"


UINavigationPlug::UINavigationPlug() :
    _mainPanel(nullptr)
{
}

QWidget *UINavigationPlug::widget()
{
    return _mainPanel;
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void UINavigationPlug::init()
{
    if (!_mainPanel)
    {
        _mainPanel = new NavigationMianPanel;
    }
}

void UINavigationPlug::updateReadCount()
{
    if (nullptr != _mainPanel)
    {
        _mainPanel->updateTatalReadCount();
    }
}
