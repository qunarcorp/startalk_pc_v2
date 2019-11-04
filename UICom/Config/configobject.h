#ifndef CONFIGOBJECT_H
#define CONFIGOBJECT_H

#include "../uicom_global.h"
#include <QJsonObject>
#include <QObject>
#include <QReadWriteLock>

class UICOMSHARED_EXPORT ConfigObject : public QObject
{
    Q_OBJECT
public:
    explicit ConfigObject(QObject *parent = 0);
    virtual ~ConfigObject();
    virtual const QJsonValue Get(const QString &key);
    virtual void Set(const QString &key, const QJsonValue &jsonvalue);
    virtual const QJsonObject ToJson();
    virtual void FromJson(const QJsonObject &souce);
protected:
    void SetRaw(const QString &key, const QJsonValue &jsonvalue);
    QJsonValue GetRaw(const QString &key);
Q_SIGNALS:
    void ValueChanged(const QString &key, const QJsonValue &value);

public slots:

private:
    QJsonObject _data;
    QReadWriteLock _lock;
};

#endif // CONFIGOBJECT_H
