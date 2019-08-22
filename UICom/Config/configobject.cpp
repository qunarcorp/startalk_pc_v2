#include "configobject.h"

ConfigObject::ConfigObject(QObject *parent) : QObject(parent)
{
    _data = QJsonObject();
}

ConfigObject::~ConfigObject()
{

}

const QJsonValue ConfigObject::Get(const QString &key)
{
    return GetRaw(key);
}


void ConfigObject::Set(const QString &key, const QJsonValue &jsonvalue)
{
    SetRaw(key, jsonvalue);
}


const QJsonObject ConfigObject::ToJson()
{
    QReadLocker readLock(&_lock);
    return _data;
}

void ConfigObject::SetRaw(const QString &key, const QJsonValue &jsonvalue)
{
    QWriteLocker writeLock(&_lock);
    QJsonValue preValue = _data.value(key);
    _data.insert(key, jsonvalue);
    if (preValue != jsonvalue)
    {
        emit ValueChanged(key, jsonvalue);
    }
}

QJsonValue ConfigObject::GetRaw(const QString &key)
{
    QReadLocker readLock(&_lock);
    QJsonValue value = _data.value(key);
    return value;
}

void ConfigObject::FromJson(const QJsonObject &souce)
{
    _data = souce;
}

