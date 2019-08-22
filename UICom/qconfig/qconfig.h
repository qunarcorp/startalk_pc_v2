#include <utility>

//
// Created by cc on 19-1-13.
//

#ifndef QTALK_V2_QCONFIG_H
#define QTALK_V2_QCONFIG_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QList>
#include <QPair>
#include "../uicom_global.h"

#define CONFIG_TRUE "1"
#define CONFIG_FALSE "0"

namespace QTalk
{
    struct StConfig
    {
        StConfig* parent = nullptr;              // 父类
        QVector<StConfig*> children;             // 所有子类
        QString            tagName;            // 节点名称
        QString            tagVal;
        QList<QPair<QString, QString>> attributes;  // 所有属性

        StConfig()= default;
        explicit StConfig(QString name) : tagName(std::move(name)){}
        //
        bool hasChild(const QString& childTagName)
        {
            auto itFind = std::find_if(children.begin(), children.end(), [childTagName](const StConfig* config){
                return childTagName == config->tagName;
            });
            return itFind != children.end();
        }

        void addChild(StConfig* child)
        {
            child->parent = this;
            children.push_back(child);
        }

        QString attribute(const QString& key)
        {
            QString ret;
            auto itFind = std::find_if(attributes.begin(), attributes.end(), [key](const QPair<QString, QString>& attr){
                return (key == attr.first);
            });
            if(itFind != attributes.end())
                ret = itFind->second;
            return ret;
        }

        void setAttribute(const QString& key, const QString& val)
        {
            auto itFind = std::find_if(attributes.begin(), attributes.end(), [key](const QPair<QString, QString>& attr){
                return (key == attr.first);
            });
            if(itFind != attributes.end())
                itFind->second = val;
            else
                attributes.push_back(QPair<QString, QString>(key, val));
        }

        void setAttribute(const QString& key, bool val)
        {
            setAttribute(key, QString::number(val));
        }

        bool hasAttribute(const QString& key)
        {
            auto itFind = std::find_if(attributes.begin(), attributes.end(), [key](const QPair<QString, QString>& attr){
                return (key == attr.first);
            });
            return itFind != attributes.end();
        }
    };

    namespace qConfig {

        bool UICOMSHARED_EXPORT loadConfig(const QString& xmlPath, bool encrypt, StConfig* config);
        bool UICOMSHARED_EXPORT saveConfig(const QString& xmlPath, bool encrypt, const StConfig* config);
    }
}
#endif //QTALK_V2_QCONFIG_H
