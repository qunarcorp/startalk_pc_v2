#include "pluginmanager.h"
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QApplication>
#include <iostream>
#include "../QtUtil/Utils/Log.h"

PluginManager::PluginManager(QObject *parent) :
        ConfigObject(parent),
        _pluginPath("") {
    Init();
}

PluginManager::~PluginManager() {

}


/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.11
  */
void PluginManager::Init() {

}

/**
  * @函数名
  * @功能描述 加载所有qt插件
  * @参数
  * @date 2018.9.11
  */
void PluginManager::LoadPluginAllQt() {
    //
    QJsonObject obj = Get("PluginsQt").toObject();
            foreach (QString plugkey, obj.keys()) {
#ifdef _MACOS
#ifdef QT_NO_DEBUG
            QString plugin = _pluginPath + "lib" + obj.value(plugkey).toString() + ".dylib";
#else
            QString plugin = _pluginPath + "lib" + obj.value(plugkey).toString() + "d.dylib";
#endif
#else
#ifdef _WINDOWS
#ifdef QT_NO_DEBUG
            QString plugin = _pluginPath + obj.value(plugkey).toString() + ".dll";
#else
            QString plugin = _pluginPath + obj.value(plugkey).toString() + "d.dll";
#endif
#else
#ifdef QT_NO_DEBUG
            QString plugin = _pluginPath + "lib" + obj.value(plugkey).toString() + ".so";
#else
            QString plugin = _pluginPath + "lib" + obj.value(plugkey).toString() + "d.so";
#endif
#endif
#endif

            QFileInfo file(plugin);
            qDebug() << "plugin name:  " << plugin;
            QString filepath = file.absoluteFilePath();
            if (file.exists()) {
                std::cout << "plugin has been found:  " << filepath.toStdString() << std::endl;
                QPluginLoader *loader = new QPluginLoader(plugin);

                _pluginRegisterQt.insert(plugkey, loader);

            } else {
                std::cout << "plugin file is not exists... path: " << filepath.toStdString() << std::endl;
            }
        }
}

/**
  * @函数名
  * @功能描述 加载qt插件
  * @参数
  * @date 2018.9.11
  */
bool PluginManager::LoadPluginQt(const QString &key, const QString &PluginAllPath) {
    QFileInfo file(PluginAllPath);
    if (file.exists()) {
        QPluginLoader *loader = new QPluginLoader(PluginAllPath);
        if (loader->instance()) {
            _pluginRegisterQt.insert(key, loader);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/**
  * @函数名
  * @功能描述 卸载说有插件
  * @参数
  * @date 2018.9.11
  */
void PluginManager::UnloadPluginAllQt() {
            foreach (QPluginLoader *loader, _pluginRegisterQt.values()) {
            loader->unload();
            delete loader;
        }
    _pluginRegisterQt.clear();
}

/**
  * @函数名
  * @功能描述 根据关键字卸载插件
  * @参数
  * @date 2018.9.11
  */
bool PluginManager::UnloadPluginQt(const QString &key) {
    QPluginLoader *loader = _pluginRegisterQt.value(key);
    if (nullptr != loader) {
        bool result = loader->unload();
        _pluginRegisterQt.remove(key);
        delete loader;
        return result;
    }
    return false;
}

/**
  * @函数名
  * @功能描述 获取插件实例
  * @参数
  * @date 2018.9.11
  */
QObject *PluginManager::GetPluginInstanceQt(const QString &key) {
    // 如果插件没有加载，则加载插件
    if (_pluginRegisterQt.find(key) == _pluginRegisterQt.end()) {
        QJsonObject obj = Get("PluginsQt").toObject();
#ifdef _MACOS
#ifdef QT_NO_DEBUG
        QString plugin = _pluginPath + "lib" + obj.value(key).toString() + ".dylib";
#else
        QString plugin = _pluginPath + "lib" + obj.value(key).toString() + "d.dylib";
#endif
#else
#ifdef _WINDOWS
#ifdef QT_NO_DEBUG
        QString plugin = _pluginPath + obj.value(key).toString() + ".dll";
#else
        QString plugin = _pluginPath + obj.value(key).toString() + "d.dll";
#endif
#else
#ifdef QT_NO_DEBUG
        QString plugin = _pluginPath + "lib" + obj.value(key).toString() + ".so";
#else
        QString plugin = _pluginPath + "lib" + obj.value(key).toString() + "d.so";
#endif
#endif
#endif
        if (!LoadPluginQt(key, plugin)) {
            return nullptr;
        }
    }

    if (_pluginRegisterQt.value(key)) {
        QObject *plugin = _pluginRegisterQt.value(key)->instance();
        if (plugin) {
            return plugin;
        } else {
            std::string strErr = _pluginRegisterQt.value(key)->errorString().toStdString();
            std::cout << "error load:  " << strErr << std::endl;
            return NULL;
        }
    } else {
        return NULL;
    }
}

/**
  * @函数名
  * @功能描述
  * @参数
  * @date 2018.9.27
  */
std::shared_ptr<QMap<QString, QObject *> > PluginManager::GetAllPluginInstanceQt() const {
    std::shared_ptr<QMap<QString, QObject *> > plugins(new QMap<QString, QObject *>);
            foreach (QString pluginName, _pluginRegisterQt.keys()) {
            QPluginLoader *pluginloader = _pluginRegisterQt.value(pluginName);
            if (pluginloader->instance()) {
                plugins->insert(pluginName, pluginloader->instance());
            }
        }
    return plugins;
}

/**
  * @函数名
  * @功能描述 设置插件相对应用程序路径
  * @参数
  * @date 2018.9.27
  */
void PluginManager::setPluginPath(const QString &path) {
    _pluginPath = path;
}
