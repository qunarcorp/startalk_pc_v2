//
// Created by QITMAC000260 on 2019-06-17.
//

#include "NoOperationThread.h"
#include <QDateTime>
#include "../Platform/AppSetting.h"

NoOperationThread::NoOperationThread()
{
    _stop = false;
    _unOperatorTime = QDateTime::currentMSecsSinceEpoch();
    _leaveTime = 5 * 60 * 1000;
    //
    setLeaveMinute();
    //
    start();
}

NoOperationThread::~NoOperationThread() = default;

void NoOperationThread::run()
{
    while (!_stop)
    {
        if(!_isLeave)
        {
            qint64 cur_time = QDateTime::currentMSecsSinceEpoch();
            QMutexLocker locker(&_mutex);
            qint64 un_t = cur_time - _unOperatorTime;
            //
            if(!_isLeave && un_t > _leaveTime)
            {
                _isLeave = true;
                emit sgUserLeave(true);
            }
        }

        msleep(1000);
    }
}

/**
 *
 * @param minute
 */
void NoOperationThread::setLeaveMinute()
{
    QMutexLocker locker(&_mutex);
    int leaveMinute = AppSetting::instance().getLeaveMinute();
    if(leaveMinute <= 1)
        leaveMinute = 1;
    _leaveTime = leaveMinute * 60 * 1000;
}

/**
 *
 */
void NoOperationThread::resetUnOperatorTime()
{
    QMutexLocker locker(&_mutex);
    _unOperatorTime = QDateTime::currentMSecsSinceEpoch();
    if(_isLeave)
    {
        _isLeave = false;
        emit sgUserLeave(false);
    }
}

/**
 *
 */
void NoOperationThread::setAutoReplyFlag(bool flag)
{
    QMutexLocker locker(&_mutex);
    _isLeave = flag;
}
