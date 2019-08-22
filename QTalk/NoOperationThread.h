//
// Created by QITMAC000260 on 2019-06-17.
//

#ifndef QTALK_V2_NOOPERATIONTHREAD_H
#define QTALK_V2_NOOPERATIONTHREAD_H

#include <QThread>
#include <QMutexLocker>

/**
* @description: NoOperationThread
* @author: cc
* @create: 2019-06-17 10:26
**/
class NoOperationThread : public QThread {
    Q_OBJECT
public:
    NoOperationThread();
    ~NoOperationThread() override;

public:
    void setLeaveMinute();
    void resetUnOperatorTime();

public slots:
    void setAutoReplyFlag(bool);

Q_SIGNALS:
    void sgUserLeave(bool);

protected:
    void run() override ;

private:
    qint64 _unOperatorTime;
    qint64 _leaveTime;

    bool _isLeave = false;

private:
    QMutex _mutex;
    bool   _stop;
};


#endif //QTALK_V2_NOOPERATIONTHREAD_H
