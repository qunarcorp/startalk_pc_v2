#include "UIChatViewPlug.h"


UIChatViewPlug::UIChatViewPlug() :
    _mainPanel(nullptr)
{
}

UIChatViewPlug::~UIChatViewPlug() {

    if(_mainPanel)
    {
        delete _mainPanel;
        _mainPanel = nullptr;
    }
}

QWidget *UIChatViewPlug::widget()
{
    return _mainPanel;
}

/**
  * @功能描述
  * @参数
  * @date 2018.9.17
  */
void UIChatViewPlug::init()
{
    if (!_mainPanel)
    {
        _mainPanel = new ChatViewMainPanel;
    }
}

void UIChatViewPlug::setStyleSheet(const QString& qss)
{
    _mainPanel->setStyleSheet(qss);
    _mainPanel->setSearchStyle(qss);
}