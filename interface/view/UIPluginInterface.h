#ifndef UIPLUGININTERFACE_H
#define UIPLUGININTERFACE_H

#include <QWidget>

class UIPluginInterface
{
public:

    virtual QWidget * widget() = 0;

    virtual void init() = 0;

    virtual void setStyleSheet(const QString& sheet)
    {
        if (this->widget())
        {
            this->widget()->setStyleSheet(sheet);
        }
    }
};
#endif // UIPLUGININTERFACE_H
