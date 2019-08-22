//
// Created by cc on 18-12-14.
//

#include "SearchThread.h"
#include <QDateTime>
#include "SearchResultPanel.h"
#include "../QtUtil/Utils/Log.h"


SearchThread::SearchThread(SearchResultPanel *searchPanel)
        : _pSearchPanel(searchPanel), _isRun(false), _lastTime(0), _searchSts(false) {


    std::function<int(STLazyQueue<std::string> *)> func =

            [this](STLazyQueue<std::string> *queue) -> int {

                int runningCount = 0;
                if (queue != nullptr && !queue->empty()) {
                    const QString lastOne = queue->empty() ? "" : QString::fromStdString(queue->tail());
                    while (!queue->empty()) {
                        runningCount++;
                        queue->pop();
                    }
                    QMutexLocker locker(&_mutex);
                    emit sgSearchStart(lastOne);
                }

                return runningCount;
            };

    _searchQueue = new STLazyQueue<std::string>(40, func);
}

SearchThread::~SearchThread() {
    _isRun = false;
}

//void SearchThread::run() {
//    _isRun = true;
//
//    while (_isRun) {
//        if (_searchSts) {
//            long long curTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
//            if ((curTime - _lastTime > 150) && _pSearchPanel) {
//                QMutexLocker locker(&_mutex);
//                emit sgSearchStart(_lastReq);
//                _searchSts = false;
//            }
//        }
//        msleep(50);
//    }
//}

void SearchThread::addSearchReq(const QString &req) {
    QMutexLocker locker(&_mutex);
    _searchSts = true;
    _lastReq = req;
    _searchQueue->push(req.toStdString());
    _lastTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
}
