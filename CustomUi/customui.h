#ifndef CUSTOMUI_H
#define CUSTOMUI_H
#include <QObject>
#include "customui_global.h"

class CUSTOMUISHARED_EXPORT CustomUi: public QObject
{
   Q_OBJECT
public:
    CustomUi();
    static QString GetStyle()
    {
        return m_style;
    }
public slots:
    static void SetStyle(QString arg)
    {
        if (m_style == arg)
            return;

        m_style = arg;
    }
private:
    static QString m_style;
};

#endif // CUSTOMUI_H
