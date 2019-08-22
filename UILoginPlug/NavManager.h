//
// Created by QITMAC000260 on 2018-12-25.
//

#ifndef QTALK_V2_NAVMANAGER_H
#define QTALK_V2_NAVMANAGER_H

#include "../CustomUi/UShadowWnd.h"
#include <QPushButton>
#include <QMap>
#include "../QtUtil/lib/ini/ConfigLoader.h"
#include "../UICom/qconfig/qconfig.h"
#include "NavView.h"

class LoginPanel;
class NavManager : public UShadowDialog
{
	Q_OBJECT
public:
    explicit NavManager(LoginPanel *loginPanel);
    ~NavManager() override;

public:
    QString getDefaultNavUrl();
    QString getNavName();
    QString getDefaultDomain();

protected:
    void initConfig();
    void initUi();
    void saveConfig();
    void onSaveConf();
    void onAddNav(const QString &name, const QString &navAddr, const bool &isDebug);

private:
    LoginPanel*          _pLoginPanel;
    NavView*             _pNavView{};

private://data
    QMap<QString, StNav> _mapNav;
    QString              _defaultKey;

private:
    QPushButton* _pCloseBtn{}; // 关闭按钮

};


#endif //QTALK_V2_NAVMANAGER_H
