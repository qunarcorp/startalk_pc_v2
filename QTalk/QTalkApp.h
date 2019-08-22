#pragma once

#include <QApplication>
#include <QMutexLocker>
#include "MainWindow.h"

class UIGolbalManager;

class LogicManager;

class Platform;

class QTalkApp : public QApplication {
Q_OBJECT

public:
    QTalkApp(int argc, char *argv[]);

    ~QTalkApp() override;

protected:
    void initLogSys();

    void initDump();

	void dealDumpFile();

    bool notify(QObject *receiver, QEvent *e) Q_DECL_OVERRIDE;

    void initTTF();

private:
    UIGolbalManager *_pUiManager;
    LogicManager *_pLogicManager;

public:
    MainWindow *_pMainWnd;

};
