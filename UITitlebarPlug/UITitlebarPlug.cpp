#include "UITitlebarPlug.h"


UITitlebarPlug::UITitlebarPlug() :
    _mainPanel(nullptr)
{
}

QWidget *UITitlebarPlug::widget()
{
    return _mainPanel;
}

void UITitlebarPlug::setCtrlWdt(QWidget *wdt)
{
    if (_mainPanel)
    {
        _mainPanel->setCtrlWdt(wdt);
    }
}

void UITitlebarPlug::init()
{
    if (!_mainPanel)
    {
        _mainPanel = new MainPanel;
    }
}
