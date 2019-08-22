//
// Created by QITMAC000260 on 2019-07-12.
//

#ifndef QTALK_V2_GIFMANAGER_H
#define QTALK_V2_GIFMANAGER_H

#include <QMap>
#include <QObject>
#include <QMovie>
#include <QString>
#include "../include/Spinlock.h"

/**
* @description: GIFManager
* @author: cc
* @create: 2019-07-12 16:38
**/
class GIFManager{
public:
    GIFManager();

public:
    QMovie* getMovie(const QString& filePath);
    void removeMovie(QMovie *mov);

private:
    QMap<QString, QMovie*>      _mapMovie;
    QMap<QMovie*, int>          _movieCount;
    QTalk::util::spin_mutex     sm;
};


#endif //QTALK_V2_GIFMANAGER_H
