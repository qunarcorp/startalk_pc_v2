//
// Created by cc on 18-12-14.
//

#ifndef QTALK_V2_SEARCHTHREAD_H
#define QTALK_V2_SEARCHTHREAD_H

#include <QThread>
#include <QMutexLocker>
#include "../include/STLazyQueue.h"

class SearchResultPanel;

class SearchThread : public QObject {
Q_OBJECT
public:
    SearchThread(SearchResultPanel *searchPanel);

    ~SearchThread();

public:
    void addSearchReq(const QString &req);

Q_SIGNALS:

    void sgSearchStart(const QString &);

protected:
    void run();

private:
    SearchResultPanel *_pSearchPanel;


    STLazyQueue<std::string> *_searchQueue;


    QString _lastReq;
    long long _lastTime;

    bool _isRun;
    bool _searchSts;
    QMutex _mutex;
};


#endif //QTALK_V2_SEARCHTHREAD_H
