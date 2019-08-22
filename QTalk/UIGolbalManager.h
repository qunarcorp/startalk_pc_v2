#ifndef UIGOLBALMANAGER_H
#define UIGOLBALMANAGER_H
#include "../UICom/Config/configobject.h"
#include "pluginmanager.h"
#include "stylesheetmanager.h"
#include "../QtUtil/lib/ini/ConfigLoader.h"
#include "../Platform/AppSetting.h"

class UIGolbalManager : public ConfigObject
{
    Q_OBJECT
public:
    ~UIGolbalManager() override;

public:
    static UIGolbalManager *GetUIGolbalManager();//获取单例对象指针的静态方法
    static QJsonDocument LoadJsonConfig(const QString &path);
    static void SaveJsonConfig(const QJsonDocument &doc, const QString &filepath, const QString &defaultpath);
    void SaveUIGolbalManager();
    void SavePluginManager();

    QObject *GetPluginInstanceQt(const QString &key)const;
    bool UnloadPluginQt(const QString &key);
    std::shared_ptr<QMap<QString, QObject *> > GetAllPluginInstanceQt() const;
    void Init();
    void InitPluginManager();
    void InitStyleManager();

    void setStyleSheetAll();
    void setStylesForApp();
    void setStyleSheetForPlugin(const QString& plgName);

    void initThemeConfig();

public slots:
    void saveSysConfig();

private:
    UIGolbalManager();

signals:
    void sgMousePressGlobalPos(const QPoint & pos);

private:
    static UIGolbalManager *_pInstance; //对象指针为单例指针类保证了只有一个对象

    QString _ConfigDataDir; // 配置文件路径
    static const QString DEFAULT_PluginManagerPath;
    static const QString DEFAULT_StyleManagerPath;
    static const QString DEFAULT_PluginPath;

    PluginManager * _pluginManager;
    StyleSheetManager * _pstyleSheetManager;

private:
    QTalk::ConfigLoader *_pSystemConfig;

    int _theme = 1;
    std::string _font;
};

#endif // UIGOLBALMANAGER_H
