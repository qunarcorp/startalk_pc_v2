#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QMap>
#include <QPluginLoader>
#include <QLibrary>
#include <memory>
#include "../UICom/Config/configobject.h"

class PluginManager : public ConfigObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager() override;

public:
    void Init();
    void LoadPluginAllQt();
    bool LoadPluginQt(const QString &key, const QString &PluginAllPath);
    void UnloadPluginAllQt();
    bool UnloadPluginQt(const QString &key);
    QObject *GetPluginInstanceQt(const QString &key);
    std::shared_ptr<QMap<QString, QObject *>> GetAllPluginInstanceQt() const;
    void setPluginPath(const QString &path);

private:
    QMap<QString, QPluginLoader*> _pluginRegisterQt;
    QString _pluginPath;
};

#endif // PLUGINMANAGER_H
