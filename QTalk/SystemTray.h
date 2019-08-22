//
// Created by cc on 18-12-10.
//

#ifndef QTALK_V2_SYSTEMTRAY_H
#define QTALK_V2_SYSTEMTRAY_H


#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

class MainWindow;
class SystemTray : public QObject
{
    Q_OBJECT
public:
    explicit SystemTray(MainWindow* mainWnd);
    ~SystemTray() override;

public slots:
    void onRecvMessage();

public:
    void onWndActived();

protected:
    void activeTray(QSystemTrayIcon::ActivationReason reason);
    void onSendLog();

private:
    void onTimer();
    void stopTimer();

private:
    MainWindow*      _pMainWindow;
    QSystemTrayIcon* _pSysTrayIcon;
    QTimer*          _timer;

private:
    unsigned short  _timerCount;
};


#endif //QTALK_V2_SYSTEMTRAY_H
