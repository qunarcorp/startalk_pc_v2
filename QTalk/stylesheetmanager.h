#ifndef STYLESHEETMANAGER_H
#define STYLESHEETMANAGER_H

#include "../UICom/Config/configobject.h"

class StyleSheetManager : public QObject
{
    Q_OBJECT
public:
    explicit StyleSheetManager(QObject *parent = NULL);
    void setStyleSheets(int theme, const std::string& font);
    void setStylesForApp(int theme, const std::string& font);
    void setStyleSheetForPlugin(const QString& plgName, int theme);
};

#endif // STYLESHEETMANAGER_H
